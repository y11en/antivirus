


#ifndef __KUDATA_INT_H
#define __KUDATA_INT_H

#include "kudata.h"

KU_CDECL_BEGIN

typedef struct __kudata_header
{
	uint8_t magic[ 4 ];			// "KUDT"

	kudata_info_t	info;
} kudata_header_t;


struct __kudata
{
	kudata_header_t* header;
	int32_t length;

	int heap_alloc;


	void* map;
};


void kudata_init( kudata_t* data );
int kudata_new( kudata_t** pdata );
void kudata_assign( kudata_t* dst, kudata_t* src );

KU_CDECL_END

#endif // __KUDATA_INT_H
