
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <curses.h>

#include "etraom.h"

void init_game( void )
{
	int i, r;
	int ax, ay;
	int tries = 0;

	init_screen();
	title_screen();

	turn_count = 0;

	entity_count = 1;
	item_count = 0;
	link_count = 0;
	message_count = 0;

	srand( time( 0 ) );

	messagelist_size = MIN_MESSAGELIST_SIZE;
	message = malloc( MIN_MESSAGELIST_SIZE * sizeof(message_t) );

	clear();
	attron( COLOR_PAIR( COLOR_WHITE ) );
	mvprintw( 12, 5, "Generating dungeon" );

	for( i = 0; i < MAX_MAPS; i++ )
	{
		dungeon[i] = alloc_map( MAP_X, MAP_Y );
	
		r = 0;
		while( r < 100 )
		{
			r = generate_dfa_maze( dungeon[i], MAP_X/2, MAP_Y/2, 200, 3 );
			if( r == 0 ) r = 200;
		}
		
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
				tries++;
				if( tries > MAP_MAGIC )
				{
					while( 1 )
					{
						ax = rand() % MAP_X;
						ay = rand() % MAP_Y;

						if( dungeon[i]->tile[ax][ay].type == tile_floor )
							break;
					}
				}

				ax = rand() % MAP_X;
				ay = rand() % MAP_Y;
				if(	( dungeon[i  ]->tile[ax][ay].type == tile_floor ) &&
					( dungeon[i+1]->tile[ax][ay].type == tile_floor ) &&
					link_exists( dungeon[i], ax, ay ) == -1 )
				{
					break;
				}
			}

			link[link_count].map = dungeon[i];
			link[link_count].x = ax;
			link[link_count].y = ay;
			link[link_count].dest_map = dungeon[i+1];
			link[link_count].dest_x = ax;
			link[link_count].dest_y = ay;
			link[link_count].face = '>';
			link_count++;
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
	int i;
	for( i = 0; i < MAX_MAPS; i++ )
		free_map( dungeon[i] );

	for( i = 0; i < message_count; i++ )
	{
		if( message[i].allocated )
			free( message[i].s );
	}
	
	free( message );
}

int open_door( entity_t *e, int x, int y )
{
	if( is_legal( e->map, x, y ) &&
		e->map->tile[x][y].type == tile_door_closed )
	{
		char *msg = malloc( MAX_MESSAGE_SIZE * sizeof(char) );
		snprintf( msg, MAX_MESSAGE_SIZE, "%s opens the door.", e->name );
		push_message( msg, message_normal, 1, turn_count );

		e->map->tile[x][y].type = tile_door_open;
		return 1;
	}
	else
	{
		push_message( "That's not a closed door.", message_normal, 0,
			turn_count );

		return 0;
	}

	return 0;
}

int close_door( entity_t *e, int x, int y )
{
	if( is_legal( e->map, x, y ) &&
		e->map->tile[x][y].type == tile_door_open )
	{
		char *msg = malloc( MAX_MESSAGE_SIZE * sizeof(char) );
		snprintf( msg, MAX_MESSAGE_SIZE, "%s closes the door.", e->name );
		push_message( msg, message_normal, 1, turn_count );

		e->map->tile[x][y].type = tile_door_closed;
		return 1;
	}
	else
	{
		push_message( "That's not an open door.", message_normal, 0,
			turn_count );
		return 0;
	}

	return 0;
}

int follow_stairs( entity_t *e )
{
	int i;
	i = link_exists( e->map, e->x, e->y );
	
	if( i != -1 )
	{
		char *msg = malloc( MAX_MESSAGE_SIZE * sizeof(char) );
		if( link[i].face == '>' )
			snprintf( msg, MAX_MESSAGE_SIZE, "%s descends.", e->name );
		else if( link[i].face == '<' )
			snprintf( msg, MAX_MESSAGE_SIZE, "%s ascends.", e->name );
		else
			strcat( msg, "Bug." );
		push_message( msg, message_normal, 1, turn_count );

		entity_follow_link( e, &link[i] );
		return 1;
	}
	else
	{
		push_message( "Those aren't stairs.", message_normal, 0, turn_count );
		return 0;
	}
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
		push_message( "What direction?", message_normal, 0, turn_count );
		input_direction( &tx, &ty );
		open_door( e, e->x+tx, e->y+ty );
		break;
	case 'c':
		push_message( "What direction?", message_normal, 0, turn_count );
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
	
		turn_count++;
	}

	terminate_screen();
	terminate_game();
}
