//Standard includes
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/signal.h>

//My inludes
#include "Global_Defines.h"
#include "Master_Functions.h"
#include "My_UART_BB.h"
#include "TivaComm_Thread.h"
#include "Logger_Thread.h"


/* Global Variables */
struct sigaction UART1_RX_SignalAction;                 //Used in the Init_UART for UART1
char Start_RX[1] = "0";                                 //Will store the start CMD coming in from Tiva
UART_Struct *BB_UART1;                                  //Global struct of UART1 as it is used in two files (TivaComm and My_UART)
volatile bool POLL_RX = false;                          //Flag used to know if we are currently RXing from Tiva or not 


/* LAST WORKING ON:
 * KEEP WORKING ON PROPER API FOR RXING A LOG MESSAGE FROM TIVA
 * THEN GO TO TIVA AND MAKE A PROPER LOGMSG STRUCT SEND UART FUNCTION!
 * 
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
 * 8- [COMPLETED] FIX UART READING METHOD (MAYBE INTERUPPTS???)
 *              L-> ADDED INTERRUPTS (SIGNAL HANDLER) FOR UART1!
 * 
 * 9- [COMPLETED] UPDATE SendToThreadQ()
 * 				L-> UPDATED TO SUPPORT NEW Log_Msg() AND NAME CHANGES IN THIS PROJECT
 * 
 * 10- [COMPLETED] ADD NEW TEST STRUCT
 *              L-> CREATED TivaBB_MsgStruct, FOUND IN Global_Defines.h
 * 
 * 11- [COMPLETED] TEST STRUCT SIZE? 
 * 				L-> STRUCT SIZE IS 86 BYTES. NOT AS BAD AS I EXPECTED
 * 
 * 12- [] ATTEMPT TX OF STRUCT TO TIVA WITH PROPER CMDS USED
 *              L-> CREATED Send_Struct_UARTx() 
 * 
 * 13- [COMPLETED] ADD CURRENT VERSION DEFINE
 * 				L-> FOUND IN Global_Defines.h. JUST CHANGE THE VERSION NO. THERE AND IT WILL UPDATE WHERE NEEDED
 * 
 * 14- [COMPLETED] CREATE A PROPER TX FUNCTION FOR THE STRUCTURE
 * 				L-> CREATED 
 * 
 * 15- [] RX A STRUCT FROM TIVA IN A COMPLETE WAY WITH START, CONFIRM AND END CMDS
 * 
 * 16- [UNABLE TO DO] PROBABLY NEED TO HAVE FUNCITON HANDLE TX DIFFERENT STRUCT TYPES
 *              L-> IN C YOU CANNOT PASS DIFFFERENT STURCTS IN ONE FUNCITON, HAVE TO HAVE SEPEARATE FUNCTIONS. 
 *                  LOOK AT #19
 * 
 * 17- [COMPLETED] UPDATE FUCNTIONS WITH NEW DEFINE SOURCES
 * 				L-> A LOT OF CHANGES EVERYWHERE
 * 
 * 18- [UPDATE - COMPLETED] UPDATE SendToThreadQ() TO WORK WITH NEW PTHREADS
 *              L-> FUNCTION IS NOW SendToLoggerQ(), EACH THREAD THAT EXPECTS ITS OWN STRUCT NEEDS IT OWN 
 *                  SendToXXXQ() 
 * 
 * 19- [COMPLETED] CREATE A NEW FUNCTION THAT UPDATED GIVEN STRING BASED ON ENUM SOURCE NUMBER
 * 				L-> CREATED EnumtoString() FOUND IN Global_Defines.h/.c
 * 
 * 20- [] NEED TO ADD THE NEW LOGGER STURCT 
 * 
 * 21- [] TEST RXING A LOG EVENT FROM TIVA
 * 
 * 22- [] NEED TO CREATE PROPER POSIX Q FOR TIVACOMM THAT STORES BYTE ARRAYS
 * 
 * 23- [] 
 * 
 *
 * +++++++++++++++++++++ CHANGES/INFO: +++++++++++++++++++++
 * - Should have started this eariler with so many changes happening eveywhere...
 * 
 * - Added new Log_Msg struct in Gloobal_Defines.h
 * 
 * - Found out that BB can handle fast UART data coming in from Tiva, meaning no need for confirmation
 *   (#) when RXing here on BB side
 * 
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
		char TempTxt[100];
		snprintf(TempTxt, 100, "Chosen log file path: %s", User_LogFilePath);
		Log_Msg(BB_Main, "INFO", TempTxt, 0, LOCAL_ONLY);
	}
	/* Else, use default logfile path */
	else
	{
		sprintf(User_LogFilePath, "./LogFile.txt");
		char TempTxt[100];
		snprintf(TempTxt, 100, "No logfile path chosen. Using default location './LogFile.txt'");
		Log_Msg(BB_Main, "INFO", TempTxt, 0, LOCAL_ONLY);
	}

	/* Store filepath to pass to pThreads */
	strcpy(args.LogFile_Path, User_LogFilePath);

	
	/* Create the needed pThreads */
	pthread_t Logger_pThread, TivaComm_pThread;


	/* Create Logger pThread */
	if(pthread_create(&Logger_pThread, NULL, &LoggerThread, (void *)&args) != 0)
	{
		Log_Msg(BB_Main, "FATAL", "Logger pthread_create()", errno, LOCAL_ONLY);
	}
	else
	{
		Log_Msg(BB_Main, "INFO", "SUCCESS: Created Logger Thread!", 0, LOCAL_ONLY);
	}
	
	/* Wait a bot to ensure the Logger thread starts up first */
	sleep(2);

	/* Create TivaComm pThread */
	if(pthread_create(&TivaComm_pThread, NULL, &TivaCommThread, NULL) != 0)
	{
		Log_Msg(BB_Main, "FATAL", "TiveComm pthread_create()", errno, LOGGER_AND_LOCAL);
	}
	else
	{
		Log_Msg(BB_Main, "INFO", "SUCCESS: Created TivaComm Thread!", 0, LOGGER_AND_LOCAL);
	}


	/* Wait for pThreads to finish */
	pthread_join(Logger_pThread, NULL);
	pthread_join(TivaComm_pThread, NULL);

	return 0;
}

