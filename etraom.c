
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <curses.h>

#include "etraom.h"

int main( void )
{
	/*map_t *map = alloc_map( 80, 20 );

	int nc, nr;
	int c = 0;

	//nc = generate_dfa( map, 40, 12, 500, 7, 0 );
	nc = generate_dfa_maze( map, 40, 10, 200, 3 );
	if( nc == 0 ) nc = 200;
	nr = dig_rooms( map, nc/20, 1, 1, 4, 3 );
	if( nr == 0 ) nr = nc/20;

	mvprintw( 0, 0, "Dug %i cells and %i rooms.\n", nc, nr );

	draw_map( map );

	getch();
	
	free_map( map );*/

	init_game();

	game_loop();
	
	return 0;
}
