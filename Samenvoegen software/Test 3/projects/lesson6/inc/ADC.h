/******************************************************************************
 * Project        : HAN ESE PRJ2, PRJ1V & PRJ1D
 * File           : USART driver header file
 * Copyright      : 2013 HAN Embedded Systems Engineering
 ******************************************************************************
  Change History:

    Version 1.0 - April 2013
    > Initial revision

******************************************************************************/
#ifndef _ADC_H_
#define _ADC_H_

#include "stdint.h"

/******************************************************************************
  Defines
******************************************************************************/


/******************************************************************************
  Function prototypes
******************************************************************************/
void ADC_Setup(void);
void MeasureADC(float *TempInfra);
void extra(volatile uint16_t *adc, volatile uint16_t *adc2);

#endif // _ADC_H_
