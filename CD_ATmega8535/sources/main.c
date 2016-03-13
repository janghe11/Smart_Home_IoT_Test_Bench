/*
 * main.c
 *
 *  Created on: 2016. 3. 4.
 *      Author: jang
 */
//#include "io8535.h"
//#include "LCD4.h"
#include <avr/io.h>
#include "set_doorlock.h"

#define X0 PINB4
#define X1 PINB5
#define X2 PINB6
#define X3 PINB7

// 키 1행 입력 저장 Save 1Key Row (from set_doorlock.h)
extern unsigned char KEY;
// 특정 키 검출 변수 Check specific key code (from set_doorlock.h)
extern unsigned char FLAG;
unsigned char KEY2;
// 비밀번호 저장 변수 Save password (default is 0000)
char password[4] = "0000";
char inputPassword[4] = "0000";
int correct = 0;
/*
 * Key Matrix codes for ATmega8535.
 * If you want to compile in Eclipse Ubuntu 14.04,remove "__flash" in front of "unsigned".
 */
/*__flash */unsigned char KCODE[16] = { 0x00, 0x04, 0x08, 0x0c, 0x01, 0x05,
		0x09, 0x0d, 0x02, 0x06, 0x0a, 0x0e, 0x03, 0x07, 0x0b, 0x0f };
// LCD 화면 표시를 위한 변수 선언 (최대 4개 가능) Message variables for character LCD (Maximum 4 variables)
/*__flash */char msg1[] = " Test Message 1";
/*__flash */char msg2[] = " Test Message 2";
///*__flash */char msg3[] = " Test Message 3";
///*__flash */char msg4[] = " Test Message 4";

int delay(unsigned int i) {
	while (i--)
		;
	return 0;
}

// Rotate Step Motor 180 on/off
int valveMotor(void) {
	return 0;
}

int main(int argc, char* argv[]) {
	// 4자리 키를 순서대로 저장하기 위한 변수
	unsigned int keyRotate = 0;
	//DISPLAY function for MDA_Multi (Not working in regular ATmega header files.)
	//L_INIT();
	//DISPLAY();
	/*
	 * DDRA : Keypad L0 ~ L3 (Input PA7 ~ PA4), C3 ~ C0 (Output PA3 ~ PA0)
	 * DDRB : Push button (Input PB7 ~ PB4), Rotary Switch (Output PB3 ~ PB0)
	 * DDRC : Character LCD (Output PC7 ~ PC0)
	 * DDRD : Step motor (Output PD7 ~ PD0)
	 */
	DDRA = 0x0f;
	DDRB = 0x00;
	DDRC = 0xff;
	DDRD = 0xff;
	do {
		//1. Door lock & Step Motor Open Process
		if (X0) {
			// LCD Display -> Door lock and Password

			// a 혹은 e를 검출하기 위해 3번 row 검사
			keyScan3();
			if (!(FLAG == 1)) {
				// a를 누르면 비밀번호 입력 대기 화면
				if (KCODE[KEY] == 0x0a) {
					KCODE[KEY] = 0;
					keyRotate = 0;
					// 다시 a를 누르면 입력 종료
					while (!(KCODE[KEY] == 0x0a) || keyRotate < 4) {
						keyScan1();
						if (!(FLAG == 1)) {
							KEY2 = KCODE[KEY];
						}

						keyScan2();
						if (!(FLAG == 1)) {
							KEY2 = KCODE[KEY];
						}

						keyScan3();
						if (!(FLAG == 1)) {
							KEY2 = KCODE[KEY];
						}

						keyScan4();
						if (!(FLAG == 1)) {
							KEY2 = KCODE[KEY];
						}

						inputPassword[keyRotate] = KEY2;
						keyRotate++;
					}
					// 입력된 키와 저장된 키가 맞는지 검사
					for (keyRotate = 0; keyRotate < 4; keyRotate++) {
						if (password[keyRotate] == inputPassword[keyRotate]) {
							correct = 1;
						} else {
							correct = 0;
							break;
						}
					}
					// e를 누르면 비밀번호 수정 대기 화면
				} else if (KEY2 == 0x0b) {
					KCODE[KEY] = 0;
					keyRotate = 0x0b;
					// 다시 a를 누르면 입력 종료
					while (!(KCODE[KEY] == 0x0b) || keyRotate < 4) {
						keyScan1();
						if (!(FLAG == 1)) {
							KEY2 = KCODE[KEY];
						}

						keyScan2();
						if (!(FLAG == 1)) {
							KEY2 = KCODE[KEY];
						}

						keyScan3();
						if (!(FLAG == 1)) {
							KEY2 = KCODE[KEY];
						}

						keyScan4();
						if (!(FLAG == 1)) {
							KEY2 = KCODE[KEY];
						}

						password[keyRotate] = KEY2;
						keyRotate++;
					}
				}
			}
		// 2. Door lock & Step Motor Close Process
		} else if (X1) {
		// 3.Rotary Switch Boiler Process
		} else if (X2) {
		// 4.Heating Gas Valve On/Off Process
		} else if (X3) {

		}
	} while (1);
}
