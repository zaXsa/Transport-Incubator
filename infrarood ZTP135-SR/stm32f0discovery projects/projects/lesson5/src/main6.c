/******************************************************************************
 * Project        : HAN ESE PRJ2, PRJ1V & PRJ1D
 * File           : Main program - DAC External Trigger
 * Copyright      : 2013 HAN Embedded Systems Engineering
 ******************************************************************************
  Change History:

    Version 1.0 - May 2013
    > Initial revision

******************************************************************************/
#include "stm32f0xx.h"
#include "stm32f0xx_conf.h"
#include "stm32f0_discovery.h"
#include "helper.h"

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void DAC_Setup(void);

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void)
{
  uint32_t i;
  
  // Configure LED3 and LED4 on STM32F0-Discovery
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
  // Initialize User Button on STM32F0-Discovery
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
  
  DAC_Setup();
  
  while(1)
  {
    // Assuming Uled = 1.8 V
    // (4095 * 1.8) / 3 = 2457

    for(i=4095; i>2457; i--)
    {
      Delay(SystemCoreClock/8/1000);
      DAC_SetChannel1Data(DAC_Align_12b_R, i);
    }

    for(i=2457; i<=4095; i++)
    {
      Delay(SystemCoreClock/8/1000);
      DAC_SetChannel1Data(DAC_Align_12b_R, i);
    }
  }
}

/**
  * @brief  This function sets PA4 to analog mode and initializes
  *         DAC1. The DAC will be triggered externally by the TIM3 TRGO
  *         command. TIM3 is initialzed to generate TRGO trigger every second.
  * @param  None
  * @retval None
  */
void DAC_Setup(void)
{
  GPIO_InitTypeDef        GPIO_InitStructure;
  DAC_InitTypeDef         DAC_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  
  //(+) Enable DAC APB1 clock to get write access to DAC registers
  //    using RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE)
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  
  //(+) Configure DAC_OUT1 (DAC_OUT1: PA4) in analog mode
  //    using GPIO_Init() function  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //(+) Configure the DAC channel using DAC_Init()
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T3_TRGO;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  
  //(+) Enable the DAC channel using DAC_Cmd()
  DAC_Cmd(DAC_Channel_1, ENABLE);
  
  
  //(#) Enable TIM clock using 
  //    RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE) function.
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  
  //(#) Fill the TIM_TimeBaseInitStruct with the desired parameters.
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 1000-1;
  TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t)((SystemCoreClock / 1000) - 1);
  
  //(#) Call TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStruct) to configure 
  //    the Time Base unit with the corresponding configuration.
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  // TIM3 TRGO selection
  TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

  //(#) Call the TIM_Cmd(ENABLE) function to enable the TIM counter.
  TIM_Cmd(TIM3, ENABLE);
}
