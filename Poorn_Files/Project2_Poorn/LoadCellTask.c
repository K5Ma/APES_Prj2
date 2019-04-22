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

#include "LoadCellTask.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Variables that will be shared between functions
uint8_t LC_us = 0;
uint8_t LC_Retries;
bool LC_Error;

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
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
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
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, (SYSTEM_CLOCK/1000000));
    IntMasterEnable();
    TimerIntRegister(TIMER0_BASE, TIMER_A, LC_Timer0ISR);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
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
    TimerEnable(TIMER0_BASE, TIMER_A);
    while(LC_us == 0);
    TimerDisable(TIMER0_BASE, TIMER_A);
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
    TimerEnable(TIMER0_BASE, TIMER_A);
    while(LC_us == 0);
    TimerDisable(TIMER0_BASE, TIMER_A);
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
    static uint32_t stat;
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
    static uint32_t data;
    static int i;
    data = 0;
    for(i = 23; i >= 0; i --)
    {
        LC_SetClkHigh();
        if(LC_ReadDataPinStatus())     data |= (1 << i);
        LC_SetClkLow();
    }
    LC_SetClkHigh();
    LC_SetClkLow();

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

/*
 *
 * Callback Function for Load Cell Task
 *
 * Return: Null
 *
 */
void LoadCellTask(void *pvParameters)
{
    LC_Error = false;

    static char tp[50];
    static uint16_t millivolts;

    LC_TimerInit();
    LC_DriverInit();

    #if     LC_DEBUG_PRINTF
                cust_print("\nLoadCell Init Completed");
    #endif

#if     LC_INDIVIDUAL_TESTING

        /*
         * This portion just tests the Load Cell individually
         * It doesn't rely on anything that is not covered
         * or provided by its own header and source files
         *
         * This mode is only for raw testing, and may not
         * include any of the error checking feature(s)
         */

    static uint8_t valid;

    while(1)
    {
        LC_ClearClk();

        while(LC_ReadDataPinStatus() == true)   vTaskDelay(1);

        millivolts = (LC_ReadLoadCellVoltage() * 1000);

        if((millivolts > LC_FilterLowThresholdmv) && (millivolts < LC_FilterHighThresholdmv))
        {
            if((millivolts >= LC_VerificationLowmv) && (millivolts <= LC_VerificationHighmv))    valid += 1;
            else    valid = 0;

            if(valid >= LC_ConsecutiveVerificationNeeded)
            {
                cust_print("\n>>>>>>>>>>>>DOOR OPEN<<<<<<<<<<<<<<<");
                valid = 0;
            }

            snprintf(tp, sizeof(tp), "\nMillivolts: %d", millivolts);
            cust_print(tp);
        }
        else
        {
            #if     LC_DEBUG_PRINTF
                        cust_print("\nLoadCell Voltage is outside Valid Range");
            #endif
        }

        vTaskDelay((1000 / LC_PollingFrequencyHz));
    }

#else

    /*
     * Normal Operation (Parameters and Returns indicate communication with Central Task)
     *
     * This Task should get 1 parameter from central task through IPC
     *
     * This Task will have 1 array of LC_MaxSamples length - all in uint16_t
     * that should be reported back to the central task through IPC
     * It will also report back the current state of the LoadCell
     *
     * Param_1: bool Poll_LoadCell
     *          (false: don't do anything, true: start polling Load Cell)
     *
     * Return_1: uint16_t LC_SamplesArraymv[LC_MaxSamples]
     *          (this will have samples recorded on specified frequency, which are to be
     *           processed by Control Node and take a decision. Therefore, the entire array
     *           must be transferred to Central Task through proper IPC method. In case of
     *           the sensor failure, the array will be filled with zeroes.)
     *
     * Return_2: bool LC_Error
     *           (false: Online, true: OFfline - error present)
     *
     */

    static uint8_t storedsamplecount, idletimecount;
    static uint16_t LC_SamplesArraymv[LC_MaxSamples];

    idletimecount = 0;

    while(1)
    {
        // Check the status of Load Cell on regular intervals
        if(idletimecount >= (LC_Online_Test_Timems / LC_Polling_Timems))
        {
            LC_TestSensor();
            #if     LC_DEBUG_PRINTF
                    cust_print("\nChecking LoadCell Status...");
                    if(LC_Error == false)   cust_print("\nLoadCell is Online");
                    else    cust_print("\nLoadCell is Offline");
            #endif
            idletimecount = 0;
        }

        // If the Load Cell module seems to be offline, try to get it back online
        // by reinitializing the interface, and waiting for a valid sample with timeout
        // This feature supports both - limited and infinite retry modes
        if(LC_Error == true)
        {
        #if     (LC_Retry_Mode == LC_Limited)
            if(LC_Retries != 0x00)
            {
                LC_Retries -= 1;
        #endif
                LC_TimerInit();
                LC_DriverInit();
                LC_TestSensor();
        #if     (LC_Retry_Mode == LC_Limited)
            }
        #endif
        }

        if((Poll_LoadCell == true) && (LC_Error == false))
        {
            storedsamplecount = 0;

            // Taking samples till the entire array is filled up
            while(storedsamplecount <= LC_MaxSamples)
            {

                // Clearing the clock because it should be idle when not reading
                // anything from the module
                LC_ClearClk();

                // Resetting the variable used to sense the timeout
                noresptime = 0;

                // Testing sensor/waiting for the module to have a valid sample ready
                LC_TestSensor();

                // Proceed only if there was no timeout, otherwise - write a 0 in the array
                if(LC_Error == false)
                {

                    // Converting float voltages to integer millivolts (to save size)
                    millivolts = (LC_ReadLoadCellVoltage() * 1000);

                    // Filtering samples - so that the processing on the Control Node becomes easier
                    if((millivolts > LC_FilterLowThresholdmv) && (millivolts < LC_FilterHighThresholdmv))   LC_SamplesArraymv[storedsamplecount ++] = millivolts;
                    else        LC_SamplesArraymv[storedsamplecount ++] = 0;
                    #if     LC_DEBUG_PRINTF
                            snprintf(tp, sizeof(tp), "\nMillivolts: %d", millivolts);
                            cust_print(tp);
                    #endif
                }

                // There was a timeout, write a 0
                else        LC_SamplesArraymv[storedsamplecount ++] = 0;

                // Waiting for period defined by polling frequency of the sensor
                vTaskDelay((1000 / LC_PollingFrequencyHz));
            }

            // Resetting polling flag
            Poll_LoadCell = false;
        }

        // Nothing important to do. Just wait
        vTaskDelay(LC_Polling_Timems);
        idletimecount += 1;
    }
#endif


}

//void adc_all(void *pvParameters)
//{
//    cust_print("\nADC Setup Start");
//
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
//
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
//
//    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
//
//    ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_HALF, 1);
//
//    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
//
//    ADCHardwareOversampleConfigure(ADC0_BASE, 16);
//
//    ADCSoftwareOversampleConfigure(ADC0_BASE, 0, 8);
//
//    ADCSoftwareOversampleStepConfigure(ADC0_BASE, 0, 0, (ADC_CTL_CH0 | ADC_CTL_END));
//
//    ADCSequenceEnable(ADC0_BASE, 0);
//
//    static uint32_t adc_data[32], adc_avg;
//    static uint8_t samples_count, i;
//    static char tp[50];
//
//    cust_print("\nADC Setup Done");
//
//    while(1)
//    {
//        ADCProcessorTrigger(ADC0_BASE, 0);
//
//        while(ADCBusy(ADC0_BASE));
//
//        samples_count = ADCSequenceDataGet(ADC0_BASE, 0, &adc_data[0]);
//
//        snprintf(tp, sizeof(tp), "\nSamples: %d", samples_count);
//        cust_print(tp);
//
//        adc_avg = 0;
//
//        for(i = 0; i < samples_count; i++)
//        {
//            adc_avg += adc_data[i];
//        }
//        adc_avg >>= 3;
//
//        snprintf(tp, sizeof(tp), "\nAvg ADC: %d", adc_avg);
//        cust_print(tp);
//
//        vTaskDelay(1000);
//    }
//}
