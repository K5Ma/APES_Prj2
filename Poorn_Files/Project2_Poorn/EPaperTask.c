/*
 * EPaperTask.c
 *
 *  Created on: Apr 12, 2019
 *      Author: poorn
 */


#include "EPaperTask.h"

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

uint8_t EP_Parity_Loop_Counter, EP_Command_Array[EP_Command_Max_Length], EP_Parity, EP_String_Length, EP_Image_Name_Length;
uint8_t EP_Image_String_Counter, EP_Command_Array_Universal_Counter, EP_Command_Array_Local_Counter, EP_Response_ms_Counter, EP_Response_Init;
volatile uint8_t Rx_Byte;
uint16_t EP_Full_Command_Length, EP_Response_us_Counter;

uint8_t EP_Parity_Generator(uint16_t EP_Command_Length)
{
    EP_Parity = 0x00;
    EP_Command_Length -= 1;
    for(EP_Parity_Loop_Counter = 0; EP_Parity_Loop_Counter < EP_Command_Length; EP_Parity_Loop_Counter ++)
    {
        EP_Parity ^= EP_Command_Array[EP_Parity_Loop_Counter];
    }
    return EP_Parity;
}

void EP_Display_Image(char EP_Image_Name[], uint16_t EP_Image_X_Start_Pos, uint16_t EP_Image_Y_Start_Pos)
{
    EP_Image_String_Counter = 0;
    EP_Command_Array_Universal_Counter = 4;
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)((EP_Image_X_Start_Pos & 0xFF00) >> 8);
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)(EP_Image_X_Start_Pos & 0x00FF);
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)((EP_Image_Y_Start_Pos & 0xFF00) >> 8);
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)(EP_Image_Y_Start_Pos & 0x00FF);
    while(EP_Image_Name[EP_Image_String_Counter] != 0x00)
    {
        EP_Command_Array[EP_Command_Array_Universal_Counter ++] = EP_Image_Name[EP_Image_String_Counter ++];
    }
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = 0x00;
    EP_Image_String_Counter += 1;
    EP_Full_Command_Length = EP_Display_Image_CMD_Length(EP_Image_String_Counter);
}

void EP_Display_Circle(uint16_t EP_Circle_Center_X_Pos, uint16_t EP_Circle_Center_Y_Pos, uint16_t EP_Circle_Radius)
{
    EP_Command_Array_Universal_Counter = 4;
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)((EP_Circle_Center_X_Pos & 0xFF00) >> 8);
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)(EP_Circle_Center_X_Pos & 0x00FF);
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)((EP_Circle_Center_Y_Pos & 0xFF00) >> 8);
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)(EP_Circle_Center_Y_Pos & 0x00FF);
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)((EP_Circle_Radius & 0xFF00) >> 8);
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)(EP_Circle_Radius & 0x00FF);
}

uint8_t EP_UART_Rx(void)
{
    EP_Response_ms_Counter = 0;

    while (HWREG(UART7_BASE + UART_O_FR) & UART_FR_RXFE)
    {
        vTaskDelay(1);
        EP_Response_ms_Counter += 1;
        if(EP_Response_Init == 0x00)
        {
            if(EP_Response_ms_Counter >= EP_Response_Start_Timeout_ms)   return 0xFF;
        }
        else
        {
            if(EP_Response_ms_Counter >= EP_Response_End_Timeout_ms) return 0xFF;
        }
    }

    return (HWREG(UART7_BASE + UART_O_DR));
}

void EP_Get_Response(void)
{
    uint8_t rx_arr[50], j;
    static char tp[5];
    uint8_t i = 0;
    do
    {
        Rx_Byte = EP_UART_Rx();
        if((Rx_Byte >= Valid_ASCII_Lowest) && (Rx_Byte <= Valid_ASCII_Highest))
        {
            EP_Response_Init = 0xFF;
            rx_arr[i++] = Rx_Byte;
        }
    }while(Rx_Byte != 0xFF);
    for(j = 0; j < i; j ++)
    {
        snprintf(tp, sizeof(tp), "%c", rx_arr[j]);
        cust_print(tp);
    }
}

void EP_Send_Command(uint8_t EP_Command_ID, uint8_t EP_Command_Type)
{
//    static char tp[5];
    if(EP_Command_Type != 0x00)
    {
        if(EP_Command_Type == 0x01) EP_Full_Command_Length = EP_Type1_Length;
        else if(EP_Command_Type == 0x02)    EP_Full_Command_Length = EP_Type2_Length;
        else if(EP_Command_Type == 0x03)    EP_Full_Command_Length = EP_Type3_Length;
        else if(EP_Command_Type == 0x04)    EP_Full_Command_Length = EP_Type4_Length;
    }
    EP_Command_Array_Local_Counter = 0;
    EP_Command_Array[EP_Command_Array_Local_Counter ++] = EP_Frame_Header;
    EP_Command_Array[EP_Command_Array_Local_Counter ++] = (uint8_t)((EP_Full_Command_Length & 0xFF00) >> 8);
    EP_Command_Array[EP_Command_Array_Local_Counter ++] = (uint8_t)(EP_Full_Command_Length & 0x00FF);
    EP_Command_Array[EP_Command_Array_Local_Counter ++] = EP_Command_ID;
    if(EP_Command_Type == 0x02)
    {
        EP_Command_Array[EP_Command_Array_Local_Counter ++] = 0x01;     // Currently built for setting storage to TF card. Build this further for inclusion of additional functions with length of 0x0A (type2)
    }
    else if(EP_Command_Type == 0x03)
    {
        EP_Command_Array[EP_Command_Array_Local_Counter ++] = 0x03;
        EP_Command_Array[EP_Command_Array_Local_Counter ++] = 0x00;
    }
    else if((EP_Command_Type == 0x00) || (EP_Command_Type == 0x04))
    {
        EP_Command_Array_Local_Counter = EP_Command_Array_Universal_Counter;
    }
    EP_Command_Array[EP_Command_Array_Local_Counter ++] = EP_Frame_End_1;
    EP_Command_Array[EP_Command_Array_Local_Counter ++] = EP_Frame_End_2;
    EP_Command_Array[EP_Command_Array_Local_Counter ++] = EP_Frame_End_3;
    EP_Command_Array[EP_Command_Array_Local_Counter ++] = EP_Frame_End_4;
    EP_Command_Array[EP_Command_Array_Local_Counter ++] = EP_Parity_Generator(EP_Full_Command_Length);
//    cust_print("\n");
//    for(EP_Command_Array_Local_Counter = 0; EP_Command_Array_Local_Counter < EP_Full_Command_Length; EP_Command_Array_Local_Counter ++)
//    {
//        snprintf(tp, sizeof(tp), "%x ", EP_Command_Array[EP_Command_Array_Local_Counter]);
//        cust_print(tp);
//    }
    for(EP_Command_Array_Local_Counter = 0; EP_Command_Array_Local_Counter < EP_Full_Command_Length; EP_Command_Array_Local_Counter ++)
    {
        UARTCharPut(UART7_BASE, EP_Command_Array[EP_Command_Array_Local_Counter]);
    }
    EP_Response_Init = 0x00;
    cust_print("\nResp: ");
    EP_Get_Response();
}


void epaper_all(void *pvParameters)
{

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);

    GPIOPinConfigure(GPIO_PC4_U7RX);
    GPIOPinConfigure(GPIO_PC5_U7TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    UARTConfigSetExpClk(UART7_BASE, SYSTEM_CLOCK, 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    vTaskDelay(3000);

//    cust_print("\nEPaper Wakeup");

    //wakeup
//    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, false);
//    vTaskDelay(1);
//    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, true);
//    vTaskDelay(1);
//    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, false);
//    vTaskDelay(10);

//    cust_print("\nEPaper Reset");

    //reset
//    GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_1, false);
//    vTaskDelay(1);
//    GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_1, true);
//    vTaskDelay(1);
//    GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_1, false);
//    vTaskDelay(3000);

    cust_print("\nHandshake");

    EP_Send_Command(EP_CMD_Handshake, EP_Handshake_Type);
//    Rx_Byte = 0;
//    while(Rx_Byte == 0)
//    {
//        EP_Send_Command(EP_CMD_Handshake, EP_Handshake_Type);
//        Rx_Byte = (volatile uint8_t)EP_UART_Rx();
//    }

//    uint8_t tarr[50] = {0};
//    char tprint[30];
//
//    Rx_Byte = 0;
//    while(Rx_Byte == 0)
//    {
//        EP_Send_Command(EP_CMD_Handshake, EP_Handshake_Type);
//        Rx_Byte = (volatile uint8_t)UARTCharGet(UART7_BASE);
//    }
//    snprintf(tprint, sizeof(tprint), "\nGot %c", Rx_Byte);
//    cust_print(tprint);
//    Rx_Byte = (volatile uint8_t)UARTCharGet(UART7_BASE);
//    snprintf(tprint, sizeof(tprint), "\nGot %c", Rx_Byte);
//    cust_print(tprint);
//    vTaskDelay(1);
    cust_print("\nStorage Set");
    EP_Send_Command(EP_CMD_Storage_Set, EP_Storage_Set_Type);

//    Rx_Byte = 0;
//    while(Rx_Byte == 0)
//    {
//        EP_Send_Command(EP_CMD_Storage_Set, EP_Storage_Set_Type);
//        Rx_Byte = (volatile uint8_t)UARTCharGet(UART7_BASE);
//    }
//    if(Rx_Byte == 'E')
//    {
//        snprintf(tprint, sizeof(tprint), "\nGot %c", Rx_Byte);
//        cust_print(tprint);
//
//        Rx_Byte = (volatile uint8_t)UARTCharGet(UART7_BASE);
//        snprintf(tprint, sizeof(tprint), "\nGot %c", Rx_Byte);
//        cust_print(tprint);
//
//        Rx_Byte = (volatile uint8_t)UARTCharGet(UART7_BASE);
//        snprintf(tprint, sizeof(tprint), "\nGot %c", Rx_Byte);
//        cust_print(tprint);
//
//        Rx_Byte = (volatile uint8_t)UARTCharGet(UART7_BASE);
//        snprintf(tprint, sizeof(tprint), "\nGot %c", Rx_Byte);
//        cust_print(tprint);
//
//        Rx_Byte = (volatile uint8_t)UARTCharGet(UART7_BASE);
//        snprintf(tprint, sizeof(tprint), "\nGot %c", Rx_Byte);
//        cust_print(tprint);
//
//        Rx_Byte = (volatile uint8_t)UARTCharGet(UART7_BASE);
//        snprintf(tprint, sizeof(tprint), "\nGot %c", Rx_Byte);
//        cust_print(tprint);
//
//        Rx_Byte = (volatile uint8_t)UARTCharGet(UART7_BASE);
//        snprintf(tprint, sizeof(tprint), "\nGot %c", Rx_Byte);
//        cust_print(tprint);
//    }
//    else
//    {
//        snprintf(tprint, sizeof(tprint), "\nGot %c", Rx_Byte);
//        cust_print(tprint);
//    }

//    vTaskDelay(1);
    cust_print("\nStorage Get");
    EP_Send_Command(EP_CMD_Storage_Get, EP_Storage_Get_Type);

//    Rx_Byte = 0;
//    while(Rx_Byte == 0)
//    {
//        EP_Send_Command(EP_CMD_Storage_Get, EP_Storage_Get_Type);
//        Rx_Byte = (volatile uint8_t)UARTCharGet(UART7_BASE);
//    }
//    snprintf(tprint, sizeof(tprint), "\nGot %c", Rx_Byte);
//    cust_print(tprint);

    cust_print("\nDisplay Circle");
//    vTaskDelay(1);
  EP_Send_Command(EP_CMD_Set_Draw_Color, EP_Set_Draw_Color_Type);
  EP_Send_Command(EP_CMD_Get_Draw_Color, EP_Get_Draw_Color_Type);
  EP_Send_Command(EP_CMD_Clear, EP_Clear_Type);
  EP_Display_Circle(400, 300, 200);
  EP_Send_Command(EP_CMD_Fill_Circle, EP_Fill_Circle);

    vTaskDelay(2500);
    EP_Send_Command(EP_CMD_Refresh_Update, EP_Refresh_Update_Type);
        cust_print("\nDisplay Image");
        EP_Display_Image("123.BMP", 0x0000, 0x0000);
        EP_Send_Command(EP_CMD_Display_Image, 0x00);
        EP_Send_Command(EP_CMD_Refresh_Update, EP_Refresh_Update_Type);
//    vTaskDelay(1);
//    EP_Send_Command(EP_CMD_Stop, EP_Stop_Type);
//    vTaskDelay(1);
    cust_print("\nEPaper End");
}
