/*
 * PIRTask.h
 *
 *  Created on: Apr 23, 2019
 *  Last Update: Apr 24, 2019
 *      Author: Poorn Mehta
 *
 *  Driver for Interfacing with PIR Sensor which is connected
 *  to TM4C1294XL (TIVA Development Board) using GPIO
 *
 *  This driver code is developed based on provided documentation from the manufacturer
 *
 */

#ifndef PIRTASK_H_
#define PIRTASK_H_

#define PIR_Polling_Timems          500

#define PIR_Host_Unknown           false

#if     PIR_Host_Unknown
    #define PIR_Print   UARTprintf
#else
    #define PIR_Print   cust_print
#endif

#define PIR_INDIVIDUAL_TESTING  true

#define PIR_DEBUG_PRINTF        true

#define PIR_Stay_Off_Timeoutms   60000

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

void PIR_Init(void);
void PIR_Read(void);
void PIRTask(void *pvParameters);

#endif /* PIRTASK_H_ */
