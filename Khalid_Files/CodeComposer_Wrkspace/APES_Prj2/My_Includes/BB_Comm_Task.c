/*
 * BB_Comm_Task.c
 *
 *  Created on: Apr 15, 2019
 *      Author:  Author: Khalid AlAwadhi
 */
#include "BB_Comm_Task.h"

//Standard includes
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

//TivaWare includes


//FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"

/* Global Variables */


uint8_t BB_Comm_TaskInit()
{
    //Create task, if it fails return 1, else 0
    if( xTaskCreate(BB_Comm_Task,					/* The function that implements the task */
			   (const portCHAR *)"BB_Comm",			/* The text name assigned to the task - for debug only as it is not used by the kernel */
               ((configMINIMAL_STACK_SIZE) * 5),	/* The size of the stack to allocate to the task */
			   NULL,								/* The parameter passed to the task */
			   PRIORITY_BB_COMM_TASK, 				/* The priority assigned to the task */
			   NULL)								/* The task handle is not required, so NULL is passed */
    			!= pdTRUE
      )
      {
    	return 1;
      }

	return 0;
}



void BB_Comm_Task(void *pvParameters)
{
//	int a = 1;

	while(1)
	{
		Log_UART0(GetCurrentTime(), BB_Comm, "INFO", "Hi I am alive!");
	}

}
