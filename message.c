
#include <stdlib.h>
#include <string.h>

#include "etraom.h"

void clear_messages( int begin, int end )
{
	int i;
	for( i = begin; i < end; i++ )
	{
		if( ( i > 0 ) &&
			( i < messagelist_size ) )
		{
			message[i].s = "";
			message[i].type = message_normal;
			message[i].allocated = 0;
			message[i].time = 0;
		}
	}
}

void push_message( char* s, int type, int allocated, int time )
{
	if( message_count == messagelist_size )
	{
		message = realloc( message, messagelist_size*2 * sizeof(message_t) );
		clear_messages( messagelist_size, messagelist_size*2 );
		messagelist_size *= 2;
	}

	message[message_count].s = s;
	message[message_count].type = type;
	message[message_count].allocated = allocated;
	message[message_count].time = time;

	message_count++;
}
