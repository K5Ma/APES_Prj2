/*
 * KeyPadTask.h
 *
 *  Created on: Apr 22, 2019
 *  Last Update: Apr 23, 2019
 *      Author: Poorn Mehta
 *
 *  Driver for Interfacing with 4x4 Keypad which is connected to
 *  TM4C1294XL (TIVA Development Board) using GPIO pins
 *
 *  This driver code is developed based on provided documentation from the manufacturer
 *
 */

#ifndef KEYPADTASK_H_
#define KEYPADTASK_H_

#define KeyPad_Polling_Timems       100
#define KeyPad_Timeoutms            (400 * KeyPad_Polling_Timems)

#define KeyPad_Host_Unknown           false

#if     KeyPad_Host_Unknown
    #define KeyPad_Print   UARTprintf
#else
    #define KeyPad_Print   cust_print
#endif

#define KeyPad_INDIVIDUAL_TESTING  true

#define KeyPad_DEBUG_PRINTF        true

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
#include "inc/hw_pwm.h"

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
#include "driverlib/pwm.h"

#include "sensorlib/i2cm_drv.h"

// System clock rate, 120 MHz
#define SYSTEM_CLOCK    120000000U

#define KeyPad_Rows     4
#define KeyPad_Cols     4

#define KeyPad_Code_Size    6

void KeyPad_Init(void);
void KeyPad_SelectRow(uint8_t row);
void KeyPad_BlinkLED(void);
bool KeyPad_ScanCode(uint8_t row);
void KeyPadTask(void *pvParameters);

#endif /* KEYPADTASK_H_ */
