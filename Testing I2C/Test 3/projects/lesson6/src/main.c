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
// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void){
	double humidity;
	double temperature;
	uint8_t buf[4] = {1,2,3,4};
	char charbuf[10];
	// Configure LED3 and LED4 on STM32F0-Discovery
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	
	// Initialize User Button on STM32F0-Discovery
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
	
	SYSCFG->EXTICR[0]  &= ~(0x000F);
	EXTI->RTSR |= EXTI_RTSR_TR0;
	EXTI->RTSR |= EXTI_IMR_MR0;
	NVIC_SetPriority(EXTI0_1_IRQn,1);
	NVIC_EnableIRQ(EXTI0_1_IRQn);
	
	// Setup USART1 (PA9(TX) & PA10(RX))
	USART_init();																										// Initializes USART 1
	USART_putstr("Test USART\n");
	
	// Initialize I2C1 (PB6 (SCL) & (PB7 (SDA)))
	BTHQ21605V_Setup();																								// Initializes I2C 1
	
	// Initialize BTHQ21605V
	//BTHQ21605V_PowerOn();
	//BTHQ21605V_Clear();
	//
	//BTHQ21605V_Puts((uint8_t *)("Test line 1"));
	//BTHQ21605V_GotoXY(2,1);
	//BTHQ21605V_Puts((uint8_t *)("Test line 2")); 
	
	Delay((SystemCoreClock/8)*2);
	
	while(1){
		// In case of error, blink LED3 very fast
		//if(BTHQ21605V_CommStatus != BTHQ21605V_COMM_OK){
		//	while(1){
		//		STM_EVAL_LEDToggle(LED3);
		//		Delay(SystemCoreClock/8/20);
		//	}
		//}
	
		Delay(SystemCoreClock/8);
		BTHQ21605V_Clear();
		
		MesureHIH8120(buf,4);																						// Function to make a new measurement from the HIH8120
	
		/* Display the the incommeing data in usart*/
		USART_putstr("incomming data: \n");
		itoa_simple(charbuf, buf[0]);		
		USART_putstr(charbuf);
		USART_putstr("\n");
		itoa_simple(charbuf, buf[1]);                                                
		USART_putstr(charbuf);                                                       
		USART_putstr("\n");                                                          
		itoa_simple(charbuf, buf[2]);                                                
		USART_putstr(charbuf);                                                       
		USART_putstr("\n");                                                          
		itoa_simple(charbuf, buf[3]);                                                
		USART_putstr(charbuf);                                                       
		USART_putstr("\n");
		
		/* Get the humidity and display it */
		humidity = ReadHumidity(buf,4);																			// Function to get the latest measured humidity
		USART_putstr("Humidity: ");
		itoa_simple(charbuf, humidity);
		USART_putstr(charbuf);
		USART_putstr("\n");
		
		/* Display the humidity on the first line of the  LCD screen */
		//BTHQ21605V_GotoXY(1,1);
		//BTHQ21605V_Puts((uint8_t *)("Hum:"));
		//BTHQ21605V_GotoXY(1,7);
		//BTHQ21605V_Puts((uint8_t *)(charbuf));
		
		/* Get the temperature and display it */
		temperature = ReadTemperature(buf,4);																	// Function to get the latest measured temperature
		itoa_simple(charbuf, temperature);
		USART_putstr("Temperature: ");
		USART_putstr(charbuf);
		USART_putstr("\n");		
		
		/* Display the temperture on the second line of the LCD screen */
		//BTHQ21605V_GotoXY(2,1);
		//BTHQ21605V_Puts((uint8_t *)("Temp:"));
		//BTHQ21605V_GotoXY(1,7);
		//BTHQ21605V_Puts((uint8_t *)(charbuf));
	}
}
