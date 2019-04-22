/*
 * TivaComm_Thread.c
 *
 *  Created on: Apr 20, 2019
 *      Author: Khalid AlAwadhi
 */

#include "TivaComm_Thread.h"

//Standard includes
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"
#include "My_UART_BB.h"



void * TivaCommThread(void * args)
{
	/* Init UART1 */
	UART_Struct *BB_UART1 = malloc(sizeof(UART_Struct));
	BB_UART1->UART_ID = UART1;
	BB_UART1->BaudRate = B9600;
	
	/* Open the UART port */
	if( !Open_UARTx(BB_UART1) )
	{
		printf("UART 1 opened!\n");
	}
	else
	{
		printf("UART 1 could not be opened\n");
	}
	
	
	Send_UARTx(BB_UART1, "HELLO THIS IS FROM BB!\n");
	
	
	char RX[30];
	for(uint8_t i =0; i < 10; i++)
	{
		Read_UARTx(BB_UART1, RX);
	}
	
	
	
	if( !Close_UARTx(BB_UART1) )
	{
		printf("UART 1 closed!\n");
		
		/* Remember to free the malloced memory */
		free(BB_UART1);
		BB_UART1 = NULL;
	}
	else
	{
		printf("UART 1 could not be closed\n");
	}
	
	printf("[%lf] TivaComm Thread: TivaComm Thread has terminated successfully and will now exit\n\n", GetCurrentTime());

	return 0;
}

