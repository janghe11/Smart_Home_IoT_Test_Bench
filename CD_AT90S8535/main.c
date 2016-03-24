?/*
* Switch / 4x4 Keypad
*  Created on: 2016. 3. 4.
*      Author: Taehee Jang
*
* LCD / Rotary Switch
*  Created on: 2016. 3. 16.
*      Author: Jongwoo Jun
* 
* Compiler : IAR
*/

#include "io8535.h"
// 스위치 입력 레지스터
#define X0 PINB_Bit0
#define X1 PINB_Bit1
#define X2 PINB_Bit2
#define X3 PINB_Bit3
// RS232 상태 레지스터
#define    RXC      7 //수신 완료 표시 비트
#define    TXC      6 //송신 완료 표시 비트
#define    UDRE     5 //송신 데이터 레지스터 준비 완료 표시 비트

__flash char KCODE[16] = {0x00, 0x04, 0x08, 0x0c, 0x01, 0x05, 0x09, 0x0d, 0x02, 0x06, 0x0a, 0x0e, 0x03, 0x07, 0x0b, 0x0f};
__flash char SPINANGLE[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08, 0x09};


// LCD 초기 출력 화면
__flash unsigned char msg1[]="**** Hello **** ";
__flash unsigned char msg2[]="PUSH Button Plz ";

// 1.도어락 OPEN LCD 출력 화면
__flash unsigned char Door_lock1[] = "1-Door Lock Fun?";
__flash unsigned char Door_lock2[] = "2-PASSWORD :    ";

#include "LCD4.h"

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
int passwordCorrect;
int passwordWrong;
// Step Motor count를 위한 변수 선언
unsigned int spinCount, spinStep;
/*
* Key Matrix codes for ATmega8535.
* If you want to compile in Eclipse Ubuntu 14.04,remove "__flash" in front of "unsigned".
*/
// RS232 통신 초기화
int init_rs232(void)
{
  UBRR = 23; //UART Baud Rate Register 3.6854MHz일 경우 9600bps
  UCR = 0x18; //UART Control Register -> RXEN, TXEN Enable
  
  return 0;
}

// RS232 데이터 송신 함수
char set_rs232Data(char data)
{
  // 데이터가 들어와서 송신 대기 중일때
  while(!UDRE);
  // 데이터 송신
  UDR = data;
  
  return 0; 
}

// RS232 데이터 수신 함수
char get_rs232Data(void)
{
  // 수신 확인 레지스터에 데이터가 들어왔을 때
  while(!RXC);
  // 데이터 수신
  return UDR;
}

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
  passwordWrong = 0;
  // keyRotate가 4번 돌며 패스워드 검사
  for (keyRotate = 0; keyRotate < 4; keyRotate++) {
    // password가 맞으면 correct를 1로
    if (password[keyRotate] != inputPassword[keyRotate])
      passwordWrong += 1;
  }
  // 패스워드가 틀리면 0을 반환, 맞으면 1 반환
  if(passwordWrong > 0)
    return 0;
  else
    return 1;
}

int main(void) {
  //DISPLAY function for MDA_Multi (Not working in regular ATmega header files.)
  L_INIT();
  DISPLAY();
  /*
  * DDRA : Keypad L0 ~ L3 (Input PA7 ~ PA4), C3 ~ C0 (Output PA3 ~ PA0)
  * DDRB : Switch (Input PB7 ~ PB4), Rotary Switch (Input PB3 ~ PB0)
  * DDRC : Speaker (Output PC7), Character LCD (Output PC6 ~ PC0)
  * DDRD : Debug LED (Output PD7, PD6), Tx / Rx (Output PD5 Input PD4), Step motor (Output PD3 ~ PD0)
  */
  DDRA = 0x0f;
  DDRB = 0x00;
  DDRC = 0xff;
  DDRD = 0xff;
  
  do {
    //1. Door lock & Step Motor Open Process
    if (X0) {
      // X0 Enable Debug LED Off
      PORTD = 0xff;
      // LCD Display -> Door lock and Password
      // LCD 클리어
      COMMAND(0x01);
      // 1라인 데이터 출력
      COMMAND(0x02);  // 커서를 홈으로 셋
      for (k = 0; k < 16; k++) {
        CHAR_O(Door_lock1[k]);   // 데이터를 LCD로 데이터 출력
      }
      // 2라인 데이터 출력
      COMMAND(0xc0);  // 커서를 라인 2로 셋
      for (k = 0; k < 16; k++) {
        CHAR_O(Door_lock2[k]);   // 데이터를 LCD로 데이터 출력
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
            // a를 누르면 입력 종료
            if(SCAN3() == 0x0a)
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
        // a를 누르면 입력 종료
        if(SCAN3() == 0x0a)
        {
          // 패스워드 검사
          if(passwordCheck())
          {
            spinRight();
            //한번만 확인이 되면 그 이후부터는 밖에서 키를 눌러도 동작이 되지 않기 때문에 X0 버튼을 내리기 전까지 break을 통해 동작하지 않음.
            break;
          }
        } 
        else
          keyRotate++;
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


