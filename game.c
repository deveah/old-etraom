
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "etraom.h"

void init_game( void )
{
	entity_count = 1;
	item_count = 0;

	srand( time( 0 ) );

	dungeon[0] = alloc_map( MAP_X, MAP_Y );
	int r = generate_dfa_maze( dungeon[0], MAP_X/2, MAP_Y/2, 200, 3 );
	if( r == 0 ) r = 200;
	dig_rooms( dungeon[0], r/20, 1, 1, 4, 3 );

	entity[0].name = "Player";
	entity[0].face = '@';
	entity[0].level = 1;
	entity[0].hp = 1;
	entity[0].max_hp = 1;
	entity[0].body = 1;
	entity[0].mind = 1;
	entity[0].luck = 1;
	entity[0].map = dungeon[0];

	int ix = 0, iy = 0;
	while( entity[0].map->tile[ix][iy].type != tile_floor )
	{
		ix = rand() % MAP_X;
		iy = rand() % MAP_Y;
	}
	entity[0].x = ix;
	entity[0].y = iy;

	init_screen();
}

void terminate_game( void )
{
	free_map( dungeon[0] );
}

void game_loop( void )
{
	int i, j, c;
	int running = 1;

	while( running )
	{
		c = getch();

		switch( c )
		{
		case 'q':
			running = 0;
			break;
		case 'h':
			entity_move_rel( &entity[0], -1,  0 );
			break;
		case 'j':
			entity_move_rel( &entity[0],  0,  1 );
			break;
		case 'k':
			entity_move_rel( &entity[0],  0, -1 );
			break;
		case 'l':
			entity_move_rel( &entity[0],  1,  0 );
			break;
		case 'z':
			for( i = 0; i < entity[0].map->width; i++ )
				for( j = 0; j < entity[0].map->height; j++ )
					entity[0].map->tile[i][j].flags |= TILEFLAG_SEEN;
			break;
		default:
			break;
		}

		draw_screen();
	}

	terminate_screen();
	terminate_game();
}
