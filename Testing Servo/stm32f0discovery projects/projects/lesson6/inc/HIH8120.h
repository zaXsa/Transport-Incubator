/******************************************************************************
 * Project        : HAN ESE PRJ2, PRJ1V & PRJ1D
 * File           : USART driver header file
 * Copyright      : 2013 HAN Embedded Systems Engineering
 ******************************************************************************
  Change History:

    Version 1.0 - April 2013
    > Initial revision

******************************************************************************/
#ifndef _HIH8120_H_
#define _HIH8120_H_

#include "stdint.h"

/******************************************************************************
  Defines
******************************************************************************/


/******************************************************************************
  Function prototypes
******************************************************************************/
void MesureHIH8120(uint8_t buf[], int size);
double ReadHumidity(const uint8_t buf[], const int size);
double ReadTemperature(const uint8_t buf[], const int size);
void left(void);
void right(void);
#endif // _HIH8120_H_
