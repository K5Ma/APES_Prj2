/*
 * EPaper_Keypad_Task.c
 *
 *  Created on: Apr 26, 2019
 *      Author: poorn
 */

#include "EPaper_Keypad_Task.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Variables that will be shared between functions
uint8_t EP_Command_Array[EP_Command_Max_Length], EP_Retries;
uint8_t EP_Image_String_Counter, EP_Command_Array_Universal_Counter, EP_Response_Init;
uint16_t EP_Full_Command_Length, EP_Response_ms_Counter;

uint8_t KeyPad_Pos;
uint16_t KeyPad_Counter;

const char Key_LookUp[KeyPad_Rows][KeyPad_Cols] = {
                               {'1', '2', '3', 'A'},
                               {'4', '5', '6', 'B'},
                               {'7', '8', '9', 'C'},
                               {'*', '0', '#', 'D'}
};

bool EP_Update_Cmd, KeyPad_Error, EP_Error;

typedef struct {
    uint8_t KeyPad_Code[KeyPad_Code_Size];
} KE_T2B_Struct;

typedef struct {
    bool KeyPad_Poll;
    bool EP_Update;
    char Image_Name[10];
} KE_B2T_Struct;


KE_T2B_Struct EP_Tx;
KE_B2T_Struct EP_Rx;

void EPaper_Keypad_Task(void *pvParameters)
{
    EP_UART_Init();
    EP_Error = false;
    EP_Update_Cmd = false;

    EP_Rx.KeyPad_Poll = false;
//    EP_Rx.KeyPad_Poll = true;

    EP_Rx.EP_Update = false;
//    EP_Rx.EP_Update = true;
//    strncpy(EP_Rx.Image_Name, "P1.bmp", sizeof(EP_Rx.Image_Name));

    #if     EP_DEBUG_PRINTF
        EP_Print("\nHandshake");
    #endif
        EP_Send_Command(EP_CMD_Handshake, EP_Handshake_Type);
        if(EP_Error == false)
        {
            #if     EP_DEBUG_PRINTF
                EP_Print("\nStartup Handshake Succeeded");
            #endif
        }
        else
        {
            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send KE BIST Failure LOG to BB
            #if     EP_DEBUG_PRINTF
                EP_Print("\nStartup Handshake Failed");
            #endif
        }

    #if     EP_DEBUG_PRINTF
        EP_Print("\nStorage Set");
    #endif
        EP_Update_Cmd = true;
        EP_Send_Command(EP_CMD_Storage_Set, EP_Storage_Set_Type);
        if(EP_Error == false)
        {
            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send KE BIST Success LOG to BB
            #if     EP_DEBUG_PRINTF
                EP_Print("\nStorage Set Succeeded");
                EP_Print("\nStarting KE Normal Operation");
            #endif
        }
        else
        {
            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send KE BIST Failure LOG to BB
            #if     EP_DEBUG_PRINTF
                EP_Print("\nStorage Set Failed");
            #endif
        }
        EP_Update_Cmd = false;

    static uint8_t i;
    static char tp[50];

    KeyPad_Init();
    #if KeyPad_DEBUG_PRINTF
        KeyPad_Print("\nKeyPad Initialized");
    #endif

    while(1)
    {
        vTaskDelay(EP_Polling_Timems);
        // Testing whether EPaper is still functional - and connected with the Controller
        // This is done by sending a handshake, and later on checking error flag
        if(EP_Error == false)
        {
            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send KE Failure LOG to BB
            #if     EP_DEBUG_PRINTF
                EP_Print("\nChecking EPaper Status... Command: Handshake");
            #endif
            EP_Send_Command(EP_CMD_Handshake, EP_Handshake_Type);
            #if     EP_DEBUG_PRINTF
                if(EP_Error == false)   EP_Print("\nEPaper is Online");
                else    EP_Print("\nEPaper if Offline");
            #endif
        }

        // Attempting to communicate with EPaper while supporting both
        // Limited and Infinite retry modes
        else
        {
            //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send KE Online LOG to BB
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

        //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Receive EP_Rx structure from BBComm Task with 100ms Timeout

        if((EP_Rx.KeyPad_Poll == true) && (EP_Error == false))
        {
            KeyPad_Error = false;
            for(i = 0; i < KeyPad_Code_Size; i ++)      EP_Tx.KeyPad_Code[i] = 0;
            #if     (KeyPad_INDIVIDUAL_TESTING == false)
                EP_Rx.KeyPad_Poll = false;
            #endif
            KeyPad_Counter = 0;
            KeyPad_Pos = 0;

            #if KeyPad_DEBUG_PRINTF
                KeyPad_Print("\nCode Scanning Started");
            #endif

            while(1)
            {
                for(i = 0; i < KeyPad_Rows; i ++)
                {
                    KeyPad_SelectRow(i);
                    while(1)
                    {
                        if(KeyPad_ScanCode(i) == true)
                        {
                            #if KeyPad_DEBUG_PRINTF
                                KeyPad_Print("\nDigit Stored");
                            #endif
                        }
                        break;
                    }
                }

                if(KeyPad_Pos >= KeyPad_Code_Size)
                {
                    KeyPad_Error = false;
                    #if     (KeyPad_INDIVIDUAL_TESTING == false)
                    EP_Rx.KeyPad_Poll = false;
                    #endif
                    #if KeyPad_DEBUG_PRINTF
                        snprintf(tp, sizeof(tp), "\nEntered Code: %c%c%c%c%c%c", EP_Tx.KeyPad_Code[0], EP_Tx.KeyPad_Code[1],
                                 EP_Tx.KeyPad_Code[2], EP_Tx.KeyPad_Code[3], EP_Tx.KeyPad_Code[4], EP_Tx.KeyPad_Code[5]);
                        KeyPad_Print(tp);
                    #endif
                    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Transmit EP_Tx structure to BBComm Task
                    break;
                }
                else if(KeyPad_Counter >= (KeyPad_Timeoutms / KeyPad_Polling_Timems))
                {
                    KeyPad_Error = true;
                    #if KeyPad_DEBUG_PRINTF
                        KeyPad_Print("\nKeypad Timed Out");
                    #endif
                    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Send KE Failure LOG to BB
                    break;
                }
            }
        }

        if((EP_Rx.EP_Update == true) && (EP_Error == false))
        {
//            strncpy(EP_Rx.Image_Name, "123.BMP", sizeof(EP_Rx.Image_Name));

           // Display image on screen, for now - always start drawing from top left corner
           EP_Display_Image(EP_Rx.Image_Name, 0, 0);

           // Update the global status/variable if possible - to prevent redisplaying
           // the same image again and again. Redisplaying must be taken care of in
           // some other way if this is not doable, since EPaper refresh cycles are
           // very limited - and redrawing uselessly will kill the display eventually
           EP_Rx.EP_Update = false;

           // EPaper Update rate is slow (requires 3 seconds)
           vTaskDelay(EP_Reset_Timems - EP_Polling_Timems);
        }
    }
}

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
        if(EP_Update_Cmd == true)
        {
            if(EP_Response_Init == 0)
            {
                if(EP_Response_ms_Counter >= EP_Response_Start_Timeout_ms)      return 0xFF;
            }
            else
            {
                if(EP_Response_ms_Counter >= EP_Response_End_Timeout_ms)    return 0xFF;
            }
        }
        else
        {
            if(EP_Response_ms_Counter >= EP_Response_End_Timeout_ms)    return 0xFF;
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
  EP_Update_Cmd = true;
  EP_Send_Command(EP_CMD_Refresh_Update, EP_Refresh_Update_Type);
  EP_Update_Cmd = false;
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
    EP_Update_Cmd = true;
    EP_Send_Command(EP_CMD_Refresh_Update, EP_Refresh_Update_Type);
    EP_Update_Cmd = false;
}

/*
 * Function to setup GPIO pins for KeyPad
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void KeyPad_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_2);

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);

    GPIOPinTypeGPIOInput(GPIO_PORTH_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOInput(GPIO_PORTH_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOInput(GPIO_PORTK_BASE, GPIO_PIN_6);
    GPIOPinTypeGPIOInput(GPIO_PORTK_BASE, GPIO_PIN_7);

    GPIOPadConfigSet(GPIO_PORTH_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOPadConfigSet(GPIO_PORTH_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
}

/*
 * Function to select and set the specifc row high (3.3V)
 *
 * Param: uint8_t row (from 0 to 3)
 *
 * Return: Null
 *
 */
void KeyPad_SelectRow(uint8_t row)
{
    switch(row)
    {
        case 0:
            GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_5, GPIO_PIN_5);
            GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, false);
            GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_1, false);
            GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_2, false);
            break;
        case 1:
            GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_5, false);
            GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, GPIO_PIN_0);
            GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_1, false);
            GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_2, false);
            break;
        case 2:
            GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_5, false);
            GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, false);
            GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_1, GPIO_PIN_1);
            GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_2, false);
            break;
        case 3:
            GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_5, false);
            GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, false);
            GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_1, false);
            GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_2, GPIO_PIN_2);
            break;
    }
}

/*
 * Function to blink the User LED 0 (PN0) when
 * a key is pressed
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void KeyPad_BlinkLED(void)
{
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
    vTaskDelay(500);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, false);
}

/*
 * Function to scan the columns and if any key is pressed,
 * store the ASCII representation of that key in the array
 * that will be sent to the Control Node. This function also
 * supports timeout - so if no key is pressed withing given time,
 * it will raise an error flag and exit polling
 *
 * Param: uint8_t row (from 0 to 3)
 *
 * Return: boolean state (false: No key was pressed, true: key pressed)
 *
 */
bool KeyPad_ScanCode(uint8_t row)
{
    static char tp[50];
    static uint32_t stat;

    stat = 0;
    stat = GPIOPinRead(GPIO_PORTH_BASE, GPIO_PIN_0) & 0x00FF;
    if(stat != 0)
    {
        KeyPad_Counter = 0;
        EP_Tx.KeyPad_Code[KeyPad_Pos] = Key_LookUp[row][3];
        #if KeyPad_DEBUG_PRINTF
            snprintf(tp, sizeof(tp), "\nPressed Key is: %c Stored at %d", EP_Tx.KeyPad_Code[KeyPad_Pos], KeyPad_Pos);
            KeyPad_Print(tp);
        #endif
        KeyPad_Pos += 1;
        KeyPad_BlinkLED();
        vTaskDelay(KeyPad_Wait_Timems);
        return true;
    }

    stat = 0;
    stat = GPIOPinRead(GPIO_PORTH_BASE, GPIO_PIN_1) & 0x00FF;
    if(stat != 0)
    {
        KeyPad_Counter = 0;
        EP_Tx.KeyPad_Code[KeyPad_Pos] = Key_LookUp[row][2];
        #if KeyPad_DEBUG_PRINTF
            snprintf(tp, sizeof(tp), "\nPressed Key is: %c Stored at %d", EP_Tx.KeyPad_Code[KeyPad_Pos], KeyPad_Pos);
            KeyPad_Print(tp);
        #endif
        KeyPad_Pos += 1;
        KeyPad_BlinkLED();
        vTaskDelay(KeyPad_Wait_Timems);
        return true;
    }

    stat = 0;
    stat = GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_6) & 0x00FF;
    if(stat != 0)
    {
        KeyPad_Counter = 0;
        EP_Tx.KeyPad_Code[KeyPad_Pos] = Key_LookUp[row][1];
        #if KeyPad_DEBUG_PRINTF
            snprintf(tp, sizeof(tp), "\nPressed Key is: %c Stored at %d", EP_Tx.KeyPad_Code[KeyPad_Pos], KeyPad_Pos);
            KeyPad_Print(tp);
        #endif
        KeyPad_Pos += 1;
        KeyPad_BlinkLED();
        vTaskDelay(KeyPad_Wait_Timems);
        return true;
    }

    stat = 0;
    stat = GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_7) & 0x00FF;
    if(stat != 0)
    {
        KeyPad_Counter = 0;
        EP_Tx.KeyPad_Code[KeyPad_Pos] = Key_LookUp[row][0];
        #if KeyPad_DEBUG_PRINTF
            snprintf(tp, sizeof(tp), "\nPressed Key is: %c Stored at %d", EP_Tx.KeyPad_Code[KeyPad_Pos], KeyPad_Pos);
            KeyPad_Print(tp);
        #endif
        KeyPad_Pos += 1;
        KeyPad_BlinkLED();
        vTaskDelay(KeyPad_Wait_Timems);
        return true;
    }

    vTaskDelay(KeyPad_Polling_Timems);
    KeyPad_Counter += 1;

    return false;
}




