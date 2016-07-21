



#include "kudata.h"
#include "kudata_int.h"
#include "kudata_mapfile.h"
#include "kumutex.h"
#include "kualloc.h"

#include "kuinit_int.h"

#include <memory.h>
#include <string.h>

#if KU_IS_BIG_ENDIAN
# define KUDATA_FILE "kulib_b.dat"
#else
# define KUDATA_FILE "kulib_l.dat"
#endif


static kumutex_t g_data_mtx;

static char g_data_file[ 1024 ] = KUDATA_FILE;
static kudata_t* g_cmn_data;

int _kudata_init( void )
{
	return kumutex_init( &g_data_mtx );
}

int _kudata_term( void )
{
	kumutex_lock( &g_data_mtx );

	if( g_cmn_data != NULL )
	{
		kudata_close( g_cmn_data );
		g_cmn_data = NULL;
	}

	kumutex_unlock( &g_data_mtx );

	return kumutex_destroy( &g_data_mtx );
}

int kudata_set_data_file( const char* filename )
{
	if( filename == NULL || *filename == '\0'
		|| ( strlen( filename ) + 1 ) > sizeof( g_data_file ) )
	{
		return KU_INVALID_ARG;
	}

	strcpy( g_data_file, filename );

	return KU_OK;
}

static int check_cmn_data( kudata_t* data )
{
	kudata_header_t* hdr = data->header;

	if( !( hdr->magic[ 0 ] == 'K'
			&& hdr->magic[ 1 ] == 'U'
			&& hdr->magic[ 2 ] == 'D'
			&& hdr->magic[ 3 ] == 'T'

			&& hdr->info.size >= sizeof( kudata_info_t )

			&& hdr->info.is_big_endian == KU_IS_BIG_ENDIAN

			&& hdr->info.data_format[ 0 ] == 'd'
			&& hdr->info.data_format[ 1 ] == 'a'
			&& hdr->info.data_format[ 2 ] == 't'
			&& hdr->info.data_format[ 3 ] == 'a'

			&& hdr->info.format_version[ 0 ] == 1 ) )
	{
		return KU_INVALID_FORMAT_ERROR;
	}

	return KU_OK;
}

static int get_cmn_data( kudata_t** pdata )
{
	kudata_t* cmn_data;
	kudata_t* new_cmn_data;
	int ret;

	kumutex_lock( &g_data_mtx );
	cmn_data = g_cmn_data;
	kumutex_unlock( &g_data_mtx );

	if( cmn_data == NULL )
	{
		kudata_t data;

		if( !kudata_mapfile( &data, g_data_file ) )
			return KU_FILE_ACCESS_ERROR;

		if( ( ret = check_cmn_data( &data ) ) != KU_OK )
		{
			kudata_close( &data );
			return ret;
		}

		if( ( ret = kudata_new( &new_cmn_data ) ) != KU_OK )
		{
			kudata_close( &data );
			return ret;
		}

		kudata_assign( new_cmn_data, &data );

		kumutex_lock( &g_data_mtx );

		cmn_data = g_cmn_data;
		if( cmn_data == NULL )
			g_cmn_data = new_cmn_data;

		kumutex_unlock( &g_data_mtx );

		if( cmn_data != NULL )
			kudata_close( new_cmn_data );
		else
			cmn_data = new_cmn_data;
	}


	*pdata = cmn_data;
	return KU_OK;
}

typedef struct __kudata_toc_entry
{
	uint32_t name_offset;
	uint32_t data_offset;
} kudata_toc_entry_t;


typedef struct __kudata_toc
{
	uint32_t size;
	kudata_toc_entry_t entries[ 1];
} kudata_toc_t;

static kudata_header_t* lookup_data( void* data, const char* name )
{
	uint32_t start, limit, mid, prev;
	kudata_toc_t* toc = (kudata_toc_t*) data;
	kudata_toc_entry_t* entries = toc->entries;
	char* base = data;

	start = 0;
	limit = toc->size;
	prev = limit;
	for(;;)
	{
		int res;

		mid = ( start + limit ) / 2;
		if( mid == prev )
			break;

		res = strcmp( name, (char*) base + entries[ mid ].name_offset );
		if( res < 0 )
		{
			limit = mid;
		}
		else if( res > 0 )
		{
			start = mid;
		}
		else
		{
			return (kudata_header_t*) ( base + entries[ mid ].data_offset );
		}

		prev = mid;
	}

	return NULL;
}

static int check_data_item( kudata_header_t* hdr,
		const char* name, const char* type,
		kudata_is_acceptable_proc is_acceptable, void* ctx,
		kudata_t** pdata )
{
	int ret;
	kudata_t* data;

	if( !( hdr->magic[ 0 ] == 'K'
			&& hdr->magic[ 1 ] == 'U'
			&& hdr->magic[ 2 ] == 'D'
			&& hdr->magic[ 3 ] == 'T'
			&& ( is_acceptable == NULL || is_acceptable( name, type, ctx, &(hdr->info) ) ) ) )
	{
		return KU_INVALID_FORMAT_ERROR;
	}

	if( ( ret = kudata_new( &data ) ) != KU_OK )
	{
		return ret;
	}

	data->header = hdr;

	*pdata = data;
	return KU_OK;
}

int kudata_open( const char* name, const char* type, kudata_is_acceptable_proc is_acceptable, void* ctx, kudata_t** pdata )
{
	int ret;
	kudata_t* cmn_data;
	void* data;
	char buf[ 100 ];
	kudata_header_t* hdr;

	if( ( strlen( name ) + 1 + strlen( type ) + 1 ) > sizeof(buf) )
		return KU_INVALID_ARG;

	if( ( ret = get_cmn_data( &cmn_data ) ) != KU_OK )
		return ret;

	data = kudata_getdata( cmn_data );

	strcpy( buf, name );
	strcat( buf, "." );
	strcat( buf, type );

	hdr = lookup_data( data, buf );
	if( hdr == NULL )
		return KU_FILE_ACCESS_ERROR;

	return check_data_item( hdr, name, type, is_acceptable, ctx, pdata );
}


void kudata_close( kudata_t* data )
{
	kudata_unmapfile( data );

	if( data->heap_alloc )
		ku_free( data );
	else
		kudata_init( data );

}


void* kudata_getdata( kudata_t* data )
{

	if( data->header == NULL )
		return NULL;

	return (uint8_t*) &data->header->info + data->header->info.size;
}



