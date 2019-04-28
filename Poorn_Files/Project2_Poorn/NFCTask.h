/*
 * NFCTask.h
 *
 *  Created on: Apr 17, 2019
 *  Last Update: Apr 22, 2019
 *      Author: Poorn Mehta
 *
 *  Driver for reading NFC Tag ID from NFC Module v1.0
 *  (manufactured by DFRobot) which is connected to
 *  TM4C1294XL (TIVA Development Board) using UART Bus
 *
 *  Note: This module can do a lot more than just reading
 *  prefixed tag ID (like reading additional personalized
 *  data, including passwords and many more things; also
 *  this module can write the Tags as well) but due to
 *  severe lack of time, I am forced to leave all that
 *  aside for now. Hopefuily, I will get back to this in
 *  the future, and write a complete library.
 *
 *  This driver code is developed based on provided documentation from the manufacturer
 *
 */

#ifndef NFCTASK_H_
#define NFCTASK_H_

//#include "main.h"

#define NFC_Infinite             true
#define NFC_Limited              false

#define NFC_Retry_Mode           NFC_Infinite

#if     (NFC_Retry_Mode == NFC_Limited)
    #define NFC_Max_Retries          5
#endif

#define NFC_Polling_Timems       1000
#define NFC_Wait_Timems          1000

#define NFC_Online_Test_Timems   1000

#define NFC_Host_Unknown           false

#if     NFC_Host_Unknown
    #define NFC_Print   UARTprintf
#else
    #define NFC_Print   cust_print
#endif

#define NFC_INDIVIDUAL_TESTING  false

#define NFC_DEBUG_PRINTF        true

// Standard Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

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
#include "inc/hw_ssi.h"

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
#include "driverlib/ssi.h"

#include "sensorlib/i2cm_drv.h"

#define NFC_Firmware_Version_Byte1_Val      0x01
#define NFC_Firmware_Version_Byte1_Pos      8

#define NFC_Firmware_Version_Byte2_Val      0x06
#define NFC_Firmware_Version_Byte2_Pos      9

#define NFC_Standard_ACK_Byte1              0x00
#define NFC_Standard_ACK_Byte2              0x00
#define NFC_Standard_ACK_Byte3              0xFF
#define NFC_Standard_ACK_Byte4              0x00
#define NFC_Standard_ACK_Byte5              0xFF
#define NFC_Standard_ACK_Byte6              0x00

#define NFC_Tag_ID_Length                   4
#define NFC_Tag_ID_Start_Byte_Pos           13

#define NFC_Standard_ACK_Size               6
#define NFC_WakeUp_ACK_Size                 15
#define NFC_Firmware_Version_ACK_Size       19
#define NFC_Tag_ID_Size                     25

void NFC_Sensor_Test(void);
void NFC_UART_Init(void);
uint8_t NFC_UART_Rx(void);
bool NFC_Get_Verify_Standard_ACK(void);
bool NFC_Verify_Firmware_Version(void);
bool NFC_Read_Tag_ID(void);
bool NFC_Module_Init(void);
void NFCTask(void *pvParameters);

#endif /* NFCTASK_H_ */
