///*
// * SpeakJetTask.h
// *
// *  Created on: Apr 20, 2019
// *      Author: poorn
// */
//
//#ifndef SPEAKJETTASK_H_
//#define SPEAKJETTASK_H_
//
//// Standard Includes
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include <stdint.h>
//#include <stdbool.h>
//
//#include "drivers/pinout.h"
//#include "utils/uartstdio.h"
//
//// Third Party Includes
//#include "inc/hw_ints.h"
//#include "inc/hw_memmap.h"
//#include "inc/hw_i2c.h"
//#include "inc/hw_sysctl.h"
//#include "inc/hw_types.h"
//#include "inc/hw_uart.h"
//#include "inc/hw_adc.h"
//#include "inc/hw_pwm.h"
//
//// TivaWare includes
//#include "driverlib/sysctl.h"
//#include "driverlib/debug.h"
//#include "driverlib/rom.h"
//#include "driverlib/interrupt.h"
//#include "driverlib/i2c.h"
//#include "driverlib/debug.h"
//#include "driverlib/gpio.h"
//#include "driverlib/pin_map.h"
//#include "driverlib/uart.h"
//#include "driverlib/timer.h"
//#include "driverlib/fpu.h"
//#include "driverlib/adc.h"
//#include "driverlib/pwm.h"
//
//#include "sensorlib/i2cm_drv.h"
//
//// System clock rate, 120 MHz
//#define SYSTEM_CLOCK    120000000U
//
//#define     SJ_Enable_Pos       0
//#define     SJ_Enable_Mask      (1 << SJ_Enable_Pos)
//
//#define     SJ_DayTime_Pos      1
//#define     SJ_DayTime_Mask     (3 << SJ_DayTime_Pos)
//#define     SJ_Morning_Time     (0 << SJ_DayTime_Pos)
//#define     SJ_Afternoon_Time   (1 << SJ_DayTime_Pos)
//#define     SJ_Evening_Time     (2 << SJ_DayTime_Pos)
//#define     SJ_Night_Time       (3 << SJ_DayTime_Pos)
//
//#define     SJ_Person_ID_Pos    3
//#define     SJ_Person_ID_Mask   (1 << SJ_Person_ID_Pos)
//#define     SJ_Person_ID_Poorn  (0 << SJ_Person_ID_Pos)
//#define     SJ_Person_ID_Khalid (1 << SJ_Person_ID_Pos)
//
//// Set up a memorable token for "Word Pause"
//#define WP 6    // 6 is 90ms pause
//
//void SpeakJetTask(void *pvParameters);
//
//#endif /* SPEAKJETTASK_H_ */
