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

//TivaWare includes
#include "utils/uartstdio.h"

//FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"

//My includes
#include "Global_Defines.h"
#include "My_UART.h"



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

