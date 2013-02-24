
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <curses.h>

#include "etraom.h"

int main( void )
{
	init_game();

	game_loop();
	
	return 0;
}
