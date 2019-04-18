/*
 * LoadCellTask.c
 *
 *  Created on: Apr 13, 2019
 *      Author: poorn
 */

#include "LoadCellTask.h"

#include <stdint.h>
#include <stdbool.h>
#include "drivers/pinout.h"
#include "utils/uartstdio.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_i2c.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "inc/hw_adc.h"

#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/fpu.h"
#include "driverlib/adc.h"

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"

#include "sensorlib/i2cm_drv.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

uint8_t us_flag = 0;

void T0ISR(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    us_flag = 1;
}

void loadcell_all(void *pvParameters)
{
    static char tp[50];
    static uint32_t data;
    static int i;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, (SYSTEM_CLOCK/100000));
    IntMasterEnable();
    TimerIntRegister(TIMER0_BASE, TIMER_A, T0ISR);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    cust_print("\nTimer Init Success");

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_4); //clk
    GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_5); //data

//    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, GPIO_PIN_4);
    vTaskDelay(3000);

//    static uint32_t prev = 0;
    static uint32_t stat = 0;
//
//    while(1)
//    {
//        stat = GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_5);
//        if(stat != prev)
//        {
//            snprintf(tp, sizeof(tp), "\nOld: %d New: %d", prev, stat);
//            cust_print(tp);
//            prev = stat;
//        }
//        vTaskDelay(1);
//    }

    // start
    cust_print("\nNormal Operation Start");
    while(1)
    {
        GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, false);
        stat = GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_5) & 0x00FF;
        stat >>= 5;
        while(stat == 1)
        {
            stat = GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_5) & 0x00FF;
            stat >>= 5;
            vTaskDelay(1);
        }

        data = 0;

        for(i = 23; i >= 0; i --)
        {
            us_flag = 0;
            GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, GPIO_PIN_4);
            TimerEnable(TIMER0_BASE, TIMER_A);
            while(us_flag == 0);
            TimerDisable(TIMER0_BASE, TIMER_A);

            stat = GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_5) & 0x00FF;
            stat >>= 5;
            if(stat == 1)     data |= (1 << i);

            us_flag = 0;
            GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, false);
            TimerEnable(TIMER0_BASE, TIMER_A);
            while(us_flag == 0);
            TimerDisable(TIMER0_BASE, TIMER_A);
        }

        us_flag = 0;
        GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, GPIO_PIN_4);
        TimerEnable(TIMER0_BASE, TIMER_A);
        while(us_flag == 0);
        TimerDisable(TIMER0_BASE, TIMER_A);

        us_flag = 0;
        GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_4, false);
        TimerEnable(TIMER0_BASE, TIMER_A);
        while(us_flag == 0);
        TimerDisable(TIMER0_BASE, TIMER_A);

//        snprintf(tp, sizeof(tp), "\nRaw: %x", data);
        cust_print(tp);

        data &= 0x00FFFFFF;
        data ^= 0x00FFFFFF;
        data += 1;

        snprintf(tp, sizeof(tp), "\nNew: %d", data);
        cust_print(tp);
    }
}

//void adc_all(void *pvParameters)
//{
//    cust_print("\nADC Setup Start");
//
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
//
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
//
//    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
//
//    ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_HALF, 1);
//
//    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
//
//    ADCHardwareOversampleConfigure(ADC0_BASE, 16);
//
//    ADCSoftwareOversampleConfigure(ADC0_BASE, 0, 8);
//
//    ADCSoftwareOversampleStepConfigure(ADC0_BASE, 0, 0, (ADC_CTL_CH0 | ADC_CTL_END));
//
//    ADCSequenceEnable(ADC0_BASE, 0);
//
//    static uint32_t adc_data[32], adc_avg;
//    static uint8_t samples_count, i;
//    static char tp[50];
//
//    cust_print("\nADC Setup Done");
//
//    while(1)
//    {
//        ADCProcessorTrigger(ADC0_BASE, 0);
//
//        while(ADCBusy(ADC0_BASE));
//
//        samples_count = ADCSequenceDataGet(ADC0_BASE, 0, &adc_data[0]);
//
//        snprintf(tp, sizeof(tp), "\nSamples: %d", samples_count);
//        cust_print(tp);
//
//        adc_avg = 0;
//
//        for(i = 0; i < samples_count; i++)
//        {
//            adc_avg += adc_data[i];
//        }
//        adc_avg >>= 3;
//
//        snprintf(tp, sizeof(tp), "\nAvg ADC: %d", adc_avg);
//        cust_print(tp);
//
//        vTaskDelay(1000);
//    }
//}
