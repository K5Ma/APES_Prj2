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

//FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"
#include "My_UART.h"
#include "BB_Comm_Task.h"


/* Global Variables */
QueueHandle_t xQueue_Msgs = NULL;				//This is the queue the BB_Comm Task will read from and other tasks will send to


/* LAST WORKING ON:
 *
 * TO ASK PROF:
 * - STACK SIZE HEAP SIZE ON FREERTOS? I AM NOT SURE
 * - UART ON THE BBG - ARE THEE DEFINE LIBS TO INTERFACE WITH HARDWARE
 * - TELL HIM ABOUT HOW I AM PLANNING TO TANSFER STUFF VIA UART
 * - ANY SUGGESTIONS FROM HIM?
 * -
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
 * 5- [] WORK ON RX => BB_Comm_Task()
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
 * 16- [] ADD BOOT-UP MSG
 *
 * 17- [COMPLETED] CHANGE FREERTOS HEAP TYPE TO HEAP 3 OR 4
 * 				L-> USED heap_4.c
 *
 * 18- [] CONNECT TivaBLE MODULE
 *
 * 19- [] HAVE RX INTERRUPPTS FOR BB_Comm TASK
 *
 * 20- []
 *
 *
 * +++++++++++++++++++++ QUESTIONS: +++++++++++++++++++++
 * - [FROM HW 5 - STILL NEED TO DOUBLE CHECK] Is the current way I am sending messages correct and efficient? While
 *   searching I saw that pointers are used for large structs, however, if I declare a pointer struct and send it
 *   doesn't the pointer get overwritten the next time I send a message using the same pointer? Meaning if I use
 *   pointer I would have to malloc first then free after I receive and read it? Correct?
 *   ====> ANSWERED: CHECK TO DO LIST
 *
 * -
 *
 */


int main()
{
	/* Initialize system clock to 120 MHz */
	uint32_t output_clock_rate_hz = SysCtlClockFreqSet( (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), SYSTEM_CLOCK);
	ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

	/* Initialize the GPIO pins for the Launchpad */
	PinoutSet(false, false);

	/* Init UART0 - Used for local debugging and errors */
	Init_UARTx(UART0, SYSTEM_CLOCK, 9600);
	Log_UART0(GetCurrentTime(), Main, "INFO", "UART0 was init successfully!");


	/* Init BB_Comm Task */
	if(BB_Comm_TaskInit())
	{
		Log_UART0(GetCurrentTime(), Main, "CRITICAL", "Could not init BB_Comm Task!");
	}
	else
	{
		Log_UART0(GetCurrentTime(), Main, "INFO", "BB_Comm Task init successfully!");
	}

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
