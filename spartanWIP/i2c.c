#include <xmk.h>
#include <os_config.h>
#include <sys/process.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "xparameters.h"
#include "xgpio.h"
#include "i2c.h"
#include "VTio.h"

#define setLED(n)  XGpio_mWriteReg(XPAR_LEDS_8BIT_BASEADDR,1,n);

void i2c_status_handler(void *dptr,XStatus s_event)
{
	I2C_Comm *cptr = (I2C_Comm *) dptr;
	void	*msg_ptr;
	XStatus	status;
	int i;
	const	int	MAXLEN = 10;
	int	msg_buffer[MAXLEN];
	char str[20];

	if (s_event == XII_MASTER_WRITE_EVENT) {
		status = XIic_SlaveRecv(&(cptr->instance),cptr->iic_recv_msg,
				sizeof(Xuint8)*IIC_MSGLEN);
		if (status != XST_SUCCESS) {
			VTprintLCD("i2c error       ",2);
		}
	} else if (s_event == XII_BUS_NOT_BUSY_EVENT) {
		//sprintf(str,"Status: %X",status);
		//VTprintLCD(str,2);
	}
	// send a message to the i2c thread
	msg_buffer[0] = MSGTYPE_I2C_STATUS;
	msg_buffer[1] = s_event;
	msgsnd(cptr->internal_status_queue_id,msg_buffer,sizeof(int)*2,IPC_NOWAIT);
}

void i2c_send_handler(void *dptr,int bytecount)
{
	I2C_Comm *cptr = (I2C_Comm *) dptr;
	void	*msg_ptr;
	const	int	MAXLEN = 10;
	int	msg_buffer[MAXLEN];
	int	length;


	msg_buffer[0] = MSGTYPE_I2C_SEND_COMPLETE;
	msg_buffer[1] = bytecount;
	msgsnd(cptr->internal_status_queue_id,msg_buffer,sizeof(int)*2,IPC_NOWAIT);
}

void i2c_recv_handler(void *dptr,int bytecount)
{
	I2C_Comm *cptr = (I2C_Comm *) dptr;
	void	*msg_ptr;
	const	int	MAXLEN = 10;
	int	msg_buffer[MAXLEN];


	// send a message to a receiver
	msg_ptr = (void *) cptr->iic_recv_msg;
	msgsnd(cptr->outgoing_msg_queue_id,msg_ptr,IIC_MSGLEN-bytecount,IPC_NOWAIT);


	msg_buffer[0] = MSGTYPE_I2C_RECV_COMPLETE;
	msg_buffer[1] = IIC_MSGLEN-bytecount;
	msgsnd(cptr->internal_status_queue_id,msg_buffer,sizeof(int)*2,IPC_NOWAIT);
}

XStatus init_i2c(I2C_Comm *cptr,int device_id,int int_id,int i2c_addr,
	  	int master)
{
	XStatus status;
	int	actual_address;
	char str[20];

	// initialize the driver
	status = XIic_Initialize(&(cptr->instance),device_id);
	if (status != XST_SUCCESS) {
		xil_printf("INIT IIC Failed\n");
		sprintf(str,"INIT IIC Failed!\n");
		VTprintLCD(str,2);
		return(status);
	}

	// ensure that Slave code is included
	XIic_SlaveInclude();

	// Set the IIC address
	if(master) {
		status = XIic_SetAddress(&(cptr->instance),XII_ADDR_TO_SEND_TYPE,
			i2c_addr);
	} else {
		status = XIic_SetAddress(&(cptr->instance),XII_ADDR_TO_RESPOND_TYPE,
			i2c_addr);
	}
	if (status != XST_SUCCESS) {
		xil_printf("Set Address Error %d\n",status);
		sprintf(str,"Set Address Error %d\n",status);
		VTprintLCD(str,2);
		return(status);
	}

	if(master) {
		actual_address = XIic_GetAddress(&(cptr->instance),
			XII_ADDR_TO_SEND_TYPE);
	} else {
		actual_address = XIic_GetAddress(&(cptr->instance),
			XII_ADDR_TO_RESPOND_TYPE);
	}

	// connect the interrupt handler
	cptr->int_id = int_id;
	status = register_int_handler(int_id,XIic_InterruptHandler,
		&(cptr->instance));
	if (status != XST_SUCCESS) {
		xil_printf("INT_REG error\n");
		sprintf(str,"INT REG Failed!\n");
		VTprintLCD(str,2);
		return(status);
	}

	// connect the recv handler
	XIic_SetRecvHandler(&(cptr->instance),cptr,i2c_recv_handler);
	XIic_SetSendHandler(&(cptr->instance),cptr,i2c_send_handler);
	XIic_SetStatusHandler(&(cptr->instance),cptr,i2c_status_handler);

	enable_interrupt(int_id);
	XIic_Start(&(cptr->instance));
	if (status != XST_SUCCESS) {
		xil_printf("IIC Start Error\n");
		sprintf(str,"IIC Start Failed!\n");
		VTprintLCD(str,2);
		return(status);
	}

	//xil_printf("Options are %x\n",
			//XIic_GetOptions(&(cptr->instance)));



	return(status);
}

// This thread receives messages from other threads/handlers.
// The first byte in the message is a "code" that indicates the type of message.
void *i2c_thread(void *dptr)
{
	I2C_Comm *cptr = (I2C_Comm *) dptr;
	const	int	MAXLEN = 10;
	unsigned char	msg_buffer[MAXLEN];
	int	status_buffer[MAXLEN];
	int	length;
	XStatus status;
	int internal_message;
	char str[20];

	while (1) {
	
		length = msgrcv(cptr->incoming_msg_queue_id,(void *) msg_buffer,
			sizeof(unsigned char)*MAXLEN,0,0);
		// start a message send
		// Note: This could return something other than success (e.g., 
		// it could indicate that the i2c bus is busy).  This routine 
		// is not set up to handle that.
		switch(msg_buffer[0]) {
			case I2C_SEND_MSG:
			{
				internal_message = MSGTYPE_I2C_SEND_COMPLETE;
				status = XIic_SetAddress(&(cptr->instance),XII_ADDR_TO_SEND_TYPE,
					msg_buffer[1]);
				status = XIic_MasterSend(&(cptr->instance),(void *) msg_buffer,
					length);
				break;
			}
			case I2C_RECV_MSG:
			{
				internal_message = MSGTYPE_I2C_RECV_COMPLETE;
				status = XIic_SetAddress(&(cptr->instance),XII_ADDR_TO_SEND_TYPE,
					msg_buffer[1]);
				status = XIic_MasterRecv(&(cptr->instance),cptr->iic_recv_msg,
					msg_buffer[2]);
				break;
			}
			default:
			{
			}
		}
		
		// wait on confirmation that the message has been sent
		// (or determine that an error has occurred)
		while (status == XST_SUCCESS) {
			length = msgrcv(cptr->internal_status_queue_id,
				(void *) status_buffer,
				sizeof(int)*MAXLEN,0,0);
			// either we get the message type we want and succeed
			// or we keep going [we just dump the other status
			// messages w/o processing them]
			//setLED(status_buffer[0]);
			if (status_buffer[0] == MSGTYPE_I2C_SEND_COMPLETE &&
					internal_message == MSGTYPE_I2C_SEND_COMPLETE) {
				// make sure the message was fully sent
				if (status_buffer[1] == 0) {
					break; // leave the loop
				} else {
					status = XST_FAILURE;
				}
			}
			if (status_buffer[0] == MSGTYPE_I2C_RECV_COMPLETE &&
					internal_message == MSGTYPE_I2C_RECV_COMPLETE) {
				// make sure the message was fully sent
				if (status_buffer[1] == 12) {
					break; // leave the loop
				} else {
					status = XST_FAILURE;
				}
			}
			if (status_buffer[0] == MSGTYPE_I2C_STATUS) {
				status = status_buffer[1];
			}
		}
		// send a status message to the outgoing status queue
		msgsnd(cptr->out_status_queue_id,(void *) &status,
			sizeof(XStatus),0);
	}
}
