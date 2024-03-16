// This program shows how to measure the period of a signal using timer 1 free running counter.

//#define F_CPU 16000000UL
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "lcd.h"

/* Pinout for DIP28 ATMega328P:

                           -------
     (PCINT14/RESET) PC6 -|1    28|- PC5 (ADC5/SCL/PCINT13)
       (PCINT16/RXD) PD0 -|2    27|- PC4 (ADC4/SDA/PCINT12)
       (PCINT17/TXD) PD1 -|3    26|- PC3 (ADC3/PCINT11)
      (PCINT18/INT0) PD2 -|4    25|- PC2 (ADC2/PCINT10)
 (PCINT19/OC2B/INT1) PD3 -|5    24|- PC1 (ADC1/PCINT9)
    (PCINT20/XCK/T0) PD4 -|6    23|- PC0 (ADC0/PCINT8)
                     VCC -|7    22|- GND
                     GND -|8    21|- AREF
(PCINT6/XTAL1/TOSC1) PB6 -|9    20|- AVCC
(PCINT7/XTAL2/TOSC2) PB7 -|10   19|- PB5 (SCK/PCINT5)
   (PCINT21/OC0B/T1) PD5 -|11   18|- PB4 (MISO/PCINT4)
 (PCINT22/OC0A/AIN0) PD6 -|12   17|- PB3 (MOSI/OC2A/PCINT3)
      (PCINT23/AIN1) PD7 -|13   16|- PB2 (SS/OC1B/PCINT2)
  (PCINT0/CLKO/ICP1) PB0 -|14   15|- PB1 (OC1A/PCINT1)
                           -------
*/




unsigned int cnt = 0;

void wait_1ms(void)
{
	unsigned int saved_TCNT1;
	
	saved_TCNT1=TCNT1;
	
	while((TCNT1-saved_TCNT1)<(F_CPU/1000L)); // Wait for 1 ms to pass
}

void waitms(int ms)
{
	while(ms--) wait_1ms();
}

#define PIN_PERIOD (PINB & 0b00000010)

// GetPeriod() seems to work fine for frequencies between 30Hz and 300kHz.
long int GetPeriod (int n)
{
	int i;
	unsigned int overflow;
	unsigned int saved_TCNT1a, saved_TCNT1b;
	
	overflow=0;

	TIFR1=1; // TOV1 can be cleared by writing a logic one to its bit location.  Check ATmega328P datasheet page 113.
	//printf("1\r\n");
	while (PIN_PERIOD!=0) // Wait for square wave to be 0
	{
		//printf(".");
		if(TIFR1&1)	{ 
			TIFR1=1; 
			overflow++; 
			if(overflow>1024) 
				return 0;
		}
	}
	overflow=0;
	TIFR1=1;
	while (PIN_PERIOD==0) // Wait for square wave to be 1
	{
		//printf("-");
		if(TIFR1&1)	{ 
			TIFR1=1; 
			overflow++; 
			if(overflow>1024) 
				return 0;
		}
	}
	
	overflow=0;
	TIFR1=1;
	saved_TCNT1a=TCNT1;
	for(i=0; i<n; i++) // Measure the time of 'n' periods
	{
		while (PIN_PERIOD!=0) // Wait for square wave to be 0
		{
			if(TIFR1&1)	{ 
				TIFR1=1; 
				overflow++; 
				if(overflow>1024) 
					return 0;
			}
		}
		while (PIN_PERIOD==0) // Wait for square wave to be 1
		{
			if(TIFR1&1)	{ 
				TIFR1=1; overflow++; 
				if(overflow>1024) 
					return 0;
			}
		}
	}
	saved_TCNT1b=TCNT1; // Added an extra overflow.  Get rid of it.

	return overflow*0x10000L+(saved_TCNT1b-saved_TCNT1a);
}
void setting_detect (float period, float bpm) {

	
	float frequency;

	char bpm_buffer[17];

	frequency = 1/period;
	printf("%3.4f\r\n", period);

	LCD_4BIT();
	sprintf(bpm_buffer, "%.4f", bpm);
	LCDprint("Current BPM: ", 1, 1);
	LCDprint(bpm_buffer, 2, 1);
	waitms(15);

	sprintf(bpm_buffer, "%.4f", period);
	LCDprint("Period(s): ", 1, 1);
	LCDprint(bpm_buffer, 2, 1);

	waitms(15);
	sprintf(bpm_buffer, "%.4f", frequency);
	LCDprint("Frequency(Hz): ", 1, 1);
	LCDprint(bpm_buffer, 2, 1);
	waitms(15);
	sprintf(bpm_buffer, "%.4f", bpm);
	LCDprint("Current BPM: ", 1, 1);
	LCDprint(bpm_buffer, 2, 1);
}
int main(void)
{
	long int count;
	float T;
	float bpm;

	usart_init(); // Configure the usart and baudrate
	// uses one hot??
	DDRB  &= 0b11111101; // Configure PB1 as input
	// added by athina
	// configure additional pins as input
	DDRC &= 0b11101111; // configure pin \c4 - 27 - 3
	DDRC &= 0b11110111; // configure pin \c3 - 26 - 2
	DDRC &= 0b11111011; // configure pin \c2 - 25 - 1

	PORTB |= 0b00000010; // Activate pull-up in PB1
	//NEED TO INITIALIZE LCD PINS?
	DDRB|=0b00000001; // PB0 is output.
	DDRD|=0b11111000; // PD3, PD4, PD5, PD6, and PD7 are outputs.

	// Turn on timer with no prescaler on the clock.  We use it for delays and to measure period.
	TCCR1B |= _BV(CS10); // Check page 110 of ATmega328P datasheet

	waitms(500); // Wait for putty to start
	printf("Period measurement using the free running counter of timer 1.\n"
	       "Connect signal to PB1 (pin 15).\n");
	
	while (1)
	{
		
		count=GetPeriod(1);
		if(count>0)
		{
			T=(float)count/(F_CPU);
			bpm = 60.0/T;
			//send period to serial
			
		}
		else
		{
			printf("NO SIGNAL -- %ld                    \r\n", count);
		}

		 setting_detect(T, bpm);

		waitms(200);

	}	

	
}