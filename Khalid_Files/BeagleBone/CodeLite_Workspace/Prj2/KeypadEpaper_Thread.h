/*
 * KeypadEpaper_Thread.h
 *
 *  Created on: Apr 28, 2019
 *      Author: Khalid AlAwadhi
 */

#ifndef KEYPADEPAPER_THREAD_H_
#define KEYPADEPAPER_THREAD_H_

#include <stdint.h>
#include "Global_Defines.h"


/**************************************************************************************************************
 * USAGE: This function contains all what the KeypadEpaper pThread will do.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void * KeypadEpaperThread(void * args);



/**************************************************************************************************************
 * USAGE: This function confirms the code user has inputted on Tiva side.
 *
 * PARAMETERS:
 *            - KE_T2B_Struct EK_Rx => The struct received
 *
 * RETURNS: Success => 0
 *          Failure => 1
 **************************************************************************************************************/
bool EK_Function(KE_T2B_Struct EK_Rx);



/**************************************************************************************************************
 * USAGE: This function converts the struct into a byte array and then send it to the TivaComm pThread to TX. 
 *
 * PARAMETERS:
 *            - LC_B2T_Struct StructToSend => The structure to send 
 * 
 * RETURNS: NONE
 **************************************************************************************************************/
void Send_LCStruct_ToTiva(LC_B2T_Struct StructToSend);



#endif /* KEYPADEPAPER_THREAD_H_ */
