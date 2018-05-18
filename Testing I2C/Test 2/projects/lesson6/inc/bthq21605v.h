/******************************************************************************
 * Project        : HAN ESE PRJ2, PRJ1V & PRJ1D
 * File           : BTHQ21605V implementation
 * Copyright      : 2013 HAN Embedded Systems Engineering
 ******************************************************************************
  Change History:

    Version 1.0 - May 2013
    > Initial revision

******************************************************************************
   Hardware:
                        GND
                         |   BTHQ21605V-FSRE-I2C-COG
                 Vdd 1uF =  +-------------+
                  |  CAP |  |             |
                  |      +--+1 VLCD       |
   ------------+  | GND|----+2 VSS        |
               |  +---------+3 VDD        |
   I2C1_SDA/PB7+------------+4 SDA        |
            PB8+------------+5 POR        |
   I2C1_SCL/PB6+------------+6 SCL        |
               |            |             |
   ------------+            +-------------+

 http://docs-europe.electrocomponents.com/webdocs/103d/0900766b8103daf7.pdf
 http://www.nxp.com/documents/data_sheet/PCF2119X.pdf

******************************************************************************/
#ifndef _BTHQ21605V_H_
#define _BTHQ21605V_H_

#include "stm32f0xx.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#define BTHQ21605V_ADDR               ((uint16_t)(0x76))

// Instruction Register overview
#define BTHQ21605V_CONTROL_BYTE       ((uint8_t)(0x00))
#define BTHQ21605V_CONTROL_BYTE_CO    ((uint8_t)(0x80))
#define BTHQ21605V_CONTROL_BYTE_RS    ((uint8_t)(0x40))
   
#define BTHQ21605V_FUNCTION_SET       ((uint8_t)(0x20))
#define BTHQ21605V_FUNCTION_SET_DL    ((uint8_t)(0x10))
#define BTHQ21605V_FUNCTION_SET_M     ((uint8_t)(0x04))
#define BTHQ21605V_FUNCTION_SET_SL    ((uint8_t)(0x02))
#define BTHQ21605V_FUNCTION_SET_H     ((uint8_t)(0x01))

#define BTHQ21605V_CLEAR_DISPLAY      ((uint8_t)(0x01))

#define BTHQ21605V_RETURN_HOME        ((uint8_t)(0x02))

#define BTHQ21605V_ENTRY_MODE_SET     ((uint8_t)(0x04))
#define BTHQ21605V_ENTRY_MODE_SET_I_D ((uint8_t)(0x02))
#define BTHQ21605V_ENTRY_MODE_SET_S   ((uint8_t)(0x01))

#define BTHQ21605V_DISPLAY_CTL        ((uint8_t)(0x08))
#define BTHQ21605V_DISPLAY_CTL_D      ((uint8_t)(0x04))
#define BTHQ21605V_DISPLAY_CTL_C      ((uint8_t)(0x02))
#define BTHQ21605V_DISPLAY_CTL_B      ((uint8_t)(0x01))

#define BTHQ21605V_CURS_DISP_SHIFT    ((uint8_t)(0x10))
#define BTHQ21605V_CURS_DISP_SHIFT_SC ((uint8_t)(0x08))
#define BTHQ21605V_CURS_DISP_SHIFT_RL ((uint8_t)(0x04))

#define BTHQ21605V_SET_CGRAM          ((uint8_t)(0x40))

#define BTHQ21605V_SET_DDRAM          ((uint8_t)(0x80))

#define BTHQ21605V_SCREEN_CONF        ((uint8_t)(0x02))
#define BTHQ21605V_SCREEN_CONF_L      ((uint8_t)(0x01))

#define BTHQ21605V_DISP_CONF          ((uint8_t)(0x04))
#define BTHQ21605V_DISP_CONF_P        ((uint8_t)(0x02))
#define BTHQ21605V_DISP_CONF_Q        ((uint8_t)(0x01))

#define BTHQ21605V_ICON_CTL           ((uint8_t)(0x08))
#define BTHQ21605V_ICON_CTL_IM        ((uint8_t)(0x04))
#define BTHQ21605V_ICON_CTL_IB        ((uint8_t)(0x02))
#define BTHQ21605V_ICON_CTL_DM        ((uint8_t)(0x01))

#define BTHQ21605V_TEMP_CTL           ((uint8_t)(0x10))
#define BTHQ21605V_TEMP_CTL_TC1       ((uint8_t)(0x02))
#define BTHQ21605V_TEMP_CTL_TC2       ((uint8_t)(0x01))

#define BTHQ21605V_HV_GEN             ((uint8_t)(0x40))
#define BTHQ21605V_HV_GEN_S1          ((uint8_t)(0x02))
#define BTHQ21605V_HV_GEN_S0          ((uint8_t)(0x01))

#define BTHQ21605V_VLCD_SET           ((uint8_t)(0x80))
#define BTHQ21605V_VLCD_SET_V         ((uint8_t)(0x40))

// Display status bits
#define BTHQ21605V_STATUS_BF          (0x0)
#define BTHQ21605V_STATUS_AC          (0x1)
#define BTHQ21605V_STATUS_BF_AC       (0x2)

// Display character defines
#define BTHQ21605V_EMPTY_CHAR         ((uint8_t)(0xA0))

// Maximum timeout value for waiting loops
// This timeout is not based on an accurate timing value. It just guarantess
// that the program will not get stuck in an endless loop whenever there is an
// error in the I2C communication
#define BTHQ21605V_TIMEOUT            ((uint32_t)(0x00001000))

// BTHQ21605V communication status information
#define BTHQ21605V_COMM_OK            (0x0)
#define BTHQ21605V_COMM_ERROR         (0x1)

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void BTHQ21605V_Setup(void);
void BTHQ21605V_PowerOn(void);
void BTHQ21605V_Clear(void);
void BTHQ21605V_Home(void);
void BTHQ21605V_GotoXY(uint8_t x, uint8_t y);
void BTHQ21605V_WriteInstruction(uint8_t ins);
void BTHQ21605V_WriteData(uint8_t d);
uint8_t BTHQ21605V_ReadData(void);
uint8_t BTHQ21605V_ReadStatus(uint8_t bf_ac);

// Convenience functions
void BTHQ21605V_Putc(uint8_t c);
void BTHQ21605V_Puts(uint8_t *str);
uint8_t BTHQ21605V_Getc(void);
void BTHQ21605V_WaitForI2CFlag(uint32_t flag);


#endif /* _BTHQ21605V_H_ */
