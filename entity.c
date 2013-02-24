
#include <stdlib.h>

#include "etraom.h"

entity_t *alloc_entity( void )
{
	entity_t *e = malloc( sizeof(entity_t) );

	e->name = "";
	e->level = 0;
	e->hp = 0;
	e->max_hp = 0;
	e->body = 0;
	e->mind = 0;
	e->luck = 0;
	e->x = 0;
	e->y = 0;
	e->map = NULL;

	return e;
}

void free_entity( entity_t *e )
{
	if( e )
		free( e );
}

void entity_move_rel( entity_t *e, int rx, int ry )
{
	if( is_legal( e->map, e->x+rx, e->y+ry ) &&
		!blocks_movement( e->map->tile[e->x+rx][e->y+ry].type ) )
	{
		e->x += rx;
		e->y += ry;
	}
}

void entity_follow_link( entity_t *e, link_t *l )
{
	/*	followed link is assumed to be legal; i.e. it shouldn't lead into a
		wall, or a null map */
	e->x = l->dest_x;
	e->y = l->dest_y;
	e->map = l->dest_map;
}
