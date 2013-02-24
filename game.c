
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <curses.h>

#include "etraom.h"

void init_game( void )
{
	int i, r;
	int ax, ay;

	init_screen();
	title_screen();
	
	entity_count = 1;
	item_count = 0;
	link_count = 0;

	srand( time( 0 ) );

	clear();
	attron( COLOR_PAIR( COLOR_WHITE ) );
	mvprintw( 12, 5, "Generating dungeon" );

	for( i = 0; i < MAX_MAPS; i++ )
	{
		dungeon[i] = alloc_map( MAP_X, MAP_Y );
		r = generate_dfa_maze( dungeon[i], MAP_X/2, MAP_Y/2, 200, 3 );
		if( r == 0 ) r = 200;
		dig_rooms( dungeon[i], r/20, 1, 1, 4, 3 );
		post_process( dungeon[i] );
		
		mvaddch( 12, 23+i, '.' );
	}

	for( i = 0; i < MAX_MAPS; i++ )
	{
		/* link back */
		if( i > 0 )
		{
			link[link_count].map = dungeon[i];
			link[link_count].x = link[link_count-1].x;
			link[link_count].y = link[link_count-1].y;
			link[link_count].dest_map = dungeon[i-1];
			link[link_count].dest_x = link[link_count-1].x;
			link[link_count].dest_y = link[link_count-1].y;
			link[link_count].face = '<';
			link_count++;
		}

		/* link forward */
		if( i < MAX_MAPS-1 )
		{
			while( 1 )
			{
				ax = rand() % MAP_X;
				ay = rand() % MAP_Y;
				if(	( dungeon[i  ]->tile[ax][ay].type == tile_floor ) &&
					( dungeon[i+1]->tile[ax][ay].type == tile_floor ) )
				{
					link[link_count].map = dungeon[i];
					link[link_count].x = ax;
					link[link_count].y = ay;
					link[link_count].dest_map = dungeon[i+1];
					link[link_count].dest_x = ax;
					link[link_count].dest_y = ay;
					link[link_count].face = '>';
					link_count++;
					break;
				}
			}
		}
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

	draw_screen();
}

void terminate_game( void )
{
	free_map( dungeon[0] );
}

int open_door( entity_t *e, int x, int y )
{
	if( is_legal( e->map, x, y ) &&
		e->map->tile[x][y].type == tile_door_closed )
	{
		/* TODO: "e opens the door" message */
		e->map->tile[x][y].type = tile_door_open;
		return 1;
	}
	else
	{
		/* TODO: "that's not a door" message */
		return 0;
	}

	return 0;
}

int close_door( entity_t *e, int x, int y )
{
	if( is_legal( e->map, x, y ) &&
		e->map->tile[x][y].type == tile_door_open )
	{
		/* TODO: "e closes the door" message */
		e->map->tile[x][y].type = tile_door_closed;
		return 1;
	}
	else
	{
		/* TODO: "that's not an open door" message */
		return 0;
	}

	return 0;
}

int follow_stairs( entity_t *e )
{
	int i;
	for( i = 0; i < link_count; i++ )
	{
		if( ( link[i].map == e->map ) &&
			( link[i].x == e->x ) &&
			( link[i].y == e->y ) )
		{
			entity_follow_link( e, &link[i] );
			/* TODO: "you descend/ascend" message */
			return 1;
		}
	}

	/* TODO: "those are not stairs" message */
	return 0;
}

int player_act( entity_t *e, int c )
{
	int i, j;
	int dx = 0, dy = 0;
	int tx = 0, ty = 0;

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
	case '>':
		follow_stairs( e );
		break;
	case 'o':
		/* TODO: "what direction?" message */
		input_direction( &tx, &ty );
		open_door( e, e->x+tx, e->y+ty );
		break;
	case 'c':
		/* TODO: "what direction?" message */
		input_direction( &tx, &ty );
		close_door( e, e->x+tx, e->y+ty );
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
				open_door( e, e->x+dx, e->y+dy );
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
