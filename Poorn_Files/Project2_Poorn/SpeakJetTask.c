/*
 * SpeakJetTask.c
 *
 *  Created on: Apr 20, 2019
 *      Author: poorn
 */

//hw
//pin 1 - 9 (all on left) - ground
//pin 10 - PP1
// pin 11 & 12 to 5V using resistor
// pin 13 ground
// pin 14 5V
// pin 18 speaker output

#include "SpeakJetTask.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

uint8_t SJ_Data;

const uint8_t SJ_Welcome[] = {147, 159, 194, 134, 140, WP, WP, WP, WP, WP};
const uint8_t SJ_And[] = {8, 132, 8, 141, 177, WP, WP};
const uint8_t SJ_Have[] = {183, 8, 132, 166, WP, WP, WP};
const uint8_t SJ_A[] = {154, 128, WP, WP, WP};

const uint8_t SJ_Good[] = {8, 179, 138, 138, 177, WP, WP};

const uint8_t SJ_Morning[] =  {140, 7, 137, 7, 153, 141, 129, 143, WP, WP, WP};
const uint8_t SJ_Afternoon[] = {132, 186, 7, 191, 7, 151, 7, 141, 139, 8, 141, WP, WP, WP};
const uint8_t SJ_Evening[] = {8, 128, 166, 130, 141, 129, 143, WP, WP, WP};
const uint8_t SJ_Night[] = {140, 7, 135, 155, 191, WP, WP, WP};

const uint8_t SJ_Poorn[] = {199, 7, 137, 153, 141, WP};
const uint8_t SJ_Khalid[] = {8, 194, 8, 183, 8, 145, 128, 174, WP};

void SJ_Select_Message(void)
{
    static uint8_t i, sw_var;

    for (i = 0; i < sizeof(SJ_Welcome); i ++)    UARTCharPut(UART6_BASE, SJ_Welcome[i]);
    for (i = 0; i < sizeof(SJ_And); i ++)        UARTCharPut(UART6_BASE, SJ_And[i]);
    for (i = 0; i < sizeof(SJ_Have); i ++)       UARTCharPut(UART6_BASE, SJ_Have[i]);
    for (i = 0; i < sizeof(SJ_A); i ++)       UARTCharPut(UART6_BASE, SJ_A[i]);
    for (i = 0; i < sizeof(SJ_Good); i ++)       UARTCharPut(UART6_BASE, SJ_Good[i]);

    sw_var = SJ_Data & SJ_DayTime_Mask;
    switch(sw_var)
    {
        case    SJ_Morning_Time:
            cust_print("\nMorning");
            for (i = 0; i < sizeof(SJ_Morning); i ++)       UARTCharPut(UART6_BASE, SJ_Morning[i]);
            break;
        case    SJ_Afternoon_Time:
            cust_print("\nAfternoon");
            for (i = 0; i < sizeof(SJ_Afternoon); i ++)       UARTCharPut(UART6_BASE, SJ_Afternoon[i]);
            break;
        case    SJ_Evening_Time:
            cust_print("\nEvening");
            for (i = 0; i < sizeof(SJ_Evening); i ++)       UARTCharPut(UART6_BASE, SJ_Evening[i]);
            break;
        case    SJ_Night_Time:
            cust_print("\nNight");
            for (i = 0; i < sizeof(SJ_Night); i ++)       UARTCharPut(UART6_BASE, SJ_Night[i]);
            break;
        default:
            break;
    }

    sw_var = SJ_Data & SJ_Person_ID_Mask;
    switch(sw_var)
    {
        case    SJ_Person_ID_Poorn:
            cust_print(" Poorn\n");
            for (i = 0; i < sizeof(SJ_Poorn); i ++)       UARTCharPut(UART6_BASE, SJ_Poorn[i]);
            break;
        case    SJ_Person_ID_Khalid:
            cust_print(" Khalid\n");
            for (i = 0; i < sizeof(SJ_Khalid); i ++)       UARTCharPut(UART6_BASE, SJ_Khalid[i]);
            break;
        default:
            break;
    }

}

void SpeakJetTask(void *pvParameters)
{

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
        SJ_Data = 0;
        SJ_Data |= SJ_Enable_Mask;
        SJ_Data |= SJ_Morning_Time;
        SJ_Data |= SJ_Person_ID_Poorn;
        SJ_Select_Message();
        vTaskDelay(5000);

        SJ_Data = 0;
        SJ_Data |= SJ_Enable_Mask;
        SJ_Data |= SJ_Morning_Time;
        SJ_Data |= SJ_Person_ID_Khalid;
        SJ_Select_Message();
        vTaskDelay(5000);

        SJ_Data = 0;
        SJ_Data |= SJ_Enable_Mask;
        SJ_Data |= SJ_Afternoon_Time;
        SJ_Data |= SJ_Person_ID_Poorn;
        SJ_Select_Message();
        vTaskDelay(5000);

        SJ_Data = 0;
        SJ_Data |= SJ_Enable_Mask;
        SJ_Data |= SJ_Afternoon_Time;
        SJ_Data |= SJ_Person_ID_Khalid;
        SJ_Select_Message();
        vTaskDelay(5000);

        SJ_Data = 0;
        SJ_Data |= SJ_Enable_Mask;
        SJ_Data |= SJ_Evening_Time;
        SJ_Data |= SJ_Person_ID_Poorn;
        SJ_Select_Message();
        vTaskDelay(5000);

        SJ_Data = 0;
        SJ_Data |= SJ_Enable_Mask;
        SJ_Data |= SJ_Evening_Time;
        SJ_Data |= SJ_Person_ID_Khalid;
        SJ_Select_Message();
        vTaskDelay(5000);

        SJ_Data = 0;
        SJ_Data |= SJ_Enable_Mask;
        SJ_Data |= SJ_Night_Time;
        SJ_Data |= SJ_Person_ID_Poorn;
        SJ_Select_Message();
        vTaskDelay(5000);

        SJ_Data = 0;
        SJ_Data |= SJ_Enable_Mask;
        SJ_Data |= SJ_Night_Time;
        SJ_Data |= SJ_Person_ID_Khalid;
        SJ_Select_Message();
        vTaskDelay(5000);
    }

}

