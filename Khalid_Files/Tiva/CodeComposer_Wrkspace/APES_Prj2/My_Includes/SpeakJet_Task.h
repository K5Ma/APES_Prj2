/*
 * SpeakJet_Task.h
 *
 *  Created on: Apr 25, 2019
 *      Author:  Author: Khalid AlAwadhi
 */

#ifndef MY_INCLUDES_SPEAKJET_TASK_H_
#define MY_INCLUDES_SPEAKJET_TASK_H_

#include <stdint.h>

/**************************************************************************************************************
 * USAGE: This function will initialize the SpeakJet task
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: 0 => Init was successful
 * 			1 => Init failed
 **************************************************************************************************************/
uint8_t SpeakJet_TaskInit();


/**************************************************************************************************************
 * USAGE: This function XXX
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void SpeakJet_Task(void *pvParameters);



#endif /* MY_INCLUDES_SPEAKJET_TASK_H_ */
