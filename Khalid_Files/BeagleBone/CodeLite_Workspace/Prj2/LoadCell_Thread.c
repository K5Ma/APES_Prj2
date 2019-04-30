/*
 * LoadCell_Thread.c
 *
 *  Created on: Apr 28, 2019
 *      Author: Khalid AlAwadhi
 */


#include "LoadCell_Thread.h"

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
OI_B2T_Struct OI_Tx;			//Sent to Tiva
//extern char Check_Code[6];
//extern bool Person_ID; //0: Poorn, 1: Khalid



void * LoadCellThread(void * args)
{
	/* Create the LoadCell Thread POSIX queue */
	mqd_t MQ;											//Message queue descriptor

	/* Delete any previous LoadCell POSIX Qs */
	if(mq_unlink(LOADCELL_POSIX_Q) != 0)
	{
		Log_Msg(BB_LoadCell, "ERROR", "mq_unlink()", errno, LOCAL_ONLY);
	}
	else
	{
		Log_Msg(BB_LoadCell, "INFO", "Previous LoadCell queue was found and successfully unlinked!", 0, LOCAL_ONLY);
	}
	
	
	/* Initialize the queue attributes */
	struct mq_attr attr;
	attr.mq_flags = 0;                                  /* Flags: 0 or O_NONBLOCK */
	attr.mq_maxmsg = 10;                                /* Max. # of messages on queue */
	attr.mq_msgsize = sizeof(LC_T2B_Struct);            /* Max. message size (bytes) */
	attr.mq_curmsgs = 0;                                /* # of messages currently in queue */


	/* Create the LoadCell Thread queue to get structs sent from Tiva */
	MQ = mq_open(LOADCELL_POSIX_Q, O_CREAT | O_RDONLY | O_CLOEXEC, 0666, &attr);
	if(MQ == (mqd_t) -1)
	{
		Log_Msg(BB_LoadCell, "FATAL", "mq_open()", errno, LOGGER_AND_LOCAL);
	}
	
	
	
	/* Struct to hold any recivied messages */
	LC_T2B_Struct Got_LC_RX;
	
	
	while(1)
	{
		/* Keep polling for LC structs */
		if ( (mq_receive(MQ, &Got_LC_RX, sizeof(LC_T2B_Struct), NULL) ) != -1 )
		{
			Log_Msg(BB_LoadCell, "DEBUG", "GOT LC STRUCT FROM TIVA!", 0, LOCAL_ONLY);
			
			
			/* Call LoadCell funciton */
			if (!LC_Function(Got_LC_RX))
			{
				Send_OIStruct_ToTiva(OI_Tx);
			}
		}
	}
	
	
	Log_Msg(BB_LoadCell, "INFO", "LoadCell Thread has terminated successfully and will now exit", 0, LOGGER_AND_LOCAL);
	
	return 0;
}



bool LC_Function(LC_T2B_Struct LC_Rx)
{
	uint8_t i, valid;
	
	for(i = 0; i < 20; i ++)
	{
        if((LC_Rx.LC_SamplesArraymv[i] >= LC_VerificationLowmv) && (LC_Rx.LC_SamplesArraymv[i] <= LC_VerificationHighmv))		valid += 1;
        else		valid = 0;
        
		if(valid >= LC_ConsecutiveVerificationNeeded)
		{
			Log_Msg(BB_LoadCell, "INFO", "Samples Verified... Stage 3 Succeeded", 0, LOGGER_AND_LOCAL);
			OI_Tx.OI_Data |= SV_Enable_Mask;
			return false;
		}	
	}
	
	Log_Msg(BB_LoadCell, "ERROR", "Samples are outside the Range... Stage 3 Failed", 0, LOGGER_AND_LOCAL);
	return true;
}



void Send_OIStruct_ToTiva(OI_B2T_Struct StructToSend)
{
	StructToSend.ID = OI_B2T_Struct_ID;
	StructToSend.Src = BB_LoadCell; 
	
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
		snprintf(ErrMsg, MSGSTR_SIZE, "Send_OIStruct_ToTiva() => mq_open(), attempted to open TivaComm queue");
		Log_Msg(BB_LoadCell, "ERROR", ErrMsg, errno, LOGGER_AND_LOCAL);
		return;
	}
	
	/* Send Struct to POSIX queue */
	if(mq_send(MQ, Buffer_Struct, sizeof(StructToSend), 0) != 0)
	{
		snprintf(ErrMsg, MSGSTR_SIZE, "Send_OIStruct_ToTiva() => mq_send(), attempted to send buffer struct to TivaComm queue");
		Log_Msg(BB_LoadCell, "ERROR", ErrMsg, errno, LOGGER_AND_LOCAL);
		return;
	}
	
	if(mq_close(MQ) != 0)
	{
		Log_Msg(BB_LoadCell, "ERROR", "Send_OIStruct_ToTiva() => mq_close(), attempted to close TivaComm queue", errno, LOGGER_AND_LOCAL);
	}
}