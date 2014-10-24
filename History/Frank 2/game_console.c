#include "game_console.h"
#include <util/delay.h>
#include <stdlib.h>

#define SELECT		0x00
#define DESELECT	0xFF


char cSREG;

float batteryLevel = 0;

byte frameBuffer[MAX_COLUMN][MAX_PAGE];
byte currentRow = ROW;
byte currentColumn = COLUMN;


byte numArray[10][8] = 
{
	{0x00,0x7C,0xC3,0x81,0x81,0xC3,0x3E,0x00}, /*"0",0*/
	{0x00,0x00,0x82,0x81,0xFF,0x80,0x80,0x00}, /*"1",1*/
	{0x00,0x80,0xC2,0xA1,0xA1,0x91,0x8E,0x00}, /*"2",2*/
	{0x00,0x42,0x89,0x89,0x89,0x76,0x00,0x00}, /*"3",3*/
	{0x20,0x30,0x2C,0x26,0x23,0xFF,0x20,0x00}, /*"4",4*/
	{0x00,0x00,0x4F,0x89,0x89,0x89,0x71,0x00}, /*"5",5*/
	{0x00,0x7C,0xD2,0x89,0x89,0x89,0x70,0x00}, /*"6",6*/
	{0x00,0x01,0x81,0x71,0x1D,0x07,0x01,0x00}, /*"7",7*/
	{0x00,0x66,0x99,0x89,0x89,0x99,0x66,0x00}, /*"8",8*/
	{0x00,0x0E,0x91,0x91,0x91,0x53,0x3E,0x00}, /*"9",9*/
};

byte iconArray[8][8] = 
{
	{0b00000000, 0b00000000, 0b10000001, 0b11111111, 0b11111111, 0b10000001, 0b00000000, 0b00000000}, // 0 "I"
	{0b00011100, 0b00111110, 0b01111111, 0b11111110, 0b11111110, 0b01111111, 0b00111110, 0b00011100}, // 1 Heart
	{0b11111111, 0b11111111, 0b11011011, 0b11011011, 0b11011011, 0b11011011, 0b11011011, 0b11000011}, // 2 "E"
	{0b00100100, 0b01001110, 0b11011111, 0b11011011, 0b11011011, 0b11111011, 0b01110010, 0b00100100}, // 3 "S"
	{0b11111111, 0b11111111, 0b11000011, 0b11000011, 0b11000011, 0b11000011, 0b01111110, 0b00111100}, // 4 "D"
	//{0b11111111, 0b10001001, 0b10001001, 0b10001001, 0b10001001, 0b10001001, 0b10001001, 0b10000001}, // 2 "E"
	//{0b00100100, 0b01001010, 0b10001001, 0b10001001, 0b10001001, 0b10001001, 0b01010010, 0b00100100}, // 3 "S"
	//{0b11111111, 0b10000001, 0b10000001, 0b10000001, 0b10000001, 0b10000001, 0b01000010, 0b00111100}, // 4 "D"
	{0b00111111, 0b01000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b01000000, 0b00111111}, // 5 "U"
	{0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b10000001, 0b11111111, 0b00111100}, // 6 Battery
	{0b10000110, 0b01001001, 0b00101001, 0b00010110, 0b01101000, 0b10010100, 0b10010010, 0b01100001}, // 7 "%"
};


/* --------- Initialization --------- --------- */
void pinInit(void) 
{
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
	
	HOLDFM_DIR(OUT);
	HOLDFM_STATE(HIGH);
}

void interruptInit(void) 
{
	sei();	// Set global interrupt enable
	GICR = _BV(INT1);

	MCUCR = _BV(ISC11) | _BV(ISC10); // Page 68
}

void lowLED_Init(void) 
{
	LOW_LED_STATE(OFF);
	// Make sure it is off before changing direction
	_delay_ms(10);
	LOW_LED_DIR(OUT);
	// Set BATTERY LED I/Os as outputs.
}
/* --------- Initialization --------- --------- */


/* --------- ADC ------------------ */
void ADC_Init(void) 
{
	ADMUX =  _BV(REFS0); // Page 217

	// Bit 4:0 choosing which ACD pin is used. i.e. 00000 = ADC0 (PA0)

	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);;
}

unsigned int getADC(void) 
{
	ADCSRA |= _BV(ADSC);
	// ADC Start Conversion, Prescaler Select
	while(( ADCSRA & (_BV(ADSC)) ));
	// determine the end of conversion
	return(ADC);
}
/* --------- ADC ------------------ */


/* --------- SPI --------- --------- */
void SPI_MasterInit(void) 
{
	MOSI_DIR(OUT);
	SCK_DIR(OUT);

	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0); // Page 140
}

void SPI_MasterTransmit(char cData) 
{
	SPDR = cData;
	// Start transmission
	while(!( SPSR & _BV(SPIF) ));
	// Wait for transmission complete
}
/* --------- SPI --------- --------- */


/* --------- LCD --------- --------- */
void resetLCD(void) 
{
	RSTLCD_DIR(OUT);
	
	RSTLCD_STATE(LOW);	// Reset (active low)
	_delay_ms(1);

	RSTLCD_STATE(HIGH);
	_delay_ms(5);
}

void LCD_Tx(char cd, char cData) 
{
	CDLCD_DIR(OUT);
	CDLCD_STATE(cd);	// Low: COMMAND; High: DATA;

	CSLCD_DIR(OUT);
	CSLCD_STATE(SELECT);

	SPI_MasterTransmit(cData);

	CSLCD_STATE(DESELECT);
}

byte LCD_Init(void) 
{
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

	return(TRUE);
}

byte selectPage(byte page) 
{
	byte pageAddress = (CMD_PAGE | page);
	LCD_Tx(COMMAND, pageAddress);
	return(TRUE);
}

byte selectColumn(byte column) 
{
	column += 30;
	byte colAddressLSB = (CMD_COL_LSB | (column & 0x0F));
	byte colAddressMSB = (CMD_COL_MSB | (column >> 4));
	LCD_Tx(COMMAND, colAddressLSB);
	LCD_Tx(COMMAND, colAddressMSB);
	return(TRUE);
}

byte LCD_Clean(void) 
{
	byte page;
	for (page=0x00; page<MAX_PAGE; page++) 
	{
		selectPage(page);
		byte column;

		for (column=0x00; column<MAX_COLUMN; column++) 
		{
			selectColumn(column);
			LCD_Tx(DATA, LCD_CLEAN);

			frameBuffer[column][page] = 0;
		}

	}
	return(TRUE);
}

byte LCD_Ready(void) 
{
	selectPage(7);
	selectColumn(30);
	LCD_Tx(DATA, 0b00000001);
	return(TRUE);
}

void Draw_Char(int Page, int Col, int No_Char) 
{
	selectPage(Page);
	int i;
	for (i = 0; i < 8; i++)	
	{
		selectColumn(i+Col);
		LCD_Tx(DATA, iconArray[No_Char][i]);
	}
}

void Draw_Num(int Page, int Col, int No_Num) 
{
	selectPage(Page);
	int i;
	for (i = 0; i < 8; i++)	
	{
		selectColumn(i+Col);
		LCD_Tx(DATA, numArray[No_Num][i]);
	}
}

void Draw_Batt_Level(int Batt_Level)
{
	selectPage(0);
	int i;
	for (i = 0; i < 8; i++)
	{
		selectColumn(i+70);
		LCD_Tx(DATA, numArray[Batt_Level][i]);
	}
}
/* --------- LCD --------- --------- */


/* --------- FRAM --------- --------- */
void FRAM_Write(byte bData) 
{
	CSFM_STATE(SELECT);
	SPI_MasterTransmit(WREN);	// Set Write Enable Latch
	SPI_MasterTransmit(WMSB);	// Write memory data MSB
	SPI_MasterTransmit(WLSB);	// Write memory data LSB
	SPI_MasterTransmit(bData);
	CSFM_STATE(DESELECT);
}

byte FRAM_Read(void) 
{
	CSFM_STATE(SELECT);
	SPI_MasterTransmit(RMSB);	// Read memory data MSB
	SPI_MasterTransmit(RLSB);	// Read memory data LSB
	CSFM_STATE(DESELECT);
	
	return SPDR;
}
/* --------- FRAM --------- --------- */


/* --------- PWM --------- --------- */
void PWM_Init(void) 
{
	PWM_DIR(OUT);
	TCCR0 = _BV(WGM00) | _BV(COM01) | _BV(COM00) | _BV(WGM01) | _BV(CS00);  // Page 83

	OCR0 = BOTTOM; // Page 85
}

void Brightness(byte LEVEL)
{
		OCR0 = LEVEL;
}
/* --------- PWM --------- --------- */

void USART_Init(unsigned int ubrr) {
	UBRRH = (byte)(ubrr>>8);
	UBRRL = (byte)ubrr;
	UCSRB = _BV(RXEN) | _BV(TXEN);
	UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);
}
void USART_Tx(byte data) {
	while ( !( UCSRA & _BV(UDRE)) );
	UDR = data;
}


/* --------- Interrupt Service Routine --------- --------- */
ISR(INT1_vect) 
{
	if (UP_BUTTON) 
	{
		currentRow--;
		USART_Tx('U');
	}
	if (DN_BUTTON) 
	{
		currentRow++;
		USART_Tx('D');
	}
	if (LT_BUTTON) 
	{
		currentColumn--;
		USART_Tx('L');
	}
	if (RT_BUTTON) 
	{
		currentColumn++;
		USART_Tx('R');
	}

	byte currentPage  = currentRow / 8;
	byte currentPixel = currentRow % 8;

	byte currentRegister = _BV(currentPixel) | frameBuffer[currentColumn][currentPage];
	frameBuffer[currentColumn][currentPage] = currentRegister;

	selectPage(currentPage);
	selectColumn(currentColumn);
	LCD_Tx(DATA, currentRegister);
	_delay_ms(255);
}
/* --------- /Interrupt Service Routine --------- */


int main(void) {

	WRITE(DDRA, _BV(PA0),  IN);

	WRITE(DDRB, _BV(PB4),  OUT);
	WRITE(PORTB, _BV(PB4), HIGH);


	SPI_MasterInit();
	LCD_Init();
	_delay_ms(200);
	LCD_Clean();
	// LCD_Ready();
	PWM_Init();
	interruptInit();
	pinInit();
	ADC_Init();
	lowLED_Init();
	USART_Init(51);


/*----------/Print First Character----------*/
	Draw_Char(3, 15, 0); // Draw I
	Draw_Char(3, 30, 1); // Draw Heart
	Draw_Char(3, 45, 2); // Draw E
	Draw_Char(3, 60, 3); // Draw S
	Draw_Char(3, 75, 4); // Draw D

	Draw_Char(0, 60, 6); // Draw Battery
	Draw_Char(0, 90, 7); // Draw "%"
	Draw_Num (0, 80, 0); // Draw "0" for Battery
/*----------/Print First Character----------*/



		byte current_level;
			 current_level = BOTTOM;

	while (TRUE) 
	{
		_delay_ms(1000);

    /*----------/Display Battery Level----------*/
/* Batt_Level(max) = (1.5V*1024)/3.3V = 465*/
/* The Boost Converter will not work properly if the battery is less than 1.2V*/
/* We can consider 1.2V as the minimum voltage for a 'useful' battery*/
/* Batt_level(min) = (1.2V*1024)/3.3V = 372*/
/* The difference between the max. and min. is 93*/
/* Evenly divided this into 10 sectors representing 10 different Battery Levels*/
		unsigned int Batt_Level = getADC()/46.5;
		Draw_Batt_Level(Batt_Level);
	/*----------/Display Battery Level----------*/


/*----------/Change Backlignt Level----------*/
		if (AA_BUTTON)
		{
			while(AA_BUTTON)
			{
				LOW_LED_STATE(ON);
			};
			if(current_level == 0) 	  current_level = 200;
			else current_level -= 50;
			Brightness(current_level);
		}

		if(BB_BUTTON)
		{
			while(BB_BUTTON)
			{
				LOW_LED_STATE(ON);
			} 
			if(current_level == 200)  current_level = 0;
			else current_level += 50;
			Brightness(current_level);
		}


		if(getADC()<400)
		{
			LOW_LED_STATE(ON);
		}
		else
		{
			LOW_LED_STATE(OFF);
		}
/*----------/Change Backlignt Level----------*/


	}
	return(TRUE);
}