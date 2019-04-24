/*
 * EPaperTask.h
 *
 *  Created on: Apr 12, 2019
 *  Last Update: Apr 19, 2019
 *      Author: Poorn Mehta
 *
 *  Driver for displaying images residing in SD card on Waveshare 4.3Inch UART E-Paper Display
 *  using TM4C1294XL (TIVA Development Board)
 *
 *  This driver code is developed based on provided documentation from the manufacturer
 *
 */

#ifndef EPAPERTASK_H_
#define EPAPERTASK_H_

//#include "main.h"

#define EP_Infinite             true
#define EP_Limited              false

#define EP_Retry_Mode           EP_Infinite

#if     (EP_Retry_Mode == EP_Limited)
#define EP_Max_Retries          5
#endif

#define EP_Reset_Timems         3000

#define EP_Online_Test_Timems   (2 * EP_Reset_Timems)

#define EP_INDIVIDUAL_TESTING  true

#define EP_DEBUG_PRINTF        true

#define EP_Host_Unknown           false

#if     EP_Host_Unknown
    #define EP_Print   UARTprintf
#else
    #define EP_Print   cust_print
#endif

// Standard Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "drivers/pinout.h"
#include "utils/uartstdio.h"

// Third Party Includes
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_i2c.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "inc/hw_adc.h"

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/fpu.h"
#include "driverlib/adc.h"

#include "sensorlib/i2cm_drv.h"

// System clock rate, 120 MHz
#define SYSTEM_CLOCK    120000000U

#define Valid_ASCII_Lowest  0x20
#define Valid_ASCII_Highest 0x7E

// E-Paper Defines
#define EP_Set_SD_Storage           0x01
#define EP_Frame_Header             0xA5
#define EP_Frame_End_1              0xCC
#define EP_Frame_End_2              0x33
#define EP_Frame_End_3              0xC3
#define EP_Frame_End_4              0x3C
#define EP_Command_Max_Length       1023
#define EP_CMD_Handshake            0x00
#define EP_CMD_Storage_Get          0x06
#define EP_CMD_Storage_Set          0x07
#define EP_CMD_Stop                 0x08
#define EP_CMD_Refresh_Update       0x0A
#define EP_CMD_Set_Draw_Color       0x10
#define EP_CMD_Get_Draw_Color       0x11
#define EP_CMD_Fill_Circle          0x27
#define EP_CMD_Clear                0x2E
#define EP_CMD_Display_Image_Type   0x00
#define EP_Handshake_Type           0x01
#define EP_Storage_Get_Type         0x01
#define EP_Storage_Set_Type         0x02
#define EP_Stop_Type                0x01
#define EP_Refresh_Update_Type      0x01
#define EP_Set_Draw_Color_Type      0x03
#define EP_Get_Draw_Color_Type      0x01
#define EP_Fill_Circle              0x04
#define EP_Clear_Type               0x01
#define EP_CMD_Display_Image        0x70
#define EP_Type1_Length             0x0009
#define EP_Type2_Length             0x000A
#define EP_Type3_Length             0x000B
#define EP_Type4_Length             0x000F
#define EP_Display_Image_CMD_Length(EP_String_Length)       ((0x000D) + (EP_String_Length))
#define EP_Response_Start_Timeout_ms        2500            // Max allowed value for now is 2540
#define EP_Response_End_Timeout_ms          100

// EPaper Function Prototypes
uint8_t EP_Parity_Generator(uint16_t EP_Command_Length);
void EP_UART_Init(void);
void EP_UART_Tx(uint8_t tx_byte);
uint8_t EP_UART_Rx(void);
void EP_Get_Response(void);
void EP_Send_Command(uint8_t EP_Command_ID, uint8_t EP_Command_Type);
void EP_Set_Circle_CMD(uint16_t EP_Circle_Center_X_Pos, uint16_t EP_Circle_Center_Y_Pos, uint16_t EP_Circle_Radius);
void EP_Draw_Circle(uint16_t EP_Circle_Center_X_Pos, uint16_t EP_Circle_Center_Y_Pos, uint16_t EP_Circle_Radius);
void EP_Set_Image_CMD(char *EP_Image_Name, uint16_t EP_Image_X_Start_Pos, uint16_t EP_Image_Y_Start_Pos);
void EP_Display_Image(char *EP_Image_Name, uint16_t EP_Image_X_Start_Pos, uint16_t EP_Image_Y_Start_Pos);
void EPaperTask(void *pvParameters);

#endif /* EPAPERTASK_H_ */
