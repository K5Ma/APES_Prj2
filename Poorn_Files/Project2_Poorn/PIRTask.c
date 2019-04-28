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

extern bool Servo_Open;

bool PIR_Alert;

// Variables that will be shared between functions
bool PIR_State;

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
 * Param_1: bool Servo_Open
 *          (false: keep polling, true: stop the sensor for 60 seconds)
 *
 * Return_1: bool PIR_State
 *          (false: Door is Closed, true: Door is opened)
 *
 */
void PIRTask(void *pvParameters)
{
    PIR_Init();

    PIR_Alert = false;

    while(1)
    {
        if(Servo_Open == false)
        {
            PIR_Read();
            if(PIR_State == true)
            {
                PIR_Alert = true;
                //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send Security Breach Alert LOG to BB
            }
            else
            {
                PIR_Alert = false;
                //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send Door Secured LOG to BB
            }
        }
         vTaskDelay(PIR_Polling_Timems);
    }
}


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
