/*
* Switch / 4x4 Keypad / UART
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
// Switch input pin
#define X0 PINB_Bit4
#define X1 PINB_Bit5
#define X2 PINB_Bit6
#define X3 PINB_Bit7
// RS232 UART setting pin
#define    UDRE     USR_Bit5
#define    TXC      USR_Bit6
#define    RXC      USR_Bit7

__flash char KCODE[16] = {0x00, 0x04, 0x08, 0x0c, 0x01, 0x05, 0x09, 0x0d, 0x02, 0x06, 0x0a, 0x0e, 0x03, 0x07, 0x0b, 0x0f};
__flash char SPINANGLE[8] = {0x10, 0x30, 0x20, 0x60, 0x40, 0xc0, 0x80, 0x90};

// LCD 초기 출력 화면
unsigned char msg1[]="**** Hello **** ";
unsigned char msg2[]="PUSH Button Plz ";

// 1.도어락 OPEN LCD 출력 화면
__flash unsigned char Door_lock1[] = "1-Door Lock Fun?";
__flash unsigned char Door_lock2[] = "2-PASSWORD :    ";

// 3.보일러 LCD 출력 화면
__flash unsigned char Boiler1[] = "1-Boiler Fun?   ";
__flash unsigned char Boiler2[] = "2-Temperature:  ";


#include "LCD4.h"
// Rotary 포트B의 입력핀 어드레스 받는 변수와 제어하는 변수
unsigned char r;
unsigned char LCD[16] = { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f };

// 4자리 키를 순서대로 저장하기 위한 변수
unsigned int insert_array = 0;
// Save 1 key Row
unsigned char KEY;
// Check specific key code
unsigned char FLAG;
unsigned char KEY2;
// Saved password (default is 0000)
unsigned char check_password[4] = "1234";
unsigned char set_password[4] = "0000";
// Check Password (+1 if password is wrong)
int passwordWrong;
// 비밀번호 * 표시 제어 변수
int number = 0;
// Step Motor counts
unsigned int spinCount, spinStep;

// Initialize UART
int init_rs232(void)
{
  UBRR = 23; //UART Baud Rate Register  9600bps in 3.6854MHz
  UCR = 0x18; //UART Control Register -> RXEN, TXEN Enable
  
  return 0;
}

/*
* UART transmission function(informations)
* 'u' : Doorlock unlocked
*/
unsigned char set_rs232_data(unsigned char data)
{
  // Wait until data is received
  // Transmit data
  if(UDRE)
    UDR = data;
  
  return 0; 
}

/*
* UART receive function(instructions)
* 'l' : Doorlock lock
* 
*/
unsigned char get_rs232_data(void)
{
  //  When data recieve complete
  // Get data from UDR
  if(RXC)
    return UDR;
  else
    return 0;
}

int delay(unsigned int i) {
  while (i--);
  return 0;
}

// 비밀번호 **** lcd 출력 함수
int encryption(void) {
  if(number==0){
    COMMAND(0xcc);
    CHAR_O(0x2A);
    delay(65000);
  }
  else if(number==1) {
    COMMAND(0xcd);
    CHAR_O(0x2A);
    delay(65000);
  }
  else if(number==2) {
    COMMAND(0xce);
    CHAR_O(0x2A);
    delay(65000);
  }
  else { // (number==3)
    COMMAND(0xcf);
    CHAR_O(0x2A);
    delay(65000);
  }
  return 0;
}

void boiler(void) {
  unsigned char i;
  // 디스플레이 클리어
  COMMAND(0x01);
  // 1라인 데이터 출력
  COMMAND(0x02);
  for (i = 0; i < 16; i++) {
    CHAR_O(Boiler1[i]);
  }
  // 2라인 데이터 출력
  COMMAND(0xc0);
  for (i = 0; i < 16; i++) {
    CHAR_O(Boiler2[i]);
  }
  // 온도 십의 자리,일의 자리 기본값 출력
  COMMAND(0xce);
  CHAR_O(0x31);
  //CHAR_O(temperature1);
  COMMAND(0xcf);
  CHAR_O(0x38);
  //CHAR_O(temperature2);
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
void SCAN(void)
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
  // Key가 오작동으로 두번 들어가는 것을 막기 위한 delay
  //encryption();
  // 비밀번호 자리 제어 하기 위한 변수
  //number++;
}

void SCAN2(void)
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
  //encryption();
  //number++;
}

unsigned char SCAN3(void)
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
  //encryption();
  //number++;
  
  return KCODE[KEY];
}

void SCAN4(void)
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
  //encryption();
  //number++;
}

int password_checker(void)
{
  passwordWrong = 0;
  // Check insert_array 4 times
  for (char key_array = 0; key_array < 4; key_array++) {
    if (check_password[key_array] != set_password[key_array])
      passwordWrong += 1;
  }
  // Password correct = 1
  if(passwordWrong > 0)
  {
    return 0;  
  }
  else {
    // Transmit info (Doorlock is unlocked)
    //set_rs232_data('u');
    return 1;
  }
}

// Initialize and check devices
int init_devices(void)
{
  unsigned int delay_time = 60000;
  // Check Debug LED
  PORTD = 0xf3;
  while(delay_time--);
  PORTD = PORTD << 1;
  delay_time = 60000;
  while(delay_time--);
  PORTD = PORTD << 1;
  
  // Check step motor left and right
  spinCount = 10;
  spinStep = 7;
  delay_time = 100;
  do {
    PORTD = SPINANGLE[spinStep];
    spinStep--;
    spinStep &= 0x07;
    while(delay_time--);
  }while(spinCount--);
  
  spinCount = 10;
  spinStep = 0;
  delay_time = 100;
  do {
    PORTD = SPINANGLE[spinStep];
    spinStep++;
    spinStep &= 0x07;
    while(delay_time--);
  }while(spinCount--);
  
  return 0;
}

int main(void) {
  /*
  * DDRA : Keypad C0 ~ C3 (Output PA0 ~ PA3), L0 ~ L3 (Input PA4 ~ PA7)
  * DDRB : Rotary Switch (Input PB0 ~ PB3), Switch (Input PB4 ~ PB7) 
  * DDRC : Character LCD (Output (D4~D7) PC0 ~ PC3  (RW,RW,E) PC5 ~ PC7 ), Speaker (Output PC4) 
  * DDRD : Step motor (Output PD7 ~ PD4), Debug LED (Output PD3, PD2), UART (TXD Output PD1 RXD Input PD0)
  */
  DDRA = 0x0f;
  DDRB = 0x00;
  DDRC = 0xff;
  DDRD = 0xfe;
  // Initialize and check LED / step motor
  init_devices();
  // Initialize RS232 Communication
  init_rs232();
  //DISPLAY function for MDA_Multi (LCD4.H)
  L_INIT();
  DISPLAY();
  
  do {
    r = PINB; // r이라는 상수에 포트B의 입력핀 어드레스를 넣는다.
    //1. Doorlock & Step Motor Open Process
    if (X0) {
      unsigned char k;
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
      // Infiniteloop until password_checker correct
      insert_array = 0;
      while (1)
      {
        //e 버튼을 눌렀을 경우 새 비밀번호 입력 화면
        if(SCAN3() == 0x0b)
        {
          // Key가 오작동으로 두번 들어가는 것을 막기 위한 delay
          delay(60000);
          insert_array = 0;
          // a를 누르기 전까지 무한 반복 대기
          while(1)
          {
            SCAN();
            if (!(FLAG == 1)) 
            {
              KEY2 = KCODE[KEY];
              // 새로운 패스워드 입력
              set_password[insert_array] = KEY2;
              // 디버깅용 PORTD 출력
              PORTD = 0xf7;
              insert_array++;
              // 비밀번호 * 출력 함수  
              delay(60000);
              // 디버깅용 PORTD 출력
            }
            
            SCAN2();
            if (!(FLAG == 1)) 
            {
              KEY2 = KCODE[KEY];
              set_password[insert_array] = KEY2;
              PORTD = 0xfc;
              insert_array++;
              delay(60000);
            }
            
            SCAN3();
            if (!(FLAG == 1)) 
            {
              KEY2 = KCODE[KEY];
              set_password[insert_array] = KEY2;
              PORTD = 0xf3;
              insert_array++;
              delay(60000);
            }
            
            SCAN4();
            if (!(FLAG == 1)) 
            {
              KEY2 = KCODE[KEY];
              set_password[insert_array] = KEY2;
              PORTD = 0xff;
              insert_array++;
              delay(60000);
            }
            // a를 누르면 입력 종료
            if(SCAN3() == 0x0a)
              break;
          }
        }
        
        SCAN();
        if (!(FLAG == 1)) 
        {
          KEY2 = KCODE[KEY];
          // Insert password checking arrays.
          check_password[insert_array] = KEY2;
          PORTD = 0xf7;
          insert_array++;
          // Key가 오작동으로 두번 들어가는 것을 막기 위한 delay
          delay(60000);
        }
        
        SCAN2();
        if (!(FLAG == 1)) 
        {
          KEY2 = KCODE[KEY];
          check_password[insert_array] = KEY2;
          PORTD = 0xfc;
          insert_array++;
          delay(60000);
        }
        
        SCAN3();
        if (!(FLAG == 1)) 
        {
          KEY2 = KCODE[KEY];
          check_password[insert_array] = KEY2;
          PORTD = 0xf3;
          insert_array++;
          delay(60000);
        }
        
        SCAN4();
        if (!(FLAG == 1)) 
        {
          KEY2 = KCODE[KEY];
          check_password[insert_array] = KEY2;
          PORTD = 0xff;
          insert_array++;
          delay(60000);
        }
        // Press a to stop input
        if(SCAN3() == 0x0a)
        {
          if(password_checker())
          {
            spinRight();
            //한번만 확인이 되면 그 이후부터는 밖에서 키를 눌러도 동작이 되지 않기 때문에 X0 버튼을 내리기 전까지 break을 통해 동작하지 않음.
            break;
          }
        } 
      }
      // 2. Door lock & Step Motor Close Process
    } 
    else if (X1) {
      spinLeft();
      // 3.Rotary Switch Boiler Process
    } 
    else if (X2) {
      if(LCD[r&0x0f]==0){ // 18도 = ROTARY B(0)
        COMMAND(0x01);
        boiler();
        delay(65000);
        // while(LCD[r&0x0f]==0);
        // while(PINB==0); or if 문 밖에 작성.
      }
      else if(LCD[r&0x0f]==1){ // 19도 = 1
        COMMAND(0xce);
        CHAR_O(0x31);
        COMMAND(0xcf);
        CHAR_O(0x39);
        delay(65000);
      }
      else if(LCD[r&0x0f]==2){ // 20도 = 2
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x30);
        delay(65000);
      }
      else if(LCD[r&0x0f]==3){ // 21도 = 3
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x31);
        delay(65000);
      }
      else if(LCD[r&0x0f]==4){ // 22도 = 4
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x32);
        delay(65000);
      }
      else if(LCD[r&0x0f]==5){ // 23도 = 5
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x33);
        delay(65000);
      }
      else if(LCD[r&0x0f]==6){ // 24도 = 6
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x34);
        delay(65000);
      }
      else if(LCD[r&0x0f]==7){ // 25도 = 7
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x35);
        delay(65000);
      }
      else if(LCD[r&0x0f]==8){ // 26도 = 8
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x36);
        delay(65000);
      }
      else if(LCD[r&0x0f]==9){ // 27도 = 9
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x37);
        delay(65000);
      }
      else if(LCD[r&0x0f]==10){ // 28도 = A
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x38);
        delay(65000);
      }
      else if(LCD[r&0x0f]==11){ // 29도 = B
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x39);
        delay(65000);
      }
      else if(LCD[r&0x0f]==12){ // 30도 = C
        COMMAND(0xce);
        CHAR_O(0x33);
        COMMAND(0xcf);
        CHAR_O(0x30);
        delay(65000);
      }
      else if(LCD[r&0x0f]==13){ // 31도 = D
        COMMAND(0xce);
        CHAR_O(0x33);
        COMMAND(0xcf);
        CHAR_O(0x31);
        delay(65000);
      }
      else if(LCD[r&0x0f]==14){ // 32도 = E
        COMMAND(0xce);
        CHAR_O(0x33);
        COMMAND(0xcf);
        CHAR_O(0x32);
        delay(65000);
      }
      else if(LCD[r&0x0f]==15){ // 33도 = F
        COMMAND(0xce);
        CHAR_O(0x33);
        COMMAND(0xcf);
        CHAR_O(0x33);
        delay(65000);
      }
    }
    // 4.Heating Gas Valve On/Off Process 
    else if (X3) {
      
    }
    else {
      COMMAND(0x01);
      DISPLAY();
      delay(65000);
    }
  } while (1);
}


