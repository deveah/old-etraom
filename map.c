
#include <stdlib.h>

#include "etraom.h"

map_t *alloc_map( int width, int height )
{
	map_t *m = (map_t*) malloc( sizeof(map_t) );
	m->width = width;
	m->height = height;

	m->tile = (tile_t**) malloc( width * sizeof(int*) );

	int i;
	for( i = 0; i < width; i++ )
	{
		m->tile[i] = (tile_t*) malloc( height * sizeof(tile_t) );
	}

	clear_map( m );

	return m;
}

void free_map( map_t *m )
{
	int i;

	if( m )
	{
		if( m->tile )
		{
			for( i = 0; i < m->width; i++ )
				if( m->tile[i] ) free( m->tile[i] );

			free( m->tile );
		}
		free( m );
	}
}

void clear_map( map_t *m )
{
	int i, j;

	m->name = "";
	m->flags = 0;

	for( i = 0; i < m->width; i++ )
		for( j = 0; j < m->height; j++ )
		{
			m->tile[i][j].type = tile_void;
			m->tile[i][j].flags = 0;
		}
}

int is_legal( map_t *m, int x, int y )
{
	return (	( x >= 0 ) && ( x < m->width-1 ) &&
				( y >= 0 ) && ( y < m->height-1 ) );
}

int blocks_movement( int t )
{
	return (	( t == tile_void ) ||
				( t == tile_wall ) ||
				( t == tile_door_closed ) );
}

int blocks_light( int t )
{
	return (	( t == tile_wall ) ||
				( t == tile_door_closed ) );
}
