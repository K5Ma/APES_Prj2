/*
 * PIRTask.c
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

/*
 *
 * Hardware Connections
 *
 * Note: Left Side - Module Pins & Right Side - Controller Pins
 *
 * 5V - 5V
 * GND - GND
 * ALT - PH2 (Input, Through a pull up resistor connect to 3.3V)
 *
 */

#include "PIRTask.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Variables that will be shared between functions
bool PIR_State, PIR_Disable;

/*
 * Function to setup GPIO pin for PIR Sensor
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void PIR_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    GPIOPinTypeGPIOInput(GPIO_PORTH_BASE, GPIO_PIN_2);
}

/*
 * Function to read the state of PIR Sensor
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void PIR_Read(void)
{
    static uint32_t stat;

    stat = 0;
    stat = GPIOPinRead(GPIO_PORTH_BASE, GPIO_PIN_2) & 0x00FF;
    if(stat != 0)
    {
        PIR_State = false;
        #if     PIR_DEBUG_PRINTF
            PIR_Print("\nDoor Closed");
        #endif
    }
    else
    {
        PIR_State = true;
        #if     PIR_DEBUG_PRINTF
            PIR_Print("\nDoor Opened");
        #endif
    }
}

/*
 *
 * Callback Function for PIR Task
 *
 * Return: Null
 *
 */

/*
 * Normal Operation (Parameters and Returns indicate communication with Central Task)
 *
 * This Task requires 1 parameter from Central Task through IPC
 *
 * This Task will return 1 boolen value representing current state of the sensor
 *
 * Param_1: bool PIR_Disable
 *          (false: keep polling, true: stop the sensor for 60 seconds)
 *
 * Return_1: bool PIR_State
 *          (false: Door is Closed, true: Door is opened)
 *
 */
void PIRTask(void *pvParameters)
{
    PIR_Init();

    #if     PIR_DEBUG_PRINTF
        PIR_Print("\nPIR Init Done");
    #endif

    #if PIR_INDIVIDUAL_TESTING
        PIR_Disable = false;
    #endif

    while(1)
    {
        if(PIR_Disable == true)
        {
            vTaskDelay(PIR_Stay_Off_Timeoutms);
            PIR_Disable = false;
        }

        if(PIR_Disable == false)    PIR_Read();

         vTaskDelay(PIR_Polling_Timems);
    }
}
