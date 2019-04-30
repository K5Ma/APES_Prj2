/*
 * Outputs_Task.c
 *
 *  Created on: Apr 25, 2019
 *      Author:  Author: Khalid AlAwadhi
 */

#include "Outputs_Task.h"

// Standard Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

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
#include "inc/hw_pwm.h"

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
#include "driverlib/pwm.h"

//FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"

/* Global Variables */
extern QueueHandle_t xQueue_OIStruct;

//extern bool Temperature_Alert, Humidity_Alert, Gas_Alert, Lux_Alert, PIR_Alert;

//>>>>>>>>>>>>>>> Share Servo_Open among Tasks

bool Servo_Open, Buzzer_Flag;

uint8_t SJ_Data;

const uint8_t SJ_Welcome[] = {147, 159, 194, 134, 140, WP, WP, WP, WP, WP};
const uint8_t SJ_And[] = {8, 132, 8, 141, 177, WP, WP};
const uint8_t SJ_Have[] = {183, 8, 132, 166, WP, WP, WP};
const uint8_t SJ_A[] = {154, 128, WP, WP, WP};

const uint8_t SJ_Good[] = {8, 179, 138, 138, 177, WP, WP};

const uint8_t SJ_Morning[] =  {140, 7, 137, 7, 153, 141, 129, 143, WP, WP, WP};
const uint8_t SJ_Afternoon[] = {132, 186, 7, 191, 7, 151, 7, 141, 139, 8, 141, WP, WP, WP};
const uint8_t SJ_Evening[] = {8, 128, 166, 130, 141, 129, 143, WP, WP, WP};
const uint8_t SJ_Night[] = {140, 7, 135, 155, 191, WP, WP, WP};

const uint8_t SJ_Poorn[] = {199, 7, 137, 153, 141, WP};
const uint8_t SJ_Khalid[] = {8, 194, 8, 183, 8, 145, 128, 174, WP};


uint8_t Outputs_TaskInit()
{
    //Create task, if it fails return 1, else 0
    if( xTaskCreate(Outputs_Task,					/* The function that implements the task */
			   (const portCHAR *)"Outputs",			/* The text name assigned to the task - for debug only as it is not used by the kernel */
               ((configMINIMAL_STACK_SIZE) * 10),	/* The size of the stack to allocate to the task */
			   NULL,								/* The parameter passed to the task */
			   PRIORITY_OUTPUTS_TASK,	 			/* The priority assigned to the task */
			   NULL)								/* The task handle is not required, so NULL is passed */
    			!= pdTRUE
      )
      {
    	return 1;
      }

	return 0;
}



void Outputs_Task(void *pvParameters)
{
	/* Delay a bit to make sure BBComm Task starts-up first */
	const TickType_t xDelay = 10 / portTICK_PERIOD_MS;
	vTaskDelay(xDelay);


	/* Create a queue capable of containing 6 structs */
	xQueue_OIStruct = xQueueCreate(2,  sizeof(OI_B2T_Struct));

	if( xQueue_OIStruct == NULL )
	{
		/* Queue was not created and must not be used. */
		Log_Msg(T_Outputs, "FATAL", "Could not create xQueue_OIStruct!", LOCAL_ONLY);
	}
	else
	{
		Log_Msg(T_Outputs, "INFO", "Created xQueue_OIStruct successfully!", LOCAL_ONLY);
	}

	OI_B2T_Struct OI_Rx;


    Buzzer_Init();
    Servo_PWM_Init();
    SJ_Init();

    Buzzer_Flag = false;
//    Buzzer_Flag = true;

    Servo_Open = false;
    SJ_Data = 0;


	while(1)
	{
//        //enter critical
//        if((Temperature_Alert == true) || (Humidity_Alert == true) || (Gas_Alert == true) || (Lux_Alert == true) || (PIR_Alert == true))
//        {
////            #if     Output_DEBUG_PRINTF
////                Output_Print("\nHigh Alert Detected...");
////            #endif
//            if((Temperature_Alert == true) || (Humidity_Alert == true) || (Gas_Alert == true))      Servo_Door_Open();
//            Buzzer_Flag = true;
//        }
//        else
//        {
////            #if     Output_DEBUG_PRINTF
////                Output_Print("\nNo Alert");
////            #endif
//            Buzzer_Flag = false;
//            Servo_Door_Close();
//        }
//        //exit critical
//
//        if(Buzzer_Flag == true)
//        {
//            Buzzer_On();
//        }
//        else
//        {
//            Buzzer_Off();
//        }

		if( xQueue_OIStruct != 0 )
		{
			/* Block for 10 ticks if a message (struct buffer) is not immediately available */
			if( xQueueReceive( xQueue_OIStruct, &OI_Rx, ( TickType_t ) 100 ) )
			{
		//		Log_Msg(T_Outputs, "DEBUG", "OI Got struct from BB!", LOCAL_ONLY);

		        //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Receive OI_Rx structure from BBComm Task with 100ms Timeout

		        //>>> Drive Speakjet Here

		        //enter critical
		        Servo_Open = ((OI_Rx.OI_Data & SV_Enable_Mask) >> SV_Enable_Pos);
		//        SJ_Data = OI_Rx.OI_Data;
		        SJ_Select_Message();
		        SJ_Data = 0;
		        //exit critical

		        if(Servo_Open == true)
		        {
		            Servo_Door_Open();
		            vTaskDelay(Servo_Door_Open_Timeoutms);
		            Servo_Door_Close();
		            Servo_Open = false;
		            OI_Rx.OI_Data = 0;
		        }


			}
		}
	}
}



void Buzzer_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_64);
    GPIOPinConfigure(GPIO_PF2_M0PWM2);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, 2500); //750hz
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, 1250);
    PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, true);
//    PWMGenEnable(PWM0_BASE, PWM_GEN_1);
}

void Buzzer_On(void)
{
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);
//    static int i;
//    static uint32_t clocks;
//    while(1)
//    {
//        for(i = 0; i < 40; i ++)
//        {
//            clocks = (1875000 / ((i * 50) + 50));
//            PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, clocks);
//            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, (clocks / 2));
//            vTaskDelay(50);
//        }
//        for(i = 39; i >= 0; i --)
//        {
//            clocks = (1875000 / ((i * 50) + 50));
//            PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, clocks);
//            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, (clocks / 2));
//            vTaskDelay(50);
//        }
//    }
}

void Buzzer_Off(void)
{
    PWMGenDisable(PWM0_BASE, PWM_GEN_1);
}

/*
 * Function to setup PWM for Servo
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void Servo_PWM_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_64);
    GPIOPinConfigure(GPIO_PF0_M0PWM0);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, Servo_PWM_Cycles);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, Servo_Close_Position);
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
//    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
}

/*
 * Function to drive Servo into Open Position
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void Servo_Door_Open(void)
{
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, Servo_Open_Position);
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    #if     Output_DEBUG_PRINTF
        Log_Msg(T_Outputs, "INFO", "Door Opened", LOCAL_ONLY);
    #endif
}

/*
 * Function to drive Servo into Close Position
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void Servo_Door_Close(void)
{
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, Servo_Close_Position);
    #if     Output_DEBUG_PRINTF
        Log_Msg(T_Outputs, "INFO", "Door Closed", LOCAL_ONLY);
    #endif
    vTaskDelay(1000);
    PWMGenDisable(PWM0_BASE, PWM_GEN_0);
}

/*
 *
 * Callback Function for Servo Task
 *
 * Return: Null
 *
 */
void ServoTask(void *pvParameters)
{
    Servo_PWM_Init();
    #if     Output_DEBUG_PRINTF
        Log_Msg(T_Outputs, "INFO", "Servo PWM Init Done", LOCAL_ONLY);
    #endif

    /*
     * Normal Operation (Parameters and Returns indicate communication with Central Task)
     *
     * This Task requires 1 parameter from Central Task through IPC
     *
     * This Task will return nothing to the Central Task
     *
     * Param_1: bool Servo_Open
     *          (false: do nothing, true: open the door)
     *
     * Return_1: Null
     *
     */

    while(1)
    {
        if(Servo_Open == true)
        {
            Servo_Door_Open();
            vTaskDelay(Servo_Door_Open_Timeoutms);
            Servo_Door_Close();
            Servo_Open = false;
        }
        vTaskDelay(Servo_Polling_Timems);
    }
}

void SJ_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART6);

    GPIOPinConfigure(GPIO_PP0_U6RX);
    GPIOPinConfigure(GPIO_PP1_U6TX);
    GPIOPinTypeUART(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(UART6_BASE, SYSTEM_CLOCK, 9600,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    UARTCharPut(UART6_BASE, 20); //volume set command
    UARTCharPut(UART6_BASE, 127); //volume level
    UARTCharPut(UART6_BASE, 21); //speed set command
    UARTCharPut(UART6_BASE, 114); //speed level
}

void SJ_Select_Message(void)
{
    static uint8_t i, sw_var;

    if((SJ_Data & SJ_Enable_Mask) == true)
    {
        for (i = 0; i < sizeof(SJ_Welcome); i ++)    UARTCharPut(UART6_BASE, SJ_Welcome[i]);
        for (i = 0; i < sizeof(SJ_And); i ++)        UARTCharPut(UART6_BASE, SJ_And[i]);
        for (i = 0; i < sizeof(SJ_Have); i ++)       UARTCharPut(UART6_BASE, SJ_Have[i]);
        for (i = 0; i < sizeof(SJ_A); i ++)       UARTCharPut(UART6_BASE, SJ_A[i]);
        for (i = 0; i < sizeof(SJ_Good); i ++)       UARTCharPut(UART6_BASE, SJ_Good[i]);

        sw_var = SJ_Data & SJ_DayTime_Mask;
        switch(sw_var)
        {
            case    SJ_Morning_Time:
           //     cust_print("\nMorning");
                for (i = 0; i < sizeof(SJ_Morning); i ++)       UARTCharPut(UART6_BASE, SJ_Morning[i]);
                break;
            case    SJ_Afternoon_Time:
            //    cust_print("\nAfternoon");
                for (i = 0; i < sizeof(SJ_Afternoon); i ++)       UARTCharPut(UART6_BASE, SJ_Afternoon[i]);
                break;
            case    SJ_Evening_Time:
             //   cust_print("\nEvening");
                for (i = 0; i < sizeof(SJ_Evening); i ++)       UARTCharPut(UART6_BASE, SJ_Evening[i]);
                break;
            case    SJ_Night_Time:
              //  cust_print("\nNight");
                for (i = 0; i < sizeof(SJ_Night); i ++)       UARTCharPut(UART6_BASE, SJ_Night[i]);
                break;
            default:
                break;
        }

        sw_var = SJ_Data & SJ_Person_ID_Mask;
        switch(sw_var)
        {
            case    SJ_Person_ID_Poorn:
          //      cust_print(" Poorn\n");
                for (i = 0; i < sizeof(SJ_Poorn); i ++)       UARTCharPut(UART6_BASE, SJ_Poorn[i]);
                break;
            case    SJ_Person_ID_Khalid:
           //     cust_print(" Khalid\n");
                for (i = 0; i < sizeof(SJ_Khalid); i ++)       UARTCharPut(UART6_BASE, SJ_Khalid[i]);
                break;
            default:
                break;
        }
    }
}
