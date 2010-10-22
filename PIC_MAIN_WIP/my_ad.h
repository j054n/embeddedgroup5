#ifndef __MY_AD_H
#define __MY_AD_H
#include <adc.h>
#include "messages.h"
#define BUFF_BYTES 2
#define LIGHT1 ADC_CH1
#define LIGHT2 ADC_CH2
#define SOUND1 ADC_CH3
#define CONTACT1 ADC_CH4

(void *) readADCChan(unsigned char);
void readSensors(void); 
void readADC(void);
void initADC(void);
#endif