/*
 * AESD (APES) Project 2
 *
 * By Khalid AlAwadhi - Spring 2019
 *
 * Uses FreeRTOS 9.0, additionally all the driver source files were updated to 2.1.4.178
 *
 * This base project was gotten from GitHub:
 * https://github.com/akobyl/TM4C129_FreeRTOS_Demo
 *
 */
//Standard includes
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

//TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "drivers/pinout.h"
#include "driverlib/interrupt.h"

//FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"
#include "My_UART.h"
#include "Logger_Task.h"
#include "BBComm_Task.h"
#include "NFC_Task.h"
#include "KeypadEpaper_Task.h"
#include "LoadCell_Task.h"
#include "Outputs_Task.h"


/* Global Variables */
QueueHandle_t xQueue_TXStruct = NULL;				//This is the queue the BB_Comm Task will read from and other tasks will send to
char Start_RX[1] = "0";								//Will store the start CMD coming in from BB
volatile bool POLL_RX = false;						//Flag used to know if we are currently RXing from BB or not


/* LAST WORKING ON:
 *
 *
 */


/*
 * ++++++++++ TO-DO: ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * 1- [COMPLETED] CLEAN PROJECT FROM HW5 CODE
 *
 * 2- [COMPLETED] TEST UART TX IN ANOTHER PROJECT
 *				L-> Created My_UART.h/.c
 *
 * 3- [COMPLETED] ADD ALL AVAILABLE UARTS SUPPORT IN My_UART.h/.c
 *				L-> ALL PINs SUPPORT WAS ADDED (NOTE THAT SOME PINS HAVE AN ADDITIONAL TX OR RX PIN, HOWEVER, I
 *				    ONLY ADDED ONE OF THEM AS I DON'T THINK WE WILL NEED TO TX/RX FROM TWO SOURCES.
 *
 * 4- [COMPLETED!!! - THEY ALL WORK] TEST ALL NEWLY ADDED UARTS AND MAKE SURE TX WORKS FROM ALL
 *
 * 5- [COMPLETED!!!] WORK ON RX => BB_Comm_Task(): RECIEVE TEST STRUCTURE FROM BB AND DECODE PROPERLY
 *
 * 6- [COMPLETED] THINK ABOUT HOW MSGS WILL BE STORED AND TRANSMITTED (CIRC BUFF AND UART? CIRC BUFF WOULD BE SENT TO BB AND
 * 		 		|  UART IS JUST LOCAL ECHO)
 * 		 		L-> TALKED TO PROF. RICK ABOUT IT AND HE RECOMMENDED I USE THE XQUEUE AS IT HANDLES MORE STUFF IN THE
 * 		 			BACKGROUND. ADDITIONALY, HE RECOMENDED JUST COPYING THE MSG STRUCT TO THE QUEUE (WHICH XQUEUE DOES
 * 		 			ALREADY) AS OPPOSED TO MALLOC'ING (TO AVOID MEMORY LEAKS AND OTHER ISSUES)
 *
 * 7- [COMPLETED] ADD CIRC BUFF LIB
 * 				L-> ADDED My_CircBuff.h/.c
 *
 * 8- [COMPLETED] CREATE TEST STRUCT
 *
 * 9- [COMPLETED] TEST STORING AND READING FROM TEST STRUCT
 *
 * 10- [] MAYBE DYNAMICALLY ALLOCATE STRING IN Log_UART0()? FOR NOW IT IS A STATIC SIZE
 *
 * 11- [] HAVE LEDs BLINK DURING SENDING FROM BB_COMM
 *
 * 12- [COMPLETED] CREATE AN XQUEUE
 * 				L-> CREATED xQueue_Msgs GLOBAL VAR USED IN BB_Comm_Task.h/.c AND Master_Functions.h/.c
 * 				    ADDITIONALLY, CREATED A MSG SENDING FUNCTION 'SendMsgToBB()' IN Master_Functions.h/.c
 *
 * 13- [COMPLETED] TEST STORING MSG TO XQUEUE
 *
 * 14- [COMPLETED] TEST READING MSG FROM XQUEUE
 *
 * 15- [COMPLETED] REMOVE MY CIRC BUFF LIB
 *
 * 16- [COMPLETED] ADD BOOT-UP MSG
 * 				L-> CREATED DisplayBootUpMsg()
 *
 * 17- [COMPLETED] CHANGE FREERTOS HEAP TYPE TO HEAP 3 OR 4
 * 				L-> USED heap_4.c
 *
 * 18- [COMPLETED] CONNECT TivaBLE MODULE
 * 				L-> TivaBLE MAC ADDRESS: 9C1D_5888_6519
 *
 * 19- [COMPLETED] HAVE RX INTERRUPPTS FOR BB_Comm TASK
 * 				L-> ALL UART PORTS CAN NOW HAVE INTERUPPTS ENABLED SIMPLY BY CALLING Init_UARTx() AND HAVING THE
 * 				    LAST PARAMETER BE TRUE. HOWEVER, YOU NEED TO DEFINE WHAT THE RX INTERUPPT WILL DO IN ITS
 * 				    SPECIFIC INTERUPPT HANDLER.
 *
 * 20- [COMPLETED] ADD TEST STRUCT (SAME AS BB SIDE)
 *
 * 21- [COMPLETED] ADD DEFINE FOR CURRENT VERSION TO UPDATE DYNAMICALLY
 * 				L-> ONLY NEED OT CHANGE THE DEFINE IN Global_Defines.h
 *
 * 22- [COMPLETED!] TEST STRUCT SIZE ON TIVA SIDE
 * 				L-> SIZE IS THE SAME AS BB SIDE => 86 BYTES!
 *
 * 23- [SCRAPPED] RX STRUCT FROM BB, MODIFY, AND THEN TX BACK TO BB
 *
 * 24- [COMPLETED] CREATE ALL THE NEEDED BASE TASKS (NFC, KEYPADEPAPER, LOADCELL, LUX AND SERVO)
 * 				L-> RESPECTIVE .h/.c FILES CREATED AND DEFINES ADDED AS WELL AS CALLS TO INIT FROM MAIN
 *
 * 25- [COMPLETED!!!] CREATE A FUNCTION THAT LOGS TO BB AS A NORMAL LOG
 * 				L-> CREATED Send_LogMsgStruct_ToBB()
 *
 * 26- [COMPLETED] CREATE UART SEND STRING FUNCTION
 *
 * 27- [] ADD METHOD TO CHECK IF BT LINK IS ALIVE
 *
 * 28- [] LOG LOCALLY IF BT LINK IS DEAD
 *
 * 29- []
 *
 *
 * +++++++++++++++++++++ CHANGES: +++++++++++++++++++++
 * -
 *
 *
 */


int main()
{
	/* Initialize system clock to 120 MHz */
	uint32_t output_clock_rate_hz = SysCtlClockFreqSet( (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), SYSTEM_CLOCK);
	ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

	/* Initialize the GPIO pins for the Launchpad */
	PinoutSet(false, false);

	/* Init UART0 - Used for local debugging and msgs */
	Init_UARTx(UART0, SYSTEM_CLOCK, 9600, false);
	Log_UART0(GetCurrentTime(), T_Main, "INFO", "UART0 was init successfully!");

	/* Displays messages to UART0 */
	DisplayBootUpMsg(UART0);

	/* Enable global interrupts */
	IntMasterEnable();


	/* Init Logger Task */
	if(Logger_TaskInit())
	{
		Log_Msg(T_Main, "CRITICAL", "Could not init Logger Task!", LOCAL_ONLY);
	}
	else
	{
		Log_Msg(T_Main, "INFO", "Logger Task init successfully!", LOCAL_ONLY);
	}

	/* Init BBComm Task */
	if(BBComm_TaskInit())
	{
		Log_Msg(T_Main, "CRITICAL", "Could not init BBComm Task!", LOCAL_ONLY);
	}
	else
	{
		Log_Msg(T_Main, "INFO", "BBComm Task init successfully!", LOCAL_ONLY);
	}

	/* Init NFC Task */
	if(NFC_TaskInit())
	{
		Log_Msg(T_Main, "CRITICAL", "Could not init NFC Task!", LOCAL_ONLY);
	}
	else
	{
		Log_Msg(T_Main, "INFO", "NFC Task init successfully!", LOCAL_ONLY);
	}

//	/* Init KeypadEpaper Task */
//	if(KeypadEpaper_TaskInit())
//	{
//		Log_Msg(T_Main, "CRITICAL", "Could not init KeypadEpapaer Task!", LOCAL_ONLY);
//	}
//	else
//	{
//		Log_Msg(T_Main, "INFO", "KeypadEpapaer Task init successfully!", LOCAL_ONLY);
//	}
//
//	/* Init LoadCell Task */
//	if(LoadCell_TaskInit())
//	{
//		Log_Msg(T_Main, "CRITICAL", "Could not init LoadCell Task!", LOCAL_ONLY);
//	}
//	else
//	{
//		Log_Msg(T_Main, "INFO", "LoadCell Task init successfully!", LOCAL_ONLY);
//	}

//	/* Init Outputs Task */
//	if(Outputs_TaskInit())
//	{
//		Log_Msg(T_Main, "CRITICAL", "Could not init Outputs Task!", LOCAL_ONLY);
//	}
//	else
//	{
//		Log_Msg(T_Main, "INFO", "Outputs Task init successfully!", LOCAL_ONLY);
//	}

	vTaskStartScheduler();
	return 0;
}




/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {
    }
}


void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
