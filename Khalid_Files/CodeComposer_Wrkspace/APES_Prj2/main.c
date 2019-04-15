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


/* LAST WORKING ON:
 * GETIING LOG_UART0 TO WORK PROPERLY.
 * NEED TO CONSIDER STACK SIZE NEED MORE INFO ON THAT.
 * ALSO I INCREASED HEAP SIZE IN PROJECT OPTION TO MATCH FREERTOS CONFIG
 * NOT SURE WHERE TO CHANGE OPTIONS TO GIVE MAX SOACE FOR EACH OF MY TASKS IF NEEDED
 * WORK ON RX NEXT!
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
 * 5- [] WORK ON RX
 *
 * 6- [] THINK ABOUT HOW MSGS WILL BE STORED AND TRANSMITTED (CIRC BUFF AND UART? CIRC BUFF WOULD BE SENT TO BB AND
 * 		 UART IS JUST LOCAL ECHO)
 *
 * 7- [COMPLETED] ADD CIRC BUFF LIB
 * 				L-> ADDED My_CircBuff.h/.c
 *
 * 8- [] CREATE TEST STRUCT
 *
 * 9- [] TEST STORING AND READING FROM TEST STRUCT
 *
 * 10- [] MAYBE DYNAMICALLY ALLOCATE STRING IN Log_UART0()? FOR NOW IT IS A STATIC SIZE
 *
 * 11- []
 *
 * +++++++++++++++++++++ QUESTIONS: +++++++++++++++++++++
 * - [FROM HW 5 - STILL NEED TO DOUBLE CHECK] Is the current way I am sending messages correct and efficient? While
 *   searching I saw that pointers are used for large structs, however, if I declare a pointer struct and send it
 *   doesn't the pointer get overwritten the next time I send a message using the same pointer? Meaning if I use
 *   pointer I would have to malloc first then free after I receive and read it? Correct?
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
	Log_UART0(GetCurrentTime(), Main, "INFO", "BB_Comm Task init successfully!");

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
