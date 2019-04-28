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
extern QueueHandle_t xQueue_TXStruct;


void DisplayBootUpMsg(uint8_t UARTPort)
{
	UART_Putchar_n(UARTPort, "\n\r> ***************************************************\n\r");
	UART_Putchar_n(UARTPort, "> *             APES Project 2 (Tiva Side):         *\n\r");
	UART_Putchar_n(UARTPort, "> *               S.E.C.U.R.E.R.O.O.M               *\n\r");
	UART_Putchar_n(UARTPort, "> * 'S'uper 'E'ncrypted 'C'ourse project which is a *\n\r");
	UART_Putchar_n(UARTPort, "> * 'U'seful 'R'oom 'E'mbedded 'S'ystem that        *\n\r");
	UART_Putchar_n(UARTPort, "> * communicates 'R'emotely 'O'ver the air and it's *\n\r");
	UART_Putchar_n(UARTPort, "> *              'O'bviously 'M'arvelous            *\n\r");
	UART_Putchar_n(UARTPort, "> *                                                 *\n\r");
	UART_Putchar_n(UARTPort, "> *        By: Khalid AlAwadhi | Poorn Mehta        *\n\r");
	char LineTemp[60];
	snprintf(LineTemp, 60, "> *                                            v%s *\n\r", CURRENT_VER);
	UART_Putchar_n(UARTPort, LineTemp);
	UART_Putchar_n(UARTPort, "> ***************************************************\n\n\r");
}



float GetCurrentTime()
{
	return ((float)xTaskGetTickCount())/1000;
}



void EnumtoString(uint8_t EnumNum, char* Str)
{
	/* Store name based on enum */
	switch(EnumNum)
	{
		/* Tiva Sources */
		case T_Main:
			strcpy(Str, "Main Task");
			break;

		case T_Logger:
			strcpy(Str, "Logger Task");
			break;

		case T_BBComm:
			strcpy(Str, "BBComm Task");
			break;

		case T_NFC:
			strcpy(Str, "NFC Task");
			break;

		case T_KeypadEpaper:
			strcpy(Str, "KeypadEpaper Task");
			break;

		case T_LoadCell:
			strcpy(Str, "LoadCell Task");
			break;

		case T_Lux:
			strcpy(Str, "Lux Task");
			break;

		case T_Servo:
			strcpy(Str, "Servo Task");
			break;

		case T_SpeakJet:
			strcpy(Str, "SpeakJet Task");
			break;

		case T_Outputs:
			strcpy(Str, "Outputs Task");
			break;

		/* BeagleBone Sources */
		case BB_Main:
			strcpy(Str, "Main Thread");
			break;

		case BB_Logger:
			strcpy(Str, "Logger Thread");
			break;

		case BB_TivaComm:
			strcpy(Str, "TivaComm Thread");
			break;

		case BB_NFC:
			strcpy(Str, "NFC Thread");
			break;

		case BB_KeypadEpaper:
			strcpy(Str, "KeypadEpaper Thread");
			break;

		case BB_LoadCell:
			strcpy(Str, "LoadCell Thread");
			break;

		default:
			strcpy(Str, "Unknown Thread/Task");
			break;
	}
}


void Log_Msg(uint8_t Src, char* LogLvl, char* OutMsg, uint8_t Mode)
{
	/* Get current Tiva time */
	float CurrTivaTime = GetCurrentTime();

	taskENTER_CRITICAL();

	/* Output message depending on chosen mode */
	switch(Mode)
	{
		case LOGGER_ONLY:
			Send_LogMsgStruct_ToBB(Src, LogLvl, OutMsg);
			break;

		case LOGGER_AND_LOCAL:
			Log_UART0(CurrTivaTime, Src, LogLvl, OutMsg);
			Send_LogMsgStruct_ToBB(Src, LogLvl, OutMsg);
			break;

		default:
			Log_UART0(CurrTivaTime, Src, LogLvl, OutMsg);
			break;
	}

	taskEXIT_CRITICAL();
}




void Log_UART0(float CurrTime, uint8_t Src, char* LogLvl, char* Msg)
{
	/* Get name of source */
//	char Source_text[SRC_SIZE];
	char *Source_text = (char*) malloc(sizeof(char) * SRC_SIZE);
	if(Source_text == NULL)
	{
		UART_Putchar_n(UART0, ">>> Error Source_text malloc\n");
	}

	//char Source_text[SRC_SIZE];
	EnumtoString(Src, Source_text);

	/* Convert all data into a string */
//	char UART0_Log[100];
	char *UART0_Log = (char*) malloc(sizeof(char) * 120);
	if(UART0_Log == NULL)
	{
		UART_Putchar_n(UART0, ">>> Error UART0_Log malloc\n");
	}
	snprintf(UART0_Log, 120, "> [%.3f] Log Event(%s): %s\n\r> L->Source: %s\n\n\r", CurrTime, LogLvl, Msg, Source_text);

	/* Output to UART0 */
	UART_Putchar_n(UART0, UART0_Log);

	free(Source_text);
	free(UART0_Log);
}


void Send_LogMsgStruct_ToBB(uint8_t Src, char* LogLvl, char* OutMsg)
{
	/* Create the struct to send */
	LogMsg_Struct StructToSend =
	{
	 	 .ID = LogMsg_Struct_ID,
		 .Src = Src
	};
	strcpy(StructToSend.LogLevel, LogLvl);
	strcpy(StructToSend.Msg, OutMsg);

	/* Create an array of bytes to fit the given struct */
	uint8_t Buffer_Struct[sizeof(StructToSend)+1];

	/* Copy the contents of our struct to the char array */
	memcpy(Buffer_Struct, &StructToSend, sizeof(StructToSend));

	/* Send Struct to BBComm Task xQueue - Wait for 10 ticks if xQueue is full */
	if ( (xQueueSend( xQueue_TXStruct, &StructToSend, ( TickType_t ) 10 ) ) != pdTRUE)
	{
		Log_Msg(Src, "ERROR", "Could not send LogMsgStruct to xQueue_TXStruct", LOCAL_ONLY);
	}
}

