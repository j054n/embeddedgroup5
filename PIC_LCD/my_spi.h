#ifndef _MY_SPI_H
#define _MY_SPI_H
#include "maindefs.h"
#include <spi.h>

#define RESET LATCbits.LATC7
void initSPI(void);
#endif