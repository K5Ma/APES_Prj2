/*
 * LoadCellTask.c
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
/*
 *
 * Hardware Connections
 *
 * Note: This module is not using any standard interface (e.g. TWI/I2C)
 *       Moreover, this driver aims to solve a specific problem - and not
 *       calculate acutal force. It can be done with minor modifications though.
 *
 * Note: Left Side - Module Pins & Right Side - Controller Pins
 *
 * VDD - 3.3V (This is the voltage level of interface)
 * VCC - 5V (This is the voltage which will be provided to the LoadCell as excitation)
 * DAT - PM5 (This is the data pin)
 * CLK - PM4 (This is the clock pin)
 * GND - Ground
 *
 */

#include "LoadCell_Task.h"

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

//FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"

/* Global Variables */
extern QueueHandle_t xQueue_LCStruct;
LC_T2B_Struct LC_Tx;				//Struct to be sent to BB
// Variables that will be shared between functions
uint8_t LC_us = 0;
uint8_t LC_Retries;
bool LC_Error;
uint16_t LC_counter = 0;






uint8_t LoadCell_TaskInit()
{
    //Create task, if it fails return 1, else 0
    if( xTaskCreate(LoadCell_Task,					/* The function that implements the task */
			   (const portCHAR *)"LoadCell",		/* The text name assigned to the task - for debug only as it is not used by the kernel */
               ((configMINIMAL_STACK_SIZE) * 10),	/* The size of the stack to allocate to the task */
			   NULL,								/* The parameter passed to the task */
			   PRIORITY_LOADCELL_TASK,	 			/* The priority assigned to the task */
			   NULL)								/* The task handle is not required, so NULL is passed */
    			!= pdTRUE
      )
      {
    	return 1;
      }

	return 0;
}



void LoadCell_Task(void *pvParameters)
{
	/* Delay a bit to make sure BBComm Task starts-up first */
	const TickType_t xDelay = 10 / portTICK_PERIOD_MS;
	vTaskDelay(xDelay);


	/* Create a queue capable of containing 6 structs */
	xQueue_LCStruct = xQueueCreate(5,  sizeof(LC_B2T_Struct));

	if( xQueue_LCStruct == NULL )
	{
		/* Queue was not created and must not be used. */
		Log_Msg(T_LoadCell, "FATAL", "Could not create xQueue_LCStruct!", LOCAL_ONLY);
	}
	else
	{
		Log_Msg(T_LoadCell, "INFO", "Created xQueue_LCStruct successfully!", LOCAL_ONLY);
	}


	LC_B2T_Struct LC_Rx;

    uint8_t storedsamplecount, i;

    LC_Error = false;

    LC_Rx.LC_Poll = false;
    LC_Rx.LC_Poll = true;

    for(i = 0; i < LC_MaxSamples; i ++)     LC_Tx.LC_SamplesArraymv[i] = 0;

    char tp[50];
    uint16_t millivolts;

    LC_TimerInit();
    LC_DriverInit();
    LC_TestSensor();

    if(LC_Error == false)
    {
        //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send LC BIST Success LOG to BB
        #if     LC_DEBUG_PRINTF
            Log_Msg(T_LoadCell, "INFO", "LoadCell Initialized", LOCAL_ONLY); //LOGGER_AND_LOCAL
            Log_Msg(T_LoadCell, "INFO", "Starting LC Normal Operation", LOGGER_AND_LOCAL); //LOGGER_AND_LOCAL
        #endif
    }
    else
    {
        //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send LC BIST Failure LOG to BB
        #if     LC_DEBUG_PRINTF
            Log_Msg(T_LoadCell, "CRITICAL", "LoadCell Initialization Failed", LOGGER_AND_LOCAL); //LOGGER_AND_LOCAL
        #endif
    }


    /*
     * Normal Operation (Parameters and Returns indicate communication with Central Task)
     *
     * This Task should get 1 parameter from central task through IPC
     *
     * This Task will have 1 array of LC_MaxSamples length - all in uint16_t
     * that should be reported back to the central task through IPC
     * It will also report back the current state of the LoadCell
     *
     * Param_1: bool LC_Rx.LC_Poll
     *          (false: don't do anything, true: start polling Load Cell)
     *
     * Return_1: uint16_t LC_Tx.LC_SamplesArraymv[LC_MaxSamples]
     *          (this will have samples recorded on specified frequency, which are to be
     *           processed by Control Node and take a decision. Therefore, the entire array
     *           must be transferred to Central Task through proper IPC method. In case of
     *           the sensor failure, the array will be filled with zeroes.)
     *
     * Return_2: bool LC_Error
     *           (false: Online, true: OFfline - error present)
     *
     */

	while(1)
	{
		// Nothing important to do. Just wait
		        vTaskDelay(LC_Polling_Timems);
		        LC_counter += 1;

		        // If the Load Cell module seems to be offline, try to get it back online
		        // by reinitializing the interface, and waiting for a valid sample with timeout
		        // This feature supports both - limited and infinite retry modes
		        if(LC_Error == true)
		        {
		            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send LC Failure LOG to BB
		            #if     (LC_Retry_Mode == LC_Limited)
		                if(LC_Retries != 0x00)
		                {
		                    LC_Retries -= 1;
		            #endif
//		                    #if     LC_DEBUG_PRINTF
//		                            LC_Print("\nRetrying Connection with LoadCell");
//		                    #endif

		                    LC_TimerInit();
		                    LC_DriverInit();
		                    LC_TestSensor();

		                    #if     LC_DEBUG_PRINTF
		                            if(LC_Error == false)  Log_Msg(T_LoadCell, "INFO", "LoadCell is Back Online", LOGGER_AND_LOCAL); //LOGGER_AND_LOCAL
		                            else
									{
//                                        Log_Msg(T_LoadCell, "ERROR", "Retry Failed", LOCAL_ONLY);
									}
		                    #endif
		            #if     (LC_Retry_Mode == LC_Limited)
		                }
		            #endif
		        }
		        else
		        {
		            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send LC Online LOG to BB
		            // Check the status of Load Cell on regular intervals
		            LC_TestSensor();
		            #if     LC_DEBUG_PRINTF
//		                    LC_Print("\nChecking LoadCell Status...");
		                    if(LC_Error == false)
							{
                            	if(LC_counter >= 50)
                            	{
                            		LC_counter = 0;
                            		Log_Msg(T_LoadCell, "INFO", "LoadCell is Online", LOCAL_ONLY);
                            	}
							}
		                    else     Log_Msg(T_LoadCell, "CRITICAL", "LoadCell Went Offline", LOGGER_AND_LOCAL); //LOGGER_AND_LOCAL
		            #endif
		        }

		if( xQueue_LCStruct != 0 )
		{
			/* Block for 10 ticks if a message (struct buffer) is not immediately available */
			if( xQueueReceive( xQueue_LCStruct, &LC_Rx, ( TickType_t ) 100 ) )
			{
		//		Log_Msg(T_LoadCell, "INFO", "LC Got struct from BB!", LOCAL_ONLY);

//				if(LC_Rx.LC_Poll == true)		Log_Msg(T_LoadCell, "INFO", "LC Poll is Set", LOCAL_ONLY);
//				else		Log_Msg(T_LoadCell, "INFO", "LC Poll is Not Set", LOCAL_ONLY);

		        if((LC_Rx.LC_Poll == true) && (LC_Error == false))
		        {

		        	Log_Msg(T_LoadCell, "INFO", "LoadCell Polling Will Start Soon", LOCAL_ONLY);
		        	vTaskDelay(5000);
		        	Log_Msg(T_LoadCell, "INFO", "LoadCell Polling Has Been Started", LOCAL_ONLY);

		            for(i = 0; i < LC_MaxSamples; i ++)     LC_Tx.LC_SamplesArraymv[i] = 0;

		            storedsamplecount = 0;

		            // Taking samples till the entire array is filled up
		            while(storedsamplecount < LC_MaxSamples)
		            {

		                // Clearing the clock because it should be idle when not reading
		                // anything from the module
		                LC_ClearClk();

		                // Testing sensor/waiting for the module to have a valid sample ready
		                LC_TestSensor();

//		                Log_Msg(T_LoadCell, "INFO", "LC Poll is Set", LOCAL_ONLY);

		                // Proceed only if there was no timeout, otherwise - write a 0 in the array
		                if(LC_Error == false)
		                {

		                	taskENTER_CRITICAL();
		                    // Converting float voltages to integer millivolts (to save size)
		                    millivolts = (LC_ReadLoadCellVoltage() * 1000);
		                    taskEXIT_CRITICAL();

		                    // Filtering samples - so that the processing on the Control Node becomes easier
		                    if((millivolts > LC_FilterLowThresholdmv) && (millivolts < LC_FilterHighThresholdmv))   LC_Tx.LC_SamplesArraymv[storedsamplecount ++] = millivolts;
		                    else        LC_Tx.LC_SamplesArraymv[storedsamplecount ++] = 0;
		                    #if     LC_DEBUG_PRINTF
		                            snprintf(tp, sizeof(tp), "Millivolts: %d", millivolts);
		                            Log_Msg(T_LoadCell, "INFO", tp, LOCAL_ONLY);
		                    #endif
		                }

		                // There was a timeout, write a 0
		                else        LC_Tx.LC_SamplesArraymv[storedsamplecount ++] = 0;

		                // Waiting for period defined by polling frequency of the sensor
		                vTaskDelay((1000 / LC_PollingFrequencyHz));
		            }

		            // Resetting polling flag
		            LC_Rx.LC_Poll = false;

//		            #if     LC_DEBUG_PRINTF
//		                for(i = 0; i < LC_MaxSamples; i ++)
//		                {
//		                    snprintf(tp, sizeof(tp), "\nSample[%d]: %d", i, LC_Tx.LC_SamplesArraymv[i]);
//		                    LC_Print(tp);
//		                }
//		            #endif

		            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Transmit LC_Tx structure to BBComm Task

		            /* Send struct to BeagleBone */
		            Send_LC_T2B_Struct_ToBB(LC_Tx);
		        }

			}
		}
	}
}




/*
 * Function to handle timer0 interrupts
 * (triggering at every 1 microsecond, when timer is running)
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void LC_Timer0ISR(void)
{
//	UBaseType_t uxSavedInterruptStatus;
//
//	uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
//    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT | TIMER_TIMB_TIMEOUT);
//    taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
//

	/* Get interrupts */
	uint32_t Flags =  TimerIntStatus(TIMER1_BASE, true);

	/* Clear the asserted interrupts */
	TimerIntClear(TIMER1_BASE, Flags);
//	TimerDisable(TIMER1_BASE, TIMER_A);
    LC_us = 1;
}

/*
 * Function to initialize timer0
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void LC_TimerInit(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER1_BASE, TIMER_A, (SYSTEM_CLOCK/1000000));
//    IntMasterEnable();
    TimerIntRegister(TIMER1_BASE, TIMER_A, LC_Timer0ISR);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER1A);
}

/*
 * Function to initialize GPIO pins
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void LC_DriverInit(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_4); //clk
    GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_5); //data
    GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_5, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

/*
 * Function to send the digital High portion of the clock pulse
 * It keeps it high for 1 microsecond
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void LC_SetClkHigh(void)
{
    LC_us = 0;
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, GPIO_PIN_4);
    TimerEnable(TIMER1_BASE, TIMER_A);
//    uint32_t i;
//    taskENTER_CRITICAL();
//    for(i = 0; i < LC_Cycle_Counter; i ++);
//    taskEXIT_CRITICAL();
    while(LC_us == 0);
    TimerDisable(TIMER1_BASE, TIMER_A);
}

/*
 * Function to send the digital Low portion of the clock pulse
 * It keeps it low for 1 microsecond
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void LC_SetClkLow(void)
{
    LC_us = 0;
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, false);
    TimerEnable(TIMER1_BASE, TIMER_A);
    //    uint32_t i;
    //    taskENTER_CRITICAL();
    //    for(i = 0; i < LC_Cycle_Counter; i ++);
    //    taskEXIT_CRITICAL();
        while(LC_us == 0);
        TimerDisable(TIMER1_BASE, TIMER_A);
}

/*
 * Function to assert the clock pin at digital Low level
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void LC_ClearClk(void)
{
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, false);
}

/*
 * Function to read the Data pin state at given time
 *
 * Param: Null
 *
 * Return: Boolean pin status
 *
 */
bool LC_ReadDataPinStatus(void)
{
    uint32_t stat;
    stat = 0;
    stat = GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_5) & 0x00FF;
    return (stat >> 5);
}

/*
 * Function to (1) Wait for a valid new sample to be available from the module
 * and (2) To check that the module is functioning properly. In case of the
 * detected failure, proper variables will be set to reflect this state
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void LC_TestSensor(void)
{
    static uint16_t noresptime;

    noresptime = 0;
    LC_Error = false;

    while(LC_ReadDataPinStatus() == true)
    {
        vTaskDelay(1);
        noresptime += 1;
        if(noresptime >= LC_ModuleNotRespondingTimeoutms)
        {
            LC_Error = true;
            #if     (LC_Retry_Mode == LC_Limited)
                LC_Retries = LC_Max_Retries;
            #endif
            break;
        }
    }
}

/*
 * Function to read data from the module, and convert it to float volts
 *
 * Param: Null
 *
 * Return: Float voltage which is converted from ADC reading value
 *
 */
float LC_ReadLoadCellVoltage(void)
{
    uint32_t data;
    int i;
    data = 0;


//    Log_Msg(T_LoadCell, "INFO", "Read Volt Start", LOCAL_ONLY);

    for(i = 23; i >= 0; i --)
    {
        LC_SetClkHigh();
        if(LC_ReadDataPinStatus())     data |= (1 << i);
        LC_SetClkLow();
    }
    LC_SetClkHigh();
    LC_SetClkLow();

//    Log_Msg(T_LoadCell, "INFO", "Read Volt End", LOCAL_ONLY);

    data &= 0x00FFFFFF;
    if(data == 0x800000)  return 0;
    else if(data == 0x7FFFFF)   return 3.3;
    else
    {
        data ^= 0x00FFFFFF;
        data += 1;
        return (((float)data / 16777216.0) * 3.3);
    }
}

