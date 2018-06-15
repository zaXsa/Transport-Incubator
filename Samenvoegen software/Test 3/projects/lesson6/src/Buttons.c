/******************************************************************************
 * Project        : HAN ESE PRJ2, Transport Incubator
 * File           : Sub program - Buttons initialization and control 
 * Copyright      : 12018 HAN Embedded Systems Engineering ZT5-1
 ******************************************************************************
  Change History:

    Version 1.0 - May 12018		> Initial revision

******************************************************************************/
// ----------------------------------------------------------------------------
// Libraries 
// ----------------------------------------------------------------------------
#include "stm32f0xx.h"
#include "stm32f0xx_conf.h"
#include "helper.h"
#include "Buttons.h"
#include "bthq21605v.h"

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
int CurrentMode = 0;
extern float HumDesired;
extern float TempDesired;
extern float TempBabyAcc;

// ----------------------------------------------------------------------------
// Local function prototypes
// ----------------------------------------------------------------------------

/**
  * @brief  This function initializes the interrupt input buttons A4, A5, A6 and A7
  * @param  No parameters
  * @retval none
  */
void InputEnable(){
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	// Pin config for PA4 to PA7 input 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	 /* Connect Button EXTI Line to Button GPIO Pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource5);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource6);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource7);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line4 | EXTI_Line5 | EXTI_Line6 | EXTI_Line7;														/* PB12 is connected to EXTI_Line12 */
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;															/* Enable interrupt */
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;										/* Interrupt mode */
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;								/* Triggers on rising edge */
	EXTI_Init(&EXTI_InitStructure);																				/* Add to EXTI */	
	
		/* Add IRQ vector to NVIC */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;										/* PB12 is connected to EXTI_Line12, which has EXTI15_10_IRQn vector */
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;										/* Set priority */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;												/* Enable interrupt */	
	NVIC_Init(&NVIC_InitStructure);																				/* Add to NVIC */
}	

/**
  * @brief  This function sets the new mode and updates the display
  * @param  No parameters
  * @retval The new mode
  */
void SetNextMode(){
	CurrentMode++;
	if (CurrentMode>2){
		CurrentMode = 0;
	}
	SetDisplay();	
	Delay((SystemCoreClock/8/50));
}

/**
  * @brief  This function checks the currentmode and adds to the Desired amount 
  * @param  No parameters
  * @retval Updated Desired amount
  */
void DesiredUp(){
	switch(CurrentMode){
		case 0:
			TempDesired++;
			if(TempDesired > 38){
				TempDesired = 38;
			}
			break;
		case 1:
			HumDesired++;
			if(HumDesired > 80){
				HumDesired = 80;
			}
			break;
		case 2:
			break;
		default:
			break;
	}
	SetDisplay();
	Delay((SystemCoreClock/8/50));
}

/**
  * @brief  This function checks the currentmode and retracts from the Desired amount 
  * @param  No parameters
  * @retval Updated Desired amount
  */
void DesiredDown(){
	switch(CurrentMode){
		case 0:
			TempDesired--;
			if(TempDesired < 24){
				TempDesired = 24;
			}
			break;
		case 1:
			HumDesired--;
			if(HumDesired < 40){
				HumDesired = 40;
			}
			break;
		case 2:
			break;
		default:
			break;
	}
	SetDisplay();
	Delay((SystemCoreClock/8/50));
}
