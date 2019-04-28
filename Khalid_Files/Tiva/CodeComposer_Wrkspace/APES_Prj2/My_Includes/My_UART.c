/*
 * My_UART.c
 *
 *  Created on: Apr 14, 2019
 *
 *  # ALL INFO CAN BE FOUND IN THE .h FILE #
 */

#include "My_UART.h"

//Standard includes
#include <string.h>

//TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "driverlib/debug.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"


/* Global variables */
extern char Start_RX[1];
extern bool POLL_RX;



/*******************************************************************
 * List of UART peripherals
 *******************************************************************/
static const uint32_t UARTPeriph[8] =
{
    SYSCTL_PERIPH_UART0, SYSCTL_PERIPH_UART1, SYSCTL_PERIPH_UART2,
	SYSCTL_PERIPH_UART3, SYSCTL_PERIPH_UART4, SYSCTL_PERIPH_UART5,
	SYSCTL_PERIPH_UART6, SYSCTL_PERIPH_UART7
};


/*******************************************************************
 * List of UART Bases
 *******************************************************************/
static const uint32_t UARTBase[8] =
{
 	 UART0_BASE, UART1_BASE, UART2_BASE,
	 UART3_BASE, UART4_BASE, UART5_BASE,
	 UART6_BASE, UART7_BASE
};


/*******************************************************************
 * List of UART Interrupts
 *******************************************************************/
static const uint32_t UARTInt[8] =
{
 	 INT_UART0, INT_UART1, INT_UART2,
	 INT_UART3, INT_UART4, INT_UART5,
	 INT_UART6, INT_UART7
};



char UART_Getchar(uint8_t UART_Num)
{
	/* Error handling - Assert the given UART number is valid */
	ASSERT( UART_Num <= 7 );

    /* Wait until a char is available */
    while(HWREG(UARTBase[UART_Num] + UART_O_FR) & UART_FR_RXFE);

    /* Return char */
    return (HWREG(UARTBase[UART_Num] + UART_O_DR));
}


void UART_Putchar(uint32_t UART_BASE, char TX_Char)
{
	/* Wait while the UART is busy */
	while(HWREG(UART_BASE + UART_O_FR) & UART_FR_BUSY);

	HWREG(UART_BASE + UART_O_DR) = TX_Char;
}



void UART_Putchar_n(uint8_t UART_Num, char* TX_String)
{
	/* Error handling - Assert the given UART number is valid */
	ASSERT( UART_Num <= 7 );

	uint32_t i;
	uint32_t Length = strlen((const char*)TX_String);
	for(i=0; i < Length; i++)
	{
		UART_Putchar(UARTBase[UART_Num], TX_String[i]);
	}
}


bool Send_StructBuffer_UARTx(uint8_t UART_Num, uint8_t* StructToSend)
{
	/* Error handling - Assert the given UART number is valid */
	ASSERT( UART_Num <= 7 );

	uint16_t SizeOfStruct;

	/* Get what structure it is, based on the first byte */
	switch( StructToSend[0] )
	{
		case LogMsg_Struct_ID:
			Log_Msg(T_BBComm, "DEBUG", "STRUCTURE TO SEND IS LOGMSG", LOCAL_ONLY);
			SizeOfStruct = sizeof(LogMsg_Struct);
			break;



		default:
			Log_Msg(T_BBComm, "ERROR", "Send_StructBuffer_UARTx() aborted - unknown structure!", LOCAL_ONLY);
			return false;
	}

	uint16_t i;
	for(i = 0; i < SizeOfStruct; i++)
	{
		UART_Putchar(UARTBase[UART_Num], StructToSend[i]);
	}

	return true;
}


void Init_UARTx(uint8_t UART_Num, uint32_t Clock, uint32_t BaudRate, bool Interrupt_EN)
{
	/* Error handling - Assert the given UART number is valid */
	ASSERT( UART_Num <= 7 );

	/* Init the needed PINs */
	switch(UART_Num)
	{
		/* UART0 PINs - PA0(RX) | PA1(TX) */
		case 0:
		    GPIOPinConfigure(GPIO_PA0_U0RX);
		    GPIOPinConfigure(GPIO_PA1_U0TX);
		    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
			break;

		/* UART1 PINs - PB0(RX) | PB1(TX) */
		case 1:
			GPIOPinConfigure(GPIO_PB0_U1RX);
			GPIOPinConfigure(GPIO_PB1_U1TX);
			GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
			break;

		/* UART2 PINs - PA6(RX) | PA7(TX) */
		case 2:
			GPIOPinConfigure(GPIO_PA6_U2RX);
			GPIOPinConfigure(GPIO_PA7_U2TX);
			GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7);
			break;

		/* UART3 PINs - PA4(RX) | PA5(TX) */
		case 3:
			GPIOPinConfigure(GPIO_PA4_U3RX);
			GPIOPinConfigure(GPIO_PA5_U3TX);
			GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_4 | GPIO_PIN_5);
			break;

		/* UART4 PINs - PK0(RX) | PK1(TX) */
		case 4:
			GPIOPinConfigure(GPIO_PK0_U4RX);
			GPIOPinConfigure(GPIO_PK1_U4TX);
			GPIOPinTypeUART(GPIO_PORTK_BASE, GPIO_PIN_0 | GPIO_PIN_1);
			break;

		/* UART5 PINs - PC6(RX) | PC7(TX) */
		case 5:
			GPIOPinConfigure(GPIO_PC6_U5RX);
			GPIOPinConfigure(GPIO_PC7_U5TX);
			GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
			break;

		/* UART6 PINs - PP0(RX) | PP1(TX) */
		case 6:
			GPIOPinConfigure(GPIO_PP0_U6RX);
			GPIOPinConfigure(GPIO_PP1_U6TX);
			GPIOPinTypeUART(GPIO_PORTP_BASE, GPIO_PIN_0 | GPIO_PIN_1);
			break;

		/* UART7 PINs - PC4(RX) | PC5(TX) */
		case 7:
			GPIOPinConfigure(GPIO_PC4_U7RX);
			GPIOPinConfigure(GPIO_PC5_U7TX);
			GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
			break;
	}

	/* Enable the needed UARTx peripheral */
	SysCtlPeripheralEnable(UARTPeriph[UART_Num]);

	/* Configure UART to be => No Parity, 8 BITs, 1 Stop BIT
	 * Other parameters are chosen by user.
	 * Enables it afterwards */
	UARTConfigSetExpClk(UARTBase[UART_Num], Clock, BaudRate, (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE | UART_CONFIG_WLEN_8));

	if(Interrupt_EN)
	{
		/* Enable UART interrupts */
		IntEnable(UARTInt[UART_Num]);

		/* Enable RX interrupts */
		UARTIntEnable(UARTBase[UART_Num], UART_INT_RX | UART_INT_RT);
	}
}


void UART0_IntHandler(void)
{
	/* Get interrupts */
	uint32_t Flags =  UARTIntStatus(UART0_BASE, true);

	/* Clear the asserted interrupts */
	UARTIntClear(UART0_BASE, Flags);

	//NO FUCNTIONALITY IMPLEMENTED FOR THIS UART HANDLER
}

void UART1_IntHandler(void)
{
	/* Get interrupts */
	uint32_t Flags =  UARTIntStatus(UART1_BASE, true);

	/* Clear the asserted interrupts */
	UARTIntClear(UART1_BASE, Flags);

	if( !POLL_RX )
	{
		Start_RX[0] = UARTCharGetNonBlocking(UART1_BASE);

		if( strcmp(Start_RX, START_CMD) == 0 )
		{
			POLL_RX = true;
		}
	}
}


void UART2_IntHandler(void)
{
	/* Get interrupts */
	uint32_t Flags =  UARTIntStatus(UART2_BASE, true);

	/* Clear the asserted interrupts */
	UARTIntClear(UART2_BASE, Flags);

	//NO FUCNTIONALITY IMPLEMENTED FOR THIS UART HANDLER
}

void UART3_IntHandler(void)
{
	/* Get interrupts */
	uint32_t Flags =  UARTIntStatus(UART3_BASE, true);

	/* Clear the asserted interrupts */
	UARTIntClear(UART3_BASE, Flags);

	//NO FUCNTIONALITY IMPLEMENTED FOR THIS UART HANDLER
}

void UART4_IntHandler(void)
{
	/* Get interrupts */
	uint32_t Flags =  UARTIntStatus(UART4_BASE, true);

	/* Clear the asserted interrupts */
	UARTIntClear(UART4_BASE, Flags);

	//NO FUCNTIONALITY IMPLEMENTED FOR THIS UART HANDLER
}

void UART5_IntHandler(void)
{
	/* Get interrupts */
	uint32_t Flags =  UARTIntStatus(UART5_BASE, true);

	/* Clear the asserted interrupts */
	UARTIntClear(UART5_BASE, Flags);

	//NO FUCNTIONALITY IMPLEMENTED FOR THIS UART HANDLER
}

void UART6_IntHandler(void)
{
	/* Get interrupts */
	uint32_t Flags =  UARTIntStatus(UART6_BASE, true);

	/* Clear the asserted interrupts */
	UARTIntClear(UART6_BASE, Flags);

	//NO FUCNTIONALITY IMPLEMENTED FOR THIS UART HANDLER
}

void UART7_IntHandler(void)
{
	/* Get interrupts */
	uint32_t Flags =  UARTIntStatus(UART7_BASE, true);

	/* Clear the asserted interrupts */
	UARTIntClear(UART7_BASE, Flags);

	//NO FUCNTIONALITY IMPLEMENTED FOR THIS UART HANDLER
}
