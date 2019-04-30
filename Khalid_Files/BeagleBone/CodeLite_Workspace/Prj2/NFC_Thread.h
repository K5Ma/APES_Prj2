/*
 * NFC_Thread.h
 *
 *  Created on: Apr 28, 2019
 *      Author: Khalid AlAwadhi
 */

#ifndef NFC_THREAD_H_
#define NFC_THREAD_H_

#include <stdint.h>
#include "Global_Defines.h"

/**************************************************************************************************************
 * USAGE: This function contains all what the NFC pThread will do.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void * NFCThread(void * args);



/**************************************************************************************************************
 * USAGE: This function handles recieved NFC structs from Tiva.
 *
 * PARAMETERS:
 *            - NFC_T2B_Struct NFC_Rx => The struct recieved 
 *
 * RETURNS: Success => 0
 *          Failure => 1
 **************************************************************************************************************/
bool NFC_Function(NFC_T2B_Struct NFC_Rx);



/**************************************************************************************************************
 * USAGE: This function converts the struct into a byte array and then send it to the TivaComm pThread to TX. 
 *
 * PARAMETERS:
 *            - KE_B2T_Struct StructToSend => The structure to send 
 * 
 * RETURNS: NONE
 **************************************************************************************************************/
void Send_KEStruct_ToTiva(KE_B2T_Struct StructToSend);



#endif /* NFC_THREAD_H_ */
