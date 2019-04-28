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

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"
#include "My_UART_BB.h"


/* Global Variables */
extern UART_Struct *BB_UART1;
extern bool POLL_RX;



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
	
	
	while(1)
	{
		/* If we need to RX something: */
		if(POLL_RX)
		{
			int16_t RX_Index = -1;
			
		//	Log_Msg(BB_TivaComm, "DEBUG", "STARTING RX POLL", 0, LOCAL_ONLY);
			
			/* Create an array of bytes to fit the given struct */
			uint8_t RX_Struct_Buffer[MAX_STRUCT_SIZE];
			
			/* Create a pointer that will iterate through the array and RX from Tiva side */
			uint8_t* ptr = &RX_Struct_Buffer;
			
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
			
			RX_Struct_Buffer[RX_Index] = '\x00';
			
		//	Log_Msg(BB_TivaComm, "DEBUG", "GOT STRUCT", 0, LOCAL_ONLY);
			
			Decode_StructBuffer(RX_Struct_Buffer);
			
			POLL_RX = false;
		}

		/* Else, check if we need to TX something */
	//	else if ( (mq_receive(MQ, TX_Struct_Buffer, sizeof(MAX_STRUCT_SIZE), NULL) ) != -1 )
	//	{
			Log_Msg(BB_TivaComm, "DEBUG", "GOT STRUCT TO TX!", 0, LOCAL_ONLY);
			
			Send_String_UARTx(BB_UART1, START_CMD);                       //Send Start CMD to Tiva
			
			//NEED TO TX STRCUT HERE
			Send_StructBuffer_UARTx(UART_Struct *UART, uint8_t* StructToSend);
			
			Send_String_UARTx(BB_UART1, END_CMD);                         //Send END CMD to Tiva
			
			while(1);
//		}
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
	
	/* Create the needed structs that will store buffer contents */
	LogMsg_Struct LogMsgToSend;
			
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



		default:
			Log_Msg(BB_TivaComm, "ERROR", "Decode_StructBuffer() aborted - unknown structure!", 0, LOCAL_ONLY);
			return;
	}
}


//HUGE TEST STRUCT IF NEEDED	
//	//TEST STRUCTURE 
//	TivaBB_MsgStruct TESTSTRUCT;
//	
//	/* NFC */
//	TESTSTRUCT.T_NFC_Tag_ID_Array[0] = 2;
//	TESTSTRUCT.T_NFC_Tag_ID_Array[1] = 2; 
//	TESTSTRUCT.T_NFC_Tag_ID_Array[2] = 2; 
//	TESTSTRUCT.T_NFC_Tag_ID_Array[3] = 2; 
//	TESTSTRUCT.T_NFC_Error = true;
//	
//	/* EPaper */
//	TESTSTRUCT.T_EP_Error = true;
//	TESTSTRUCT.B_Update_EPaper = true;
//	strcpy(TESTSTRUCT.B_Image_Name, "Khalid:)");
//	
//	/* KeyPad */
//	TESTSTRUCT.T_KeyPad_Code[0] = 2;
//	TESTSTRUCT.T_KeyPad_Code[1] = 2;
//	TESTSTRUCT.T_KeyPad_Code[2] = 2;
//	TESTSTRUCT.T_KeyPad_Code[3] = 2;
//	TESTSTRUCT.T_KeyPad_Code[4] = 2;
//	TESTSTRUCT.T_KeyPad_Code[5] = 2;
//	TESTSTRUCT.T_KeyPad_Error = true;
//	TESTSTRUCT.B_KeyPad_Poll = true;
//	
//	/* Load Cell */
//	TESTSTRUCT.T_LC_SamplesArraymv[0] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[1] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[2] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[3] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[4] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[5] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[6] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[7] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[8] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[9] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[10] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[11] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[12] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[13] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[14] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[15] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[16] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[17] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[18] = 2;
//	TESTSTRUCT.T_LC_SamplesArraymv[19] = 2;
//	TESTSTRUCT.T_LC_Error = true;
//	TESTSTRUCT.B_Poll_LoadCell = true;
//	
//	/* BME280 */
//	TESTSTRUCT.T_BME280_milliTemperatureCelcius = -20;
//	TESTSTRUCT.T_BME280_milliHumidityPercent = 29;
//	TESTSTRUCT.T_BME280_Error = true;
//	
//	/* Lux */
//	TESTSTRUCT.T_Lux_Level = 2;
//	TESTSTRUCT.T_Lux_Error = true;
//	TESTSTRUCT.B_Lux_Disable = true;
//	
//	/* Gas */
//	TESTSTRUCT.T_Gas_Level = 2;
//	TESTSTRUCT.T_Gas_Error = true;
//	
//	/* PIR */
//	TESTSTRUCT.T_PIR_State = true;
//	TESTSTRUCT.B_PIR_Disable = true;
//	
//	/* SpeakJet */
//	TESTSTRUCT.T_SJ_Data = 2;
//	
//	/* Servo */
//	TESTSTRUCT.B_Servo_Open = true;
//	
//	/* Output Indicators */
//	TESTSTRUCT.B_OI_Data = 2;
	
	

