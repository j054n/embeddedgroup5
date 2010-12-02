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

#define USER_DATA_MSGLEN 1

#define BUTTON_Q_1 0x51
#define LCD_Q_1 0x52
#define I2C_STATUS_Q 0x53
#define I2C_ISTATUS_Q 0x54
#define I2C_RECV_Q 0x55
#define I2C_INCOMING_Q 0x56
#define CONTROLLER_Q_1 0x57
#define DATA_Q_1 0x58

// Define some message types
#define BUTTON_MSG 0x01
#define TIMER_MSG 0x02

#define setLED(n)  XGpio_mWriteReg(XPAR_LEDS_8BIT_BASEADDR,1,n);
#define CONVERGE 2

// entry point into the program
int main(void)
{
	VTinit();
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
	unsigned int	user_msg_buffer[MAXLEN];
	unsigned	int	button_val;
	unsigned char button_char;
	int	done;
	unsigned int flag;
	unsigned int user;
	char str[20];
	XStatus	status;
	
	flag = 0;
	
	sprintf(str,"Select User");
	VTprintLCD(str,1);

	for (;;) {
		length = msgrcv(cptr->recv_msg_q_id,
			(void *) msg_buffer,MAXLEN*sizeof(unsigned int),
			0,0);

		// Need to mask out and shift to retrieve the button values
		// that we want.
		button_val = msg_buffer[1] & 0x3E0;
		button_char = button_val >> 6;
		

		// only send a message if a button is actually pressed
		if (button_char != 0) {
			// Assemble the i2c message
			// message type
			/*i2c_msg_buffer[0] = I2C_SEND_MSG;
			i2c_msg_buffer[1] = SENSOR_BRD_ADDR;
			i2c_msg_buffer[2] = button_char;
			i2c_length = 3*sizeof(unsigned char);
			msgsnd(cptr->send_msg_q_id,i2c_msg_buffer,i2c_length,0); //send message to I2C handler*/
			/*if(!flag) {
				if(button_char == 1) { user = 1; }//user 1 active
				else if(button_char == 2) { user = 2; }//user 2 active
				else if(button_char == 4) { user = 3; }//user 3 active
				else if(button_char == 8) { user = 4; }//user 4 active
				sprintf(str, "User%d:Activity?", user);
				VTprintLCD(str,1);
				flag++;
			}
			else {
				if(button_char == 1) { //activity 1 active
					sprintf(str, "User%d:Activity:1", user);
					user_msg_buffer[0] = 	user==1 ? 100 :
											user==2 ? 80 :
											user==3 ? 60 :
											user==4 ? 40 : 0;
				}
				else if(button_char == 2) { //activity 2 active
					sprintf(str, "User%d:Activity:2", user);
					user_msg_buffer[0] = 	user==1 ? 75 :
											user==2 ? 60 :
											user==3 ? 45 :
											user==4 ? 30 : 0;
				}
				else if(button_char == 4) { //activity 3 active
					sprintf(str, "User%d:Activity:3", user);
					user_msg_buffer[0] = 	user==1 ? 50 :
											user==2 ? 40 :
											user==3 ? 30 :
											user==4 ? 20 : 0;
				}
				else if(button_char == 8) { //return to user select
					flag--; 
					sprintf(str,"Select User");
					VTprintLCD(str,1);
				}
				if(button_char != 8) {
					VTprintLCD(str,1);
					msgsnd(cptr->send_msg_q_id,user_msg_buffer,sizeof(unsigned int),0);
				}
			}*/
		}
	}
}

///////////////////////////////////////////
// the lcd thread
//  - Waits for a message and then prints that message
///////////////////////////////////////////
typedef struct __LCD_Thread_Comm {
	int	msg_recv_queue_id;
	int msg_send_queue_id;
} LCD_Thread_Comm;
void *lcd_thread(void *dptr)
{
	LCD_Thread_Comm *cptr = (LCD_Thread_Comm *) dptr;
	int	length;
	unsigned char msgbuf[MAX_IIC_MSGLEN];
	unsigned char iic_send_msg[MAX_IIC_MSGLEN];

	for (;;) {
		length = msgrcv(cptr->msg_recv_queue_id,(void *) msgbuf,
			sizeof(Xuint8)*MAX_IIC_MSGLEN,0,0);
			
		VTprintLCD(msgbuf,1);
	}
}

///////////////////////////////////////////
// Data processing thread
//  - Waits for data from I2C, processes that data, 
//     and sends the appropriate on-value over I2C to insteon
///////////////////////////////////////////
typedef struct __Data_Thread_Comm {
	int	recv_msg_q_id;
	int	send_iic_q_id;
	int send_lcd_q_id;
} Data_Thread_Comm;
void *data_thread(void *dptr)
{
	Data_Thread_Comm *cptr = (Data_Thread_Comm *) dptr;
	int	length;
	unsigned char iic_msg[MAX_IIC_MSGLEN];
	unsigned char str[20];
	unsigned char iic_send_msg[MAX_IIC_MSGLEN];
	unsigned char user_val, current;
	int avg1, avg2, avg;
	int diff;
	unsigned int flags;
	unsigned int insteon_val;
	unsigned int target;
	unsigned int user[17] = {100,75,50,25,80,60,40,20,60,45,30,15,40,30,20,10,100}; //activities are hardcoded for now

	avg1=0;
	avg2=0;
	avg=0;
	current=16;
	insteon_val=0xFF;
	target=0x3FF;

	for (;;) {
		length = msgrcv(cptr->recv_msg_q_id,(void *) iic_msg,	//wait for a message from I2C
			sizeof(Xuint8)*MAX_IIC_MSGLEN,0,0);
		
		unsigned char val1 = iic_msg[0];
		unsigned int val1i = val1;
		unsigned char val2 = iic_msg[1];
		unsigned int val2i = val2;
		unsigned char val3 = iic_msg[2];
		unsigned int val3i = val3;
		unsigned char val4 = iic_msg[3];
		unsigned int val4i = val4;
		
		user_val = ((iic_msg[4]&0x3)<<2)|(iic_msg[5]&0x3);
		flags = ((iic_msg[4]&0xFC)<<6)|(iic_msg[5]&0xFC); //outside system status flags
		
		if(flags&0x40 == 0x40) user_val = 16; //user/activity not yet selected
		if(user_val != current) { //target value has changed
			target = (user[user_val] * 0x3FF)/100;
			current = user_val;
		}

		if((val1i&0xF0) != 0xF0 && (val3i&0xF0) != 0xF0) { //valid data arrived from the sensors
			val1i = (val2i + (val1i << 8)); //assemble 12-bit A/D values
			val3i = (val4i + (val3i << 8));
			
			diff = val1i - avg1;		//converge over time to the current sensor value
			avg1 += diff / CONVERGE;	//rate of convergence determined by CONVERGE
			diff = val3i - avg2;
			avg2 += diff / CONVERGE;
			avg = (avg1 + avg2) / 2;
			
			if(avg < target) { //adjust up
				if(avg < target-120) {insteon_val = insteon_val<225 ? insteon_val+30 : 0xFF;}
				else if(avg < target-80)  {insteon_val = insteon_val<235 ? insteon_val+20 : 0xFF;}
				else if(avg < target-40)  {insteon_val = insteon_val<245 ? insteon_val+10 : 0xFF;}
				else if(avg < target-20)  {insteon_val = insteon_val<250 ? insteon_val+ 5 : 0xFF;}
				else if(avg < target-8)   {insteon_val = insteon_val<253 ? insteon_val+ 2 : 0xFF;}
			}
			else if(avg > target) { //adjust down
				if(avg > target+120) {insteon_val = insteon_val>30 ? insteon_val-30 : 0;}
				else if(avg > target+80)  {insteon_val = insteon_val>20 ? insteon_val-20 : 0;}
				else if(avg > target+40)  {insteon_val = insteon_val>10 ? insteon_val-10 : 0;}
				else if(avg > target+20)  {insteon_val = insteon_val>5  ? insteon_val- 5 : 0;}
				else if(avg > target+8)   {insteon_val = insteon_val>2  ? insteon_val- 2 : 0;}
			}
			
			sprintf(str,"Val:  %03X/%03X",avg,target);
			msgsnd(cptr->send_lcd_q_id,str,20*sizeof(unsigned char),0); //send message off to be printed on LCD
			
			iic_send_msg[0] = I2C_SEND_MSG;
			iic_send_msg[1] = INSTEON_ADDR;
			iic_send_msg[2] = insteon_val;
			
			setLED(insteon_val); //set the on-value to the LEDs for debug purposes
		}

		msgsnd(cptr->send_iic_q_id,iic_send_msg,3*sizeof(unsigned char),0); //send on-value to Insteon PIC
		//iic_send_msg[1] = SENSOR_BRD_ADDR;
		//msgsnd(cptr->send_iic_q_id,iic_send_msg,3*sizeof(unsigned char),0);
	}
}

///////////////////////////////////////////
// Controller thread
// Sends requests for sensor data to the sensor
// board on each timer interrupt
///////////////////////////////////////////
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

	for ( ;; ) {
	   length = msgrcv(cptr->recv_msg_q_id,
			(void *) msg_buffer,MAXLEN*sizeof(unsigned int),
			0,0);
		if(length > 0) {	
			switch(msg_buffer[0]) {
				case TIMER_MSG: {
					i2c_msg_buffer[0] = I2C_RECV_MSG;
					i2c_msg_buffer[1] = SENSOR_BRD_ADDR;
					i2c_msg_buffer[2] = 6;	//size of message expected back
					i2c_length = 3*sizeof(unsigned char);

					msgsnd(cptr->send_msg_q_id,i2c_msg_buffer,i2c_length,0); //request data packet from PIC
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
	Data_Thread_Comm data_thread_comm;
	
	pthread_t my_threadt, button_thread_t, lcd_thread_t,
		i2c_thread_t, controller_thread_t, data_thread_t;
	pthread_attr_t my_thread_attr_t, button_thread_attr_t,
		lcd_thread_attr_t,
		i2c_thread_attr_t,
		controller_thread_attr_t, data_thread_attr_t;
	struct sched_param my_thread_s_params, button_s_params,
		lcd_s_params, i2c_s_params, controller_s_params, data_s_params;
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
	data_thread_comm.recv_msg_q_id = msgget(DATA_Q_1, IPC_CREAT);
	
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
		data_thread_comm.recv_msg_q_id;
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
		sprintf(str,"Init I2C Failed!\n");
		VTprintLCD(str,2);
		return;
	}
	
	lcd_thread_comm.msg_send_queue_id = i2c_comm.incoming_msg_queue_id;

	////////////////////////////////////////
	// Give the Button thread structure a pointer to the Button
	// device structure and share the msg_queue id between them
	////////////////////////////////////////
	buttons_thread_comm.recv_msg_q_id = buttons_comm.send_msg_q_id;
	buttons_thread_comm.send_msg_q_id = 
		lcd_thread_comm.msg_recv_queue_id;
	buttons_thread_comm.button_device = &buttons_comm;


    controller_thread_comm.send_msg_q_id = i2c_comm.incoming_msg_queue_id;
	data_thread_comm.send_iic_q_id = i2c_comm.incoming_msg_queue_id;
	data_thread_comm.send_lcd_q_id = lcd_thread_comm.msg_recv_queue_id;

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
	// Code for setting up the Data Thread
	////////////////////////////////////////
	// set up an attribute structure for this thread
	status = pthread_attr_init(&data_thread_attr_t);
	// get the current scheduling parameters for this thread
	pthread_attr_getschedparam(&data_thread_attr_t,&data_s_params);
		data_s_params.sched_priority = 0x5;
	pthread_attr_setschedparam(&data_thread_attr_t,&data_s_params);
	// create the thread -- it is runnable after this call
	status = pthread_create(&data_thread_t,&data_thread_attr_t,
		data_thread,&data_thread_comm);
	if (status != XST_SUCCESS) {
		return;
	}
	////////////////////////////////////////
	// End of Data Thread setup
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
	// Wait on status messages from the i2c
	// For the master, we note when msgs are
	// send, on the slave, we recieve the msgs
	// sent by the internal status queue
	////////////////////////////////////////
	for (;;) {
		length = msgrcv(i2c_comm.out_status_queue_id,
			(void *) msg_buffer,MAXLEN*sizeof(unsigned int),0,0);
		if(msg_buffer[0] == XII_SLAVE_NO_ACK_EVENT) sprintf(str,"I2C disconnected");
		else if(msg_buffer[0] == XST_IIC_BUS_BUSY) sprintf(str,"Reset PIC"); //PIC is holding IIC low...
		else sprintf(str,"I2C status %x",msg_buffer[0]);
		VTprintLCD(str,2);
	}
}
