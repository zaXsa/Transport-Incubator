/******************************************************************************
 * Project        : HAN ESE PRJ2, PRJ1V & PRJ1D
 * File           : USART driver implementation file
 * Copyright      : 2013 HAN Embedded Systems Engineering
 ******************************************************************************
  Change History:

    Version 1.0 - April 2013
    > Initial revision

******************************************************************************/
#include "stm32f0xx.h"
#include "stm32f0xx_conf.h"
#include "bthq21605v.h"
#include "helper.h"
#include "HIH8120.h"

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Local function prototypes
// ----------------------------------------------------------------------------
void ReadHIH8120(uint8_t buf[], int size){
	//// Start I2C write transfer for 1 byte, do not end transfer (SoftEnd_Mode)
	I2C_TransferHandling(I2C1, (0x27<<1), 0, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
	BTHQ21605V_WaitForI2CFlag(I2C_ISR_STOPF);
	I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);

	Delay(SystemCoreClock/8/10);
	//
	if (size < 4){
		// Do nothing
	} else {
		//// 1. Write control byte: select data register
		I2C_TransferHandling(I2C1, (0x27<<1), 4, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
		
		BTHQ21605V_WaitForI2CFlag(I2C_ISR_RXNE);
		//
		//// 1. Read data
		buf[0] = I2C_ReceiveData(I2C1);
		BTHQ21605V_WaitForI2CFlag(I2C_ISR_RXNE);
		
		buf[1] = I2C_ReceiveData(I2C1);
		BTHQ21605V_WaitForI2CFlag(I2C_ISR_RXNE);
	
		buf[2] = I2C_ReceiveData(I2C1);
		BTHQ21605V_WaitForI2CFlag(I2C_ISR_RXNE);
		//
		buf[3] = I2C_ReceiveData(I2C1);
		
		// Wait for- and clear stop condition
		BTHQ21605V_WaitForI2CFlag(I2C_ISR_STOPF);
		I2C1->ICR = I2C_ICR_STOPCF;
	}
}

double ReadHumidity(const uint8_t buf[], const int size){
	int reading_hum;
	
	reading_hum = (buf[0]<<8) + buf[1];
	return reading_hum / 16382.0 * 100.0;
}
double ReadTemperature(const uint8_t buf[], const int size){
	int reading_temp;
	
	reading_temp = (buf[2]<<6) + (buf[3]>>2);
	return reading_temp / 16382.0 * 165.0 - 40;
}
