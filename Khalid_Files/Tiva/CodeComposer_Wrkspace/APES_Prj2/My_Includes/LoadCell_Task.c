/*
 * LoadCell_Task.c
 *
 *  Created on: Apr 25, 2019
 *      Author:  Author: Khalid AlAwadhi
 */

#include "LoadCell_Task.h"

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



uint8_t LoadCell_TaskInit()
{
    //Create task, if it fails return 1, else 0
    if( xTaskCreate(LoadCell_Task,					/* The function that implements the task */
			   (const portCHAR *)"LoadCell",		/* The text name assigned to the task - for debug only as it is not used by the kernel */
               ((configMINIMAL_STACK_SIZE) * 3),	/* The size of the stack to allocate to the task */
			   NULL,								/* The parameter passed to the task */
			   PRIORITY_LOADCELL_TASK,	 			/* The priority assigned to the task */
			   NULL)								/* The task handle is not required, so NULL is passed */
    			!= pdTRUE
      )
      {
    	return 1;
      }

	return 0;
}



void LoadCell_Task(void *pvParameters)
{
	/* Delay a bit to make sure BBComm Task starts-up first */
	const TickType_t xDelay = 10 / portTICK_PERIOD_MS;
	vTaskDelay(xDelay);


	while(1)
	{

	}
}