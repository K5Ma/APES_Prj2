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
#include <locale.h>
#include <pthread.h>

//My includes
#include "Global_Defines.h"


/* Global variables */
extern pthread_mutex_t LogLock; 	


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
	LogMsg_Struct Msg2Send =
	{
		.ID = LogMsg_Struct_ID,
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
	if(mq_send(MQ, &Msg2Send, sizeof(LogMsg_Struct), 0) != 0)
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



void SendToNFCThreadQ(NFC_T2B_Struct StructToSend)
{
//	printf("SendToNFCThreadQ DEBUG ID: %c | %u\n", StructToSend.ID, StructToSend.ID);
//	printf("SendToNFCThreadQ DEBUG SRC: %c | %u\n", StructToSend.Src, StructToSend.Src);
	
	mqd_t MQ;						//Message queue descriptor
		
	/* Open the NFC Thread POSIX queue - write only */
	MQ = mq_open(NFC_POSIX_Q, O_WRONLY | O_CLOEXEC);		//NOTE: IF YOU GET AN 'O_CLOEXEC' UNDEFINED ERROR ADD -D_GNU_SOURCE TO COMPILER OPTIONS
	
	
	char ErrMsg[MSGSTR_SIZE];								//Temp variable
	
	
	/* Error check */
	if(MQ == (mqd_t) -1)
	{
		snprintf(ErrMsg, MSGSTR_SIZE, "SendToNFCThreadQ() => mq_open(), attempted to open NFC queue");
		Log_Msg(BB_TivaComm, "ERROR", ErrMsg, errno, LOGGER_AND_LOCAL);
		return;
	}
	
	/* Send Struct to POSIX queue */
	if(mq_send(MQ, &StructToSend, sizeof(NFC_T2B_Struct), 0) != 0)
	{
		snprintf(ErrMsg, MSGSTR_SIZE, "SendToNFCThreadQ() => mq_send(), attempted to send struct to NFC queue");
		Log_Msg(BB_TivaComm, "ERROR", ErrMsg, errno, LOGGER_AND_LOCAL);
		return;
	}
	
	if(mq_close(MQ) != 0)
	{
		Log_Msg(BB_TivaComm, "ERROR", "SendToNFCThreadQ() => mq_close(), attempted to close NFC queue", errno, LOGGER_AND_LOCAL);
	}
}



void SendToKEThreadQ(KE_T2B_Struct StructToSend)
{
//	printf("SendToKEThreadQ DEBUG ID: %c | %u\n", StructToSend.ID, StructToSend.ID);
//	printf("SendToKEThreadQ DEBUG SRC: %c | %u\n", StructToSend.Src, StructToSend.Src);
//	for(uint8_t i = 0; i < 6; i ++)
//	{
//		printf("SENDQ DEBUG: %c | %u\n", StructToSend.KeyPad_Code[i], StructToSend.KeyPad_Code[i]);
//	}
	
	mqd_t MQ;						//Message queue descriptor
		
	/* Open the KeypadEpaper Thread POSIX queue - write only */
	MQ = mq_open(KEYPADEPAPER_POSIX_Q, O_WRONLY | O_CLOEXEC);		//NOTE: IF YOU GET AN 'O_CLOEXEC' UNDEFINED ERROR ADD -D_GNU_SOURCE TO COMPILER OPTIONS
	
	
	char ErrMsg[MSGSTR_SIZE];								//Temp variable
	
	
	/* Error check */
	if(MQ == (mqd_t) -1)
	{
		snprintf(ErrMsg, MSGSTR_SIZE, "SendToKEThreadQ() => mq_open(), attempted to open KE queue");
		Log_Msg(BB_TivaComm, "ERROR", ErrMsg, errno, LOGGER_AND_LOCAL);
		return;
	}
	
	/* Send Struct to POSIX queue */
	if(mq_send(MQ, &StructToSend, sizeof(KE_T2B_Struct), 0) != 0)
	{
		snprintf(ErrMsg, MSGSTR_SIZE, "SendToKEThreadQ() => mq_send(), attempted to send struct to KE queue");
		Log_Msg(BB_TivaComm, "ERROR", ErrMsg, errno, LOGGER_AND_LOCAL);
		return;
	}
	
	if(mq_close(MQ) != 0)
	{
		Log_Msg(BB_TivaComm, "ERROR", "SendToKEThreadQ() => mq_close(), attempted to close KE queue", errno, LOGGER_AND_LOCAL);
	}
}


void SendToLCThreadQ(LC_T2B_Struct StructToSend)
{
	mqd_t MQ;						//Message queue descriptor
		
	/* Open the LoadCell Thread POSIX queue - write only */
	MQ = mq_open(LOADCELL_POSIX_Q, O_WRONLY | O_CLOEXEC);		//NOTE: IF YOU GET AN 'O_CLOEXEC' UNDEFINED ERROR ADD -D_GNU_SOURCE TO COMPILER OPTIONS
	
	
	char ErrMsg[MSGSTR_SIZE];								//Temp variable
	
	
	/* Error check */
	if(MQ == (mqd_t) -1)
	{
		snprintf(ErrMsg, MSGSTR_SIZE, "SendToLCThreadQ() => mq_open(), attempted to open LC queue");
		Log_Msg(BB_TivaComm, "ERROR", ErrMsg, errno, LOGGER_AND_LOCAL);
		return;
	}
	
	/* Send Struct to POSIX queue */
	if(mq_send(MQ, &StructToSend, sizeof(LC_T2B_Struct), 0) != 0)
	{
		snprintf(ErrMsg, MSGSTR_SIZE, "SendToLCThreadQ() => mq_send(), attempted to send struct to LC queue");
		Log_Msg(BB_TivaComm, "ERROR", ErrMsg, errno, LOGGER_AND_LOCAL);
		return;
	}
	
	if(mq_close(MQ) != 0)
	{
		Log_Msg(BB_TivaComm, "ERROR", "SendToLCThreadQ() => mq_close(), attempted to close LC queue", errno, LOGGER_AND_LOCAL);
	}
}

//void SendToTivaCommThreadQ(uint8_t Buffer[], uint8_t StructSize)
//{
//	
//	Log_Msg(BB_NFC, "DEBUG", " REACHED SendToTivaCommThreadQ!", 0, LOCAL_ONLY);
//	
//	printf("sizeof buffer: %d\n\n", StructSize);
//	
//	mqd_t MQ;						//Message queue descriptor
//		
//	/* Open the TivaComm Thread POSIX queue - write only */
//	MQ = mq_open(TIVACOMM_POSIX_Q, O_WRONLY | O_CLOEXEC);		//NOTE: IF YOU GET AN 'O_CLOEXEC' UNDEFINED ERROR ADD -D_GNU_SOURCE TO COMPILER OPTIONS
//	
//	
//	char ErrMsg[MSGSTR_SIZE];								//Temp variable
//	
//	
//	/* Error check */
//	if(MQ == (mqd_t) -1)
//	{
//		snprintf(ErrMsg, MSGSTR_SIZE, "SendToTivaCommThreadQ() => mq_open(), attempted to open TivaComm queue");
//		Log_Msg(BB_NFC, "ERROR", ErrMsg, errno, LOGGER_AND_LOCAL);
//		return;
//	}
//	
//	/* Send Struct to POSIX queue */
//	if(mq_send(MQ, &Buffer, StructSize, 0) != 0)
//	{
//		snprintf(ErrMsg, MSGSTR_SIZE, "SendToTivaCommThreadQ() => mq_send(), attempted to send buffer struct to TivaComm queue");
//		Log_Msg(BB_NFC, "ERROR", ErrMsg, errno, LOGGER_AND_LOCAL);
//		return;
//	}
//	
//	if(mq_close(MQ) != 0)
//	{
//		Log_Msg(BB_NFC, "ERROR", "SendToTivaCommThreadQ() => mq_close(), attempted to close TivaComm queue", errno, LOGGER_AND_LOCAL);
//	}
//}



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
		/* Get error via a thread-safe function which is strerror_l(). 	
		 * Create locale */
		locale_t locale = newlocale(LC_CTYPE_MASK|LC_NUMERIC_MASK|LC_TIME_MASK|
                                    LC_COLLATE_MASK|LC_MONETARY_MASK|LC_MESSAGES_MASK,
                                    "",(locale_t)0);
		
		/* Combine user message with the strerror */
		snprintf(Output_Log, MSGSTR_SIZE, "%s: %s", OutMsg, strerror_l(errnum, locale));	
	}
	else
	{
		/* Just copy the user message */
		strcpy(Output_Log, OutMsg);
	}
	
	/* Temp variable to store current date/time */
	char CurTime[TIMESTR_SIZE]; 
			
	
	/* LOCK */
	pthread_mutex_lock(&LogLock);
			
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
	
	/* UNLOCK */
	pthread_mutex_unlock(&LogLock);
}




