
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "etraom.h"

void init_game( void )
{
	int i, r;

	entity_count = 1;
	item_count = 0;

	srand( time( 0 ) );

	for( i = 0; i < MAX_MAPS; i++ )
	{
		dungeon[i] = alloc_map( MAP_X, MAP_Y );
		r = generate_dfa_maze( dungeon[i], MAP_X/2, MAP_Y/2, 200, 3 );
		if( r == 0 ) r = 200;
		dig_rooms( dungeon[i], r/20, 1, 1, 4, 3 );
		post_process( dungeon[i] );
	}

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
	title_screen();
	draw_screen();
}

void terminate_game( void )
{
	free_map( dungeon[0] );
}

int player_act( entity_t *e, int c )
{
	int i, j;
	int dx = 0, dy = 0;

	switch( c )
	{
	case 'q':
		return -1;
		break;
	case 'h':
		dx = -1;
		break;
	case 'j':
		dy =  1;
		break;
	case 'k':
		dy = -1;
		break;
	case 'l':
		dx =  1;
		break;
	case 'z':
		for( i = 0; i < e->map->width; i++ )
			for( j = 0; j < e->map->height; j++ )
				e->map->tile[i][j].flags |= TILEFLAG_SEEN;
		break;
	default:
		break;
	}

	if( dx || dy )
	{
		if( is_legal( e->map, e->x+dx, e->y+dy ) )
		{
			if( !blocks_movement( e->map->tile[e->x+dx][e->y+dy].type ) )
				entity_move_rel( e, dx, dy );
			else if( e->map->tile[e->x+dx][e->y+dy].type == tile_door_closed )
			{
				/* TODO: open door message */
				e->map->tile[e->x+dx][e->y+dy].type = tile_door_open; 
			}
		}
	}
	return 0;
}

void game_loop( void )
{
	int c;
	int running = 1;

	while( running )
	{
		c = getch();
		if( player_act( &entity[0], c ) )
			running = 0;

		draw_screen();
	}

	terminate_screen();
	terminate_game();
}
