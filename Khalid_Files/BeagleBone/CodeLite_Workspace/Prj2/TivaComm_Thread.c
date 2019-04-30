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
#include <mqueue.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"
#include "My_UART_BB.h"


/* Global Variables */
extern UART_Struct *BB_UART1;
extern bool POLL_RX;
extern pthread_mutex_t TXLock; 
extern pthread_mutex_t RXLock; 


void * TivaCommThread(void * args)
{
	/* Create the TivaComm Thread POSIX queue */
	mqd_t MQ;											//Message queue descriptor

	/* Delete any previous TivaComm POSIX Qs */
	if(mq_unlink(TIVACOMM_POSIX_Q) != 0)
	{
		Log_Msg(BB_TivaComm, "ERROR", "mq_unlink()", errno, LOCAL_ONLY);
	}
	else
	{
		Log_Msg(BB_TivaComm, "INFO", "Previous TivaComm queue was found and successfully unlinked!", 0, LOCAL_ONLY);
	}
	
	
	/* Initialize the queue attributes */
	struct mq_attr attr;
	attr.mq_flags = O_NONBLOCK;                         /* Flags: 0 or O_NONBLOCK */
	attr.mq_maxmsg = 10;                                /* Max. # of messages on queue */
	attr.mq_msgsize = MAX_STRUCT_SIZE;                  /* Max. message size (bytes) */
	attr.mq_curmsgs = 0;                                /* # of messages currently in queue */


	/* Create the TivaComm Thread queue to get byte structs from other pThreads
	 * and TX them to Tiva via UART (Bluetooth) */
	MQ = mq_open(TIVACOMM_POSIX_Q, O_CREAT | O_RDONLY | O_NONBLOCK | O_CLOEXEC, 0666, &attr);
	if(MQ == (mqd_t) -1)
	{
		Log_Msg(BB_TivaComm, "FATAL", "mq_open()", errno, LOGGER_AND_LOCAL);
	}
	
	/* Init UART1 */
	BB_UART1 = malloc(sizeof(UART_Struct));
	BB_UART1->UART_ID = UART1;
	BB_UART1->BaudRate = B9600;
	
	/* Open the UART port */
	if( !Init_UARTx(BB_UART1) )
	{
		Log_Msg(BB_TivaComm, "INFO", "UART1 port initialized successfully", 0, LOGGER_AND_LOCAL);
	}
	else
	{
		Log_Msg(BB_TivaComm, "FATAL", "UART1 port could not be initialized!", 0, LOGGER_AND_LOCAL);
	}
	
	
	/* Create an array of bytes to fit any given struct */
	uint8_t TX_Struct_Buffer[MAX_STRUCT_SIZE];
	
	/* Used to keep track of the RX array */
	int16_t RX_Index = -1;
			
	/* Create a pointer that will iterate through the array and RX from Tiva side */
	uint8_t* ptr = NULL;

	
	while(1)
	{
		/* If we need to RX something: */
		if(POLL_RX)
		{
			Log_Msg(BB_TivaComm, "DEBUG", "STARTING RX POLL", 0, LOCAL_ONLY);
			
			/* LOCK */
			pthread_mutex_lock(&RXLock);
			
			/* Create an array of bytes to fit the given struct */
			uint8_t RX_Struct_Buffer[MAX_STRUCT_SIZE];
			
			/* Reset */
			RX_Index = -1;
			ptr = &RX_Struct_Buffer;
			
			/* Keep adding to the buffer until we get a "!" */
			do
			{				
				/* Block and get byte */
				if(read(BB_UART1->fd, ptr, 1) == -1)
				{
					printf("> Error: Could not read byte from UART1\n");
				}
				else
				{
					/* RX was successful, increment pointer */
					ptr++;
					RX_Index++;
				}
			} while ( RX_Struct_Buffer[RX_Index] != END_CMD_CHAR );
			

//			for(uint8_t i = 0; i < RX_Index; i++)
//			{
//				printf("DEBUG: RX_Struct_Buffer[%u]: %u | %c\n", i, RX_Struct_Buffer[i], RX_Struct_Buffer[i]);
//			}


			/* UNLOCK */
			pthread_mutex_unlock(&RXLock);
				
			
			RX_Struct_Buffer[RX_Index] = '\x00';
			
			Log_Msg(BB_TivaComm, "DEBUG", "GOT STRUCT", 0, LOCAL_ONLY);
			
			Decode_StructBuffer(RX_Struct_Buffer);
			
			POLL_RX = false;
		}
		
		/* Else, check if we need to TX something */
		else
		{
			if ( (mq_receive(MQ, TX_Struct_Buffer, MAX_STRUCT_SIZE, NULL) ) != -1 )
			{
			//	Log_Msg(BB_TivaComm, "DEBUG", "SENDING STRUCT TO TIVA", 0, LOCAL_ONLY);
				
				/* LOCK */
				pthread_mutex_lock(&TXLock);
	
				Send_String_UARTx(BB_UART1, START_CMD);                       //Send Start CMD to Tiva
				
				Send_String_UARTx(BB_UART1, (char *)TX_Struct_Buffer);        //Sending struct buffer 
				
				Send_String_UARTx(BB_UART1, END_CMD);                         //Send END CMD to Tiva
				Send_String_UARTx(BB_UART1, END_CMD);                         //Send END CMD to Tiva
				Send_String_UARTx(BB_UART1, END_CMD);                         //Send END CMD to Tiva
				Send_String_UARTx(BB_UART1, END_CMD);                         //Send END CMD to Tiva
				
				/* UNLOCK */
				pthread_mutex_unlock(&TXLock);
			}
		}

	}
	
	
	if( !Close_UARTx(BB_UART1) )
	{
		Log_Msg(BB_TivaComm, "INFO", "UART1 port closed successfully", 0, LOGGER_AND_LOCAL);
		
		/* Remember to free the malloced memory */
		free(BB_UART1);
		BB_UART1 = NULL;
	}
	else
	{
		Log_Msg(BB_TivaComm, "ERROR", "UART1 port could not be closed!", 0, LOGGER_AND_LOCAL);
	}
	
	Log_Msg(BB_TivaComm, "INFO", "TivaComm Thread has terminated successfully and will now exit", 0, LOGGER_AND_LOCAL);
	
	return 0;
}


void Decode_StructBuffer(uint8_t* StructToDecode)
{
	char TmpTxt[100];
	
	/* Create the needed structs that will store buffer contents */
	LogMsg_Struct LogMsgToSend;
	NFC_T2B_Struct NFC_T2BToSend;
	KE_T2B_Struct KE_T2BToSend;
	LC_T2B_Struct LC_T2BToSend;
			
			
	/* Get what structure it is, based on the first byte */
	switch( StructToDecode[0] )
	{
		case LogMsg_Struct_ID:
			//Log_Msg(BB_TivaComm, "DEBUG", "STRUCTURE TO DECODE IS LOGMSG", 0, LOCAL_ONLY);
			
			/* Copy the contents of the buffer to the struct */
			memcpy(&LogMsgToSend , StructToDecode, sizeof(LogMsg_Struct));
			
			/* Send to Logger POSIX Q */
			Log_Msg(LogMsgToSend.Src, LogMsgToSend.LogLevel, LogMsgToSend.Msg, 0, LOGGER_AND_LOCAL);
			break;
			

		case NFC_T2B_Struct_ID:
		//	Log_Msg(BB_TivaComm, "DEBUG", "STRUCTURE TO DECODE IS NFC_T2B", 0, LOCAL_ONLY);
			
			/* Copy the contents of the buffer to the struct */
			memcpy(&NFC_T2BToSend , StructToDecode, sizeof(NFC_T2B_Struct));
			
			/* Send to NFC POSIX Q */
			SendToNFCThreadQ(NFC_T2BToSend);
			break;
			
			
		case KE_T2B_Struct_ID:
		//	Log_Msg(BB_TivaComm, "DEBUG", "STRUCTURE TO DECODE IS KE_T2B_Struct", 0, LOCAL_ONLY);
			
			/* Copy the contents of the buffer to the struct */
			memcpy(&KE_T2BToSend , StructToDecode, sizeof(KE_T2B_Struct));
	
			/* Send to KeypadEpaper POSIX Q */
			SendToKEThreadQ(KE_T2BToSend);
			break;
			
			
		case LC_T2B_Struct_ID:
//			Log_Msg(BB_TivaComm, "DEBUG", "STRUCTURE TO DECODE IS LC_T2B_Struct", 0, LOCAL_ONLY);
			
			/* Copy the contents of the buffer to the struct */
			memcpy(&LC_T2BToSend , StructToDecode, sizeof(LC_T2B_Struct));
	
			/* Send to LoadCell POSIX Q */
			SendToLCThreadQ(LC_T2BToSend);
			break;



		default:
			snprintf(TmpTxt, 100, "Decode_StructBuffer() aborted - unknown structure! ID: %u | Src: %u", StructToDecode[0], StructToDecode[1]);
			Log_Msg(BB_TivaComm, "ERROR", TmpTxt, 0, LOCAL_ONLY);
			return;
	}
}

