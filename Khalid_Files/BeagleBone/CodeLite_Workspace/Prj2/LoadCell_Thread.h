/*
 * LoadCell_Thread.h
 *
 *  Created on: Apr 28, 2019
 *      Author: Khalid AlAwadhi
 */

#ifndef LOADCELL_THREAD_H_
#define LOADCELL_THREAD_H_

#include <stdint.h>
#include "Global_Defines.h"


#define     LC_VerificationLowmv            200
#define     LC_VerificationHighmv           300
#define     LC_ConsecutiveVerificationNeeded      5

#define     SV_Enable_Pos       0
#define     SV_Enable_Mask      (1 << SV_Enable_Pos)



/**************************************************************************************************************
 * USAGE: This function contains all what the LoadCell pThread will do.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void * LoadCellThread(void * args);



/**************************************************************************************************************
 * USAGE: This function decides what outputs to enable/disable on Tiva side.
 *
 * PARAMETERS:
 *            - LC_T2B_Struct LC_Rx => The struct received
 *
 * RETURNS: Success => 0
 *          Failure => 1
 **************************************************************************************************************/
bool LC_Function(LC_T2B_Struct LC_Rx);



/**************************************************************************************************************
 * USAGE: This function converts the struct into a byte array and then send it to the TivaComm pThread to TX. 
 *
 * PARAMETERS:
 *            - OI_B2T_Struct StructToSend => The structure to send 
 * 
 * RETURNS: NONE
 **************************************************************************************************************/
void Send_OIStruct_ToTiva(OI_B2T_Struct StructToSend);


#endif /* LOADCELL_THREAD_H_ */
