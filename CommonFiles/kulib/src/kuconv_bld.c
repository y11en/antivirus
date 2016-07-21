



#include "kuconv_int.h"

#include "kuconv_mbcs.h"

#include "kuhash.h"
#include "kumutex.h"
#include "kualloc.h"
#include "kudata.h"

#include "kuinit_int.h"

#include <string.h>



static kumutex_t g_cache_mtx;
static kuhash_t g_cnv_info_hash /* = NULL */;

static struct
{
	const char *const name;
	const conv_info *const info;
} const cnv_name_info[] =	
{
	/* array sorted by 'name' */
#if KU_IS_BIG_ENDIAN
	{ "utf16", &utf16be_data },
#else
	{ "utf16", &utf16le_data },
#endif
	{ "utf16le", &utf16le_data },
	{ "utf8", &utf8_data }
};


int _kuconv_init( void )
{
	return kumutex_init( &g_cache_mtx );
}

int _kuconv_term( void )
{
	kuhash_t cnv_info_hash;

	kumutex_lock( &g_cache_mtx );
	cnv_info_hash = g_cnv_info_hash;
	kumutex_unlock( &g_cache_mtx );

    if( cnv_info_hash != NULL )
	{
		kuconv_flush_cache();
        if( /* g_cnv_info_hash != NULL && */ kuhash_size( g_cnv_info_hash ) == 0 )
		{
			kuhash_destroy( g_cnv_info_hash );
			g_cnv_info_hash = NULL;
		}
    }

	return kumutex_destroy( &g_cache_mtx );
}

#define __tolower( c ) \
	( ( 'A' <= c && c <= 'Z' ) ? ( c ) + ( 'a' - 'A' ) : ( c ) )

static char* normalize_converter_name( char* norm, const char* name )
{
	char* norm_it = norm;
	char c;

	for(;;)
	{

		while( ( c = *name ) == '_' || c == '-' || c == ' ' ) ++name;

		*norm_it++ = __tolower( c );

		if( c == '\0' )
		{
			break;
		}

		++name;
	}

	return norm;
}


static const conv_info* get_algorithmic_conv_info( const char* name )
{
	char norm_name[ KUCONV_MAX_CONVERTER_NAME_LENGTH ];

	uint32_t start, limit, mid, prev_mid;
	int res;

	normalize_converter_name( norm_name, name);

	start = 0;
	limit = sizeof(cnv_name_info) / sizeof(cnv_name_info[0]);
	prev_mid = limit;
	for(;;)
	{
		mid = ( start + limit ) / 2;
		if( prev_mid == mid )
		{
			break;
		}

		res = strcmp( norm_name, cnv_name_info[ mid ].name );
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
			return cnv_name_info[ mid ].info;
		}

		prev_mid = mid;
	}

	return NULL;
}


static void cache_conv_info( conv_info* cnv_info )
{
	if( g_cnv_info_hash == NULL )
	{
		uint16_t aliases_count;

		if( KU_FAILURE( kuconv_get_aliases_count( &aliases_count ) ) )
		{
			return;
		}

		if( KU_FAILURE( kuhash_create( kuhash_chars, kuhash_compare_chars,
				aliases_count, GROW_ONLY, &g_cnv_info_hash ) ) )
		{
			return;
		}
	}

	cnv_info->cashed = 1;

	kuhash_set( g_cnv_info_hash, (kuhash_key_t) cnv_info->static_info->name, cnv_info );
}

static conv_info* get_cached_conv_info( const char* name )
{
	const kuhash_elem_t* e;

	if( g_cnv_info_hash == NULL )
	{
		return NULL;
	}


#if 0 
	{
		const kuhash_elem_t* e;
		int32_t pos = -1;
		fprintf( stderr, "<--\n" );
		while( ( e = kuhash_next_elem( g_cnv_info_hash, &pos ) ) != NULL )
		{
			fprintf( stderr, "'%s'\n", e->key );
		}
		fprintf( stderr, "-->\n" );
	}
	fprintf( stderr, "name: '%s'\n", name );
#endif
	
	e = kuhash_find( g_cnv_info_hash, (kuhash_key_t) name );
	if( e == NULL )
	{
		return NULL;
	}
	
	return (conv_info*) e->value;
}

static void delete_conv_info( conv_info* cnv_info )
{
	if( cnv_info->data != NULL )
	{
		kudata_close( (kudata_t*) cnv_info->data );
		cnv_info->data = NULL;
	}

	ku_free( cnv_info );
}


static int make_conv_info( kudata_t* data, conv_info** pcnv_info )
{
	int ret;
	conv_info* cnv_info;
	const uint8_t* raw_data = kudata_getdata( data );
	const static_conv_info* static_info = (const static_conv_info*) raw_data;


	cnv_info = ku_malloc( sizeof(conv_info) );
	if( cnv_info == NULL )
	{
		return KU_MEMORY_ALLOCATION_ERROR;
	}

	memcpy( cnv_info, &mbcs_data, sizeof(conv_info) );

	cnv_info->static_info = static_info;
//	cnv_info->cashed = 0;
	cnv_info->data = data;

	ret = mbcs_load( cnv_info, raw_data + static_info->size );
	if( KU_FAILURE( ret ) )
	{
		ku_free( cnv_info );
		return ret;
	}

	*pcnv_info = cnv_info;
	return KU_OK;
}

static int is_conv_acceptable( const char* name, const char* type, void* ctx, kudata_info_t* info )
{
	return info->size >= sizeof(kudata_info_t)
		&& info->is_big_endian == KU_IS_BIG_ENDIAN
		&& info->data_format[ 0 ] == 'c'
		&& info->data_format[ 1 ] == 'o'
		&& info->data_format[ 2 ] == 'n'
		&& info->data_format[ 3 ] == 'v'
		&& info->format_version[ 0 ] == 1;
}

static int load_converter_from_data( const char* name, conv_info** pcnv_info )
{
	int ret;
	kudata_t* data;

	ret = kudata_open( name, "cnv", is_conv_acceptable, NULL, &data );
	if( KU_FAILURE( ret ) )
	{
		return ret;
	}

	ret = make_conv_info( data, pcnv_info );
	if( KU_FAILURE( ret ) )
	{
		kudata_close( data );
	}

	return ret;
}

static int kuconv_load( const char* name, conv_info** pcnv_info )
{
	conv_info* cnv_info;

	cnv_info = get_cached_conv_info( name );
	if( cnv_info == NULL )
	{
		int ret = load_converter_from_data( name, &cnv_info );
		if( KU_FAILURE( ret ) )
		{
			return ret;
		}

		cache_conv_info( cnv_info );
	}
	else
	{
		++cnv_info->ref_cnt;
	}

	*pcnv_info = cnv_info;
	return KU_OK;
}

static int load_conv_info( const char* name, conv_info** pcnv_info )
{
	int ret;
	const char* converter;
	conv_info* cnv_info;

	if( strlen( name ) >= KUCONV_MAX_CONVERTER_NAME_LENGTH )
		return KU_INVALID_ARG;

	ret = kuconv_get_converter_name( name, &converter );
	if( ret != KU_OK || converter == NULL )
	{
		converter = name;
	}

	cnv_info = (conv_info*) get_algorithmic_conv_info( converter );
	if( cnv_info == NULL  )
	{
		kumutex_lock( &g_cache_mtx );
		ret = kuconv_load( name, &cnv_info );
		kumutex_unlock( &g_cache_mtx );
		if( KU_FAILURE( ret ) )
		{
			return ret;
		}
	}

	*pcnv_info = cnv_info;
	return KU_OK;
}

static void kuconv_unload( conv_info* info )
{
	if( --info->ref_cnt == 0 && !info->cashed )
	{
		delete_conv_info( info );
	}
	
}

static void unload_conv_info( conv_info* info )
{
	if( info->ref_cnt != -1 )
	{
		kumutex_lock( &g_cache_mtx );
		kuconv_unload( info );
		kumutex_unlock( &g_cache_mtx );
	}
}

static int create_converter_from_info( conv_info* info, kuconv* cnv, kuconv** pcnv )
{
//	kuconv* cnv;
	int8_t is_alloc;

	if( cnv == NULL )
	{
		cnv = ku_malloc( sizeof(kuconv) );
		if( cnv == NULL )
		{
			return KU_MEMORY_ALLOCATION_ERROR;
		}
		
		is_alloc = 1;
	}
	else
	{
		is_alloc = 0;
	}

	memset( cnv, 0, sizeof(kuconv) );

	cnv->is_alloc = is_alloc;
	cnv->info = info;
	cnv->to_ucs_callback = KUCONV_TO_UCS_CALLBACK_SUBSTITUTE;
	cnv->from_ucs_callback = KUCONV_FROM_UCS_CALLBACK_SUBSTITUTE;
    cnv->sub_char1 = cnv->info->static_info->sub_char1;
    cnv->sub_char_len = cnv->info->static_info->sub_char_len;
    memcpy( cnv->sub_char, cnv->info->static_info->sub_char, cnv->sub_char_len );

	if( pcnv != NULL )
	{
		*pcnv = cnv;
	}
	
	return KU_OK;
}

int kuconv_create_converter( const char* name, kuconv* cnv, kuconv** pcnv )
/* int kuconv_open( const char* name, kuconv_t* pcnv ) */
{
	int ret;
	conv_info* info;

	if( KU_FAILURE( ret = load_conv_info( name, &info ) ) )
		return ret;

	if( KU_FAILURE( ret = create_converter_from_info( info, cnv, pcnv ) ) )
	{
		unload_conv_info( info );
		return ret;
	}

	return KU_OK;
}

int kuconv_close( kuconv_t cnv )
{
	if( cnv == NULL )
		return KU_INVALID_ARG;

	if( cnv->info->ref_cnt != -1 )	/* optimization */
	{
		unload_conv_info( cnv->info );
	}
	cnv->info = NULL;

	if( cnv->is_alloc )
	{
		ku_free( cnv );
	}

	return KU_OK;
}

int32_t kuconv_flush_cache( void )
{
	conv_info* cnv_info;
	int32_t flush_count = 0;

	kumutex_lock( &g_cache_mtx );

	if( g_cnv_info_hash != NULL )
	{
		int32_t pos = -1;
		const kuhash_elem_t* e;

		while( ( e = kuhash_next_elem( g_cnv_info_hash, &pos ) ) != NULL )
		{
			cnv_info = (conv_info*) e->value;
			
			if( cnv_info->ref_cnt == 0 )
			{
				
				kuhash_remove_elem( g_cnv_info_hash, e );
				cnv_info->cashed = 0;
				delete_conv_info( cnv_info );
				
				++flush_count;
			}
			
		}
	}

	kumutex_unlock( &g_cache_mtx );

	return flush_count;
}
