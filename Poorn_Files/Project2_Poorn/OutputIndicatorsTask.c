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

void OutputIndicatorsTask(void *pvParameters)
{

    cust_print("\nPWM Init");

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

    static int i;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_64);

    GPIOPinConfigure(GPIO_PF2_M0PWM2);

    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);

    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, 2500); //750hz

    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, 1250);

    PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT, true);

    PWMGenEnable(PWM0_BASE, PWM_GEN_1);

    cust_print("\nPWM Init Done");

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





