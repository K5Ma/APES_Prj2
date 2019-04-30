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
extern QueueHandle_t xQueue_KEStruct;
extern QueueHandle_t xQueue_LCStruct;
extern QueueHandle_t xQueue_OIStruct;


uint8_t BBComm_TaskInit()
{
    //Create task, if it fails return 1, else 0
    if( xTaskCreate(BBComm_Task,					/* The function that implements the task */
			   (const portCHAR *)"BBComm",			/* The text name assigned to the task - for debug only as it is not used by the kernel */
               ((configMINIMAL_STACK_SIZE) * 25),	/* The size of the stack to allocate to the task */
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
	xQueue_TXStruct = xQueueCreate(25,  MAX_STRUCT_SIZE);

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
	//const TickType_t xDelay = 10 / portTICK_PERIOD_MS;

	int16_t RX_Index = 0;
	uint32_t RX_Timeout = 0;
	bool RX_Error = false;

	while(1)
	{
		/* If we need to RX something: */
		if(POLL_RX)
		{
			Log_Msg(T_BBComm, "DEBUG", "POLL RX STARTED!", LOCAL_ONLY);

			taskENTER_CRITICAL();

			/* Reset */
			RX_Index = 0;
			RX_Timeout = 0;
			RX_Error = false;

			/* Create an array of bytes to fit the given struct */
			uint8_t RX_Struct_Buffer[MAX_STRUCT_SIZE];

			/* Keep adding to the buffer until we get a "!" */
			do
			{
				/* Send confirmation back */
		//		UART_Putchar_n(UART1, CONFIRM_CMD);

				/* If char is available */
				if(UARTCharsAvail(UART1_BASE) == true)
				{
					/* Block and get byte */
					RX_Struct_Buffer[RX_Index] = UARTCharGet(UART1_BASE);
					/* RX was successful, increment index */
					RX_Index++;
				}
				else
				{
					RX_Timeout++;
				}

				if(RX_Timeout == RX_TIMEOUT_VALUE)
				{
					RX_Error = true;
					break;
				}

			} while ( RX_Struct_Buffer[RX_Index-1] != END_CMD_CHAR );

			taskEXIT_CRITICAL();

			if(RX_Error)
			{
				Log_Msg(T_BBComm, "ERROR", "RX timeout! Discarding data...", LOCAL_ONLY);
			}
			else
			{
				RX_Struct_Buffer[RX_Index-1] = '\x00';

			//	Log_Msg(T_BBComm, "DEBUG", "GOT STRUCT", LOCAL_ONLY);

				Decode_StructBuffer(RX_Struct_Buffer);
			}
			POLL_RX = false;
		}
		else
		{
			/* Else, check if we need to TX something */
			if( xQueue_TXStruct != 0 )
			{
				/* Block for 10 ticks if a message (struct buffer) is not immediately available */
				if( xQueueReceive( xQueue_TXStruct, TX_Struct_Buffer, ( TickType_t ) 10 ) )
				{

//					if(TX_Struct_Buffer[0] == 4)
//					{
//						char prt[50];
//						snprintf(prt, 50, "\nEK Struct is:%x %x %x %x %x %x %x %x", TX_Struct_Buffer[0], TX_Struct_Buffer[1],
//								 TX_Struct_Buffer[2], TX_Struct_Buffer[3], TX_Struct_Buffer[4], TX_Struct_Buffer[5],
//								 TX_Struct_Buffer[6], TX_Struct_Buffer[7]);
//						Log_Msg(T_BBComm, "DEBUG", prt, LOCAL_ONLY);
//					}

//					if(TX_Struct_Buffer[0] == 6)
//					{
//
//						uint8_t i;
//						for(i = 0; i < sizeof(LC_T2B_Struct); i++)
//						{
//							char prt[50];
//							snprintf(prt, 50, "TX_Struct_Buffer[%u] = %u | %c\n", i, TX_Struct_Buffer[i], TX_Struct_Buffer[i]);
//							Log_Msg(T_BBComm, "DEBUG", prt, LOCAL_ONLY);
//						}
//					}

					UART_Putchar_n(UART1, START_CMD);                        //Send Start CMD to BB

					Send_StructBuffer_UARTx(UART1, TX_Struct_Buffer);        //Send structure to BB

					UART_Putchar_n(UART1, END_CMD);                          //Send End CMD to BB
				}
			}
		}

		/* Delay for 10 mS */
		//vTaskDelay(xDelay);
		taskYIELD();
	}
}



void Decode_StructBuffer(uint8_t* StructToDecode)
{
	/* Create the needed structs that will store buffer contents */
	KE_B2T_Struct KE_B2T_ToSend;
	LC_B2T_Struct LC_B2T_ToSend;
	OI_B2T_Struct OI_B2T_ToSend;

	/* Get what structure it is, based on the first byte */
	switch( StructToDecode[0] )
	{
		case LogMsg_Struct_ID:
			Log_Msg(T_BBComm, "ERROR", "How did you even send a LogMsg_Struct back here to Tiva!?", LOCAL_ONLY);
			break;

		case KE_B2T_Struct_ID:
		//	Log_Msg(T_BBComm, "DEBUG", "GOT KE_B2T_Struct!", LOCAL_ONLY);

			/* Copy the contents of the buffer to the struct */
			memcpy(&KE_B2T_ToSend , StructToDecode, sizeof(KE_B2T_Struct));

			/* Send Struct to KeypadEpaper Task xQueue - Wait for 10 ticks if xQueue is full */
			if ( (xQueueSend( xQueue_KEStruct, &KE_B2T_ToSend, ( TickType_t ) 10 ) ) != pdTRUE)
			{
				Log_Msg(T_BBComm, "ERROR", "Could not send KE_B2T_Struct to xQueue_KEStruct", LOCAL_ONLY);
			}
			break;

		case LC_B2T_Struct_ID:
		//	Log_Msg(T_BBComm, "DEBUG", "GOT LC_B2T_Struct!", LOCAL_ONLY);

			/* Copy the contents of the buffer to the struct */
			memcpy(&LC_B2T_ToSend , StructToDecode, sizeof(LC_B2T_Struct));

			/* Send Struct to LoadCell Task xQueue - Wait for 10 ticks if xQueue is full */
			if ( (xQueueSend( xQueue_LCStruct, &LC_B2T_ToSend, ( TickType_t ) 10 ) ) != pdTRUE)
			{
				Log_Msg(T_BBComm, "ERROR", "Could not send LC_B2T_Struct to xQueue_LCStruct", LOCAL_ONLY);
			}
			break;

		case OI_B2T_Struct_ID:
			Log_Msg(T_BBComm, "DEBUG", "GOT OI_B2T_Struct!", LOCAL_ONLY);

			/* Copy the contents of the buffer to the struct */
			memcpy(&OI_B2T_ToSend , StructToDecode, sizeof(OI_B2T_Struct));

			/* Send Struct to Outputs Task xQueue - Wait for 10 ticks if xQueue is full */
			if ( (xQueueSend( xQueue_OIStruct, &OI_B2T_ToSend, ( TickType_t ) 10 ) ) != pdTRUE)
			{
				Log_Msg(T_BBComm, "ERROR", "Could not send OI_B2T_Struct to xQueue_OIStruct", LOCAL_ONLY);
			}
			break;

		default:
			Log_Msg(T_BBComm, "ERROR", "Decode_StructBuffer() aborted - unknown structure!", LOCAL_ONLY);
			return;
	}
}

