/*
 * LuxTask.c
 *
 *  Created on: Apr 23, 2019
 *  Last Update: Apr 23, 2019
 *      Author: Poorn Mehta
 *
 *  Driver for Interfacing with Lux Sensor (APDS-9301) which is
 *  connected to TM4C1294XL (TIVA Development Board) using I2C Bus
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
 * SCL - PB2 (I2C0_SCL)
 * SDA - PB3 (I2C0_SDA)
 * 3.3V - 3.3V
 * GND - GND
 *
 */

#include "LuxTask.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Variables that will be shared between functions
uint8_t Lux_I2C_Data[2], Lux_Retries;
bool Lux_Error = false, Lux_Disable;
float Lux_Value;
uint16_t Lux_Level;

/*
 * Function to wait for Lux Sensor to respond on
 * I2C bus - while also supporting timeout for
 * error detection
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void Lux_I2C_busy_wait_blocking(void)
{
    Lux_Error = false;
    static uint32_t timeoutcount;
    timeoutcount = 0;

    while(!I2CMasterBusy(I2C0_BASE))
    {
        vTaskDelay(1);
        timeoutcount += 1;
        if(timeoutcount >= Lux_Timeoutms)
        {
            Lux_Error = true;
            #if     (Lux_Retry_Mode == Lux_Limited)
                Lux_Retries = Lux_Max_Retries;
            #endif
            break;
        }
    }

    if(Lux_Error == false)
    {
        while(I2CMasterBusy(I2C0_BASE))
        {
            vTaskDelay(1);
            timeoutcount += 1;
            if(timeoutcount >= Lux_Timeoutms)
            {
                Lux_Error = true;
                #if     (Lux_Retry_Mode == Lux_Limited)
                    Lux_Retries = Lux_Max_Retries;
                #endif
                break;
            }
        }
    }
}

/*
 * Function to initialize pin configuration for Lux Sensor
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void Lux_I2C_pin_config(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
}

/*
 * Function to setup I2C0 peripheral for Lux Sensor
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void Lux_I2C_perph_config(void)
{
    SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0));
}

/*
 * Function to write to Lux Sensor's internal register
 *
 * Param: Either the internal address of the register, or the data to be written
 *
 * Return: Null
 *
 */
void Lux_I2C_Reg_Access(uint8_t access_data)
{
    static uint32_t Lux_Resp;
    static char tp[50];

    I2CMasterSlaveAddrSet(I2C0_BASE, Lux_Address, I2C_Write_Mode);
    I2CMasterDataPut(I2C0_BASE, access_data);

    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

    Lux_I2C_busy_wait_blocking();

    Lux_Resp = I2CMasterErr(I2C0_BASE);
    if(Lux_Resp != I2C_MASTER_ERR_NONE)
    {
        Lux_Error = true;
        #if     (Lux_Retry_Mode == Lux_Limited)
            Lux_Retries = Lux_Max_Retries;
        #endif
        #if Lux_DEBUG_PRINTF
            snprintf(tp, sizeof(tp), "\nError while Writing: %x", Lux_Resp);
            Lux_Print(tp);
        #endif
    }
}

/*
 * Function to read from Lux Sensor's internal register
 *
 * Param_1: Pointer to the address where read data should be stored
 * Param_2: Number of bytes to be read consecutively
 *
 * Return: Null
 *
 */
void Lux_I2C_read(uint8_t *target_variable, uint8_t no_of_bytes)
{
    static uint8_t i;
    static uint32_t Lux_Resp;
    static char tp[50];

    I2CMasterSlaveAddrSet(I2C0_BASE, Lux_Address, I2C_Read_Mode);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

    Lux_I2C_busy_wait_blocking();

    Lux_Resp = I2CMasterErr(I2C0_BASE);
    if(Lux_Resp != I2C_MASTER_ERR_NONE)
    {
        Lux_Error = true;
        #if     (Lux_Retry_Mode == Lux_Limited)
            Lux_Retries = Lux_Max_Retries;
        #endif
        #if Lux_DEBUG_PRINTF
            snprintf(tp, sizeof(tp), "\nRead 1 Error: %x", Lux_Resp);
            Lux_Print(tp);
        #endif
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
    }
    else
    {
        *target_variable++ = (uint8_t)I2CMasterDataGet(I2C0_BASE);
        for(i = 0; i < (no_of_bytes - 1); i ++)
        {
            I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);

            Lux_I2C_busy_wait_blocking();

             Lux_Resp = I2CMasterErr(I2C0_BASE);
             if(Lux_Resp != I2C_MASTER_ERR_NONE)
             {
                 Lux_Error = true;
                #if     (Lux_Retry_Mode == Lux_Limited)
                    Lux_Retries = Lux_Max_Retries;
                #endif
                #if Lux_DEBUG_PRINTF
                     snprintf(tp, sizeof(tp), "\nRead 2 Error: %x", Lux_Resp);
                     Lux_Print(tp);
                #endif
                 I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
                 break;
             }
             else       *target_variable++ = (uint8_t)I2CMasterDataGet(I2C0_BASE);
        }
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
        Lux_I2C_busy_wait_blocking();
    }
}

/*
 * Function to initialize Lux Sensor (I2C0 bus)
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void Lux_I2C_init(void)
{
    Lux_I2C_pin_config();
    Lux_I2C_perph_config();
    I2CMasterInitExpClk(I2C0_BASE, SYSTEM_CLOCK, I2C_100KHZ_Mode);
    #if Lux_DEBUG_PRINTF
        Lux_Print("\nI2C Init Success");
    #endif
}

/*
 * Function to check whether Lux sensor is online or not
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void Lux_Test_Sensor(void)
{
    static uint8_t Lux_I2C_Data = 0;
    Lux_I2C_Data = Lux_Command_Reg_Data(Lux_Control_Reg);
    Lux_I2C_Reg_Access(Lux_I2C_Data);
    if(Lux_Error == false)  Lux_I2C_Reg_Access(Lux_Control_Power_ON);
}

/*
 * Function to write given data to the internal register address
 *
 * Param_1: Internal register address
 * Param_2: Data to be written
 *
 * Return: Null
 *
 */
void Lux_I2C_write(uint8_t r_addr, uint8_t r_val)
{
        static uint8_t Lux_I2C_Data = 0;
        Lux_I2C_Data = Lux_Command_Reg_Data(r_addr);

        if(Lux_Error == false)      Lux_I2C_Reg_Access(Lux_I2C_Data);
        if(Lux_Error == false)      Lux_I2C_Reg_Access(r_val);
}

/*
 * Function to read Lux Level
 *
 * Param_1: Null
 *
 * Return: Null
 *
 */
void Lux_read(void)
{
    static float lux_ch0, lux_ch1, ratio;

    if(Lux_Error == false)      Lux_I2C_write(Lux_Control_Reg, Lux_Control_Power_ON);
    if(Lux_Error == false)      Lux_I2C_write(Lux_Timing_Reg, Lux_Set_Gain_High(Lux_High_Integration_Time));

    Lux_I2C_Data[0] = Lux_Command_Word_Data(Lux_Data0_Low);

    if(Lux_Error == false)      Lux_I2C_Reg_Access(Lux_I2C_Data[0]);
    if(Lux_Error == false)      Lux_I2C_read(&Lux_I2C_Data[0], 2);

    if(Lux_Error == false)      lux_ch0 = (float)((Lux_I2C_Data[1] << 8) | Lux_I2C_Data[0]);

    Lux_I2C_Data[0] = Lux_Command_Word_Data(Lux_Data1_Low);

    if(Lux_Error == false)      lux_ch0 = (float)((Lux_I2C_Data[1] << 8) | Lux_I2C_Data[0]);
    if(Lux_Error == false)      Lux_I2C_Reg_Access(Lux_I2C_Data[0]);

    if(Lux_Error == false)      Lux_I2C_read(&Lux_I2C_Data[0], 2);

    if(Lux_Error == false)      lux_ch1 = (float)((Lux_I2C_Data[1] << 8) | Lux_I2C_Data[0]);

    if(Lux_Error == false)
    {
        ratio = (lux_ch1 / lux_ch0);
        if((ratio > 0.0) && (ratio <= 0.5))         Lux_Value = (0.0304 * lux_ch0) - (0.062 * lux_ch0 * pow(ratio, 1.4));
        else if((ratio > 0.5) && (ratio <= 0.61))       Lux_Value = (0.0224 * lux_ch0) - (0.031 * lux_ch1);
        else if((ratio > 0.61) && (ratio <= 0.80))      Lux_Value = (0.0128 * lux_ch0) - (0.0153 * lux_ch1);
        else if((ratio > 0.80) && (ratio <= 1.30))      Lux_Value = (0.00146 * lux_ch0) - (0.00112 * lux_ch1);
        else    Lux_Value = 0;
    }
    Lux_Level = (uint16_t)Lux_Value;
}

/*
 *
 * Callback Function for Lux Task
 *
 * Return: Null
 *
 */

/*
 * Normal Operation (Parameters and Returns indicate communication with Central Task)
 *
 * This Task requires 1 parameter from Central Task through IPC
 *
 * This Task will return 1 integer value containing Lux Level
 * It will also report back the current sensor state
 *
 * Param_1: bool Lux_Disable
 *          (false: keep polling, true: stop the sensor for 60 seconds)
 *
 * Return_1: uint16_t Lux_Level
 *          (the current lux level of the surrounding area)
 *
 * Return_2: bool Lux_Error
 *           (false: Online, true: OFfline - error present)
 *
 */
void LuxTask(void *pvParameters)
{
    static char tp[50];
    Lux_I2C_init();

    #if Lux_INDIVIDUAL_TESTING
        Lux_Disable = false;
    #endif

    while(1)
    {
        if(Lux_Disable == true)
        {
            vTaskDelay(Lux_Stay_Off_Timeoutms);
            Lux_Disable = false;
        }

        if(Lux_Error == false)
        {
            Lux_read();
            if(Lux_Error == false)
            {
            #if Lux_DEBUG_PRINTF
                snprintf(tp, sizeof(tp), "\nLux Level: %d", Lux_Level);
                Lux_Print(tp);
            #endif
            }
        }

        if(Lux_Error == true)
        {
            #if     (Lux_Retry_Mode == Lux_Limited)
                if(Lux_Retries != 0)
                {
                    Lux_Retries -= 1;
            #endif
            #if Lux_DEBUG_PRINTF
                Lux_Print("\nLux Sensor Offline... Retrying...");
            #endif
                    Lux_Test_Sensor();
                    if(Lux_Error == false)
                    {
                        #if Lux_DEBUG_PRINTF
                            Lux_Print("\nLux Sensor is Back Online");
                        #endif
                    }
            #if     (Lux_Retry_Mode == Lux_Limited)
                }
            #endif
        }
        vTaskDelay(Lux_Polling_Timems);
    }
}
