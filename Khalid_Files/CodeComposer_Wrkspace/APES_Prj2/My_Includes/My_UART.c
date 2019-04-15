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

//My includes
#include "Global_Defines.h"


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



void Init_UARTx(uint8_t UART_Num, uint32_t Clock, uint32_t BaudRate)
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
}


