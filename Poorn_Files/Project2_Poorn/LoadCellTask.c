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
#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"

#include "sensorlib/i2cm_drv.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void adc_all(void *pvParameters)
{
    cust_print("\nADC Setup Start");

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

    ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_HALF, 1);

    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);

    ADCHardwareOversampleConfigure(ADC0_BASE, 16);

    ADCSoftwareOversampleConfigure(ADC0_BASE, 0, 8);

    ADCSoftwareOversampleStepConfigure(ADC0_BASE, 0, 0, (ADC_CTL_CH0 | ADC_CTL_END));

    ADCSequenceEnable(ADC0_BASE, 0);

    static uint32_t adc_data[32], adc_avg;
    static uint8_t samples_count, i;
    static char tp[50];

    cust_print("\nADC Setup Done");

    while(1)
    {
        ADCProcessorTrigger(ADC0_BASE, 0);

        while(ROM_ADCBusy(ADC0_BASE));

        samples_count = ADCSequenceDataGet(ADC0_BASE, 0, &adc_data[0]);

        snprintf(tp, sizeof(tp), "\nSamples: %d", samples_count);
        cust_print(tp);

        adc_avg = 0;

        for(i = 0; i < samples_count; i++)
        {
            adc_avg += adc_data[i];
        }
        adc_avg >>= 3;

        snprintf(tp, sizeof(tp), "\nAvg ADC: %d", adc_avg);
        cust_print(tp);

        vTaskDelay(1000);
    }
}
