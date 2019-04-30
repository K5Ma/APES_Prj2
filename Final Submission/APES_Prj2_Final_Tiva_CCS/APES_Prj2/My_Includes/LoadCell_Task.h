/*
 * LoadCell_Task.h
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

#ifndef MY_INCLUDES_LOADCELL_TASK_H_
#define MY_INCLUDES_LOADCELL_TASK_H_


#define LC_Infinite             true
#define LC_Limited              false

#define LC_Retry_Mode           LC_Infinite

#if     (LC_Retry_Mode == LC_Limited)
#define LC_Max_Retries          5
#endif

#define LC_Cycle_Counter		4000

#define LC_Polling_Timems       100

#define LC_Host_Unknown           false

#if     LC_Host_Unknown
    #define LC_Print   UARTprintf
#else
    #define LC_Print   cust_print
#endif

#define LC_INDIVIDUAL_TESTING  false

#define LC_DEBUG_PRINTF        true

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


#include <stdint.h>
#include <stdbool.h>

/**************************************************************************************************************
 * USAGE: This function will initialize the LoadCell task
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: 0 => Init was successful
 * 			1 => Init failed
 **************************************************************************************************************/
uint8_t LoadCell_TaskInit();


/**************************************************************************************************************
 * USAGE: This function handles all what the LoadCell task will do.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void LoadCell_Task(void *pvParameters);



void LC_Timer0ISR(void);
void LC_TimerInit(void);
void LC_DriverInit(void);
void LC_SetClkHigh(void);
void LC_SetClkLow(void);
void LC_ClearClk(void);
bool LC_ReadDataPinStatus(void);
void LC_TestSensor(void);
float LC_ReadLoadCellVoltage(void);


#endif /* MY_INCLUDES_LOADCELL_TASK_H_ */
