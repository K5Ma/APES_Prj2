/*
 * Temp_Task.c
 *
 *  Created on: Apr 10, 2019
 *      Author: poorn
 */

//#include "main.h"

#include "LED_Task.h"

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

void LED_Task(void *pvParameters)
{
    static char tmsg[50];
    static struct ipc_data ipc_info;
    static uint32_t count = 0;
    while(1)
    {
        vTaskDelay(50);
        ipc_info.sender = LED_Sender;
        ipc_info.led_time = xTaskGetTickCount();
        strcpy(ipc_info.name, "Poorn");
        count += 1;
        ipc_info.led_count = count;
        if(led_state)
        {
            LEDWrite(0x01, 0x01);
            LEDWrite(0x02, 0x00);
            LEDWrite(0x04, 0x04);
            LEDWrite(0x08, 0x00);
            led_state = false;
        }
        else
        {
            LEDWrite(0x01, 0x00);
            LEDWrite(0x02, 0x02);
            LEDWrite(0x04, 0x00);
            LEDWrite(0x08, 0x08);
            led_state = true;
        }
        if(log_queue)
        {
            if(xQueueSend(log_queue,(void *)&ipc_info, 0) != pdTRUE)
            {
                cust_print("\nLED to Log Error");
            }
        }

//        UBaseType_t uxHighWaterMark;
//        uxHighWaterMark = uxTaskGetStackHighWaterMark(0);
//            snprintf(tmsg, sizeof(tmsg), "\nTemp Unused Stack: %d", uxHighWaterMark);
//            cust_print(tmsg);
//        if(uxHighWaterMark < 20)
//        {
//            snprintf(tmsg, sizeof(tmsg), "\n>>>>>>>>>>>>STACK OVERFLOW WARNING IN LED\n");
//            cust_print(tmsg);
//        }
    }
}


