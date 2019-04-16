/*
 * EPaperTask.h
 *
 *  Created on: Apr 12, 2019
 *      Author: poorn
 */

#ifndef EPAPERTASK_H_
#define EPAPERTASK_H_

#include "main.h"

#define Valid_ASCII_Lowest  0x20
#define Valid_ASCII_Highest 0x7E

// E-Paper Section
#define EP_Frame_Header     0xA5
#define EP_Frame_End_1      0xCC
#define EP_Frame_End_2      0x33
#define EP_Frame_End_3      0xC3
#define EP_Frame_End_4      0x3C
#define EP_Command_Max_Length       1023
#define EP_CMD_Handshake            0x00
#define EP_CMD_Storage_Get          0x06
#define EP_CMD_Storage_Set          0x07
#define EP_CMD_Stop                 0x08
#define EP_CMD_Refresh_Update       0x0A
#define EP_CMD_Set_Draw_Color           0x10
#define EP_CMD_Get_Draw_Color           0x11
#define EP_CMD_Fill_Circle          0x27
#define EP_CMD_Clear                0x2E
#define EP_Handshake_Type           0x01
#define EP_Storage_Get_Type         0x01
#define EP_Storage_Set_Type         0x02
#define EP_Stop_Type                        0x01
#define EP_Refresh_Update_Type      0x01
#define EP_Set_Draw_Color_Type          0x03
#define EP_Get_Draw_Color_Type          0x01
#define EP_Fill_Circle              0x04
#define EP_Clear_Type               0x01
#define EP_CMD_Display_Image        0x70
#define EP_Type1_Length             0x0009
#define EP_Type2_Length             0x000A
#define EP_Type3_Length             0x000B
#define EP_Type4_Length             0x000F
#define EP_Display_Image_CMD_Length(EP_String_Length)       ((0x000D) + (EP_String_Length))
#define EP_Response_Start_Timeout_ms        2500            // Max allowed value for now is 2540
#define EP_Response_End_Timeout_ms          100

void epaper_all(void *pvParameters);

#endif /* EPAPERTASK_H_ */
