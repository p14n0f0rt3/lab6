/*
 * uart.h
 *
 * UART example for ATMega328P clocked at 16 MHz
 *
 * TODO :-
 * 	- Implement string read function
 * 	- Optimize for size
 * 	- Add helper routines and compile to .a file
 *
 *  Created on: 22-Jan-2014
 *      Author: Shrikant Giridhar
 */

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdint.h>

/* Probably already defined somewhere else. Define here, if isn't. */
#ifndef F_CPU
#define	F_CPU			16000000UL
#endif

/* Settings */
#define _BAUD			115200					// Baud rate (115200 is default)
#define _DATA			0x03					// Number of data bits in frame = byte tranmission
#define _UBRR			(F_CPU/8)/_BAUD - 1		// Used for UBRRL and UBRRH

#define RX_BUFF			100

/* Useful macros */
#define TX_START()		UCSR0B |= _BV(TXEN0)	// Enable TX
#define TX_STOP()		UCSR0B &= ~_BV(TXEN0)	// Disable TX
#define RX_START()		UCSR0B |= _BV(RXEN0)	// Enable RX
#define RX_STOP()		UCSR0B &= ~_BV(RXEN0)	// Disable RX
#define COMM_START()	TX_START(); RX_START()	// Enable communications

/* Interrupt macros; Remember to set the GIE bit in SREG before using (see datasheet) */
#define RX_INTEN()		UCSR0B |= _BV(RXCIE0)	// Enable interrupt on RX complete
#define RX_INTDIS()		UCSR0B &= ~_BV(RXCIE0)	// Disable RX interrupt
#define TX_INTEN()		UCSR0B |= _BV(TXCIE0)	// Enable interrupt on TX complete
#define TX_INTDIS()		UCSR0B &= ~_BV(TXCIE0)	// Disable TX interrupt

/* Prototypes */
void initUART(void);
uint8_t getByte(void);
void putByte(unsigned char data);
void writeString(char *str);
char* readString(void);

#endif /* UART_H_ */
