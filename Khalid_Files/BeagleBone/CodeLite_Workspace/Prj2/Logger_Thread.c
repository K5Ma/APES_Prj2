/*
 * Logger_Thread.c
 *
 *  Created on: Apr 21, 2019
 *      Author: Khalid AlAwadhi
 */
#include "Logger_Thread.h"

//Standard includes
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"



void * LoggerThread(void * args)
{
	/* Get the passed arguments */
	struct Pthread_ArgsStruct *Arguments = args;

	/* Init the log file */
	LogFile_Init(Arguments->LogFile_Path);


	/* Create the Logging Thread POSIX queue */
	mqd_t MQ;											//Message queue descriptor

	/* Initialize the queue attributes */
	struct mq_attr attr;
	attr.mq_flags = 0;									/* Flags: 0 or O_NONBLOCK */
	attr.mq_maxmsg = 10;								/* Max. # of messages on queue */
	attr.mq_msgsize = sizeof(POSIX_MsgStruct);			/* Max. message size (bytes) */
	attr.mq_curmsgs = 0;								/* # of messages currently in queue */

	/* Create the Logging Thread queue to get messages from other pThreads */
	MQ = mq_open(LOGGER_QUEUE, O_CREAT | O_RDONLY | O_CLOEXEC, 0666, &attr);
	if(MQ == (mqd_t) -1)
	{
		Log_Msg(Logger, "CRITICAL", "mq_open()", errno, LOCAL_ONLY);
	}

	POSIX_MsgStruct MsgRecv;							//Temp variable used to store received messages

	/* Loop forever waiting for Msgs from other pThreads */
	while(1)
	{
		/* Block until a msg is received */
		if(mq_receive(MQ, &MsgRecv, sizeof(POSIX_MsgStruct), NULL) == -1)
		{
			Log_Msg(Logger, "ERROR", "mq_receive()", errno, LOCAL_ONLY);
		}
		/* If a msg is received, log it */
		else
		{
			LogFile_Log(Arguments->LogFile_Path, &MsgRecv);
		}
	}

	if(mq_unlink(LOGGER_QUEUE) != 0)
	{
		Log_Msg(Logger, "ERROR", "mq_unlink()", errno, LOCAL_ONLY);
	}
	else
	{
		Log_Msg(Logger, "INFO", "Successfully unlinked Logger queue!", 0, LOCAL_ONLY);
	}
	
	Log_Msg(Logger, "INFO", "Logger Thread has terminated successfully and will now exit", 0, LOCAL_ONLY);
	return 0;
}



void LogFile_Init(char* LogFilePath)
{
	/* File pointer */
	FILE *MyFileP;

	/* Modify the permissions of the file to be write and open the file (anything stored previously will be erased) */
	MyFileP = fopen(LogFilePath, "w");

	if(MyFileP == NULL)
	{
		char TempMsg[100];
		snprintf(TempMsg, 100, "Could not open file path %s", LogFilePath);
		Log_Msg(Logger, "FATAL", TempMsg, errno, LOCAL_ONLY);
		exit(1);
	}

	/* NOTE:
	 * Statements are stored in strings because we want to have a debug output functionality.
	 * So, if we change anything in fprintf() we will also need to go to the printf() and
	 * change the text there. Using string and storing our text there makes it easier
	 * as we only need to change the text in one place rather than two. */
	char* Line1 = "> [%s] 'Logger Thread' (INFO) => Logfile successfully created! TID: %ld\n\n";
	
	char* Line2 = "\n\r> ***************************************************\n\r";
	char* Line3 = "> *         APES Project 2 (BeagleBone Side):       *\n\r";
	char* Line4 = "> *               S.E.C.U.R.E.R.O.O.M               *\n\r";
	char* Line5 = "> * 'S'uper 'E'ncrypted 'C'ourse project which is a *\n\r";
	char* Line6 = "> * 'U'seful 'R'oom 'E'mbedded 'S'ystem that        *\n\r";
	char* Line7 = "> * communicates 'R'emotely 'O'ver the air and it's *\n\r";
	char* Line8 = "> *              'O'bviously 'M'arvelous            *\n\r";
	char* Line9 = "> *                                                 *\n\r";
	char* Line10 = "> *        By: Khalid AlAwadhi | Poorn Mehta        *\n\r";
	char* Line11 = "> *                                            v1.1 *\n\r";
	char* Line12 = "> ***************************************************\n\n\r";
	
	/* Temp variable to store current date/time */
	char CurTime[TIMESTR_SIZE];
	GetRealTime(CurTime, TIMESTR_SIZE);
	
	/* Print to file */
	fprintf(MyFileP, Line1, CurTime, syscall(SYS_gettid));
	fprintf(MyFileP, Line2);
	fprintf(MyFileP, Line3);
	fprintf(MyFileP, Line4);
	fprintf(MyFileP, Line5);
	fprintf(MyFileP, Line6);
	fprintf(MyFileP, Line7);
	fprintf(MyFileP, Line8);
	fprintf(MyFileP, Line9);
	fprintf(MyFileP, Line10);
	fprintf(MyFileP, Line11);
	fprintf(MyFileP, Line12);

	/* Flush file output */
	fflush(MyFileP);

	/* Close the file */
	fclose(MyFileP);
	
	/* Print to stdout */
	printf(Line1, CurTime, syscall(SYS_gettid));
	printf(Line2);
	printf(Line3);
	printf(Line4);
	printf(Line5);
	printf(Line6);
	printf(Line7);
	printf(Line8);
	printf(Line9);
	printf(Line10);
	printf(Line11);
	printf(Line12);
}



void LogFile_Log(char* LogFilePath, POSIX_MsgStruct* Message)
{
	/* File pointer */
	FILE *MyFileP;

	/* Modify the permissions of the file to append */
	MyFileP = fopen(LogFilePath, "a");

	if(MyFileP == NULL)
	{
		printf("> !! ERROR: Could not open log file: %s\n", LogFilePath);
		printf("> 	|--> Logging from source '%u' failed\n", Message->Source);
		printf("> 	|--> Destination: %u\n", Message->Dest);
		printf("> 	|--> Log Level: %s\n", Message->LogLevel);
		printf("> 	L--> Message: %s\n\n", Message->Msg);
		return;
	}

	/* Get the source number and turn it into a string.
	 * This is done for readability in the logging file */
	char Source_text[20];
	switch(Message->Source)
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


	/* This string will store the text to output later on (just a temp variable) */
	char text[MSGSTR_SIZE];
	
	/* Temp variable to store current date/time */
	char CurTime[TIMESTR_SIZE];
	GetRealTime(CurTime, TIMESTR_SIZE);


	strcpy(text, "> [%s] Log Event(%s): %s\n> L->Source: %s\n\n");
	
	fprintf(MyFileP, text, CurTime, Message->LogLevel, Message->Msg, Source_text);
	printf(text, CurTime, Message->LogLevel, Message->Msg, Source_text);


//	switch(Message->Dest)
//	{
//		case Main:
//			strcpy(text, "[%s] 'Main Thread' got msg from '%s'(%s) => %s\n\n");
//			
//			fprintf(MyFileP, text, CurTime, Source_text, Message->LogLevel, Message->Msg);
//
//			printf(text, text, CurTime, Source_text, Message->LogLevel, Message->Msg);
//			break;
//
//		case Logger:
//			strcpy(text, "[%s] 'Logger Thread' got msg from '%s'(%s) => %s\n\n");
//			
//			fprintf(MyFileP, text, CurTime, Source_text, Message->LogLevel, Message->Msg);
//
//			printf(text, text, CurTime, Source_text, Message->LogLevel, Message->Msg);
//			break;
//
//		case TivaComm:
//			strcpy(text, "[%s] 'TivaComm Thread' got msg from '%s'(%s) => %s\n\n");
//			
//			fprintf(MyFileP, text, CurTime, Source_text, Message->LogLevel, Message->Msg);
//
//			printf(text, text, CurTime, Source_text, Message->LogLevel, Message->Msg);
//			break;
//
//		default:
//			strcpy(text, "[%s] 'Unknown Thread' got msg from '%s'(%s) => %s\n\n");
//			
//			fprintf(MyFileP, text, CurTime, Source_text, Message->LogLevel, Message->Msg);
//
//			printf(text, text, CurTime, Source_text, Message->LogLevel, Message->Msg);
//			break;
//	}

	/* Flush file output */
	fflush(MyFileP);

	/* Close the file */
	fclose(MyFileP);
}
