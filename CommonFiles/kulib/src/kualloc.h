

#ifndef __KUALLOC_H
#define __KUALLOC_H


#include "kulib/ku_defs.h"

KU_CDECL_BEGIN

void* ku_malloc( size_t size );
void* ku_realloc( void* mem, size_t size );
void ku_free( void* mem );


typedef void* (*ku_malloc_proc) ( void* context, size_t size );
typedef void* (*ku_realloc_proc) ( void* context, void* mem, size_t size );
typedef void (*ku_free_proc) ( void* context, void* mem );

int ku_set_malloc( void* context,
		ku_malloc_proc m, ku_realloc_proc r, ku_free_proc f );

KU_CDECL_END

#endif // __KUALLOC_H
