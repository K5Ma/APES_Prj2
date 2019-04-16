/*
 * main.h
 *
 *  Created on: Mar 28, 2015
 *      Author: akobyljanec
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// System clock rate, 120 MHz
#define SYSTEM_CLOCK    120000000U

#define LED_Sender          0x01
#define Temp_Sender         0x02

#define LED_Period_ms       500
#define Temp_Period_ms      1000

extern struct ipc_data{
    char name[10];
    uint32_t led_time;
    uint32_t led_count;
    uint32_t temp_time;
    float temp_value;
    uint8_t sender;
};

extern uint8_t i2c_data[2];

extern uint8_t led_state;

extern uint32_t fun_resp;

char msg[50];

void cust_print(char *txt);

#endif /* MAIN_H_ */
