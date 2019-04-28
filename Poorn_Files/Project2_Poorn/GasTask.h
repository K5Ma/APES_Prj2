/*
 * GasTask.h
 *
 *  Created on: Apr 23, 2019
 *  Last Update: Apr 23, 2019
 *      Author: Poorn Mehta
 *
 *  Driver for Interfacing with Gas Sensor (MiCS-5524) which is
 *  connected to TM4C1294XL (TIVA Development Board) using ADC
 *
 *  This driver code is developed based on provided documentation from the manufacturer
 *
 */

#ifndef GASTASK_H_
#define GASTASK_H_

#define Gas_Infinite             true
#define Gas_Limited              false

#define Gas_Retry_Mode           Gas_Infinite

#if     (Gas_Retry_Mode == Gas_Limited)
    #define Gas_Max_Retries          5
#endif

#define Gas_Polling_Timems          500
#define Gas_Offline_Wait_Timems     2000

#define Gas_Host_Unknown           false

#if     Gas_Host_Unknown
    #define Gas_Print   UARTprintf
#else
    #define Gas_Print   cust_print
#endif

#define Gas_INDIVIDUAL_TESTING  true

#define Gas_DEBUG_PRINTF        true


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

#define Gas_Offline_Maximum_Level       100
#define Gas_Online_Minimum_Level    150

void Gas_ADC_Init(void);
void Gas_Read(void);
void GasTask(void *pvParameters);

#endif /* GASTASK_H_ */
