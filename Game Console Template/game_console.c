/*************************************************************************
Title:			Game Console Template
Inital Author:	David Jahshan
Extended by:	https://github.com/leeang/Embedded-System-Design
Software:		AVR-GCC
Hardware:		ATMEGA16 @ 8Mhz

DESCRIPTION:
	Main and functions for Game Console basic sample code

*************************************************************************/


#include "game_console.h" 


int main(void)
{
	BAT_LOW_LED(OFF); //Make sure it is off before changing direction
	BAT_LOW_LED_DIR(OUT); //Set BATTERY LED I/Os as outputs.
	UP_BUTTON_DIR(IN); //Set UP_BUTTON I/Os as input.


	while (TRUE)//Master loop always true so always loop
	{

		//Turn on the LED if UP_BUTTON is pressed
		if (UP_BUTTON)
		{
			BAT_LOW_LED(ON);	
		} 	
		else
		{
			BAT_LOW_LED(OFF);
		}

	}

}
