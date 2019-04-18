/*
 * NFCTask.c
 *
 *  Created on: Apr 17, 2019
 *      Author: poorn
 *
 *  This code is based on the library provided by dfrobot
 */

#include "NFCTask.h"

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

#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/fpu.h"

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

const uint8_t wake[24]={0x55,0x55,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x03,0xfd,0xd4,0x14,0x01,0x17,0x00};//wake up NFC module
const uint8_t firmware[9]={0x00,0x00,0xFF,0x02,0xFE,0xD4,0x02,0x2A,0x00};//
const uint8_t tag[11]={0x00,0x00,0xFF,0x04,0xFC,0xD4,0x4A,0x01,0x00,0xE1,0x00};//detecting tag command

uint8_t receive_ACK[25];//Command receiving buffer
int inByte = 0;               //incoming serial byte buffer


void nfc_all(void *pvParameters)
{
    static char tp[50];

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);

    GPIOPinConfigure(GPIO_PC6_U5RX);
    GPIOPinConfigure(GPIO_PC7_U5TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    UARTConfigSetExpClk(UART5_BASE, SYSTEM_CLOCK, 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

//    UARTFIFODisable(UART5_BASE);

    cust_print("\nNFC UART Init Done");

//    static uint32_t tmp;

//    tmp = UARTFlowControlGet(UART5_BASE);

    UARTFlowControlSet(UART5_BASE, UART_FLOWCONTROL_RX);

//    if(tmp & UART_FLOWCONTROL_TX)  cust_print("\nTX Flow Control ON");
//    if(tmp & UART_FLOWCONTROL_RX)  cust_print("\nRX Flow Control ON");

    vTaskDelay(3000);

    static uint8_t i;

    cust_print("\nSending Wakeup");

    for(i = 0; i < 24; i ++)    UARTCharPut(UART5_BASE, wake[i]);

    vTaskDelay(100);

    cust_print("\nReading ACK");

    for(i = 0; i < 15; i ++)
    {
        receive_ACK[i] = (uint8_t)UARTCharGet(UART5_BASE);
    }

    vTaskDelay(100);

    cust_print("\nDisplaying ACK");

    for(i = 0; i < 15; i ++)
    {
        snprintf(tp, sizeof(tp), "\nRx[%d]: %x", i, receive_ACK[i]);
        cust_print(tp);
    }
/*
    cust_print("\nDisplaying Version");
    for(i = 0; i < 9; i ++)    UARTCharPut(UART5_BASE, firmware[i]);;
    vTaskDelay(100);
    for(i = 0; i < 19; i ++)
    {
        receive_ACK[i] = (uint8_t)UARTCharGet(UART5_BASE);
        vTaskDelay(100);
    }
    vTaskDelay(100);
    for(i = 0; i < 19; i ++)
    {
        snprintf(tp, sizeof(tp), "\nRx[%d]: %x", i, receive_ACK[i]);
        cust_print(tp);
    }*/

    cust_print("\nNormal Operation Start");

    while(1)
    {
        cust_print("\nSend Tag Command");
        for(i = 0; i < 11; i ++)    UARTCharPut(UART5_BASE, tag[i]);

        cust_print("\nRead Tag");
        for(i = 0; i < 6; i ++)
        {
            receive_ACK[i] = (uint8_t)UARTCharGet(UART5_BASE);
        }
        for(i = 0; i < 6; i ++)
        {
            snprintf(tp, sizeof(tp), "\nRx[%d]: %x", i, receive_ACK[i]);
            cust_print(tp);
        }
        for(; i < 25; i ++)
        {
            receive_ACK[i] = (uint8_t)UARTCharGet(UART5_BASE);
        }
        for(; i < 25; i ++)
        {
            snprintf(tp, sizeof(tp), "\nRx[%d]: %x", i, receive_ACK[i]);
            cust_print(tp);
        }
        vTaskDelay(100);

        cust_print("\nDisplay Tag");
        for(i = 0; i < 25; i ++)
        {
            snprintf(tp, sizeof(tp), "\nRx[%d]: %x", i, receive_ACK[i]);
            cust_print(tp);
        }
        vTaskDelay(1000);
    }
}
