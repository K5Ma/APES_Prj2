/*
 * LoadCellTask.h
 *
 *  Created on: Apr 13, 2019
 *  Last Update: Apr 21, 2019
 *      Author: Poorn Mehta
 *
 *  Driver for reading LoadCell values connected to HX711 Amplifier which is connected to
 *  TM4C1294XL (TIVA Development Board) using 2 digital pins
 *
 *  This driver code is developed based on provided documentation from the manufacturer
 *
 */

#ifndef LOADCELLTASK_H_
#define LOADCELLTASK_H_

//#include "main.h"

#define LC_Infinite             true
#define LC_Limited              false

#define LC_Retry_Mode           LC_Infinite

#if     (LC_Retry_Mode == LC_Limited)
#define LC_Max_Retries          5
#endif

#define LC_Polling_Timems       2500

#define LC_Online_Test_Timems   (2 * LC_Polling_Timems)

#define LC_Host_Unknown           false

#if     LC_Host_Unknown
    #define LC_Print   UARTprintf
#else
    #define LC_Print   cust_print
#endif

#define LC_INDIVIDUAL_TESTING  true

#define LC_DEBUG_PRINTF        true

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

#define LC_ModuleNotRespondingTimeoutms       1000
#define LC_PollingFrequencyHz           4
#define LC_FilterLowThresholdmv         100
#define LC_FilterHighThresholdmv        3000
#define LC_IdleTimeoutms                3000
#define LC_MaxSamples                  20

#if     LC_INDIVIDUAL_TESTING

#define     LC_VerificationLowmv            200
#define     LC_VerificationHighmv           300
#define     LC_ConsecutiveVerificationNeeded      5

#endif

void LC_Timer0ISR(void);
void LC_TimerInit(void);
void LC_DriverInit(void);
void LC_SetClkHigh(void);
void LC_SetClkLow(void);
void LC_ClearClk(void);
bool LC_ReadDataPinStatus(void);
void LC_TestSensor(void);
float LC_ReadLoadCellVoltage(void);
void LoadCellTask(void *pvParameters);
//void adc_all(void *pvParameters);



#endif /* LOADCELLTASK_H_ */
