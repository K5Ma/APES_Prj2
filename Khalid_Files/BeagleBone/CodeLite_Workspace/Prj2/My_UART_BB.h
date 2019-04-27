/*
 * My_UART_BB.h
 *
 *  Created on: Apr 20, 2019
 *      Author: Khalid AlAwadhi
 ---------------------------------------------------------------------------------------------
 * # LIBRARY VERSION: v1.0
 *
 * # INFO: A UART Driver library for the BeagleBone Green board
 *
 * # DEVLEPOMENT: Developed using CodeLite on Debian 8.3
 *
 * # LAST UPDATED ON: April 2019 - Used in APES Project 2
 *
 * # NOTES:
 * - The base code of this library was taken from: 
 *   https://github.com/sijpesteijn/BBCLib
 * 
 *  # FUTURE PLANS:
 *  - NONE
 ---------------------------------------------------------------------------------------------*/
 
#ifndef MY_UART_BB_H_
#define MY_UART_BB_H_

#include <stdint.h>
#include "Global_Defines.h" 

/* Available UARTs on my BeagleBone Enum */
typedef enum
{
	UART0 = 0,
	UART1 = 1,
	UART2 = 2,
	UART3 = 3
} UART_Num;


/* Structure to hold porpoerties of each open UART */
typedef struct
{
	int fd;
	UART_Num UART_ID;			//Valid parameters are in the UART_Num enum
	uint32_t BaudRate;
} UART_Struct;
 
 
 
 
 /**************************************************************************************************************
 * USAGE: Signal handler for UART1.
 *
 * PARAMETERS:
 * 			  - int Status =>
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void UART1_RX_Signal_Handler(int Status);

 
 
/**************************************************************************************************************
 * USAGE: This function inits and opens a chosen UART port on the BeagleBone Green board. If UART1 is chosen,
 *        it will also enable RX signal interruppts. 
 *
 *        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *        !! Note: You MUST make sure you initialized the desired UARTs in the system itself! !!
 *        !!	   Run ls /dev/ttyO* and you should see an output like this:				  !!
 * 	      !!	   # /dev/ttyO0  /dev/ttyO1	/dev/ttyO2  /dev/ttyO3							  !!
 *        !!	   Each of these map to an enabled UART on the BeagleBone system, O0 is UART0,!!
 *		  !!	   O1 is UART1, O2 is UART2 and O3 is UART3. Your system might have more or   !!
 *        !!	   less so double check before calling this function.						  !!
 *        !!	   Do not use UART0 as that is used by the BeagleBone itself (did not test)	  !!
 *        !!       To enable PINs try for example: $config-pin P9.21 uart                     !!
 * 	      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 * PARAMETERS:
 * 			  - UART_Struct *UART => The UART structure to open
 *                                   (Make sure you malloc it! Example:
 *                                    > UART_Struct *BB_UART1 = malloc(sizeof(UART_Struct));
 *                                    > BB_UART1->UART_ID = UART1;
 *                                    > BB_UART1->BaudRate = B9600;)
 *
 * RETURNS: Success => 0
 *          Failure => -1
 **************************************************************************************************************/
int8_t Init_UARTx(UART_Struct *UART);



/**************************************************************************************************************
 * USAGE: This function closes an open UART port on the BeagleBone Green board.
 *
 * PARAMETERS:
 * 			  - UART_Struct *UART => The UART structure to close
 *
 * RETURNS: Success => 0
 * 			Failure	=> -1
 **************************************************************************************************************/
int8_t Close_UARTx(UART_Struct *UART);



/**************************************************************************************************************
 * USAGE: This function send a string to the wanted UART port.
 * 
 * PARAMETERS:
 * 			  - UART_Struct *UART => The UART structure to close
 * 			  - char *TX_String => String to transmit 
 *
 * RETURNS: Success => 0
 * 			Failure	=> -1
 **************************************************************************************************************/
int8_t Send_String_UARTx(UART_Struct *UART, char *TX_String);



/**************************************************************************************************************
 * USAGE: This function sends a TivaBB_MsgStruct structure to the wanted UART port.
 * 
 * PARAMETERS:
 *            - UART_Struct *UART => The UART port to send the struct to
 *            - TivaBB_MsgStruct StructToSend => Structure to send
 *
 * RETURNS: Success => true
 *          Failure => false
 **************************************************************************************************************/
bool Send_Struct_UARTx(UART_Struct *UART, TivaBB_MsgStruct StructToSend);



/**************************************************************************************************************
 * USAGE: This function XXX
 * 
 * PARAMETERS:
 * 			  - UART_Struct *UART => The UART structure to close
 * 			  - char *RX_String => XXX
 *
 * RETURNS: Success => int16_t Number_of_bytes_read
 * 			Failure	=> -1
 **************************************************************************************************************/
int16_t Read_UARTx(UART_Struct *UART, char *RX_String);


#endif /* MY_UART_BB_H_ */