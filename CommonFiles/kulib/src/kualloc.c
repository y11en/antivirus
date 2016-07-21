

#include "kualloc.h"

#include <stdlib.h>


static ku_malloc_proc __malloc;
static ku_realloc_proc __realloc;
static ku_free_proc __free;
static void* __context;

int ku_set_malloc( void* context,
	ku_malloc_proc m, ku_realloc_proc r, ku_free_proc f )
{
	if( m == NULL || r == NULL || f == NULL )
	{
		return KU_INVALID_ARG;
	}

	__malloc = m;
	__realloc = r;
	__free = f;
	__context = context;

	return KU_OK;
}


void* ku_malloc( size_t size )
{
	if( __malloc )
		return __malloc( __context, size );
	else
		return malloc( size );

}

void* ku_realloc( void* mem, size_t size )
{
	if( __realloc )
		return __realloc( __context, mem, size );
	else
		return realloc( mem, size );
}

void ku_free( void* mem )
{
	if( __free )
		__free( __context, mem );
	else
		free( mem );
}

