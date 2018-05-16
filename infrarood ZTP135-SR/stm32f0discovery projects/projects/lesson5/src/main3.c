/******************************************************************************
 * Project        : HAN ESE PRJ2, PRJ1V & PRJ1D
 * File           : Main program - ADC Temperature Sensor
 * Copyright      : 2013 HAN Embedded Systems Engineering
 ******************************************************************************
  Change History:

    Version 1.0 - May 2013
    > Initial revision

******************************************************************************/
#include "stm32f0xx.h"
#include "stm32f0xx_conf.h"
#include "stm32f0_discovery.h"
#include "helper.h"
#include <math.h>

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void  ADC_Setup(void);

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void)
{
  uint16_t adc;
	 uint16_t adc2;
	

const float Vin = 3.3;   // [V]
const int Rt = 100000;  // Resistor t [ohm] serie weerstand vaste waarde
const int R0 = 100000;  // value of rct in T0 [ohm] 25º C
const float T0 = 298.15; // use T0 in Kelvin [K] 25º C
const int beta = 3960; 

float Vout = 0.0;  // Vout in A0
float Rout = 0.0;  // Rout in A0
float Rinf = 0.0;  // initial parameters [ohm]
float TempK = 0.0; // variable output
float TempC = 0.0; // variable output
float TempInfra = 0.0;
//float test= 0.0;

	  char str[10];
	
	Rinf = R0 * exp(-beta / T0);
	

  
  // Configure LED3 and LED4 on STM32F0-Discovery
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
  // Initialize User Button on STM32F0-Discovery
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
  
  USART_Setup();
  USART_Clearscreen();
  USART_Putstr("Lesson 5: ADC Temperature Sensor\n");
  
  ADC_Setup();

  //(#) Enable the internal connection of Temperature sensor, Vrefint or Vbat sources 
  //    with the ADC channels using ADC_TempSensorCmd(), ADC_VrefintCmd() or ADC_VbatCmd()
  //    functions. 
 // ADC_TempSensorCmd(ENABLE);
  
  //(#) select the ADC_Channel_16(Temperature sensor), ADC_Channel_17(Vrefint)
  //    or ADC_Channel_18(Voltage battery) using ADC_ChannelConfig() function 
  //
  // Accoording the datasheet the sampling time when reading temperature must be at
  // least 17.1 us. (see table 6.3.19)
  // As the ADC operates at 14 MHz, one cycle takes 1/14M sec.
  // So the minimum cycles required are 17.1 u / (1/14M) = 239.4
  ADC_ChannelConfig(ADC1, ADC_Channel_11, ADC_SampleTime_239_5Cycles);//PC1 is de ADC pin
  
  // Start the first conversion
  ADC_StartOfConversion(ADC1);

  while(1)
  {
    // Delay ~0.2 sec.
    Delay(SystemCoreClock/8/5);
		 Delay(SystemCoreClock/8);// 1 sec
    
    //(#) Get the voltage values, using ADC_GetConversionValue() function
    adc = ADC_GetConversionValue(ADC1);

    // Calculate corresponding voltage level    
    // V25 = (4095 * 1.43V) / 3V = 1952
    // Avg_Slope = 4.3mV / (3V / 4095) = 5,8695 ~ 6
    //temperature = ((1952 - adc) / 6) + 25; //is een hier komt de formule voor onze sensor voorbeeld wij hebben een andere sensor
	//	test = adc;
		
		Vout=Vin*((float)adc/4095); // calc for ntc
		 //Vout=Vin*(test/4095); // calc for ntc
  //Vout = Vin * ((1) / 3.0); // voor het test 
  Rout = ((Rt * Vout) / (Vin - Vout));
    TempK = (beta / log(Rout / Rinf)); // calc for temperature 
    TempC = TempK - 273.15;
		
//TempInfra=

    // Output the values
    USART_Putstr("Temperature: ");
    USART_itoa(TempC, str);
    USART_Putstr(str);
    USART_Putstr(" degrees Celcius\r\n");
		
		    USART_Putstr("ADC ");
    USART_itoa(adc, str);
    USART_Putstr(str);
    USART_Putstr(" ADC\r\n");
		

			 
		

  }
}

/**
  * @brief  This function initializes and calibrates ADC1.
  * @param  None
  * @retval None
  */
void ADC_Setup(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  ADC_InitTypeDef  ADC_InitStructure;
 
  //(#) Enable the ADC interface clock using 
  //    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  //(#) ADC pins configuration
  //   (++) Enable the clock for the ADC GPIOs using the following function:
  //        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOx, ENABLE);   
  //   (++) Configure these ADC pins in analog mode using GPIO_Init();  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
  
  //(#) Configure the ADC conversion resolution, data alignment, external
  //    trigger and edge, scan direction and Enable/Disable the continuous mode
  //    using the ADC_Init() function.
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
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
