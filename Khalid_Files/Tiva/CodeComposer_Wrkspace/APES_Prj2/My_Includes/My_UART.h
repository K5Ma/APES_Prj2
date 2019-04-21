/*
 * My_UART.h
 *
 *  Created on: Apr 14, 2019
 *      Author: Khalid AlAwadhi
 ---------------------------------------------------------------------------------------------
 * # LIBRARY VERSION: v1.0
 *
 * # INFO: A UART Driver library for the Tiva EK-TM4C129XL board
 *
 * # DEVLEPOMENT: Developed using Code Composer Studio v9.0
 *
 * # LAST UPDATED ON: April 2019 - Used in APES Project 2
 *
 * # NOTES:
 * - Influenced by the example code provided by TI (uartstdio.h), which helped me understand
 *   where and how to begin creating my own driver library.
 *
 *  # FUTURE PLANS:
 *  - NONE
 ---------------------------------------------------------------------------------------------*/

#ifndef MY_INCLUDES_MY_UART_H_
#define MY_INCLUDES_MY_UART_H_

#include <stdint.h>


/**************************************************************************************************************
 * USAGE: Helper function called from UART_Putchar_n(). Simply sends a char to the chosen UART.
 *
 * PARAMETERS:
 *            - uint32_t UART_BASE => UART base address
 *            - char TX_Char => Char to send
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void UART_Putchar(uint32_t UART_BASE, char TX_Char);



/**************************************************************************************************************
 * USAGE: This function send a string to the chosen UART.
 *
 * PARAMETERS:
 * 			  - uint8_t UART_Num => The UART to initialize (valid parameters 0-7 as the Tiva only supports 8 UARTs.
 * 			  											    Or simply use UARTx (ex: UART1) but you must include
 * 			  											    Global_Defines.h)
 *            - char* TX_String => String to send
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void UART_Putchar_n(uint8_t UART_Num, char* TX_String);



/**************************************************************************************************************
 * USAGE: This function initializes any of the UARTs on the Tiva board.
 *				- Init the needed PINs (Depends on the UART being init)
 *				- Set PIN type to UART PINs
 *				- Enable the UARTx peripheral
 *				- Configure UART: No Parity, 8 BITs, 1 Stop BIT
 *				- Enables UARTx
 *
 * PARAMETERS:
 * 			  - uint8_t UART_Num => The UART to initialize (valid parameters 0-7 as the Tiva only supports 8 UARTs.
 * 			  											    Or simply use UARTx (ex: UART1) but you must include
 * 			  											    Global_Defines.h)
 *            - uint32_t Clock => Clock source to config UART at
 *            - uint32_t BaudRate => The wanted Baud Rate
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void Init_UARTx(uint8_t UART_Num, uint32_t Clock, uint32_t BaudRate);



#endif /* MY_INCLUDES_MY_UART_H_ */
