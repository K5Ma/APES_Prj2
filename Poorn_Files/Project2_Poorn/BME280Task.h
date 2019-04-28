/*
 * BME280Task.h
 *
 *  Created on: Apr 15, 2019
 *  Last Update: Apr 21, 2019
 *      Author: Poorn Mehta
 *
 *  Driver for reading Atmospheric Parameters (such as Temperature, Humidity,
 *  Barometric Pressure) from BME280 sensor which is connected to
 *  TM4C1294XL (TIVA Development Board) using SPI Bus
 *
 *  This driver code is developed based on provided documentation from the manufacturer
 *
 */

#ifndef BME280TASK_H_
#define BME280TASK_H_

//#include "main.h"

#define BME280_Infinite             true
#define BME280_Limited              false

#define BME280_Retry_Mode           BME280_Infinite

#if     (BME280_Retry_Mode == BME280_Limited)
#define BME280_Max_Retries          5
#endif

#define BME280_Polling_Timems       2500

#define BME280_Host_Unknown           false

#if     BME280_Host_Unknown
    #define BME280_Print   UARTprintf
#else
    #define BME280_Print   cust_print
#endif

#define BME280_INDIVIDUAL_TESTING  false

#define BME280_DEBUG_PRINTF        true

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
#include "inc/hw_ssi.h"

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
#include "driverlib/ssi.h"

#include "sensorlib/i2cm_drv.h"

#define BME280_Temp_High_Threshold      30
#define BME280_Temp_Low_Threshold       20

#define BME280_Humidity_High_Threshold      50

#define BME280_DevID        0x60

#define BME280_DataRegNos           8

#define BME280_Pressure_MSB_Pos     0
#define BME280_Pressure_LSB_Pos     1
#define BME280_Pressure_XLSB_Pos    2

#define BME280_Temp_MSB_Pos     3
#define BME280_Temp_LSB_Pos     4
#define BME280_Temp_XLSB_Pos    5

#define BME280_Hum_MSB_Pos     6
#define BME280_Hum_LSB_Pos     7

#define BME280_MODE_Pos            0
#define BME280_MODE_MASK           0x03
#define BME280_MODE_SLEEP          0x00
#define BME280_MODE_FORCED         0x01
#define BME280_MODE_NORMAL         0x03

#define BME280_Standby_Pos         5
#define BME280_Standby_Mask        0xE0
#define BME280_Standby_500us       0x00
#define BME280_Standby_62500us     0x01
#define BME280_Standby_125ms       0x02
#define BME280_Standby_250ms       0x03
#define BME280_Standby_500ms       0x04
#define BME280_Standby_1000ms      0x05
#define BME280_Standby_10ms        0x06
#define BME280_Standby_20ms        0x07

#define BME280_Hum_OVS_Pos         0
#define BME280_Hum_OVS_Mask        0x07
#define BME280_Hum_OVS_Skip        0x00
#define BME280_Hum_OVS1            0x01
#define BME280_Hum_OVS2            0x02
#define BME280_Hum_OVS4            0x03
#define BME280_Hum_OVS8            0x04
#define BME280_Hum_OVS16           0x05

#define BME280_Temp_OVS_Pos         5
#define BME280_Temp_OVS_Mask        0xE0
#define BME280_Temp_OVS_Skip        0x00
#define BME280_Temp_OVS1            0x01
#define BME280_Temp_OVS2            0x02
#define BME280_Temp_OVS4            0x03
#define BME280_Temp_OVS8            0x04
#define BME280_Temp_OVS16           0x05

#define BME280_Pressure_OVS_Pos         2
#define BME280_Pressure_OVS_Mask        0x1C
#define BME280_Pressure_OVS_Skip        0x00
#define BME280_Pressure_OVS1            0x01
#define BME280_Pressure_OVS2            0x02
#define BME280_Pressure_OVS4            0x03
#define BME280_Pressure_OVS8            0x04
#define BME280_Pressure_OVS16           0x05

//Register names:
#define BME280_DIG_T1_LSB_REG           0x88
#define BME280_DIG_T1_MSB_REG           0x89
#define BME280_DIG_T2_LSB_REG           0x8A
#define BME280_DIG_T2_MSB_REG           0x8B
#define BME280_DIG_T3_LSB_REG           0x8C
#define BME280_DIG_T3_MSB_REG           0x8D
#define BME280_DIG_P1_LSB_REG           0x8E
#define BME280_DIG_P1_MSB_REG           0x8F
#define BME280_DIG_P2_LSB_REG           0x90
#define BME280_DIG_P2_MSB_REG           0x91
#define BME280_DIG_P3_LSB_REG           0x92
#define BME280_DIG_P3_MSB_REG           0x93
#define BME280_DIG_P4_LSB_REG           0x94
#define BME280_DIG_P4_MSB_REG           0x95
#define BME280_DIG_P5_LSB_REG           0x96
#define BME280_DIG_P5_MSB_REG           0x97
#define BME280_DIG_P6_LSB_REG           0x98
#define BME280_DIG_P6_MSB_REG           0x99
#define BME280_DIG_P7_LSB_REG           0x9A
#define BME280_DIG_P7_MSB_REG           0x9B
#define BME280_DIG_P8_LSB_REG           0x9C
#define BME280_DIG_P8_MSB_REG           0x9D
#define BME280_DIG_P9_LSB_REG           0x9E
#define BME280_DIG_P9_MSB_REG           0x9F
#define BME280_DIG_H1_REG               0xA1
#define BME280_CHIP_ID_REG              0xD0 //Chip ID
#define BME280_RST_REG                  0xE0 //Softreset Reg
#define BME280_DIG_H2_LSB_REG           0xE1
#define BME280_DIG_H2_MSB_REG           0xE2
#define BME280_DIG_H3_REG               0xE3
#define BME280_DIG_H4_MSB_REG           0xE4
#define BME280_DIG_H4_LSB_REG           0xE5
#define BME280_DIG_H5_MSB_REG           0xE6
#define BME280_DIG_H6_REG               0xE7
#define BME280_CTRL_HUMIDITY_REG        0xF2 //Ctrl Humidity Reg
#define BME280_STAT_REG                 0xF3 //Status Reg
#define BME280_CTRL_MEAS_REG            0xF4 //Ctrl Measure Reg
#define BME280_CONFIG_REG               0xF5 //Configuration Reg
#define BME280_PRESSURE_MSB_REG         0xF7 //Pressure MSB
#define BME280_PRESSURE_LSB_REG         0xF8 //Pressure LSB
#define BME280_PRESSURE_XLSB_REG        0xF9 //Pressure XLSB
#define BME280_TEMPERATURE_MSB_REG      0xFA //Temperature MSB
#define BME280_TEMPERATURE_LSB_REG      0xFB //Temperature LSB
#define BME280_TEMPERATURE_XLSB_REG     0xFC //Temperature XLSB
#define BME280_HUMIDITY_MSB_REG         0xFD //Humidity MSB
#define BME280_HUMIDITY_LSB_REG         0xFE //Humidity LSB

void BME280_SPISetup(void);
uint8_t BME280_SPIReadReg(uint8_t spi_addr);
void BME280_SPIReadMultiReg(uint32_t *rx_addr, uint8_t start_addr, uint8_t no_of_regs);
void BME280_SPIWriteReg(uint8_t spi_addr, uint8_t spi_data);
void BME280_SetStandBy(uint8_t timeout);
uint8_t BME280_GetStandBy(void);
void BME280_SetMode(uint8_t mode);
uint8_t BME280_GetMode(void);
void BME280_SetHumOVS(uint8_t ovs);
void BME280_SetTempOVS(uint8_t ovs);
void BME280_SetPressureOVS(uint8_t ovs);
void BME280_ReadAllCalib(void);
void BME280_ReadAllDataReg(void);
void BME280_GetPressure(float *var_p);
void BME280_GetTemp(float *var_t);
void BME280_GetHum(float *var_h);
bool BME280_SensorSetup(void);
void BME280_TestSensor(void);
void BME280Task(void *pvParameters);



#endif /* BME280TASK_H_ */
