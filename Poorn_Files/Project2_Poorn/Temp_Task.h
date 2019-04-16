/*
 * Temp_Task.h
 *
 *  Created on: Apr 10, 2019
 *      Author: poorn
 */

#ifndef TEMP_TASK_H_
#define TEMP_TASK_H_

#include "main.h"

#define Temp_High_Thr    25

#define TMP102_Address      0x48

#define TMP102_Data_Reg         0x00
#define TMP102_Config_Reg       0x01

#define I2C_100KHZ_Mode         false
#define I2C_400KHZ_Mode         true

#define I2C_Write_Mode          false
#define I2C_Read_Mode           true

extern void cust_print(char *txt);
void tmp102_i2c_init(void);
void tmp102_read(float *t_data);
void Temp_Task(void *pvParameters);


#endif /* TEMP_TASK_H_ */
