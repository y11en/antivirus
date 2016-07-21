
#include "kuconv_int.h"
#include "kuinit_int.h"

#include "kumutex.h"
#include "kudata.h"

static kumutex_t g_alias_mtx;

static kudata_t* g_alias_data;

static const uint16_t* g_conv_list;
static const uint16_t* g_alias_list;
static const uint16_t* g_alias_to_conv_list;
static const char* g_string_table;

static uint32_t g_conv_list_size;
static uint32_t g_alias_list_size;
static uint32_t g_alias_to_conv_list_size;
static uint32_t g_string_table_size;

#define GET_STRING( idx ) (const char*) ( g_string_table + (idx) )


int _kuconv_alias_init( void )
{
	return kumutex_init( &g_alias_mtx );
}

int _kuconv_alias_term( void )
{
	kumutex_lock( &g_alias_mtx );

	if( g_alias_data != NULL )
	{
		kudata_close( g_alias_data );
		g_alias_data = NULL;
	}

	kumutex_unlock( &g_alias_mtx );
	
	return kumutex_destroy( &g_alias_mtx );
}

static int is_acceptable( const char* name, const char* type, void* ctx, kudata_info_t* info )
{
	return info->size >= sizeof(kudata_info_t)
		&& info->is_big_endian == KU_IS_BIG_ENDIAN
		&& info->data_format[ 0 ] == 'c'
		&& info->data_format[ 1 ] == 'v'
		&& info->data_format[ 2 ] == 'a'
		&& info->data_format[ 3 ] == 'l'
		&& info->format_version[ 0 ] == 1;
}

static int check_data_loaded( void )
{
	int ret;
	kudata_t* alias_data;

	kumutex_lock( &g_alias_mtx );
	alias_data = g_alias_data;
	kumutex_unlock( &g_alias_mtx );


	if( alias_data == NULL )
	{
		char* data;
		uint32_t* table;
		uint32_t table_size;
		uint32_t offset;

		if( ( ret = kudata_open( "alias", "kud", is_acceptable, NULL, &alias_data ) ) != KU_OK )
		{
			return ret;
		}

		data = (char*) kudata_getdata( alias_data );
		table = (uint32_t*) data;

		table_size = table[ 0 ];
		if( table_size < 4 )
		{
			kudata_close( alias_data );
			return KU_INVALID_FORMAT_ERROR;
		}


		kumutex_lock( &g_alias_mtx );

		if( g_alias_data == NULL )
		{
			g_alias_data = alias_data;
			alias_data = NULL;

			g_conv_list_size = table[ 1 ];
			g_alias_list_size = table[ 2 ];
			g_alias_to_conv_list_size = table[ 3 ];
			g_string_table_size = table[ 4 ];

			offset = sizeof(uint32_t) + sizeof(uint32_t) * table_size;
			g_conv_list = (const uint16_t*) ( data + offset );

			offset += g_conv_list_size;
			g_alias_list = (const uint16_t*) ( data + offset );

			offset += g_alias_list_size;
			g_alias_to_conv_list = (const uint16_t*) ( data + offset );

			offset += g_alias_to_conv_list_size;
			g_string_table = (const char*) ( data + offset );

			g_conv_list_size /= 2;
			g_alias_list_size /= 2;
			g_alias_to_conv_list_size /= 2;
		}

		kumutex_unlock( &g_alias_mtx );

		if( alias_data != NULL )
		{
			kudata_close( alias_data );
		}

	}

	return KU_OK;
}

static uint32_t find_converter( const char* alias )
{
	uint32_t start, limit, mid, prev_mid;
	int res;

	start = 0;
	limit = g_alias_to_conv_list_size;
	prev_mid = limit;
	for(;;)
	{
		mid = ( start + limit ) / 2;
		if( prev_mid == mid )
		{
			break;
		}

		res = kuconv_compare_names( alias, GET_STRING( g_alias_list[ mid ] ) );
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
			return g_alias_to_conv_list[ mid ];
		}

		prev_mid = mid;
	}

	return UINT32_MAX;
}

int kuconv_get_converter_name( const char* alias, const char** converter )
{
	int ret;
	uint32_t cnv;

	if( ( ret = check_data_loaded() ) != KU_OK )
		return ret;

	cnv = find_converter( alias );
	if( cnv < g_conv_list_size )
		*converter = GET_STRING( g_conv_list[ cnv ] );
	else
		*converter = NULL;

	return KU_OK;
}


int kuconv_get_aliases_count( uint16_t* paliases_count )
{
	int ret;

	if( ( ret = check_data_loaded() ) != KU_OK )
		return ret;

	*paliases_count = g_alias_list_size;
	return KU_OK;
}




#define __tolower( c ) \
	( ( 'A' <= c && c <= 'Z' ) ? ( c ) + ( 'a' - 'A' ) : ( c ) )

int kuconv_compare_names( const char* name1, const char* name2 )
{
    int ret;
    char c1, c2;

    for(;;)
	{
        while( ( c1 = *name1 ) == '-' || c1 == '_' || c1 == ' ' ) ++name1;
        while( ( c2 = *name2 ) == '-' || c2 == '_' || c2 == ' ' ) ++name2;

        /* Case-insensitive comparison */
        ret = (int) (unsigned char) __tolower( c1 ) - (int) (unsigned char) __tolower( c2 );
        if( ret != 0 )
		{
            return ret;
        }

        if( c1 == '\0' )
		{
			return 0;
		}

        ++name1;
        ++name2;
    }
}
