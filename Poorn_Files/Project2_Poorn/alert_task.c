/*
 * alert_task.c
 *
 *  Created on: Apr 10, 2019
 *      Author: poorn
 */

#include "alert_task.h"

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

#include "Temp_Task.h"
#include "LED_Task.h"
#include "Log_Task.h"
#include "alert_task.h"

extern TaskHandle_t alert_task_handle;

void Alert_Task(void *pvParameters)
{
    alert_task_handle = xTaskGetCurrentTaskHandle();
    uint32_t alert_notify = 0;
    const TickType_t block_time = pdMS_TO_TICKS(1000);

    while(1)
    {
        alert_notify = ulTaskNotifyTake( pdTRUE, block_time );
        if(alert_notify == 1)
        {
            taskENTER_CRITICAL() ;
            cust_print("\n>>>>>>>>>>>>>ALERT TEMPERATURE IS VERY HIGH<<<<<<<<<<<<<<<");
            taskEXIT_CRITICAL() ;
        }
    }
}
