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
#define PRIORITY_BB_COMM_TASK			0



/***************************************
 *  Task Numbering Enum:               *
 *  Used for source and destination    *
 ***************************************/
typedef enum
{
	Main = 1,
	BB_Comm = 2
} Sources;



/*****************************************
 *      Message Structure                *
 *****************************************/
typedef struct MsgStruct
{
	float Timestamp;
	uint8_t Source;
	char LogLevel[20];
	char Msg[80];
	float Data_float1;
	float Data_float2;
} MsgStruct;




#endif /* MY_INCLUDES_GLOBAL_DEFINES_H_ */
