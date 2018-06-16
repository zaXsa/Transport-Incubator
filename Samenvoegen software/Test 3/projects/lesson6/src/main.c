/******************************************************************************
 * Project        : HAN ESE PRJ2, Transport Incubator
 * File           : Main program - Master Main
 * Copyright      : 12018 HAN Embedded Systems Engineering ZT5-1
 ******************************************************************************
  Change History:

	version 1.4	- June 12018	> Add Display error reset
   Version 1.3 - June 12018	> Fix Bugs  
	Version 1.2 - June 12018	> Rewrite main  	
	Version 1.1 - May 12018		> Add Libraries
	Version 1.0 - May 12018		> Initial revision

******************************************************************************/
// ----------------------------------------------------------------------------
// Libraries 
// ----------------------------------------------------------------------------
#include "stm32f0xx.h"
#include "stm32f0xx_conf.h"
#include "stm32f0_discovery.h"

#include "bthq21605v.h"
#include "helper.h"
#include "usart.h"
#include "HIH8120.h"
#include "Buttons.h"
#include "PWM.h"
#include "ADC.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
extern uint32_t BTHQ21605V_CommStatus;
extern char rx_buffer;
extern uint32_t ServoPos;
// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void){
	float humidity, temperature;
	uint8_t bufA[4];
	
	// Configure LED3 and LED4 on STM32F0-Discovery
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	
	// Initialize User Button on STM32F0-Discovery
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
	
	// Initializes USART1 (PA9(TX) & PA10(RX))
//	USART_init();																						
//	USART_putstr("Test USART\n");	
		
	// Initialize I2C1 (PB6 (SCL) & (PB7 (SDA)))	
	BTHQ21605V_Setup();																				
	
	// Initialize PWM 
	PWMConfig();
	
	// Initialize ADC for pin PC1 and PC2
	ADC_Setup();
	ADC_ChannelConfig(ADC1, ADC_Channel_11, ADC_SampleTime_239_5Cycles);				// PC1 is de ADC pin
	ADC_ChannelConfig(ADC1, ADC_Channel_12, ADC_SampleTime_239_5Cycles);				// PC2 is de ADC pin
	
	// Initialize the buttons (A4, A5, A6 and A7 )
	InputEnable();
	
	// Initialize BTHQ21605V
	BTHQ21605V_PowerOn();
	BTHQ21605V_Clear();
	
	// Test screen
	BTHQ21605V_Puts((uint8_t *)("Test line 1"));
	BTHQ21605V_GotoXY(0,1);
	BTHQ21605V_Puts((uint8_t *)("Test line 2")); 
	
	// Resets the servo to 0
	TIM_SetCompare4(TIM2, 30000);
	
	// Delays for 2 seconds
	Delay((SystemCoreClock/8)*2);
	while(1){
		// Delays for 1 seconds
		Delay((SystemCoreClock/8));
		
		MesureHIH8120(bufA,4);																		// Function to make a new measurement from the HIH8120
		humidity = ReadHumidity(bufA,4);															// Function to get the latest measured humidity
		setHunmidity(humidity);																		// Sets the PWM for the humidity regulator
		temperature = ReadTemperature(bufA,4);													// Function to get the latest measured temperature	
		setTemperature(temperature);																// Sets the PWM for the temperature regulator
		MeasureADC();																					// Function to make a new measurement from the ZTP135-sr	
		SetDisplay();
		
		CheckError();
	}
}
