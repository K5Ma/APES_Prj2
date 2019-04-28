/*
 * OutputIndicatorsTask.c
 *
 *  Created on: Apr 20, 2019
 *      Author: poorn
 */

#include "OutputIndicatorsTask.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef struct{
    uint8_t OI_Data;
} OI_B2T_Struct;

OI_B2T_Struct OI_Rx;

bool Servo_Open, Buzzer_Flag;

void OutputIndicatorsTask(void *pvParameters)
{
    Buzzer_Init();
    Servo_PWM_Init();

    Buzzer_Flag = false;
    Servo_Open = false;

    while(1)
    {
        //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Receive OI_Rx structure from BBComm Task with Maximum Timeout

        Servo_Open = ((OI_Rx.OI_Data & SV_Enable_Mask) >> SV_Enable_Pos);
//        Buzzer_Flag = ((OI_Rx.OI_Data & BZ_Enable_Mask) >> BZ_Enable_Pos);

//        if(Buzzer_Flag == true)
//        {
//            Buzzer_On();
//        }
//        else
//        {
//            Buzzer_Off();
//        }

        if(Servo_Open == true)
        {
            Servo_Door_Open();
            vTaskDelay(Servo_Door_Open_Timeoutms);
            Servo_Door_Close();
            Servo_Open = false;
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
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);
}

void Buzzer_On(void)
{
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);
    static int i;
    static uint32_t clocks;
    while(1)
    {
        for(i = 0; i < 50; i ++)
        {
            clocks = (1875000 / ((i * 50) + 50));
            PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, clocks);
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, (clocks / 2));
            vTaskDelay(200);
        }
        for(i = 49; i >= 0; i --)
        {
            clocks = (1875000 / ((i * 50) + 50));
            PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, clocks);
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, (clocks / 2));
            vTaskDelay(200);
        }
    }
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
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
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
    #if     Servo_DEBUG_PRINTF
        Servo_Print("\nDoor Opened");
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
    #if     Servo_DEBUG_PRINTF
        Servo_Print("\nDoor Closed");
    #endif
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
    #if     Servo_DEBUG_PRINTF
        Servo_Print("\nServo PWM Init Done");
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






