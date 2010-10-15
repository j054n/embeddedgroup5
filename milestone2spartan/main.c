#include <xmk.h>
#include <os_config.h>
#include <sys/process.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "xparameters.h"
#include "xgpio.h"
#include "buttons.h"
#include "timer.h"
#include "i2c.h"
#include "VTio.h"


#define BUTTON_Q_1 0x51
#define LCD_Q_1 0x52
#define I2C_STATUS_Q 0x53
#define I2C_ISTATUS_Q 0x54
#define I2C_RECV_Q 0x55
#define I2C_INCOMING_Q 0x56
#define CONTROLLER_Q_1 0x57

// Define some message types
#define BUTTON_MSG 0x01
#define TIMER_MSG 0x02

// entry point into the program
int main(void)
{
	// starting the kernel
	xilkernel_main(); // This routine starts the kernel and leaves main()
	return 0;
}

///////////////////////////////////////////
// the button thread
//  - It waits for a message from the button interrupt handler
//  - When it gets a message, it changes the current setting for either
//    the led mask or the "duration" of the LED blinking cycle, depending
//    on which button is pressed. 
//  - It then sends the new values to the LED thread via a message
///////////////////////////////////////////
typedef struct __Buttons_Thread_Comm {
	int	recv_msg_q_id;
	int	send_msg_q_id;
	Buttons_Comm	*button_device;
} Buttons_Thread_Comm;
void *button_thread(void *dptr)
{
	const	int	MAXLEN = 10;
	Buttons_Thread_Comm *cptr = (Buttons_Thread_Comm *) dptr;
	unsigned	int	msg_buffer[MAXLEN];
	int	length, i2c_length;
	unsigned char	i2c_msg_buffer[MAXLEN];
	unsigned	int	button_val;
	unsigned char button_char;
	int	done;
	XStatus	status;

	for (;;) {
		length = msgrcv(cptr->recv_msg_q_id,
			(void *) msg_buffer,MAXLEN*sizeof(unsigned int),
			0,0);

		// Need to mask out and shift to retrieve the button values
		// that we want.
		button_val = msg_buffer[1] & 0x3E0;
		button_char = button_val >> 6;
		

		// only send a message is a button is actually pressed
		if (button_char != 0) {
			// Assemble the i2c message
			// message type
			i2c_msg_buffer[0] = I2C_SEND_MSG;
			i2c_msg_buffer[1] = SENSOR_BRD_ADDR;
			i2c_msg_buffer[2] = button_char;
			i2c_length = 3*sizeof(unsigned char);
			msgsnd(cptr->send_msg_q_id,i2c_msg_buffer,i2c_length,0);
		}
	}
}

///////////////////////////////////////////
// the lcd thread
//  - It waits for a message and then prints that message
///////////////////////////////////////////
typedef struct __LCD_Thread_Comm {
	int	msg_recv_queue_id;
	int msg_send_queue_id;
} LCD_Thread_Comm;
void *lcd_thread(void *dptr)
{
	LCD_Thread_Comm *cptr = (LCD_Thread_Comm *) dptr;
	int	length;
	int	count;
	unsigned char iic_msg[MAX_IIC_MSGLEN];
	char str[20];
	unsigned char iic_send_msg[MAX_IIC_MSGLEN];

	count=0;

	for (;;) {
		length = msgrcv(cptr->msg_recv_queue_id,(void *) iic_msg,
			sizeof(Xuint8)*MAX_IIC_MSGLEN,0,0);
/*
		switch (iic_msg[0]) {
			case I2C_SEND_MSG: {
				
				break;
			}
			default: {
				//VTprintLCD("Unknown MsgType",1);
				break;
			}
		}*/
		unsigned char val1 = iic_msg[0];
		unsigned int val1i = val1;
		unsigned char val2 = iic_msg[1];
		unsigned int val2i = val2;
		sprintf(str,"Val:  %2X%02X",val1i, val2i);
				VTprintLCD(str,1);
				val1 = val1 << 1;
				val1 = val1 | (val2 >> 7);
				val1i = val2i + (val1i << 8);
				
				if (val1i > 0x2D0)
					val1 = 0;
				else if (val1i > 0x200)
					val1 = 1;
				else if (val1i > 0x100)
					val1 = 3;
				else
					val1 = 7;
				
				
				iic_send_msg[0] = I2C_SEND_MSG;
				iic_send_msg[1] = INSTEON_ADDR;
				iic_send_msg[2] = val1;
				//iic_send_msg[3] = iic_msg[2];
				msgsnd(cptr->msg_send_queue_id,iic_send_msg,3*sizeof(unsigned char),0);
	}
}



typedef struct __Controller_Thread_Comm {
	int	recv_msg_q_id;
	int	send_msg_q_id;
} Controller_Thread_Comm;
void *controller_thread(void *dptr)
{
	Controller_Thread_Comm *cptr = (Controller_Thread_Comm *) dptr;
	const	int	MAXLEN = 10;
	unsigned	int	msg_buffer[MAXLEN];
	int	length, i2c_length;
	unsigned char	i2c_msg_buffer[MAXLEN];
	int	count;
	unsigned int iic_msg[MAX_IIC_MSGLEN];

	i2c_msg_buffer[0] = I2C_SEND_MSG;
	i2c_msg_buffer[1] = SENSOR_BRD_ADDR;
	i2c_msg_buffer[2] = 0x14;
	i2c_length = 3*sizeof(unsigned char);
	//msgsnd(cptr->send_msg_q_id,i2c_msg_buffer,i2c_length,0);
	count = 0;
	for ( ;; ) {
	   length = msgrcv(cptr->recv_msg_q_id,
			(void *) msg_buffer,MAXLEN*sizeof(unsigned int),
			0,0);
		if(length > 0) {
			
			switch(msg_buffer[0]) {
				case TIMER_MSG: {

					i2c_msg_buffer[0] = I2C_RECV_MSG;
					i2c_msg_buffer[1] = SENSOR_BRD_ADDR;
					i2c_msg_buffer[2] = 0x02;						//size of message expected back
					i2c_length = 3*sizeof(unsigned char);

					msgsnd(cptr->send_msg_q_id,i2c_msg_buffer,i2c_length,0);
					break;
				}
				default: {
					break;
				}
			}
			
		}
	}
	
	}

////////////////////////////////////////
// This thread is "statically" started by the kernel on initialization
//   -- It takes no arguments (unlike other threads)
//   -- In this program, it is responsible for initializing devices and threads
////////////////////////////////////////
void *user_main()
{
	// Device-related structures
	Buttons_Comm buttons_comm;
	Tcomm_struct	debug_timer_data;
	Tcomm_struct	timer_data;
	I2C_Comm	i2c_comm;

	// Thread-related structures
	LCD_Thread_Comm lcd_thread_comm;
	Buttons_Thread_Comm buttons_thread_comm;
	Controller_Thread_Comm controller_thread_comm;
	
	pthread_t my_threadt, button_thread_t, lcd_thread_t,
		i2c_thread_t, controller_thread_t;
	pthread_attr_t my_thread_attr_t, button_thread_attr_t,
		lcd_thread_attr_t,
		i2c_thread_attr_t,
		controller_thread_attr_t;
	struct sched_param my_thread_s_params, button_s_params,
		lcd_s_params, i2c_s_params, controller_s_params;
	int	policy;

	// Return code variable for calls to Xilinx routines
	XStatus status;
	int	length;
	const	int	MAXLEN = 10;
	int	msg_buffer[MAXLEN];

	unsigned int i_am_master;
	char str[20];
	int msgs_recieved = 0;
	

	

	////////////////////////////////////////
	// setup a msg queue for the LCD thread to receive messages on
	////////////////////////////////////////
	lcd_thread_comm.msg_recv_queue_id = msgget(LCD_Q_1,IPC_CREAT);

	controller_thread_comm.recv_msg_q_id = msgget(CONTROLLER_Q_1, IPC_CREAT);
	
	////////////////////////////////////////
	// initialize the button device, including its message queue
	// to which it will send messages.
	////////////////////////////////////////
	status = init_buttons(&buttons_comm,msgget(BUTTON_Q_1,
		IPC_CREAT),BUTTON_MSG);
	if (status != XST_SUCCESS) {
		return;
	}

	i_am_master = 0x01;


	//Setup timer 3 to send messages to controller thread for period polling of sensors
	status = Start_Timer(&timer_data,XPAR_XPS_TIMER_3_DEVICE_ID,
		XPAR_XPS_INTC_0_XPS_TIMER_3_INTERRUPT_INTR,0,50000000,500,
		controller_thread_comm.recv_msg_q_id,TIMER_MSG);
	
	////////////////////////////////////////
	// Set up i2c message queues
	////////////////////////////////////////
	i2c_comm.incoming_msg_queue_id = 
		msgget(I2C_INCOMING_Q,IPC_CREAT);
	i2c_comm.outgoing_msg_queue_id = 
		lcd_thread_comm.msg_recv_queue_id;
	i2c_comm.out_status_queue_id = 
		msgget(I2C_STATUS_Q,IPC_CREAT);
	i2c_comm.internal_status_queue_id = 
		msgget(I2C_ISTATUS_Q,IPC_CREAT);

	////////////////////////////////////////
	// Initialize the master or slave i2c 
	// device on J2
	////////////////////////////////////////
	status = init_i2c(&i2c_comm,XPAR_XPS_IIC_J2_DEVICE_ID,
		XPAR_XPS_INTC_0_XPS_IIC_J2_IIC2INTC_IRPT_INTR,
		0x45,i_am_master);
	if (status != XST_SUCCESS) {
		xil_printf("Init I2C Failed\n");
		return;
	}
	
	lcd_thread_comm.msg_send_queue_id = i2c_comm.incoming_msg_queue_id;

	////////////////////////////////////////
	// Give the Button thread structure a pointer to the Button
	// device structure and share the msg_queue id between them
	////////////////////////////////////////
	buttons_thread_comm.recv_msg_q_id = buttons_comm.send_msg_q_id;
	buttons_thread_comm.send_msg_q_id = 
		i2c_comm.incoming_msg_queue_id;
	buttons_thread_comm.button_device = &buttons_comm;


   controller_thread_comm.send_msg_q_id = i2c_comm.incoming_msg_queue_id;

	////////////////////////////////////////
	// Code for examining the priority of this thread
	// -- This isn't "necessary", but it does tell us the priority of
	//    this thread
	////////////////////////////////////////
	// get schedule information on the static thread (this thread)
	my_threadt = pthread_self();
	pthread_getschedparam(my_threadt,&policy,&my_thread_s_params);
	////////////////////////////////////////
	// End of Code for examining and setting the priority 
	// of this thread
	////////////////////////////////////////

	////////////////////////////////////////
	// Code for setting up the LCD Thread
	////////////////////////////////////////
	// set up an attribute structure for this thread
	status = pthread_attr_init(&lcd_thread_attr_t);
	// get the current scheduling parameters for this thread
	pthread_attr_getschedparam(&lcd_thread_attr_t,&lcd_s_params);
	lcd_s_params.sched_priority = 0x5;
	pthread_attr_setschedparam(&lcd_thread_attr_t,&lcd_s_params);
	// create the thread -- it is runnable after this call
	status = pthread_create(&lcd_thread_t,&lcd_thread_attr_t,
		lcd_thread,&lcd_thread_comm);
	if (status != XST_SUCCESS) {
		return;
	}
	////////////////////////////////////////
	// End of LCD Thread setup
	////////////////////////////////////////
	
		////////////////////////////////////////
	// Code for setting up the Controller Thread
	////////////////////////////////////////
	// set up an attribute structure for this thread
	status = pthread_attr_init(&controller_thread_attr_t);
	// get the current scheduling parameters for this thread
	pthread_attr_getschedparam(&controller_thread_attr_t,&controller_s_params);
		controller_s_params.sched_priority = 0x5;
	pthread_attr_setschedparam(&controller_thread_attr_t,&controller_s_params);
	// create the thread -- it is runnable after this call
	status = pthread_create(&controller_thread_t,&controller_thread_attr_t,
		controller_thread,&controller_thread_comm);
	if (status != XST_SUCCESS) {
		return;
	}
	////////////////////////////////////////
	// End of Controller Thread setup
	////////////////////////////////////////
	

	////////////////////////////////////////
	// Code for setting up the Button Thread
	// Only create the button thread if we are
	// the i2c master.
	////////////////////////////////////////
	// set up an attribute structure for this thread
	status = pthread_attr_init(&button_thread_attr_t);
	// get the current scheduling parameters for this thread
	pthread_attr_getschedparam(&button_thread_attr_t,&button_s_params);
	button_s_params.sched_priority = 0x6;
	pthread_attr_setschedparam(&button_thread_attr_t,&button_s_params);
	// create the thread -- it is runnable after this call
	////////////////////////////////////////
	// End of Button Thread setup
	////////////////////////////////////////

	/////////////////////////////	
	status = pthread_create(&button_thread_t,&button_thread_attr_t,
		button_thread,&buttons_thread_comm);
	if (status != XST_SUCCESS) {
		return;
	}
///////////
	// Code for setting up the I2C Thread
	// Only create this thread for the master.
	////////////////////////////////////////
	// set up an attribute structure for this thread
	status = pthread_attr_init(&i2c_thread_attr_t);
	// get the current scheduling parameters for this thread
	pthread_attr_getschedparam(&i2c_thread_attr_t,
		&i2c_s_params);
	i2c_s_params.sched_priority = 0x6;
	pthread_attr_setschedparam(&i2c_thread_attr_t,
		&i2c_s_params);
	// create the thread -- it is runnable after this call

	status = pthread_create(&i2c_thread_t,&i2c_thread_attr_t,
		i2c_thread,&i2c_comm);
	if (status != XST_SUCCESS) {
		return;
	}
	////////////////////////////////////////
	// End of I2C Thread setup
	////////////////////////////////////////

	////////////////////////////////////////
	// On LCD indicate if master or slave
	////////////////////////////////////////
	VTprintLCD("I am the Master ",1);
	VTprintLCD("                ",2);

	////////////////////////////////////////
	// Wait on status messages from the i2c
	// For the master, we note when msgs are
	// send, on the slave, we recieve the msgs
	// sent by the internal status queue
	////////////////////////////////////////
	for (;;) {
		length = msgrcv(i2c_comm.out_status_queue_id,
			(void *) msg_buffer,MAXLEN*sizeof(unsigned int),0,0);
		sprintf(str,"I2C status %d",msg_buffer[0]);
		//VTprintLCD(str,2);
	}
}
