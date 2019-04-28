/*
 * BME280Task.c
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

/*
 *
 * Hardware Connections
 *
 *
 * Note: Left Side - Module Pins & Right Side - Controller Pins
 *
 * CS - PA3 (This is the slave select pin)
 * SDI - PA4 (This is the Serial Data In for the sensor (MOSI) pin)
 * SDO - PA5 (This is the Serial Data In for the sensor (MISO) pin)
 * SCK - PA2 (This is the serial clock pin)
 * 3.3V - 3.3V
 * GND - Ground
 *
 */

#include "BME280Task.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Variables that will be shared between functions
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

SensorCalibration   BME280_calib;

uint32_t BME280_Tx[2];
uint32_t BME280_Rx;
bool BME280_RunStatus, BME280_Error;
uint32_t BME280_Data[8];
int32_t BME280_TempFine;
uint8_t BME280_Retries;

/*
 * Function to setup SPI bus for BME280
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void BME280_SPISetup(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    GPIOPinConfigure(GPIO_PA4_SSI0XDAT0);
    GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);

    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);

    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 |
                   GPIO_PIN_2);

    SSIConfigSetExpClk(SSI0_BASE, SYSTEM_CLOCK, SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 1000000, 8);

    SSIEnable(SSI0_BASE);

    while(SSIDataGetNonBlocking(SSI0_BASE, &BME280_Rx));
}

/*
 * Function to read a single byte from sensor using SPI bus
 * If there is no valid byte then it raises the error flag
 * and sets other variables as per the retry mode
 *
 * Param: Internal Register Address which is to be read
 *
 * Return: Byte which was returned from Sensor
 *
 */
uint8_t BME280_SPIReadReg(uint8_t spi_addr)
{
    BME280_Tx[0] = spi_addr | (1 << 7);
    BME280_Tx[1] = 0x00;

    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, false);

    SSIDataPut(SSI0_BASE, BME280_Tx[0]);
    while(SSIBusy(SSI0_BASE));
    SSIDataPut(SSI0_BASE, BME280_Tx[1]);
    while(SSIBusy(SSI0_BASE));

    SSIDataGet(SSI0_BASE, &BME280_Rx);
    SSIDataGet(SSI0_BASE, &BME280_Rx);

    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);

    if((BME280_RunStatus == true) && ((BME280_Rx & 0x00FF) == 0xFF))
    {
        BME280_Error = true;
        #if     (BME280_Retry_Mode == BME280_Limited)
            BME280_Retries = BME280_Max_Retries;
        #endif
        #if     BME280_DEBUG_PRINTF
            BME280_Print("\nBME280 is Offline");
        #endif
    }

    return (BME280_Rx & 0x00FF);
}

/*
 * Function to read multiple registers (in consecutive locations) of Sensor
 *
 * Param_1: Pointer to the array/buffer where the return bytes should be stored
 * Param_2: Starting address (lowest in series) of the internal register
 * Param_3: Number of registers/8 bit locations to be read
 *
 * Return: Null
 *
 */
void BME280_SPIReadMultiReg(uint32_t *rx_addr, uint8_t start_addr, uint8_t no_of_regs)
{
    static uint8_t i;

    BME280_Tx[0] = start_addr | (1 << 7);
    BME280_Tx[1] = 0x00;

    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, false);

    SSIDataPut(SSI0_BASE, BME280_Tx[0]);
    while(SSIBusy(SSI0_BASE));
    SSIDataGet(SSI0_BASE, rx_addr);

    for(i = 0; i < no_of_regs; i ++)
    {
        SSIDataPut(SSI0_BASE, BME280_Tx[1]);
        while(SSIBusy(SSI0_BASE));
        SSIDataGet(SSI0_BASE, rx_addr++);
    }

    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
}

/*
 * Function to write 8 bit data to the internal register of the sensor
 *
 * Param_1: Address of the internal sensor
 * Param_2: 8 bit data to be writeen
 *
 * Return: Null
 *
 */
void BME280_SPIWriteReg(uint8_t spi_addr, uint8_t spi_data)
{
    BME280_Tx[0] = spi_addr & ~(1 << 7);
    BME280_Tx[1] = spi_data;

    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, false);

    SSIDataPut(SSI0_BASE, BME280_Tx[0]);
    while(SSIBusy(SSI0_BASE));
    SSIDataPut(SSI0_BASE, BME280_Tx[1]);
    while(SSIBusy(SSI0_BASE));

    SSIDataGet(SSI0_BASE, &BME280_Rx);
    SSIDataGet(SSI0_BASE, &BME280_Rx);

    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
}

/*
 * Function to setup standby time parameter of the sensor
 *
 * Param: Timeout setting (available from predefined set)
 *
 * Return: Null
 *
 */
void BME280_SetStandBy(uint8_t timeout)
{
    static uint8_t ctrl_data;
    ctrl_data = BME280_SPIReadReg(BME280_CONFIG_REG);
    if(timeout > BME280_Standby_20ms)     timeout = BME280_Standby_1000ms;
    ctrl_data &= ~(BME280_Standby_Mask);
    ctrl_data |= (timeout << BME280_Standby_Pos);
    BME280_SPIWriteReg(BME280_CONFIG_REG, ctrl_data);
}

/*
 * Function to read currently set standby time parameter of the sensor
 *
 * Param: Null
 *
 * Return: Timeout setting (available from predefined set)
 *
 */
uint8_t BME280_GetStandBy(void)
{
    static uint8_t ctrl_data;
    ctrl_data = BME280_SPIReadReg(BME280_CONFIG_REG);
    ctrl_data &= (BME280_Standby_Mask);
    return (ctrl_data >> BME280_Standby_Pos);
}

/*
 * Function to setup operation mode parameter of the sensor
 *
 * Param: Mode setting (available from predefined set)
 *
 * Return: Null
 *
 */
void BME280_SetMode(uint8_t mode)
{
    static uint8_t ctrl_data;
    ctrl_data = BME280_SPIReadReg(BME280_CTRL_MEAS_REG);
    if((mode != BME280_MODE_SLEEP) && (mode != BME280_MODE_FORCED) && (mode != BME280_MODE_NORMAL))     mode = BME280_MODE_NORMAL;
    ctrl_data &= ~(BME280_MODE_MASK);
    ctrl_data |= (mode << BME280_MODE_Pos);
    BME280_SPIWriteReg(BME280_CTRL_MEAS_REG, ctrl_data);
}

/*
 * Function to read currently set operation mode parameter of the sensor
 *
 * Param: Null
 *
 * Return: Mode setting (available from predefined set)
 *
 */
uint8_t BME280_GetMode(void)
{
    static uint8_t ctrl_data;
    ctrl_data = BME280_SPIReadReg(BME280_CTRL_MEAS_REG);
    ctrl_data &= (BME280_MODE_MASK);
    return (ctrl_data >> BME280_MODE_Pos);
}

/*
 * Function to setup Humidity Oversampling parameter of the sensor
 *
 * Param: Oversampling Rate (available from predefined set)
 *
 * Return: Null
 *
 */
void BME280_SetHumOVS(uint8_t ovs)
{
    static uint8_t ctrl_data, orig_mode;
    orig_mode = BME280_GetMode();
    BME280_SetMode(BME280_MODE_SLEEP);
    ctrl_data = BME280_SPIReadReg(BME280_CTRL_HUMIDITY_REG);
    if(ovs > BME280_Hum_OVS16)    ovs = BME280_Hum_OVS16;
    ctrl_data &= ~(BME280_Hum_OVS_Mask);
    ctrl_data |= (ovs << BME280_Hum_OVS_Pos);
    BME280_SPIWriteReg(BME280_CTRL_HUMIDITY_REG, ctrl_data);
    BME280_SetMode(orig_mode);
}

/*
 * Function to setup Temperature Oversampling parameter of the sensor
 *
 * Param: Oversampling Rate (available from predefined set)
 *
 * Return: Null
 *
 */
void BME280_SetTempOVS(uint8_t ovs)
{
    static uint8_t ctrl_data, orig_mode;
    orig_mode = BME280_GetMode();
    BME280_SetMode(BME280_MODE_SLEEP);
    ctrl_data = BME280_SPIReadReg(BME280_CTRL_MEAS_REG);
    if(ovs > BME280_Temp_OVS16)    ovs = BME280_Temp_OVS16;
    ctrl_data &= ~(BME280_Temp_OVS_Mask);
    ctrl_data |= (ovs << BME280_Temp_OVS_Pos);
    BME280_SPIWriteReg(BME280_CTRL_MEAS_REG, ctrl_data);
    BME280_SetMode(orig_mode);
}

/*
 * Function to setup Pressure Oversampling parameter of the sensor
 *
 * Param: Oversampling Rate (available from predefined set)
 *
 * Return: Null
 *
 */
void BME280_SetPressureOVS(uint8_t ovs)
{
    static uint8_t ctrl_data, orig_mode;
    orig_mode = BME280_GetMode();
    BME280_SetMode(BME280_MODE_SLEEP);
    ctrl_data = BME280_SPIReadReg(BME280_CTRL_MEAS_REG);
    if(ovs > BME280_Pressure_OVS16)    ovs = BME280_Pressure_OVS16;
    ctrl_data &= ~(BME280_Pressure_OVS_Mask);
    ctrl_data |= (ovs << BME280_Pressure_OVS_Pos);
    BME280_SPIWriteReg(BME280_CTRL_MEAS_REG, ctrl_data);
    BME280_SetMode(orig_mode);
}

/*
 * Function to read all the calibration data from the sensor
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void BME280_ReadAllCalib(void)
{
    //Reading all compensation data, range 0x88:A1, 0xE1:E7
    BME280_calib.dig_T1 = ((uint16_t)((BME280_SPIReadReg(BME280_DIG_T1_MSB_REG) << 8) + BME280_SPIReadReg(BME280_DIG_T1_LSB_REG)));
    BME280_calib.dig_T2 = ((int16_t)((BME280_SPIReadReg(BME280_DIG_T2_MSB_REG) << 8) + BME280_SPIReadReg(BME280_DIG_T2_LSB_REG)));
    BME280_calib.dig_T3 = ((int16_t)((BME280_SPIReadReg(BME280_DIG_T3_MSB_REG) << 8) + BME280_SPIReadReg(BME280_DIG_T3_LSB_REG)));

    BME280_calib.dig_P1 = ((uint16_t)((BME280_SPIReadReg(BME280_DIG_P1_MSB_REG) << 8) + BME280_SPIReadReg(BME280_DIG_P1_LSB_REG)));
    BME280_calib.dig_P2 = ((int16_t)((BME280_SPIReadReg(BME280_DIG_P2_MSB_REG) << 8) + BME280_SPIReadReg(BME280_DIG_P2_LSB_REG)));
    BME280_calib.dig_P3 = ((int16_t)((BME280_SPIReadReg(BME280_DIG_P3_MSB_REG) << 8) + BME280_SPIReadReg(BME280_DIG_P3_LSB_REG)));
    BME280_calib.dig_P4 = ((int16_t)((BME280_SPIReadReg(BME280_DIG_P4_MSB_REG) << 8) + BME280_SPIReadReg(BME280_DIG_P4_LSB_REG)));
    BME280_calib.dig_P5 = ((int16_t)((BME280_SPIReadReg(BME280_DIG_P5_MSB_REG) << 8) + BME280_SPIReadReg(BME280_DIG_P5_LSB_REG)));
    BME280_calib.dig_P6 = ((int16_t)((BME280_SPIReadReg(BME280_DIG_P6_MSB_REG) << 8) + BME280_SPIReadReg(BME280_DIG_P6_LSB_REG)));
    BME280_calib.dig_P7 = ((int16_t)((BME280_SPIReadReg(BME280_DIG_P7_MSB_REG) << 8) + BME280_SPIReadReg(BME280_DIG_P7_LSB_REG)));
    BME280_calib.dig_P8 = ((int16_t)((BME280_SPIReadReg(BME280_DIG_P8_MSB_REG) << 8) + BME280_SPIReadReg(BME280_DIG_P8_LSB_REG)));
    BME280_calib.dig_P9 = ((int16_t)((BME280_SPIReadReg(BME280_DIG_P9_MSB_REG) << 8) + BME280_SPIReadReg(BME280_DIG_P9_LSB_REG)));

    BME280_calib.dig_H1 = ((uint8_t)(BME280_SPIReadReg(BME280_DIG_H1_REG)));
    BME280_calib.dig_H2 = ((int16_t)((BME280_SPIReadReg(BME280_DIG_H2_MSB_REG) << 8) + BME280_SPIReadReg(BME280_DIG_H2_LSB_REG)));
    BME280_calib.dig_H3 = ((uint8_t)(BME280_SPIReadReg(BME280_DIG_H3_REG)));
    BME280_calib.dig_H4 = ((int16_t)((BME280_SPIReadReg(BME280_DIG_H4_MSB_REG) << 4) + (BME280_SPIReadReg(BME280_DIG_H4_LSB_REG) & 0x0F)));
    BME280_calib.dig_H5 = ((int16_t)((BME280_SPIReadReg(BME280_DIG_H5_MSB_REG) << 4) + ((BME280_SPIReadReg(BME280_DIG_H4_LSB_REG) >> 4) & 0x0F)));
    BME280_calib.dig_H6 = ((int8_t)BME280_SPIReadReg(BME280_DIG_H6_REG));
}

/*
 * Function to read all 8 data registers of the sensor (from Pressure MSB to Humidity LSB)
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void BME280_ReadAllDataReg(void)
{
    BME280_SPIReadMultiReg(&BME280_Data[0], BME280_PRESSURE_MSB_REG, BME280_DataRegNos);
}

/*
 * Function to calculate the pressure in Pascals
 *
 * Param: float pointer to the variable where the result should be stored
 *
 * Return: Null
 *
 */
void BME280_GetPressure(float *var_p)
{
    static int32_t adc_pressure;
    static int64_t var1, var2, pressure_fine;

    adc_pressure = ((uint32_t)BME280_Data[BME280_Pressure_MSB_Pos] << 12) |
            ((uint32_t)BME280_Data[BME280_Pressure_LSB_Pos] << 4) | ((BME280_Data[BME280_Pressure_XLSB_Pos] >> 4) & 0x0F);

    var1 = ((int64_t)BME280_TempFine) - 128000;
    var2 = var1 * var1 * (int64_t)BME280_calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)BME280_calib.dig_P5)<<17);
    var2 = var2 + (((int64_t)BME280_calib.dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)BME280_calib.dig_P3)>>8) + ((var1 * (int64_t)BME280_calib.dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)BME280_calib.dig_P1)>>33;
    if (var1 == 0)
    {
        *var_p = -1;
    }
    else
    {
        pressure_fine = 1048576 - adc_pressure;
        pressure_fine = (((pressure_fine<<31) - var2)*3125)/var1;
        var1 = (((int64_t)BME280_calib.dig_P9) * (pressure_fine>>13) * (pressure_fine>>13)) >> 25;
        var2 = (((int64_t)BME280_calib.dig_P8) * pressure_fine) >> 19;
        pressure_fine = ((pressure_fine + var1 + var2) >> 8) + (((int64_t)BME280_calib.dig_P7)<<4);

        *var_p = (float)(pressure_fine / 256.0);
    }
}

/*
 * Function to calculate the temperature in degree Celcius
 *
 * Param: float pointer to the variable where the result should be stored
 *
 * Return: Null
 *
 */
void BME280_GetTemp(float *var_t)
{
    static int32_t adc_temp;
    static int64_t var1, var2;


    adc_temp = ((uint32_t)BME280_Data[BME280_Temp_MSB_Pos] << 12) |
            ((uint32_t)BME280_Data[BME280_Temp_LSB_Pos] << 4) | ((BME280_Data[BME280_Temp_XLSB_Pos] >> 4) & 0x0F);

    //By datasheet, calibrate
    var1 = ((((adc_temp>>3) - ((int32_t)BME280_calib.dig_T1<<1))) * ((int32_t)BME280_calib.dig_T2)) >> 11;
    var2 = (((((adc_temp>>4) - ((int32_t)BME280_calib.dig_T1)) * ((adc_temp>>4) - ((int32_t)BME280_calib.dig_T1))) >> 12) * ((int32_t)BME280_calib.dig_T3)) >> 14;
    BME280_TempFine = var1 + var2;
    *var_t = (BME280_TempFine * 5 + 128) >> 8;
    *var_t /= 100;
}

/*
 * Function to calculate the relative humidity in percentages
 *
 * Param: float pointer to the variable where the result should be stored
 *
 * Return: Null
 *
 */
void BME280_GetHum(float *var_h)
{
    static int32_t adc_hum;
    static int64_t var1;

    adc_hum = ((uint32_t)BME280_Data[BME280_Hum_MSB_Pos] << 8) | ((uint32_t)BME280_Data[BME280_Hum_LSB_Pos]);

    var1 = (BME280_TempFine - ((int32_t)76800));
    var1 = (((((adc_hum << 14) - (((int32_t)BME280_calib.dig_H4) << 20) - (((int32_t)BME280_calib.dig_H5) * var1)) +
    ((int32_t)16384)) >> 15) * (((((((var1 * ((int32_t)BME280_calib.dig_H6)) >> 10) * (((var1 * ((int32_t)BME280_calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
    ((int32_t)BME280_calib.dig_H2) + 8192) >> 14));
    var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)BME280_calib.dig_H1)) >> 4));
    var1 = (var1 < 0 ? 0 : var1);
    var1 = (var1 > 419430400 ? 419430400 : var1);

    *var_h = (float)((var1>>12) / 1024.0);
}

/*
 * Function to set the preferred initial parameters of the sensor
 *
 * Param: Null
 *
 * Return: Boolean variable indicating whether the setup was successful or not
 * (True: Success, False: Failure)
 *
 */
bool BME280_SensorSetup(void)
{
    if(BME280_SPIReadReg(BME280_CHIP_ID_REG) == BME280_DevID)
    {
        #if BME280_DEBUG_PRINTF
            BME280_Print("\nBME280 Device ID Verified");
        #endif
    }
    else
    {
        #if BME280_DEBUG_PRINTF
            BME280_Print("\nBME280 Device ID Verification Failed");
        #endif
        return true;
    }

    BME280_ReadAllCalib();

    BME280_SetStandBy(BME280_Standby_20ms);
    if(BME280_GetStandBy() == BME280_Standby_20ms)
    {
        #if BME280_DEBUG_PRINTF
                BME280_Print("\nBME280 Standby set to 20ms");
        #endif
    }
    else
    {
        #if BME280_DEBUG_PRINTF
                BME280_Print("\nBME280 Standby Setting Failed");
        #endif
        return true;
    }

    BME280_SetMode(BME280_MODE_NORMAL);
    if(BME280_GetMode() == BME280_MODE_NORMAL)
    {
        #if BME280_DEBUG_PRINTF
                BME280_Print("\nBME280 Mode Set to Normal");
        #endif
    }
    else
    {
        #if BME280_DEBUG_PRINTF
                BME280_Print("\nBME280 Mode Setting Failed");
        #endif
        return true;
    }

    BME280_SetHumOVS(BME280_Hum_OVS1);
    #if BME280_DEBUG_PRINTF
            BME280_Print("\nBME280 Humidity Oversampling Set to 1x");
    #endif

    BME280_SetTempOVS(BME280_Temp_OVS1);
    #if BME280_DEBUG_PRINTF
            BME280_Print("\nBME280 Temperature Oversampling Set to 1x");
    #endif

    BME280_SetPressureOVS(BME280_Pressure_OVS1);
    #if BME280_DEBUG_PRINTF
            BME280_Print("\nBME280 Pressure Oversampling Set to 1x");
    #endif

    return false;
}

/*
 * Function to check whether the sensor is online or not
 * by simply reading the ID of the sensor. Updates the
 * proper varialbes if there is an error.
 *
 * Param: Null
 *
 * Return: Null
 *
 */
void BME280_TestSensor(void)
{
    BME280_Error = false;
    if(BME280_SPIReadReg(BME280_CHIP_ID_REG) != BME280_DevID)
    {
        #if BME280_DEBUG_PRINTF
            BME280_Print("\nBME280 Device ID Verification Failed");
        #endif
        BME280_Error = true;
        #if     (BME280_Retry_Mode == BME280_Limited)
            BME280_Retries = BME280_Max_Retries;
        #endif
        #if     BME280_DEBUG_PRINTF
            BME280_Print("\nBME280 is Offline");
        #endif
    }
}

/*
 *
 * Callback Function for BME280 Task
 *
 * Return: Null
 *
 */
void BME280Task(void *pvParameters)
{
    BME280_RunStatus = false;
    BME280_Error = false;

    static char tp[200];

    BME280_SPISetup();
    BME280_Error = BME280_SensorSetup();

    if(BME280_Error == true)
    {
        #if     (BME280_Retry_Mode == BME280_Limited)
            BME280_Retries = BME280_Max_Retries;
        #endif
        #if     BME280_DEBUG_PRINTF
            BME280_Print("\nBME280 Setup Failed");
        #endif
    }
    else
    {
        #if     BME280_DEBUG_PRINTF
            BME280_Print("\nBME280 Setup Succeeded");
        #endif
    }

    static float temperature, pressure, height, humidity;
    static float ref_pressure = 101325.0;

    BME280_RunStatus = true;

#if     BME280_INDIVIDUAL_TESTING

    /*
     * This portion just tests the BME280 Sensor individually
     * It doesn't rely on anything that is not covered
     * or provided by its own header and source files
     *
     * This mode is only for raw testing, and may not
     * include any of the error checking feature(s)
     */

    while(1)
    {
        BME280_ReadAllDataReg();

        BME280_GetTemp(&temperature);

        BME280_GetPressure(&pressure);

        height = ((-44330.77)*(pow((pressure/ref_pressure), 0.190263) - 1.0)) * 3.28084;

        BME280_GetHum(&humidity);

        snprintf(tp, sizeof(tp), "\nTemp. - %.2fC(%.2fF) Atm. Pres. - %.2fPa Alt. - %.2fFt R.H. - %.2f%", temperature,
                 ((temperature * 1.8) + 32), pressure, height, humidity);
        BME280_Print(tp);

        vTaskDelay(BME280_Polling_Timems);
    }
#else

    /*
     * Normal Operation (Parameters and Returns indicate communication with Central Task)
     *
     * This Task does not require any parameter to get from central task through IPC
     * since this is a fail-safe mechanism, and this task should always be running
     *
     * This Task will have 2 variables - BME280_milliTemperatureCelcius and BME280_milliHumidityPercent
     * both in uint16_t that should be reported back to the central task through IPC
     * It will also report back the current state of the BME280 Sensor
     *
     * Param_1: Null
     *
     * Return_1: int16_t BME280_milliTemperatureCelcius
     *          (To save size, I have converted float to integer
     *           and while doing that - multiplied float with 1000
     *           in order to get the required accuracy)
     *
     * Return_2: uint16_t BME280_milliHumidityPercent
     *          (To save size, I have converted float to integer
     *           and while doing that - multiplied float with 1000
     *           in order to get the required accuracy)
     *
     * Return_3: bool BME280_Error
     *           (false: Online, true: OFfline - error present)
     *
     */

    static uint16_t BME280_milliTemperatureCelcius, BME280_milliHumidityPercent;
    static uint8_t idletimecount;

    idletimecount = 0;

    while(1)
    {

        if(idletimecount >= (BME280_Online_Test_Timems / BME280_Polling_Timems))
        {
            BME280_TestSensor();
            #if     BME280_DEBUG_PRINTF
                    BME280_Print("\nChecking BME280 Status...");
                    if(LC_Error == false)   BME280_Print("\nBME280 is Online");
                    else    BME280_Print("\nBME280 is Offline");
            #endif
            idletimecount = 0;
        }

        if(BME280_Error == true)
        {
        #if     (BME280_Retry_Mode == BME280_Limited)
            if(BME280_Retries != 0x00)
            {
                BME280_Retries -= 1;
        #endif
                BME280_SPISetup();
                BME280_Error = BME280_SensorSetup();
        #if     (BME280_Retry_Mode == BME280_Limited)
            }
        #endif
        }

        // Note: Poll_BME280 flag shouldn't be cleared automatically.
        if((Poll_BME280 == true) && (BME280_Error == false))
        {
            BME280_ReadAllDataReg();
            BME280_GetTemp(&temperature);
            BME280_GetHum(&humidity);

            if(BME280_Error == false)
            {
                #if     BME280_DEBUG_PRINTF
                    snprintf(tp, sizeof(tp), "\nTemp. - %.2fC(%.2fF) R.H. - %.2f%", temperature,
                             ((temperature * 1.8) + 32), humidity);
                    BME280_Print(tp);
                #endif
                BME280_milliTemperatureCelcius = (uint16_t)(temperature * 1000);
                BME280_milliHumidityPercent = (uint16_t)(humidity * 1000);
            }
            else
            {
                #if     BME280_DEBUG_PRINTF
                    BME280_Print("\nBME280 is Offline");
                #endif
                BME280_milliTemperatureCelcius = 0;
                BME280_milliHumidityPercent = 0;
            }
        }

        vTaskDelay(BME280_Polling_Timems);
        idletimecount += 1;
    }
#endif
}


