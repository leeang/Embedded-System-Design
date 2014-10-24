#include <util/delay.h>

#define byte unsigned char 
// typedef unsigned char byte;

/* --------- Constants --------- */
#define HIGH	0xFF
#define LOW		0x00

#define TRUE	0xFF
#define FALSE	0x00

#define ON		0xFF
#define OFF		0x00

#define OUT		0xFF
#define IN		0x00

#define DATA	0xFF
#define COMMAND	0x00

#define Varef	2.56
#define BAT_MIN	1.2


/* --------- Write and Read --------- */
#define WRITE(REGISTER, MASK, VALUE) REGISTER = ((VALUE & MASK) | (REGISTER & ~MASK))
#define READ(PIN, MASK) PIN & MASK


/* --------- IO Direction --------- */
#define BAT_LOW_LED_DIR(DIR)	WRITE(DDRA, _BV(PA1), DIR)
#define UP_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA2), DIR)
#define LT_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA3), DIR)
#define DN_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA4), DIR)
#define RT_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA5), DIR)
#define BB_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA6), DIR)
#define AA_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA7), DIR)

#define PWM_DIR(DIR)			WRITE(DDRB, _BV(PB3), DIR)
#define MOSI_DIR(DIR)			WRITE(DDRB, _BV(PB5), DIR)
#define MISO_DIR(DIR)			WRITE(DDRB, _BV(PB6), DIR)
#define SCK_DIR(DIR)			WRITE(DDRB, _BV(PB7), DIR)

#define RX_DIR(DIR)				WRITE(DDRD, _BV(PD0), DIR)
#define TX_DIR(DIR)				WRITE(DDRD, _BV(PD1), DIR)
#define csFM_DIR(DIR)			WRITE(DDRD, _BV(PD2), DIR)
#define csLCD_DIR(DIR)			WRITE(DDRD, _BV(PD3), DIR)
#define rstLCD_DIR(DIR)			WRITE(DDRD, _BV(PD4), DIR)
#define cdLCD_DIR(DIR)			WRITE(DDRD, _BV(PD5), DIR)
#define holdFM_DIR(DIR)			WRITE(DDRD, _BV(PD6), DIR)
#define wpFM_DIR(DIR)			WRITE(DDRD, _BV(PD7), DIR)


/* --------- Outputs --------- */
#define BAT_LOW_LED(STATE)		WRITE(PORTA, _BV(PA6), ~STATE)

#define PWM_STATE(STATE)		WRITE(PORTB, _BV(PB3), STATE)

#define csFM_STATE(STATE)		WRITE(PORTD, _BV(PD2), STATE)
#define csLCD_STATE(STATE)		WRITE(PORTD, _BV(PD3), STATE)
#define rstLCD_STATE(STATE)		WRITE(PORTD, _BV(PD4), STATE)
#define cdLCD_STATE(STATE)		WRITE(PORTD, _BV(PD5), STATE)
#define holdFM_STATE(STATE)		WRITE(PORTD, _BV(PD6), STATE)
#define wpFM_STATE(STATE)		WRITE(PORTD, _BV(PD7), STATE)


/* --------- Inputs --------- */
#define UP_BUTTON		~READ(PINA, _BV(PA2))
#define LT_BUTTON		~READ(PINA, _BV(PA3))
#define DN_BUTTON		~READ(PINA, _BV(PA4))
#define RT_BUTTON		~READ(PINA, _BV(PA5))
#define BB_BUTTON		~READ(PINA, _BV(PA6))
#define AA_BUTTON		~READ(PINA, _BV(PA7))


/* --------- Pull-up --------- */
#define UP_PULL_UP		WRITE(PORTA, _BV(PA2), HIGH)
#define LT_PULL_UP		WRITE(PORTA, _BV(PA3), HIGH)
#define DN_PULL_UP		WRITE(PORTA, _BV(PA4), HIGH)
#define RT_PULL_UP		WRITE(PORTA, _BV(PA5), HIGH)
#define BB_PULL_UP		WRITE(PORTA, _BV(PA6), HIGH)
#define AA_PULL_UP		WRITE(PORTA, _BV(PA7), HIGH)