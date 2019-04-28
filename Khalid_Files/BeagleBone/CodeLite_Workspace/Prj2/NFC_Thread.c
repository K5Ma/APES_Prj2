/*
 * NFC_Thread.c
 *
 *  Created on: Apr 28, 2019
 *      Author: Khalid AlAwadhi
 */

#include "NFC_Thread.h"

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




void * NFCThread(void * args)
{
	/* Create the NFC Thread POSIX queue */
	mqd_t MQ;											//Message queue descriptor

	/* Delete any previous NFC POSIX Qs */
	if(mq_unlink(NFC_POSIX_Q) != 0)
	{
		Log_Msg(BB_NFC, "ERROR", "mq_unlink()", errno, LOCAL_ONLY);
	}
	else
	{
		Log_Msg(BB_NFC, "INFO", "Previous NFC queue was found and successfully unlinked!", 0, LOCAL_ONLY);
	}
	
	
	
	
//	
//	/* Initialize the queue attributes */
//	struct mq_attr attr;
//	attr.mq_flags = O_NONBLOCK;                         /* Flags: 0 or O_NONBLOCK */
//	attr.mq_maxmsg = 10;                                /* Max. # of messages on queue */
//	attr.mq_msgsize = MAX_STRUCT_SIZE;                  /* Max. message size (bytes) */
//	attr.mq_curmsgs = 0;                                /* # of messages currently in queue */
//
//
//	/* Create the NFC Thread queue to get byte structs from other pThreads
//	 * and TX them to Tiva via UART (Bluetooth) */
//	MQ = mq_open(TIVACOMM_POSIX_Q, O_CREAT | O_RDONLY | O_NONBLOCK | O_CLOEXEC, 0666, &attr);
//	if(MQ == (mqd_t) -1)
//	{
//		Log_Msg(BB_TivaComm, "FATAL", "mq_open()", errno, LOGGER_AND_LOCAL);
//	}
//	
//	
//	
//	while(1)
//	{
//		
//	}
	
	
	Log_Msg(BB_NFC, "INFO", "NFC Thread has terminated successfully and will now exit", 0, LOGGER_AND_LOCAL);
	
	return 0;
}

