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
#include <errno.h>
#include <sys/signal.h>

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"


/* Global Variables */
extern struct sigaction UART1_RX_SignalAction;
extern char Start_RX[1];
extern UART_Struct *BB_UART1;
extern bool POLL_RX;




void UART1_RX_Signal_Handler(int Status)
{		
	if(!POLL_RX)
	{
		if( ( read(BB_UART1->fd, Start_RX, 1) ) == -1)
		{
			printf("> Error: Could not read from UART1\n");
		}

		if( (strcmp(Start_RX, START_CMD)) == 0 )
		{
			POLL_RX = true;
		}
	}
}


int8_t Init_UARTx(UART_Struct *UART)
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
		return -1; 
	}
	
	/* For UART1 we enable RX signal interruppts (Bluetooth module) */
	if(UART->UART_ID == UART1)
	{
		/* Set RX signal interruppts */
		UART1_RX_SignalAction.sa_handler = UART1_RX_Signal_Handler;
		UART1_RX_SignalAction.sa_flags = 0;
		UART1_RX_SignalAction.sa_restorer = NULL;
		sigaction(SIGIO, &UART1_RX_SignalAction, NULL);
		fcntl(UART->fd, F_SETFL, 0);
		fcntl(UART->fd, F_SETOWN, getpid());
		fcntl(UART->fd, F_SETFL,  O_ASYNC); 
	}
	
	/* Zero the structure location */
	bzero(&UART_Port, sizeof(UART_Port));

	/* Set flags */
	if( (cfsetispeed(&UART_Port, UART->BaudRate)) == -1 )
	{
		printf("> Error: Could net set input BaudRate => UART port %s\n", UART_PortNum);
	}
	if( (cfsetospeed(&UART_Port, UART->BaudRate)) == -1 )
	{
		printf("> Error: Could net set output BaudRate => UART port %s\n", UART_PortNum);
	}
	
	UART_Port.c_cflag |= CS8 | CLOCAL | CREAD;
	UART_Port.c_iflag = IGNPAR | ICRNL;
	UART_Port.c_oflag = 0;
	UART_Port.c_lflag = 0;

	/* Read function behavior */
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



int8_t Send_String_UARTx(UART_Struct *UART, char *TX_String)
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



//bool Send_StructBuffer_UARTx(UART_Struct *UART, uint8_t* StructToSend)
//{
//	/* Error handling */
//	if(UART == NULL)
//	{
//		printf("> Error: UART structure not initialized\n");
//		return false;
//	}
//	
//	uint16_t SizeOfStruct;
//
//	/* Get what structure it is, based on the first byte */
//	switch( StructToSend[0] )
//	{
//		case KE_B2T_Struct_ID:
//			Log_Msg(BB_TivaComm, "DEBUG", "STRUCTURE TO SEND IS KE_B2T_Struct", 0, LOCAL_ONLY);
//			SizeOfStruct = sizeof(KE_B2T_Struct);
//			break;
//
//
//		default:
//			Log_Msg(BB_TivaComm, "ERROR", "Send_StructBuffer_UARTx() aborted - unknown structure!", 0, LOCAL_ONLY);
//			return false;
//	}
//
//	uint16_t i;
//	int16_t BytesRead;		//Stores bytes read or error
//	char RX_BYTE[1];		//Stores the RX byte
//	
//	
//	if(write(UART->fd, StructToSend, SizeOfStruct) == -1)
//	{
//		printf("> Error: Could not send struct to UART1\n");
//		return false;
//	}
//
//	
////	
////	/* Loop for each element */
////	for(i = 0; i < SizeOfStruct; i++)
////	{
//////		/* Wait for a response => '#' before the next TX - Blocking RX poll */
//////		BytesRead = read(UART->fd,(void*)RX_BYTE, 1);
//////		
//////		/* Error check */
//////		if(BytesRead == -1)
//////		{
//////		//	Log_Msg(BB_TivaComm, "ERROR", "Could not finish sending struct to UART1! Exiting Send_Struct_UARTx()", 0, LOGGER_AND_LOCAL);
//////			return false;
//////		}
//////		else
//////		{
//////			printf("> Read '%s' from UART %u\n", RX_BYTE, UART->UART_ID);
//////		}
////		
////		/* TX one byte at a time, check for errors */
////		if(write(UART->fd, StructToSend, 1) == -1)
////		{
////		//	Log_Msg(BB_TivaComm, "ERROR", "Could not finish sending struct to UART1! Exiting Send_Struct_UARTx()", 0, LOGGER_AND_LOCAL);
////			return false;
////		}
////		else
////		{
////			/* TX was successful, increment pointer */
////			StructToSend++;
////		//	printf("> DEBUG => Sent Byte!\n");
////		}
////
////		
////	}
//	
//	return true;
//}

//bool Send_Struct_UARTx(UART_Struct *UART, TivaBB_MsgStruct StructToSend)
//{
//	/* Error handling */
//	if(UART == NULL)
//	{
//		printf("> Error: UART structure not initialized\n");
//		return false;
//	}
//	
//	/* Create an array of bytes to fit the given struct */
//	unsigned char buffer[sizeof(StructToSend)+1];
//	
//	/* Copy the contents of our struct to the char array */
//	memcpy( buffer, &StructToSend, sizeof(StructToSend) );
//	
//	/* Create a pointer that will iterate through the array and TX to Tiva side */
//	unsigned char* ptr = &buffer;
//	
//	int16_t BytesRead;		//Stores bytes read or error
//	char RX_BYTE[1];		//Stores the RX byte
//	
//	/* Loop for each element */
//	for(uint8_t i = 0; i < sizeof(StructToSend); i++)
//	{
//		/* TX one byte at a time, check for errors */
//		if(write(UART->fd, ptr, 1) == -1)
//		{
//			printf("> Error: Could not finish sending struct to UART %u\n> Exiting Send_Struct_UARTx()", UART->UART_ID);
//			return false;
//		}
//		else
//		{
//			/* TX was successful, increment pointer */
//			ptr++;
//		//	printf("> DEBUG => Sent Byte!\n");
//		}
//		
//		/* Wait for a response => '#' before the next TX - Blocking RX poll */
//		BytesRead = read(UART->fd,(void*)RX_BYTE, 1);
//		
//		/* Error check */
//		if(BytesRead == -1)
//		{
//			printf("> Error: Could not finish sending struct to UART %u\n> Exiting Send_Struct_UARTx()", UART->UART_ID);
//			return false;
//		}
//		else
//		{
//		//	printf("> Read '%s' from UART %u\n", RX_BYTE, UART->UART_ID);
//		}
//	}
//	
//	/* After all structure bytes are sent, send END => '!' */
//	if(write(UART->fd, (void *)"!", 1) == -1)
//	{
//		printf("> Error: Could not finish sending struct to UART %u\n> Exiting Send_Struct_UARTx()", UART->UART_ID);
//		return false;
//	}
//	else
//	{
//		return true;
//	}
//}



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