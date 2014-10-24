#include "game_console.h"
#include <util/delay.h>

int main(void) {
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

	LOW_LED_STATE(OFF);
	LOW_LED_DIR(OUT);

	byte x = OFF;

	while (TRUE) {
		if (UP_BUTTON || DN_BUTTON || LT_BUTTON || RT_BUTTON) {
			x = ON;
		}
		if (AA_BUTTON || BB_BUTTON) {
			x = OFF;
		}
		LOW_LED_STATE(x);
	}

	return(TRUE);
}
