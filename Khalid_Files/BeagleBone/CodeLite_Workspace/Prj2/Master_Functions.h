/*
 * Master_Functions.h
 *
 *  Created on: Apr 20, 2019
 *      Author: Khalid AlAwadhi
 */

#ifndef MY_INCLUDES_MASTER_FUNCTIONS_H_
#define MY_INCLUDES_MASTER_FUNCTIONS_H_

#include <stdint.h>
#include "Global_Defines.h"


/**************************************************************************************************************
 * USAGE: This function will simply get the current time in seconds with milisecond resolution and return its 
 *        double value. It was created to simplify our code since we need to get the current time many times 
 *        and having it in this format helps keeps things organized.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: double CurrentTime
 **************************************************************************************************************/
double GetCurrentTime();



/**************************************************************************************************************
 * USAGE: This function stores the current system date/time in a given string using a thread-safe method.
 * 		  
 *        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *        !! NOTE: The given string size MUST be at least 26. Check the implementation for the reason.!! 
 *        !!       In addition, you may need to add -D_GNU_SOURCE to compiler options or this function!! 
 *        !!       might not work.                                                                    !!
 *        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 * PARAMETERS:
 *            - char* TimeStr => The string to store the date/time info to
 * 			  - uint8_t StrLen => Size of the given empty string
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void GetRealTime(char* TimeStr, uint8_t StrLen);



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
 * USAGE: This function will send a message to the Logger pThread based on the parameters. In addition, it has
 *        error handling.
 *
 * PARAMETERS:
 *            - uint8_t Src => Source of the message (Look at Global_Defines.h Source Enums)
 *            - char* Log => The log level message
 *            - char* Message => mesaage to send
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void SendToLoggerThreadQ(uint8_t Src, char* Log, char* Message);


/**************************************************************************************************************
 * USAGE: This function will send an NFC struct to the NFC pThread. In addition, it has
 *        error handling.
 *
 * PARAMETERS:
 *            - NFC_T2B_Struct StructToSend => Struct to send
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void SendToNFCThreadQ(NFC_T2B_Struct StructToSend);


/**************************************************************************************************************
 * USAGE: This function will send an KE_T2B struct to the KeypadEpaper pThread. In addition, it has error 
 *        handling.
 *
 * PARAMETERS:
 *            - KE_T2B_Struct StructToSend => Struct to send
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void SendToKEThreadQ(KE_T2B_Struct StructToSend);


/**************************************************************************************************************
 * USAGE: This function will send an LC_T2B struct to the LoadCell pThread. In addition, it has error 
 *        handling.
 *
 * PARAMETERS:
 *            - LC_T2B_Struct StructToSend => Struct to send
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void SendToLCThreadQ(LC_T2B_Struct StructToSend);


///**************************************************************************************************************
// * SCRAPPED!
// * 
// * USAGE: This function will send a buffer containing struct data to the TivaComm pThread to TX to Tiva. In 
// *        addition, it has error handling.
// *
// * PARAMETERS:
// *            - uint8_t Buffer[] => Buffer data to send
// *
// * RETURNS: NONE
// **************************************************************************************************************/
//void SendToTivaCommThreadQ(uint8_t Buffer[], uint8_t StructSize);



/**************************************************************************************************************
 * USAGE: This function will output a user message or UNIX errors alongside a message to either: send them to 
 *        the Logger thread, output to stdout, or both. This was created using a thread-safe error retrieval
 *        function.
 *
 * PARAMETERS:
 *            - uint8_t Src => Source of the message (Look at Global_Defines.h Source Enums)
 *            - char* LogLvl => The level of the message (INFO | WARNING | ERROR | CRITICAL | FATAL)
 *            - char* OutMsg => The message to display (will be dipalyed before the UNIX errorif applicable)
 *            - int errnum => must be the 'errno' varible (need to include #include <errno.h>) or just 0 for
 *                            no error
 *            - uint8_t Mode => The below modes are already defined in Global_Defines.h
 *                              0x01 = LOGGER_AND_LOCAL:    In this mode, the message will be displayed to 
 *                                                          the user in addition to sending the message to the
 *                                                          Logger pThread.
 * 
 *                              0x02 = LOGGER_ONLY:         In this mode, the message will only be sent to the
 *                                                          Logger pThread.
 * 
 *                              0x03 = LOCAL_ONLY(Default): In this mode, the message will only be displayed 
 *                                                          to user via stdout(printf).
 * 
 * RETURNS: NONE
 **************************************************************************************************************/
void Log_Msg(uint8_t Src, char* LogLvl, char* OutMsg, int errnum, uint8_t Mode);






#endif /* MY_INCLUDES_MASTER_FUNCTIONS_H_ */
