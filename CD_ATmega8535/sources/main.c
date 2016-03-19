/*
 * main / 4x4 Keypad
 *  Created on: 2016. 3. 4.
 *      Author: Taehee Jang
 *
 * LCD / Rotary Switch / Push Button
 *  Created on: 2016. 3. 16.
 *      Author: Jongwoo Jun
 */

#include <avr/io.h>

#define X0 PINB0
#define X1 PINB1
#define X2 PINB2
#define X3 PINB3

char KCODE[16] = {0x00, 0x04, 0x08, 0x0c, 0x01, 0x05, 0x09, 0x0d, 0x02, 0x06, 0x0a, 0x0e, 0x03, 0x07, 0x0b, 0x0f};
char SPINANGLE[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09};


// LCD 초기 출력 화면
unsigned char msg1[]="**** Hello **** ";
unsigned char msg2[]="PUSH Button Plz ";

// 1.도어락 OPEN LCD 출력 화면
unsigned char Door_lock1[] = "1-Door Lock Fun?";
unsigned char Door_lock2[] = "2-PASSWORD :    ";

//#include "LCD4.h"

// LCD for문 출력 변수
unsigned char k;

// 4자리 키를 순서대로 저장하기 위한 변수
unsigned int keyRotate = 0;
// 키 1행 입력 저장 Save 1Key Row (from set_doorlock.h)
unsigned char KEY;
// 특정 키 검출 변수 Check specific key code (from set_doorlock.h)
unsigned char FLAG;
unsigned char KEY2;
// 비밀번호 저장 변수 Save password (default is 0000)
unsigned char password[4] = "0000";
unsigned char inputPassword[4] = "0000";
// 비밀번호 확인 변수 Check Password
int correct = 0;
// Step Motor count를 위한 변수 선언
unsigned int spinCount, spinStep;
/*
 * Key Matrix codes for ATmega8535.
 * If you want to compile in Eclipse Ubuntu 14.04,remove "__flash" in front of "unsigned".
 */

int delay(unsigned int i) {
	while (i--);
	return 0;
}

// Rotate Step Motor 180 left
int spinLeft(void) {
	spinCount = 200;
	spinStep = 7;
	do {
		PORTD = SPINANGLE[spinStep];
		spinStep--;
		spinStep &= 0x07;
		delay(60000);
	}while(spinCount--);

	return 0;
}

// Rotate Step Motor 180 right
int spinRight(void) {
	spinCount = 200;
	spinStep = 0;
	do {
		PORTD = SPINANGLE[spinStep];
		spinStep++;
		spinStep &= 0x07;
		delay(60000);
	}while(spinCount--);

	return 0;
}

// Catch 4x4 Hex Keypad Input
void SCAN()
{
    unsigned char i, temp, key1;
    KEY = key1 = 0;
    FLAG = 1;
    PORTA = 0xfe;
    asm ("nop");
    asm ("nop");

    temp = PINA;
    temp = (temp >> 4) | 0xf0;
        for (i=0; i<4; i++) {
            if (!(temp & 0x01)) {
                key1 = KEY; 
                FLAG = 0;
            }
            temp >>= 1;
            KEY++;
        }
        KEY = key1 & 0x0f;
}

void SCAN2()
{
    unsigned char i, temp, key1;
    KEY = key1 = 4;
    FLAG = 1;
    PORTA = 0xfd;
    asm ("nop");
    asm ("nop");

    temp = PINA;
    temp = (temp >> 4) | 0xf0;
        for (i=0; i<4; i++) {
            if (!(temp & 0x01)) {
                key1 = KEY; FLAG = 0;
            }
            temp >>= 1;
            KEY++;
        }
        KEY = key1 & 0x0f;
}

unsigned char SCAN3()
{
    unsigned char i, temp, key1;
    KEY = key1 = 8;
    FLAG = 1;
    PORTA = 0xfb;
    asm ("nop");
    asm ("nop");

    temp = PINA;
    temp = (temp >> 4) | 0xf0;
        for (i=0; i<4; i++) {
            if (!(temp & 0x01)) {
                key1 = KEY; FLAG = 0;
            }
            temp >>= 1;
            KEY++;
        }
    KEY = key1 & 0x0f;

    return KCODE[KEY];
}

void SCAN4()
{
    unsigned char i, temp, key1;
    KEY = key1 = 12;
    FLAG = 1;
    PORTA = 0xf7;
    asm ("nop");
    asm ("nop");

    temp = PINA;
    temp = (temp >> 4) | 0xf0;
        for (i=0; i<4; i++) {
            if (!(temp & 0x01)) {
                key1 = KEY; FLAG = 0;
            }
            temp >>= 1;
            KEY++;
        }
        KEY = key1 & 0x0f;
}

int passwordCheck()
{
	correct = 0;
	// keyRotate가 4번 돌며 패스워드 검사
	for (keyRotate = 0; keyRotate < 4; keyRotate++) {
		// password가 맞으면 correct를 1씩 증가
		if (password[keyRotate] == inputPassword[keyRotate]) {
			correct += 1;
		} else {
			correct = 0;
		}
	}
	// correct가 4가 되면 1을 반환하고 while 루프를 빠져나옴
	if(correct == 4)
		return 1;
	else
		return 0;
}

int main(void) {
	//DISPLAY function for MDA_Multi (Not working in regular ATmega header files.)
	//L_INIT();
	//DISPLAY();
	/*
	 * DDRA : Keypad L0 ~ L3 (Input PA7 ~ PA4), C3 ~ C0 (Output PA3 ~ PA0)
	 * DDRB : Push button (Input PB7 ~ PB4), Rotary Switch (Output PB3 ~ PB0)
	 * DDRC : Character LCD (Output PC7 ~ PC0)
	 * DDRD : Tx / Rx (Output PD5 Input PD4), Step motor (Output PD3 ~ PD0)
	 */
	DDRA = 0x0f;
    DDRB = 0x00;
	//DDRC = 0xff;
	DDRD = 0xff;

	do {
		//1. Door lock & Step Motor Open Process
		if (X0) {
			// LCD Display -> Door lock and Password

            // LCD 클리어
            //COMMAND(0x01);
            // 1라인 데이터 출력
            //COMMAND(0x02);  // 커서를 홈으로 셋
                for (k = 0; k < 16; k++) {
                    //CHAR_O(Door_lock1[k]);   // 데이터를 LCD로 데이터 출력
                }
            // 2라인 데이터 출력
            //COMMAND(0xc0);  // 커서를 라인 2로 셋
                for (k = 0; k < 16; k++) {
                   // CHAR_O(Door_lock2[k]);   // 데이터를 LCD로 데이터 출력
                }
			// a를 누르면 비밀번호 입력 대기 화면
			keyRotate = 0;
			while (1) 
			{
				//e 버튼을 눌렀을 경우 새 비밀번호 입력 화면
				if(SCAN3() == 0x0b)
				{
					// Key가 오작동으로 두번 들어가는 것을 막기 위한 delay
					delay(60000);
					keyRotate = 0;
					// a를 누르거나 숫자 4개를 입력하기 전까지 무한 반복 대기
					while(1)
					{
						SCAN();
						if (!(FLAG == 1)) 
						{
							KEY2 = KCODE[KEY];
							// Key가 오작동으로 두번 들어가는 것을 막기 위한 delay
							delay(60000);
						}

						SCAN2();
						if (!(FLAG == 1)) 
						{
							KEY2 = KCODE[KEY];
							delay(60000);
						}

						SCAN3();
						if (!(FLAG == 1)) 
						{
							KEY2 = KCODE[KEY];
							delay(60000);
						}

						SCAN4();
						if (!(FLAG == 1)) 
						{
							KEY2 = KCODE[KEY];
							delay(60000);
						}
						// 새로운 패스워드 입력
						password[keyRotate] = KEY2;
						// a를 누르거나 숫자 4개를 입력하면 종료
						if(SCAN3() == 0x0a || keyRotate > 3)
							break;
						else
							keyRotate++;
					}
				}
				
				SCAN();
				if (!(FLAG == 1)) 
				{
					KEY2 = KCODE[KEY];
					// Key가 오작동으로 두번 들어가는 것을 막기 위한 delay
					delay(60000);
				}

				SCAN2();
				if (!(FLAG == 1)) 
				{
					KEY2 = KCODE[KEY];
					delay(60000);
				}

				SCAN3();
				if (!(FLAG == 1)) 
				{
					KEY2 = KCODE[KEY];
					delay(60000);
				}

				SCAN4();
				if (!(FLAG == 1)) 
				{
					KEY2 = KCODE[KEY];
					delay(60000);
				}
				
				inputPassword[keyRotate] = KEY2;
				// a를 누르거나 숫자 4개를 입력하면 입력 종료
				if(SCAN3() == 0x0a || keyRotate > 3)
				{
					// 패스워드 검사
					if(passwordCheck() == 1)
					{
						spinRight();
						//한번만 확인이 되면 그 이후부터는 밖에서 키를 눌러도 동작이 되지 않기 때문에 X0 버튼을 내리기 전까지 break을 통해 동작하지 않음.
						break;
					}
				} 
				else {
					keyRotate++;
				}
            }
		// 2. Door lock & Step Motor Close Process
		} 
		else if (X1) {
			spinLeft();
		// 3.Rotary Switch Boiler Process
		} 
		else if (X2) {
		// 4.Heating Gas Valve On/Off Process
		} 
		else if (X3) {

		}
	} while (1);
}


