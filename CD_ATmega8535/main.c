/*
* 4x4 Keypad / UART / Sound
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
// Sound settings interrupt and pin
#define    OCIE1A   TIMSK_Bit4
#define    I             SREG_Bit7
#define    DDD5      DDRD_Bit5
#define    PD5        PORTD_Bit5

// Doorlock Keyscan speed control
#define SCAN_SPEED      8000
#define ENABLE	             1
#define DISABLE	     0

__flash unsigned char KCODE[16] = {0x00, 0x04, 0x08, 0x0c, 0x01, 0x05, 0x09, 0x0d, 0x02, 0x06, 0x0a, 0x0e, 0x03, 0x07, 0x0b, 0x0f};
__flash unsigned char SPINANGLE[8] = {0x04, 0x0c, 0x08, 0x48, 0x40, 0xc0, 0x80, 0x84};

// LCD ÃÊ±â Ãâ·Â È­¸é
unsigned char msg1[]="**** Hello **** ";
unsigned char msg2[]="PUSH Button Plz ";

// 1.µµ¾î¶ô OPEN LCD Ãâ·Â È­¸é
__flash unsigned char Door_lock1[] = "1-Door Lock Fun?";
__flash unsigned char Door_lock2[] = "2-PASSWORD :    ";

// 2.ÆÐ½º¿öµå ÀÏÄ¡ ¾ð¶ô Ãâ·Â È­¸é
__flash unsigned char Un_lock1[] = " PASSWORD  MATCH";
__flash unsigned char Un_lock2[] = " ** Door OPEN **";

// 3.º¸ÀÏ·¯ LCD Ãâ·Â È­¸é
__flash unsigned char Boiler1[] = "1-Boiler Fun?   ";
__flash unsigned char Boiler2[] = "2-Temperature:  ";

__flash unsigned char KeyChange[] = "PWD Modified";

#include "LCD4.h"
// Á¦¾î º¯¼ö
unsigned char k;
// Door Lock LCD Á¦¾î º¯¼ö
unsigned char door_lcd;

// Rotary Æ÷Æ®BÀÇ ÀÔ·ÂÇÉ ¾îµå·¹½º ¹Þ´Â º¯¼ö¿Í Á¦¾îÇÏ´Â º¯¼ö
unsigned char r;
unsigned char LCD[16] = { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f };
unsigned char temp_changed = '\0';                                		 // Check temperature is changed (To set data to rs232)
unsigned char data = '\0';                                                                // RS232 UART UDR data input / output

unsigned char pwd_check_array = 0;                                 		 // Check password insert array
unsigned char KEY;                                                                          // Save 1 key Row
unsigned char FLAG;                                                                        // Check specific key code
unsigned char KEY2;
unsigned int  delay_count = SCAN_SPEED;                         	         // Delay count for key scan
unsigned char check_password[] = {0x0f, 0x0f, 0x0f, 0x0f}; 		 // Saved password (default is ffff)
unsigned char set_password[] = {0x04, 0x05, 0x06, 0x07};
int passwordWrong; 										 // Check Password (+1 if password is wrong)
int pwd_correct_incorrect = 0;                                                               // Get password_checker(); return value
// ºñ¹Ð¹øÈ£ * Ç¥½Ã Á¦¾î º¯¼ö
int number = 0;
volatile unsigned int sound_count = 0;                                                        // Set count for sound output
#pragma vector = TIMER1_COMPA_vect
__interrupt void COMP_A(void)
{
  sound_count--;
}

unsigned int spinCount, spinStep; 							 // Step Motor counts

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
* unsigned char '0' ~ 'f' : Boiler temperature control
* unsigned char 'g' : Loosen gas valve
* unsigned char 'v' : Fasten gas valve
*/
int avr_sound(int sound_num) {
	TCNT1H = 0;
	TCNT1L = 0;
	TCCR1A = 0x40;
	TCCR1B = 0x09;
	OCIE1A = 1;
	I = 1;
	
	if(sound_num == 0) {                                                // If password is wrong
		for(unsigned char sound_wrong_count = 0; sound_wrong_count < 5; sound_wrong_count++) {
			OCR1AH = 784 >> 8;
			OCR1AL = 784 & 0x00ff;
			sound_count = 294;
			while(sound_count != 0);
			
			OCR1AH = 0xff;
			OCR1AL = 0xff;
			TCCR1A = 0;
			sound_count = 5;
			while(sound_count != 0);
		}										
	} else if(sound_num == 1) {											// If password is correct(As same as door is unlocked)
		OCR1AH = 880 >> 8;
		OCR1AL = 880 & 0x00ff;
		sound_count = 2093;
		while(sound_count != 0);
			
		OCR1AH = 699 >> 8;
		OCR1AL = 699 & 0x00ff;
		sound_count = 1431;
		while(sound_count != 0);
			
		OCR1AH = 587 >> 8;
		OCR1AL = 587 & 0x00ff;
		sound_count = 1568;
		while(sound_count != 0);
		
	} else if(sound_num == 2) {											// If door is locked
		OCR1AH = 699 >> 8;
		OCR1AL = 699 & 0x00ff;
		sound_count = 1431;
		while(sound_count != 0);
		  
		OCR1AH = 880 >> 8;
		OCR1AL = 880 & 0x00ff;
		sound_count = 2093;
		while(sound_count != 0);
		
	} else if(sound_num == 3) {											// When keypad is pressed
		OCR1AH = 440 >> 8;
		OCR1AL = 440 & 0x00ff;
		sound_count = 1570;
		while(sound_count != 0);
	}
	
	OCR1AH = 0xff;
    OCR1AL = 0xff;
    TCCR1A = 0;
    sound_count = 50;
    while(sound_count != 0);
	
	TCCR1B = 0;
	OCIE1A = 0;
	I = 0;
	
	return 0;
}

void doorlock(void) {
  // LCD Display -> Door lock and Password
  // LCD Å¬¸®¾î
  COMMAND(0x01);
  // 1¶óÀÎ µ¥ÀÌÅÍ Ãâ·Â
  COMMAND(0x02);  // Ä¿¼­¸¦ È¨À¸·Î ¼Â
  for (k = 0; k < 16; k++) {
    CHAR_O(Door_lock1[k]);   // µ¥ÀÌÅÍ¸¦ LCD·Î µ¥ÀÌÅÍ Ãâ·Â
  }
  // 2¶óÀÎ µ¥ÀÌÅÍ Ãâ·Â
  COMMAND(0xc0);  // Ä¿¼­¸¦ ¶óÀÎ 2·Î ¼Â
  for (k = 0; k < 16; k++) {
    CHAR_O(Door_lock2[k]);   // µ¥ÀÌÅÍ¸¦ LCD·Î µ¥ÀÌÅÍ Ãâ·Â
  }
  door_lcd=1;
}

void door_unlock(void) {
  // LCD Å¬¸®¾î
  COMMAND(0x01);
  // 1¶óÀÎ µ¥ÀÌÅÍ Ãâ·Â
  COMMAND(0x02);  // Ä¿¼­¸¦ È¨À¸·Î ¼Â
  for (k = 0; k < 16; k++) {
    CHAR_O(Un_lock1[k]);   // µ¥ÀÌÅÍ¸¦ LCD·Î µ¥ÀÌÅÍ Ãâ·Â
  }
  // 2¶óÀÎ µ¥ÀÌÅÍ Ãâ·Â
  COMMAND(0xc0);  // Ä¿¼­¸¦ ¶óÀÎ 2·Î ¼Â
  for (k = 0; k < 16; k++) {
    CHAR_O(Un_lock2[k]);   // µ¥ÀÌÅÍ¸¦ LCD·Î µ¥ÀÌÅÍ Ãâ·Â
  }
  number=0;
}

// Rotate Step Motor 180 left
int spinLeft(void) {
  spinCount = 100;
  spinStep = 6;
  do {
    PORTD = SPINANGLE[spinStep];
    spinStep = spinStep - 2;
    spinStep &= 0x07;
    delay(60000);
  }while(spinCount--);
  
  return 0;
}

// Rotate Step Motor 180 right
int spinRight(void) {
  spinCount = 100;
  spinStep = 0;
  do {
    PORTD = SPINANGLE[spinStep];
    spinStep = spinStep + 2;
    spinStep &= 0x07;
    delay(60000);
  }while(spinCount--);
  
  return 0;
}

int init_rs232(void)
{
  UBRR = 23; 															// UART Baud Rate Register  9600bps in 3.6854MHz
  UCR = 0x18; 															// UART Control Register -> RXEN, TXEN Enable
  
  return 0;
}

unsigned char set_rs232_data(unsigned char data)
{
  if(UDRE)																// Wait until data is received
    UDR = data;															// Transmit data
  
  return 0; 
}

unsigned char get_rs232_data(void)
{
  if(RXC)																// When data recieve complete
    return UDR;															// Get data from UDR
  else
    return 0;
}

int rs232_get_command(unsigned char data)
{
  unsigned char show_char = 0;
  unsigned char tens_digit = 0;
  unsigned char one_digit = 0;
  unsigned char boiler_temp = 0;
  
  if((data >= '0') && (data <= '9')) {                                	// Data 0 ~ 9
    COMMAND(0x01);                                                    	// Clear screen
    COMMAND(0x02);                                                    	// Set cursor to 1st line
    for (show_char = 0; show_char < 16; show_char++) {     				// Show basic strings for boiler
      CHAR_O(Boiler1[show_char]);
    }
    
    COMMAND(0xc0);                                                    	// Set cursor to 2nd line
    for (show_char = 0; show_char < 16; show_char++) {
      CHAR_O(Boiler2[show_char]);
    }
    
    boiler_temp = data - 0x12;                                               // Make tens digit number
    tens_digit = (boiler_temp >> 4) + 0x30;
    
    if((data == 0x38) || (data == 0x39)) {                          	// Make one digit number
      boiler_temp = data - 0x02;                                       	// if data 8 ~ 9
    } else {
      boiler_temp = data + 0x18;                                       	// if data 0 ~ 7
    }
    
    one_digit = (boiler_temp & 0x0f) + 0x30;                       		// if data 0 ~ 1
    if((one_digit >= 0x3a) && (one_digit <= 0x3f)) {            		// if data 2 ~ 7
      one_digit -= 0x0a;
    }
    
    COMMAND(0xce);
    CHAR_O(tens_digit);
    COMMAND(0xcf);
    CHAR_O(one_digit);
    set_rs232_data(data);
    
  } else if((data >= 'a') && (data <= 'f')) {                     		// data a ~ c
    COMMAND(0x01);                                                    	// Clear screen
    COMMAND(0x02);                                                    	// Set cursor to 1st line
    for (show_char = 0; show_char < 16; show_char++) {     				// Show basic strings for boiler
      CHAR_O(Boiler1[show_char]);
    }
    
    COMMAND(0xc0);                                                    	// Set cursor to 2nd line
    for (show_char = 0; show_char < 16; show_char++) {
      CHAR_O(Boiler2[show_char]);
    }
    
    boiler_temp = data - 0x33;                                          // Make tens digit number
    tens_digit = (boiler_temp >> 4) + 0x30;
    
    if((data == 0x61) || (data == 0x62)) {                            	// Make one difit number
      boiler_temp = data + 0x07;                                        // if data a ~ b
    } else {
      boiler_temp = data - 0x03;                                        // if data c ~ f
    }
    one_digit = (boiler_temp & 0x0f) + 0x30;                        
    
    COMMAND(0xce);
    CHAR_O(tens_digit);
    COMMAND(0xcf);
    CHAR_O(one_digit);
    set_rs232_data(data);
  } else {
    switch(data) {
    case 'u':
      // Doorlock unlock
      door_unlock();
      set_rs232_data('u');
      break;
    case 'l':
      // Doorlock lock
      set_rs232_data('l');
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
  }
  
  return 0;
}

// ºñ¹Ð¹øÈ£ **** lcd Ãâ·Â ÇÔ¼ö
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
  // µð½ºÇÃ·¹ÀÌ Å¬¸®¾î
  COMMAND(0x01);
  // 1¶óÀÎ µ¥ÀÌÅÍ Ãâ·Â
  COMMAND(0x02);
  for (i = 0; i < 16; i++) {
    CHAR_O(Boiler1[i]);
  }
  // 2¶óÀÎ µ¥ÀÌÅÍ Ãâ·Â
  COMMAND(0xc0);
  for (i = 0; i < 16; i++) {
    CHAR_O(Boiler2[i]);
  }
  // ¿Âµµ ½ÊÀÇ ÀÚ¸®,ÀÏÀÇ ÀÚ¸® ±âº»°ª Ãâ·Â
  COMMAND(0xce);
  CHAR_O(0x31);
  //CHAR_O(temperature1);
  COMMAND(0xcf);
  CHAR_O(0x38);
  //CHAR_O(temperature2);
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
  
  if(!(passwordWrong)) {                                             // When password wrong (return 0)
    set_rs232_data('w');
    return 0;  
  } else {                                                                  // When password correct (return 1) 
    return 1;
  }
}

// Initialize and check devices
int init_devices(void)
{
  unsigned int delay_time = 60000;
  
  // Check step motor left and right
  spinCount = 10;
  spinStep = 6;
  delay_time = 100;
  do {
    PORTD = SPINANGLE[spinStep];
    spinStep -= 2;
    spinStep &= 0x07;
    while(delay_time--);
  }while(spinCount--);
  
  spinCount = 10;
  spinStep = 0;
  delay_time = 100;
  do {
    PORTD = SPINANGLE[spinStep];
    spinStep += 2;
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
  * DDRD : Step motor (Output PD7 ~ 6, PD3 ~ 2), Deprecate - (Debug LED (Output PD3, PD2)), UART (TXD Output PD1 RXD Input PD0)
  */
  DDRA = 0x0f;
  DDRB = 0x00;
  DDRC = 0xff;
  DDRD = 0xfe;
  init_devices();	                                                        // Initialize and check LED / step motor
  init_rs232();		                                                // Initialize RS232 Communication
  
  L_INIT();			                                                //DISPLAY function for MDA_Multi (LCD4.H)
  DISPLAY();
  
  DDD5 = 1;
  PD5 = 1;
  OCIE1A = 0;
  I = 0;     
  
  do {
    // Get data from UART and command informaions  
    data = get_rs232_data();
    rs232_get_command(data);
    
    if(!X0) {
      door_lcd = 0;
      number = 0;
      pwd_check_array = 0;                                           // Execute only when Key Matrix scan
    }
    
    //1. Doorlock & Step Motor Open Process
    if (X0) {
      //PORTD = 0xff; // X0 Enable Debug LED Off
      
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
          //PORTD = 0xf7;
          pwd_check_array++;
          delay_count = SCAN_SPEED;
          encryption();
          number++;
        }
        
        SCAN2();
        if (!(FLAG == 1)) {
          KEY2 = KCODE[KEY];
          check_password[pwd_check_array] = KEY2;
          //PORTD = 0xfb;
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
            //PORTD = 0xf3;
            pwd_check_array++;
            delay_count = SCAN_SPEED;
            encryption();
            number++;
          } else if(KEY2 == 0x0a) {
            pwd_correct_incorrect = password_checker();
            delay_count = SCAN_SPEED;
          } else if(KEY2 == 0x0e) {
            //PORTD = 0xf3;
            for(unsigned char pwd_set_array = 0; pwd_set_array < 4; pwd_set_array++) {
              set_password[pwd_set_array] = check_password[pwd_set_array];
            }
            pwd_check_array = 0;
            delay_count = SCAN_SPEED;
            
            // LCD Å¬¸®¾î
            COMMAND(0x01);
            // 1¶óÀÎ µ¥ÀÌÅÍ Ãâ·Â
            COMMAND(0x02);  // Ä¿¼­¸¦ È¨À¸·Î ¼Â
            for (k = 0; k < 16; k++) {
              CHAR_O(Door_lock1[k]);                               // µ¥ÀÌÅÍ¸¦ LCD·Î µ¥ÀÌÅÍ Ãâ·Â
            }
            // 2¶óÀÎ µ¥ÀÌÅÍ Ãâ·Â
            COMMAND(0xc0);  // Ä¿¼­¸¦ ¶óÀÎ 2·Î ¼Â
            for (k = 0; k < 16; k++) {
              CHAR_O(KeyChange[k]);                              // µ¥ÀÌÅÍ¸¦ LCD·Î µ¥ÀÌÅÍ Ãâ·Â
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
      
      if(pwd_correct_incorrect == 1) {                              // When password correct (door is unlocked)
			door_unlock();
			pwd_correct_incorrect = avr_sound(1);
			set_rs232_data('u');
      }
    } else if (X1) {                                                      // 2. Door lock & Step Motor Close Process
      spinLeft();                                                           // Fasten gas valve
      avr_sound(2);
      set_rs232_data('v');                                              // Set rs232 data (v)
    } else if (X2) {                                                      // 3.Rotary Switch Boiler Process
      r = PINB;                                                             // rÀÌ¶ó´Â »ó¼ö¿¡ Æ÷Æ®BÀÇ ÀÔ·ÂÇÉ ¾îµå·¹½º¸¦ ³Ö´Â´Ù.
      if(LCD[r&0x0f]==0){                                              // 18µµ = ROTARY B(0)
        COMMAND(0x01);
        boiler();
        delay(65000);
        // while(LCD[r&0x0f]==0);
        // while(PINB==0); or if ¹® ¹Û¿¡ ÀÛ¼º.
      }
      else if(LCD[r&0x0f]==1){ // 19µµ = 1
        COMMAND(0xce);
        CHAR_O(0x31);
        COMMAND(0xcf);
        CHAR_O(0x39);
        delay(65000);
      }
      else if(LCD[r&0x0f]==2){ // 20µµ = 2
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x30);
        delay(65000);
      }
      else if(LCD[r&0x0f]==3){ // 21µµ = 3
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x31);
        delay(65000);
      }
      else if(LCD[r&0x0f]==4){ // 22µµ = 4
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x32);
        delay(65000);
      }
      else if(LCD[r&0x0f]==5){ // 23µµ = 5
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x33);
        delay(65000);
      }
      else if(LCD[r&0x0f]==6){ // 24µµ = 6
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x34);
        delay(65000);
      }
      else if(LCD[r&0x0f]==7){ // 25µµ = 7
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x35);
        delay(65000);
      }
      else if(LCD[r&0x0f]==8){ // 26µµ = 8
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x36);
        delay(65000);
      }
      else if(LCD[r&0x0f]==9){ // 27µµ = 9
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x37);
        delay(65000);
      }
      else if(LCD[r&0x0f]==10){ // 28µµ = A
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x38);
        delay(65000);
      }
      else if(LCD[r&0x0f]==11){ // 29µµ = B
        COMMAND(0xce);
        CHAR_O(0x32);
        COMMAND(0xcf);
        CHAR_O(0x39);
        delay(65000);
      }
      else if(LCD[r&0x0f]==12){ // 30µµ = C
        COMMAND(0xce);
        CHAR_O(0x33);
        COMMAND(0xcf);
        CHAR_O(0x30);
        delay(65000);
      }
      else if(LCD[r&0x0f]==13){ // 31µµ = D
        COMMAND(0xce);
        CHAR_O(0x33);
        COMMAND(0xcf);
        CHAR_O(0x31);
        delay(65000);
      }
      else if(LCD[r&0x0f]==14){ // 32µµ = E
        COMMAND(0xce);
        CHAR_O(0x33);
        COMMAND(0xcf);
        CHAR_O(0x32);
        delay(65000);
      }
      else if(LCD[r&0x0f]==15){ // 33µµ = F
        COMMAND(0xce);
        CHAR_O(0x33);
        COMMAND(0xcf);
        CHAR_O(0x33);
        delay(65000);
      }
      
      if(temp_changed != r) {                                         // Send boiler temperature data to RPi
        if((r >= 0x40) && (r <= 0x49)) {                             // if data 0 ~ 9
          set_rs232_data(r - 0x10);
        } else {                                                            // if data a ~ f
          set_rs232_data(r + 0x17);
        }
      }
      temp_changed = r;                                                // Insert previous boiler temperature
    }
    // 4.Heating Gas Valve On/Off Process 
    else if (X3) {
		spinRight();
		set_rs232_data('g');
    }
  } while (1);
}


