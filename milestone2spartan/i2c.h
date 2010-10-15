#ifndef __i2c_h
#define __i2c_h
#include "xiic.h"

// *All* i2c messages are of length IIC_MSGLEN
// This means that all messages that are sent/received via the queues need to
//   be of length IIC_MSGLEN
// This value can be changed -- alternatively, the code could be reworked to
//  for other lengths.
// The code only works for i2c devices that are master or slave.  It has to
//   be reworked for multi-master
#define MAX_IIC_MSGLEN 20
#define IIC_MSGLEN 12
#define TRUE 1
#define FALSE 0
typedef struct __I2C_Comm {
	XIic instance;
	// this is the queue that accepts messages to send
	int incoming_msg_queue_id;
	// this is the queue to which reports on the status of i2c sends
	// are sent
	int out_status_queue_id;
	// purely for the internal i2c use
	int internal_status_queue_id;
	// queue to which received i2c messages are sent
	int outgoing_msg_queue_id;
	Xuint8 iic_recv_msg[MAX_IIC_MSGLEN];
	int	int_id;
} I2C_Comm;

// Types of Status messages
#define I2C_SEND_MSG 0x01
#define I2C_RECV_MSG 0x02
#define SENSOR_BRD_ADDR 0x45
#define INSTEON_ADDR 0x46

// Types of Internal messages
#define MSGTYPE_I2C_STATUS 0x10
#define MSGTYPE_I2C_SEND_COMPLETE 0x11
#define MSGTYPE_I2C_RECV_COMPLETE 0x12

// function definitions
XStatus init_i2c(I2C_Comm *,int,int,int,int);
void *i2c_thread(void *);
#endif
