/*
 * BME280Task.c
 *
 *  Created on: Apr 15, 2019
 *      Author: poorn
 */

#include "BME280Task.h"

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
#include "inc/hw_ssi.h"

#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/fpu.h"
#include "driverlib/ssi.h"

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


uint32_t BME280_Tx[10];
uint32_t BME280_Rx[10];
uint32_t BME280_index;

void bme280_all(void *pvParameters)
{
    static char tp[50];

    cust_print("\nSPI Setup Start");

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_6);
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, true);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0XDAT0);
    GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);

    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 |
                   GPIO_PIN_2);

    SSIConfigSetExpClk(SSI0_BASE, SYSTEM_CLOCK, SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 1000000, 8);

    SSIEnable(SSI0_BASE);

    while(SSIDataGetNonBlocking(SSI0_BASE, &BME280_Rx[0]));

    cust_print("\nSPI Setup Done");

    vTaskDelay(3000);

    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, false);

//    BME280_Tx[0] = BME280_CHIP_ID_REG | (1 << 7);
//    BME280_Tx[1] = BME280_CHIP_ID_REG | (1 << 7);
//    BME280_Tx[2] = BME280_CHIP_ID_REG | (1 << 7);

    BME280_Tx[0] = 0xD0;
    BME280_Tx[1] = 0xD0;
    BME280_Tx[2] = 0xD0;

/*    SSIDataPut(SSI0_BASE, BME280_Tx[0]);

    while(SSIBusy(SSI0_BASE));

    cust_print("\nByte 1 Transmitted");

    SSIDataGet(SSI0_BASE, &BME280_Rx[0]);

//    BME280_Rx[0] &= 0x00FF;

    snprintf(tp, sizeof(tp), "\nSPI Got: %x", BME280_Rx[0]);
    cust_print(tp);
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, true);

    vTaskDelay(1);

    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, false);
    SSIDataPut(SSI0_BASE, BME280_Tx[1]);

    while(SSIBusy(SSI0_BASE));

    cust_print("\nByte 2 Transmitted");

    SSIDataGet(SSI0_BASE, &BME280_Rx[1]);

//    BME280_Rx[1] &= 0x00FF;

    snprintf(tp, sizeof(tp), "\nSPI Got: %x", BME280_Rx[1]);
    cust_print(tp);
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, true);

    vTaskDelay(1);

    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, false);
    SSIDataPut(SSI0_BASE, BME280_Tx[2]);

    while(SSIBusy(SSI0_BASE));

    cust_print("\nByte 3 Transmitted");

    SSIDataGet(SSI0_BASE, &BME280_Rx[2]);

//    BME280_Rx[2] &= 0x00FF;

    snprintf(tp, sizeof(tp), "\nSPI Got: %x", BME280_Rx[2]);
    cust_print(tp);
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, true);

    vTaskDelay(1);*/

    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, false);
    SSIDataPut(SSI0_BASE, BME280_Tx[0]);

    while(SSIBusy(SSI0_BASE));

    cust_print("\nByte 1 Transmitted");

    SSIDataPut(SSI0_BASE, BME280_Tx[1]);

    while(SSIBusy(SSI0_BASE));

    cust_print("\nByte 2 Transmitted");

    SSIDataPut(SSI0_BASE, BME280_Tx[2]);

    while(SSIBusy(SSI0_BASE));

    cust_print("\nByte 3 Transmitted");

    SSIDataGet(SSI0_BASE, &BME280_Rx[0]);

//    BME280_Rx[0] &= 0x00FF;

    snprintf(tp, sizeof(tp), "\nSPI Got: %x", BME280_Rx[0]);
    cust_print(tp);

    SSIDataGet(SSI0_BASE, &BME280_Rx[1]);

//    BME280_Rx[1] &= 0x00FF;

    snprintf(tp, sizeof(tp), "\nSPI Got: %x", BME280_Rx[1]);
    cust_print(tp);

    SSIDataGet(SSI0_BASE, &BME280_Rx[2]);

//    BME280_Rx[2] &= 0x00FF;

    snprintf(tp, sizeof(tp), "\nSPI Got: %x", BME280_Rx[2]);
    cust_print(tp);
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, true);

    cust_print("\nSPI End");
}


