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

// Doorlock Keyscan speed control
#define SCAN_SPEED      6000
#define ENABLE	        1
#define DISABLE			0

__flash unsigned char KCODE[16] = {0x00, 0x04, 0x08, 0x0c, 0x01, 0x05, 0x09, 0x0d, 0x02, 0x06, 0x0a, 0x0e, 0x03, 0x07, 0x0b, 0x0f};
__flash unsigned char SPINANGLE[8] = {0x10, 0x30, 0x20, 0x60, 0x40, 0xc0, 0x80, 0x90};

// LCD 초기 출력 화면
unsigned char msg1[]="**** Hello **** ";
unsigned char msg2[]="PUSH Button Plz ";

// 1.도어락 OPEN LCD 출력 화면
__flash unsigned char Door_lock1[] = "1-Door Lock Fun?";
__flash unsigned char Door_lock2[] = "2-PASSWORD :    ";

// 2.패스워드 일치 언락 출력 화면
__flash unsigned char Un_lock1[] = " PASSWORD  MATCH";
__flash unsigned char Un_lock2[] = " ** Door OPEN **";

// 3.보일러 LCD 출력 화면
__flash unsigned char Boiler1[] = "1-Boiler Fun?   ";
__flash unsigned char Boiler2[] = "2-Temperature:  ";

__flash unsigned char KeyChange[] = "PWD Modified";

#include "LCD4.h"
// 제어 변수
unsigned char k;
// Door Lock LCD 제어 변수
unsigned char door_lcd;

// Rotary 포트B의 입력핀 어드레스 받는 변수와 제어하는 변수
unsigned char r;
unsigned char LCD[16] = { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f };

unsigned char data = 0; // RS232 UART UDR data input / output

//unsigned char modify_password = 0;
unsigned char pwd_check_array = 0; // check password insert array
unsigned char KEY; // Save 1 key Row
unsigned char FLAG; // Check specific key code
unsigned char KEY2;
unsigned int delay_count = SCAN_SPEED; //delay count for key scan
unsigned char check_password[] = {0x0f, 0x0f, 0x0f, 0x0f}; // Saved password (default is ffff)
unsigned char set_password[] = {0x04, 0x05, 0x06, 0x07};
int passwordWrong; // Check Password (+1 if password is wrong)
// 비밀번호 * 표시 제어 변수
int number = 0;

unsigned int spinCount, spinStep; // Step Motor counts

int delay(unsigned int i) {
  while (i--);
  return 0;
}

/* 
* ====================RS232 control functions====================
* init_rs232 -> Initialize UART
* set_rs232_data -> UART transmission (only send status informations)
* get_rs232_data -> UART receive (get data and do instructions)
* ====================RS232 data variables====================
* unsigned char 'u' : Doorlock unlock 
* unsigned char 'l' : Doorlock lock
* unsigned char 't' : Boiler temp increase
* unsigned char 'b' : Boiler temp decrease
* unsigned char 'g' : Loosen gas valve
* unsigned char 'v' : Fasten gas valve
*/
int init_rs232(void)
{
  UBRR = 23; //UART Baud Rate Register  9600bps in 3.6854MHz
  UCR = 0x18; //UART Control Register -> RXEN, TXEN Enable
  
  return 0;
}

unsigned char set_rs232_data(unsigned char data)
{
  // Wait until data is received
  // Transmit data
  if(UDRE)
    UDR = data;
  
  return 0; 
}

unsigned char get_rs232_data(void)
{
  //  When data recieve complete
  // Get data from UDR
  if(RXC)
    return UDR;
  else
    return 0;
}

int rs232_get_command(unsigned char data)
{
  switch(data) {
  case 'u':
    // Doorlock unlock
    set_rs232_data('u');
    break;
  case 'l':
    // Doorlock lock
    set_rs232_data('l');
    break;
  case 't':
    // Boiler temp increase
    r += 1;
    set_rs232_data('t');
    break;
  case 'b':
    // Boiler temp increase
    r -= 1;
    set_rs232_data('b');
    break;
  case 'g':
    // Loosen gas valve
    spinRight();
    set_rs232_data('g');
    break;
  case 'v':
    // Fasten gas valve
    spinLeft();
    set_rs232_data('v');
    break;
  default:
    asm("nop");
    
  }
  
  return 0;
}

void doorlock(void) {
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
  door_lcd=1;
}

void door_unlock(void) {
  // LCD 클리어
  COMMAND(0x01);
  // 1라인 데이터 출력
  COMMAND(0x02);  // 커서를 홈으로 셋
  for (k = 0; k < 16; k++) {
    CHAR_O(Un_lock1[k]);   // 데이터를 LCD로 데이터 출력
  }
  // 2라인 데이터 출력
  COMMAND(0xc0);  // 커서를 라인 2로 셋
  for (k = 0; k < 16; k++) {
    CHAR_O(Un_lock2[k]);   // 데이터를 LCD로 데이터 출력
  }
  number=0;
}


// 비밀번호 **** lcd 출력 함수
void encryption(void) {
  if(number==0){
    COMMAND(0xcc);
    CHAR_O(0x2A);
  }
  else if(number==1) {
    COMMAND(0xcd);
    CHAR_O(0x2A);
  }
  else if(number==2) {
    COMMAND(0xce);
    CHAR_O(0x2A);
  }
  else { // (number==3)
    COMMAND(0xcf);
    CHAR_O(0x2A);
  }
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
  door_unlock();
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
  delay(6000);
  
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

void SCAN2(void)
{
  unsigned char i, temp, key1;
  KEY = key1 = 4;
  FLAG = 1;
  PORTA = 0xfd;
  delay(6000);
  
  temp = PINA;
  temp = (temp >> 4) | 0xf0;
  // i=0 error?
  for (i=0; i<4; i++) {
    if (!(temp & 0x01)) {
      key1 = KEY; FLAG = 0;
    }
    temp >>= 1;
    KEY++;
  }
  KEY = key1 & 0x0f;
}

unsigned char SCAN3(void)
{
  unsigned char i, temp, key1;
  KEY = key1 = 8;
  FLAG = 1;
  PORTA = 0xfb;
  delay(6000);
  
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

void SCAN4(void)
{
  unsigned char i, temp, key1;
  KEY = key1 = 12;
  FLAG = 1;
  PORTA = 0xf7;
  delay(6000);
  
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

int password_checker(void)
{
  passwordWrong = 0;
  pwd_check_array = 0;
  // Check pwd_check_array 4 times
  for (unsigned char pwd_match_array = 0; pwd_match_array < 4; pwd_match_array++) {
    if(check_password[pwd_match_array] == set_password[pwd_match_array]) {
      passwordWrong = 1;
      PORTD = 0xf7;
      delay(60000);
      PORTD = 0xf7;
      delay(60000);
      PORTD = 0xff;
      delay(60000);
      PORTD = 0xff;
      delay(60000);
    } else {
      passwordWrong = 0;
      break;
    }
  }
  
  // Insert dummy values after checking
  for (unsigned char pwd_match_array = 0; pwd_match_array < 4; pwd_match_array++)  {
    check_password[pwd_check_array] = 0x0f;
  }
  
  // Password correct = 1
  if(!(passwordWrong)) {
    set_rs232_data('e');
    return 0;  
  } else {
    PORTD = 0xf3;
    delay(60000);
    PORTD = 0xf3;
    delay(60000);
    PORTD = 0xfb;
    delay(60000);
    PORTD = 0xfb;
    delay(60000);
    set_rs232_data('u');
    spinRight();  
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
  * DDRC : Character LCD (Output D4~D7 (PC3 ~ PC0)  E,RW,RS (PC7 ~ PC5)), Speaker (Output PC4) 
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
    // Get data from UART and command informaions  
    data = get_rs232_data();
    rs232_get_command(data);
    r = PINB; // r이라는 상수에 포트B의 입력핀 어드레스를 넣는다.
    //1. Doorlock & Step Motor Open Process
    
    if(!X0) {
      door_lcd = 0;
      number = 0;
      pwd_check_array = 0; // Execute only when Key Matrix scan
    }
    
    if (X0) {
      PORTD = 0xff; // X0 Enable Debug LED Off
      
      if(door_lcd==0) {
        doorlock();
      }
      
      /*
      * Doorlock routine
      * modify_password ENABLE -> password modify mode
      * modify_mode : 0 -> normal scan, 1 -> set password mode, 2 -> check password mode
      */
      if(delay_count > 0) 
      {
        delay_count--;
      }
      
      if(delay_count == 0) {
        // normal password check input mode
        SCAN();
        if (!(FLAG == 1)) {
          KEY2 = KCODE[KEY];
          check_password[pwd_check_array] = KEY2;
          PORTD = 0xf7;
          pwd_check_array++;
          delay_count = SCAN_SPEED;
          encryption();
          number++;
        }
        
        SCAN2();
        if (!(FLAG == 1)) {
          KEY2 = KCODE[KEY];
          check_password[pwd_check_array] = KEY2;
          PORTD = 0xfb;
          pwd_check_array++;
          delay_count = SCAN_SPEED;
          encryption();
          number++;
        }
        
        SCAN3();
        if (!(FLAG == 1)) {
          KEY2 = KCODE[KEY];
          if((KEY2 == 0x02) || (KEY2 == 0x06)) {
            check_password[pwd_check_array] = KEY2;
            PORTD = 0xf3;
            pwd_check_array++;
            delay_count = SCAN_SPEED;
            encryption();
            number++;
          } else if(KEY2 == 0x0a) {
            password_checker();
            delay_count = SCAN_SPEED;
          } else if(KEY2 == 0x0e) {
            PORTD = 0xf3;
            for(unsigned char pwd_set_array = 0; pwd_set_array < 4; pwd_set_array++) {
              set_password[pwd_set_array] = check_password[pwd_set_array];
            }
            pwd_check_array = 0;
            delay_count = SCAN_SPEED;
            
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
              CHAR_O(KeyChange[k]);   // 데이터를 LCD로 데이터 출력
            }
            number=0;
          }
        }
        
        SCAN4();
        if (!(FLAG == 1)) {
          KEY2 = KCODE[KEY];
          check_password[pwd_check_array] = KEY2;
          PORTD = 0xf3;
          pwd_check_array++;
          delay_count = SCAN_SPEED;
          encryption();
          number++;
        }	
      }
    } 
    // 2. Door lock & Step Motor Close Process
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


