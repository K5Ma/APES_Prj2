/*
 * SpeakJetTask.c
 *
 *  Created on: Apr 20, 2019
 *      Author: poorn
 */


#include "SpeakJetTask.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Set up a memorable token for "Word Pause"
#define WP 6    // 6 is 90ms pause

uint8_t message[] = {
/* hello    */ 183, 007, 159, 146, 164, WP, WP,
/* my       */ 140, 155, WP,
/* name     */ 141, 154, 140, WP,
/* is       */ 8, 129, 167, WP,
/* arduino  */ 152, 148, 175, 147, 128, 141, 164, WP
};

uint8_t message2[] = {
/* good    */  8, 179, 138, 138, 177, WP,
/* afternoon */ 132, 186, 7, 191, 7, 151, 7, 141, 139, 8, 141, WP, WP, WP,
/* poorn     */ 199, 7, 137, 153, 141, WP
};

uint8_t message3[] = {
/* good    */  8, 179, 138, 138, 177, WP,
/* afternoon       */ 132, 186, 7, 191, 7, 151, 7, 141, 139, 8, 141, WP, WP, WP,
/* khalid     */ 8, 194, 8, 183, 8, 145, 128, 174, WP
};

int messageSize;// = sizeof(message2);

void SpeakJetTask(void *pvParameters)
{
    static uint8_t i;

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

    while(1)
    {
        for (i=0; i<sizeof(message2); i++)
        {
            UARTCharPut(UART6_BASE, message2[i]);
        }
        vTaskDelay(3500);
        for (i=0; i<sizeof(message3); i++)
        {
            UARTCharPut(UART6_BASE, message3[i]);
        }
        vTaskDelay(3500);
    }

}

