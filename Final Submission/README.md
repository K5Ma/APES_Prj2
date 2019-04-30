# APES_Prj2

This is Project 2 Developed for the Course AESD (Spring 19) by Khalid AlAwadhi and Poorn Mehta

The relevant Documentation Links Can be found in Final Report.

Our workspaces contain all the needed files. 

*****Description of Files*****

> Control Node (BeagleBone)

>> For Makefile (auto generated) Go to /BeagleBone/CodeLite_Workspace/

>> For Source Code and Files, Go to /BeagleBone/CodeLite_Workspace/Prj2

>> File: Global_Defines.h

>>> Contains Information about Shared Global Variables, Enums, and Structures

>> File: KeypadEpaper_Thread.c and KeypadEpaper_Thread.h

>>> Contains Functions, and Queue, required for Verification Stage 2 (Keypad and EPaper)

>> File: LoadCell_Thread.c and LoadCell_Thread.h

>>> Contains Functions, and Queue, required for Verification Stage 3 (LoadCell)

>> File: Logger_Thread.c and Logger_Thread.h

>>> Contains Functions, and Queues, for Logging all the Important Information

>> File: main.c

>>> This is main file. It sets up everything.

>> File: Master_Functions.c and Master_Functions.h

>>> Contains Functions, and Queues, to make IPC between Threads unified, and Easy to Use, while also providing some other functionalities to make the code flow of different threads/tasks similar

>> File: My_UART_BB.c and My_UART_BB.h

>>> Contains required Functions, and Queue, for handling all the UART control (Software and Hardware)

>> File: NFC_Thread.c and NFC_Thread.handling

>>> Contains Functions, and Queue, required for Verification Stage 1 (NFC)

>> File: TivaComm_Thread.c and TivaComm_Thread.h

>>> Contains Functions, and Queues, for Handling all the Communication Between TIVA (Remote Node) and BeagleBone (Control Node)


> Remote Node (BeagleBone)

>> For TIVA Project, Go to /Tiva/CodeComposer_Wrkspace/APES_Prj2

>> For Source Code and Files, Go to /Tiva/CodeComposer_Wrkspace/APES_Prj2/My_Includes

>> File: BBComm_Task.c and BBComm_Task.h

>>> Contains Functions, and Queues, for Handling all the Communication Between BeagleBone (Control Node) and TIVA (Remote Node)

>> File: Global_Defines.h

>>> Contains Information about Shared Global Variables, Enums, and Structures

>> File: KeypadEpaper_Task.c and KeypadEpaper_Task.h

>>> Contains Functions, Queue, and Driver Code to Interface with Keypad and EPaper Module - to enable Functionality of Verification Stage 2

>> File: LoadCell_Task.c and LoadCell_Task.h

>>> Contains Functions, Queue, and Driver Code to Interface with LoadCell Module - to enable Functionality of Verification Stage 3

>> File: Master_Functions.c and Master_Functions.h

>>> Contains Functions, and Queues, to make IPC between Threads unified, and Easy to Use, while also providing some other functionalities to make the code flow of different threads/tasks similar

>> File: My_UART.c and My_UART.h

>>> Contains required Functions, and Queue, for handling all the UART control (Software and Hardware)

>> File: NFC_Task.c and NFC_Task.h

>>> Contains Functions, Queue, and Driver Code to Interface with NFC Module - to enable Functionality of Verification Stage 1

>> File: Outputs_Task.c and Outputs_Task.h

>>> Contains Functions, Queue, and Driver Code to Interface with various Output Devices