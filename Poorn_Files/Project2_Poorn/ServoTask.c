///*
// * ServoTask.c
// *
// *  Created on: Apr 19, 2019
// *  Last Update: Apr 22, 2019
// *      Author: Poorn Mehta
// *
// *  Driver for Driving Servo Motor which is connected to
// *  TM4C1294XL (TIVA Development Board) using PWM output
// *
// *  This driver code is developed based on provided documentation from the manufacturer
// *
// */
//
///*
// *
// * Hardware Connections
// *
// *
// * Note: Left Side - Module Pins & Right Side - Controller Pins
// *
// * Orange - PF0 (This is the PWM0 Output of the Controller)
// * Red - 5V
// * GND - Ground
// *
// */
//
//#include "ServoTask.h"
//
//// FreeRTOS includes
//#include "FreeRTOSConfig.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "queue.h"
//
//// Variables that will be shared between functions
//bool Servo_Open;
//
///*
// * Function to setup PWM for Servo
// *
// * Param: Null
// *
// * Return: Null
// *
// */
//void Servo_PWM_Init(void)
//{
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
//    PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_64);
//    GPIOPinConfigure(GPIO_PF0_M0PWM0);
//    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0);
//    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
//    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, Servo_PWM_Cycles);
//    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, Servo_Close_Position);
//    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
//    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
//}
//
///*
// * Function to drive Servo into Open Position
// *
// * Param: Null
// *
// * Return: Null
// *
// */
//void Servo_Door_Open(void)
//{
//    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, Servo_Open_Position);
//    #if     Servo_DEBUG_PRINTF
//        Servo_Print("\nDoor Opened");
//    #endif
//}
//
///*
// * Function to drive Servo into Close Position
// *
// * Param: Null
// *
// * Return: Null
// *
// */
//void Servo_Door_Close(void)
//{
//    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, Servo_Close_Position);
//    #if     Servo_DEBUG_PRINTF
//        Servo_Print("\nDoor Closed");
//    #endif
//}
//
///*
// *
// * Callback Function for Servo Task
// *
// * Return: Null
// *
// */
//void ServoTask(void *pvParameters)
//{
//    Servo_PWM_Init();
//    #if     Servo_DEBUG_PRINTF
//        Servo_Print("\nServo PWM Init Done");
//    #endif
//
//#if     Servo_INDIVIDUAL_TESTING
//
//    /*
//     * This portion just tests the Servo Motor individually
//     * It doesn't rely on anything that is not covered
//     * or provided by its own header and source files
//     *
//     * This mode is only for raw testing, and may not
//     * include any of the error checking feature(s)
//     */
//
//    while(1)
//    {
//        Servo_Door_Close();
//        vTaskDelay(2500);
//
//        Servo_Door_Open();
//        vTaskDelay(2500);
//    }
//
//#else
//
//    /*
//     * Normal Operation (Parameters and Returns indicate communication with Central Task)
//     *
//     * This Task requires 1 parameter from Central Task through IPC
//     *
//     * This Task will return nothing to the Central Task
//     *
//     * Param_1: bool Servo_Open
//     *          (false: do nothing, true: open the door)
//     *
//     * Return_1: Null
//     *
//     */
//
//    while(1)
//    {
//        if(Servo_Open == true)
//        {
//            Servo_Door_Open();
//            vTaskDelay(Servo_Door_Open_Timeoutms);
//            Servo_Door_Close();
//            Servo_Open = false;
//        }
//        vTaskDelay(Servo_Polling_Timems);
//    }
//
//#endif
//}
//
