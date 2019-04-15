/*
 * MyCircBuff.c
 *
 *  Created on: Nov 14, 2018
 *      Author: Khalid AlAwadhi
 *
 *  # ALL INFO CAN BE FOUND IN THE .h FILE #
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "My_CircBuff.h"

/* Includes for APES Prj2 */
#include "Global_Defines.h"
#include "My_UART.h"



int8_t CircBuff_Init(Circular_Buff_t* C_Buff, uint32_t buff_size)
{
	if(buff_size < 1)
	{
		UART_Putchar_n(UART0, "> ERROR: Buffer size must be greater than 0!\n\r");
		return -1;
	}

	C_Buff->buffer = (uint32_t*)malloc( buff_size * sizeof(uint32_t) );

	if(C_Buff->buffer == NULL)
	{
		UART_Putchar_n(UART0, "> ERROR: Unable to malloc buffer!\n\r");
		return -1;
	}

	C_Buff->head = 0;
	C_Buff->tail = 0;
	C_Buff->size = buff_size;
	C_Buff->full = false;

	UART_Putchar_n(UART0, "> Initialized buffer!\n\r");

	return 0;
}



int8_t CircBuff_Free(Circular_Buff_t* C_Buff)
{
	if(C_Buff->buffer == NULL)
	{
		UART_Putchar_n(UART0, "> ERROR: Buffer not initialized!\n");
		return -1;
	}

	free(C_Buff->buffer);
	C_Buff->buffer = NULL;
	C_Buff->head = 0;
	C_Buff->tail = 0;
	C_Buff->size = 0;
	C_Buff->full = false;

	UART_Putchar_n(UART0, "> Successfully freed circular buffer!\n");

	return 0;
}



int8_t CircBuff_Clear(Circular_Buff_t* C_Buff)
{
	if(C_Buff->buffer == NULL)
	{
		UART_Putchar_n(UART0, "> ERROR: Buffer not initialized!\n");
		return -1;
	}

	C_Buff->head = 0;
	C_Buff->tail = 0;
	C_Buff->full = false;

	UART_Putchar_n(UART0, "> Successfully cleared circular buffer!\n");

	return 0;
}



int8_t CircBuff_Add(Circular_Buff_t* C_Buff, uint32_t data)
{
	if(C_Buff->buffer == NULL)
	{
		UART_Putchar_n(UART0, "> ERROR: Buffer not initialized!\n");
		return -1;
	}

	C_Buff->buffer[C_Buff->head] = data;

	/* if the buffer is full, advance the tail */
	if(C_Buff->full)
	{
		C_Buff->tail = (C_Buff->tail + 1) % C_Buff->size;
		UART_Putchar_n(UART0, "> WARNING: Buffer is full! Old data will be overwritten\n");
	}

	/* Increment the head - Using an efficient way ;) */
	C_Buff->head = (C_Buff->head + 1) % C_Buff->size;

	/* Check if the buffer is full and store in flag */
	C_Buff->full = (C_Buff->head == C_Buff->tail);

	return 0;
}



int8_t CircBuff_Remove(Circular_Buff_t* C_Buff, uint32_t* RemovedData)
{
	if(C_Buff->buffer == NULL)
	{
		UART_Putchar_n(UART0, "> ERROR: Buffer not initialized!\n");
		return -1;
	}

    if( !IsBuffEmpty(C_Buff) )
    {
    	*RemovedData = C_Buff->buffer[C_Buff->tail];

    	C_Buff->full = false;
    	C_Buff->tail = (C_Buff->tail + 1) % C_Buff->size;
        return 0;
    }

    UART_Putchar_n(UART0, "> ERROR: Buffer is empty!\n");
    return -1;
}



int8_t CircBuff_Peek(Circular_Buff_t* C_Buff, uint32_t* Data)
{
	if(C_Buff->buffer == NULL)
	{
		UART_Putchar_n(UART0, "> ERROR: Buffer not initialized!\n");
		return -1;
	}

    if( !IsBuffEmpty(C_Buff) )
    {
    	*Data = C_Buff->buffer[C_Buff->tail];
        return 0;
    }

    UART_Putchar_n(UART0, "> ERROR: Buffer is empty!\n");
    return -1;
}



bool IsBuffEmpty(Circular_Buff_t* C_Buff)
{
    return (!C_Buff->full && (C_Buff->head == C_Buff->tail));
}



uint32_t CircBuff_Size(Circular_Buff_t* C_Buff)
{
	if(C_Buff->buffer == NULL)
	{
		UART_Putchar_n(UART0, "> ERROR: Buffer not initialized!\n");
		return 0;
	}

    return C_Buff->size;
}



uint32_t CircBuff_Num_of_Elements(Circular_Buff_t* C_Buff)
{
	if(C_Buff->buffer == NULL)
	{
		UART_Putchar_n(UART0, "> ERROR: Buffer not initialized!\n");
		return 0;
	}

	uint32_t NumElems = C_Buff->size;

    if(!C_Buff->full)
    {
        if(C_Buff->head >= C_Buff->tail)
        {
        	NumElems = (C_Buff->head - C_Buff->tail);
        }
        else
        {
        	NumElems = (C_Buff->size + C_Buff->head - C_Buff->tail);
        }
    }

    return NumElems;
}



int8_t CircBuff_PrintAll(Circular_Buff_t* C_Buff)
{
	if(C_Buff->buffer == NULL)
	{
		UART_Putchar_n(UART0, "> ERROR: Buffer not initialized!\n");
		return -1;
	}

	printf("> Printing buffer contents...\n");

	if(IsBuffEmpty(C_Buff))
    {
		UART_Putchar_n(UART0, "> ERROR: Buffer is empty!\n");
        return -1;
    }
	else
	{
		uint32_t index = C_Buff->tail;
		uint32_t count = 0;
		while( count < CircBuff_Num_of_Elements(C_Buff) )
		{
			/* This was split up to accommodate printing on different platforms */
			UART_Putchar_n(UART0, "> Element ");

			char count_char[10];

			snprintf(count_char, sizeof(count_char), "%u", count);
	//		__itoa(count, count_char);

			printf(count_char);

	//		printf(": [");

			char buffelem_char[10];
			snprintf(buffelem_char, sizeof(buffelem_char), "%u", C_Buff->buffer[index]);
	//		__itoa(C_Buff->buffer[index], buffelem_char);
			printf(buffelem_char);

	//		printf("]\n");

			index = (index + 1) % C_Buff->size;
			count++;
		}
	}

	return 0;
}



int8_t CircBuff_Resize(Circular_Buff_t* C_Buff, uint32_t new_buff_size)
{

	uint32_t* TempPtr = C_Buff->buffer;

	if(C_Buff->buffer == NULL)
	{
		UART_Putchar_n(UART0, "> ERROR: Buffer not initialized!\n");
		return -1;
	}

	if(new_buff_size < 1)
	{
		UART_Putchar_n(UART0, "> ERROR: Buffer size must be greater than 0!\n");
		return -1;
	}

	C_Buff->buffer = (uint32_t*)realloc(C_Buff->buffer, new_buff_size);

	if(C_Buff->buffer == NULL)
	{
		UART_Putchar_n(UART0, "> ERROR: Unable to realloc buffer!\n");
		C_Buff->buffer = TempPtr;
		return -1;
	}

	C_Buff->size = new_buff_size;
	C_Buff->full = false;

	UART_Putchar_n(UART0, "> Success: Buffer resized!\n");

	return 0;
}
