/*
 * KeyPadTask.c
 *
 *  Created on: Apr 22, 2019
 *  Last Update: Apr 23, 2019
 *      Author: Poorn Mehta
 *
 *  Driver for Interfacing with 4x4 Keypad which is connected to
 *  TM4C1294XL (TIVA Development Board) using GPIO pins
 *
 *  This driver code is developed based on provided documentation from the manufacturer
 *
 */

/*
 *
 * Hardware Connections
 *
 *
 * Note: Left Side - Module Pins & Right Side - Controller Pins
 *
 * ROW0(bottom - from left, first black connection) - PK5 (Output)
 * ROW1(bottom - from left, second black connection) - PM0 (Output)
 * ROW2(bottom - from left, third black connection) - PM1 (Output)
 * ROW3(bottom - from left, fourth black connection) - PM2 (Output)
 * COL0(bottom - from left, first white connection) - PK7 (Input with external pull down of 250k)
 * COL1(bottom - from left, second white connection) - PK6 (Input with external pull down of 250k)
 * COL2(bottom - from left, third white connection) - PH1 (Input with external pull down of 250k)
 * COL3(bottom - from left, fourth white connection) - PH0 (Input with external pull down of 250k)
 *
 */

#include "KeyPadTask.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Variables that will be shared between functions
uint8_t KeyPad_Code[KeyPad_Code_Size], KeyPad_Pos;
uint16_t KeyPad_Counter;
bool KeyPad_Error, KeyPad_Poll;

const char Key_LookUp[KeyPad_Rows][KeyPad_Cols] = {
                               {'1', '2', '3', 'A'},
                               {'4', '5', '6', 'B'},
                               {'7', '8', '9', 'C'},
                               {'*', '0', '#', 'D'}
};

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
        KeyPad_Code[KeyPad_Pos] = Key_LookUp[row][3];
        #if KeyPad_DEBUG_PRINTF
            snprintf(tp, sizeof(tp), "\nPressed Key is: %c Stored at %d", KeyPad_Code[KeyPad_Pos], KeyPad_Pos);
            KeyPad_Print(tp);
        #endif
        KeyPad_Pos += 1;
        KeyPad_BlinkLED();
        return true;
    }

    stat = 0;
    stat = GPIOPinRead(GPIO_PORTH_BASE, GPIO_PIN_1) & 0x00FF;
    if(stat != 0)
    {
        KeyPad_Counter = 0;
        KeyPad_Code[KeyPad_Pos] = Key_LookUp[row][2];
        #if KeyPad_DEBUG_PRINTF
            snprintf(tp, sizeof(tp), "\nPressed Key is: %c Stored at %d", KeyPad_Code[KeyPad_Pos], KeyPad_Pos);
            KeyPad_Print(tp);
        #endif
        KeyPad_Pos += 1;
        KeyPad_BlinkLED();
        return true;
    }

    stat = 0;
    stat = GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_6) & 0x00FF;
    if(stat != 0)
    {
        KeyPad_Counter = 0;
        KeyPad_Code[KeyPad_Pos] = Key_LookUp[row][1];
        #if KeyPad_DEBUG_PRINTF
            snprintf(tp, sizeof(tp), "\nPressed Key is: %c Stored at %d", KeyPad_Code[KeyPad_Pos], KeyPad_Pos);
            KeyPad_Print(tp);
        #endif
        KeyPad_Pos += 1;
        KeyPad_BlinkLED();
        return true;
    }

    stat = 0;
    stat = GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_7) & 0x00FF;
    if(stat != 0)
    {
        KeyPad_Counter = 0;
        KeyPad_Code[KeyPad_Pos] = Key_LookUp[row][0];
        #if KeyPad_DEBUG_PRINTF
            snprintf(tp, sizeof(tp), "\nPressed Key is: %c Stored at %d", KeyPad_Code[KeyPad_Pos], KeyPad_Pos);
            KeyPad_Print(tp);
        #endif
        KeyPad_Pos += 1;
        KeyPad_BlinkLED();
        return true;
    }

    vTaskDelay(KeyPad_Polling_Timems / 4);
    KeyPad_Counter += 1;

    return false;
}

/*
 *
 * Callback Function for KeyPad Task
 *
 * Return: Null
 *
 */

/*
 * Normal Operation (Parameters and Returns indicate communication with Central Task)
 *
 * This Task requires 1 parameter from Central Task through IPC
 *
 * This Task will return 1 array of 6 bytes to the Central Task through IPC
 * It will also report back the success or failure of keypad scanning operation
 *
 * Param_1: bool KeyPad_Poll
 *          (false: do nothing, true: start polling the keypad)
 *
 * Return_1: uint8_t KeyPad_Code[4]
 *          (each element represent pressed key, it will be Null if KeyPad timed out)
 *
 * Return_2: bool NFC_Error
 *           (false: Online, true: OFfline - error present)
 *
 */

void KeyPadTask(void *pvParameters)
{
    static uint8_t i;
    static char tp[50];

    KeyPad_Init();
    #if KeyPad_DEBUG_PRINTF
        KeyPad_Print("\nKeyPad Initialized");
    #endif

    #if     KeyPad_INDIVIDUAL_TESTING
        KeyPad_Poll = true;
    #endif
    while(1)
    {
        if(KeyPad_Poll == true)
        {
            #if     (KeyPad_INDIVIDUAL_TESTING == false)
                KeyPad_Poll = false;
            #endif
            KeyPad_Counter = 0;
            KeyPad_Pos = 0;

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
                    KeyPad_Poll = false;
                    #if KeyPad_DEBUG_PRINTF
                        snprintf(tp, sizeof(tp), "\nEntered Code: %c%c%c%c%c%c", KeyPad_Code[0], KeyPad_Code[1],
                                 KeyPad_Code[2], KeyPad_Code[3], KeyPad_Code[4], KeyPad_Code[5]);
                        KeyPad_Print(tp);
                    #endif
                    break;
                }
                else if(KeyPad_Counter >= (KeyPad_Timeoutms / KeyPad_Polling_Timems))
                {
                    KeyPad_Error = true;
                    #if KeyPad_DEBUG_PRINTF
                        KeyPad_Print("\nTimed Out");
                    #endif
                    break;
                }
            }
        }
    }
}


