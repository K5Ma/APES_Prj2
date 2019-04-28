/*
 * NFCTask.c
 *
 *  Created on: Apr 17, 2019
 *  Last Update: Apr 22, 2019
 *      Author: Poorn Mehta
 *
 *  Driver for reading NFC Tag ID from NFC Module v1.0
 *  (manufactured by DFRobot) which is connected to
 *  TM4C1294XL (TIVA Development Board) using UART Bus
 *
 *  Note: This module can do a lot more than just reading
 *  prefixed tag ID (like reading additional personalized
 *  data, including passwords and many more things; also
 *  this module can write the Tags as well) but due to
 *  severe lack of time, I am forced to leave all that
 *  aside for now. Hopefuily, I will get back to this in
 *  the future, and write a complete library.
 *
 *  This driver code is developed based on provided documentation from the manufacturer
 *
 */

/*
 *
 * Hardware Connections
 *
 *
 * Note: Left Side - Module Pins & Right Side - Controller Pins
 *
 * TXD - PC6 (This is the UART5Rx for Controller)
 * RXD - PC7 (This is the UART5Tx for Controller)
 * 5V - 5V
 * GND - Ground
 * 3.3V - 3.3V
 *
 */

#include "NFC_Task.h"

//Standard includes

//TivaWare includes
// Standard Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "drivers/pinout.h"
#include "utils/uartstdio.h"

// Third Party Includes
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_i2c.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "inc/hw_adc.h"
#include "inc/hw_ssi.h"

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/fpu.h"
#include "driverlib/adc.h"
#include "driverlib/ssi.h"


//FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"
#include "My_UART.h"

/* Global Variables */
NFC_T2B_Struct NFC_Tx;

// Variables that will be shared between functions
bool NFC_Error, NFC_Tag_Mode, NFC_Startup_Mode;
uint8_t NFC_Retries;
uint8_t NFC_Rx_Array[25];

const uint8_t NFC_WakeUp[]={0x55,0x55,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x03,0xfd,0xd4,0x14,0x01,0x17,0x00};
const uint8_t NFC_FirmwareVersion[]={0x00,0x00,0xFF,0x02,0xFE,0xD4,0x02,0x2A,0x00};
const uint8_t NFC_TagRead[]={0x00,0x00,0xFF,0x04,0xFC,0xD4,0x4A,0x01,0x00,0xE1,0x00};

/* DEBUGGING */
UBaseType_t NFC_uxHighWaterMark;


uint8_t NFC_TaskInit()
{
    //Create task, if it fails return 1, else 0
    if( xTaskCreate(NFC_Task,						/* The function that implements the task */
			   (const portCHAR *)"NFC",				/* The text name assigned to the task - for debug only as it is not used by the kernel */
               ((configMINIMAL_STACK_SIZE) * 10),	/* The size of the stack to allocate to the task */
			   NULL,								/* The parameter passed to the task */
			   PRIORITY_NFC_TASK,	 				/* The priority assigned to the task */
			   NULL)								/* The task handle is not required, so NULL is passed */
    			!= pdTRUE
      )
      {
    	return 1;
      }

	return 0;
}



void NFC_Task(void *pvParameters)
{
	/* Delay a bit to make sure BBComm Task starts-up first */
	const TickType_t xDelay = 10 / portTICK_PERIOD_MS;
	vTaskDelay(xDelay);

    NFC_Tag_Mode = false;
    NFC_Error = false;

  //  NFC_UART_Init();
    Init_UARTx(UART5, SYSTEM_CLOCK, 115200, false);

    Log_Msg(T_NFC, "INFO", "UART5 Init done", LOCAL_ONLY);

    if(NFC_Module_Init() == true)
    {
    	NFC_Error = true;
    }
    else
    {
    	NFC_Error = false;
    }

    if(NFC_Error == true)
    {

        //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send NFC BIST Failure LOG to BB
    	Log_Msg(T_NFC, "CRITICAL", "NFC BIST failed!", LOGGER_AND_LOCAL);

        #if     (NFC_Retry_Mode == NFC_Limited)
            NFC_Retries = NFC_Max_Retries;
        #endif
        #if     NFC_DEBUG_PRINTF
            Log_Msg(T_NFC, "ERROR", "NFC Module Initialization Failed", LOCAL_ONLY);
        #endif
    }
    else
    {
        #if     NFC_DEBUG_PRINTF
            Log_Msg(T_NFC, "INFO", "NFC Module Initialization Succeeded", LOCAL_ONLY);
        #endif
    }

    if(NFC_Verify_Firmware_Version())   NFC_Error = true;
    else    NFC_Error = false;

    if(NFC_Error == true)
    {
        //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send NFC BIST Failure LOG to BB
        #if     (NFC_Retry_Mode == NFC_Limited)
            NFC_Retries = NFC_Max_Retries;
        #endif
        #if     NFC_DEBUG_PRINTF
            Log_Msg(T_NFC, "ERROR", "NFC Firmwave Version Verification Failed", LOCAL_ONLY);
        #endif
    }
    else
    {
        //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send NFC BIST Success LOG to BB
        #if     NFC_DEBUG_PRINTF
            Log_Msg(T_NFC, "INFO", "NFC Firmwave Version Verification Succeeded", LOCAL_ONLY);
            Log_Msg(T_NFC, "INFO", "Starting NFC Normal Operation", LOCAL_ONLY);
        #endif
    }

    /*
     * Normal Operation (Parameters and Returns indicate communication with Central Task)
     *
     * This Task does not require any parameter to get from central task through IPC
     * since this is the first step in the verification, and this task should always be running
     *
     * This Task will have have 1 array of 4 bytes length
     * that should be reported back to the central task through IPC
     * It will also report back the current state of the NFC Module
     *
     * Param_1: Null
     *
     * Return_1: uint8_t NFC_Tx.NFC_Tag_ID_Array[4]
     *          (this will contain the 4 ID bytes of the NFC tag presented at the Module)
     *
     * Return_2: bool NFC_Error
     *           (false: Online, true: OFfline - error present)
     *
     */
    while(1)
    {

        vTaskDelay(NFC_Polling_Timems);

//        taskENTER_CRITICAL();

        if(NFC_Error == true)
        {
            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send NFC Failure LOG to BB
            NFC_Sensor_Test();
        }
        else
        {
            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send NFC Online LOG to BB
            if(NFC_Read_Tag_ID() == true)
            {
                NFC_Error = true;
                #if     (NFC_Retry_Mode == NFC_Limited)
                    NFC_Retries = NFC_Max_Retries;
                #endif
                #if     NFC_DEBUG_PRINTF
                    Log_Msg(T_NFC, "ERROR", "NFC Tag Read Command Failed", LOCAL_ONLY);

                #endif
            }

            if((NFC_Tag_Mode == true) && (NFC_Error == false))
            {

                //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send NFC_Tx structure to BBComm Task

                #if     NFC_DEBUG_PRINTF
                    Log_Msg(T_NFC, "INFO", "Sending NFC Tag to Control Node", LOCAL_ONLY);
                #endif
                vTaskDelay(NFC_Wait_Timems);
            }
        }

//        taskEXIT_CRITICAL();

    }
}

/*
 * Function to setup UART5 bus for NFC
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void NFC_UART_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);

    GPIOPinConfigure(GPIO_PC6_U5RX);
    GPIOPinConfigure(GPIO_PC7_U5TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    UARTConfigSetExpClk(UART5_BASE, SYSTEM_CLOCK, 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
}


/*
 * Function to receive 1 byte over UART with timeout
 * (The timeout is only applied when waiting for a Tag)
 *
 * Param: Null
 *
 * Return: Either Timeout Byte (0x00) or received character
 *
 */
uint8_t NFC_UART_Rx(void)
{
    uint16_t timeout;

    timeout = 0;

    while (HWREG(UART5_BASE + UART_O_FR) & UART_FR_RXFE)
    {
        if(NFC_Tag_Mode == true)
        {
            vTaskDelay(1);
            timeout += 1;
            if(timeout >= NFC_Online_Test_Timems)
            {
                timeout = 0;
                #if     NFC_DEBUG_PRINTF
                    Log_Msg(T_NFC, "INFO", "Checking NFC Module State...", LOCAL_ONLY);
                #endif
                NFC_Tag_Mode = false;
                return 0;
            }
        }
        else if(NFC_Startup_Mode == true)
        {
            vTaskDelay(1);
            timeout += 1;
            if(timeout >= NFC_Online_Test_Timems)
            {
                timeout = 0;
                #if     NFC_DEBUG_PRINTF
                    Log_Msg(T_NFC, "ERROR", "NFC Module is Offline", LOCAL_ONLY);
                #endif

                NFC_uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );

                NFC_Startup_Mode = false;
                return 0;
            }
        }
    }
    return (HWREG(UART5_BASE + UART_O_DR));
}

/*
 * Function to Verify standard acknowledgement
 * portion of the response sent by NFC module
 *
 * Param: Null
 *
 * Return: boolean flag (false: success, true: failure)
 *
 */
bool NFC_Get_Verify_Standard_ACK(void)
{
    uint8_t i, rxbyte;

//    NFC_Startup_Mode = true;

    for(i = 0; i < NFC_Standard_ACK_Size; i ++)
    {
        rxbyte = NFC_UART_Rx();
        if(NFC_Startup_Mode == true)    NFC_Rx_Array[i] = rxbyte;
        else
        {
            NFC_Sensor_Test();
            return true;
        }
    }

    if((NFC_Rx_Array[0] != NFC_Standard_ACK_Byte1) || (NFC_Rx_Array[1] != NFC_Standard_ACK_Byte2) ||
            (NFC_Rx_Array[2] != NFC_Standard_ACK_Byte3) || (NFC_Rx_Array[3] != NFC_Standard_ACK_Byte4) ||
            (NFC_Rx_Array[4] != NFC_Standard_ACK_Byte5) || (NFC_Rx_Array[5] != NFC_Standard_ACK_Byte6))       return true;
    else        return false;
}

/*
 * Function to Verify firmware version of the
 * chipset driving the NFC module
 *
 * Param: Null
 *
 * Return: boolean flag (false: success, true: failure)
 *
 */
bool NFC_Verify_Firmware_Version(void)
{
    uint8_t i;
    NFC_Startup_Mode = true;

//    #if     NFC_DEBUG_PRINTF
//        NFC_Print("\nVerifying NFC Firmware Version");
//    #endif
    for(i = 0; i < sizeof(NFC_FirmwareVersion); i ++)    UARTCharPut(UART5_BASE, NFC_FirmwareVersion[i]);

    if(NFC_Get_Verify_Standard_ACK() == true)      return true;

    for(i = NFC_Standard_ACK_Size; i < NFC_Firmware_Version_ACK_Size; i ++)     NFC_Rx_Array[i] = NFC_UART_Rx();

    if(((NFC_Rx_Array[NFC_Standard_ACK_Size + NFC_Firmware_Version_Byte1_Pos]) != NFC_Firmware_Version_Byte1_Val) ||
            ((NFC_Rx_Array[NFC_Standard_ACK_Size + NFC_Firmware_Version_Byte2_Pos]) != NFC_Firmware_Version_Byte2_Val))       return true;

    return false;
}

/*
 * Function to test the sensor state while
 * supporting both retry modes
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void NFC_Sensor_Test(void)
{
#if     (NFC_Retry_Mode == NFC_Limited)
    if(NFC_Retries != 0x00)
    {
        NFC_Retries -= 1;
#endif
        if(NFC_Verify_Firmware_Version() == true)
        {
            NFC_Error = true;
            #if     (NFC_Retry_Mode == NFC_Limited)
                NFC_Retries = NFC_Max_Retries;
            #endif
//            #if     NFC_DEBUG_PRINTF
//                NFC_Print("\nNFC Module is Offline");
//            #endif
        }
        else
        {
            NFC_Error = false;
            #if     NFC_DEBUG_PRINTF
                Log_Msg(T_NFC, "INFO", "NFC Module is Online", LOCAL_ONLY);
            #endif
        }
#if     (NFC_Retry_Mode == NFC_Limited)
    }
#endif
}

/*
 * Function to read the tag ID  from NFC module
 * If a valid tag is detected, the 4 bytes representing
 * unique ID will be stored in the array which should
 * be sent back to Central Task using IPC
 *
 * Param: Null
 *
 * Return: boolean flag (false: success, true: failure)
 * (false or true only applies to the command sent, and
 *  not the received data)
 *
 */
bool NFC_Read_Tag_ID(void)
{
    uint8_t i;
    char tp[50];
    uint8_t rxbyte;

    for(i = 0; i < 4; i ++)     NFC_Tx.NFC_Tag_ID_Array[i] = 0;

    #if     NFC_DEBUG_PRINTF
        Log_Msg(T_NFC, "INFO", "Waiting for Tag", LOCAL_ONLY);
    #endif

    NFC_Tag_Mode = true;

    for(i = 0; i < sizeof(NFC_TagRead); i ++)    UARTCharPut(UART5_BASE, NFC_TagRead[i]);

    if(NFC_Get_Verify_Standard_ACK() == true)      return true;

    for(i = NFC_Standard_ACK_Size; i < NFC_Tag_ID_Size; i ++)
    {
        rxbyte = NFC_UART_Rx();
        if(NFC_Tag_Mode == true)    NFC_Rx_Array[i] = rxbyte;
        else
        {
            NFC_Sensor_Test();
            break;
        }
    }

    if(NFC_Tag_Mode == true)
    {
        for(i = 0; i < NFC_Tag_ID_Length; i ++)    NFC_Tx.NFC_Tag_ID_Array[i] = NFC_Rx_Array[(i + NFC_Standard_ACK_Size + NFC_Tag_ID_Start_Byte_Pos)];
        #if     NFC_DEBUG_PRINTF
            snprintf(tp, 50, "Tag ID: %02X:%02X:%02X:%02X", NFC_Tx.NFC_Tag_ID_Array[0], NFC_Tx.NFC_Tag_ID_Array[1], NFC_Tx.NFC_Tag_ID_Array[2], NFC_Tx.NFC_Tag_ID_Array[3]);
            Log_Msg(T_NFC, "INFO", tp, LOCAL_ONLY);
        #endif
    }
    else    for(i = 0; i < NFC_Tag_ID_Length; i ++)    NFC_Tx.NFC_Tag_ID_Array[i] = 0;

    return false;
}

/*
 * Function to initialize the NFC module
 *
 * Param: Null
 *
 * Return: Null
 *
 */
bool NFC_Module_Init(void)
{
  //  static char tp[50];
    uint8_t i;

    NFC_Startup_Mode = true;

    #if     NFC_DEBUG_PRINTF
         Log_Msg(T_NFC, "INFO", "Waking up the Module", LOCAL_ONLY);
    #endif

    for(i = 0; i < sizeof(NFC_WakeUp); i ++)    UARTCharPut(UART5_BASE, NFC_WakeUp[i]);

    vTaskDelay(100);

    #if     NFC_DEBUG_PRINTF
         Log_Msg(T_NFC, "INFO", "Getting Acknowledgement", LOCAL_ONLY);
    #endif

    if(NFC_Get_Verify_Standard_ACK() == true)      return true;

    for(i = NFC_Standard_ACK_Size; i < NFC_WakeUp_ACK_Size; i ++)    NFC_Rx_Array[i] = NFC_UART_Rx();

    #if     NFC_DEBUG_PRINTF
//        Log_Msg(T_NFC, "INFO", "Displaying ACK:", LOCAL_ONLY);
//
//        for(i = 0; i < NFC_WakeUp_ACK_Size; i ++)
//        {
//            snprintf(tp, sizeof(tp), "%x ", NFC_Rx_Array[i]);
//            Log_Msg(T_NFC, "INFO", tp, LOCAL_ONLY);
//        }
    #endif

    return false;
}

