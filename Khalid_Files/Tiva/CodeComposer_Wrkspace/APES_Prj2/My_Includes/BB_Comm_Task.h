/*
 * BB_Comm_Task.h
 *
 *  Created on: Apr 15, 2019
 *      Author:  Author: Khalid AlAwadhi
 */

#ifndef MY_INCLUDES_BB_COMM_TASK_H_
#define MY_INCLUDES_BB_COMM_TASK_H_

#include <stdint.h>

/**************************************************************************************************************
 * USAGE: This function will initialize the BB_Comm task
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: 0 => Init was successful
 * 			1 => Init failed
 **************************************************************************************************************/
uint8_t BB_Comm_TaskInit();


/**************************************************************************************************************
 * USAGE: This function handles communication with the external BeagleBone board via UART.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void BB_Comm_Task(void *pvParameters);



#endif /* MY_INCLUDES_BB_COMM_TASK_H_ */
