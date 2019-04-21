/* FreeRTOS 8.2 Tiva Demo
 *
 * main.c
 *
 * Base project Credits: Andy Kobyljanec
 * I2C driver is based on sources provided by TI
 *
 * This is a simple demonstration project of FreeRTOS 8.2 on the Tiva Launchpad
 * EK-TM4C1294XL.  TivaWare driverlib sourcecode is included.
 */

#include "main.h"

#include <stdint.h>
#include <stdbool.h>
#include "drivers/pinout.h"
#include "utils/uartstdio.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
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
//#include "driverlib/map.h"

#include "sensorlib/i2cm_drv.h"
#include "driverlib/i2c.h"
#include "inc/hw_i2c.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "Temp_Task.h"
#include "LED_Task.h"
#include "Log_Task.h"
#include "alert_task.h"
#include "EPaperTask.h"
#include "LoadCellTask.h"
#include "BME280Task.h"
#include "NFCTask.h"
#include "ServoTask.h"
#include "SpeakJetTask.h"
#include "OutputIndicatorsTask.h"

QueueHandle_t log_queue;

TaskHandle_t alert_task_handle, temp_task;

uint8_t i2c_data[2];

uint8_t led_state = false;

uint32_t fun_resp = 0;

char msg[50];

void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    while(ui32Count--)
    {
        UARTCharPut(UART0_BASE, *pui8Buffer++);
    }
}

void cust_print(char *txt)
{
    UARTSend((uint8_t *)txt, strlen(txt));
}

// Main function
int main(void)
{
    // Initialize system clock to 120 MHz
    uint32_t output_clock_rate_hz;
    output_clock_rate_hz = SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               SYSTEM_CLOCK);
    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);


    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);

    // Set up the UART which is connected to the virtual COM port
    UARTStdioConfig(0, 115200, SYSTEM_CLOCK);

    cust_print("\nProgram Start");

//    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_6);
//    GPIOPinTypeGPIOOutput(GPIO_PORTQ_BASE, GPIO_PIN_1);

//    tmp102_i2c_init();

    // Stack sizes are manually fine tuned

//    if(xTaskCreate(Log_Task, (const portCHAR *)"Log",
//                400, 0, 1, NULL) != pdTRUE)
//    {
//        cust_print("\nLOG Task Creation Failed");
//    }
//    else        cust_print("\nLOG Task Created");
//
//    if(xTaskCreate(LED_Task, (const portCHAR *)"LEDs",
//                250, 0, 1, NULL) != pdTRUE)
//    {
//        cust_print("\nLED Task Creation Failed");
//    }
//    else        cust_print("\nLED Task Created");
//
//    if(xTaskCreate(Alert_Task, (const portCHAR *)"Alert",
//               300, 0, 1, NULL) != pdTRUE)
//    {
//        cust_print("\nAlert Task Creation Failed");
//    }
//    else        cust_print("\nAlert Task Created");
//
//    if(xTaskCreate(Temp_Task, (const portCHAR *)"Temp",
//               300, 0, 1, &temp_task) != pdTRUE)
//    {
//        cust_print("\nTEMP Task Creation Failed");
//    }
//    else        cust_print("\nTEMP Task Created");

//    if(xTaskCreate(EPaperTask, (const portCHAR *)"EPaper",
//               500, 0, 1, 0) != pdTRUE)
//    {
//        cust_print("\nEPaper Task Creation Failed");
//    }
//    else        cust_print("\nEPaper Task Created");

//    if(xTaskCreate(LoadCellTask, (const portCHAR *)"LoadCell",
//               500, 0, 1, 0) != pdTRUE)
//    {
//        cust_print("\nLoadCell Task Creation Failed");
//    }
//    else        cust_print("\nLoadCell Task Created");

    if(xTaskCreate(bme280_all, (const portCHAR *)"BME280",
               500, 0, 1, 0) != pdTRUE)
    {
        cust_print("\nBME280 Task Creation Failed");
    }
    else        cust_print("\nBME280 Task Created");

//    if(xTaskCreate(nfc_all, (const portCHAR *)"NFC",
//               500, 0, 1, 0) != pdTRUE)
//    {
//        cust_print("\nNFC Task Creation Failed");
//    }
//    else        cust_print("\nNFC Task Created");

//    if(xTaskCreate(ServoTask, (const portCHAR *)"Servo",
//               500, 0, 1, 0) != pdTRUE)
//    {
//        cust_print("\nServo Task Creation Failed");
//    }
//    else        cust_print("\nServo Task Created");

//    if(xTaskCreate(OutputIndicatorsTask, (const portCHAR *)"OutputIndicators",
//               500, 0, 1, 0) != pdTRUE)
//    {
//        cust_print("\nOutputIndicators Task Creation Failed");
//    }
//    else        cust_print("\nOutputIndicators Task Created");

//    if(xTaskCreate(SpeakJetTask, (const portCHAR *)"SpeakJet",
//               500, 0, 1, 0) != pdTRUE)
//    {
//        cust_print("\nSpeakJet Task Creation Failed");
//    }
//    else        cust_print("\nSpeakJet Task Created");


    vTaskStartScheduler();
    return 0;
}


/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {
    }
}
