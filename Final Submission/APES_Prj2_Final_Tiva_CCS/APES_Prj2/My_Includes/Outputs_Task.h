/*
 * Outputs_Task.h
 *
 *  Created on: Apr 25, 2019
 *      Author:  Author: Khalid AlAwadhi
 */

#ifndef MY_INCLUDES_OUTPUTS_TASK_H_
#define MY_INCLUDES_OUTPUTS_TASK_H_

#include <stdint.h>

#define Servo_Polling_Timems       500

#define Servo_Host_Unknown           false

#if     Servo_Host_Unknown
    #define Output_Print   UARTprintf
#else
    #define Output_Print   cust_print
#endif

#define Output_DEBUG_PRINTF        true

#define Servo_PWM_Freq      50
#define Servo_PWM_Cycles    (uint32_t)((SYSTEM_CLOCK / 64) / Servo_PWM_Freq)

#define Servo_Open_Position     (uint32_t)(Servo_PWM_Cycles * 0.2)
#define Servo_Close_Position    (uint32_t)(Servo_PWM_Cycles * 0.075)

#define Servo_Door_Open_Timeoutms           10000

#define     SV_Enable_Pos       0
#define     SV_Enable_Mask      (1 << SV_Enable_Pos)

#define     BZ_Enable_Pos       4
#define     BZ_Enable_Mask      (1 << SV_Enable_Pos)


#define     SJ_Enable_Pos       0
#define     SJ_Enable_Mask      (1 << SJ_Enable_Pos)

#define     SJ_DayTime_Pos      1
#define     SJ_DayTime_Mask     (3 << SJ_DayTime_Pos)
#define     SJ_Morning_Time     (0 << SJ_DayTime_Pos)
#define     SJ_Afternoon_Time   (1 << SJ_DayTime_Pos)
#define     SJ_Evening_Time     (2 << SJ_DayTime_Pos)
#define     SJ_Night_Time       (3 << SJ_DayTime_Pos)

#define     SJ_Person_ID_Pos    3
#define     SJ_Person_ID_Mask   (1 << SJ_Person_ID_Pos)
#define     SJ_Person_ID_Poorn  (0 << SJ_Person_ID_Pos)
#define     SJ_Person_ID_Khalid (1 << SJ_Person_ID_Pos)

// Set up a memorable token for "Word Pause"
#define WP 6    // 6 is 90ms pause



/**************************************************************************************************************
 * USAGE: This function will initialize the Outputs task
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: 0 => Init was successful
 * 			1 => Init failed
 **************************************************************************************************************/
uint8_t Outputs_TaskInit();


/**************************************************************************************************************
 * USAGE: This function XXX
 *
 * PARAMETERS:
 *            - NONE
 *
 * RETURNS: NONE
 **************************************************************************************************************/
void Outputs_Task(void *pvParameters);



void Servo_PWM_Init(void);
void Servo_Door_Open(void);
void Servo_Door_Close(void);


void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);


void SJ_Init(void);
void SJ_Select_Message(void);



#endif /* MY_INCLUDES_OUTPUTS_TASK_H_ */
