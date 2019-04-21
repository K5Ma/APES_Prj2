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
	TivaComm = 2,
	Logger = 3
} Sources;


/***************************************
 *        Message Structure            *
 ***************************************/
typedef struct MsgStruct 
{
	uint8_t Source;
	uint8_t Dest;
	char LogLevel[150];				//Expected values: INFO | WARNING | ERROR | CRITICAL
	char Msg[150];
} MsgStruct;


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
#define MAIN_QUEUE					"/MAIN_POSIX_Q"
#define LOGGING_QUEUE				"/LOGGING_POSIX_Q"



/***************************************
 *  Log_error() Function Parameters:   *
 *   (found in Master_Functions.h)     *
 ***************************************/
#define LOGGING_AND_LOCAL			0x01
#define LOGGING_ONLY				0x02
#define LOCAL_ONLY					0x03 
 
 




#endif /* GLOBAL_DEFINES_H_ */
