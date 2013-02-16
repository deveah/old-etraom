
#include <curses.h>

#include "etraom.h"

void init_screen( void )
{
	int i;
	int x, y;

	initscr();
	getmaxyx( stdscr, y, x );
	if( ( y < SCREEN_Y ) || ( x < SCREEN_X ) )
		die( "Terminal too small." );

	keypad( stdscr, TRUE );
	noecho();
	curs_set( 0 );

	if( has_colors() )
	{
		start_color();
		use_default_colors();

		for( i = 0; i < 8; i++ )
			init_pair( i, i, -1 );
	}
	else
		die( "No color support." );
}

void terminate_screen( void )
{
	endwin();
}

void set_color( int f, int attr )
{
	if( attr )
		attron( attr );

	attron( COLOR_PAIR( f ) );
}

void draw_screen( void )
{
	int i, j;
	int c = 0, t = 0;
	map_t *m = entity[0].map;

	clear();

	do_fov();

	for( i = 0; i < m->width; i++ )
		for( j = 0; j < m->height; j++ )
		{
			switch( m->tile[i][j].type )
			{
			case tile_floor:
			case tile_room_floor:
				t = '.';
				c = COLOR_FLOOR;
				break;
			case tile_wall:
				t = '#';
				c = COLOR_WALL;
				break;
			case tile_door_open:
				t = '\'';
				c = COLOR_DOOR;
				break;
			case tile_door_closed:
				t = '+'; 
				c = COLOR_DOOR;
				break;
			}

			if( m->tile[i][j].flags & TILEFLAG_LIT )
			{
				attron( COLOR_PAIR( c ) );
				mvaddch( j, i, t );
			}
			else if( m->tile[i][j].flags & TILEFLAG_SEEN )
			{
				attron( COLOR_PAIR( COLOR_WHITE ) );
				mvaddch( j, i, t );
			}
		}
	
	for( i = 0; i < entity_count; i++ )
	{
		attron( COLOR_PAIR( COLOR_WHITE ) );
		if( entity[i].map == m )
			mvaddch( entity[i].y, entity[i].x, entity[i].face );
	}

	attron( COLOR_PAIR( COLOR_WHITE ) );
	mvprintw( 0, 0, "Here be dragons." );
	mvprintw( SCREEN_Y-2, 0, "$name ($hp/$maxhp)" );
	mvprintw( SCREEN_Y-1, 0, "w: $weapon" );
	mvprintw( SCREEN_Y-1, SCREEN_X/2, "a: $armor" );
}
