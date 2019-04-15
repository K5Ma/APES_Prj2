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


void Log_UART(uint8_t Src, char* LogLvl, char* Msg)
{
	/* Get name of source */
	char* Source_text;
	switch(Src)
	{
		case Main:
			Source_text = "Main";
			break;

		default:
			Source_text = "Unknown";
			break;
	}

	char Time_mS[20];

	snprintf( Time_mS, 20, "%.3f", (float)xTaskGetTickCount()/1000 );

	/* Output error to UART */
	UARTprintf("[%s] %s Task (%s) => %s\n\n", Time_mS, Source_text, LogLvl, Msg);
}
