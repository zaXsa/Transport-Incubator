/******************************************************************************
 * Project        : HAN ESE PRJ2, PRJ1V & PRJ1D
 * File           : BTHQ21605V implementation
 * Copyright      : 2013 HAN Embedded Systems Engineering
 ******************************************************************************
  Change History:

    Version 1.0 - May 2013
    > Initial revision

******************************************************************************/
#include "stm32f0xx_conf.h"
#include "stm32f0_discovery.h"
#include "bthq21605v.h"
#include "helper.h"
#include "usart.h"

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
uint32_t BTHQ21605V_CommStatus = BTHQ21605V_COMM_OK;
extern int CurrentMode;
extern float HumDesired;
extern float TempDesired;
extern float TempBabyAcc;
extern float HumAcc;
extern float TempAcc;
// ----------------------------------------------------------------------------
// Local function prototypes
// ----------------------------------------------------------------------------
//void BTHQ21605V_WaitForI2CFlag(uint32_t flag);

/**
  * @brief  This function sets up the I2C1 hardware as mentioned in the
  *         hardware description in the header file. Afterwards I2C1 is
  *         enabled.
  * @param  None
  * @retval None
  */
void BTHQ21605V_Setup(void){
  GPIO_InitTypeDef GPIO_InitStructure;
  I2C_InitTypeDef  I2C_InitStructure;
  
  // Set I2C1 clock to SYSCLK (see system_stm32f0.c)
  RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);

  //(#) Enable peripheral clock using RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2Cx, ENABLE)
  //    function for I2C1 or I2C2.
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  
  //(#) Enable SDA, SCL  and SMBA (when used) GPIO clocks using 
  //    RCC_AHBPeriphClockCmd() function. 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  
  // Enable PB8 and make it active high
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  //(#) Peripherals alternate function: 
  //    (++) Connect the pin to the desired peripherals' Alternate 
  //         Function (AF) using GPIO_PinAFConfig() function.
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_1);

  //    (++) Configure the desired pin in alternate function by:
  //         GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;

  //    (++) Select the type, OpenDrain and speed via  
  //         GPIO_PuPd, GPIO_OType and GPIO_Speed members
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  //    (++) Call GPIO_Init() function.
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  //(#) Program the Mode, Timing , Own address, Ack and Acknowledged Address 
  //    using the I2C_Init() function.
  I2C_StructInit(&I2C_InitStructure);
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter = 0;
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_OwnAddress1 = 0;
//I2C_InitStructure.I2C_Timing = 0x00310309; // ~400 kHz. @ 8 MHz (HSI) see Ref. Man. Table 72
//I2C_InitStructure.I2C_Timing = 0x50330309; // ~400 kHz. @ 48 MHz (SYSCLK) see Ref. Man. Table 74
  I2C_InitStructure.I2C_Timing = 0x2033030A; // =400 kHz. @ 48 MHz (SYSCLK) measured with Logic Analyzer

  I2C_Init(I2C1, &I2C_InitStructure);
  
  //(#) Optionally you can enable/configure the following parameters without
  //    re-initialization (i.e there is no need to call again I2C_Init() function):
  //    (++) Enable the acknowledge feature using I2C_AcknowledgeConfig() function.
  //    (++) Enable the dual addressing mode using I2C_DualAddressCmd() function.
  //    (++) Enable the general call using the I2C_GeneralCallCmd() function.
  //    (++) Enable the clock stretching using I2C_StretchClockCmd() function.
  //    (++) Enable the PEC Calculation using I2C_CalculatePEC() function.
  //    (++) For SMBus Mode:
  //         (+++) Enable the SMBusAlert pin using I2C_SMBusAlertCmd() function.

  //(#) Enable the NVIC and the corresponding interrupt using the function
  //    I2C_ITConfig() if you need to use interrupt mode.
  
  //(#) When using the DMA mode 
  //   (++) Configure the DMA using DMA_Init() function.
  //   (++) Active the needed channel Request using I2C_DMACmd() function.
  
  //(#) Enable the I2C using the I2C_Cmd() function.
  I2C_Cmd(I2C1, ENABLE);

  //(#) Enable the DMA using the DMA_Cmd() function when using DMA mode in the 
  //    transfers.
}

/**
  * @brief  This function powers on and initializes the BTHQ21605V LCD module by
  *         writing data to 10 instruction registers. After executing this function:
  *         - Function_set: 2 lines x 16, display on, cursor off, no blink
  *         - Display_conf: right-to-left and top-to-bottom
  *         - Icon_ctl: character mode, full display icon blink dsabled
  *         - HV_gen: voltage multiplier 2
  *         - VLCD_set: VA is set to 0x28
  *         - Return home, set DDRAM address 0 in address counter
  * @param  None
  * @retval None
  */
void BTHQ21605V_PowerOn(void)
{
  // POR: Power On Reset
  GPIOB->BSRR = GPIO_BSRR_BS_8;
  Delay(SystemCoreClock/8/500); // 2ms
  GPIOB->BSRR = GPIO_BSRR_BR_8;
  Delay(SystemCoreClock/8/500); // 2ms

  // Wait until BTHQ21605V ready
  while(BTHQ21605V_ReadStatus(BTHQ21605V_STATUS_BF) != RESET){;}

  // Wait while I2C peripheral is not ready
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_BUSY);

  // Start I2C write transfer for 10 bytes
  I2C_TransferHandling(I2C1, BTHQ21605V_ADDR, 10, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);
    
  // 1. Write control byte: select instruction register
  I2C_SendData(I2C1, BTHQ21605V_CONTROL_BYTE);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);

  // 2. Send instruction: 2 lines x 16, 1/18 duty, extended instruction set
  I2C_SendData(I2C1, BTHQ21605V_FUNCTION_SET |
                     BTHQ21605V_FUNCTION_SET_M |
                     BTHQ21605V_FUNCTION_SET_H);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);
    
  // 3. Send instruction: Set display configuration to right to left & top to bottom
  I2C_SendData(I2C1, BTHQ21605V_DISP_CONF |
                     BTHQ21605V_DISP_CONF_P |
                     BTHQ21605V_DISP_CONF_Q);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);
    
  // 4. Send instruction: Set to character mode, full display, icon blink disabled
  I2C_SendData(I2C1, BTHQ21605V_ICON_CTL);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);
  
  // 5. Send instruction: Set voltage multiplier to 2
  I2C_SendData(I2C1, BTHQ21605V_HV_GEN);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);

  // 6. Send instruction: Set Vlcd and store in register VA
  I2C_SendData(I2C1, BTHQ21605V_VLCD_SET | 0x28);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);

  // 7. Send instruction: Change from extended instruction set to basic instruction set
  I2C_SendData(I2C1, BTHQ21605V_FUNCTION_SET |
                     BTHQ21605V_FUNCTION_SET_M);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);

  // 8. Send instruction: Display control: set display on, cursor off, no blink
  I2C_SendData(I2C1, BTHQ21605V_DISPLAY_CTL |
                     BTHQ21605V_DISPLAY_CTL_D);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);
    
  // 9. Send instruction: Entry mode set: increase DDRAM after access, no shift
  I2C_SendData(I2C1, BTHQ21605V_ENTRY_MODE_SET |
                     BTHQ21605V_ENTRY_MODE_SET_I_D);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);
    
  // 10. Send instruction: Return home, set DDRAM address 0 in address counter
  I2C_SendData(I2C1, BTHQ21605V_RETURN_HOME);

  BTHQ21605V_WaitForI2CFlag(I2C_ISR_STOPF);
  I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
}

/**
  * @brief  This function clears the entire DDRAM. There is a instruction available in the
  *         LCD controller, but this controller uses Character Set 'R'. This means that the
  *         'empty' character (space) is not empty.
  *         This problem is solved by:
  *         - setting the cursor home
  *         - transfering 'empty' characters (0xA0) to the entire DDRAM (80 addresse)
  *         - setting the cursor home
  * @param  None
  * @retval None
  */
void BTHQ21605V_Clear(void)
{
  uint32_t n = 80;
  
  // Set the cursor home
  BTHQ21605V_Home();
  
  // Wait until BTHQ21605V ready
  while(BTHQ21605V_ReadStatus(BTHQ21605V_STATUS_BF) != RESET){;}

  // Wait while I2C peripheral is not ready
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_BUSY);

  // Start I2C write transfer for 80 + 1 bytes
  I2C_TransferHandling(I2C1, BTHQ21605V_ADDR, n+1, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);
    
  // 1. Write control byte: select data register
  I2C_SendData(I2C1, BTHQ21605V_CONTROL_BYTE | 
                     BTHQ21605V_CONTROL_BYTE_RS);
  
  // 2 ... 80. Write 'empty' characters
  while(n-- > 0)
  {
    BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);
    I2C_SendData(I2C1, BTHQ21605V_EMPTY_CHAR);
  }
  
  // Wait for- and clear stop condition
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_STOPF);
  I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);  

  BTHQ21605V_Home();
}

/**
  * @brief  This function sets the cursor to it's home position
  *         x=0, y=0. It also switches a shifted display back to an unshifted
  *         state.
  * @param  None
  * @retval None
  */
void BTHQ21605V_Home(void)
{
  BTHQ21605V_WriteInstruction(BTHQ21605V_RETURN_HOME);
}

/**
  * @brief  This function sets the cursor to position (x,y):
  *
  *         +----+----+---- ----+----+----+
  *         | 0,0| 1,0|         |38,0|39,0|
  *         +----+----+---- ----+----+----+
  *         | 0,1| 1,1|         |38,1|39,1|
  *         +----+----+---- ----+----+----+
  * 
  * @param  x: horizontal position (0-39)
  *         y: vertical position (0-1)
  * @retval None
  */
void BTHQ21605V_GotoXY(uint8_t x, uint8_t y)
{
  BTHQ21605V_WriteInstruction(BTHQ21605V_SET_DDRAM + (y * 0x40) + x);
}

/**
  * @brief  This function writes an instruction to the BTHQ21605V. First 
  *         the instruction register is selected and then the instruction is
  *         written.
  * @param  ins: an instruction that the LCD controller can interpret. For an
  *              overview of all instructions see the header file and/or the
  *              LCD controller datasheet PCF2119x:
  *              http://www.nxp.com/documents/data_sheet/PCF2119X.pdf
  * @retval None
  */
void BTHQ21605V_WriteInstruction(uint8_t ins)
{
  // Wait until BTHQ21605V ready
  while(BTHQ21605V_ReadStatus(BTHQ21605V_STATUS_BF) != RESET){;}

  // Wait while I2C peripheral is not ready
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_BUSY);

  // Start I2C write transfer for 2 bytes
  I2C_TransferHandling(I2C1, BTHQ21605V_ADDR, 2, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);

  // 1. Write control byte: select instruction register
  I2C_SendData(I2C1, BTHQ21605V_CONTROL_BYTE);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);

  // 2. Write instruction
  I2C_SendData(I2C1, ins);
    
  // Wait for- and clear stop condition
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_STOPF);
  I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
}

/**
  * @brief  This function writes 8-bit data to the BTHQ21605V. Data will be
  *         visible at the current cursur position.
  * @param  d: 8-bit data to be written. Verify the characterset 'R' how this
  *            8-bit data translates to display characters:
  *            http://docs-europe.electrocomponents.com/webdocs/103d/0900766b8103daf7.pdf
  * @retval None
  */
void BTHQ21605V_WriteData(uint8_t d)
{
  // Wait until BTHQ21605V ready
  while(BTHQ21605V_ReadStatus(BTHQ21605V_STATUS_BF) != RESET){;}

  // Wait while I2C peripheral is not ready
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_BUSY);

  // Start I2C write transfer for 2 bytes
  I2C_TransferHandling(I2C1, BTHQ21605V_ADDR, 2, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);

  // 1. Write control byte: select data register
  I2C_SendData(I2C1, BTHQ21605V_CONTROL_BYTE | 
                     BTHQ21605V_CONTROL_BYTE_RS);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);

  // 2. Write data
  I2C_SendData(I2C1, d);
    
  // Wait for- and clear stop condition
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_STOPF);
  I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
}

/**
  * @brief  This function reads 8-bit data from the BTHQ21605V. Data will be
  *         read from the current cursur position.
  * @param  None
  * @retval 8-bit data read from the current cursor position. Verify the
  *         characterset 'R' how this 8-bit data translates to display
  *         characters:
  *         http://docs-europe.electrocomponents.com/webdocs/103d/0900766b8103daf7.pdf
  */
uint8_t BTHQ21605V_ReadData(void)
{
  uint8_t tmp = 0xFF;
  
  // Wait until BTHQ21605V ready
  while(BTHQ21605V_ReadStatus(BTHQ21605V_STATUS_BF) != RESET){;}

  // Wait while I2C peripheral is not ready
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_BUSY);

  // Start I2C write transfer for 1 byte, do not end transfer (SoftEnd_Mode)
  I2C_TransferHandling(I2C1, BTHQ21605V_ADDR, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);

  // 1. Write control byte: select data register
  I2C_SendData(I2C1, BTHQ21605V_CONTROL_BYTE |
                     BTHQ21605V_CONTROL_BYTE_RS);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TC);

  // Repeated start I2C read transfer for 1 byte
  I2C_TransferHandling(I2C1, BTHQ21605V_ADDR, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_RXNE);
  
  // 1. Read data
  tmp = I2C_ReceiveData(I2C1);
    
  // Wait for- and clear stop condition
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_STOPF);
  I2C1->ICR = I2C_ICR_STOPCF;
  
  return(tmp);
}

/**
  * @brief  This function reads 8-bit status from the BTHQ21605V.
  * @param  bf_ac: BTHQ21605V_STATUS_BF   : function returns only the Busy Flag,
  *                                         all AC bits are cleared  
  *                BTHQ21605V_STATUS_AC   : function returns only the Address
  *                                         Counter, the BF bit is cleared
  *                BTHQ21605V_STATUS_BF_AC: function returns both the Busy Flag
  *                                         and the Address Counter
  * @retval 8-bit data read from the BF_AC register
  */
uint8_t BTHQ21605V_ReadStatus(uint8_t bf_ac)
{
  uint8_t status = RESET;

  // Wait while I2C peripheral is not ready
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_BUSY);

  // Start I2C write transfer for 1 byte, do not end transfer (SoftEnd_Mode)
  I2C_TransferHandling(I2C1, BTHQ21605V_ADDR, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);

  // 1. Write control byte: select instruction register
  I2C_SendData(I2C1, BTHQ21605V_CONTROL_BYTE);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_TC);

  // Repeated start I2C read transfer for 1 byte
  I2C_TransferHandling(I2C1, BTHQ21605V_ADDR, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_RXNE);
  
  // 1. Read status
  status = I2C_ReceiveData(I2C1);
    
  // Wait for- and clear stop condition
  BTHQ21605V_WaitForI2CFlag(I2C_ISR_STOPF);
  I2C1->ICR = I2C_ICR_STOPCF;
  
  // Return requested result
  if(bf_ac == BTHQ21605V_STATUS_BF)
  {
    return(status & 0x80);
  }
  else if(bf_ac == BTHQ21605V_STATUS_AC)
  {
    return(status & 0x7F);
  }
  else // BTHQ21605V_STATUS_BF_AC
  {
    return(status); 
  }
}

/**
  * @brief  This function writes an ASCII character to the display at the 
  *         current cursor position. The BTHQ21605V uses character set 'R',
  *         which means standard charaters must have the MSB set to logic 1
  *         instead of logic 0.
  *         This function therefore sets the MSB to logic 1 and writes it to
  *         the BTHQ21605V.
  *         Alternate characters should be displayed with the function
  *         BTHQ21605V_WriteData().
  * @param  c: character to be displayed
  * @retval None
  */
void BTHQ21605V_Putc(uint8_t c)
{
  // Write character with MSB set
  BTHQ21605V_WriteData(c | 0x80); 
}

/**
  * @brief  This function writes a string of characters to the BTHQ21605V.
  *         It first determines the length of the string and then transmits
  *         all the characters. Characters ar send within one I2C START-STOP
  *         frame.
  * @param  str: pointer to NULL termineted string to be displayed
  * @retval None
  */
void BTHQ21605V_Puts(uint8_t *str)
{
  uint32_t i=0;
  uint8_t *p = str;
  
  // Determine string length
  while(*p++ != '\0'){ i++; }

  // I2C communication only if string has 1 or more characters
  if(i != 0)    
  {
    // Wait until BTHQ21605V ready
    while(BTHQ21605V_ReadStatus(BTHQ21605V_STATUS_BF) != RESET){;}

    // Wait while I2C peripheral is not ready
    BTHQ21605V_WaitForI2CFlag(I2C_ISR_BUSY);

    // Start I2C write transfer for string len + 1 bytes
    I2C_TransferHandling(I2C1, BTHQ21605V_ADDR, i+1, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
    BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);
      
    // 1. Write control byte: select data register
    I2C_SendData(I2C1, BTHQ21605V_CONTROL_BYTE | 
                       BTHQ21605V_CONTROL_BYTE_RS);
    
    // 2 ... (string length + 1). Write characters
    while(i-- > 0)
    {
      BTHQ21605V_WaitForI2CFlag(I2C_ISR_TXIS);
      I2C_SendData(I2C1, (*str++) | 0x80);
    }
    
    // Wait for- and clear stop condition
    BTHQ21605V_WaitForI2CFlag(I2C_ISR_STOPF);
    I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);      
  }
}

/**
  * @brief  This function reads an ASCII character from the display at the 
  *         current cursor position. The BTHQ21605V uses character set 'R',
  *         which means the standard charaters will have the MSB set to logic 1
  *         instead of logic 0.
  *         This function therefore resets the MSB to logic 0 and returns the
  *         result.
  * @param  None
  * @retval Character at current cursor position
  */
uint8_t BTHQ21605V_Getc(void)
{
  // Read character and return with MSB reset
  return(BTHQ21605V_ReadData() & 0x7F);
}

/**
  * @brief  This function checks a flag in the I2C1 Interrupt and Status 
  *         register. The 'timeout' value indicates how many times the
  *         flag should be checked. Depending on the clock frequency of the
  *         core this takes a certain time.
  * @param  flag: the flag to check, refer to documentation of
  *         I2C_GetFlagStatus() for valid parameter values
  * @retval none
  */
void BTHQ21605V_WaitForI2CFlag(uint32_t flag){
  uint32_t timeout = BTHQ21605V_TIMEOUT;  
  if(flag == I2C_ISR_BUSY){
    while(I2C_GetFlagStatus(I2C1, flag) != RESET){
      if(timeout-- == 0){
        BTHQ21605V_CommStatus = BTHQ21605V_COMM_ERROR;
        return;
      }
    }
  } else{
    while(I2C_GetFlagStatus(I2C1, flag) == RESET){
      if(timeout-- == 0){
        BTHQ21605V_CommStatus = BTHQ21605V_COMM_ERROR;
        return;
      }
    }
  }
}

/**
  * @brief  This function checks the currentmode and displays the wanted information 
  * @param  No parameters
  * @retval none
  */
void SetDisplay(){
	char CharBuff[10];
	char CharBuff2[10];
	BTHQ21605V_Clear();
	switch(CurrentMode){
		case 0:
			itoa_simple(CharBuff, TempDesired);
			itoa_simple(CharBuff2, TempAcc);
			BTHQ21605V_GotoXY(0,0);
			BTHQ21605V_Puts((uint8_t *)("Want Temp: "));
			BTHQ21605V_GotoXY(12,0);
			BTHQ21605V_Puts((uint8_t *)(CharBuff));
			BTHQ21605V_GotoXY(0,1);
			BTHQ21605V_Puts((uint8_t *)("Cur Temp: ")); 
			BTHQ21605V_GotoXY(12,1);
			BTHQ21605V_Puts((uint8_t *)(CharBuff2));
			break;
		case 1:
			itoa_simple(CharBuff, HumDesired);
			itoa_simple(CharBuff2, HumAcc);
			BTHQ21605V_GotoXY(0,0);
			BTHQ21605V_Puts((uint8_t *)("Want Hum: "));
			BTHQ21605V_GotoXY(12,0);
			BTHQ21605V_Puts((uint8_t *)(CharBuff));
			BTHQ21605V_GotoXY(0,1);
			BTHQ21605V_Puts((uint8_t *)("Cur Hum: ")); 
			BTHQ21605V_GotoXY(12,1);
			BTHQ21605V_Puts((uint8_t *)(CharBuff2));		
			break;
		case 2:
			itoa_simple(CharBuff, TempBabyAcc);
			BTHQ21605V_GotoXY(0,0);
			BTHQ21605V_Puts((uint8_t *)("Baby Temp: "));
			BTHQ21605V_GotoXY(12,0);
			BTHQ21605V_Puts((uint8_t *)(CharBuff));		
			break;
		default:
			BTHQ21605V_GotoXY(0,0);
			BTHQ21605V_Puts((uint8_t *)("Mode Error: "));
			break;
	}
}

/**
  * @brief  This function checks if a error has accured and resets based on the error
  * @param  No parameters
  * @retval none
  */
void CheckError(){
		// In case of error, set LED3 on
	if(BTHQ21605V_CommStatus != BTHQ21605V_COMM_OK){
		STM_EVAL_LEDOn(LED3);
		BTHQ21605V_Setup();
		BTHQ21605V_PowerOn();
		SetDisplay();
		BTHQ21605V_CommStatus = BTHQ21605V_COMM_OK;
	}else{
		STM_EVAL_LEDOff(LED3);
	}
}
