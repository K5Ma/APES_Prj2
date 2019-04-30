/*
 * KeypadEpaper_Thread.c
 *
 *  Created on: Apr 28, 2019
 *      Author: Khalid AlAwadhi
 */

#include "KeypadEpaper_Thread.h"

//Standard includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <errno.h>

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"



/* Global Variables */
LC_B2T_Struct LC_Tx;			//Sent to Tiva if applicable
extern char Check_Code[6];
extern bool Person_ID; //0: Poorn, 1: Khalid


void * KeypadEpaperThread(void * args)
{
	/* Create the KeypadEpaper Thread POSIX queue */
	mqd_t MQ;											//Message queue descriptor

	/* Delete any previous KeypadEpaper POSIX Qs */
	if(mq_unlink(KEYPADEPAPER_POSIX_Q) != 0)
	{
		Log_Msg(BB_KeypadEpaper, "ERROR", "mq_unlink()", errno, LOCAL_ONLY);
	}
	else
	{
		Log_Msg(BB_KeypadEpaper, "INFO", "Previous KeypadEpaper queue was found and successfully unlinked!", 0, LOCAL_ONLY);
	}
	
	
	/* Initialize the queue attributes */
	struct mq_attr attr;
	attr.mq_flags = 0;                                  /* Flags: 0 or O_NONBLOCK */
	attr.mq_maxmsg = 10;                                /* Max. # of messages on queue */
	attr.mq_msgsize = sizeof(KE_T2B_Struct);            /* Max. message size (bytes) */
	attr.mq_curmsgs = 0;                                /* # of messages currently in queue */


	/* Create the KeypadEpaper Thread queue to get structs sent from Tiva */
	MQ = mq_open(KEYPADEPAPER_POSIX_Q, O_CREAT | O_RDONLY | O_CLOEXEC, 0666, &attr);
	if(MQ == (mqd_t) -1)
	{
		Log_Msg(BB_KeypadEpaper, "FATAL", "mq_open()", errno, LOGGER_AND_LOCAL);
	}
	
	/* Struct to hold any recivied messages */
	KE_T2B_Struct Got_KET2B_RX;
	
	while(1)
	{
		/* Keep polling for KE structs */
		if ( (mq_receive(MQ, &Got_KET2B_RX, sizeof(KE_T2B_Struct), NULL) ) != -1 )
		{
			Log_Msg(BB_KeypadEpaper, "DEBUG", "GOT KE_T2B STRUCT FROM TIVA!", 0, LOCAL_ONLY);
			
			
			/* Call EK funciton */
			if (!EK_Function(Got_KET2B_RX))
			{
				Send_LCStruct_ToTiva(LC_Tx);
			}
		}
	}
	
	
	Log_Msg(BB_KeypadEpaper, "INFO", "KeypadEpaper Thread has terminated successfully and will now exit", 0, LOGGER_AND_LOCAL);
	
	return 0;
}



bool EK_Function(KE_T2B_Struct EK_Rx)
{
//	printf("\n------>>>>>> Code Was Set to: %c%c%c%c%c%c\n", Check_Code[0], Check_Code[1], Check_Code[2], Check_Code[3], Check_Code[4], Check_Code[5]);
	
	uint8_t i;
	
	Log_Msg(BB_KeypadEpaper, "INFO", "Checking Code...", 0, LOGGER_AND_LOCAL);
	
//	for(i = 0; i < 6; i ++)
//	{
//		printf("DEBUG: %c | %u", EK_Rx.KeyPad_Code[i], EK_Rx.KeyPad_Code[i]);
//	}
//	
	
//	printf("\n------>>>>>> Actual Reception is:");
	
	for(i = 0; i < 6; i ++)
	{
	//	printf("%c",EK_Rx.KeyPad_Code[i]);
		if(EK_Rx.KeyPad_Code[i] != Check_Code[i])		break;
	}
		
	if(i != 6)
	{
		Log_Msg(BB_KeypadEpaper, "ERROR", "Incorrect Code Entered... Stage 2 Failed", 0, LOGGER_AND_LOCAL);
		return true;
	}
	else
	{
		LC_Tx.LC_Poll = true;
		Log_Msg(BB_KeypadEpaper, "INFO", "Code Verified... Stage 2 Succeeded", 0, LOGGER_AND_LOCAL);
		return false;
	}
}



void Send_LCStruct_ToTiva(LC_B2T_Struct StructToSend)
{
	StructToSend.ID = LC_B2T_Struct_ID;
	StructToSend.Src = BB_KeypadEpaper; 
	
	/* Create an array of bytes to fit the given struct */
	uint8_t Buffer_Struct[sizeof(StructToSend)+1];

	/* Copy the contents of our struct to the char array */
	memcpy(Buffer_Struct, &StructToSend, sizeof(StructToSend));


	/* Send Struct to TivaComm POSIX Q */
	mqd_t MQ;						//Message queue descriptor
		
	/* Open the TivaComm Thread POSIX queue - write only */
	MQ = mq_open(TIVACOMM_POSIX_Q, O_WRONLY | O_CLOEXEC);		//NOTE: IF YOU GET AN 'O_CLOEXEC' UNDEFINED ERROR ADD -D_GNU_SOURCE TO COMPILER OPTIONS
	
	
	char ErrMsg[MSGSTR_SIZE];								//Temp variable
	
	
	/* Error check */
	if(MQ == (mqd_t) -1)
	{
		snprintf(ErrMsg, MSGSTR_SIZE, "Send_LCStruct_ToTiva() => mq_open(), attempted to open TivaComm queue");
		Log_Msg(BB_KeypadEpaper, "ERROR", ErrMsg, errno, LOGGER_AND_LOCAL);
		return;
	}
	
	/* Send Struct to POSIX queue */
	if(mq_send(MQ, Buffer_Struct, sizeof(StructToSend), 0) != 0)
	{
		snprintf(ErrMsg, MSGSTR_SIZE, "Send_LCStruct_ToTiva() => mq_send(), attempted to send buffer struct to TivaComm queue");
		Log_Msg(BB_KeypadEpaper, "ERROR", ErrMsg, errno, LOGGER_AND_LOCAL);
		return;
	}
	
	if(mq_close(MQ) != 0)
	{
		Log_Msg(BB_KeypadEpaper, "ERROR", "Send_LCStruct_ToTiva() => mq_close(), attempted to close TivaComm queue", errno, LOGGER_AND_LOCAL);
	}
}
