/*
 * NFC_Thread.c
 *
 *  Created on: Apr 28, 2019
 *      Author: Khalid AlAwadhi
 */

#include "NFC_Thread.h"

//Standard includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <errno.h>
#include <time.h> 

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"



/* Global Variables */
extern char Check_Code[6];
extern bool Person_ID; //0: Poorn, 1: Khalid

KE_B2T_Struct EK_Tx;		//Sent to Tiva if applicable

const char Khalid_Image_1[] = "K1.bmp";
const char Khalid_Image_2[] = "K2.bmp";
const char Khalid_Image_3[] = "K3.bmp";

const char Poorn_Image_1[] = "P1.bmp";
const char Poorn_Image_2[] = "P2.bmp";
const char Poorn_Image_3[] = "P3.bmp";

const char Khalid_Code_1[] = {'1', '1', '1', '1', '1', '1'};
const char Khalid_Code_2[] = {'2', '2', '2', '2', '2', '2'};
const char Khalid_Code_3[] = {'3', '3', '3', '3', '3', '3'};

const char Poorn_Code_1[] = {'C', '3', '1', '3', '*', '*'};
const char Poorn_Code_2[] = {'#', '4', '0', '3', '1', '9'};
const char Poorn_Code_3[] = {'2', '5', '0', '7', '9', '7'};

const uint8_t Poorn_NFC_ID[] = {0xB4, 0x58, 0xC3, 0x94};

const uint8_t Khalid_NFC_ID[] = {0xF4, 0x95, 0xA6, 0x94};



void * NFCThread(void * args)
{
	/* Create the NFC Thread POSIX queue */
	mqd_t MQ;											//Message queue descriptor

	/* Delete any previous NFC POSIX Qs */
	if(mq_unlink(NFC_POSIX_Q) != 0)
	{
		Log_Msg(BB_NFC, "ERROR", "mq_unlink()", errno, LOCAL_ONLY);
	}
	else
	{
		Log_Msg(BB_NFC, "INFO", "Previous NFC queue was found and successfully unlinked!", 0, LOCAL_ONLY);
	}
	
	
	
	/* Initialize the queue attributes */
	struct mq_attr attr;
	attr.mq_flags = 0;                                  /* Flags: 0 or O_NONBLOCK */
	attr.mq_maxmsg = 10;                                /* Max. # of messages on queue */
	attr.mq_msgsize = sizeof(NFC_T2B_Struct);           /* Max. message size (bytes) */
	attr.mq_curmsgs = 0;                                /* # of messages currently in queue */


	/* Create the NFC Thread queue to get structs sent from Tiva */
	MQ = mq_open(NFC_POSIX_Q, O_CREAT | O_RDONLY | O_CLOEXEC, 0666, &attr);
	if(MQ == (mqd_t) -1)
	{
		Log_Msg(BB_NFC, "FATAL", "mq_open()", errno, LOGGER_AND_LOCAL);
	}
	
	
	/* Struct to hold any recivied messages */
	NFC_T2B_Struct Got_NFC_RX;
	
	
	while(1)
	{
		/* Keep polling for NFC structs */
		if ( (mq_receive(MQ, &Got_NFC_RX, sizeof(NFC_T2B_Struct), NULL) ) != -1 )
		{
	//		Log_Msg(BB_NFC, "DEBUG", "GOT NFC STRUCT FROM TIVA!", 0, LOCAL_ONLY);
			
			
			/* Call NFC funciton */
			if (!NFC_Function(Got_NFC_RX))
			{
				Send_KEStruct_ToTiva(EK_Tx);
			}
		}
	}
	
	
	Log_Msg(BB_NFC, "INFO", "NFC Thread has terminated successfully and will now exit", 0, LOGGER_AND_LOCAL);
	
	return 0;
}





bool NFC_Function(NFC_T2B_Struct NFC_Rx)
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
			Log_Msg(BB_NFC, "ERROR", "Unknown Tag is Presented... Stage 1 Failed", 0, LOGGER_AND_LOCAL);
			return true;
		}		
		else
		{
			Log_Msg(BB_NFC, "INFO", "Tag of Khalid is Verified... Stage 1 Succeeded", 0, LOGGER_AND_LOCAL);
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
	//		printf("\n------>>>>>> Code Set to: %c%c%c%c%c%c\n", Check_Code[0], Check_Code[1], Check_Code[2], Check_Code[3], Check_Code[4], Check_Code[5]);
			return false;
		}
	}		
	else
	{
		Log_Msg(BB_NFC, "INFO", "Tag of Poorn is Verified... Stage 1 Succeeded", 0, LOGGER_AND_LOCAL);
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



void Send_KEStruct_ToTiva(KE_B2T_Struct StructToSend)
{
	StructToSend.ID = KE_B2T_Struct_ID;
	StructToSend.Src = BB_NFC; 
	
	/* Create an array of bytes to fit the given struct */
	uint8_t Buffer_Struct[sizeof(StructToSend)+1];

	/* Copy the contents of our struct to the char array */
	memcpy(Buffer_Struct, &StructToSend, sizeof(StructToSend));


				
	/* Send Struct to TivaComm POSIX Q */
	mqd_t MQ;						//Message queue descriptor
		
	/* Open the TivaComm Thread POSIX queue - write only */
	MQ = mq_open(TIVACOMM_POSIX_Q, O_WRONLY | O_CLOEXEC);		//NOTE: IF YOU GET AN 'O_CLOEXEC' UNDEFINED ERROR ADD -D_GNU_SOURCE TO COMPILER OPTIONS
	
	
	char ErrMsg[MSGSTR_SIZE];								//Temp variable
	
	
	/* Error check */
	if(MQ == (mqd_t) -1)
	{
		snprintf(ErrMsg, MSGSTR_SIZE, "Send_KEStruct_ToTiva() => mq_open(), attempted to open TivaComm queue");
		Log_Msg(BB_NFC, "ERROR", ErrMsg, errno, LOGGER_AND_LOCAL);
		return;
	}
	
	/* Send Struct to POSIX queue */
	if(mq_send(MQ, Buffer_Struct, sizeof(StructToSend), 0) != 0)
	{
		snprintf(ErrMsg, MSGSTR_SIZE, "Send_KEStruct_ToTiva() => mq_send(), attempted to send buffer struct to TivaComm queue");
		Log_Msg(BB_NFC, "ERROR", ErrMsg, errno, LOGGER_AND_LOCAL);
		return;
	}
	
	if(mq_close(MQ) != 0)
	{
		Log_Msg(BB_NFC, "ERROR", "Send_KEStruct_ToTiva() => mq_close(), attempted to close TivaComm queue", errno, LOGGER_AND_LOCAL);
	}
	
}
