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
 * USAGE: This function simply gets the current time in ms (elapsed time not real-time) based on the ticks.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: float Current_Time_in_ms
 **************************************************************************************************************/
float GetCurrentTime();



/**************************************************************************************************************
 * USAGE: This function takes in a number and updates a given string with the name linked to the enum
 *        (enums found in Global_Defines.h). Tasks are on Tiva while Threads are on BeagleBone.
 *
 * PARAMETERS:
 *            - uint8_t EnumNum => The enum you want to get the name of
 * 			  - char* Str => String to store the name into (Make sure it at least a size of 20 to work for all
 *                                                          cases!)
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void EnumtoString(uint8_t EnumNum, char* Str);



/**************************************************************************************************************
 * USAGE: This function will output a message to the UART0 (make sure to Init the UART first!). This function
 *		  should mainly be used in tasks to log errors to the local UART0 and small stuff that do not need to be
 *		  sent to the BeagleBone.
 *
 * PARAMETERS:
 *            - float CurrTime => The current time (use GetCurrentTime() in this parameter)
 *            - uint8_t Src => Source of the message (Look at Global_Defines.h Source Enums)
 *            - char* LogLvl => Level of message (INFO / ERROR / CRITICAL etc.)
 *            - char* Msg => The message to display
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void Log_UART0(float CurrTime, uint8_t Src, char* LogLvl, char* Msg);



/**************************************************************************************************************
 * USAGE: This function will send a user message to the Logger Task to: send msg to the BBComm Task, output to
 *        UART0, or both.
 *
 * PARAMETERS:
 *            - uint8_t Src => Source of the message (Look at Global_Defines.h Source Enums)
 *            - char* LogLvl => The level of the message (INFO | WARNING | ERROR | CRITICAL | FATAL)
 *            - char* OutMsg => The message to display (will be dipalyed before the UNIX errorif applicable)
 *            - uint8_t Mode => The below modes are already defined in Global_Defines.h
 *                              0x01 = LOGGER_AND_LOCAL:    In this mode, the message will be displayed to
 *                                                          the user b=via UART0 in addition to sending the
 *                                                          message to the BeagleBone via BBComm task.
 *
 *                              0x02 = LOGGER_ONLY:         In this mode, the message will only be sent to the
 *                                                          BBComm Task and then the BeagleBone.
 *
 *                              0x03 = LOCAL_ONLY(Default): In this mode, the message will only be displayed
 *                                                          to user via UART0.
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void Log_Msg(uint8_t Src, char* LogLvl, char* OutMsg, uint8_t Mode);



/**************************************************************************************************************
 * USAGE: This function usues the given info to create a LogMsg_Struct and then send it to the BBComm xQueue
 *        to TX the struct over to the BeagleBone.
 *
 * PARAMETERS:
 *            - uint8_t Src => Source of the message (Look at Global_Defines.h Source Enums)
 *            - char* LogLvl => Level of message (INFO / ERROR / CRITICAL etc.)
 *            - char* Msg => The message to display
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void Send_LogMsgStruct_ToBB(uint8_t Src, char* LogLvl, char* OutMsg);



#endif /* MY_INCLUDES_MASTER_FUNCTIONS_H_ */
