

#include "kudata_int.h"
#include "kualloc.h"

#include <memory.h>

void kudata_init( kudata_t* data )
{
	memset( data, 0, sizeof(kudata_t) );
	data->length = -1;
}


int kudata_new( kudata_t** pdata )
{
	kudata_t* data;

	data = ku_malloc( sizeof(kudata_t) );
	if( data == NULL )
		return KU_MEMORY_ALLOCATION_ERROR;

	kudata_init( data );
	data->heap_alloc = 1;

	*pdata = data;
	return KU_OK;
}

void kudata_assign( kudata_t* dst, kudata_t* src )
{
	int heap_alloc = dst->heap_alloc;
	memcpy( dst, src, sizeof(kudata_t) );
	dst->heap_alloc = heap_alloc;
}
