/*
 * Master_Functions.c
 *
 *  Created on: Apr 20, 2019
 *      Author: Khalid AlAwadhi
 */
 #include "Master_Functions.h"
 
//Standard includes
#include <sys/time.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>



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



void GetRealTime(char* TimeStr, uint8_t StrLen)
{
	/* Error handling: the user must make sure the length of the 
	 * given string is at least 26 as the function asctime_r()
	 * below does not care and will overwrite adjacent array locations
	 * if the size does not fit and might cause bugs.. or worse */
	if(StrLen < 26)
	{
		return;
	}
	time_t RawTime;
	struct tm TimeInfo;
	
	/* Get raw time from system */
	time(&RawTime);
	
	/* Convert time_t to tm* */
	localtime_r(&RawTime, &TimeInfo);
	
	/* Store the current system time in the given string */
	asctime_r(&TimeInfo, TimeStr);
	
	/* Format the string by removing the \n and the year */
	TimeStr[25] = '\x00';
	TimeStr[24] = '\x00';
	TimeStr[23] = '\x00';
	TimeStr[22] = '\x00';
	TimeStr[21] = '\x00';
	TimeStr[20] = '\x00';
	TimeStr[19] = '\x00';
}



void SendToThreadQ(uint8_t Src, uint8_t Dst, char* Log, char* Message)
{
	POSIX_MsgStruct Msg2Send =
	{
		.Source = Src,
		.Dest = Dst
	};
	strcpy(Msg2Send.LogLevel, Log);
	strcpy(Msg2Send.Msg, Message);
	
	mqd_t MQ;						//Message queue descriptor
	
	char DEST_Q_NAME[16];			//This will store the name of destination queue
	
	/* Check what is the destination pThread */
	switch(Msg2Send.Dest)
	{
		case Logger:
			strcpy(DEST_Q_NAME, LOGGER_QUEUE);
			break;

		default:
			Msg2Send.Dest = Logger;
			strcpy(DEST_Q_NAME, LOGGER_QUEUE);
			char *text1 = "WARNING - No destination thread for this msg!";
			strcpy(Msg2Send.LogLevel, text1);
			break;
	}
	
	/* Open the chosen Thread POSIX queue - write only */
	MQ = mq_open(DEST_Q_NAME, O_WRONLY | O_CLOEXEC);		//NOTE: IF YOU GET AN 'O_CLOEXEC' UNDEFINED ERROR ADD -D_GNU_SOURCE TO COMPILER OPTIONS
	
	
	char ErrMsg[MSGSTR_SIZE];								//Temp variable
	
	/* Error check */
	if(MQ == (mqd_t) -1)
	{
		snprintf(ErrMsg, MSGSTR_SIZE, "SendToThreadQ() => mq_open(), attempted to open '%u' queue, called by thread '%u'", Msg2Send.Dest, Msg2Send.Source);
		Log_Msg(0, Msg2Send.LogLevel, ErrMsg, errno, LOCAL_ONLY);
	}
	
	/* Send Msg to POSIX queue */
	if(mq_send(MQ, &Msg2Send, sizeof(POSIX_MsgStruct), 0) != 0)
	{
		snprintf(ErrMsg, MSGSTR_SIZE, "SendToThreadQ() => mq_send(), attempted to send message '%s' from '%u' to '%u'", Msg2Send.Msg, Msg2Send.Source, Msg2Send.Dest);
		Log_Msg(0, Msg2Send.LogLevel, ErrMsg, errno, LOCAL_ONLY);
	}
	
	if(mq_close(MQ) != 0)
	{
		snprintf(ErrMsg, MSGSTR_SIZE, "SendToThreadQ() => mq_close(), attempted to close '%u' queue", Msg2Send.Source);
		Log_Msg(0, Msg2Send.LogLevel, ErrMsg, errno, LOCAL_ONLY);
	}
}



void Log_Msg(uint8_t Src, char* LogLvl, char* OutMsg, int errnum, uint8_t Mode)
{
	/* Get name of source */
	char Source_text[20]; 
	switch(Src)
	{
		case Main:
			strcpy(Source_text, "Main Thread");
			break;

		case Logger:
			strcpy(Source_text, "Logger Thread");
			break;
			
		case TivaComm:
			strcpy(Source_text, "TivaComm Thread");
			break;

		default:
			strcpy(Source_text, "Unknown Thread");
			break;
	}
	
	/* This will store the final output message */
	char Output_Log[MSGSTR_SIZE];
	
	/* If the errno is not 0, that means we are logging an error */
	if(errnum != 0)
	{
		char ErrMsg_strerror[MSGSTR_SIZE];										//Stores the strerror_r error message
	
		strerror_r(errnum, ErrMsg_strerror, MSGSTR_SIZE);						//Get error via a thread-safe function
	
		snprintf(Output_Log, MSGSTR_SIZE, "%s: %s", OutMsg, ErrMsg_strerror);	//Combine user message with the strerror
	}
	else
	{
		/* Just copy the user message */
		strcpy(Output_Log, OutMsg);
	}
	
	/* Temp variable to store current date/time */
	char CurTime[TIMESTR_SIZE]; 
			
	/* Output message depending on chosen mode */
	switch(Mode)
	{
		case LOGGER_ONLY:
			SendToThreadQ(Src, Logger, LogLvl, Output_Log);
			break;

		case LOGGER_AND_LOCAL:
			GetRealTime(CurTime, TIMESTR_SIZE);
			printf("> [%s] '%s' (%s) => %s\n\n", CurTime, Source_text, LogLvl, Output_Log);
			SendToThreadQ(Src, Logger, LogLvl, Output_Log);
			break;
			
		default:
			GetRealTime(CurTime, TIMESTR_SIZE);
			printf("> [%s] '%s' (%s) => %s\n\n", CurTime, Source_text, LogLvl, Output_Log);
			break;
	}
}
