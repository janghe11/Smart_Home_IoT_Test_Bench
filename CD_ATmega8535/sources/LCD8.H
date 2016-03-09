#define  RS     PORTB_Bit5
#define  RW     PORTB_Bit6
#define  E      PORTB_Bit7
#define  DDB5   DDRB_Bit5
#define  DDB6   DDRB_Bit6
#define  DDB7   DDRB_Bit7
// 지연 함수
void DELAY(unsigned int i)
{
   while(i--);
}
// 비지 플래그 검사
void BUSY(void)
{
    unsigned char temp1;
    DDRC = 0x00;          // C 포트 입력
    temp1 = 0;
    // 비지 플래그가 "0"인 경우 빠져 나온다.
    do{
       RW = 1;   // E = 1, RW = 1, RS = 0
       RS = 0;
       E = 1;
       asm ("nop");
       temp1 = PINC;       // 8비트 입력
       E = 0;
    }while(temp1 & 0x80);  // BUSY = 1인 경우 루프 
    // BUSY = 0인 경우, 원래대로 리턴
    DDRC = 0xff;           // C 포트 출력
    PORTC = 0xff;          // C 포트 모두 "1"
}
// LCD로 코맨드 전송
void COMMAND( unsigned char data)
{
   BUSY();    // 비지 플래그 검사
   // 8비트 출력
   PORTC = data;
   RW = 0;   // E = 1, RW = 0, RS = 0
   RS = 0;
   E = 1;
   asm ("nop");
   E = 0;
} 
// LCD로 1문자 전송
void CHAR_O( unsigned char data)
{
   BUSY();    // 비지 플래그 검사
   // 8비트 출력
   PORTC = data;
   RW = 0;   // E = 1, RW = 0, RS = 1
   RS = 1;
   E = 1;
   asm ("nop");
   E = 0;
} 
// LCD 1-2 라인을 모두 디스플레이
void DISPLAY(void)
{
   unsigned char i;
     // 1라인 데이터 출력
     COMMAND(0x02);        // 커서를 홈으로 셋
     for (i=0; i<16 ; i++){
       CHAR_O(msg1[i]);   // 데이터를 얻어서 LCD로 데이터 출력
      }
     // 2라인 데이터 출력
     COMMAND(0xc0);        // 커서를 라인 2로 셋
     for (i=0; i<16 ; i++){
       CHAR_O(msg2[i]);   // 데이터를 얻어서 LCD로 데이터 출력
      }
}
// LCD 초기 설정
void L_INIT(void)
{
   DDRC = 0xff;             // C 포트 출력
   DDB5 = DDB6 = DDB7 = 1;  // PB7-PB5 포트 출력
   // 포트 초기화
   PORTC = 0x30;
   RW = 0;                  // E = 1, RW = 0, RS = 1
   RS = 0;
   E = 1;
   asm ("nop");
   E = 0;
   // 일정 시간 지연
   DELAY(65000);
   // 펑션 셋 8비트  
   RW = 0;                  // E = 1, RW = 0, RS = 1
   RS = 0;
   E = 1;
   asm ("nop");
   E = 0;
   DELAY(5000);             // 일정 시간 지연
   // 펑션 셋 8비트  
   RW = 0;                  // E = 1, RW = 0, RS = 1
   RS = 0;
   E = 1;
   asm ("nop");
   E = 0;
   DELAY(100);              // 일정 시간 지연
   // 펑션 셋 8비트  
   RW = 0;                  // E = 1, RW = 0, RS = 1
   RS = 0;
   E = 1;
   asm ("nop");
   E = 0;
   DELAY(100);          // 일정 시간 지연
   // 코맨드 설정
   COMMAND(0x38);       // 8비트 모드
   COMMAND(0x08);       // 디스플레이 오프
   COMMAND(0x01);       // 디스플레이 클리어
   COMMAND(0x06);       // 엔트리 모드 셋
   COMMAND(0x0f);       // 디스플레이 온
} 
