/*
 * MyCircBuff.h
 *
 *  Created on: Nov 14, 2018
 *      Author: Khalid AlAwadhi
 * 
 ---------------------------------------------------------------------------------------------
 * # LIBRARY VERSION: v1.0
 *
 * # INFO: A circular buffer library I developed to use in various projects.
 *
 * # DEVLEPOMENT: Last modified to work on Code Composer v9.0 for the TI-Ticva board.
 *
 * # LAST UPDATED ON: April 2019 - Used in APES Project 2
 *
 * # NOTES:
 * - NONE
 * 
 * # FUTURE PLANS:
 * - NONE
 *
 * # CURRENTLY CHANGED:
 *		- uint32_t CircBuff_Size(Circular_Buff_t* C_Buff) used to return -1 if buffer was
 *		  not init. Bug is that -1 will be converted to 0. uint32_t CircBuff_Num_of_Elements()
 *		  had the same issue as well.
 *		-
 ---------------------------------------------------------------------------------------------*/

#ifndef MY_CIRCBUFF_H_
#define MY_CIRCBUFF_H_

#include <stdint.h>
#include <stdbool.h>


/* My Circular Buffer structure */
typedef struct CircularBuff_t
{
	uint32_t* buffer;		//Stores data
	uint32_t head;
	uint32_t tail;
    uint32_t size; 			//Size of buffer
    bool full;				//Flag to indicate if buffer is full or not
}Circular_Buff_t;


/*****************************************************************************
 * Info: This function will initialize a circular buffer instance.
 *
 * Parameters:
 *             - Circular_Buff_t* C_Buff: Pass an uninitialized Circular_Buff_t
 *             							  variable
 *
 *             - uint32_t size: desired size of buffer
 *
 * Returns: - Success: 0
 *          - Failure: -1
 *****************************************************************************/
int8_t CircBuff_Init(Circular_Buff_t* C_Buff, uint32_t buff_size);



/*****************************************************************************
 * Info: This function will free a circular buffer instance.
 *
 * Parameters:
 *             - Circular_Buff_t* C_Buff: Pass the Circular_Buff_t variable
 *                                        you want to free
 *
 * Returns: - Success: 0
 *          - Failure: -1
 *****************************************************************************/
int8_t CircBuff_Free(Circular_Buff_t* C_Buff);



/*****************************************************************************
 * Info: This function will clear contents in a circular buffer instance.
 *
 * Parameters:
 *             - Circular_Buff_t* C_Buff: Pass the Circular_Buff_t variable
 *                                        you want to clear
 *
 * Returns: - Success: 0
 *          - Failure: -1
 *****************************************************************************/
int8_t CircBuff_Clear(Circular_Buff_t* C_Buff);



/*****************************************************************************
 * Info: This function will add data to the circular buffer overwriting old
 *       if the buffer is full.
 *
 * Parameters:
 *             - Circular_Buff_t* C_Buff: Pass the Circular_Buff_t variable
 *                                        you want to add data to
 *
 *             - uint32_t data: Data to store
 *
 * Returns: - Success: 0
 *          - Failure: -1
 *****************************************************************************/
int8_t CircBuff_Add(Circular_Buff_t* C_Buff, uint32_t data);



/*****************************************************************************
 * Info: This function will return the oldest data from the circular buffer
 *       and update the tail pointer.
 *
 * Parameters:
 *             - Circular_Buff_t* C_Buff: Pass the Circular_Buff_t variable
 *                                        you want to get data from
 *             - uint32_t* RemovedData: Removed data will be stored here
 *
 * Returns: - Success: 0
 *          - Failure: -1
 *****************************************************************************/
int8_t CircBuff_Remove(Circular_Buff_t* C_Buff, uint32_t* RemovedData);


/*****************************************************************************
 * Info: This function will return the oldest data from the circular buffer
 *       without removing it from the buffer.
 *
 * Parameters:
 *             - Circular_Buff_t* C_Buff: Pass the Circular_Buff_t variable
 *                                        you want to get data from
 *             - uint32_t* Data: Removed data will be stored here
 *
 * Returns: - Success: 0
 *          - Failure: -1
 *****************************************************************************/
int8_t CircBuff_Peek(Circular_Buff_t* C_Buff, uint32_t* Data);



/*****************************************************************************
 * Info: This function checks if the buffer is empty or not.
 *
 * Parameters:
 *             - Circular_Buff_t* C_Buff: Pass a Circular_Buff_t variable
 *
 * Returns: - bool empty: TRUE => Buffer is empty
 *      			      FALSE => Buffer is not empty
 *****************************************************************************/
bool IsBuffEmpty(Circular_Buff_t* C_Buff);



/*****************************************************************************
 * Info: This function returns the size of the buffer.
 *
 * Parameters:
 *             - Circular_Buff_t* C_Buff: Pass a Circular_Buff_t variable
 *
 * Returns: - uint32_t Size_of_buffer
 *****************************************************************************/
uint32_t CircBuff_Size(Circular_Buff_t* C_Buff);



/*****************************************************************************
 * Info: This function returns the number of elements in the buffer.
 *
 * Parameters:
 *             - Circular_Buff_t* C_Buff: Pass a Circular_Buff_t variable
 *
 * Returns: - uint32_t Num_of_Elements
 *****************************************************************************/
uint32_t CircBuff_Num_of_Elements(Circular_Buff_t* C_Buff);



/*****************************************************************************
 * Info: This function displays all the contents of the buffer.
 *
 * Parameters:
 *             - Circular_Buff_t* C_Buff: Pass a Circular_Buff_t variable
 *
 * Returns: - Success: 0
 *          - Failure: -1
 *****************************************************************************/
int8_t CircBuff_PrintAll(Circular_Buff_t* C_Buff);



/*****************************************************************************
 * Info: This function resizes a buffer, If a buffer can't be reallocated,
 *       it will just stay the same size.
 *
 * Parameters:
 *             - Circular_Buff_t* C_Buff: Pass a Circular_Buff_t variable
 *             - uint32_t new_buff_size: New buffer size
 *
 * Returns: - Success: 0
 *          - Failure: -1
 *****************************************************************************/
int8_t CircBuff_Resize(Circular_Buff_t* C_Buff, uint32_t new_buff_size);



#endif /* MY_CIRCBUFF_H_ */
