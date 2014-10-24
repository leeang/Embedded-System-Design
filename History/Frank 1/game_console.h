#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define byte unsigned char 
// typedef unsigned char byte;

/* --------- Constants --------- */
#define HIGH		0xFF
#define LOW			0x00

#define TRUE		0xFF
#define FALSE		0x00

#define ON			0xFF
#define OFF			0x00

#define OUT			0xFF
#define IN			0x00

#define DATA		0xFF
#define COMMAND		0x00

// ADC
#define Varef		2.56
#define BAT_MIN		1.2

// LCD
#define MAX_PAGE		8
#define LCD_CLEAN		0x00
#define MAX_COLUMN		102
#define CMD_PAGE		0b10110000
#define CMD_COL_LSB		0b00000000
#define CMD_COL_MSB		0b00010000
#define ROW				32
#define COLUMN			51
#define PIXEL			0b10000000
#define PAGE			4

// FRAM
#define WREN			0b00000110
#define WMSB			0b00000010
#define WLSB			0b00000000
#define RMSB			0b00000011
#define RLSB			0b00000000

// PWM
#define BOTTOM			0x00
#define TOP				0xFF


/* --------- Write and Read --------- */
#define WRITE(REGISTER, MASK, VALUE)	REGISTER = ((VALUE & MASK) | (REGISTER & ~MASK))
#define READ(PIN, MASK)					PIN & MASK


/* --------- IO Direction --------- */
#define LOW_LED_DIR(DIR)		WRITE(DDRA, _BV(PA1), DIR)
#define UP_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA2), DIR)
#define LT_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA4), DIR)
#define DN_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA3), DIR)
#define RT_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA5), DIR)
#define BB_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA6), DIR)
#define AA_BUTTON_DIR(DIR)		WRITE(DDRA, _BV(PA7), DIR)

#define PWM_DIR(DIR)			WRITE(DDRB, _BV(PB3), DIR)
#define MOSI_DIR(DIR)			WRITE(DDRB, _BV(PB5), DIR)
#define MISO_DIR(DIR)			WRITE(DDRB, _BV(PB6), DIR)
#define SCK_DIR(DIR)			WRITE(DDRB, _BV(PB7), DIR)

#define RX_DIR(DIR)				WRITE(DDRD, _BV(PD0), DIR)
#define TX_DIR(DIR)				WRITE(DDRD, _BV(PD1), DIR)
#define CSFM_DIR(DIR)			WRITE(DDRB, _BV(PB2), DIR)
#define CSLCD_DIR(DIR)			WRITE(DDRD, _BV(PD4), DIR)
#define RSTLCD_DIR(DIR)			WRITE(DDRD, _BV(PD5), DIR)
#define CDLCD_DIR(DIR)			WRITE(DDRD, _BV(PD6), DIR)
#define HOLDFM_DIR(DIR)			WRITE(DDRB, _BV(PB0), DIR)
#define WPFM_DIR(DIR)			WRITE(DDRB, _BV(PB1), DIR)


/* --------- Outputs --------- */
#define LOW_LED_STATE(STATE)	WRITE(PORTA, _BV(PA1), ~STATE)

#define PWM_STATE(STATE)		WRITE(PORTB, _BV(PB3), STATE)

#define CSFM_STATE(STATE)		WRITE(PORTB, _BV(PB2), STATE)
#define CSLCD_STATE(STATE)		WRITE(PORTD, _BV(PD4), STATE)
#define RSTLCD_STATE(STATE)		WRITE(PORTD, _BV(PD5), STATE)
#define CDLCD_STATE(STATE)		WRITE(PORTD, _BV(PD6), STATE)
#define HOLDFM_STATE(STATE)		WRITE(PORTB, _BV(PB0), STATE)
#define WPFM_STATE(STATE)		WRITE(PORTB, _BV(PB1), STATE)


/* --------- Inputs --------- */
#define UP_BUTTON		~READ(PINA, _BV(PA2))
#define LT_BUTTON		~READ(PINA, _BV(PA4))
#define DN_BUTTON		~READ(PINA, _BV(PA3))
#define RT_BUTTON		~READ(PINA, _BV(PA5))
#define BB_BUTTON		~READ(PINA, _BV(PA6))
#define AA_BUTTON		~READ(PINA, _BV(PA7))


/* --------- Pull-up --------- */
#define UP_PULL_UP		WRITE(PORTA, _BV(PA2), HIGH)
#define LT_PULL_UP		WRITE(PORTA, _BV(PA4), HIGH)
#define DN_PULL_UP		WRITE(PORTA, _BV(PA3), HIGH)
#define RT_PULL_UP		WRITE(PORTA, _BV(PA5), HIGH)
#define BB_PULL_UP		WRITE(PORTA, _BV(PA6), HIGH)
#define AA_PULL_UP		WRITE(PORTA, _BV(PA7), HIGH)
