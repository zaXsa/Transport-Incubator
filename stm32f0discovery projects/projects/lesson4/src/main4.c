/******************************************************************************
 * Project        : HAN ESE PRJ2, PRJ1V & PRJ1D
 * File           : Main program - SysTick
 * Copyright      : 2013 HAN Embedded Systems Engineering
 ******************************************************************************
  Change History:

    Version 1.0 - May 2013
    > Initial revision

******************************************************************************/
#include "stm32f0xx.h"
#include "stm32f0xx_conf.h"
#include "stm32f0_discovery.h"

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void)
{
  // Configure LED3 and LED4 on STM32F0-Discovery
  STM_EVAL_LEDInit(LED3);
  
  // Setup SysTick Timer for 5 msec interrupts
  if(SysTick_Config((SystemCoreClock * 0.005)-1))
  { 
    // Error!
    // Do nothing, LED3 will stay off indicating an erronous situation.
    while(1){;}
  }

  while(1)
  {
    ;
  }
}
