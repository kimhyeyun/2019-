/* 좌우로 번갈아 켜는 프로그램 실습 1*/
#include <avr/io.h>
#include <util/delay.h>

typedef unsigned char uc;

int main() {
	uc value = 0x80;
	int direction = 0;

	DDRA = 0xff;

	for (;;)
	{
		PORTA = value;
		_delay_ms(500); // 50ms
		if (value == 0x80) {
			direction = 0;
		}
		else if (value == 0x01) {
			direction = 1;
		}

		// 방향에 맞게 shift 
		if (direction == 0) {
			value = value >> 1;
		}
		else if (direction == 1) {
			value = value << 1;
		}
	}
}
