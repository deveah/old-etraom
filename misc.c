
#include <math.h>
#include <stdlib.h>
#include <curses.h>

#include "etraom.h"

void die( char *s )
{
	endwin();
	printf( "Error: %s\n",s );
	exit( 0 );
}

int distance( int x1, int y1, int x2, int y2 )
{
	return sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
}

int link_exists( map_t *m, int x, int y )
{
	int i;

	if( !is_legal( m, x, y ) )
		return -1;

	for( i = 0; i < link_count; i++ )
	{
		if( ( link[i].map == m ) &&
			( link[i].x == x ) &&
			( link[i].y == y ) )
			return i;
	}

	return -1;
}

int entity_exists( map_t *m, int x, int y )
{
	int i;

	if( !is_legal( m, x, y ) )
		return -1;

	for( i = 0; i < entity_count; i++ )
	{
		if( ( entity[i].map == m ) &&
			( entity[i].x == x ) &&
			( entity[i].y == y ) )
			return i;
	}

	return -1;
}
