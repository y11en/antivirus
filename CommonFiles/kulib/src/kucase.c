

#include <kulib/kucase.h>

#include "kuinit_int.h"

#include "kumutex.h"
#include "kudata.h"
#include "ku_str.h"

static kumutex_t g_case_mtx;
static kudata_t* g_case_data;

static uint16_t* g_upper_tbl;
static uint16_t* g_lower_tbl;

int _kuconv_case_init( void )
{
	return kumutex_init( &g_case_mtx );
}

int _kuconv_case_term( void )
{
	kumutex_lock( &g_case_mtx );

	if( g_case_data != NULL )
	{
		kudata_close( g_case_data );
		g_case_data = NULL;
	}

	kumutex_unlock( &g_case_mtx );
	
	return kumutex_destroy( &g_case_mtx );
}


static int is_acceptable( const char* name, const char* type, void* ctx, kudata_info_t* info )
{
	return info->size >= sizeof(kudata_info_t)
		&& info->is_big_endian == KU_IS_BIG_ENDIAN
		&& info->data_format[ 0 ] == 'c'
		&& info->data_format[ 1 ] == 'a'
		&& info->data_format[ 2 ] == 's'
		&& info->data_format[ 3 ] == 'e'
		&& info->format_version[ 0 ] == 1;
}

static int check_data_loaded( void )
{
	int ret;
	kudata_t* case_data;
	uint32_t* data;


	kumutex_lock( &g_case_mtx );
	case_data = g_case_data;
	kumutex_unlock( &g_case_mtx );

	if( case_data != NULL )
	{
		return KU_OK;
	}


	kumutex_lock( &g_case_mtx );

	if( case_data != NULL )
	{
		kumutex_unlock( &g_case_mtx );
		return KU_OK;
	}


	if( ( ret = kudata_open( "case", "kud", is_acceptable, NULL, &case_data ) ) != KU_OK )
	{
		kumutex_unlock( &g_case_mtx );
		return ret;
	}
	data = (uint32_t*) kudata_getdata( case_data );

	g_upper_tbl = (uint16_t* )( data + 1 );
	g_lower_tbl = g_upper_tbl + data[ 0 ];

	g_case_data = case_data;
	
	kumutex_unlock( &g_case_mtx );

	return KU_OK;
}

int kucase_to_upper(
		const ucs_t* src, int32_t src_len,
		ucs_t* dst, int32_t dst_len,
		int32_t* needed )
{
	int ret;
	const ucs_t* src_end;

	if( KU_FAILURE( ret = check_data_loaded() ) )
	{
		return ret;
	}
	
	if( src_len < 0 )
	{
		src_len = ku_ucslen( src ) + 1;		
	}


	src_end = src + ( ( src_len > dst_len ) ? dst_len : src_len );

	while( src < src_end )
	{
		ucs_t c;

		c = *src++;
		if( c < 0x10000 )
		{
/*			c += (int16_t) g_upper_tbl[ g_upper_tbl[ ( c >> 8 ) & 0xff ] + c & 0xff ]; */
			c += (int16_t) g_upper_tbl[ g_upper_tbl[ (uint16_t) c >> 8 ] + ( c & 0xff ) ];
		}

		*dst++ = c;
	}

	if( needed != NULL )
	{
		*needed = src_len;
	}

	return KU_OK;
}

int kucase_to_lower(
		const ucs_t* src, int32_t src_len,
		ucs_t* dst, int32_t dst_len,
		int32_t* needed )
{
	int ret;
	const ucs_t* src_end;

	if( KU_FAILURE( ret = check_data_loaded() ) )
	{
		return ret;
	}
	
	if( src_len < 0 )
	{
		src_len = ku_ucslen( src ) + 1;		
	}


	src_end = src + ( ( src_len > dst_len ) ? dst_len : src_len );

	while( src < src_end )
	{
		ucs_t c;

		c = *src++;
		if( c < 0x10000 )
		{
/*			c += (int16_t) g_lower_tbl[ g_lower_tbl[ ( c >> 8 ) & 0xff ] + c & 0xff ]; */
			c += (int16_t) g_lower_tbl[ g_lower_tbl[ (uint16_t) c >> 8 ] + ( c & 0xff ) ];
		}

		*dst++ = c;
	}

	if( needed != NULL )
	{
		*needed = src_len;
	}

	return KU_OK;
}
