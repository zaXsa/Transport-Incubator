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
	uint32_t i;
	char c1=0;
	char c2=1; 
	
	int reading_hum;
	double humidity;
	double temperature;
	int reading_temp;
	uint8_t buf;
	// Configure LED3 and LED4 on STM32F0-Discovery
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	
	// Initialize User Button on STM32F0-Discovery
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
	
	// Setup USART1 (PA9(TX) & PA10(RX))
	USART_init();
	USART_clearscreen();
	USART_putstr("This is STM32F0 microcontroller lesson 2.\n");
	USART_putstr("Press different characters.\n\n");
	USART_putstr("Why is the green LED not blinking continuously??\n\n");
	
	// Initialize BTHQ21605V on I2C1 (PB6 (SCL) & (PB7 (SDA)))
	BTHQ21605V_Setup();				
	BTHQ21605V_PowerOn();
	BTHQ21605V_Clear();
	
	BTHQ21605V_Puts((uint8_t *)("Hello"));
	BTHQ21605V_GotoXY(2,1);
	BTHQ21605V_Puts((uint8_t *)("World!"));
	
	Delay((SystemCoreClock/8)*2);
	
	while(1){
		Delay(SystemCoreClock/8);
		
		// In case of error, blink LED3 very fast
		if(BTHQ21605V_CommStatus != BTHQ21605V_COMM_OK){
			while(1){
				STM_EVAL_LEDToggle(LED3);
				Delay(SystemCoreClock/8/20);
			}
		}
		
		// Shift display right
		for(i=0; i<8; i++){
			// Delay ~0.2 sec.
			Delay(SystemCoreClock/8/5);
			STM_EVAL_LEDToggle(LED4);
			BTHQ21605V_WriteInstruction(0x1C); // Right
			
			BTHQ21605V_GotoXY(14-i,0);
			BTHQ21605V_WriteData(0x7F);
		}
		// Shift display left
		for(i=0; i<8; i++){
			// Delay ~0.1 sec.
			Delay(SystemCoreClock/8/10);
			STM_EVAL_LEDToggle(LED4);
			BTHQ21605V_WriteInstruction(0x18); // Left
			
			BTHQ21605V_GotoXY(7+i,0);
			BTHQ21605V_WriteData(BTHQ21605V_EMPTY_CHAR);    
		}
		
		I2C_TransferHandling(I2C1, 0x27, 8, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
		buf = I2C_ReceiveData(I2C1);
		/* Humidity is located in first two bytes */
		reading_hum = (buf << 8);
		humidity = reading_hum / 16382.0 * 100.0;
		USART_putstr("Humidity: ");
		USART_putc(humidity);
		USART_putstr("\n");
		
		/* Temperature is located in next two bytes, padded by two trailing bits */
		reading_temp = (buf << 6);
		temperature = reading_temp / 16382.0 * 165.0 - 40;
		USART_putstr("Temperature: ");
		USART_putc(temperature);
		USART_putstr("\n");
		
		c1 = rx_buffer;
		if(c1 != c2){
			// Yes
			c2=c1;
			USART_putstr("You pressed: ");
			USART_putc(c1);
			USART_putstr("\n");
		}	
	}
}
