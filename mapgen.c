
#include "etraom.h"

int count_neighbours( map_t *m, int x, int y,int w )
{
	 int i, j;
	 int n = 0;

	for( i = x-1; i <= x+1; i++ )
		for( j = y-1; j <= y+1; j++ )
		{
			if( is_legal( m, i, j ) && m->tile[i][j].type == w )
				n++;
		}
	
	return n;
}

int near_floor( map_t *m, int x, int y, int ortho )
{
	int i, j;

	if( ortho )
	{
		for( i = x-1; i <= x+1; i++ )
			for( j = y-1; j <= y+1; j++ )
				if( is_legal( m, i, j ) && m->tile[i][j].type == tile_floor )
					return 1;
	}
	else
	{
		if( is_legal( m, x-1, y ) && m->tile[x-1][y].type == tile_floor ) return 1;
		if( is_legal( m, x+1, y ) && m->tile[x+1][y].type == tile_floor ) return 1;
		if( is_legal( m, x, y-1 ) && m->tile[x][y-1].type == tile_floor ) return 1;
		if( is_legal( m, x, y+1 ) && m->tile[x][y+1].type == tile_floor ) return 1;
	}

	return 0;
}

int generate_dfa(	map_t *m, int root_x, int root_y, int max_cells,
					int max_neighbours, int ortho )
{
	int cx = 0, cy = 0, builder_spawned = 0, cells_dug = 0;
	int dx = 0, dy = 0;
	int n;
	int tries = 0;

	int i, j;
	for( i = 0; i < m->width; i++ )
		for( j = 0; j < m->height; j++ )
			m->tile[i][j].type = tile_wall;

	m->tile[root_x][root_y].type = tile_floor;

	while( cells_dug < max_cells )
	{
		tries++;
		if( tries > MAP_MAGIC*4 )
			return cells_dug;

		if( !builder_spawned )
		{
			cx = 2 + rand() % ( m->width - 2 );
			cy = 2 + rand() % ( m->height - 2 );

			if( m->tile[cx][cy].type == tile_wall )
				builder_spawned = 1;
		}
		else
		{
			dx = 0;
			dy = 0;
			while( dx*dx + dy*dy != 1 )
			{
				dx = ( rand() % 3 ) - 1;
				dy = ( rand() % 3 ) - 1;
			}

			if( is_legal( m, cx+dx, cy+dy ) )
			{
				cx += dx;
				cy += dy;

				n = count_neighbours( m, cx, cy, tile_floor );

				if( near_floor( m, cx, cy, ortho ) && ( n > 0 ) && ( n < max_neighbours ) )
				{
					m->tile[cx][cy].type = tile_floor;
					cells_dug++;
					builder_spawned = 0;
				}
			}
		}
	}

	return 0;
}

void link( map_t *m, int x1, int y1, int x2, int y2 )
{
	int cx = x1, cy = y1;
	int dx, dy;

	dx = ( x2 < x1 ) ? -1 : 1;
	dy = ( y2 < y1 ) ? -1 : 1;

	while( cx != x2 )
	{
		if( is_legal( m, cx+dx, cy ) )
		{
			cx += dx;
			m->tile[cx][cy].type = tile_floor;
		}
		else
			return;
	}

	while( cy != y2 )
	{
		if( is_legal( m, cx, cy+dy ) )
		{
			cy += dy;
			m->tile[cx][cy].type = tile_floor;
		}
		else
			return;
	}
}

int generate_dfa_maze(	map_t *m, int root_x, int root_y, int max_cells,
						int max_neighbours )
{
	map_t *small_map = alloc_map( m->width/2, m->height/2 );

	int cx = 0, cy = 0, builder_spawned = 0, cells_dug = 0;
	int dx = 0, dy = 0;
	int n;
	int tries = 0;

	int i, j;
	for( i = 0; i < m->width; i++ )
		for( j = 0; j < m->height; j++ )
			m->tile[i][j].type = tile_wall;

	for( i = 0; i < small_map->width; i++ )
		for( j = 0; j < small_map->height; j++ )
			small_map->tile[i][j].type = tile_wall;

	small_map->tile[root_x/2][root_y/2].type = tile_floor;

	while( cells_dug < max_cells )
	{
		tries++;
		if( tries > MAP_MAGIC )
			return cells_dug;

		if( !builder_spawned )
		{
			cx = 2 + rand() % ( small_map->width - 2 );
			cy = 2 + rand() % ( small_map->height - 2 );

			if( small_map->tile[cx][cy].type == tile_wall )
				builder_spawned = 1;
		}
		else
		{
			dx = 0;
			dy = 0;
			while( dx*dx + dy*dy != 1 )
			{
				dx = ( rand() % 3 ) - 1;
				dy = ( rand() % 3 ) - 1;
			}

			// TODO: map height inaccurate due to is_legal calculation errors;
			if( is_legal( small_map, cx+dx*2, cy+dy*2 ) )
			//if( is_legal( m, cx*2+dx*3, cy*2+dy*3 ) )
			{
				cx += dx;
				cy += dy;

				n = count_neighbours( small_map, cx, cy, tile_floor );

				if( near_floor( small_map, cx, cy, 0 ) && ( n > 0 ) && ( n < max_neighbours ) )
				{
					small_map->tile[cx][cy].type = tile_floor;
					m->tile[cx*2][cy*2].type = tile_floor;
					
					if( is_legal( small_map, cx-1, cy ) && small_map->tile[cx-1][cy].type == tile_floor )
						link( m, cx*2, cy*2, (cx-1)*2, cy*2 );
					if( is_legal( small_map, cx+1, cy ) && small_map->tile[cx+1][cy].type == tile_floor )
						link( m, cx*2, cy*2, (cx+1)*2, cy*2 );
					if( is_legal( small_map, cx, cy-1 ) && small_map->tile[cx][cy-1].type == tile_floor )
						link( m, cx*2, cy*2, cx*2, (cy-1)*2 );
					if( is_legal( small_map, cx, cy+1 ) && small_map->tile[cx][cy+1].type == tile_floor )
						link( m, cx*2, cy*2, cx*2, (cy+1)*2 );
					
					cells_dug++;
					builder_spawned = 0;
				}
			}
		}
	}

	free_map( small_map );

	return 0;
}

void dig_room( map_t *m, int x1, int y1, int x2, int y2 )
{
	int i, j;
	for( i = x1; i <= x2; i++ )
		for( j = y1; j <= y2; j++ )
		{
			if( is_legal( m, i, j ) )
			{
				if( m->tile[i][j].type == tile_wall )
					m->tile[i][j].type = tile_room_floor;
			}
		}
}

int is_free( map_t *m, int x1, int y1, int x2, int y2, int w )
{
	int i,j;
	for( i = x1-1; i <= x2+1; i++ )
		for( j = y1-1; j <= y2+1; j++ )
		{
			if( is_legal( m, i, j ) && m->tile[i][j].type == w )
				return 0;
		}
	
	return 1;
}

int count_placeable_doors( map_t *m, int x1, int y1, int x2, int y2 )
{
	int i, n = 0;

	for( i = x1; i <= x2; i++ )
	{
		if( is_legal( m, i, y1 ) && m->tile[i][y1].type == tile_floor ) n++;
		if( is_legal( m, i, y2 ) && m->tile[i][y2].type == tile_floor ) n++;
	}

	for( i = y1; i <= y2; i++ )
	{
		if( is_legal( m, x1, i ) && m->tile[x1][i].type == tile_floor ) n++;
		if( is_legal( m, x2, i ) && m->tile[x2][i].type == tile_floor ) n++;
	}

	return n;
}

void place_doors( map_t *m, int x1, int y1, int x2, int y2 )
{
	int i;

	for( i = x1; i <= x2; i++ )
	{
		if( is_legal( m, i, y1 ) && m->tile[i][y1].type == tile_floor )
			m->tile[i][y1].type = tile_door_closed;;
		if( is_legal( m, i, y2 ) && m->tile[i][y2].type == tile_floor )
			m->tile[i][y2].type = tile_door_closed;
	}

	for( i = y1; i <= y2; i++ )
	{
		if( is_legal( m, x1, i ) && m->tile[x1][i].type == tile_floor )
			m->tile[x1][i].type = tile_door_closed;
		if( is_legal( m, x2, i ) && m->tile[x2][i].type == tile_floor )
			m->tile[x2][i].type = tile_door_closed;
	}
}

int dig_rooms( map_t *m, int nrooms, int minw, int minh, int maxw, int maxh )
{
	int i, cx, cy, cw, ch;
	int tries = 0;
	int r;

	for( i = 0; i < nrooms; i++ )
	{
		while( 1 )
		{
			tries++;
			if( tries > MAP_MAGIC )
				return i;

			cx = rand() % ( m->width/2 );
			cy = rand() % ( m->height/2 );
			cw = ( rand() % ( maxw-minw ) ) + minw;
			ch = ( rand() % ( maxh-minh ) ) + minh;

			if(	is_legal( m, cx*2, cy*2 ) &&
				is_legal( m, cx*2+1+cw*2, cy*2+1+ch*2 ) &&
				m->tile[cx*2][cy*2].type == tile_floor &&
				is_free( m, cx*2, cy*2, cx*2+cw*2, cy*2+ch*2, tile_room_floor ) )
				break;
		}

		dig_room( m, cx*2, cy*2, cx*2+cw*2, cy*2+ch*2 );

		r = count_placeable_doors( m, cx*2-1, cy*2-1, cx*2+cw*2+1, cy*2+ch*2+1 );
		if( ( r < MAP_MAX_DOORS ) && ( rand() % DOOR_CHANCE == 0 ) )
			place_doors( m, cx*2-1, cy*2-1, cx*2+cw*2+1, cy*2+ch*2+1 );
	}
}
