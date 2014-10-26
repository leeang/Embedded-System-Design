/********* ********* ********* ********* ********* ********* ********* ********* *********

Title:			Game Console Header File
Processor:		ATMEGA16 @ 8Mhz
LCD:			EA DOGS102W-6 + EA LED39x41-A (backlight)
LCD Controller:	UC1701x
F-RAM:			FM25L16B

Author:			Ang Li
E-mail:			ang#ang.im	(substitute @ for #)
Blog:			http://blog.leeang.com/
Github:			https://github.com/leeang/Embedded-System-Design

********* ********* ********* ********* ********* ********* ********* ********* *********/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define byte		unsigned char
// typedef unsigned char byte;

#define F_CPU		8000000UL

/* --------- Constants --------- */
#define HIGH		0xFF
#define LOW			0x00

#define TRUE		0xFF
#define FALSE		0x00

#define ON			0xFF
#define OFF			0x00

#define OUT			0xFF
#define IN			0x00

#define DESELECT	0xFF
#define SELECT		0x00

#define DATA		0xFF
#define COMMAND		0x00

// PWM
#define TOP			0xFF
#define BOTTOM		0x00

// LCD
#define MAX_PAGE		8
#define MAX_COLUMN		102
#define PAGE_MASK		0b10110000
#define COL_LSB_MASK	0b00000000
#define COL_MSB_MASK	0b00010000
#define LCD_CLEAR		0x00
#define ROW				32
#define COLUMN			51

// FRAM
#define WREN			0b00000110
#define WRITEFM			0b00000010
#define READFM			0b00000011


/* --------- Write and Read --------- */
#define WRITE(REGISTER, MASK, VALUE)	REGISTER = ((VALUE & MASK) | (REGISTER & ~MASK))
#define READ(PIN, MASK)					PIN & MASK


/* --------- IO Direction --------- */
#define ADC_DIR(DIR)			WRITE(DDRA, _BV(PA0), DIR)
#define LOW_LED_DIR(DIR)		WRITE(DDRA, _BV(PA1), DIR)
#define UP_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA2), DIR)
#define LT_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA3), DIR)
#define DN_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA4), DIR)
#define RT_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA5), DIR)
#define BB_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA6), DIR)
#define AA_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA7), DIR)

#define PWM_DIR(DIR)			WRITE(DDRB, _BV(PB3), DIR)
#define SS_DIR(DIR)				WRITE(DDRB, _BV(PB4), DIR)
#define MOSI_DIR(DIR)			WRITE(DDRB, _BV(PB5), DIR)
#define MISO_DIR(DIR)			WRITE(DDRB, _BV(PB6), DIR)
#define SCK_DIR(DIR)			WRITE(DDRB, _BV(PB7), DIR)

#define RX_DIR(DIR)				WRITE(DDRD, _BV(PD0), DIR)
#define TX_DIR(DIR)				WRITE(DDRD, _BV(PD1), DIR)
#define CSFM_DIR(DIR)			WRITE(DDRD, _BV(PD2), DIR)
#define CSLCD_DIR(DIR)			WRITE(DDRD, _BV(PD3), DIR)
#define RSTLCD_DIR(DIR)			WRITE(DDRD, _BV(PD4), DIR)
#define CDLCD_DIR(DIR)			WRITE(DDRD, _BV(PD5), DIR)
#define HOLDFM_DIR(DIR)			WRITE(DDRD, _BV(PD6), DIR)
#define WPFM_DIR(DIR)			WRITE(DDRD, _BV(PD7), DIR)


/* --------- Outputs --------- */
#define LOW_LED_STATE(STATE)	WRITE(PORTA, _BV(PA1), ~STATE)

#define PWM_STATE(STATE)		WRITE(PORTB, _BV(PB3), STATE)
#define SS_STATE(STATE)			WRITE(PORTB, _BV(PB4), STATE)

#define CSFM_STATE(STATE)		WRITE(PORTD, _BV(PD2), STATE)
#define CSLCD_STATE(STATE)		WRITE(PORTD, _BV(PD3), STATE)
#define RSTLCD_STATE(STATE)		WRITE(PORTD, _BV(PD4), STATE)
#define CDLCD_STATE(STATE)		WRITE(PORTD, _BV(PD5), STATE)
#define HOLDFM_STATE(STATE)		WRITE(PORTD, _BV(PD6), STATE)
#define WPFM_STATE(STATE)		WRITE(PORTD, _BV(PD7), STATE)


/* --------- Inputs --------- */
#define UP_BUTTON				~READ(PINA, _BV(PA2))
#define LT_BUTTON				~READ(PINA, _BV(PA3))
#define DN_BUTTON				~READ(PINA, _BV(PA4))
#define RT_BUTTON				~READ(PINA, _BV(PA5))
#define BB_BUTTON				~READ(PINA, _BV(PA6))
#define AA_BUTTON				~READ(PINA, _BV(PA7))


/* --------- Pull-up --------- */
#define UP_PULL_UP(STATE)		WRITE(PORTA, _BV(PA2), STATE)
#define LT_PULL_UP(STATE)		WRITE(PORTA, _BV(PA3), STATE)
#define DN_PULL_UP(STATE)		WRITE(PORTA, _BV(PA4), STATE)
#define RT_PULL_UP(STATE)		WRITE(PORTA, _BV(PA5), STATE)
#define BB_PULL_UP(STATE)		WRITE(PORTA, _BV(PA6), STATE)
#define AA_PULL_UP(STATE)		WRITE(PORTA, _BV(PA7), STATE)
