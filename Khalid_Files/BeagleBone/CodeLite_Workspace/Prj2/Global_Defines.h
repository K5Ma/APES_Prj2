#ifndef GLOBAL_DEFINES_H_
#define GLOBAL_DEFINES_H_

//#include <stdio.h>
//#include <stdlib.h>
//
//#include <stdbool.h>

//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/time.h>
//#include <sys/ioctl.h>
//#include <sys/syscall.h>
//#include <sys/socket.h>
//#include <sys/wait.h>


//#include <assert.h>
//#include <errno.h>

//
//
//
//#include <signal.h>
//#include <malloc.h>

//#include <fcntl.h>
//#include <linux/i2c-dev.h>
//#include <math.h>
//#include <float.h>
//#include <complex.h>
//#include <time.h>
//#include <mqueue.h>

#include <stdint.h>
#include <stdbool.h>


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
 #define END_CMD                    '!'


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




//typedef struct POSIX_MsgStruct 
//{
//	uint8_t Src;
//	uint8_t Dest;
//	char LogLevel[MSGSTR_SIZE];		//Expected values: INFO | WARNING | ERROR | CRITICAL | FATAL
//	char Msg[MSGSTR_SIZE];
//} POSIX_MsgStruct;


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
#define MAIN_POSIX_Q				"/MAIN_PQ"
#define LOGGER_POSIX_Q				"/LOGGER_PQ"
#define TIVACOMM_POSIX_Q			"/TIVACOMM_PQ"
#define NFC_POSIX_Q					"/NFC_PQ"
#define KEYPADEPAPER_POSIX_Q		"/KEYPADEPAPER_PQ"
#define LOADCELL_POSIX_Q			"/LOADCELL_PQ"




/***************************************
 *   Log_Msg() Function Parameters:    * 
 *   (found in Master_Functions.h)     *
 ***************************************/
#define LOGGER_AND_LOCAL			0x01
#define LOGGER_ONLY					0x02
#define LOCAL_ONLY					0x03 


/*****************************************
 * The define below should be used when  *
 * initializing a string that will store *
 * the current system real time using    *
 * GetRealTime()                         *
 *****************************************/
#define TIMESTR_SIZE				26



/***************************************
 *     TiveBB Message Structure       *
 ***************************************/
 typedef struct TivaBB_MsgStruct 
{
	/* Source pThread */
	uint8_t Src;
	
	/* Destination Task */
	uint8_t Dest;
	
} TivaBB_MsgStruct;

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



#endif /* GLOBAL_DEFINES_H_ */
