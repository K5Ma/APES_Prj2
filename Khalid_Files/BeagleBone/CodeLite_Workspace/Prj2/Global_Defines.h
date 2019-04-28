#ifndef GLOBAL_DEFINES_H_
#define GLOBAL_DEFINES_H_

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
	LogMsg_Struct_ID = 1
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


//Struct used by NFC thread/task
typedef struct
{
	uint8_t ID;
	uint8_t Src;
	//ADD STUFF
} NFC_T2B_Struct;


/***************************************
 *   Log_Msg() Function Parameters:    *
 *   (found in Master_Functions.h)     *
 ***************************************/
#define LOGGER_AND_LOCAL			0x01
#define LOGGER_ONLY					0x02
#define LOCAL_ONLY					0x03


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***************************************
 *      pThread Argument Structure     *
 ***************************************/
typedef struct Pthread_ArgsStruct
{
    char LogFile_Path[100];			//Used to store the wanted logfile path
}Pthread_ArgsStruct;


/***************************************
 *          POSIX Queues               *
 ***************************************/
#define MAIN_POSIX_Q                   "/MAIN_PQ"
#define LOGGER_POSIX_Q                 "/LOGGER_PQ"
#define TIVACOMM_POSIX_Q               "/TIVACOMM_PQ"
#define NFC_POSIX_Q                    "/NFC_PQ"
#define KEYPADEPAPER_POSIX_Q	       "/KEYPADEPAPER_PQ"
#define LOADCELL_POSIX_Q               "/LOADCELL_PQ"


/*****************************************
 * The define below should be used when  *
 * initializing a string that will store *
 * the current system real time using    *
 * GetRealTime()                         *
 *****************************************/
#define TIMESTR_SIZE				26



#endif /* GLOBAL_DEFINES_H_ */
