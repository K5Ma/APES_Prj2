/*
 * Master_Functions.c
 *
 *  Created on: Apr 9, 2019
 *      Author: Khalid AlAwadhi
 */
#include "Master_Functions.h"

//Standard includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//TivaWare includes
#include "utils/uartstdio.h"

//FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//My includes
#include "Global_Defines.h"
#include "My_UART.h"


/* Global Variables */
extern QueueHandle_t xQueue_Msgs;


void Log_UART0(float CurrTime, uint8_t Src, char* LogLvl, char* Msg)
{
	/* Get name of source */
	char* Source_text;
	switch(Src)
	{
		case Main:
			Source_text = "Main";
			break;

		case BB_Comm:
			Source_text = "BB_Comm";
			break;

		default:
			Source_text = "Unknown";
			break;
	}

	/* Convert all data into a string */
	char UART0_Log[150];
	snprintf(UART0_Log, 150, "> [%.3f] %s Task (%s) => %s\n\r", CurrTime, Source_text, LogLvl, Msg);

	/* Output to UART0 */
	UART_Putchar_n(UART0, UART0_Log);
}



float GetCurrentTime()
{
	return ((float)xTaskGetTickCount())/1000;
}



uint8_t SendMsgToBB(MsgStruct *MsgToSend)
{
	/* Add current timestamp to Msg being sent */
	MsgToSend->Timestamp = GetCurrentTime();

	/* Send Msg to BB_Comm Task, block for 10 ticks (10 ms) for the msg to be sent. Then throw an error if it still fails. */
	if ( (xQueueSend( xQueue_Msgs, MsgToSend, ( TickType_t ) 10 )) != pdTRUE )
	{
		return 1;
	}
	return 0;
}
//
///* Convert all data into a string */
//char UART0_Log[250];
//snprintf(UART0_Log, 250, "TEST:\nMallocedMsg->Source:%u\nMallocedMsg->LogLevel:%s\nMallocedMsg->Msg:%s\nMallocedMsg->Data_float1:%f\nMallocedMsg->Data_float2:%f\n\n",
//		 	 	 	 	  MallocedMsg->Source, MallocedMsg->LogLevel, MallocedMsg->Msg, MallocedMsg->Data_float1, MallocedMsg->Data_float2);
//
///* Output to UART0 */
//UART_Putchar_n(UART0, UART0_Log);
