/*
 * BBComm_Task.c
 *
 *  Created on: Apr 15, 2019
 *      Author:  Author: Khalid AlAwadhi
 */
#include "BBComm_Task.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

//TivaWare includes
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"

//FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"
#include "My_UART.h"

/* Global Variables */
extern QueueHandle_t xQueue_TXStruct;
extern bool POLL_RX;

uint8_t BBComm_TaskInit()
{
    //Create task, if it fails return 1, else 0
    if( xTaskCreate(BBComm_Task,					/* The function that implements the task */
			   (const portCHAR *)"BBComm",			/* The text name assigned to the task - for debug only as it is not used by the kernel */
               ((configMINIMAL_STACK_SIZE) * 8),	/* The size of the stack to allocate to the task */
			   NULL,								/* The parameter passed to the task */
			   PRIORITY_BBCOMM_TASK, 				/* The priority assigned to the task */
			   NULL)								/* The task handle is not required, so NULL is passed */
    			!= pdTRUE
      )
      {
    	return 1;
      }

	return 0;
}



void BBComm_Task(void *pvParameters)
{
	/* Create a queue capable of containing 50 structs */
	xQueue_TXStruct = xQueueCreate(30,  MAX_STRUCT_SIZE);

	if( xQueue_TXStruct == NULL )
	{
		/* Queue was not created and must not be used. */
		Log_Msg(T_BBComm, "FATAL", "Could not create xQueue_TXStruct!", LOCAL_ONLY);
	}
	else
	{
		Log_Msg(T_BBComm, "INFO", "Created xQueue_TXStruct successfully!", LOCAL_ONLY);
	}

	/* Buffer that will store received structs */
	uint8_t RX_Struct_Buffer[MAX_STRUCT_SIZE];

	/* Buffer that will store structs to be TXed */
	uint8_t TX_Struct_Buffer[MAX_STRUCT_SIZE];

	/* Init UART1 - Used to talk to the BeagleBone via Bluetooth */
	Init_UARTx(UART1, SYSTEM_CLOCK, 9600, true);

	/* Delay before next poll */
	const TickType_t xDelay = 10 / portTICK_PERIOD_MS;

	while(1)
	{
		/* If we need to RX something: */
		if(POLL_RX)
		{
			taskENTER_CRITICAL();

			Log_Msg(T_BBComm, "DEBUG", "POLL RX STARTED!", LOCAL_ONLY);

			int16_t RX_Index = 0;

			/* Create an array of bytes to fit the given struct */
			char RX_Struct_Buffer[MAX_STRUCT_SIZE];

			/* Create a pointer that will iterate through the array and RX from Tiva side */
			uint8_t* ptr = &RX_Struct_Buffer;

			/* Keep adding to the buffer until we get a "!" */
			do
			{
				/* Send confirmation back */

				/* Block and get byte */

				RX_Struct_Buffer[RX_Index] = UARTCharGet(UART1_BASE);


				/* RX was successful, increment pointer */
				ptr++;
				RX_Index++;

			} while ( RX_Struct_Buffer[RX_Index-1] != END_CMD_CHAR );

			taskEXIT_CRITICAL();

			RX_Struct_Buffer[RX_Index-1] = '\x00';

			Log_Msg(T_BBComm, "DEBUG", "GOT STRUCT", LOCAL_ONLY);

			Decode_StructBuffer(RX_Struct_Buffer);

			POLL_RX = false;
		}

		/* Else, check if we need to TX something */
		else if( xQueue_TXStruct != 0 )
		{
			/* Block for 10 ticks if a message (struct buffer) is not immediately available */
			if( xQueueReceive( xQueue_TXStruct, TX_Struct_Buffer, ( TickType_t ) 10 ) )
			{
		//		Log_Msg(T_BBComm, "DEBUG", "GOT STRUCT TO TX!", LOCAL_ONLY);

				UART_Putchar_n(UART1, START_CMD);                        //Send Start CMD to BB

				Send_StructBuffer_UARTx(UART1, TX_Struct_Buffer);        //Send structure to BB

				UART_Putchar_n(UART1, END_CMD);                          //Send End CMD to BB
			}
		}

		/* Delay for 10 mS */
		vTaskDelay(xDelay);
	}
}



void Decode_StructBuffer(uint8_t* StructToDecode)
{
	/* Create the needed structs that will store buffer contents */
//	LogMsg_Struct LogMsgToSend;

	/* Get what structure it is, based on the first byte */
	switch( StructToDecode[0] )
	{
		case LogMsg_Struct_ID:
			Log_Msg(T_BBComm, "ERROR", "How did you even send a LogMsg_Struct back here to Tiva!?", LOCAL_ONLY);

//			/* Copy the contents of the buffer to the struct */
//			memcpy(&LogMsgToSend , StructToDecode, sizeof(LogMsg_Struct));
//
//			/* Send to Logger POSIX Q */
//			Log_Msg(LogMsgToSend.Src, LogMsgToSend.LogLevel, LogMsgToSend.Msg, 0, LOGGER_AND_LOCAL);
			break;


		default:
			Log_Msg(T_BBComm, "ERROR", "Decode_StructBuffer() aborted - unknown structure!", LOCAL_ONLY);
			return;
	}
}

