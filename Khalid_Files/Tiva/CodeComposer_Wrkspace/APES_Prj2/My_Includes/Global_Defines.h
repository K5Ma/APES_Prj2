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


/////////////////////////////////////// COMMON BETWEEN TIVA AND BB ///////////////////////////////////////////////////////////
/***************************************
 *   Current Program Version define    *
 ***************************************/
#define CURRENT_VER 				"1.5"


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


/*****************************************
 * UART Bluetooth CMDs used in the code  *
 * logic to know where we are when TXing *
 * or RXing                              *
 *****************************************/
#define START_CMD                  "?"
#define CONFIRM_CMD                "#"
#define END_CMD                    "!"
#define END_CMD_CHAR               '!'


/*****************************************
 * String size defines                   *
 *****************************************/
//This define should be used when sizing
//strings that will be used to store
//messages in the system. Helps keeps
//things consistent
#define MSGSTR_SIZE                 150
#define LOG_SIZE                    20
#define SRC_SIZE                    21


/***************************************
 *     Max Structure size              *
 ***************************************/
#define MAX_STRUCT_SIZE				220


/***************************************
 *     Struct ID Enum                  *
 ***************************************/
typedef enum
{
	LogMsg_Struct_ID = 1,
	NFC_T2B_Struct_ID = 2

} Struct_ID;


/***************************************
 *  Struct used by Logger thread/task  *
 ***************************************/
typedef struct
{
	uint8_t ID;
	uint8_t Src;
	char LogLevel[LOG_SIZE];		//Expected values: INFO | WARNING | ERROR | CRITICAL | FATAL
	char Msg[MSGSTR_SIZE];
} LogMsg_Struct;

/***************************************
 *  Struct used by NFC thread/task  *
 ***************************************/
typedef struct
{
    uint8_t ID;
    uint8_t Src;
    uint8_t NFC_Tag_ID_Array[4];
} NFC_T2B_Struct;


/***************************************
 *   Log_Msg() Function Parameters:    *
 *   (found in Master_Functions.h)     *
 ***************************************/
#define LOGGER_AND_LOCAL			0x01
#define LOGGER_ONLY					0x02
#define LOCAL_ONLY					0x03


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


// OLD
///***************************************
// *     TiveBB Message Structure       *
// ***************************************/
//typedef struct TivaBB_MsgStruct
//{
//	/* NFC */
//	uint8_t T_NFC_Tag_ID_Array[4];
//	bool T_NFC_Error;
//
//	/* EPaper */
//	bool T_EP_Error;
//	bool B_Update_EPaper;
//	char B_Image_Name[10];
//
//	/* KeyPad */
//	uint8_t T_KeyPad_Code[6];
//	bool T_KeyPad_Error;
//	bool B_KeyPad_Poll;
//
//	/* Load Cell */
//	uint16_t T_LC_SamplesArraymv[20];
//	bool T_LC_Error;
//	bool B_Poll_LoadCell;
//
//	/* BME280 */
//	int16_t T_BME280_milliTemperatureCelcius;
//	uint16_t T_BME280_milliHumidityPercent;
//	bool T_BME280_Error;
//
//	/* Lux */
//	uint16_t T_Lux_Level;
//	bool T_Lux_Error;
//	bool B_Lux_Disable;
//
//	/* Gas */
//	uint16_t T_Gas_Level;
//	bool T_Gas_Error;
//
//	/* PIR */
//	bool T_PIR_State;
//	bool B_PIR_Disable;
//
//	/* SpeakJet */
//	uint8_t T_SJ_Data;
//
//	/* Servo */
//	bool B_Servo_Open;
//
//	/* Output Indicators */
//	uint8_t B_OI_Data;
//
//} TivaBB_MsgStruct;



#endif /* MY_INCLUDES_GLOBAL_DEFINES_H_ */
