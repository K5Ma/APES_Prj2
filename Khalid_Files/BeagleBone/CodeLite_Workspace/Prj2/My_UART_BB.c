/*
 * My_UART_BB.h
 *
 *  Created on: Apr 20, 2019
 *      Author: Khalid AlAwadhi
 *
 *  # ALL INFO CAN BE FOUND IN THE .h FILE #
 */
#include "My_UART_BB.h"

//Standard includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

//My includes



int8_t Open_UARTx(UART_Struct *UART)
{
	/* Error handling */
	if(UART == NULL)
	{
		printf("> Error: UART structure not initialized\n");
		return -1; 
	}
	if(UART->UART_ID > 3)
	{
		printf("> Error: UART port must be 0-3\n");
		return -1;
	}
	
	/* Get chosen UART path */
	char UART_Path[13] = "/dev/ttyO";
	char UART_PortNum[2];
	snprintf(UART_PortNum, 2, "%u", UART->UART_ID);
	strcat(UART_Path, UART_PortNum);
	
	struct termios UART_Port;
	
	/* Open UART port */
	UART->fd = open(UART_Path, O_RDWR | O_NOCTTY);
	if(UART->fd < 0)
	{
		printf("> Error: Failed to open UART port %s\n", UART_PortNum);	
	}
	
	/* Zero the structure location */
	bzero(&UART_Port, sizeof(UART_Port));

	/* Set flags */
	UART_Port.c_cflag = UART->BaudRate | CS8 | CLOCAL | CREAD;
	UART_Port.c_iflag = IGNPAR | ICRNL;
	UART_Port.c_oflag = 0;
	UART_Port.c_lflag = 0;

	UART_Port.c_cc[VTIME] = 0;
	UART_Port.c_cc[VMIN]  = 1;

	/* Clean the line and set the attributes */
	tcflush(UART->fd, TCIFLUSH);
	tcsetattr(UART->fd, TCSANOW, &UART_Port);
	return 0;
}



int8_t Close_UARTx(UART_Struct *UART)
{
	/* Error handling */
	if(UART == NULL)
	{
		printf("> Error: UART structure not initialized\n");
		return -1;
	}
	
	close(UART->fd);
	return 0;
}



int8_t Send_UARTx(UART_Struct *UART, char *TX_String)
{
	/* Error handling */
	if(UART == NULL)
	{
		printf("> Error: UART structure not initialized\n");
		return -1;
	}
	
	if(write(UART->fd, TX_String, strlen(TX_String)) == -1)
	{
		printf("> Error: Could not write '%s' to UART %u\n", TX_String, UART->UART_ID);
		return -1;
	}
	return 0;
}



int16_t Read_UARTx(UART_Struct *UART, char *RX_String)
{
	/* Error handling */
	if(UART == NULL)
	{
		printf("> Error: UART structure not initialized\n");
		return -1;
	}
	
	int16_t BytesRead;
	while(1)
	{
		BytesRead = read(UART->fd,(void*)RX_String, strlen(RX_String));
		
		if(BytesRead == -1)
		{
			printf("> Error: Could not read from UART %u\n", UART->UART_ID);
			return -1;
		}
		
		for(uint8_t i = 0; i < strlen(RX_String); i++)
		{
			if(RX_String[i] == '\n')
			{
				RX_String[i] = '\0';
				printf("Read '%s' from UART %u\n", RX_String, UART->UART_ID);
				return BytesRead;
			}
		}
	}
}