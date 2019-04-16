/*
 * Temp_Task.c
 *
 *  Created on: Apr 10, 2019
 *      Author: poorn
 */

//#include "main.h"

#include "Log_Task.h"

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
extern TaskHandle_t  temp_task;

void Log_Task(void *pvParameters)
{
    static uint32_t t_sec, t_msec;
    static struct ipc_data ipc_info;
    static char log_msg[100];
    log_queue = xQueueCreate(10, sizeof(struct ipc_data));
    if(log_queue == 0)
    {
        cust_print("\nLOG Queue Creation Failed");
    }
    else    cust_print("\nLOG Queue Created");
    while(1)
    {
        if(xQueueReceive(log_queue, (void *)&ipc_info, portMAX_DELAY) == pdTRUE)
        {
            if(ipc_info.sender == LED_Sender)
            {
                t_sec = ipc_info.led_time / 1000;
                t_msec = ipc_info.led_time % 1000;
//                sprintf(log_msg, "\nName: %s LED Toggle Count: %d LED Time: %d.%d", ipc_info.name, ipc_info.led_count, t_sec, t_msec);
                snprintf(log_msg, sizeof(log_msg), "\nName: %s LED Toggle Count: %d LED Time: %d.%d", ipc_info.name, ipc_info.led_count, t_sec, t_msec);
            }
            else if(ipc_info.sender == Temp_Sender)
            {
                t_sec = ipc_info.temp_time / 1000;
                t_msec = ipc_info.temp_time % 1000;
//                sprintf(log_msg, "\nCurrent Temp: %f Taken at: %d.%d", ipc_info.temp_value, t_sec, t_msec);
                snprintf(log_msg, sizeof(log_msg), "\nCurrent Temp: %f Taken at: %d.%d", ipc_info.temp_value, t_sec, t_msec);
            }
            taskENTER_CRITICAL() ;
            cust_print(log_msg);
//            UBaseType_t uxHighWaterMark;
//            uxHighWaterMark = uxTaskGetStackHighWaterMark(temp_task);
//            snprintf(log_msg, sizeof(log_msg), "\nTemp Unused Stack: %d", uxHighWaterMark);
//            cust_print(log_msg);
            taskEXIT_CRITICAL() ;

            UBaseType_t uxHighWaterMark;
            uxHighWaterMark = uxTaskGetStackHighWaterMark(0);
//            snprintf(tmsg, sizeof(tmsg), "\nTemp Unused Stack: %d", uxHighWaterMark);
//            cust_print(tmsg);
            if(uxHighWaterMark < 20)
            {
                snprintf(log_msg, sizeof(log_msg), "\n>>>>>>>>>>>>STACK OVERFLOW WARNING IN LOG\n");
                cust_print(log_msg);
            }
        }
    }
}
