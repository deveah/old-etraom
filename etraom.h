
/*
 *	etraom.h
 *	main header file for etraom
*/

#ifndef _ETRAOM_H_
#define _ETRAOM_H_

#define MAP_MAGIC (1<<16)
#define MAP_MAX_DOORS 3
#define DOOR_CHANCE 2

#define MAX_ENTITIES 256
#define MAX_ITEMS 1024
#define MAX_MAPS 16

#define MAP_X 80
#define MAP_Y 24

#define FOV_LENGTH 5

#define SCREEN_X 80
#define SCREEN_Y 25

#define TILEFLAG_LIT		(1<<0)
#define TILEFLAG_SEEN		(1<<1)

#define COLOR_WALL			COLOR_CYAN
#define COLOR_FLOOR			COLOR_WHITE
#define COLOR_DOOR			COLOR_BLUE

/* general game-related enums */

enum tile_type
{
	tile_void, tile_floor, tile_room_floor, tile_wall, tile_door_closed,
	tile_door_open
};

enum item_location
{
	loc_void, loc_floor, loc_inventory
};

/* general game-related structs */

typedef struct
{
	int type, flags;
} tile_t;

typedef struct
{
	char *name;
	int width, height, flags;
	tile_t **tile;
} map_t;

typedef struct
{
	char *name;
	int type;

	int location;
	int x, y;
	map_t *map;
} item_t;

typedef struct
{
	char *name, face;
	int level, hp, max_hp;
	int body, mind, luck;

	int x, y;
	map_t *map;
} entity_t;

/*	TODO: implement links;
	links are the glue between the layers of the dungeon,
	being able to send entities from any to any other map,
	even circularly */
typedef struct
{
	map_t *dest;
	int dest_x, dest_y;

	int x, y;
	map_t *map;
} link_t;

/* main data arrays */
entity_t entity[MAX_ENTITIES];
int entity_count;

item_t item[MAX_ITEMS];
int item_count;

map_t *dungeon[MAX_MAPS];

/* map.c */
map_t *alloc_map( int width, int height );
void free_map( map_t *m );
void clear_map( map_t *m );
int is_legal( map_t *m, int x, int y );
int blocks_movement( int t );
int blocks_light( int t );

/* mapgen.c */
int count_neighbours( map_t *m, int x, int y, int w );
int near_floor( map_t *m, int x, int y, int ortho );
int generate_dfa(	map_t *m, int root_x, int root_y, int max_cells,
 					int max_neighbours, int ortho );
int generate_dfa_maze(	map_t *m, int root_x, int root_y, int max_cells,
 						int max_neighbours );
int is_free( map_t *m, int x1, int y1, int x2, int y2, int w );
int count_placeable_doors( map_t *m, int x1, int y1, int x2, int y2 );
void place_doors( map_t *m, int x1, int y1, int x2, int y2 );
void dig_room( map_t *m, int x1, int y1, int x2, int y2 );
int dig_rooms( map_t *m, int nrooms, int minw, int minh, int maxw, int maxh );
void post_process( map_t *m );

/* item.c */
item_t *alloc_item( void );
void free_item( item_t *i );

/* entity.c */
entity_t *alloc_entity( void );
void free_entity( entity_t *e );
void entity_move_rel( entity_t *e, int rx, int ry );

/* ui.c */
void init_screen( void );
void terminate_screen( void );
void set_color( int f, int attr );
void draw_screen( void );
void title_screen( void );

/* game.c */
void init_game( void );
void terminate_game( void );
int player_act( entity_t *e, int c );
void game_loop( void );

/* sight.c */
int do_ray( map_t *m, int x1, int y1, int x2, int y2 );
int do_fov( void );

/* misc.c */
void die( char *s );
int distance( int x1, int y1, int x2, int y2 );

#endif
