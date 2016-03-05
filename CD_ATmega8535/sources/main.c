/*
 * main.c
 *
 *  Created on: 2016. 3. 4.
 *      Author: jang
 */
#include <avr/io.h>

// Save 1Key Row
unsigned char KEY;
unsigned char KEY2;
// Check specific key code
unsigned char FLAG;
/*
 * Key Matrix codes for ATmega8535.
 * If you want to compile in Eclipse Ubuntu 14.04,remove "__flash" in front of unsigned.
 */
unsigned char KCODE[16] = {0x00, 0x04, 0x08, 0x0c, 0x01, 0x05, 0x09, 0x0d,
		0x02, 0x06, 0x0a, 0x0e, 0x03, 0x07, 0x0b, 0x0f};

// Catch 4x4 Hex Keypad Input
void keyScan(void)
{
	unsigned char i, temp, key1;
	KEY = key1 = 0;
	FLAG = 1;
	PORTA = 0xfe;
	asm ("nop");
	asm ("nop");
	temp = PINA;
	temp = (temp >> 4) | 0xf0;
	for(i = 0; i < 4; i++)
	{
		if(!(temp & 0x01))
		{
			key1 = KEY;
			FLAG = 0;
		}
		temp >> 1;
		KEY++;
	}
	KEY = key1 & 0x0f;
}

// Rotate Step Motor 180 on/off
int valveMotor(void)
{

}

int main(void)
{
	DDRB = 0xff;
	DDRC = 0xff;
	DDRA = 0x0f;
	PORTC = 0xff;
	PORTB = 0xff;
	do {
		keyScan();
		if(!(FLAG == 1))
		{
			KEY2 = KCODE[KEY];
			PORTB = 0xef;
		}
	}while(1);
}
