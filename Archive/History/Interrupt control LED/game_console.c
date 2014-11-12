#include "game_console.h"
#include <util/delay.h>


byte LED_flag = OFF;
char cSREG;
float batteryLevel = 0;


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
	Page 69
	MCU Control and Status Register – MCUCSR
	Bit 6 – ISC2: Interrupt Sense Control 2
		If ISC2 is written to one, a rising edge on INT2 activates the interrupt.

	MCUCR = _BV(ISC01) | _BV(ISC00);
	Page 68
	MCU Control Register – MCUCR
	Bit 1, 0 – ISC01, ISC00: Interrupt Sense Control 0 Bit 1 and Bit 0
		11 The rising edge of INT0 generates an interrupt request.
*/
}

void lowLED_Init(void) {
	LOW_LED_STATE(OFF);
	// Make sure it is off before changing direction
	_delay_ms(10);
	LOW_LED_DIR(OUT);
	// Set BATTERY LED I/Os as outputs.
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

void LCD_Tx(char cd, char cData) {
	CDLCD_DIR(OUT);
	CDLCD_STATE(cd);	// Low: COMMAND; High: DATA;

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

byte selectPage(byte page) {
	byte pageAddress = (CMD_PAGE | page);
	LCD_Tx(COMMAND, pageAddress);
	return(TRUE);
}

byte selectColumn(byte column) {
	byte colAddressLSB = (CMD_COL_LSB | (column & 0x0F));
	byte colAddressMSB = (CMD_COL_MSB | (column >> 4));
	LCD_Tx(COMMAND, colAddressLSB);
	LCD_Tx(COMMAND, colAddressMSB);
	return(TRUE);
}

byte LCD_Clean(void) {
	byte page;
	for (page=0x00; page<MAX_PAGE; page++) {

		selectPage(page);

		byte column;
		for (column=0x00; column<MAX_COLUMN; column++) {

			selectColumn(column);
			LCD_Tx(COMMAND, LCD_CLEAN);

		}

	}
	return(TRUE);
}

byte LCD_Ready(void) { 
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

/* --------- USART --------- --------- */
void USART_Init(unsigned int ubrr) {
	UBRRH = (byte)(ubrr>>8);
	UBRRL = (byte)ubrr;
/*
	Set baud rate
	Bit 15 – URSEL: Register Select
		This bit selects between accessing the UBRRH or the UCSRC Register. It is read as zero when reading UBRRH. The URSEL must be zero when writing the UBRRH.

	Bit 11:0 – UBRR11:0: USART Baud Rate Register
		This is a 12-bit register which contains the USART baud rate. The UBRRH contains the four most significant bits, and the UBRRL contains the 8 least significant bits of the USART baud rate. Ongoing transmissions by the transmitter and receiver will be corrupted if the baud rate is changed. Writing UBRRL will trigger an immediate update of the baud rate prescaler.
*/
/*
	USART Control and Status Register A – UCSRA
	
	Bit 1 – U2X: Double the USART Transmission Speed
	This bit only has effect for the asynchronous operation. Write this bit to zero when using syn- chronous operation. Writing this bit to one will reduce the divisor of the baud rate divider from 16 to 8 effectively dou- bling the transfer rate for asynchronous communication.
*/
	UCSRB = _BV(RXEN) | _BV(TXEN);
/*
	Enable receiver and transmitter

	Page 165
	USART Control and Status Register B – UCSRB

	Bit 4 – RXEN: Receiver Enable
		Writing this bit to one enables the USART Receiver. The Receiver will override normal port oper- ation for the RxD pin when enabled. Disabling the Receiver will flush the receive buffer invalidating the FE, DOR, and PE Flags.
	
	Bit 3 – TXEN: Transmitter Enable
		Writing this bit to one enables the USART Transmitter. The Transmitter will override normal port operation for the TxD pin when enabled. The disabling of the Transmitter (writing TXEN to zero) will not become effective until ongoing and pending transmissions are completed, that is, when the transmit Shift Register and transmit Buffer Register do not contain data to be transmitted. When disabled, the transmitter will no longer override the TxD port.

	Bit 2 – UCSZ2: Character Size
		The UCSZ2 bits combined with the UCSZ1:0 bit in UCSRC sets the number of data bits (Char- acter Size) in a frame the receiver and transmitter use.
*/
	UCSRC = _BV(URSEL) | _BV(USBS) | _BV(UCSZ1) | _BV(UCSZ0);
/*
	Set frame format: 8data, 2stop bit
	Set Asynchronous mode, No Parity , 2 Stop Bits

	Page 166
	USART Control and Status Register C – UCSRC

	Bit 7 – URSEL: Register Select
		This bit selects between accessing the UCSRC or the UBRRH Register. It is read as one when reading UCSRC. The URSEL must be one when writing the UCSRC.

	Bit 6 – UMSEL: USART Mode Select
		This bit selects between Asynchronous and Synchronous mode of operation.
		0	Asynchronous Operation
		1	Synchronous Operation

	Bit 3 – USBS: Stop Bit Select
		This bit selects the number of Stop Bits to be inserted by the Transmitter. The Receiver ignores this setting.
		0	1-bit
		1	2-bit

	Bit 2:1 – UCSZ1:0: Character Size
		The UCSZ1:0 bits combined with the UCSZ2 bit in UCSRB sets the number of data bits (Char- acter Size) in a frame the Receiver and Transmitter use.
		011	8-bit
*/
}

void USART_Tx(byte data) {
	while ( !( UCSRA & _BV(UDRE)) );
	/* Wait for empty transmit buffer */
	UDR = data;
	/* Put data into buffer, sends the data */
}

byte USART_Rx(void) {
	while ( !(UCSRA & _BV(RXC)) );
	/* Wait for data to be received */ 
	return UDR;
	/* Get and return received data from buffer */
}
/* --------- /USART --------- */


/* --------- Interrupt Service Routine --------- --------- */
ISR(INT2_vect) {
	if (UP_BUTTON) {
		LED_flag = ON;
	}
	if (DN_BUTTON) {
		LED_flag = ON;
	}
	if (LT_BUTTON) {
		LED_flag = OFF;
	}
	if (RT_BUTTON) {
		LED_flag = OFF;
	}
	
	_delay_ms(255);
}
/* --------- /Interrupt Service Routine --------- */

int main(void) {
	SPI_MasterInit();
	// LCD_Init();
	_delay_ms(200);
	// LCD_Clean();
	// LCD_Ready();
	PWM_Init();
	interruptInit();
	pinInit();
	ADC_Init();
	lowLED_Init();
	USART_Init(103);
	// for 16Mhz and baud 9600 UBRR = 103

	// batteryLevel = getADC() / 1024 * Varef;
	// batteryLevel < BAT_MIN ? (LOW_LED_STATE(ON)) : (LOW_LED_STATE(OFF));

	// OCR0 = FRAM_Read();
	// read the data in the FRAM every time

	while (TRUE) {
		LOW_LED_STATE(LED_flag);
		_delay_ms(1000);
	}

	return(TRUE);
}
