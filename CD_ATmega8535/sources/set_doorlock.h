/*
 * set_doorlock.h
 *
 *  Created on: 2016. 3. 7.
 *      Author: jang
 */
// Save 1Key Row (global variable to main.c)
extern unsigned char KEY;
// Check specific key code (global variable to main.c)
extern unsigned char FLAG;
// Catch 4x4 Hex Keypad Input
void keyScan1(void);
void keyScan2(void);
void keyScan3(void);
void keyScan4(void);
