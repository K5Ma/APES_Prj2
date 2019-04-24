/*
 * EPaperTask.c
 *
 *  Created on: Apr 12, 2019
 *  Last Update: Apr 21, 2019
 *      Author: Poorn Mehta
 *
 *  Driver for displaying images residing in SD card on Waveshare 4.3Inch UART E-Paper Display
 *  using TM4C1294XL (TIVA Development Board)
 *
 *  This driver code is developed based on provided documentation from the manufacturer
 *
 */

/*
 *
 * Hardware Connections
 *
 * Note: WakeUp and Reset pins are not being utilized as of now
 *       I am planning to add the wakeup feature in future to save power,
 *       but currently it is not a high priority task
 *
 * Note: Left Side - Module Pins & Right Side - Controller Pins
 *
 * Blue Wire (RST, pin1) - Not Connected
 * Yellow Wire (WAKE_UP, pin2) - Not Connected
 * Green Wire (DIN, pin3) - PC5 (UART7_Tx)
 * White Wire (DOUT, pin4) - PC4 (UART7_Rx)
 * Black Wire (GND, pin5) - Ground
 * Red Wire (VCC, pin6) - 5V
 *
 */

#include "EPaperTask.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Variables that will be shared between functions
uint8_t EP_Command_Array[EP_Command_Max_Length], EP_Retries;
uint8_t EP_Image_String_Counter, EP_Command_Array_Universal_Counter, EP_Response_ms_Counter, EP_Response_Init;
uint16_t EP_Full_Command_Length, EP_Response_us_Counter;

bool EP_Error;

/*
 * Function to generate parity
 *
 * Param: Command Length
 *
 * Return: Calculated Parity Byte
 *
 */
uint8_t EP_Parity_Generator(uint16_t EP_Command_Length)
{
    static uint8_t EP_Parity_Loop_Counter, EP_Parity;
    EP_Parity = 0x00;
    EP_Command_Length -= 1;
    for(EP_Parity_Loop_Counter = 0; EP_Parity_Loop_Counter < EP_Command_Length; EP_Parity_Loop_Counter ++)
    {
        EP_Parity ^= EP_Command_Array[EP_Parity_Loop_Counter];
    }
    return EP_Parity;
}

/*
 * Function to initialize UART7 for E-Paper
 *
 * Param: Null
 * Return: Null
 *
 */
void EP_UART_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);

    GPIOPinConfigure(GPIO_PC4_U7RX);
    GPIOPinConfigure(GPIO_PC5_U7TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    UARTConfigSetExpClk(UART7_BASE, SYSTEM_CLOCK, 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
}

/*
 * Function to transmit 1 byte over UART
 *
 * Param: Byte to transmit
 *
 * Return: Null
 *
 */
void EP_UART_Tx(uint8_t tx_byte)
{
    UARTCharPut(UART7_BASE, tx_byte);
}

/*
 * Function to receive 1 byte over UART with timeout
 *
 * Param: Null
 *
 * Return: Either Timeout Byte (0xFF) or received character
 *
 */
uint8_t EP_UART_Rx(void)
{
    EP_Response_ms_Counter = 0;

    while (HWREG(UART7_BASE + UART_O_FR) & UART_FR_RXFE)
    {
        vTaskDelay(1);
        EP_Response_ms_Counter += 1;
        if(EP_Response_Init == 0)
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

/*
 * Function to get back the entire response of module
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void EP_Get_Response(void)
{
    static volatile uint8_t Rx_Byte;
    static uint8_t rx_arr[50], i, j;
    static char tp[5];
    rx_arr[0] = 0;
    i = 0;
    do
    {
        Rx_Byte = EP_UART_Rx();
        if((Rx_Byte >= Valid_ASCII_Lowest) && (Rx_Byte <= Valid_ASCII_Highest))
        {
            EP_Response_Init = 0xFF;
            rx_arr[i++] = Rx_Byte;
        }
    }while(Rx_Byte != 0xFF);

    // Error Handling if EPaper is disconnected
    // It is done while supporting both - infinite as well as limited retries
    if(EP_Error == false)
    {
        if(rx_arr[0] == 0)
        {
            EP_Error = true;
            #if     (EP_Retry_Mode == EP_Limited)
                EP_Retries = EP_Max_Retries;
            #endif
        }
    }
    else
    {
        if(rx_arr[0] != 0)
        {
            EP_Error = false;
            #if     (EP_Retry_Mode == EP_Limited)
                EP_Retries = 0;
            #endif
        }
    }

#if     EP_DEBUG_PRINTF
    for(j = 0; j < i; j ++)
    {
        snprintf(tp, sizeof(tp), "%c", rx_arr[j]);
        EP_Print(tp);
    }
#endif
}

/*
 * Function to send command over UART
 *
 * Param: Command ID
 * Param: Command Type
 *
 * Return: Null
 *
 */
void EP_Send_Command(uint8_t EP_Command_ID, uint8_t EP_Command_Type)
{
//    static char tp[5];
    static uint8_t EP_Command_Array_Local_Counter;
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
    if(EP_Command_Type == EP_Storage_Set_Type)
    {
        EP_Command_Array[EP_Command_Array_Local_Counter ++] = EP_Set_SD_Storage;
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
//    EP_Print("\n");
//    for(EP_Command_Array_Local_Counter = 0; EP_Command_Array_Local_Counter < EP_Full_Command_Length; EP_Command_Array_Local_Counter ++)
//    {
//        snprintf(tp, sizeof(tp), "%x ", EP_Command_Array[EP_Command_Array_Local_Counter]);
//        EP_Print(tp);
//    }
    for(EP_Command_Array_Local_Counter = 0; EP_Command_Array_Local_Counter < EP_Full_Command_Length; EP_Command_Array_Local_Counter ++)
    {
        EP_UART_Tx(EP_Command_Array[EP_Command_Array_Local_Counter]);
    }
    EP_Response_Init = 0x00;

#if     EP_DEBUG_PRINTF
    EP_Print("\nResp: ");
#endif

    EP_Get_Response();
}

/*
 * Function to set command array for drawing a circle
 *
 * Param: X axis position of the center of the circle
 * Param: Y axis position of the center of the circle
 * Param: Radius of the circle
 *
 * Return: Null
 *
 */
void EP_Set_Circle_CMD(uint16_t EP_Circle_Center_X_Pos, uint16_t EP_Circle_Center_Y_Pos, uint16_t EP_Circle_Radius)
{
    EP_Command_Array_Universal_Counter = 4;
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)((EP_Circle_Center_X_Pos & 0xFF00) >> 8);
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)(EP_Circle_Center_X_Pos & 0x00FF);
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)((EP_Circle_Center_Y_Pos & 0xFF00) >> 8);
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)(EP_Circle_Center_Y_Pos & 0x00FF);
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)((EP_Circle_Radius & 0xFF00) >> 8);
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)(EP_Circle_Radius & 0x00FF);
}

/*
 * Function to draw circle on the screen
 *
 * Param: X axis position of the center of the circle
 * Param: Y axis position of the center of the circle
 * Param: Radius of the circle
 *
 * Return: Null
 *
 */
void EP_Draw_Circle(uint16_t EP_Circle_Center_X_Pos, uint16_t EP_Circle_Center_Y_Pos, uint16_t EP_Circle_Radius)
{
  EP_Send_Command(EP_CMD_Set_Draw_Color, EP_Set_Draw_Color_Type);
  EP_Send_Command(EP_CMD_Get_Draw_Color, EP_Get_Draw_Color_Type);
  EP_Send_Command(EP_CMD_Clear, EP_Clear_Type);
  EP_Set_Circle_CMD(EP_Circle_Center_X_Pos, EP_Circle_Center_Y_Pos, EP_Circle_Radius);
  EP_Send_Command(EP_CMD_Fill_Circle, EP_Fill_Circle);
  EP_Send_Command(EP_CMD_Refresh_Update, EP_Refresh_Update_Type);
}

/*
 * Function to set command array for displaying image
 *
 * Param: String containing image name
 * Param: X axis position of the pixel to start drawing from (top left is (0,0))
 * Param: Y axis position of the pixel to start drawing from (top left is (0,0))
 *
 * Return: Null
 *
 */
void EP_Set_Image_CMD(char *EP_Image_Name, uint16_t EP_Image_X_Start_Pos, uint16_t EP_Image_Y_Start_Pos)
{
    EP_Image_String_Counter = 0;
    EP_Command_Array_Universal_Counter = 4;
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)((EP_Image_X_Start_Pos & 0xFF00) >> 8);
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)(EP_Image_X_Start_Pos & 0x00FF);
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)((EP_Image_Y_Start_Pos & 0xFF00) >> 8);
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = (uint8_t)(EP_Image_Y_Start_Pos & 0x00FF);
    while(*EP_Image_Name != 0x00)
    {
        EP_Command_Array[EP_Command_Array_Universal_Counter ++] = *EP_Image_Name++;
        EP_Image_String_Counter += 1;
    }
    EP_Command_Array[EP_Command_Array_Universal_Counter ++] = 0;
    EP_Image_String_Counter += 1;
    EP_Full_Command_Length = EP_Display_Image_CMD_Length(EP_Image_String_Counter);
}

/*
 * Function to display image on screen
 *
 * Param: String containing image name
 * Param: X axis position of the pixel to start drawing from (top left is (0,0))
 * Param: Y axis position of the pixel to start drawing from (top left is (0,0))
 *
 * Return: Null
 *
 */
void EP_Display_Image(char *EP_Image_Name, uint16_t EP_Image_X_Start_Pos, uint16_t EP_Image_Y_Start_Pos)
{
    EP_Set_Image_CMD(EP_Image_Name, 0, 0);
    EP_Send_Command(EP_CMD_Display_Image, EP_CMD_Display_Image_Type);
    EP_Send_Command(EP_CMD_Refresh_Update, EP_Refresh_Update_Type);
}

/*
 *
 * Callback Function for Electronic Paper Task
 *
 * Return: Null
 *
 */
void EPaperTask(void *pvParameters)
{
    EP_UART_Init();

    EP_Error = false;

    // This is completely optional
    // I use it to have enough time to open serial monitor
    // and to start logic analyzer when needed :P
    vTaskDelay(3000);

#if     EP_DEBUG_PRINTF
    EP_Print("\nHandshake");
#endif
    EP_Send_Command(EP_CMD_Handshake, EP_Handshake_Type);

#if     EP_DEBUG_PRINTF
    EP_Print("\nStorage Set");
#endif
    EP_Send_Command(EP_CMD_Storage_Set, EP_Storage_Set_Type);

#if     EP_INDIVIDUAL_TESTING

    /*
     * This portion just tests the EPaper individually
     * It doesn't rely on anything that is not covered
     * or provided by its own header and source files
     *
     * This mode is only for raw testing, and may not
     * include any of the error checking feature(s)
     */

    #if     EP_DEBUG_PRINTF
        EP_Print("\nStorage Get");
    #endif
        EP_Send_Command(EP_CMD_Storage_Get, EP_Storage_Get_Type);

    #if     EP_DEBUG_PRINTF
        EP_Print("\nDisplay Circle");
    #endif
        EP_Draw_Circle(400, 300, 200);

        vTaskDelay(2500);

    #if     EP_DEBUG_PRINTF
        EP_Print("\nDisplay Image");
    #endif
        EP_Display_Image("123.BMP", 0, 0);

        EP_Print("\nEPaper End");

#else
        /*
         * Normal Operation (Parameters and Returns indicate communication with Central Task)
         *
         * This Task should get 2 parameters from central task through IPC
         *
         * This Task has no data to report to / send back to the Control Node
         * It only reports back a boolean indicating current state of the EPaper
         * (false: offline. true: online)
         *
         *
         * Param_1: bool Update_EPaper
         *          (false: don't do anything, true: update the EPaper Display)
         *
         * Param_2: char *Image_Name
         *          (don't care if Update_Epaper is false, otherwise should contain
         *           the string with full image name: ex- "123.BMP". Please note that
         *           the image name can't be more than 10 characters in length,
         *           including the '.')
         *
         * Return: bool EP_Error
         *         (false: Online, true: OFfline - error present)
         *
         */

        static char Image_Name_Local[10];
        static uint8_t idletimecount;

        idletimecount = 0;

        while(1)
        {
            // Testing whether EPaper is still functional - and connected with the Controller
            // This is done by sending a handshake, and later on checking error flag
            if(idletimecount >= (EP_Online_Test_Timems / EP_Reset_Timems))
            {
                idletimecount = 0;
                EP_Send_Command(EP_CMD_Handshake, EP_Handshake_Type);
                #if     EP_DEBUG_PRINTF
                    EP_Print("\nChecking EPaper Status... Command: Handshake");
                    if(EP_Error == false)   EP_Print("\nEPaper is Online");
                    else    EP_Print("\nEPaper if Offline");
                #endif
            }

            // Attempting to communicate with EPaper while supporting both
            // Limited and Infinite retry modes
            if(EP_Error == true)
            {
            #if     (EP_Retry_Mode == EP_Limited)
                if(EP_Retries != 0x00)
                {
                    EP_Retries -= 1;
            #endif
                    #if     EP_DEBUG_PRINTF
                        EP_Print("\nRetrying Connection with EPaper... Command: Handshake");
                    #endif
                        EP_UART_Init();
                        EP_Send_Command(EP_CMD_Handshake, EP_Handshake_Type);
                        if(EP_Error == true)
                        {
                            #if     EP_DEBUG_PRINTF
                                EP_Print("\nRetry failed with EPaper");
                            #endif
                        }
                        else
                        {
                            #if     EP_DEBUG_PRINTF
                                EP_Print("\nEPaper is Online now");
                            #endif
                        }
            #if     (EP_Retry_Mode == EP_Limited)
                }
            #endif
            }

            if((Update_EPaper == true) && (EP_Error == false))
            {
               // Creating a local copy of the string - just in case if the original is global
               // and there is a chance of that getting corrupted
               strncpy(Image_Name_Local, Image_Name, sizeof(Image_Name_Local));

               // Display image on screen, for now - always start drawing from top left corner
               EP_Display_Image(Image_Name_Local, 0, 0);

               // Update the global status/variable if possible - to prevent redisplaying
               // the same image again and again. Redisplaying must be taken care of in
               // some other way if this is not doable, since EPaper refresh cycles are
               // very limited - and redrawing uselessly will kill the display eventually
               Update_EPaper = false;
            }

            // The following function is optional, but highly recommended
            // for two purposes: 1 - EPaper Update rate is slow (requires 3 seconds)
            // and 2 - FreeRTOS knows that this task has nothing important to do for
            // this much of time, and doesn't waste resources (even for a short amount
            // of time) to check the status of Update_EPaper
            vTaskDelay(EP_Reset_Timems);
            idletimecount += 1;
        }

#endif
}
