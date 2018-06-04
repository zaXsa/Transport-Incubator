/******************************************************************************
 * Project        : HAN ESE PRJ2, PRJ1V & PRJ1D
 * File           : Main program - I2C 2x16 LCD BTHQ21605V
 * Copyright      : 2013 HAN Embedded Systems Engineering
 ******************************************************************************
  Change History:

    Version 1.0 - May 2013
    > Initial revision

******************************************************************************/
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
extern volatile char rx_buffer;
extern uint32_t ServoPos;
// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void){
	volatile float humidity, temperature;
	float TempInfra;
	uint8_t buf[4];
	
	// Configure LED3 and LED4 on STM32F0-Discovery
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	
	// Initialize User Button on STM32F0-Discovery
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
	
	// Setup USART1 (PA9(TX) & PA10(RX))
//	USART_init();																										// Initializes USART 1
//	USART_putstr("Test USART\n");
	
	// Initialize I2C1 (PB6 (SCL) & (PB7 (SDA)))
	BTHQ21605V_Setup();																							// Initializes I2C 1
	
	// Initialize the buttons (A4, A5, A6 and A7 )
	InputEnable();
	
	// Initialize PWM 
	PWMConfig();
	
	// Initialize ADC for pin PC1 and PC2
	ADC_Setup();
	ADC_ChannelConfig(ADC1, ADC_Channel_11, ADC_SampleTime_239_5Cycles);			//PC1 is de ADC pin
	ADC_ChannelConfig(ADC1, ADC_Channel_12, ADC_SampleTime_239_5Cycles);			//PC2 is de ADC pin
	
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
	// In case of error, set LED3 on
	if(BTHQ21605V_CommStatus != BTHQ21605V_COMM_OK){
		STM_EVAL_LEDOn(LED3);
	}
	while(1){
		// Delays for 1 seconds
		Delay((SystemCoreClock/8));
		
		MesureHIH8120(buf,4);																						// Function to make a new measurement from the HIH8120
		humidity = ReadHumidity(buf,4);																			// Function to get the latest measured humidity
		//setHunmidity(humidity);																						// Sets the PWM for the humidity regulator
		temperature = ReadTemperature(buf,4);																	// Function to get the latest measured temperature	
		//setTemperature(temperature);																				// Sets the PWM for the temperature regulator
		MeasureADC(&TempInfra);																						// Function to make a new measurement from the ZTP135-sr	
		SetDisplay();
	}
}
