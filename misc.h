
#include <curses.h>

#include "etraom.h"

void die( char *s )
{
	endwin();
	printf( "Error: %s\n",s );
}
