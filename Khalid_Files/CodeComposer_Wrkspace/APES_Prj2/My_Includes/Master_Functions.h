/*
 * Master_Functions.h
 *
 *  Created on: Apr 9, 2019
 *      Author: Khalid AlAwadhi
 */

#ifndef MY_INCLUDES_MASTER_FUNCTIONS_H_
#define MY_INCLUDES_MASTER_FUNCTIONS_H_

#include <stdint.h>



/**************************************************************************************************************
 * USAGE: This function will output a message to the UART (make sure to Init the UART first!)
 *
 * PARAMETERS:
 *            - uint8_t Src => Source of the message (Look at Global_Defines.h Source Enums)
 *            - char* LogLvl => Level of message (INFO / ERROR / CRITICAL)
 *            - char* Msg => The message to display
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void Log_UART(uint8_t Src, char* LogLvl, char* Msg);



#endif /* MY_INCLUDES_MASTER_FUNCTIONS_H_ */
