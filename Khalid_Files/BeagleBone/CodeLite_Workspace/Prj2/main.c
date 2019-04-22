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
#include "Logger_Thread.h"


/* LAST WORKING ON:
 * UPDATED SO MANY INETRNAL FUNCITONS. A GOOD BASE WILL ENSURE LESS BUGS. 
 * NEED TO WORK ON THE PROPER WAY TO TALK TIO THE TIVA NEXT. 
 */
 
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
 * 4- [COMPLETED] UPDATE Log_error() FUNCTION
 * 				L-> REVAMPED ENTIRE FUNCTION, NOW CALLED Log_Msg() AND USED FOR EVERYHTING (NOT JUST ERRORS) 
 * 
 * 5- [COMPLETED] CHANGE TIME OUTPUT TO REALTIME (BETTER FORMATTING)
 * 				L-> CREATED A THREAD-SAFE FUNCTION GetRealTime() FOUND IN Master_Functions.h/.c WITH IMPORTANT NOTES
 * 
 * 6- [COMPLETED] CREATE LOGGER THREAD
 * 				L-> CREATED Logger_Thread.h/.c WHICH IS BASED ON PRJ1 LOGGER THREAD CODE BUT COMPLETLY REVAMPED
 *                  ALSO, FORMAT OF LOGGING HAS NOW CHANGED. ANYTHING SENT TO THE LOGGER THREAD WILL NOW JUST BE A LOG
 *                  EVENT AS OPPOSED TO A MESSAGE WITH A SOURCE AND DESTINATION WHICH DID NOT MAKE MUCH SENSE WHEN
 *                  LOOKING AT THE LOG FILE WHICH IS ONLY SUPPOSED TO BE ABOUT DELIBIRATE LOGS TO IT. 
 * 
 * 7- [COMPLETED] CONNECT THE BBoneBLE MODULE
 * 				L-> BBoneBLE MAC ADDRESS: 9C1D_589C_29AB
 * 
 * 8- [] FIX UART READING METHOD (MAYBE INTERUPPTS???)
 * 
 * 9- [COMPLETED] UPDATE SendToThreadQ()
 * 				L-> UPDATED TO SUPPORT NEW Log_Msg() AND NAME CHANGES IN THIS PROJECT
 * 
 * 10- [] 
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
	pthread_t Logger_pThread, TivaComm_pThread;


	/* Create Logger pThread */
	if(pthread_create(&Logger_pThread, NULL, &LoggerThread, (void *)&args) != 0)
	{
		Log_Msg(Main, "FATAL", "Logger pthread_create()", errno, LOCAL_ONLY);
	}
	else
	{
		Log_Msg(Main, "INFO", "SUCCESS: Created Logger Thread!", 0, LOCAL_ONLY);
	}
	
	/* Wait a bot to ensure the Logger thread starts up first */
	sleep(2);

	/* Create TivaComm pThread */
	if(pthread_create(&TivaComm_pThread, NULL, &TivaCommThread, NULL) != 0)
	{
		Log_Msg(Main, "FATAL", "TiveComm pthread_create()", errno, LOGGER_AND_LOCAL);
	}
	else
	{
		Log_Msg(Main, "INFO", "SUCCESS: Created TivaComm Thread!", 0, LOGGER_AND_LOCAL);
	}


	/* Wait for pThreads to finish */
	pthread_join(Logger_pThread, NULL);
	pthread_join(TivaComm_pThread, NULL);

	return 0;
}

