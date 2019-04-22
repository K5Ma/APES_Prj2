/*
 * Master_Functions.h
 *
 *  Created on: Apr 9, 2019
 *      Author: Khalid AlAwadhi
 */

#ifndef MY_INCLUDES_MASTER_FUNCTIONS_H_
#define MY_INCLUDES_MASTER_FUNCTIONS_H_

#include <stdint.h>
#include "Global_Defines.h"


/**************************************************************************************************************
 * USAGE: This function displays our boot-up msg on the desired UART port.
 *
 * PARAMETERS:
 *            - uint8_t UARTPort => The UART port to output the message to
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void DisplayBootUpMsg(uint8_t UARTPort);



/**************************************************************************************************************
 * USAGE: This function will output a message to the UART0 (make sure to Init the UART first!). This function
 *		  should mainly be used in tasks to log errors to the local UART0 and small stuff that do not need to be
 *		  sent to the BeagleBone.
 *
 * PARAMETERS:
 *            - float CurrTime => The current time (use GetCurrentTime() in this parameter)
 *            - uint8_t Src => Source of the message (Look at Global_Defines.h Source Enums)
 *            - char* LogLvl => Level of message (INFO / ERROR / CRITICAL)
 *            - char* Msg => The message to display
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void Log_UART0(float CurrTime, uint8_t Src, char* LogLvl, char* Msg);



/**************************************************************************************************************
 * USAGE: This function simply gets the current time in ms (elapsed time not real-time) based on the ticks.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: float Current_Time_in_ms
 **************************************************************************************************************/
float GetCurrentTime();



/**************************************************************************************************************
 * USAGE: This function XXX
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
uint8_t SendMsgToBB(MsgStruct *MsgToSend);


#endif /* MY_INCLUDES_MASTER_FUNCTIONS_H_ */
