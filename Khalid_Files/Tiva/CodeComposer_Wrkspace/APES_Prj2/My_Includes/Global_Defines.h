/*
 * Global_Defines.h
 *
 *  Created on: Apr 8, 2019
 *      Author: Khalid AlAwadhi
 */

#ifndef MY_INCLUDES_GLOBAL_DEFINES_H_
#define MY_INCLUDES_GLOBAL_DEFINES_H_

#include <stdint.h>
#include <stdbool.h>



/***************************************
 *   Current Program Version define    *
 ***************************************/
#define CURRENT_VER 				"1.4"


/*****************************************
 *      System Clock Rate, 120 MHz       *
 *****************************************/
#define SYSTEM_CLOCK    120000000U


/*****************************************
 *      Priorities of Tasks              *
 *****************************************/
#define PRIORITY_LOGGER_TASK			0
#define PRIORITY_BBCOMM_TASK			0
#define PRIORITY_NFC_TASK				0
#define PRIORITY_KEYPADEPAPER_TASK		0
#define PRIORITY_LOADCELL_TASK			0
#define PRIORITY_LUX_TASK				0
#define PRIORITY_SERVO_TASK				0
#define PRIORITY_SPEAKJET_TASK			0
#define PRIORITY_OUTPUTS_TASK			0



/***************************************
 *  Task and Thread Numbering Enum:    *
 *  Used for source and destination    *
 ***************************************/
typedef enum
{
	/* Tiva Sources */
	T_Main = 1,
	T_BBComm = 2,
	T_NFC = 3,
	T_KeypadEpaper = 4,
	T_LoadCell = 5,
	T_Lux = 6,
	T_Servo = 7,
	T_SpeakJet = 8,
	T_Outputs = 9,
	T_Logger = 16,
	/* BeagleBone Sources */
	BB_Main = 10,
	BB_Logger = 11,
	BB_TivaComm = 12,
	BB_NFC = 13,
	BB_KeypadEpaper = 14,
	BB_LoadCell = 15
} Sources;


/***************************************
 *   Log_Msg() Function Parameters:    *
 *   (found in Master_Functions.h)     *
 ***************************************/
#define LOGGER_AND_LOCAL            0x01
#define LOGGER_ONLY                 0x02
#define LOCAL_ONLY                  0x03



/*****************************************
 * String size defines                   *
 *****************************************/
//This define should be used when sizing
//strings that will be used to store
//messages in the system. Helps keeps
//things consistent
#define MSGSTR_SIZE					200
#define SRC_SIZE					21


/***************************************
 *     All Structures                  *
 ***************************************/
#define MAX_STRUCT_SIZE				100

//Struct used by Logger thread/task:
typedef struct Log_MsgStruct
{
	uint8_t ID;
	uint8_t Src;
	char LogLevel[MSGSTR_SIZE];		//Expected values: INFO | WARNING | ERROR | CRITICAL | FATAL
	char Msg[MSGSTR_SIZE];
} Log_MsgStruct;


/***************************************
 *     TiveBB Message Structure       *
 ***************************************/
typedef struct TivaBB_MsgStruct
{
	/* NFC */
	uint8_t T_NFC_Tag_ID_Array[4];
	bool T_NFC_Error;

	/* EPaper */
	bool T_EP_Error;
	bool B_Update_EPaper;
	char B_Image_Name[10];

	/* KeyPad */
	uint8_t T_KeyPad_Code[6];
	bool T_KeyPad_Error;
	bool B_KeyPad_Poll;

	/* Load Cell */
	uint16_t T_LC_SamplesArraymv[20];
	bool T_LC_Error;
	bool B_Poll_LoadCell;

	/* BME280 */
	int16_t T_BME280_milliTemperatureCelcius;
	uint16_t T_BME280_milliHumidityPercent;
	bool T_BME280_Error;

	/* Lux */
	uint16_t T_Lux_Level;
	bool T_Lux_Error;
	bool B_Lux_Disable;

	/* Gas */
	uint16_t T_Gas_Level;
	bool T_Gas_Error;

	/* PIR */
	bool T_PIR_State;
	bool B_PIR_Disable;

	/* SpeakJet */
	uint8_t T_SJ_Data;

	/* Servo */
	bool B_Servo_Open;

	/* Output Indicators */
	uint8_t B_OI_Data;

} TivaBB_MsgStruct;



#endif /* MY_INCLUDES_GLOBAL_DEFINES_H_ */
