/**
  ******************************************************************************
  * @file    stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_conf.h"
#include "stm32f0xx_it.h"
#include "STM32F0_discovery.h"
#include "stm32f0xx_tim.h"
#include "helper.h"

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
extern volatile char rx_buffer;
extern volatile int32_t PulseWidth;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define PulseWithLimiter 1000
#define PULSEWITH 20000
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

void USART1_IRQHandler(void){ 
   // Read Data Register not empty interrupt?
   if(USART1->ISR & USART_ISR_RXNE){
     // Read the data, clears the interrupt flag
     rx_buffer = USART1->RDR;
   }
}

void EXTI0_1_IRQHandler(void){
	STM_EVAL_LEDToggle(LED4);
}

void TIM2_IRQHandler(void){
	//Rate of output variation defined by PulseAdd
	//If PulseAdd is larger, output will vary faster.
	
	static int16_t PulseAdd = 480;
	
	//WARNING: This value was only used to produce the waveform
	//in my logic analyzer image.
	//static int16_t PulseAdd = 10000;
	
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET){
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	
		#ifdef TRIANGLE_MODE
		
		PulseWidth+=PulseAdd;	
		
		//Made PulseWidth bounce between 0 and 48k
		//at a rate defined by the initial value of
		//PulseAdd.
		if(PulseWidth > PULSEWITH){
			PulseWidth = PULSEWITH;
			PulseAdd = -PulseAdd;
		} else if(PulseWidth < 0){
			PulseWidth = 0;
			PulseAdd = -PulseAdd;
		} 
			PulseWidth += PulseAdd;

		
		#else
		//Limit PulseWidth to 48k
		if(PulseWidth>PULSEWITH){
			PulseWidth = PULSEWITH;
		}
		#endif
		
		//Set capture compare register to 1ms+PulseWidth time
		//Each PulseWidth time is equal to 20.833ns
		//meaning 20.8333ns*48000 = 1ms
		//TIM_SetCompare1(TIM2, 96000);														//(TIM2, 48000+PulseWidth); 48000min 72000mid 96000max
	}
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
