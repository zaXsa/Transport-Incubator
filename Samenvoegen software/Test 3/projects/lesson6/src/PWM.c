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
#include "helper.h"
#include "PWM.h"
#include "math.h"

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
uint32_t ServoPos = 30000;
float TempDesired = 35;
float HumDesired = 60;
float TempBabyAcc = 0;
float HumAcc = 0;
float TempAcc = 0;
// ----------------------------------------------------------------------------
// Local function prototypes
// ----------------------------------------------------------------------------

/**
  * @brief  This function initializes the PWM
  * @param  No parameters
  * @retval none
  */
void PWMConfig(){
	GPIO_InitTypeDef        	GPIO_InitStructure1, GPIO_InitStructure2, GPIO_InitStructure3;  
	TIM_TimeBaseInitTypeDef 	TIM_TimeBaseStructure;
	TIM_OCInitTypeDef       	TIM_OCInitStructure1, TIM_OCInitStructure2, TIM_OCInitStructure3;
	
	//(#) Enable TIM clock using 
	//    RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE) function.
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	// Timer 2 enabled
	
	//(#) Configure the TIM pins by configuring the corresponding GPIO pins
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	//
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_2);
	
	//
	GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_2);
	
	//
	GPIO_InitStructure3.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure3.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure3.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure3);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_2);
	
	//(#) Configure the Time base unit as described in the first part of this 
	//    driver, if needed, else the Timer will run with the default 
	//    configuration:
	//    (++) Autoreload value = 0xFFFF.
	//    (++) Prescaler value = 0x0000.
	//    (++) Counter mode = Up counting.
	//    (++) Clock Division = TIM_CKD_DIV1.
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_Period = 960000 - 1;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	//(#) Fill the TIM_OCInitStruct with the desired parameters including:
	//    (++) The TIM Output Compare mode: TIM_OCMode.
	//    (++) TIM Output State: TIM_OutputState.
	//    (++) TIM Pulse value: TIM_Pulse.
	//    (++) TIM Output Compare Polarity : TIM_OCPolarity.
	TIM_OCInitStructure1.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure1.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure1.TIM_Pulse = 0;
	TIM_OCInitStructure1.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OCInitStructure2.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure2.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure2.TIM_Pulse = 0;
	TIM_OCInitStructure2.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OCInitStructure3.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure3.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure3.TIM_Pulse = 0;
	TIM_OCInitStructure3.TIM_OCPolarity = TIM_OCPolarity_High;
	
	//(#) Call TIM_OCxInit(TIMx, &TIM_OCInitStruct) to configure the desired 
	//    channel with the corresponding configuration.
	TIM_OC2Init(TIM2, &TIM_OCInitStructure1);
	TIM_OC3Init(TIM2, &TIM_OCInitStructure2);
	TIM_OC4Init(TIM2, &TIM_OCInitStructure3);
	
	//(#) Call the TIM_Cmd(ENABLE) function to enable the TIM counter.
	TIM_Cmd(TIM2, ENABLE);
}

/**
  * @brief  This function makes the servo turn to the most left side
  * @param  No parameters
  * @retval none
  */
void left(){
	for (ServoPos=94000;ServoPos>30000;ServoPos -= 150){							// 84000 38000
		TIM_SetCompare4(TIM2, ServoPos);													//(TIM2, 48000+PulseWidth); 12000 min 48000 mid 96000 max
		Delay(SystemCoreClock/8/200);														// Delays for 10ms
	}
	TIM_SetCompare4(TIM2, 0);	
}

/**
  * @brief  This function makes the servo turn to the most right side
  * @param  No parameters
  * @retval none
  */
void right(){
	for (ServoPos=30000;ServoPos<96000;ServoPos += 150){							// 84000 38000
		TIM_SetCompare4(TIM2, ServoPos);													//(TIM2, 48000+PulseWidth); 12000 min 48000 mid 96000 max
		Delay(SystemCoreClock/8/200);														// Delays for 10ms
	}
	TIM_SetCompare4(TIM2, 0);	
}

void setServo(const uint32_t newServoPos){
	if(newServoPos < ServoPos){
		for (ServoPos=ServoPos;ServoPos>newServoPos;ServoPos -= 150){							// 84000 38000
			TIM_SetCompare4(TIM2, ServoPos);																//(TIM2, 48000+PulseWidth); 12000 min 48000 mid 96000 max
			Delay(SystemCoreClock/8/200);																	// Delays for 10ms
		}
	}else {
		for (ServoPos=ServoPos;ServoPos<newServoPos;ServoPos += 150){							// 84000 38000
			TIM_SetCompare4(TIM2, ServoPos);																//(TIM2, 48000+PulseWidth); 12000 min 48000 mid 96000 max
			Delay(SystemCoreClock/8/200);																	// Delays for 10ms
		}
	}
	

	TIM_SetCompare4(TIM2, 0);	
}

/**
  * @brief  This function makes calculates the ideal pwm for the temp and sets the pwm for the temperature regulator
  * @param  The current measured temp
  * @retval none
  */
void setTemperature(float Temp){
	uint32_t NewTemp;
	long BufPWM;
	BufPWM = 9600*(2*pow(TempDesired-Temp,3) + 30 );
	if(BufPWM > 950000){
		NewTemp = 950000;
	}else if(BufPWM < 0){
		NewTemp = 0;
	}else{
		NewTemp = BufPWM;
	}	
	TIM_SetCompare2(TIM2,NewTemp);
}

/**
  * @brief  This function calculates the ideal setting for the humidity and sets the right setting
  * @param  The current humidity
  * @retval none
  */
void setHunmidity(float Hum){
	if(HumDesired-Hum < 5){
		setServo(30000);
	}else if(HumDesired-Hum < 20){
		setServo(52000);
	}else
		setServo(96000);
}
