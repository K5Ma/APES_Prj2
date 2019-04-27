/*
 * Logger_Task.c
 *
 *  Created on: Apr 26, 2019
 *      Author:  Author: Khalid AlAwadhi
 */

#include "Logger_Task.h"

//Standard includes

//TivaWare includes

//FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"

/* Global Variables */



uint8_t Logger_TaskInit()
{
    //Create task, if it fails return 1, else 0
    if( xTaskCreate(Logger_Task,                    /* The function that implements the task */
			   (const portCHAR *)"Logger",          /* The text name assigned to the task - for debug only as it is not used by the kernel */
               ((configMINIMAL_STACK_SIZE) * 1),    /* The size of the stack to allocate to the task */
			   NULL,                                /* The parameter passed to the task */
			   PRIORITY_LOGGER_TASK,                /* The priority assigned to the task */
			   NULL)                                /* The task handle is not required, so NULL is passed */
    			!= pdTRUE
      )
      {
    	return 1;
      }

	return 0;
}



void Logger_Task(void *pvParameters)
{

	while(1)
	{

	}
}
