/*
 * GasTask.c
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

/*
 *
 * Hardware Connections
 *
 *
 * Note: Left Side - Module Pins & Right Side - Controller Pins
 *
 * 5V - 5V
 * GND - GND
 * AO - PE3 (ADC 0)(Through a Voltage divider - to scale the output - from 5V to 3.3V
 *           Currently, I am using 180K and 360K for this (360/540)*5 = 3.33)
 * EN - GND
 *
 */

// Threshold can be set 1000 or 2000
// Values lower than 100 can be used for error detection

#include "GasTask.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Share this following variable between tasks
bool Gas_Alert;

// Variables that will be shared between functions
bool Gas_Error;
uint16_t Gas_Level;
uint8_t Gas_Retries;

/*
 *
 * Callback Function for Gas Task
 *
 * Return: Null
 *
 */

/*
 * Normal Operation (Parameters and Returns indicate communication with Central Task)
 *
 * This Task doesn't require anything from Central Task through IPC
 * As this is a safety sensor - which should be always ON
 *
 * This Task will return 1 integer value containing Gas Level
 * It will also report back the current sensor state
 *
 * Param_1: Null
 *
 * Return_1: uint16_t Gas_Level
 *          (the current Gas level of the surrounding area)
 *
 * Return_2: bool Gas_Error
 *           (false: Online, true: OFfline - error present)
 *
 */
void GasTask(void *pvParameters)
{
    Gas_Alert = false;

    Gas_ADC_Init();
    Gas_Read();

    if(Gas_Error == false)
    {
        //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send Gas BIST Success LOG to BB
        #if     Gas_DEBUG_PRINTF
            Gas_Print("\nGas Sensor Setup Succeeded");
            Gas_Print("\nStarting Gas Normal Operation");
        #endif
    }
    else
    {
        //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send Gas BIST Failure LOG to BB
        #if     Gas_DEBUG_PRINTF
            Gas_Print("\nGas Sensor Setup Failed");
        #endif
    }

    while(1)
    {
        if(Gas_Error == false)
        {
            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send Gas Online LOG to BB
            #if     Gas_DEBUG_PRINTF
                Gas_Print("\nGas Sensor is Online");
            #endif
            Gas_Read();
            if(Gas_Error == false)
            {
                if(Gas_Level > Gas_High_Threshold_Level)
                {
                    Gas_Alert = true;
                    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send Gas Too High Alert (with value) LOG to BB
                }
                else
                {
                    Gas_Alert = false;
                    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send Gas Normal (with value) LOG to BB
                }
            }
        }

        if(Gas_Error == true)
        {
            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send Gas Failure LOG to BB
            #if     (Gas_Retry_Mode == Gas_Limited)
                if(Gas_Retries != 0)
                {
                    Gas_Retries -= 1;
            #endif
                    #if     Gas_DEBUG_PRINTF
                        Gas_Print("\nGas Sensor is Offline... Retrying...");
                    #endif
            #if     (Gas_Retry_Mode == Gas_Limited)
                }
            #endif
        }
        vTaskDelay(Gas_Polling_Timems);
    }
}

/*
 * Function to setup ADC0 peripheral for Gas Sensor
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void Gas_ADC_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_HALF, 1);
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCHardwareOversampleConfigure(ADC0_BASE, 16);
    ADCSoftwareOversampleConfigure(ADC0_BASE, 0, 8);
    ADCSoftwareOversampleStepConfigure(ADC0_BASE, 0, 0, (ADC_CTL_CH0 | ADC_CTL_END));
    ADCSequenceEnable(ADC0_BASE, 0);
}

/*
 * Function to read Analog Data from the Gas Sensor
 * This also checks whether the sensor is connected or not
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void Gas_Read(void)
{
    static uint32_t adc_data[32], Gas_Level;
    static uint8_t samples_count, i;
    static char tp[50];

    ADCProcessorTrigger(ADC0_BASE, 0);
    while(ADCBusy(ADC0_BASE));
    samples_count = ADCSequenceDataGet(ADC0_BASE, 0, &adc_data[0]);
    Gas_Level = 0;
    for(i = 0; i < samples_count; i++)
    {
        Gas_Level += adc_data[i];
    }
    Gas_Level >>= 3;

    if(Gas_Error == false)
    {
        if(Gas_Level < Gas_Offline_Maximum_Level)
        {
            Gas_Error = true;
            #if     (Gas_Retry_Mode == Gas_Limited)
                Gas_Retries = Gas_Max_Retries;
            #endif
            #if     Gas_DEBUG_PRINTF
                Gas_Print("\nGas Sensor Went Offline");
            #endif
        }
        else
        {
            #if     Gas_DEBUG_PRINTF
                snprintf(tp, sizeof(tp), "\nGas Level: %d", Gas_Level);
                Gas_Print(tp);
            #endif
        }
    }
    else
    {
        if(Gas_Level > Gas_Online_Minimum_Level)
        {
            Gas_Error = false;
            #if     Gas_DEBUG_PRINTF
                Gas_Print("\nGas Sensor is Back Online");
            #endif
        }
    }
}



