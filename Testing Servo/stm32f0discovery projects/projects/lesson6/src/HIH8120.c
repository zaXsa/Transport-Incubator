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
uint32_t ServoPos;
// ----------------------------------------------------------------------------
// Local function prototypes
// ----------------------------------------------------------------------------

/**
  * @brief  This function makes the HIH8120 start is measurements and saves it in a array
  * @param  The array where the data is saved in and the size of the array.
  * @retval The HIH8120 data in the array
  */
void MesureHIH8120(uint8_t buf[], int size){
	// Wait while I2C peripheral is not ready
	BTHQ21605V_WaitForI2CFlag(I2C_ISR_BUSY);

	// Start I2C write transfer for 0 byte, do end transfer (I2C_AutoEnd_Mode)
	// causes the HIH8120 to make it's measurements.
	I2C_TransferHandling(I2C1, (0x27<<1), 0, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
	BTHQ21605V_WaitForI2CFlag(I2C_ISR_STOPF);
	I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);

	// Delays for 100ms
	Delay(SystemCoreClock/8/10);
	// Start I2C write transfer for 4 byte, do end transfer (I2C_AutoEnd_Mode)
	// Reads the read the measured data from the HIH8120 
	I2C_TransferHandling(I2C1, (0x27<<1), 4, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
	
	// Waits till the reading is done and saves the data
	BTHQ21605V_WaitForI2CFlag(I2C_ISR_RXNE);
	buf[0] = I2C_ReceiveData(I2C1);
	
	BTHQ21605V_WaitForI2CFlag(I2C_ISR_RXNE);
	buf[1] = I2C_ReceiveData(I2C1);
	
	BTHQ21605V_WaitForI2CFlag(I2C_ISR_RXNE);
	buf[2] = I2C_ReceiveData(I2C1);
	
	BTHQ21605V_WaitForI2CFlag(I2C_ISR_RXNE);
	buf[3] = I2C_ReceiveData(I2C1);
	
	// Wait for- and clear stop condition
	BTHQ21605V_WaitForI2CFlag(I2C_ISR_STOPF);
	// Clears the stop condition flag
	I2C1->ICR = I2C_ICR_STOPCF;
}

/**
  * @brief  This function uses the data from the HIH8120 and calculates the humidity
  * @param  The array with the needed data and the size of the array
  * @retval returns the humidity as a double
  */
double ReadHumidity(const uint8_t buf[], const int size){
	int reading_hum;
	
	// Uses the first 2 bytes from the HIH8120 and calculates the humidity
	reading_hum = (buf[0]<<8) + buf[1];
	return reading_hum / 16382.0 * 100.0;
}

/**
  * @brief  This function uses the data from the HIH8120 and calculates the temperature
  * @param  The array with the needed data and the size of the array
  * @retval returns the temperature as a double
  */
double ReadTemperature(const uint8_t buf[], const int size){
	int reading_temp;
	
	// Uses the last 2 bytes from the HIH8120 and calculates the Temperature
	reading_temp = (buf[2]<<6) + (buf[3]>>2);
	return reading_temp / 16382.0 * 165.0 - 40;
}

void left(){
	for (ServoPos=94000;ServoPos>=35000;ServoPos -= 150){														// 84000 38000
		TIM_SetCompare1(TIM2, ServoPos);															//(TIM2, 48000+PulseWidth); 12000 min 48000 mid 96000 max
		Delay(SystemCoreClock/8/200);														// Delays for 10ms
	}
	TIM_SetCompare1(TIM2, 0);	
}

void right(){
	for (ServoPos=35000;ServoPos<=96000;ServoPos += 150){														// 84000 38000
		TIM_SetCompare1(TIM2, ServoPos);															//(TIM2, 48000+PulseWidth); 12000 min 48000 mid 96000 max
		Delay(SystemCoreClock/8/200);														// Delays for 10ms
	}
	TIM_SetCompare1(TIM2, 0);	
}
