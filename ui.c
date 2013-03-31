
#include <string.h>
#include <curses.h>

#include "etraom.h"

char* etraom_logo[] = {
	" @@@@@@@@ @@@@@@@ @@@@@@@   @@@@@@   @@@@@@  @@@@@@@@@@ ",
	"@@!        @@!   @@!  @@@ @@!  @@@ @@!  @@@ @@! @@! @@! ",
	"@!!!:!     @!!   @!@!!@!  @!@!@!@! @!@  !@! @!! !!@ @!@ ",
	"!!:        !!:   !!: :!!  !!:  !!! !!:  !!! !!:     !!: ",
	": :: :::    :     :   : :  :   : :  : :. :   :      :   ",
	"",
	"             http://github.com/deveah/etraom "
};

char *inventory_mode_text[] = {
	"View",
	"Drop",
	"Use",
	"Equip",
	"Consume"
};

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
			else if( ( m->tile[i][j].flags & TILEFLAG_SEEN ) &&
				m->tile[i][j].type != tile_floor )
			{
				attron( COLOR_PAIR( COLOR_WHITE ) );
				mvaddch( j, i, t );
			}
		}
	
	for( i = 0; i < link_count; i++ )
	{
		if( link[i].map == m )
		{
			if( m->tile[link[i].x][link[i].y].flags & TILEFLAG_SEEN )
			{
				attron( COLOR_PAIR( COLOR_WHITE ) );
				mvaddch( link[i].y, link[i].x, link[i].face );
			}
			if( m->tile[link[i].x][link[i].y].flags & TILEFLAG_LIT )
			{
				attron( COLOR_PAIR( COLOR_YELLOW ) );
				mvaddch( link[i].y, link[i].x, link[i].face );
			}
		}
	}
	
	for( i = 0; i < item_count; i++ )
	{
		if(	( item[i].map == m ) &&
			( item[i].location == loc_floor ) &&
			( m->tile[item[i].x][item[i].y].flags & TILEFLAG_LIT ) )
		{
			attron( COLOR_PAIR( COLOR_RED ) );
			mvaddch( item[i].y, item[i].x, item[i].face );
		}
	}

	for( i = 0; i < entity_count; i++ )
	{
		attron( COLOR_PAIR( COLOR_WHITE ) );
		if( entity[i].map == m )
			mvaddch( entity[i].y, entity[i].x, entity[i].face );
	}


	attron( COLOR_PAIR( COLOR_WHITE ) );
	mvprintw( SCREEN_Y-2, 0, "$name ($hp/$maxhp)" );
	mvprintw( SCREEN_Y-1, 0, "w: $weapon" );
	mvprintw( SCREEN_Y-1, SCREEN_X/2, "a: $armor" );

	for( i = 0; i < message_count; i++ )
	{
		if( message[i].time == turn_count )
		{
			mvprintw( 0, 0, message[i].s );
		}
	}
}

void inventory_screen( int mode )
{
	int i;
	int count = 0;
	item_t *item;
	int k;

	for( i = 0; i < entity[0].inventory_item_count; i++ )
	{
		item = entity[0].inventory[i];

		mvprintw( 2+i, 0, " %c -   %s", 'a'+i, item->name );
		attron( COLOR_PAIR( COLOR_RED ) );
		mvaddch( 2+i, 5, item->face );

		attron( COLOR_PAIR( COLOR_WHITE ) );
		if( item->flags & ITEMFLAG_STACKABLE )
			mvprintw( 2+i, 8+strlen( item->name ), "(%i)", item->quantity );

		count += entity[0].inventory[i]->quantity;
	}	

	mvprintw( 1, 1, "Inventory (%i item%s) - %s", count,
		count != 1 ? "s" : "", inventory_mode_text[mode] );
	
	k = getch();

	switch( mode )
	{
	case mode_view:
		return;
	case mode_drop:
		drop_item( &entity[0], k-'a' );
		break;
	default:
		break;
	}
}

void title_screen( void )
{
	int i, j;
	int cx, cy;

	cx = ( SCREEN_X - strlen( etraom_logo[0] ) ) / 2;
	cy = ( SCREEN_Y - 7 ) / 2;

	for( j = 0; j < 7; j++ )
		for( i = 0; i < (int)strlen( etraom_logo[j] ); i++ )
		{
			attroff( A_BOLD );

			switch( etraom_logo[j][i] )
			{
			case '@':
				attron( COLOR_PAIR( COLOR_GREEN ) );
				break;
			case '!':
				attron( COLOR_PAIR( COLOR_YELLOW ) );
				break;
			case ':':
			case '.':
				attron( COLOR_PAIR( COLOR_RED ) );
				break;
			default:
				attroff( A_BOLD );
				attron( COLOR_PAIR( COLOR_WHITE ) );
			}

			mvaddch( cy+j, cx+i, etraom_logo[j][i] );
		}
	
	getch();
}

void input_direction( int *x, int *y )
{
	int c = 0;

	*x = 0;
	*y = 0;

	while( 1 )
	{
		c = getch();

		switch( c )
		{
		case 'h':
			*x = -1;
			return;
		case 'j':
			*y =  1;
			return;
		case 'k':
			*y = -1;
			return;
		case 'l':
			*x =  1;
			return;
		default:
			push_message( "Invalid input.", message_normal, 0, turn_count );
			;
		}
	}
}
