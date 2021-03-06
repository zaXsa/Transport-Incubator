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

#include "Buttons.h"
// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
extern volatile char rx_buffer;
extern int CurrentMode;
extern float HumDesired;
extern float TempDesired;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
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
/**
  * @brief  This function handles USART interrupts.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void){ 
   if(USART1->ISR & USART_ISR_RXNE){											   				// Read Data Register not empty interrupt?
     rx_buffer = USART1->RDR;																				// Read the data, clears the interrupt flag	
   }	
}	
/**
  * @brief  This function handles buttons on line 0 and 1.
  * @param  None
  * @retval None
  */	
void EXTI0_1_IRQHandler(void){	
	//if(EXTI_GetITStatus(EXTI_Line0)){																	// Easier way
	//	STM_EVAL_LEDToggle(LED4);	
	//	EXTI_ClearFlag(EXTI_Line0);
	//}
}
/**
  * @brief  This function handles buttons on line 4 to 15
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line4)){																	
		DesiredUp();	
		EXTI_ClearFlag(EXTI_Line4);
	}
	if(EXTI_GetITStatus(EXTI_Line5)){
		DesiredDown();	
		EXTI_ClearFlag(EXTI_Line5);
	}
	if(EXTI_GetITStatus(EXTI_Line6)){
		STM_EVAL_LEDToggle(LED4);	
		EXTI_ClearFlag(EXTI_Line6);
	}
	if(EXTI_GetITStatus(EXTI_Line7)){
		STM_EVAL_LEDToggle(LED4);	
		SetNextMode();
		EXTI_ClearFlag(EXTI_Line7);
	}
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
