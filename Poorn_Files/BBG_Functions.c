#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

// Global Variables that are shared between threads
char Check_Code[6];
bool Person_ID; //0: Poorn, 1: Khalid

//******************************* NFC Thread *************************//

const char Khalid_Image_1[] = "K1.bmp";
const char Khalid_Image_2[] = "K2.bmp";
const char Khalid_Image_3[] = "K3.bmp";

const char Poorn_Image_1[] = "P1.bmp";
const char Poorn_Image_2[] = "P2.bmp";
const char Poorn_Image_3[] = "P3.bmp";

const char Khalid_Code_1[] = {'0', '4', '2', '4', '1', '9'};
const char Khalid_Code_2[] = {'2', '2', '2', '2', '2', '2'};
const char Khalid_Code_3[] = {'4', '3', '1', '1', '0', '0'};

const char Poorn_Code_1[] = {'C', '3', '1', '3', '*', '*'};
const char Poorn_Code_2[] = {'#', '4', '0', '3', '1', '9'};
const char Poorn_Code_2[] = {'2', '5', '0', '7', '9', '7'};

const uint8_t Poorn_NFC_ID[] = {0xB4, 0x58, 0xC3, 0x94};

const uint8_t Khalid_NFC_ID[] = {0xF4, 0x95, 0xA6, 0x94};

typedef struct {
    uint8_t NFC_Tag_ID_Array[4];
} NFC_T2B_Struct;

typedef struct {
    bool KeyPad_Poll;
    bool EP_Update;
    char Image_Name[10];
} KE_B2T_Struct;

NFC_T2B_Struct NFC_Rx;

KE_B2T_Struct EK_Tx;

bool NFC_Function(void)
{
	static uint8_t i, image_select;
		
	for(i = 0; i < 4; i ++)
	{
		if(NFC_Rx.NFC_Tag_ID_Array[i] != Poorn_NFC_ID[i])		break;
	}
	
	if(i != 4)
	{
		for(i = 0; i < 4; i ++)
		{
			if(NFC_Rx.NFC_Tag_ID_Array[i] != Khalid_NFC_ID[i])		break;
		}
		if(i != 4)
		{
			EK_Tx.KeyPad_Poll = false;
			EK_Tx.EP_Update = false;
			Log_Error("\nUnknown Tag is Presented... Stage 1 Failed");
			return true;
		}		
		else
		{
			Log_Info("\nTag of Khalid is Verified... Stage 1 Succeeded");
			Person_ID = 1; // Khalid
			EK_Tx.KeyPad_Poll = true;
			EK_Tx.EP_Update = true;
			srand(time(0));
			image_select = rand() % 3;
			switch(image_select)
			{
				case 0:
					strncpy(EK_Tx.Image_Name, Khalid_Image_1, sizeof(EK_Tx.Image_Name));
					for(i = 0; i < 6; i ++)		Check_Code[i] = Khalid_Code_1[i];
					break;
				case 1:
					strncpy(EK_Tx.Image_Name, Khalid_Image_2, sizeof(EK_Tx.Image_Name));
					for(i = 0; i < 6; i ++)		Check_Code[i] = Khalid_Code_2[i];
					break;
				case 2:
					strncpy(EK_Tx.Image_Name, Khalid_Image_3, sizeof(EK_Tx.Image_Name));
					for(i = 0; i < 6; i ++)		Check_Code[i] = Khalid_Code_3[i];
					break;
				default:
					break;
			}
			return false;
		}
	}		
	else
	{
		Log_Info("\nTag of Poorn is Verified... Stage 1 Succeeded");
		Person_ID = 0; // Poorn
		EK_Tx.KeyPad_Poll = true;
		EK_Tx.EP_Update = true;
		srand(time(0));
		image_select = rand() % 3;
		switch(image_select)
		{
			case 0:
				strncpy(EK_Tx.Image_Name, Poorn_Image_1, sizeof(EK_Tx.Image_Name));
				for(i = 0; i < 6; i ++)		Check_Code[i] = Poorn_Code_1[i];
				break;
			case 1:
				strncpy(EK_Tx.Image_Name, Poorn_Image_2, sizeof(EK_Tx.Image_Name));
				for(i = 0; i < 6; i ++)		Check_Code[i] = Poorn_Code_2[i];
				break;
			case 2:
				strncpy(EK_Tx.Image_Name, Poorn_Image_3, sizeof(EK_Tx.Image_Name));
				for(i = 0; i < 6; i ++)		Check_Code[i] = Poorn_Code_3[i];
				break;
			default:
				break;
		}
		return false;
	}
}

//******************************* KE Thread *************************//

typedef struct {
    uint8_t KeyPad_Code[6];
} KE_T2B_Struct;

typedef struct {
    bool LC_Poll;
} LC_B2T_Struct;

KE_T2B_Struct EK_Rx;

LC_B2T_Struct LC_Tx;

bool EK_Function(void)
{
	static uint8_t i;
	
	Log_Info("\nChecking Code...");
	for(i = 0; i < 6; i ++)
	{
		if(EK_Rx.KeyPad_Code[i] != Check_Code[i])		break;
	}
		
	if(i != 6)
	{
		Log_Error("\nIncorrect Code Entered... Stage 2 Failed");
		return true;
	}
	else
	{
		Log_Info("\nCode Verified... Stage 2 Succeeded");
		return false;
	}
}


//******************************* LC Thread *************************//

#define     LC_VerificationLowmv            200
#define     LC_VerificationHighmv           300
#define     LC_ConsecutiveVerificationNeeded      5

#define     SV_Enable_Pos       0
#define     SV_Enable_Mask      (1 << SV_Enable_Pos)

typedef struct {
    uint16_t LC_SamplesArraymv[20];
} LC_T2B_Struct;

typedef struct{
    uint8_t OI_Data;
} OI_B2T_Struct;

LC_T2B_Struct LC_Rx;
OI_B2T_Struct OI_Tx;

bool LC_Function(void)
{
	static uint8_t i, valid;
	
	for(i = 0; i < 20; i ++)
	{
        if((LC_SamplesArraymv[i] >= LC_VerificationLowmv) && (LC_SamplesArraymv[i] <= LC_VerificationHighmv))		valid += 1;
        else		valid = 0;
        
		if(valid >= LC_ConsecutiveVerificationNeeded)
		{
			Log_Info("\nSamples Verified... Stage 3 Succeeded");
			OI_Data |= SV_Enable_Mask;
			return false;
		}	
	}
	
	Log_Error("\nSamples are outside the Range... Stage 3 Failed");
	return true;
}

