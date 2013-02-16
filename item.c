
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
