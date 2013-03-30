
#include <stdlib.h>

#include "etraom.h"

item_t *alloc_item( void )
{
	item_t *i = malloc( sizeof(item_t) );

	return i;
}

void free_item( item_t *i )
{
	if( i )
		free( i );
}

int find_item_by_location( map_t *m, int x, int y )
{
	int i;

	if( is_legal( m, x, y ) )
	{
		for( i = 0; i < item_count; i++ )
		{
			if(	( item[i].map == m ) &&
				( item[i].x == x ) &&
				( item[i].y == y ) )
				return i;
		}
	}

	return -1;
}
