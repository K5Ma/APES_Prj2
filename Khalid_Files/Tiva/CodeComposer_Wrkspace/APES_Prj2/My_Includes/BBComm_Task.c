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


//FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"
#include "My_UART.h"

/* Global Variables */


//TEST
extern char BB_Recv[10];
extern int BB_Recv_Index;
extern bool STRUCT_READY;
extern TivaBB_MsgStruct TESTSTRUCT;

extern unsigned char buffer[];


uint8_t BBComm_TaskInit()
{
    //Create task, if it fails return 1, else 0
    if( xTaskCreate(BBComm_Task,					/* The function that implements the task */
			   (const portCHAR *)"BBComm",			/* The text name assigned to the task - for debug only as it is not used by the kernel */
               ((configMINIMAL_STACK_SIZE) * 5),	/* The size of the stack to allocate to the task */
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
	/* Init UART1 - Used to talk to the BeagleBone via Bluetooth */
	Init_UARTx(UART1, SYSTEM_CLOCK, 9600, true);

	UART_Putchar_n(UART1, "?");
	UART_Putchar_n(UART1, "Hello this is very fast let me tell you a story...");
	UART_Putchar_n(UART1, "!");


	while(1)
	{
	//	Log_UART0(GetCurrentTime(), BB_Comm, "INFO", "Waiting for struct from BB");
		//while(!STRUCT_READY);

	//	char TempTxt[100];
	//	snprintf(TempTxt, 100, "Go from BB: %s", BB_Recv);
	//	Log_UART0(GetCurrentTime(), BB_Comm, "INFO", TempTxt);

	//	Log_UART0(GetCurrentTime(), BB_Comm, "INFO", "GOT STRUCT!");

	//	memcpy(&TESTSTRUCT,  buffer, sizeof(TESTSTRUCT));

	//	STRUCT_READY = false;
	//	BB_Recv_Index = 0;
	}



//	/* Create a queue capable of containing 50 messages */
//	xQueue_Msgs = xQueueCreate( 50, sizeof( MsgStruct ) );
//
//	if( xQueue_Msgs == NULL )
//	{
//		/* Queue was not created and must not be used. */
//		Log_UART0(GetCurrentTime(), BB_Comm, "CRITICAL", "Could not create xQueue_Msgs! x_x");
//	}
//	else
//	{
//		Log_UART0(GetCurrentTime(), BB_Comm, "INFO", "Created xQueue_Msgs successfully!");
//	}
//
//	/* Variable that will store and decode received messages */
//	struct MsgStruct RXMessage;
//
//	while(1)
//	{
//		if( xQueue_Msgs != 0 )
//		{
//			/* Receive a message on the created queue.
//			 * Block for 10 ticks if a message is not immediately available */
//			if( xQueueReceive( xQueue_Msgs, &RXMessage, ( TickType_t ) 10 ) )
//			{
//				//HERE IS WHERE I NEED TO SEND DAT TO BB
//
//			}
//		}
//	}
}
