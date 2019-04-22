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

/***************************************
 *  Thread Numbering Enum:             *
 *  Used for source and destination    *   
 ***************************************/
typedef enum
{
	Main = 1,
	Logger = 2,
	TivaComm = 3
} Sources;


/*****************************************
 * This define should be used when sizing*
 * strings that will be used to store    *
 * messages in the system. Helps keeps   *
 * things consistent.                    *
 *****************************************/
#define MSGSTR_SIZE					200
 

/***************************************
 *     POSIX Message Structure         *
 ***************************************/
typedef struct POSIX_MsgStruct 
{
	uint8_t Source;
	uint8_t Dest;
	char LogLevel[MSGSTR_SIZE];		//Expected values: INFO | WARNING | ERROR | CRITICAL | FATAL
	char Msg[MSGSTR_SIZE];
} POSIX_MsgStruct;


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
#define LOGGER_QUEUE				"/LOGGER_POSIX_Q"



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






#endif /* GLOBAL_DEFINES_H_ */
