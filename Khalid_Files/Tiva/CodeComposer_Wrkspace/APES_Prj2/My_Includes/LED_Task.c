/*
 * LED_Task.c
 *
 *  Created on: Apr 7, 2019
 *      Author: Khalid AlAwadhi
 */
#include "LED_Task.h"

//Standard includes
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//TivaWare includes
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

//FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"

/* Global Variables */
extern QueueHandle_t xQueue_Msgs;


uint8_t LED_TaskInit()
{
    //Create task, if it fails return 1, else 0
    if( xTaskCreate(LED_Task,						/* The function that implements the task */
			   (const portCHAR *)"LED",				/* The text name assigned to the task - for debug only as it is not used by the kernel */
			   ((configMINIMAL_STACK_SIZE) * 8),	/* The size of the stack to allocate to the task */
			   NULL,								/* The parameter passed to the task */
			   0, 					/* The priority assigned to the task */
			   NULL)								/* The task handle is not required, so NULL is passed */
    			!= pdTRUE
      )
      {
    	return 1;
      }

	return 0;
}



void LED_Task(void *pvParameters)
{
	/* Init LED D1 to be a GPIO OUTPUT */
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);

	/* Init LED D2 to be a GPIO OUTPUT */
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

	 /* Block for 1000 ms  */
	const TickType_t xDelay = 700 / portTICK_PERIOD_MS;


	while(1)
	{
		/* Turn D1 and D2 ON */
		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);

		vTaskDelay(xDelay);

		/* Turn D1 and D2 OFF */
		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);

		vTaskDelay(xDelay);


		/* Create Msg to send */
		struct MsgStruct TXMessage =
		{
		 	 .Source = 3,
			 .LogLevel = "INFO",
			 .Msg = "Wassup",
			 .Data_float1 = 4.2,
			 .Data_float2 = 6.9
		};

		/* Send Msg */
		if(SendMsgToBB(&TXMessage))
		{
			Log_UART0(GetCurrentTime(), 4, "ERROR", "Could not send msg to BB_Comm Task!");
		}
	}
}
