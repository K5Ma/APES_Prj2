/*
 * BBComm_Task.h
 *
 *  Created on: Apr 15, 2019
 *      Author:  Author: Khalid AlAwadhi
 */

#ifndef MY_INCLUDES_BBCOMM_TASK_H_
#define MY_INCLUDES_BBCOMM_TASK_H_

#include <stdint.h>

/**************************************************************************************************************
 * USAGE: This function will initialize the BBComm task
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: 0 => Init was successful
 * 			1 => Init failed
 **************************************************************************************************************/
uint8_t BBComm_TaskInit();


/**************************************************************************************************************
 * USAGE: This function handles communication with the external BeagleBone board via UART.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void BBComm_Task(void *pvParameters);



#endif /* MY_INCLUDES_BBCOMM_TASK_H_ */
