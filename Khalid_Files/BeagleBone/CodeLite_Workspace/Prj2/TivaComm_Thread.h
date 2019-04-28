/*
 * TivaComm_Thread.h
 *
 *  Created on: Apr 20, 2019
 *      Author: Khalid AlAwadhi
 */

#ifndef TIVACOMM_THREAD_H_
#define TIVACOMM_THREAD_H_

#include <stdint.h>


/**************************************************************************************************************
 * USAGE: This function contains all what the TivaComm pThread will do.
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void * TivaCommThread(void * args);



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


#endif /* TIVACOMM_THREAD_H_ */
