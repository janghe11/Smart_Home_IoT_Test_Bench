/*
 * set_doorlock.c
 *
 *  Created on: 2016. 3. 7.
 *      Author: jang
 */
#include <avr/io.h>
#include "set_doorlock.h"

unsigned char FLAG, KEY;

// Catch 4x4 Hex Keypad Input
void keyScan1(void)
{
	unsigned char keyRow = 0;
	unsigned char keyInsert = 0;
	unsigned char keyColumn;

	KEY = keyColumn = 0;
	FLAG = 1;

	// Output keyboard 1row
	PORTA = 0xfe;
	asm ("nop");
	asm ("nop");
	keyInsert = PINA;
	keyInsert = (keyInsert >> 4) | 0xf0;
	for(keyRow = 0; keyRow < 4; keyRow++)
	{
		if(!(keyInsert & 0x01))
		{
			keyColumn = KEY;
			FLAG = 0;
		}
		keyInsert >>= 1;
		KEY++;
	}
	KEY = keyColumn & 0x0f;
}

void keyScan2(void)
{
	unsigned char keyRow = 0;
	unsigned char keyInsert = 0;
	unsigned char keyColumn;

	KEY = keyColumn = 4;
	FLAG = 1;

	// Output keyboard 1row
	PORTA = 0xfd;
	asm ("nop");
	asm ("nop");
	keyInsert = PINA;
	keyInsert = (keyInsert >> 4) | 0xf0;
	for(keyRow = 0; keyRow < 4; keyRow++)
	{
		if(!(keyInsert & 0x01))
		{
			keyColumn = KEY;
			FLAG = 0;
		}
		keyInsert >>= 1;
		KEY++;
	}
	KEY = keyColumn & 0x0f;
}

void keyScan3(void)
{
	unsigned char keyRow = 0;
	unsigned char keyInsert = 0;
	unsigned char keyColumn;

	KEY = keyColumn = 8;
	FLAG = 1;

	// Output keyboard 1row
	PORTA = 0xfb;
	asm ("nop");
	asm ("nop");
	keyInsert = PINA;
	keyInsert = (keyInsert >> 4) | 0xf0;
	for(keyRow = 0; keyRow < 4; keyRow++)
	{
		if(!(keyInsert & 0x01))
		{
			keyColumn = KEY;
			FLAG = 0;
		}
		keyInsert >>= 1;
		KEY++;
	}
	KEY = keyColumn & 0x0f;
}

void keyScan4(void)
{
	unsigned char keyRow = 0;
	unsigned char keyInsert = 0;
	unsigned char keyColumn;

	KEY = keyColumn = 12;
	FLAG = 1;

	// Output keyboard 1row
	PORTA = 0xf7;
	asm ("nop");
	asm ("nop");
	keyInsert = PINA;
	keyInsert = (keyInsert >> 4) | 0xf0;
	for(keyRow = 0; keyRow < 4; keyRow++)
	{
		if(!(keyInsert & 0x01))
		{
			keyColumn = KEY;
			FLAG = 0;
		}
		keyInsert >>= 1;
		KEY++;
	}
	KEY = keyColumn & 0x0f;
}
