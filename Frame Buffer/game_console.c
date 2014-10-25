/********* ********* ********* ********* ********* ********* ********* ********* *********

Title:			Game Console
Processor:		ATMEGA16 @ 8Mhz
LCD:			EA DOGS102W-6 + EA LED39x41-A (backlight)
LCD Controller:	UC1701x
F-RAM:			FM25L16B

Author:			Ang Li
E-mail:			ang#ang.im	(substitute @ for #)
Blog:			http://blog.leeang.com/
Github:			https://github.com/leeang/Embedded-System-Design

********* ********* ********* ********* ********* ********* ********* ********* *********/

#include "game_console.h"
#include <util/delay.h>


byte frameBuffer[MAX_COLUMN][MAX_PAGE];
int currentRow = ROW;
int currentColumn = COLUMN;
byte currentPage, currentPixel, currentRegister;
byte keepPixel = FALSE;

byte luminance = BOTTOM;


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

	SS_DIR(OUT);
	SS_STATE(HIGH);
}

void lowLED_Init(void) {
	LOW_LED_STATE(OFF);
	// Make sure it is off before changing direction
	_delay_ms(10);
	LOW_LED_DIR(OUT);
	// Set BATTERY LED I/Os as outputs.
}
/* --------- /Initialization --------- */


/* --------- Interrupt --------- --------- */
void buttonInterruptInit(void) {
	GICR = _BV(INT2);
/*
	Page 48
	General Interrupt Control Register - GICR
	Bit 7 - INT1: External Interrupt Request 1 Enable
	Bit 6 - INT0: External Interrupt Request 0 Enable
	Bit 5 - INT2: External Interrupt Request 2 Enable
	
	PB2 INT2 (External Interrupt 2 Input)
	PD3 INT1 (External Interrupt 1 Input)
	PD2 INT0 (External Interrupt 0 Input)
*/
	MCUCSR = _BV(ISC2);
/*
	MCUCSR = _BV(ISC2);
	Page 69
	MCU Control and Status Register - MCUCSR
	Bit 6 - ISC2: Interrupt Sense Control 2
		1	If ISC2 is written to one, a rising edge on INT2 activates the interrupt.

	MCUCR = _BV(ISC11) | _BV(ISC10);
	Page 68
	MCU Control Register - MCUCR
	Bit 3, 2 - ISC11, ISC10: Interrupt Sense Control 1 Bit 1 and Bit 0
		11	The rising edge of INT1 generates an interrupt request

	MCUCR = _BV(ISC01) | _BV(ISC00);
	Page 68
	MCU Control Register - MCUCR
	Bit 1, 0 - ISC01, ISC00: Interrupt Sense Control 0 Bit 1 and Bit 0
		11	The rising edge of INT0 generates an interrupt request.
*/
}

void timerInterruptInit(void) {
	TIMSK = _BV(TOIE1);
/*
	Page 115
	Timer/Counter Interrupt Mask Register - TIMSK
	
	Bit 2 - TOIE1: Timer/Counter1, Overflow Interrupt Enable
	When this bit is written to one, and the I-flag in the Status Register is set (interrupts globally enabled), the Timer/Counter1 Overflow Interrupt is enabled. The corresponding Interrupt Vector is executed when the TOV1 Flag, located in TIFR, is set.
*/
	TCCR1B = _BV(CS12) | _BV(CS10);
/*
	Page 113
	Timer/Counter1 Control Register B - TCCR1B

	Bit 2:0 - CS12:0: Clock Select
		101	clkI/O/1024 (From prescaler)
*/
	TCNT1 = 57723;
}
/* --------- /Interrupt --------- */


/* --------- ADC --------- --------- */
void analogReadInit(void) {
	ADMUX = _BV(REFS0);
/*
	Page 217
	ADC Multiplexer Selection Register - ADMUX

	Bit 7:6 - REFS1:0: Analog Channel and Gain Selection Bits
		01	
		11	Internal 2.56V Voltage Reference with external capacitor at AREF pin

	Bit 5 - ADLAR: ADC Left Adjust Result
		0	
		1	Write one to ADLAR to left adjust the result

	Bits 4:0 - MUX4:0: Single Ended Input
		00000	ADC0 Channel 0
*/
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
/*
	ADCSRA	ADC Control and Status Register A
	ADEN	ADC Enable
*/
}

unsigned int analogRead(void) {
	ADCSRA |= _BV(ADSC);
	// ADC Start Conversion, Prescaler Select
	while( ADCSRA & _BV(ADSC) );
	// determine the end of conversion
	return ADC;
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
	SPI Control Register - SPCR
	Bit 6 - SPE: SPI Enable
		1	Enable

	Bit 4 - MSTR: Master/Slave Select
		1	This bit selects Master SPI mode when written to one

	Bits 1, 0 - SPR1, SPR0: SPI Clock Rate Select 1 and 0
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
void resetLCD(void) {
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
	CSLCD_STATE(SELECT);

	SPI_MasterTransmit(cData);

	CSLCD_STATE(DESELECT);
}

void LCD_Init(void) {
	resetLCD();
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

	_delay_ms(32);

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
}

void selectPage(byte page) {
	byte pageAddress = (PAGE_MASK | (page & 0x0F));
	LCD_Tx(COMMAND, pageAddress);
}

void selectColumn(byte column) {
	column += 30;
	byte colAddressLSB = (COL_LSB_MASK | (column & 0x0F));
	byte colAddressMSB = (COL_MSB_MASK | (column >> 4));
	LCD_Tx(COMMAND, colAddressLSB);
	LCD_Tx(COMMAND, colAddressMSB);
}

void LCD_Clear(void) {
	byte page;
	for (page=0; page<MAX_PAGE; page++) {

		selectPage(page);

		byte column;
		for (column=0; column<MAX_COLUMN; column++) {

			selectColumn(column);
			LCD_Tx(DATA, LCD_CLEAR);

		}

	}
}

void LCD_Ready(void) {
	selectPage(7);
	selectColumn(30);
	LCD_Tx(DATA, 0b00000001);
}
/* --------- /LCD --------- */


/* --------- FRAM --------- --------- */
void FRAM_Init(void) {
	MISO_DIR(IN);

	CSFM_DIR(OUT);
	CSFM_STATE(DESELECT);

	WPFM_DIR(OUT);
	WPFM_STATE(HIGH);
	// This active low pin prevents write operations to the memory array or the status register.

	HOLDFM_DIR(OUT);
	HOLDFM_STATE(HIGH);
	// When /HOLD is low, the current operation is suspended.
}

void FRAM_Write(byte bData) {
	CSFM_STATE(SELECT);
	SPI_MasterTransmit(WREN);	// Set Write Enable Latch
	CSFM_STATE(DESELECT);

	CSFM_STATE(SELECT);
	SPI_MasterTransmit(WRITEFM);
	SPI_MasterTransmit(0x00);	// 3 MSBs of address 
	SPI_MasterTransmit(0x00);	// 8 LSBs of address
	SPI_MasterTransmit(bData);
	CSFM_STATE(DESELECT);
}

byte FRAM_Read(void) {
	CSFM_STATE(SELECT);
	SPI_MasterTransmit(READFM);
	SPI_MasterTransmit(0x00);	// 3 MSBs of address 
	SPI_MasterTransmit(0x00);	// 8 LSBs of address

	SPI_MasterTransmit(0x00);
	byte bData = SPDR;
	
	CSFM_STATE(DESELECT);
	return bData;
}
/* --------- /FRAM --------- */


/* --------- PWM --------- --------- */
void PWM_Init(void) {
	PWM_DIR(OUT);
	TCCR0 = _BV(WGM00) | _BV(COM01) | _BV(COM00) | _BV(WGM01) | _BV(CS01);
/*
	Fast PWM, Set OC0 on compare match, clear OC0 at BOTTOM, clkI/O/8

	Page 83
	Timer/Counter Control Register - TCCR0

	Bit 3, 6 - WGM01:0: Waveform Generation Mode
		11	Fast PWM

	Bit 5:4 - COM01:0: Compare Match Output Mode
		When the WGM01:0 bits are set to fast PWM mode.
		11	Set OC0 on compare match, clear OC0 at BOTTOM. (inverting mode)
				OC0, Output Compare Match output: The PB3 pin can serve as an external output for the Timer/Counter0 Compare Match. The PB3 pin has to be configured as an output (DDB3 set (one)) to serve this function. The OC0 pin is also the output pin for the PWM mode timer function.

	Bit 2:0 - CS02:0: Clock Select
		010	clkI/O/8 (From prescaler)
*/
	OCR0 = BOTTOM;
/*
	Page 85
	Output Compare Register - OCR0
	The Output Compare Register contains an 8-bit value that is continuously compared with the counter value (TCNT0). A match can be used to generate an output compare interrupt, or to generate a waveform output on the OC0 pin.
	BOTTOM	The counter reaches the BOTTOM when it becomes 0x00.
*/

/*
	PB3 OC0		(Timer/Counter0 OutputCompare Match Output)
	PD7 OC2		(Timer/Counter2 Output Compare Match Output)
	PD5 OC1A	(Timer/Counter1 Output Compare A Match Output)
	PD4 OC1B	(Timer/Counter1 Output Compare B Match Output)
*/
}
/* --------- /PWM --------- */

/* --------- USART --------- --------- */
void USART_Init(unsigned int ubrr) {
	UBRRH = (byte)(ubrr>>8);
	UBRRL = (byte)ubrr;
/*
	Set baud rate
	Bit 15 - URSEL: Register Select
		This bit selects between accessing the UBRRH or the UCSRC Register. It is read as zero when reading UBRRH. The URSEL must be zero when writing the UBRRH.

	Bit 11:0 - UBRR11:0: USART Baud Rate Register
		This is a 12-bit register which contains the USART baud rate. The UBRRH contains the four most significant bits, and the UBRRL contains the 8 least significant bits of the USART baud rate. Ongoing transmissions by the transmitter and receiver will be corrupted if the baud rate is changed. Writing UBRRL will trigger an immediate update of the baud rate prescaler.
*/
/*
	USART Control and Status Register A - UCSRA
	
	Bit 1 - U2X: Double the USART Transmission Speed
	This bit only has effect for the asynchronous operation. Write this bit to zero when using syn- chronous operation. Writing this bit to one will reduce the divisor of the baud rate divider from 16 to 8 effectively dou- bling the transfer rate for asynchronous communication.
*/
	UCSRB = _BV(RXEN) | _BV(TXEN);
/*
	Enable receiver and transmitter

	Page 165
	USART Control and Status Register B - UCSRB

	Bit 4 - RXEN: Receiver Enable
		Writing this bit to one enables the USART Receiver. The Receiver will override normal port oper- ation for the RxD pin when enabled. Disabling the Receiver will flush the receive buffer invalidating the FE, DOR, and PE Flags.
	
	Bit 3 - TXEN: Transmitter Enable
		Writing this bit to one enables the USART Transmitter. The Transmitter will override normal port operation for the TxD pin when enabled. The disabling of the Transmitter (writing TXEN to zero) will not become effective until ongoing and pending transmissions are completed, that is, when the transmit Shift Register and transmit Buffer Register do not contain data to be transmitted. When disabled, the transmitter will no longer override the TxD port.

	Bit 2 - UCSZ2: Character Size
		The UCSZ2 bits combined with the UCSZ1:0 bit in UCSRC sets the number of data bits (Char- acter Size) in a frame the receiver and transmitter use.
*/
	UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);
/*
	Set frame format: 8data, 2stop bit
	Set Asynchronous mode, No Parity , 2 Stop Bits

	Page 166
	USART Control and Status Register C - UCSRC

	Bit 7 - URSEL: Register Select
		This bit selects between accessing the UCSRC or the UBRRH Register. It is read as one when reading UCSRC. The URSEL must be one when writing the UCSRC.

	Bit 6 - UMSEL: USART Mode Select
		This bit selects between Asynchronous and Synchronous mode of operation.
		0	Asynchronous Operation
		1	Synchronous Operation

	Bit 3 - USBS: Stop Bit Select
		This bit selects the number of Stop Bits to be inserted by the Transmitter. The Receiver ignores this setting.
		0	1-bit
		1	2-bit

	Bit 2:1 - UCSZ1:0: Character Size
		The UCSZ1:0 bits combined with the UCSZ2 bit in UCSRB sets the number of data bits (Char- acter Size) in a frame the Receiver and Transmitter use.
		011	8-bit
*/
}

byte USART_Tx(byte data) {
	while ( !( UCSRA & _BV(UDRE)) );
	/* Wait for empty transmit buffer */
	UDR = data;
	/* Put data into buffer, sends the data */
	return TRUE;
}

void USART_TxString(char *s) {
	while (*s) {
		USART_Tx(*s);
		s++;
	}
}

byte USART_Rx(void) {
	while ( !(UCSRA & _BV(RXC)) );
	/* Wait for data to be received */ 
	return UDR;
	/* Get and return received data from buffer */
}
/* --------- /USART --------- */

void delay(int n) {
	int i;
	for (i=0; i<n; i++) {
		_delay_ms(1);
	}
}

int constrain(int x, int a, int b) {
	if (x<a) {
		return a;
	}
	if (x>b) {
		return b;
	}
	return x;
}

/* --------- Interrupt Service Routine --------- --------- */
ISR(INT2_vect) {
	char columnChange = 0;
	char pageChange = 0;
	
	if (UP_BUTTON || DN_BUTTON || LT_BUTTON || RT_BUTTON) {
		if (keepPixel) {
			frameBuffer[currentColumn][currentPage] = currentRegister;
		}
		
		if (UP_BUTTON) {
			if (currentRow%8 == 0) {
				pageChange--;
			}
			currentRow--;
		} else if (DN_BUTTON) {
			currentRow++;
			if (currentRow%8 == 0) {
				pageChange++;
			}
		} else if (LT_BUTTON) {
			currentColumn--;
			columnChange--;
		} else if (RT_BUTTON) {
			currentColumn++;
			columnChange++;
		}

		currentColumn =  constrain(currentColumn, 0, MAX_COLUMN - 1);
		currentRow =  constrain(currentRow, 0, 64 - 1);

		currentPage = currentRow / 8;
		currentPixel = currentRow % 8;

		currentRegister = _BV(currentPixel) | frameBuffer[currentColumn][currentPage];
		selectPage(currentPage);
		selectColumn(currentColumn);
		LCD_Tx(DATA, currentRegister);

		if (columnChange != 0 || pageChange != 0) {
			byte changeRegister = frameBuffer[currentColumn-columnChange][currentPage-pageChange];
			selectPage(currentPage-pageChange);
			selectColumn(currentColumn-columnChange);
			LCD_Tx(DATA, changeRegister);
		}
	}

	if (AA_BUTTON) {
		keepPixel = ~keepPixel;
	}

	if (BB_BUTTON) {
		luminance += 51;
		OCR0 = luminance;
	}

	_delay_ms(32);
}
/* --------- /Interrupt Service Routine --------- */

int main(void) {
	pinInit();
	SPI_MasterInit();
	LCD_Init();
	_delay_ms(32);
	LCD_Clear();
	PWM_Init();
	buttonInterruptInit();
	sei();	// set global interrupt enable

	currentPage = currentRow / 8;
	currentPixel = currentRow % 8;
	currentRegister = _BV(currentPixel);

	while (TRUE) {
		
	}

	return TRUE;
}
