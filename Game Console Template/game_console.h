/*************************************************************************
Title:			Game Console Template Header File
Inital Author:	David Jahshan
Extended by:	https://github.com/leeang/Embedded-System-Design
Software:		AVR-GCC
Hardware:		ATMEGA16 @ 8Mhz

DESCRIPTION:
	Macros for Game Console

*************************************************************************/

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define byte unsigned char 

/*ON OFF*/
#define ON 0xFF
#define OFF 0x00
#define HIGH 0xFF
#define LOW 0x00
#define IN 0x00
#define OUT 0xFF
#define ALL 0xFF
#define TRUE 1
#define FALSE 0
#define FORWARD 0x00
#define BACK 0xFF

/*SET and GET MACRO*/
#define SET(PORT,MASK,VALUE) PORT = ((MASK & VALUE) | (PORT & ~MASK))
#define GET(PORT,MASK) PORT & MASK

//Devices Direction MACROs
//#define MULTI_LINES_DIR(DIR) SET(DDR?,(_BV(P?#)|_BV(P?#)|_BV(P?#)),DIR)
#define BAT_LOW_LED_DIR(DIR) SET(DDR?,_BV(P?#),DIR)
#define UP_BUTTON_DIR(DIR) SET(DDR?,_BV(P?#),DIR)

//Devices Outputs
//#define MULTI_LINES(STATE) SET(DDR?,(_BV(P?#)|_BV(P?#)|_BV(P?#)),DIR)

#define BAT_LOW_LED(STATE) SET(PORT?,_BV(P?#),~STATE)

//Devices Inputs
#define UP_BUTTON GET(PIN?,_BV(P?#))
