#include "gameConsole.h"
#include <util/delay.h>

char cSREG;
float batteryLevel = 0;

ISR(INT0_vect) {
	UP_BUTTON && currentRow++;
	DN_BUTTON && currentRow--;
	LT_BUTTON && currentColumn++;
	RT_BUTTON && currentColumn++;
}

ISR(ADC_vect){
	cSREG = SREG;
	// _CLI();

	batteryLevel = ADC / 1024 * Varef;
	batteryLevel < VBAT_MIN ? BAT_LOW_LED(ON) : BAT_LOW_LED(OFF);

	SREG = cSREG;
	
	CLEAR_TIMER_FLAG(ON);
}

void pinInit(void) {
	UP_BUTTON_DIR(IN);
	UP_PULL_UP;

	DN_BUTTON_DIR(IN);
	DN_PULL_UP;

	LT_BUTTON_DIR(IN);
	LT_PULL_UP;

	RT_BUTTON_DIR(IN);
	RT_PULL_UP;

	AA_BUTTON_DIR(IN);
	AA_PULL_UP;

	BB_BUTTON_DIR(IN);
	BB_PULL_UP;

	MISO_DIR(IN);
	csFM_DIR(OUT);

	wpFM_DIR(OUT);
	wpFM_STATE(HIGH);
	// This active low pin prevents write operations to the memory array or the status register.

	holdFM_DIR(OUT);
	holdFM_STATE(HIGH);
}

void interruptInit(void) {
	sei();	// set global interrupt enable
	GICR = _BV(INT2);
/*
	Page 48
	General Interrupt Control Register – GICR
	Bit 5 – INT2: External Interrupt Request 2 Enable
	INT2 – Port B, Bit 2, PB2: External Interrupt Source 2
*/
	MCUCSR = _BV(ISC2);
/*
	Page 41
	MCU Control and Status Register – MCUCSR
	Bit 6 – ISC2: Interrupt Sense Control 2
		If ISC2 is written to one, a rising edge on INT2 activates the interrupt.

	Page 32
	MCU Control Register – MCUCR
	Bit 1, 0 – ISC01, ISC00: Interrupt Sense Control 0 Bit 1 and Bit 0
		11 The rising edge of INT0 generates an interrupt request.
*/
}

/* --------- ADC --------- */
void ADC_Init(void) {
	ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(ADLAR);
/*
	Page 217
	ADC Multiplexer Selection Register - ADMUX

	Bit 7:6 – REFS1:0 Analog Channel and Gain Selection Bits
		11 (Internal 2.56V Voltage Reference with external capacitor at AREF pin)

	Bit 5 – ADLAR ADC Left Adjust Result
		1 (Write one to ADLAR to left adjust the result)

	Bits 4:0 - MUX4:0 Single Ended Input
		00000 (ADC0 Channel 0)
*/
	ADCSRA = _BV(ADEN);
/*
	ADCSRA	ADC Control and Status Register A
	ADEN	ADC Enable
*/
}
unsigned int getADC(void) {
    ADCSRA = _BV(ADSC) | _BV(ADPS1);
    // ADC Start Conversion, Prescaler Select
	while(!( ADCSRA & _BV(ADSC) ));
	// determine the end of conversion
	return(ADCH);
}
/* --------- /ADC --------- */

/* --------- SPI --------- */
void SPI_MasterInit(void) {
	MOSI_DIR(OUT);
	SCK_DIR(OUT);
	// Set MOSI and SCK output, all others input

	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
/*
	Enable SPI, Master, set clock rate fck/16

	Page 140
	SPI Control Register – SPCR
	Bit 6 – SPE: SPI Enable
	Bit 4 – MSTR: Master/Slave Select
		(This bit selects Master SPI mode when written to one)
	Bits 1, 0 – SPR1, SPR0: SPI Clock Rate Select 1 and 0
		Relationship Between SCK and the Oscillator Frequency
		fsck = fosc/16; 
*/
}
void SPI_MasterTransmit(char cData) {
	SPDR = cData;
	// Start transmission
	while(!( SPSR & _BV(SPIF) ));
	// Wait for transmission complete
}
/* --------- /SPI --------- */

/* --------- LCD --------- */
void powerUpLCD(void) {
	rstLCD_DIR(OUT);
	
	rstLCD_STATE(LOW);	// Reset (active low)
	_delay_ms(1);

	rstLCD_STATE(HIGH);
	_delay_ms(5);
}
void LCD_Tx(char CD, char cData) {
	cdLCD_DIR(OUT);
	cdLCD_STATE(CD);	// Low: Command; High: Data;

	csLCD_DIR(OUT);
	csLCD_STATE(LOW);	// Low: LCD is selected

	SPI_MasterTransmit(cData);

	csLCD_STATE(HIGH);	// High: LCD is released
}
/* --------- /LCD --------- */

/* --------- FRAM --------- */
void FRAM_Write(byte bData) {
	csFM_STATE(LOW);			// Low: Chip is selected
	SPI_MasterTransmit(WREN);	// Set Write Enable Latch

	csFM_STATE(HIGH);
	csFM_STATE(LOW);

	SPI_MasterTransmit(WMSB);	// Write memory data MSB
	SPI_MasterTransmit(WLSB);	// Write memory data LSB

	SPI_MasterTransmit(bData);

	csFM_STATE(HIGH);
}

byte FRAM_Read(void) {
	csFM_STATE(LOW);			// Low: Chip is selected

	SPI_MasterTransmit(RMSB);	// Read memory data MSB
	SPI_MasterTransmit(RLSB);	// Read memory data LSB

	csFM_STATE(HIGH);
	
	return SPDR;
}
/* --------- /FRAM --------- */

/* --------- PWM --------- */
void PMW_Init(void) {
	PWM_DIR(OUT);
	TCCR0 = _BV(WGM00) | _BV(COM01) | _BV(COM00) | _BV(WGM01) | _BV(CS00);
/*
	Page 83
	Timer/Counter Control Register – TCCR0

	Bit 3, 6 – WGM01:0: Waveform Generation Mode
		11 Fast PWM

	Bit 5:4 –  COM01:0: Compare Match Output Mode
		When the WGM01:0 bits are set to fast PWM mode.
			11 Set OC0 on compare match, clear OC0 at BOTTOM, (inverting mode)
				OC0, Output Compare Match output: The PB3 pin can serve as an external output for the Timer/Counter0 Compare Match. The PB3 pin has to be configured as an output (DDB3 set (one)) to serve this function. The OC0 pin is also the output pin for the PWM mode timer function.
*/
	OCR0 = BOTTOM;
}
/* --------- /PWM --------- */