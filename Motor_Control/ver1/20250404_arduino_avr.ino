/*
 * test.c
 *
 * Created: 2025-03-11 오전 10:33:20
 * Author : keyha
 */ 


 #define F_CPU 16000000UL
 #define BAUD 9600
 #define MYUBRR (F_CPU / 16 / BAUD) - 1
 #define START_BYTE 2
 #define END_BYTE 3
 
 #include <avr/io.h>
 #include <util/delay.h>
 #include <stdbool.h>
 #include <avr/interrupt.h>
 
 
 unsigned char length;
 unsigned char way = 'N';
 unsigned char buffer[8];
 volatile unsigned long cnt = 0;
 
 void uart_init(void) {
	 uint16_t  baud = MYUBRR;
	 UBRR0H = (unsigned char)(baud >> 8);
	 UBRR0L = (unsigned char)baud;
	 UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	 UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
 }
 
 unsigned char uartPut(unsigned char data) {
	 while (!(UCSR0A & (1 << UDRE0)));
	 UDR0 = data;
 }
 
 unsigned char uartGet() {
	 while (!(UCSR0A & (1 << RXC0)));
	 return UDR0;
 }
 
 bool receive_data(unsigned char *buffer, unsigned char *length) {
	 if (uartGet() != START_BYTE) { return false; }
	 *length = uartGet();
	 for (unsigned char i = 0; i < *length; i++) {
		 buffer[i] = uartGet();
	 }
	 if (uartGet() != END_BYTE) { return false; }
	 return true;
 }
 
 bool motor_start(unsigned char *buffer) {
	 unsigned char types;
	 unsigned char cw;
	 unsigned char rpm;
	 unsigned short distance;
	 unsigned long pulse;
	 
	 
	 types = buffer[0];
	 rpm = buffer[1];
	 cw = buffer[2];
	 distance = (unsigned short)(buffer[3] << 8 | buffer[4]);
	 // distance = (unsigned int)(buffer[3] << 24 | buffer[4] << 16 | buffer[5] << 8 | buffer[6]);
	 
	 // 방향 설정 파트
	 if (cw == 'D') {
	   PORTB |= (0x10);
	   }
	 else {
	   PORTD &= ~(0x10);
	 }
	 
	 way = cw;
	 
	 if (types == 'L'){
		 pulse = (unsigned long)((distance / 392.55) * 100000UL);
	 }
	 else{
		 pulse = (unsigned long)((distance / 471.0) * 16400UL);
	 }
	 uartPut('B');
   _delay_ms(3);
	 switch(rpm){
		 case 1:
			 for (volatile unsigned long i = 0; i < pulse; i++){
			 PORTD |= 0x40;
			 _delay_us(100);
			 PORTD &= ~(0x40);
			 _delay_us(100);
			 if (UDR0 == 'S') {
				 uartPut('S');
				 PORTB &= ~(0x2);
				 return false;}				
			 }
	   break;
		 case 2:
			 for (volatile unsigned long i = 0; i < pulse; i++){
				 PORTD |= 0x40;
				 _delay_us(60);
				 PORTD &= ~(0x40);
				 _delay_us(60);
				 if (UDR0 == 'S') {
					 uartPut('S');
					 PORTB &= ~(0x2);
				 return false;}
			 }
	   break;
		 case 3:
			 for (volatile unsigned long i = 0; i < pulse; i++){
				 PORTD |= 0x40;
				 _delay_us(60);
				 PORTD &= ~(0x40);
				 _delay_us(60);
				 if (UDR0 == 'S') {
					 uartPut('S');
					 PORTB &= ~(0x2);
				 return false;}
			 }
	   break;
		 case 4:
			 for (volatile unsigned long i = 0; i < pulse; i++){
				 PORTD |= 0x40;
				 _delay_us(60);
				 PORTD &= ~(0x40);
				 _delay_us(60);
				 if (UDR0 == 'S') {
					 uartPut('S');
					 PORTB &= ~(0x2);
				 return false;}
			 }
	   break;
		 case 5:
			 for (volatile unsigned long i = 0; i < pulse; i++){
				 PORTD |= 0x40;
				 _delay_us(60);
				 PORTD &= ~(0x40);
				 _delay_us(60);
				 if (UDR0 == 'S') {
					 uartPut('S');
					 PORTB &= ~(0x2);
				 return false;}
			 }
	 break;
		 }
		 uartPut('E');
		 return false;
 }
 
 int main() {	
	 uart_init();  // UART 초기화
	 DDRD |= 0xC0;  // 6,7번 출력3
	 DDRB |= 0x1B;  // 8,9,11번 출력
	 PORTB &= ~(0x1); // 8번
	 PORTB |= (0x2); // 9번
	 PORTD |= 0x4;  // PD2번 풀업저항 활성화
	 // 인터럽트 모드를 설정 (예: 상승 엣지)
	 EICRA |= (1 << ISC01) | (1 << ISC00);
	 // 외부 인터럽트 0 활성화
	 EIMSK |= (1 << INT0);
	 // 전역 인터럽트 활성화
	 sei();
	 while (true) {
		 if (receive_data(buffer, &length)){
			 motor_start(buffer);
		 }
	 }
 }
 
 ISR(INT0_vect) {
	 cnt++;
	 if (cnt - 100 == 0) {
		 // 10번마다 수행할 작업
		 cnt = 0;
		 uartPut(way);
	 }
 }