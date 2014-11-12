/********* ********* ********* ********* ********* ********* ********* ********* *********

Title:			2014 Q3 iii
Author:			Ang Li
E-mail:			ang#ang.im	(substitute @ for #)
Blog:			http://angli.me/
Github:			https://github.com/leeang/Embedded-System-Design/

********* ********* ********* ********* ********* ********* ********* ********* *********/

/********* Source Code ********* *********/
unsigned int x=0, y=0;
while (1) {
	PORTA = 0x01;	// turn on red LED
	PORTA = 0x11;	// turn on green LED

	for (x=2000; x>=0; x--) {	// delay for 2 microseconds
		y += 2;		// meaningless action so that the code is not optimized out
	}

	PORTA = 0x00;	// turn off red LED and green LED

	for (x=2000; x>=0; x--) {	// delay for 2 microseconds
		y += 2;		// meaningless action so that the code is not optimized out
	}
}
/********* /Source Code *********/

/********* Self-documenting Code ********* *********/
#define ON							0xFF
#define OFF							0x00
#define SET(REGISTER, MASK, VALUE)	PORT = ( (MASK & VALUE) | (REGISTER & ~MASK))
#define RED_LED_MASK				_BV(PA0);
#define GREEN_LED_MASK				_BV(PA4);
#define INFINITE_LOOP				1
#define DELAY_LENGTH				2000

void main() {
	while (INFINITE_LOOP) {
		SET(PORTA, RED_LED_MASK, ON);
		SET(PORTA, GREEN_LED_MASK, ON);
		delay2us();
		SET(PORTA, (RED_LED_MASK | GREEN_LED_MASK), OFF);
		delay2us();
	}
}

void delay2us() {
	unsigned int delayIndex, y=0;
	for (delayIndex=DELAY_LENGTH; delayIndex>=0; delayIndex--) {
		y += 2;
	}
}
/********* /Self-documenting Code *********/