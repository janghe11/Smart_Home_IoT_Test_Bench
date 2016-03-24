#define  RS   PORTC_Bit5
#define  RW   PORTC_Bit6
#define  E    PORTC_Bit7
// 지연 함수
void DELAY(unsigned int i)
{
   while(i--);
}
// 비지 플래그 검사
void BUSY(void)
{
    unsigned char temp1;
    DDRC = 0xf0;          // PC0-PC3 입력
    E = 0;
    temp1 = 0;
    // 비지 플래그가 "0"인 경우 빠져 나온다.
    do{
       // 상위 4비트 입력
       RS = 0;
       RW = 1;
       E = 1;
       asm ("nop");
       temp1 = PINC;
       E = 0;
       // 하위 4비트 입력해서 무시한다.
       E = 1;
       asm ("nop");
       E = 0;
    }while(temp1 & 0x08);  // BUSY = 1인 경우 루프 
    // BUSY = 0인 경우, 원래대로 리턴
    DDRC = 0xff;           // C 포트 출력
    E = 0;
}
// LCD로 코맨드 전송
void COMMAND( unsigned char data)
{
   BUSY();    // 비지 플래그 검사
   // 상위 4비트 출력
   PORTC = ((data >> 4) & 0x0f); // RW = 0, RS = 0
   E = 1;
   asm ("nop");
   E = 0;
   // 하위 4비트 출력
   PORTC = (data & 0x0f); // RW = 0, RS = 0
   E = 1;
   asm ("nop");
   E = 0;
} 
// LCD로 1문자 전송
void CHAR_O( unsigned char data)
{
   BUSY();    // 비지 플래그 검사
   // 상위 4비트 출력
   PORTC = ((data >> 4) & 0x0f); // RW = 0
   RS = 1;
   E = 1;
   asm ("nop");
   E = 0;
   // 하위 4비트 출력
   PORTC = (data & 0x0f); // RW = 0
   RS = 1;
   E = 1;
   asm ("nop");
   E = 0;
}
// LCD로 2라인 16문자씩 디스플레이
void DISPLAY(void)
{
   unsigned char i;
    // 1라인 데이터 출력
    for (i=0; i<16 ; i++){
      CHAR_O(msg1[i]);      // 데이터를 LCD로 데이터 출력
     }
   // 2라인 데이터 출력
   COMMAND(0xc0);        // 커서를 라인 2로 셋
    for (i=0; i<16 ; i++){
      CHAR_O(msg2[i]);      // 데이터를 LCD로 데이터 출력
     }
}
// LCD 초기 설정
void L_INIT(void)
{
   DDRC = 0xff;        // PC0-PC7 출력
   E = 0;              // 포트 초기 설정, E = 0
   DELAY(65000);       // 일정 시간 지연
   // 펑션 셋 8비트(하위 4비트만 사용)  
   PORTC = 0x03 & 0x0f;  // RW = 0, RS = 0
   E = 1;
   asm ("nop");
   E = 0;
   DELAY(6000);         // 일정 시간 지연
   E = 1;
   asm ("nop");
   E = 0;
   DELAY(100);          // 일정 시간 지연
   E = 1;
   asm ("nop");
   E = 0;
   DELAY(100);          // 일정 시간 지연
   // 펑션 셋 4비트(하위 4비트만 사용)      
   PORTC = 0x02 & 0x0f;  // RW = 0, RS = 0
   E = 1;
   asm ("nop");
   E = 0;
   DELAY(100);          // 일정 시간 지연
   // 코맨드 출력
   COMMAND(0x28);       // 4비트 모드
   COMMAND(0x08);       // 디스플레이 오프
   COMMAND(0x01);       // 디스플레이 클리어
   COMMAND(0x06);       // 엔트리 모드 셋
   COMMAND(0x0f);       // 디스플레이 온
} 
