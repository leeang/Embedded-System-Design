#include "gameConsole.h"
#include <util/delay.h>


char cSREG;
float batteryLevel = 0;


/* --------- Interrupt Service Routine --------- --------- */
ISR(INT2_vect) {
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
/* --------- /Interrupt Service Routine --------- */


/* --------- Initialization --------- --------- */
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
	CSFM_DIR(OUT);

	WPFM_DIR(OUT);
	WPFM_STATE(HIGH);
	// This active low pin prevents write operations to the memory array or the status register.

	HOLDFM_DIR(OUT);
	HOLDFM_STATE(HIGH);
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
*/
}

void batLowLED_Init(void) {
	BAT_LOW_LED(HIGH);
	//Make sure it is off before changing direction

	BAT_LOW_LED_DIR(OUT);
	//Set BATTERY LED I/Os as outputs.
}
/* --------- /Initialization --------- */


/* --------- ADC --------- --------- */
void ADC_Init(void) {
	ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(ADLAR);
/*
	Page 217
	ADC Multiplexer Selection Register - ADMUX

	Bit 7:6 – REFS1:0: Analog Channel and Gain Selection Bits
		11 (Internal 2.56V Voltage Reference with external capacitor at AREF pin)

	Bit 5 – ADLAR: ADC Left Adjust Result
		1 (Write one to ADLAR to left adjust the result)

	Bits 4:0 - MUX4:0: Single Ended Input
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


/* --------- SPI --------- --------- */
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
		1	Enable

	Bit 4 – MSTR: Master/Slave Select
		1	This bit selects Master SPI mode when written to one

	Bits 1, 0 – SPR1, SPR0: SPI Clock Rate Select 1 and 0
		Relationship Between SCK and the Oscillator Frequency
		01	fsck = fosc/16; 
*/
}

void SPI_MasterTransmit(char cData) {
	SPDR = cData;
	// Start transmission
	while(!( SPSR & _BV(SPIF) ));
	// Wait for transmission complete
}
/* --------- /SPI --------- */


/* --------- LCD --------- --------- */
void powerUpLCD(void) {
	RSTLCD_DIR(OUT);
	
	RSTLCD_STATE(LOW);	// Reset (active low)
	_delay_ms(1);

	RSTLCD_STATE(HIGH);
	_delay_ms(5);
}

void LCD_Tx(char CD, char cData) {
	CDLCD_DIR(OUT);
	CDLCD_STATE(CD);	// Low: COMMAND; High: DATA;

	CSLCD_DIR(OUT);
	CSLCD_STATE(LOW);	// Low: LCD is selected

	SPI_MasterTransmit(cData);

	CSLCD_STATE(HIGH);	// High: LCD is released
}

byte LCD_Init(void) {
	powerUpLCD();
/*
	UC1701x Page 12 ~ Page 17
*/
	LCD_Tx(COMMAND, 0b11100010);
	// 15. System Reset
		
	LCD_Tx(COMMAND, 0b01000000);
	// 6. Set Scroll Line

	LCD_Tx(COMMAND, 0b10100000);
	// 13. Set SEG(column) Direction

	LCD_Tx(COMMAND, 0b11001000);
	// 14. Set COM(row) Direction

	LCD_Tx(COMMAND, 0b10100100);
	// 10. Set All Pixel ON

	LCD_Tx(COMMAND, 0b10100110);
	// 11. Set Inverse Display

	_delay_ms(120);

	LCD_Tx(COMMAND, 0b10100010);
	// 17. Set LCD Bias Ratio

	LCD_Tx(COMMAND, 0b00101111);
	// 5. Set Power Control

	LCD_Tx(COMMAND, 0b00100111);
	// 8. Set VLCD Resistor Ratio

	LCD_Tx(COMMAND, 0b10000001);
	// 9. Set Electronic Volume (1st Byte)

	LCD_Tx(COMMAND, 0b00010000);
	// 9. Set Electronic Volume (2nd Byte)

	LCD_Tx(COMMAND, 0b11111010);
	// 25. Set Adv. Program Control 0

	LCD_Tx(COMMAND, 0b10010000);
	// 25. Set Adv. Program Control 0

	LCD_Tx(COMMAND, 0b10101111);
	// 12. Set Display Enable

	return(TRUE);
}

byte selectPage (byte page) {
	byte PageAddress = (CMD_PAGE | page);
	LCD_Tx(COMMAND, PageAddress);
	return(TRUE);
}

byte selectColumn (byte column) {
	byte ColAddressLSB = (CMD_COL_LSB | (column & 0x0F));
	byte ColAddressMSB = (CMD_COL_MSB | (column >> 4));
	LCD_Tx(COMMAND, ColAddressLSB);
	LCD_Tx(COMMAND, ColAddressMSB);
	return(TRUE);
}

byte LCD_Clean(void) {
	for (byte page=0; page<MAX_PAGE; page++) {

		selectPage(page);

		for (byte column=0; column<MAX_COLUMN; column++) {

			selectColumn(column);
			LCD_Tx(COMMAND, LCD_CLEAN);

		}

	}
}

byte LCD_READY(void) { 
	selectPage(PAGE);
	selectColumn(COLUMN);
	LCD_Tx(DATA, PIXEL);
	return(TRUE);
}
/* --------- /LCD --------- */


/* --------- FRAM --------- --------- */
void FRAM_Write(byte bData) {
	CSFM_STATE(LOW);			// Low: Chip is selected
	SPI_MasterTransmit(WREN);	// Set Write Enable Latch

	CSFM_STATE(HIGH);
	CSFM_STATE(LOW);

	SPI_MasterTransmit(WMSB);	// Write memory data MSB
	SPI_MasterTransmit(WLSB);	// Write memory data LSB

	SPI_MasterTransmit(bData);

	CSFM_STATE(HIGH);
}

byte FRAM_Read(void) {
	CSFM_STATE(LOW);			// Low: Chip is selected

	SPI_MasterTransmit(RMSB);	// Read memory data MSB
	SPI_MasterTransmit(RLSB);	// Read memory data LSB

	CSFM_STATE(HIGH);
	
	return SPDR;
}
/* --------- /FRAM --------- */


/* --------- PWM --------- --------- */
void PWM_Init(void) {
	PWM_DIR(OUT);
	TCCR0 = _BV(WGM00) | _BV(COM01) | _BV(COM00) | _BV(WGM01) | _BV(CS00);
/*
	Fast PWM, Set OC0 on compare match, clear OC0 at BOTTOM, clkI/O/8

	Page 83
	Timer/Counter Control Register – TCCR0

	Bit 3, 6 – WGM01:0: Waveform Generation Mode
		11	Fast PWM

	Bit 5:4 –  COM01:0: Compare Match Output Mode
		When the WGM01:0 bits are set to fast PWM mode.
		11	Set OC0 on compare match, clear OC0 at BOTTOM
				OC0, Output Compare Match output: The PB3 pin can serve as an external output for the Timer/Counter0 Compare Match. The PB3 pin has to be configured as an output (DDB3 set (one)) to serve this function. The OC0 pin is also the output pin for the PWM mode timer function.
	Bit 2:0 – CS02:0: Clock Select
		010	clkI/O/8 (From prescaler)
*/
	OCR0 = BOTTOM;
/*
	Page 85
	Output Compare Register – OCR0
	The Output Compare Register contains an 8-bit value that is continuously compared with the counter value (TCNT0). A match can be used to generate an output compare interrupt, or to generate a waveform output on the OC0 pin.
	BOTTOM	The counter reaches the BOTTOM when it becomes 0x00.
*/
}
/* --------- /PWM --------- */