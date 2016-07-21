

#ifndef __KUDATA_H
#define __KUDATA_H

#include <kulib/ku_defs.h>

KU_CDECL_BEGIN

typedef struct __kudata_info
{
	uint16_t size;

	uint8_t is_big_endian;
	uint8_t	reserved;

	uint8_t data_format[ 4 ];
	uint8_t format_version[ 4 ];  // [0] major [1] minor [2] milli [3] micro

	/* total size: 12 */
} kudata_info_t;


struct __kudata;
typedef struct __kudata kudata_t;

typedef int (*kudata_is_acceptable_proc)( const char* name, const char* type, void* ctx, kudata_info_t* info );

int kudata_open( const char* name, const char* type, kudata_is_acceptable_proc is_acceptable, void* ctx, kudata_t** pdata );
void kudata_close( kudata_t* data );
void* kudata_getdata( kudata_t* data );

int kudata_set_data_file( const char* filename );


KU_CDECL_END

#endif /* __KUDATA_H */
