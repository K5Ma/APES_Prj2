/*
 * Master_Functions.c
 *
 *  Created on: Apr 20, 2019
 *      Author: Khalid AlAwadhi
 */
//Standard includes
#include <sys/time.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <errno.h>


//My includes
#include "Global_Defines.h"



double GetCurrentTime()
{
	/* Declaring structure for time */
	struct timeval time;
	
	/* Get current time and save it */
	gettimeofday(&time, 0);
	
	/* Combine the Secs with uSecs by typecasting
     * Long Int to Double and return as 1 Double value */
	return (double)(time.tv_sec)+(((double)(time.tv_usec))/1000000);
}


//
//void SendToThreadQ(uint8_t Src, uint8_t Dst, char* Log, char* Message)
//{
//	MsgStruct Msg2Send =
//	{
//		.Source = Src,
//		.Dest = Dst
//	};
//	strcpy(Msg2Send.LogLevel, Log);
//	strcpy(Msg2Send.Msg, Message);
//	
//	mqd_t MQ;						//Message queue descriptor
//	
//	char *DEST_Q_NAME;				//This will store the name of destination queue
//	
//	/* Check what is the destination pThread */
//	switch(Msg2Send.Dest)
//	{
//		case Main:
//			DEST_Q_NAME = MAIN_QUEUE;
//			break;
//
//		case Logging:
//			DEST_Q_NAME = LOGGING_QUEUE;
//			break;
//
//		default:
//			Msg2Send.Dest = Logging;
//			DEST_Q_NAME = LOGGING_QUEUE;
//			char *text1 = "WARNING - No destination thread for this msg!";
//			strcpy(Msg2Send.LogLevel, text1);
//			break;
//	}
//	
//	/* Open the chosen Thread POSIX queue - write only */
//	MQ = mq_open(DEST_Q_NAME, O_WRONLY | O_CLOEXEC);
//	
//	char ErrMsg[250];								//Temp variable
//	
//	/* Error check */
//	if(MQ == (mqd_t) -1)
//	{
//		sprintf(ErrMsg, "SendToThreadQ() => mq_open(), attempted to open '%u' queue, called by Thread '%u'", Msg2Send.Dest, Msg2Send.Source);
//		Log_error(0, ErrMsg, errno, LOCAL_ONLY);
//	}
//	
//	/* Send Msg to POSIX queue */
//	if(mq_send(MQ, &Msg2Send, sizeof(MsgStruct), 0) != 0)
//	{
//		sprintf(ErrMsg, "SendToThreadQ() => mq_send(), attempted to send message '%s' from '%u' to '%u'", Msg2Send.Msg, Msg2Send.Source, Msg2Send.Dest);
//		Log_error(0, ErrMsg, errno, LOCAL_ONLY);
//	}
//	
//	if(mq_close(MQ) != 0)
//	{
//		sprintf(ErrMsg, "SendToThreadQ() => mq_close(), attempted to close '%u' queue", Msg2Send.Source);
//		Log_error(0, ErrMsg, errno, LOCAL_ONLY);
//	}
//}
//
//
//
//void Log_error(uint8_t Src, char* Err_Msg, int errnum, uint8_t SendToLogging)
//{
//	char Error_Log[150];										//Stores the complete error message
//	char ErrMsg_strerror[150];									//Stores the strerror_r error message
//	
//	strerror_r(errnum, ErrMsg_strerror, 150);					//Get error via a thread-safe function
//	
//	sprintf(Error_Log, "%s: %s", Err_Msg, ErrMsg_strerror);		//Combine user message with the strerror
//	
//	
//	/* Get name of source */
//	char* Source_text; 
//	switch(Src)
//	{
//		case Main:
//			Source_text = "Main Thread";
//			break;
//
//		case Logging:
//			Source_text = "Logging Thread";
//			break;
//
//		default:
//			Source_text = "Unknown Thread";
//			break;
//	}
//			
//			
//	/* Output error depending on chosen mode */
//	switch(SendToLogging)
//	{
//		case LOGGING_ONLY:
//			SendToThreadQ(Src, Logging, "ERROR", Error_Log);
//			break;
//
//		case LOGGING_AND_LOCAL:
//			printf("[%lf] Error in Thread '%s' => %s\n\n", GetCurrentTime(), Source_text, Error_Log);
//			SendToThreadQ(Src, Logging, "ERROR", Error_Log);
//			break;
//			
//		default:
//			printf("[%lf] Error in Thread '%s' => %s\n\n", GetCurrentTime(), Source_text, Error_Log);
//			break;
//	}
//}
