/*
 * Temp_Task.c
 *
 *  Created on: Apr 10, 2019
 *      Author: poorn
 */

//#include "main.h"

#include "Temp_Task.h"

#include <stdint.h>
#include <stdbool.h>
#include "drivers/pinout.h"
#include "utils/uartstdio.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/fpu.h"

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#include "sensorlib/i2cm_drv.h"
#include "driverlib/i2c.h"
#include "inc/hw_i2c.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

extern QueueHandle_t log_queue;

extern TaskHandle_t alert_task_handle;

uint32_t resp;

void tmp102_i2c_busy_wait_blocking(void)
{
    while(!ROM_I2CMasterBusy(I2C0_BASE));
    while(ROM_I2CMasterBusy(I2C0_BASE));
}

void tmp102_i2c_pin_config(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
}

void tmp102_i2c_perph_config(void)
{
    SysCtlPeripheralDisable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0));
}

void tmp102_i2c_select_register(uint8_t reg_addr)
{
    //write
    ROM_I2CMasterSlaveAddrSet(I2C0_BASE, TMP102_Address, I2C_Write_Mode);
    ROM_I2CMasterDataPut(I2C0_BASE, reg_addr);
    ROM_I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

    tmp102_i2c_busy_wait_blocking();

    resp = ROM_I2CMasterErr(I2C0_BASE);
    if(resp != I2C_MASTER_ERR_NONE)
    {
        snprintf(msg, sizeof(msg), "\nWrite: %x", resp);
        cust_print(msg);
    }
}

void tmp102_i2c_read(uint8_t *target_variable, uint8_t no_of_bytes)
{
    static uint8_t i;
    //read
    ROM_I2CMasterSlaveAddrSet(I2C0_BASE, TMP102_Address, I2C_Read_Mode);
    ROM_I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

    tmp102_i2c_busy_wait_blocking();

    resp = ROM_I2CMasterErr(I2C0_BASE);
    if(resp != I2C_MASTER_ERR_NONE)
    {
        snprintf(msg, sizeof(msg), "\nRead 1 Error: %x", resp);
        cust_print(msg);
        ROM_I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
    }
    else
    {
        *target_variable++ = (uint8_t)ROM_I2CMasterDataGet(I2C0_BASE);
        for(i = 0; i < (no_of_bytes - 1); i ++)
        {
            ROM_I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);

            tmp102_i2c_busy_wait_blocking();

             resp = ROM_I2CMasterErr(I2C0_BASE);
             if(resp != I2C_MASTER_ERR_NONE)
             {
                 snprintf(msg, sizeof(msg), "\nRead 2 Error: %x", resp);
                 cust_print(msg);
                 ROM_I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
                 break;
             }
             else       *target_variable++ = (uint8_t)ROM_I2CMasterDataGet(I2C0_BASE);
        }
        ROM_I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
        tmp102_i2c_busy_wait_blocking();
    }
}

void tmp102_i2c_init(void)
{
    tmp102_i2c_pin_config();

    tmp102_i2c_perph_config();

    ROM_I2CMasterInitExpClk(I2C0_BASE, SYSTEM_CLOCK, I2C_100KHZ_Mode);

    cust_print("\nI2C Init Success");

    tmp102_i2c_select_register(TMP102_Config_Reg);

    tmp102_i2c_read(&i2c_data[0], 2);

    if((i2c_data[0] == 0x60) && (i2c_data[1] == 0xA0))      cust_print("\nTMP102 Init Success");
    else
    {
        cust_print("\nTMP102 Init Failed");
        snprintf(msg, sizeof(msg), "\nData: 0-%x 1-%x", i2c_data[0], i2c_data[1]);
        cust_print(msg);
    }
}

void tmp102_read(float *t_data)
{
    tmp102_i2c_select_register(TMP102_Data_Reg);

    tmp102_i2c_read(&i2c_data[0], 2);

    *t_data = (((i2c_data[0] << 8) | i2c_data[1]) >> 4) * 0.0625;
}

void Temp_Task(void *pvParameters)
{
    static char tmsg[50];
    static struct ipc_data ipc_info;
    static float tempc;
    while(1)
    {
        vTaskDelay(1000);
        ipc_info.sender = Temp_Sender;
        ipc_info.temp_time = xTaskGetTickCount();
        tmp102_read(&tempc);
        ipc_info.temp_value = tempc;
        if(log_queue)
        {
            if(xQueueSend(log_queue,(void *)&ipc_info, 0) != pdTRUE)
            {
                cust_print("\nTEMP to Log Error");
            }
        }

        if(tempc > Temp_High_Thr)
        {
            xTaskNotifyGive(alert_task_handle);
        }

//            taskENTER_CRITICAL() ;
//            UBaseType_t uxHighWaterMark;
//            uxHighWaterMark = uxTaskGetStackHighWaterMark(0);
//            snprintf(tmsg, sizeof(tmsg), "\nTemp Unused Stack: %d", uxHighWaterMark);
//            cust_print(tmsg);
//            taskEXIT_CRITICAL() ;
    }
}


