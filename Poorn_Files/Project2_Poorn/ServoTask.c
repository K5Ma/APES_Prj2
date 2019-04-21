/*
 * ServoTask.c
 *
 *  Created on: Apr 19, 2019
 *      Author: poorn
 */

#include "ServoTask.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void ServoTask(void *pvParameters)
{
//    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    cust_print("\nPWM Init");

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);

    cust_print("\nPWM Init Done");

    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_64);

    GPIOPinConfigure(GPIO_PF0_M0PWM0);

    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0);

    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 37500); //1500 for 1250Hz

    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 7000); //3750

    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);

    PWMGenEnable(PWM0_BASE, PWM_GEN_0);

    cust_print("\nPWM Init Done");

    while(1)
    {

        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 7000); //3750
        cust_print("\nDoor Open");

        vTaskDelay(2500);

        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 2812); //375 for 180deg
        cust_print("\nDoor Close");

        vTaskDelay(2500);
    }

}

