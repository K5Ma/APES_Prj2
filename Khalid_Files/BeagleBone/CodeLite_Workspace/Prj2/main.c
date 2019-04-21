//Standard includes
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

//My inludes
#include "Global_Defines.h"
#include "Master_Functions.h"
#include "TivaComm_Thread.h"


/*
 * ++++++++++ TO-DO: ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * 1- [COMPLETED] ADD UART INIT FUNCTION AND CODE
 * 				L-> CREATED TivaComm_Thread.h/.c AND My_UART_BB.h/.c
 * 
 * 2- [COMPLETED] TEST UART TX
 * 
 * 3- [COMPLETED] TEST UART RX
 * 				L-> NEED TO TEST PROPER WAY TO SYNC READS
 * 
 * 4- [] UPDATE Log_error() FUNCTION
 * 
 * 5- [] CHANGE TIME OUTPUT TO REATIME (BETTER FORMATTING)
 * 
 * 6- [] CREATE LOGGING THREAD
 * 
 * 7- [] CONNECT THE BBoneBLE MODULE
 * 
 * 8- [] FIX UART READING METHOD (MAYBE INTERUPPTS???)
 * 
 * 9- [] 
 *
 * +++++++++++++++++++++ QUESTIONS: +++++++++++++++++++++
 * -
 *
 */
int main(int argc, char **argv)
{
	struct Pthread_ArgsStruct args;						//Create the pthread args structure

	char User_LogFilePath[100];							//This will store the log file path location to pass to the Logging pthread

	printf("Starting BB Prj2... PID: %d\n\n", getpid());

	/* Check if the user entered a logfile path */
	if(argc > 1)
	{
		sprintf(User_LogFilePath, "%s", argv[1]);
		printf("Chosen log file path: %s\n", User_LogFilePath);
	}

	/* Else, use default logfile path */
	else
	{
		sprintf(User_LogFilePath, "./LogFile.txt");
		printf("No logfile path chosen. Using default location './LogFile.txt'\n\n");
	}


	/* Store filepath to pass to pThreads */
	strcpy(args.LogFile_Path, User_LogFilePath);

	
	/* Create the needed pThreads */
	pthread_t TivaComm_pThread;


	/* Create TivaComm pThread */
	if(pthread_create(&TivaComm_pThread, NULL, &TivaCommThread, NULL) != 0)
	{
		printf("Could not create TIvaComm pthread\n");
	//	Log_error(Main, "TivaComm pthread_create()", errno, LOCAL_ONLY);
	}
	else
	{
		printf("[%lf] Main pThread SUCCESS: Created TivaComm Thread!\n\n", GetCurrentTime());
	}


	/* Wait for pThreads to finish */
	pthread_join(TivaComm_pThread, NULL);

	return 0;
}

