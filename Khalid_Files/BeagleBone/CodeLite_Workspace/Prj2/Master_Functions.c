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



void EnumtoString(uint8_t EnumNum, char* Str)
{
	/* Store name based on enum */
	switch(EnumNum)
	{
		/* Tiva Sources */
		case T_Main:
			strcpy(Str, "Main Task");
			break;

		case T_BBComm:
			strcpy(Str, "BBComm Task");
			break;
			
		case T_NFC:
			strcpy(Str, "NFC Task");
			break;
			
		case T_KeypadEpaper:
			strcpy(Str, "KeypadEpaper Task");
			break;

		case T_LoadCell:
			strcpy(Str, "LoadCell Task");
			break;

		case T_Lux:
			strcpy(Str, "Lux Task");
			break;
			
		case T_Servo:
			strcpy(Str, "Servo Task");
			break;
			
		case T_SpeakJet:
			strcpy(Str, "SpeakJet Task");
			break;
			
			case T_Outputs:
			strcpy(Str, "Outputs Task");
			break;
		
		/* BeagleBone Sources */
		case BB_Main:
			strcpy(Str, "Main Thread");
			break;
			
		case BB_Logger:
			strcpy(Str, "Logger Thread");
			break;
			
		case BB_TivaComm:
			strcpy(Str, "TivaComm Thread");
			break;
			
		case BB_NFC:
			strcpy(Str, "NFC Thread");
			break;
			
		case BB_KeypadEpaper:
			strcpy(Str, "KeypadEpaper Thread");
			break;

		case BB_LoadCell:
			strcpy(Str, "LoadCell Thread");
			break;

		default:
			strcpy(Str, "Unknown Thread/Task");
			break;
	}
}



void SendToLoggerThreadQ(uint8_t Src, char* Log, char* Message)
{
	Log_MsgStruct Msg2Send =
	{
		.ID = 1,
		.Src = Src
	};
	strcpy(Msg2Send.LogLevel, Log);
	strcpy(Msg2Send.Msg, Message);
	
	mqd_t MQ;						//Message queue descriptor
		
	/* Open the Logger Thread POSIX queue - write only */
	MQ = mq_open(LOGGER_POSIX_Q, O_WRONLY | O_CLOEXEC);		//NOTE: IF YOU GET AN 'O_CLOEXEC' UNDEFINED ERROR ADD -D_GNU_SOURCE TO COMPILER OPTIONS
	
	
	char ErrMsg[MSGSTR_SIZE];								//Temp variable
	
	
	/* Error check */
	if(MQ == (mqd_t) -1)
	{
		snprintf(ErrMsg, MSGSTR_SIZE, "SendToLoggerThreadQ() => mq_open(), attempted to open Logger queue");
		Log_Msg(Msg2Send.Src, "ERROR", ErrMsg, errno, LOCAL_ONLY);
		return;
	}
	
	/* Send Msg to POSIX queue */
	if(mq_send(MQ, &Msg2Send, sizeof(Log_MsgStruct), 0) != 0)
	{
		snprintf(ErrMsg, MSGSTR_SIZE, "SendToLoggerThreadQ() => mq_send(), attempted to send message '%s' to Logger queue", Msg2Send.Msg);
		Log_Msg(Msg2Send.Src, "ERROR", ErrMsg, errno, LOCAL_ONLY);
		return;
	}
	
	if(mq_close(MQ) != 0)
	{
		Log_Msg(Msg2Send.Src, "ERROR", "SendToLoggerThreadQ() => mq_close(), attempted to close Logger queue", errno, LOCAL_ONLY);
	}
}



void Log_Msg(uint8_t Src, char* LogLvl, char* OutMsg, int errnum, uint8_t Mode)
{
	/* Get name of source */
	char Source_text[SRC_SIZE]; 
	EnumtoString(Src, Source_text);
	
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
			SendToLoggerThreadQ(Src, LogLvl, Output_Log);
			break;

		case LOGGER_AND_LOCAL:
			GetRealTime(CurTime, TIMESTR_SIZE);
			printf("> [%s] Log Event(%s): %s\n> L->Source: %s\n\n", CurTime, LogLvl, Output_Log, Source_text);
			SendToLoggerThreadQ(Src, LogLvl, Output_Log);
			break;
			
		default:
			GetRealTime(CurTime, TIMESTR_SIZE);
			printf("> [%s] Log Event(%s): %s\n> L->Source: %s\n\n", CurTime, LogLvl, Output_Log, Source_text);
			break;
	}
}
