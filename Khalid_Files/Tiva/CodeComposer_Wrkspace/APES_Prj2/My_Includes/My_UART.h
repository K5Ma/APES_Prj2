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
 *  - Use UARTIntRegister() instead of modifying vector table - makes library more portable.
 *  -
 ---------------------------------------------------------------------------------------------*/

#ifndef MY_INCLUDES_MY_UART_H_
#define MY_INCLUDES_MY_UART_H_

#include <stdint.h>
#include <stdbool.h>


/***************************************
 *  All available UARTs Enums:         *
 *  Used in various functions to make  *
 *  things simpler                     *
 ***************************************/
typedef enum
{
	UART0 = 0,
	UART1 = 1,
	UART2 = 2,
	UART3 = 3,
	UART4 = 4,
	UART5 = 5,
	UART6 = 6,
	UART7 = 7
} UART_Bases;




/**************************************************************************************************************
 * USAGE: This function returns a char from the chosen UART (Blocking).
 *
 * PARAMETERS:
 *            - uint8_t UART_Num => The UART port to read a char from
 *
 * RETURNS: char Read_Char
 **************************************************************************************************************/
char UART_Getchar(uint8_t UART_Num);



/**************************************************************************************************************
 * USAGE: Helper function called from UART_Putchar_n(). Simply sends a char to the chosen UART (Blocking).
 *
 * PARAMETERS:
 *            - uint32_t UART_BASE => UART base address
 *            - char TX_Char => Char to send
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void UART_Putchar(uint32_t UART_BASE, char TX_Char);



/**************************************************************************************************************
 * USAGE: This function send a string to the chosen UART (Blocking).
 *
 * PARAMETERS:
 * 			  - uint8_t UART_Num => The UART to initialize (valid parameters 0-7 as the Tiva only supports 8 UARTs.
 * 			  											    Or simply use UARTx (ex: UART1)
 *            - char* TX_String => String to send
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void UART_Putchar_n(uint8_t UART_Num, char* TX_String);



/**************************************************************************************************************
 * USAGE: This function sends a uint8_t array (buffer) that contains all the data for a struct to the
 *        BeagleBone.
 *
 * PARAMETERS:
 *            - UART_Struct *UART => The UART port to send the struct to
 *            - uint8_t* StructToSend => Buffer structure to send
 *
 * RETURNS: Success => true
 *          Failure => false
 **************************************************************************************************************/
bool Send_StructBuffer_UARTx(uint8_t UART_Num, uint8_t* StructToSend);



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
 *            - bool Interrupt_EN => TRUE: Enable RX interrupts for the desired UART
 *            						 FALSE: Disable RX interrupts for the desired UART
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void Init_UARTx(uint8_t UART_Num, uint32_t Clock, uint32_t BaudRate, bool Interrupt_EN);



/**************************************************************************************************************
 * USAGE: IRQ Handler for UART0. Make sure this function name is exactly as what the processor expects (meaning
 * 		  make sure to modify the vector table with the same function call if you did not already or you can
 * 		  call UARTIntRegister(UARTxBASE, YourHandler) )
 *
 * PARAMETERS:
 * 			  - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void UART0_IntHandler();

/**************************************************************************************************************
 * USAGE: IRQ Handler for UART1. Make sure this function name is exactly as what the processor expects (meaning
 * 		  make sure to modify the vector table with the same function call if you did not already or you can
 * 		  call UARTIntRegister(UARTxBASE, YourHandler) )
 *
 * PARAMETERS:
 * 			  - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void UART1_IntHandler();

/**************************************************************************************************************
 * USAGE: IRQ Handler for UART2. Make sure this function name is exactly as what the processor expects (meaning
 * 		  make sure to modify the vector table with the same function call if you did not already or you can
 * 		  call UARTIntRegister(UARTxBASE, YourHandler) )
 *
 * PARAMETERS:
 * 			  - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void UART2_IntHandler();

/**************************************************************************************************************
 * USAGE: IRQ Handler for UART3. Make sure this function name is exactly as what the processor expects (meaning
 * 		  make sure to modify the vector table with the same function call if you did not already or you can
 * 		  call UARTIntRegister(UARTxBASE, YourHandler) )
 *
 * PARAMETERS:
 * 			  - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void UART3_IntHandler();

/**************************************************************************************************************
 * USAGE: IRQ Handler for UART4. Make sure this function name is exactly as what the processor expects (meaning
 * 		  make sure to modify the vector table with the same function call if you did not already or you can
 * 		  call UARTIntRegister(UARTxBASE, YourHandler) )
 *
 * PARAMETERS:
 * 			  - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void UART4_IntHandler();

/**************************************************************************************************************
 * USAGE: IRQ Handler for UART5. Make sure this function name is exactly as what the processor expects (meaning
 * 		  make sure to modify the vector table with the same function call if you did not already or you can
 * 		  call UARTIntRegister(UARTxBASE, YourHandler) )
 *
 * PARAMETERS:
 * 			  - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void UART5_IntHandler();

/**************************************************************************************************************
 * USAGE: IRQ Handler for UART6. Make sure this function name is exactly as what the processor expects (meaning
 * 		  make sure to modify the vector table with the same function call if you did not already or you can
 * 		  call UARTIntRegister(UARTxBASE, YourHandler) )
 *
 * PARAMETERS:
 * 			  - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void UART6_IntHandler();

/**************************************************************************************************************
 * USAGE: IRQ Handler for UART7. Make sure this function name is exactly as what the processor expects (meaning
 * 		  make sure to modify the vector table with the same function call if you did not already or you can
 * 		  call UARTIntRegister(UARTxBASE, YourHandler) )
 *
 * PARAMETERS:
 * 			  - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void UART7_IntHandler();





#endif /* MY_INCLUDES_MY_UART_H_ */
