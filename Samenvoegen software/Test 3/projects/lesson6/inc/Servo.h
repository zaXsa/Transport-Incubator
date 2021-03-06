/******************************************************************************
 * Project        : HAN ESE PRJ2, PRJ1V & PRJ1D
 * File           : USART driver header file
 * Copyright      : 2013 HAN Embedded Systems Engineering
 ******************************************************************************
  Change History:

    Version 1.0 - April 2013
    > Initial revision

******************************************************************************/
#ifndef _SERVO_H_
#define _SERVO_H_

#include "stdint.h"

/******************************************************************************
  Defines
******************************************************************************/


/******************************************************************************
  Function prototypes
******************************************************************************/
void PWMConfig(void);
void right(void);
void left(void);
void setTemp(float Temp);

#endif // _SERVO_H_
