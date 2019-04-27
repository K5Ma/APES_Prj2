/*
 * Logger_Thread.h
 *
 *  Created on: Apr 21, 2019
 *      Author: Khalid AlAwadhi
 */
#ifndef LOGGER_THREAD_H_
#define LOGGER_THREAD_H_

#include "Global_Defines.h"


/**************************************************************************************************************
 * USAGE: This function contains all what the Logger pThread will do.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: NONE
 *
 **************************************************************************************************************/
void * LoggerThread(void * args);



/**************************************************************************************************************
 * USAGE: This function will be called initially when the Logger Thread is first initialized.
 *
 * PARAMETERS:
 *            - char* LogFilePath => Path to the log file
 *
 * RETURNS: NONE
 *
 **************************************************************************************************************/
void LogFile_Init(char* LogFilePath);



/**************************************************************************************************************
 * USAGE: This function will log messages received by the Logger Thread. It will decode the message and
 *        specify the destination and source it came from and log it to a file.
 *
 * PARAMETERS:
 *            - char* LogFilePath => Path to the log file
 *            - Log_MsgStruct* Message => The message structure to decode and log
 *
 * RETURNS: NONE
 *
 **************************************************************************************************************/
void LogFile_Log(char* LogFilePath, Log_MsgStruct* Message);




#endif /* LOGGER_THREAD_H_ */
