/*
 * Global_Defines.h
 *
 *  Created on: Apr 8, 2019
 *      Author: Khalid AlAwadhi
 */

#ifndef MY_INCLUDES_GLOBAL_DEFINES_H_
#define MY_INCLUDES_GLOBAL_DEFINES_H_


/*****************************************
 *      System Clock Rate, 120 MHz       *
 *****************************************/
#define SYSTEM_CLOCK    120000000U



/*****************************************
 *      Priorities of Tasks              *
 *****************************************/
#define PRIORITY_LOGGER_TASK			0


/*****************************************
 *      UART Defines                     *
 *****************************************/
#define UART_PORT						0
#define UART_BAUD						9600


/***************************************
 *  Task Numbering Enum:               *
 *  Used for source and destination    *
 ***************************************/
typedef enum
{
	Main = 1
} Sources;



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



/*****************************************
 *      Message Structure                *
 *****************************************/
typedef struct MsgStruct
{
	uint8_t Source;
	char Msg[100];
	float Value;
} MsgStruct;




#endif /* MY_INCLUDES_GLOBAL_DEFINES_H_ */
