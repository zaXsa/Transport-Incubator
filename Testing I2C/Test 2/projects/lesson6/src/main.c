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
static char *itoa_simple_helper(char *dest, int i) {
  if (i <= -10) {
    dest = itoa_simple_helper(dest, i/10);
  }
  *dest++ = '0' - i%10;
  return dest;
}

char *itoa_simple(char *dest, int i) {
  char *s = dest;
  if (i < 0) {
    *s++ = '-';
  } else {
    i = -i;
  }
  *itoa_simple_helper(s, i) = '\0';
  return dest;
}
// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void){
	//uint32_t i;
	char c1=0;
	char c2=1; 
	
	int reading_hum;
	double humidity;
	double temperature;
	int reading_temp;
	uint8_t buf[4] = {1,2,3,4};
	char charbuf[10];
	// Configure LED3 and LED4 on STM32F0-Discovery
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	
	// Initialize User Button on STM32F0-Discovery
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
	
	// Setup USART1 (PA9(TX) & PA10(RX))
	USART_init();																										// Initializes USART 1
	USART_putstr("Test USART\n");
	//USART_clearscreen();
	USART_putstr("Press different characters.\n\n");
	//USART_putstr ("Why is the green LED not blinking continuously??\n\n");
	
	// Initialize BTHQ21605V on I2C1 (PB6 (SCL) & (PB7 (SDA)))
	BTHQ21605V_Setup();																								// Initializes I2C 1
	//BTHQ21605V_PowerOn();
	//BTHQ21605V_Clear();
	
	//BTHQ21605V_Puts((uint8_t *)("Hello"));
	//BTHQ21605V_GotoXY(2,1);
	//BTHQ21605V_Puts((uint8_t *)("World!")); 
	
	Delay((SystemCoreClock/8)*2);
	
	while(1){
		Delay(SystemCoreClock/8);
		
		// In case of error, blink LED3 very fast
		//if(BTHQ21605V_CommStatus != BTHQ21605V_COMM_OK){
		//	while(1){
		//		STM_EVAL_LEDToggle(LED3);
		//		Delay(SystemCoreClock/8/20);
		//	}
		//}
		//
		//// Shift display right
		//for(i=0; i<8; i++){
		//	// Delay ~0.2 sec.
		//	Delay(SystemCoreClock/8/5);
		//	STM_EVAL_LEDToggle(LED4);
		//	BTHQ21605V_WriteInstruction(0x1C); // Right
		//	
		//	BTHQ21605V_GotoXY(14-i,0);
		//	BTHQ21605V_WriteData(0x7F);
		//}
		//// Shift display left
		//for(i=0; i<8; i++){
		//	// Delay ~0.1 sec.
		//	Delay(SystemCoreClock/8/10);
		//	STM_EVAL_LEDToggle(LED4);
		//	BTHQ21605V_WriteInstruction(0x18); // Left
		//	
		//	BTHQ21605V_GotoXY(7+i,0);
		//	BTHQ21605V_WriteData(BTHQ21605V_EMPTY_CHAR);    
		//}
		
		// Wait while I2C peripheral is not ready
		BTHQ21605V_WaitForI2CFlag(I2C_ISR_BUSY);

		// Start I2C write transfer for 1 byte, do not end transfer (SoftEnd_Mode)
		I2C_TransferHandling(I2C1, 0x27, 9, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
		BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);

		// 1. Write control byte: select data register
		I2C_SendData(I2C1, 0x27 | 1);
		Delay(SystemCoreClock/8);
		// Repeated start I2C read transfer for 1 byte
		I2C_TransferHandling(I2C1, 0x27, 8, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
		BTHQ21605V_WaitForI2CFlag(I2C_ISR_RXNE);
		
		// 1. Read data
		buf[0] = I2C_ReceiveData(I2C1);
		BTHQ21605V_WaitForI2CFlag(I2C_ISR_RXNE);
		buf[1] = I2C_ReceiveData(I2C1);
		BTHQ21605V_WaitForI2CFlag(I2C_ISR_RXNE);
		buf[2] = I2C_ReceiveData(I2C1);
		BTHQ21605V_WaitForI2CFlag(I2C_ISR_RXNE);
		buf[3] = I2C_ReceiveData(I2C1);
		BTHQ21605V_WaitForI2CFlag(I2C_ISR_RXNE);		
		//Wait for- and clear stop condition
		BTHQ21605V_WaitForI2CFlag(I2C_ISR_STOPF);
		I2C1->ICR = I2C_ICR_STOPCF;
		
		/* Humidity is located in first two bytes */
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
		
		
		reading_hum = (buf[0]<<8) + buf[1];
		humidity = reading_hum / 16382.0 * 100.0;
		USART_putstr("Humidity: ");
		itoa_simple(charbuf, humidity);
		USART_putstr(charbuf);
		USART_putstr("\n");
		
		/* Temperature is located in next two bytes, padded by two trailing bits */
		reading_temp = (buf[2]<<6) + (buf[3]>>2);
		temperature = reading_temp / 16382.0 * 165.0 - 40;
		itoa_simple(charbuf, temperature);
		USART_putstr("Temperature: ");
		USART_putstr(charbuf);
		USART_putstr("\n");
		
		
		//c1 = USART_getc();
		//while(USART_getc() != '\n');
		if(c1 == c2){
			// Yes
			c2=c1;
			USART_putstr("You pressed: ");
			USART_putc(c1);
			USART_putstr("\n");
		}	
	}
}
