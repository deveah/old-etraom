
#include "etraom.h"

int do_ray( map_t *m, int x1, int y1, int x2, int y2 )
{
	int cx = x1, cy = y1;
	int dx, dy, sx, sy;
	float err, e2;

	dx = abs( x2-x1 );
	dy = abs( y2-y1 );

	if( x1 < x2 ) sx = 1; else sx = -1;
	if( y1 < y2 ) sy = 1; else sy = -1;
	err = dx-dy;

	while( 1 )
	{
		if( cx == x2 && cy == y2 )
			return 1;

		if( is_legal( m, cx, cy ) &&
			blocks_light( m->tile[cx][cy].type ) )
			return 0;

		e2 = 2 * err;
		if( e2 > -dy )
		{
			err -= dy;
			cx += sx;
		}
		if( e2 < dx )
		{
			err += dx;
			cy += sy;
		}
	}
	return 1;
}

int do_fov( void )
{
	int i, j;

	for( i = 0; i < entity[0].map->width; i++ )
		for( j = 0; j < entity[0].map->height; j++ )
		{
			if( entity[0].map->tile[i][j].flags && TILEFLAG_LIT )
			{
				entity[0].map->tile[i][j].flags &= ~TILEFLAG_LIT;
				entity[0].map->tile[i][j].flags |= TILEFLAG_SEEN;
			}
		}

	for( i = entity[0].x-FOV_LENGTH; i <= entity[0].x+FOV_LENGTH; i++ )
		for( j = entity[0].y-FOV_LENGTH; j <= entity[0].y+FOV_LENGTH; j++ )
		{
			if( is_legal( entity[0].map, i, j ) &&
				distance( entity[0].x, entity[0].y, i, j ) <= FOV_LENGTH &&
				do_ray( entity[0].map, entity[0].x, entity[0].y, i, j ) )
				entity[0].map->tile[i][j].flags |= TILEFLAG_LIT;
		}
}
