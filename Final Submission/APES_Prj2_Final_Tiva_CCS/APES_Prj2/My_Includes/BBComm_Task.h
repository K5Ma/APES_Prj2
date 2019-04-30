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



/**************************************************************************************************************
 * USAGE: This function decodes a given buffer containing buffer data, created the structure needed and then
 *        send it to the targeted thread.
 *
 * PARAMETERS:
 *            - uint8_t* StructToDecode => The buffer with structure data to decode
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void Decode_StructBuffer(uint8_t* StructToDecode);




#endif /* MY_INCLUDES_BBCOMM_TASK_H_ */
