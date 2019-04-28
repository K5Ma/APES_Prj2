/*
 * LuxTask.h
 *
 *  Created on: Apr 23, 2019
 *  Last Update: Apr 23, 2019
 *      Author: Poorn Mehta
 *
 *  Driver for Interfacing with Lux Sensor (APDS-9301) which is
 *  connected to TM4C1294XL (TIVA Development Board) using I2C Bus
 *
 *  This driver code is developed based on provided documentation from the manufacturer
 *
 */

#ifndef LUXTASK_H_
#define LUXTASK_H_

#define Lux_Infinite             true
#define Lux_Limited              false

#define Lux_Retry_Mode           Lux_Infinite

#if     (Lux_Retry_Mode == Lux_Limited)
    #define Lux_Max_Retries          5
#endif

#define Lux_Polling_Timems       500
#define Lux_Timeoutms            2000

#define Lux_Host_Unknown           false

#if     Lux_Host_Unknown
    #define Lux_Print   UARTprintf
#else
    #define Lux_Print   cust_print
#endif

#define Lux_INDIVIDUAL_TESTING  true

#define Lux_DEBUG_PRINTF        true


// Standard Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "drivers/pinout.h"
#include "utils/uartstdio.h"

// Third Party Includes
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_i2c.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "inc/hw_adc.h"
#include "inc/hw_pwm.h"

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/interrupt.h"
#include "driverlib/i2c.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/fpu.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"

#include "sensorlib/i2cm_drv.h"

// System clock rate, 120 MHz
#define SYSTEM_CLOCK    120000000U

#define I2C_100KHZ_Mode         false
#define I2C_400KHZ_Mode         true

#define I2C_Write_Mode          false
#define I2C_Read_Mode           true

#define Lux_Address    0x39

// Internal register addresses
#define Lux_Control_Reg                 0x00
#define Lux_Timing_Reg                  0x01
#define Lux_ThrLow_Low_Reg          0x02
#define Lux_ThrLow_High_Reg         0x03
#define Lux_ThrHigh_Low_Reg         0x04
#define Lux_ThrHigh_High_Reg        0x05
#define Lux_Intrp_Ctrl_Reg          0x06
#define Lux_ID_Reg                          0x0A
#define Lux_Data0_Low                       0x0C
#define Lux_Data0_High                  0x0D
#define Lux_Data1_Low                       0x0E
#define Lux_Data1_High                  0x0F

// Reserved internal register addresses
#define Lux_Res_Reg1                        0x07
#define Lux_Res_Reg2                        0x08
#define Lux_Res_Reg3                        0x09
#define Lux_Res_Reg4                        0x0B

// ALL Macros returs 0 on success and 1 on failure

// Defines and macros for gain and integration time settings
#define Lux_Integration_Pos         0
#define Lux_Integration_Mask        (3 << Lux_Integration_Pos)
#define Lux_Low_Integration_Time        (0 << Lux_Integration_Pos)      // 13.7ms
#define Lux_Test_Low_Int_Time(x)        ((x & Lux_Integration_Mask) >> Lux_Integration_Pos) == Lux_Low_Integration_Time ? 0 : 1
#define Lux_Med_Integration_Time        (1 << Lux_Integration_Pos)      // 101ms
#define Lux_Test_Med_Int_Time(x)        ((x & Lux_Integration_Mask) >> Lux_Integration_Pos) == Lux_Med_Integration_Time ? 0 : 1
#define Lux_High_Integration_Time       (2 << Lux_Integration_Pos)      // 402ms
#define Lux_Test_High_Int_Time(x)       ((x & Lux_Integration_Mask) >> Lux_Integration_Pos) == Lux_High_Integration_Time ? 0 : 1

#define Lux_Gain_Pos                    4
#define Lux_Gain_Mask                   (1 << Lux_Gain_Pos)
#define Lux_Set_Gain_Low(x)             x & ~(Lux_Gain_Mask)
#define Lux_Test_Gain_Low(x)            ((x &   Lux_Gain_Mask) >> Lux_Gain_Pos) == 0 ? 0 : 1
#define Lux_Set_Gain_High(x)                x | Lux_Gain_Mask
#define Lux_Test_Gain_High(x)           ((x &   Lux_Gain_Mask) >> Lux_Gain_Pos) == 1 ? 0 : 1

// Defines and macros for interrupt controller manipulation
#define Lux_Interrupt_Test_Data         0x1F        // Turning on Level Interrupt as well as checking N config (from datasheet)
#define Lux_Interrupt_Control_Mask      0x1F
#define Lux_Test_Intrp_Ctrl_Data(x)     (x & Lux_Interrupt_Control_Mask) == Lux_Interrupt_Test_Data ? 0 : 1

// Defines and macros for interrupt threshold registers manipulation
#define Lux_ThrLow_Low_Test_Data            0x05
#define Lux_ThrLow_High_Test_Data           0x0A
#define Lux_ThrHigh_Low_Test_Data           0x0F
#define Lux_ThrHigh_High_Test_Data      0xF0
#define Lux_Test_ThrLow_Low(x)              x == Lux_ThrLow_Low_Test_Data ? 0 : 1
#define Lux_Test_ThrLow_High(x)             x == Lux_ThrLow_High_Test_Data ? 0 : 1
#define Lux_Test_ThrHigh_Low(x)             x == Lux_ThrHigh_Low_Test_Data ? 0 : 1
#define Lux_Test_ThrHigh_High(x)                x == Lux_ThrHigh_High_Test_Data ? 0 : 1

// Defines and macros for verifying ID register of the sensor
#define Lux_Part_No             5               // Fixed value, from datasheet
#define Lux_Part_No_Pos     4
#define Lux_Part_No_Mask    (Lux_Part_No << Lux_Part_No_Pos)
#define Lux_Test_Part_No(x)     ((x & Lux_Part_No_Mask) >> Lux_Part_No_Pos) == Lux_Part_No ? 0 : 1

// Defines and macros for using command register with high flexibility
#define Lux_Command_Bit_Pos         7
#define Lux_Command_Reg_Mask        (1 << Lux_Command_Bit_Pos)
#define Lux_Command_Address_Mask        (0x0F)
#define Lux_Command_Reg_Data(x)     ((x & Lux_Command_Address_Mask) | Lux_Command_Reg_Mask)
#define Lux_Word_Mode_Bit_Pos       5
#define Lux_Word_Mode_Mask      Lux_Command_Reg_Mask | (1 << Lux_Word_Mode_Bit_Pos)
#define Lux_Command_Word_Data(x)        ((x & Lux_Command_Address_Mask) | Lux_Word_Mode_Mask)

// Macros and defines for turning ON the power to the sensor
#define Lux_Control_Power_ON        0x03
#define Lux_Control_Pos                 0
#define Lux_Control_Mask                ((Lux_Control_Power_ON) << Lux_Control_Pos)
#define Lux_Test_Control_Power(x)       (x & Lux_Control_Mask) == Lux_Control_Power_ON ? 0 : 1

#define Lux_High_Threshold_Level        10

void Lux_I2C_busy_wait_blocking(void);
void Lux_I2C_pin_config(void);
void Lux_I2C_perph_config(void);
void Lux_I2C_Reg_Access(uint8_t reg_addr);
void Lux_I2C_read(uint8_t *target_variable, uint8_t no_of_bytes);
void Lux_I2C_init(void);
void Lux_Test_Sensor(void);
void Lux_I2C_write(uint8_t r_addr, uint8_t r_val);
void Lux_read(void);
void LuxTask(void *pvParameters);

#endif /* LUXTASK_H_ */
