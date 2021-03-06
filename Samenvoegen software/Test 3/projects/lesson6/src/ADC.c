/******************************************************************************
 * Project        : HAN ESE PRJ2, Transport Incubator
 * File           : Sub program - ADC initialization and control 
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
#include "Buttons.h"
#include "helper.h"
#include "ADC.h"

#include <math.h>
// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
extern float TempBabyAcc;
extern float HumAcc;
extern float TempAcc;
// ----------------------------------------------------------------------------
// Local function prototypes
// ----------------------------------------------------------------------------
/**
  * @brief  This function initializes the ADC confertion for channel 11 and 12
  * @param  No parameters
  * @retval none
  */
void ADC_Setup(){
  GPIO_InitTypeDef GPIO_InitStructure;
  ADC_InitTypeDef  ADC_InitStructure;
 
  //(#) Enable the ADC interface clock using 
  //    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  
  //(#) ADC pins configuration
  //   (++) Enable the clock for the ADC GPIOs using the following function:
  //        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOx, ENABLE);   
  //   (++) Configure these ADC pins in analog mode using GPIO_Init();  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  //(#) Configure the ADC conversion resolution, data alignment, external
  //    trigger and edge, scan direction and Enable/Disable the continuous mode
  //    using the ADC_Init() function.
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;    
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  // Calibrate ADC before enabling
  ADC_GetCalibrationFactor(ADC1);

  //(#) Activate the ADC peripheral using ADC_Cmd() function.
  ADC_Cmd(ADC1, ENABLE);  

  // Wait until ADC enabled
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN) == RESET); 
}

/**
  * @brief  This function measures the ADC pins and calculates the temperature
  * @param  No parameters
  * @retval returns the temperature of the object infront 
  */
void MeasureADC(){
	uint16_t adc;
	uint16_t adc2;
	
	const float Vin = 3.3;   													// [V]
	const int Rt = 100000;  													// Resistor t [ohm] serie weerstand vaste waarde
	const int R0 = 100000;  													// value of rct in T0 [ohm] 25� C
	const float T0 = 298.15; 													// use T0 in Kelvin [K] 25� C
	const int beta = 3960; 														// static value	
	const float Rinf = R0 * exp(-beta / T0);  				// initial parameters [ohm]
	
	float Vout = 0.0;  																// Vout in A0
	float Rout = 0.0;  																// Rout in A0
	float TempK = 0.0; 																// variable output
	float TempC = 0.0; 
	
	ADC_StartOfConversion(ADC1);
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) {;}
	adc = ADC_GetConversionValue(ADC1);
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) {;}
	adc2 = ADC_GetConversionValue(ADC1);
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOSEQ) == RESET) {;}
	
	// Calculate corresponding voltage level    
	// V25 = (4095 * 1.43V) / 3V = 1952
	// Avg_Slope = 4.3mV / (3V / 4095) = 5,8695 ~ 6
	// temperature = ((1952 - adc) / 6) + 25; 
	
	Vout=Vin*((float)adc/4095); 																	// Calc for ntc
	Rout = ((Rt * Vout) / (Vin - Vout));													// Calc Rout
	TempK = (beta / log(Rout / Rinf)); 														// Calc for temperature 
	TempC = TempK - 273.15;																				// Calc from Kelvin to Celcius 
	  
	TempBabyAcc = Vin*((((float)adc2/4095)-0.5)/0.19)+TempC; 			// Calc temperature of object
}

