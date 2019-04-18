/*
 * BME280Task.c
 *
 *  Created on: Apr 15, 2019
 *      Author: poorn
 *
 *  This code has been written while taking Sparkfun Library as a reference
 */

#include "BME280Task.h"

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
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
#include "inc/hw_ssi.h"

#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/fpu.h"
#include "driverlib/ssi.h"

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


uint32_t BME280_Tx[2];
uint32_t BME280_Rx;
uint32_t BME280_index;

typedef struct
{
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;

    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;

    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
}SensorCalibration;

SensorCalibration   bme280_calib;

int32_t temp_fine;

uint8_t spi_readreg(uint8_t spi_addr)
{
    BME280_Tx[0] = spi_addr | (1 << 7);
    BME280_Tx[1] = 0x00;

    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, false);

    SSIDataPut(SSI0_BASE, BME280_Tx[0]);
    while(SSIBusy(SSI0_BASE));
    SSIDataPut(SSI0_BASE, BME280_Tx[1]);
    while(SSIBusy(SSI0_BASE));

    SSIDataGet(SSI0_BASE, &BME280_Rx);
    SSIDataGet(SSI0_BASE, &BME280_Rx);

    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, GPIO_PIN_6);

    return (BME280_Rx & 0x00FF);
}

void spi_read_multireg(uint32_t *rx_addr, uint8_t start_addr, uint8_t no_of_regs)
{
    static uint8_t i;

    BME280_Tx[0] = start_addr | (1 << 7);
    BME280_Tx[1] = 0x00;

    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, false);

    SSIDataPut(SSI0_BASE, BME280_Tx[0]);
    while(SSIBusy(SSI0_BASE));
    SSIDataGet(SSI0_BASE, rx_addr);

    for(i = 0; i < no_of_regs; i ++)
    {
        SSIDataPut(SSI0_BASE, BME280_Tx[1]);
        while(SSIBusy(SSI0_BASE));
        SSIDataGet(SSI0_BASE, rx_addr++);
    }

    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, GPIO_PIN_6);
}

void spi_writereg(uint8_t spi_addr, uint8_t spi_data)
{
    BME280_Tx[0] = spi_addr & ~(1 << 7);
    BME280_Tx[1] = spi_data;

    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, false);

    SSIDataPut(SSI0_BASE, BME280_Tx[0]);
    while(SSIBusy(SSI0_BASE));
    SSIDataPut(SSI0_BASE, BME280_Tx[1]);
    while(SSIBusy(SSI0_BASE));

    SSIDataGet(SSI0_BASE, &BME280_Rx);
    SSIDataGet(SSI0_BASE, &BME280_Rx);

    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, GPIO_PIN_6);
}

void bme280_setmode(uint8_t mode)
{
    static uint8_t ctrl_data;
    static char tp[50];
    ctrl_data = spi_readreg(BME280_CTRL_MEAS_REG);
    snprintf(tp, sizeof(tp), "\nOld Control Data: %x", ctrl_data);
    cust_print(tp);
    ctrl_data &= ~(0x03);
    ctrl_data |= mode;
    spi_writereg(BME280_CTRL_MEAS_REG, ctrl_data);
}

void bme280_getpressure(float *var_p)
{
    static uint32_t sensor_data[3];
    static int32_t adc_pressure;
    static int64_t var1, var2, pressure_fine;

    spi_read_multireg(&sensor_data[0], BME280_PRESSURE_MSB_REG, 3);
    adc_pressure = ((uint32_t)sensor_data[0] << 12) | ((uint32_t)sensor_data[1] << 4) | ((sensor_data[2] >> 4) & 0x0F);

    var1 = ((int64_t)temp_fine) - 128000;
    var2 = var1 * var1 * (int64_t)bme280_calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)bme280_calib.dig_P5)<<17);
    var2 = var2 + (((int64_t)bme280_calib.dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)bme280_calib.dig_P3)>>8) + ((var1 * (int64_t)bme280_calib.dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)bme280_calib.dig_P1)>>33;
    if (var1 == 0)
    {
        *var_p = -1;
    }
    else
    {
        pressure_fine = 1048576 - adc_pressure;
        pressure_fine = (((pressure_fine<<31) - var2)*3125)/var1;
        var1 = (((int64_t)bme280_calib.dig_P9) * (pressure_fine>>13) * (pressure_fine>>13)) >> 25;
        var2 = (((int64_t)bme280_calib.dig_P8) * pressure_fine) >> 19;
        pressure_fine = ((pressure_fine + var1 + var2) >> 8) + (((int64_t)bme280_calib.dig_P7)<<4);

        *var_p = (float)(pressure_fine / 256.0);
    }
}

void bme280_gettemp(float *var_t)
{
    static uint32_t sensor_data[3];
    static int32_t adc_temp;
    static int64_t var1, var2;

    spi_read_multireg(&sensor_data[0], BME280_TEMPERATURE_MSB_REG, 3);

    adc_temp = ((uint32_t)sensor_data[0] << 12) | ((uint32_t)sensor_data[1] << 4) | ((sensor_data[2] >> 4) & 0x0F);

    //By datasheet, calibrate

    var1 = ((((adc_temp>>3) - ((int32_t)bme280_calib.dig_T1<<1))) * ((int32_t)bme280_calib.dig_T2)) >> 11;
    var2 = (((((adc_temp>>4) - ((int32_t)bme280_calib.dig_T1)) * ((adc_temp>>4) - ((int32_t)bme280_calib.dig_T1))) >> 12) * ((int32_t)bme280_calib.dig_T3)) >> 14;
    temp_fine = var1 + var2;
    *var_t = (temp_fine * 5 + 128) >> 8;
    *var_t /= 100;
}

void bme280_gethum(float *var_h)
{
    static uint32_t sensor_data[3];
    static int32_t adc_hum;
    static int64_t var1, var2;

    spi_read_multireg(&sensor_data[0], BME280_HUMIDITY_MSB_REG, 2);
    adc_hum = ((uint32_t)sensor_data[0] << 8) | ((uint32_t)sensor_data[1]);

    var1 = (temp_fine - ((int32_t)76800));
    var1 = (((((adc_hum << 14) - (((int32_t)bme280_calib.dig_H4) << 20) - (((int32_t)bme280_calib.dig_H5) * var1)) +
    ((int32_t)16384)) >> 15) * (((((((var1 * ((int32_t)bme280_calib.dig_H6)) >> 10) * (((var1 * ((int32_t)bme280_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
    ((int32_t)bme280_calib.dig_H2) + 8192) >> 14));
    var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)bme280_calib.dig_H1)) >> 4));
    var1 = (var1 < 0 ? 0 : var1);
    var1 = (var1 > 419430400 ? 419430400 : var1);

    *var_h = (float)((var1>>12) / 1024.0);
}

void bme280_all(void *pvParameters)
{
    static char tp[50];

    cust_print("\nSPI Setup Start");

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_6);
    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, GPIO_PIN_6);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0XDAT0);
    GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);

    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 |
                   GPIO_PIN_2);

    SSIConfigSetExpClk(SSI0_BASE, SYSTEM_CLOCK, SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 1000000, 8); // orig SSI_FRF_MOTO_MODE_0 SSI_FRF_MOTO_MODE_3

    SSIEnable(SSI0_BASE);

    while(SSIDataGetNonBlocking(SSI0_BASE, &BME280_Rx));

    cust_print("\nSPI Setup Done");

    vTaskDelay(3000);

    if(spi_readreg(BME280_CHIP_ID_REG) == BME280_DevID)     cust_print("\nBME280 Device ID Verified");
    else        cust_print("\nBME280 Device ID Verification Failed");

    //Reading all compensation data, range 0x88:A1, 0xE1:E7
    bme280_calib.dig_T1 = ((uint16_t)((spi_readreg(BME280_DIG_T1_MSB_REG) << 8) + spi_readreg(BME280_DIG_T1_LSB_REG)));
    bme280_calib.dig_T2 = ((int16_t)((spi_readreg(BME280_DIG_T2_MSB_REG) << 8) + spi_readreg(BME280_DIG_T2_LSB_REG)));
    bme280_calib.dig_T3 = ((int16_t)((spi_readreg(BME280_DIG_T3_MSB_REG) << 8) + spi_readreg(BME280_DIG_T3_LSB_REG)));

    bme280_calib.dig_P1 = ((uint16_t)((spi_readreg(BME280_DIG_P1_MSB_REG) << 8) + spi_readreg(BME280_DIG_P1_LSB_REG)));
    bme280_calib.dig_P2 = ((int16_t)((spi_readreg(BME280_DIG_P2_MSB_REG) << 8) + spi_readreg(BME280_DIG_P2_LSB_REG)));
    bme280_calib.dig_P3 = ((int16_t)((spi_readreg(BME280_DIG_P3_MSB_REG) << 8) + spi_readreg(BME280_DIG_P3_LSB_REG)));
    bme280_calib.dig_P4 = ((int16_t)((spi_readreg(BME280_DIG_P4_MSB_REG) << 8) + spi_readreg(BME280_DIG_P4_LSB_REG)));
    bme280_calib.dig_P5 = ((int16_t)((spi_readreg(BME280_DIG_P5_MSB_REG) << 8) + spi_readreg(BME280_DIG_P5_LSB_REG)));
    bme280_calib.dig_P6 = ((int16_t)((spi_readreg(BME280_DIG_P6_MSB_REG) << 8) + spi_readreg(BME280_DIG_P6_LSB_REG)));
    bme280_calib.dig_P7 = ((int16_t)((spi_readreg(BME280_DIG_P7_MSB_REG) << 8) + spi_readreg(BME280_DIG_P7_LSB_REG)));
    bme280_calib.dig_P8 = ((int16_t)((spi_readreg(BME280_DIG_P8_MSB_REG) << 8) + spi_readreg(BME280_DIG_P8_LSB_REG)));
    bme280_calib.dig_P9 = ((int16_t)((spi_readreg(BME280_DIG_P9_MSB_REG) << 8) + spi_readreg(BME280_DIG_P9_LSB_REG)));

    bme280_calib.dig_H1 = ((uint8_t)(spi_readreg(BME280_DIG_H1_REG)));
    bme280_calib.dig_H2 = ((int16_t)((spi_readreg(BME280_DIG_H2_MSB_REG) << 8) + spi_readreg(BME280_DIG_H2_LSB_REG)));
    bme280_calib.dig_H3 = ((uint8_t)(spi_readreg(BME280_DIG_H3_REG)));
    bme280_calib.dig_H4 = ((int16_t)((spi_readreg(BME280_DIG_H4_MSB_REG) << 4) + (spi_readreg(BME280_DIG_H4_LSB_REG) & 0x0F)));
    bme280_calib.dig_H5 = ((int16_t)((spi_readreg(BME280_DIG_H5_MSB_REG) << 4) + ((spi_readreg(BME280_DIG_H4_LSB_REG) >> 4) & 0x0F)));
    bme280_calib.dig_H6 = ((int8_t)spi_readreg(BME280_DIG_H6_REG));

    bme280_setmode(MODE_NORMAL);

    static float temperature;

    bme280_gettemp(&temperature);
    snprintf(tp, sizeof(tp), "\nGot Temp: %f", temperature);
    cust_print(tp);

    static float pressure;
    bme280_getpressure(&pressure);
    snprintf(tp, sizeof(tp), "\nGot Pressure: %f", pressure);
    cust_print(tp);

    static float height;
    static float ref_pressure = 101325.0;
    height = (-44330.77)*(pow((pressure/ref_pressure), 0.190263) - 1.0);
    snprintf(tp, sizeof(tp), "\nGot Height: %f", height);
    cust_print(tp);

    static float humidity;
    bme280_gethum(&humidity);
    snprintf(tp, sizeof(tp), "\nGot Humidity: %f", humidity);
    cust_print(tp);


    cust_print("\nSPI End");
}


