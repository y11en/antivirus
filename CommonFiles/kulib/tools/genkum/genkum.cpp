
#include "converter.h"
#include "utils.h"
#include <kuconv_mbcs.h>

#include <stdio.h>
#include <time.h>

#include <list>
#include <set>

#if defined(_WIN32)
# define FILE_SEP_CHAR '\\'
# define FILE_ALT_SEP_CHAR '/'
# define FILE_SEP_STRING "\\"
# define FILE_ALT_SEP_STRING "/"
#else
# define FILE_SEP_CHAR '/'
# define FILE_ALT_SEP_CHAR '/'
# define FILE_SEP_STRING "/"
# define FILE_ALT_SEP_STRING "/"
#endif


static void generate_mapping( converter* cnv, const char* codepage, encoding_info* info );

static char* g_dst_dir = ".";
static char* g_interface = NULL;
static int g_verbose = 0;
static int g_only_list = 0;
static int g_icu_mapping_format = 0;


void help( FILE* fp )
{
	fprintf( fp,
		"usage: genkum [-options] [encoding_filter_list]\n" 
		"options:\n"
		"    -d=<output_dir> - specifies output directory\n"
		"    -i=<interface> - specifies interface used to generate output files\n"
		"    -l - list available encodings, don't generate .kum files\n"
		);
}

int main( int argc, char* argv[] )
{
	for( int arg_ind = 1; arg_ind < argc; ++arg_ind )
	{
		if( *argv[ arg_ind ] != '-' )
			break;

		if( !strncmp( argv[ arg_ind ], "-d=", 3 ) )
		{
			g_dst_dir = argv[ arg_ind ] + 3;
		}
		else if( !strncmp( argv[ arg_ind ], "-i=", 3 ) )
		{
			g_interface = argv[ arg_ind ] + 3;
		}
		else if( !strcmp( argv[ arg_ind ], "-l" ) )
		{
			g_only_list = 1;
		}
		else if( !strcmp( argv[ arg_ind ], "-v" ) )
		{
			g_verbose = 1;
		}
		else if( !strcmp( argv[ arg_ind ], "-h" ) )
		{
			help( stdout );
			exit( 0 );
		}
		else if( !strcmp( argv[ arg_ind ], "-icu" ) )
		{
			g_icu_mapping_format = 1;
		}
		else
		{
			fprintf( stderr, "error: unknown option.\n" );
			help( stderr );
			exit( 1 );
		}
	}


	converter_info* cnv_info;	
	cnv_info = converter_info::get_converter_info( g_interface );
	if( !cnv_info )
	{
		printf( "error: can't create converter info for interface \"%s\".\n", g_interface );
		exit( 1 );
	}

	argc -= arg_ind, argv += arg_ind;
	encodings_map encodings;
	cnv_info->get_encodings( encodings, argc, argv );

	for( encodings_map::iterator it = encodings.begin(); 
			it != encodings.end(); ++it )
	{
		printf( "%s - %s - %s\n", it->first.c_str(), it->second->name, it->second->description );

		if( cnv_info->is_ignorable_encoding( it->first ) )
		{
			printf( "  encoding ignored\n" );
			continue;
		}
		
		if( g_only_list )
		{
			continue;
		}

		converter* cnv = cnv_info->create_converter( it->first );

		generate_mapping( cnv, it->first.c_str(), it->second );

		cnv_info->destroy_converter( cnv );
	}

	converter_info::release_converter_info( cnv_info );

	return 0;
}

void print_byte_str( FILE* fp, const char* buf, int len )
{
	for( int i = 0; i < len; ++i )
	{
		fprintf( fp, "\\x%02x", (uint8_t) buf[ i ] );
	}
}

void add_byte_info( byte_info used_bytes, const char* buf, int len )
{
	uint8_t ( *cur_bytes )[ 0x100 ]  = &used_bytes[ 0 ];

	while( len-- )
	{
		uint8_t c = (uint8_t) *buf;
		uint8_t new_byte_val = ( len ? BYTE_CONT : BYTE_END );
			
		if( (*cur_bytes)[ c ] != BYTE_UNUSED
			&& (*cur_bytes)[ c ] != new_byte_val )
		{
			printf( "Warning!: Overwriting of byte info\n" );
		}
		
		(*cur_bytes)[ c ] = new_byte_val;

		++cur_bytes;
		++buf;
	}
}

typedef std::list< std::string > mb_list;

static void _make_mb_list( char* str, int len, byte_info used_bytes, mb_list& list )
{
	uint8_t ( *cur_bytes )[ 0x100 ]  = &used_bytes[ len ];

	for( int i = 0; i < 0x100; ++i )
	{
		str[ len ] = i;

		if( (*cur_bytes)[ i ] == BYTE_END  )
		{
			list.push_back( std::string( str, len + 1 ) );
		}
		else if( (*cur_bytes)[ i ] == BYTE_CONT )
		{
			_make_mb_list( str, len + 1, used_bytes, list );
		}
	}
}


static void make_mb_list( byte_info used_bytes, mb_list& list )
{
	char str[ MAX_MB_LEN ];

	_make_mb_list( str, 0, used_bytes, list );
}

static void test_lead_byte( byte_info used_bytes, converter* cnv );

static void output_header( FILE* fp, converter* cnv, codepage_info* cp_info, byte_info used_bytes );
static void output_trail( FILE* fp, converter* cnv );
static void print_mapping_line( FILE* fp, ucs_t uchar, const char* str, int len, int mtype );


typedef std::map< ucs_t, std::string > uni_to_mb_map;
typedef std::map< std::string, ucs_t > mb_to_uni_map;
typedef std::set< std::string > mb_set;
typedef std::map< ucs_t, mb_set > mb_to_uni_by_uni_map;

static void generate_mapping( converter* cnv, const char* codepage, encoding_info* info )
{
	ucs_t uchar;
	byte_info used_bytes;
	int max_mb_len = 0;
	int min_mb_len = 100;
	int PUA_used = 0;

	uni_to_mb_map uni_to_mb;

	mb_to_uni_map mb_to_uni;
	mb_to_uni_by_uni_map mb_to_uni_by_uni;
	

	memset( used_bytes, 0, sizeof(used_bytes) );

	for( uchar = 0; uchar < MAX_UNICODE_VALUE; ++uchar )
	{
		char buf[ 20 ];
		int len;

		// Skipping surrogates
		if( uchar == 0xd800 )
		{
			uchar = 0xe000;
		}

		len = cnv->from_ucs( uchar, buf, sizeof(buf) );

		if( len <= 0 )
		{
			continue;
		}

		uni_to_mb[ uchar ] = std::string( buf, len );

		if( len > MAX_MB_LEN )
		{
			printf( "Warning!: Very long multibyte character: " );
			printf( "U%04x ", uchar );
			print_byte_str( stdout, buf, len );
			printf( "\n" );
		}
		else
		{
//			printf( "U%04x ", uchar );
//			print_byte_str( stdout, buf, len );
//			printf( "\n" );
			
			add_byte_info( used_bytes, buf, len );
		}

		if( uchar >= 0xe000 && uchar < 0xf900 )
		{
			PUA_used = 1;
		}

		if( len > max_mb_len )
		{
			max_mb_len = len;
		}

		if( len < min_mb_len )
		{
			min_mb_len = len;
		}
	}

	printf( "  min: %d, max: %d, PUA: %d\n", min_mb_len, max_mb_len, PUA_used );

	if( max_mb_len == 2 )
	{
		test_lead_byte( used_bytes, cnv );
	}

	mb_list list;
	make_mb_list( used_bytes, list );
	
	for( mb_list::iterator it = list.begin(); it != list.end(); ++it )
	{
		int len;
		ucs_t wbuf[ 16 ];
		
//		print_byte_str( stdout, it->c_str(), it->size() );
//		printf( "\n" );

		len = cnv->to_ucs( it->c_str(), it->size(), wbuf, sizeof(wbuf) / sizeof(wbuf[0]) );

		if( len <= 0 )
		{
			continue;
		}


//			print_byte_str( stdout, it->c_str(), it->size() );
//			printf( " " );
//			for( int i = 0; i < len; ++i )
//			{
//				printf( "<U%04x>", wbuf[ i ] );
//			}
//			printf( "\n" );

		if( len > 1 )
		{
			printf( "Warning!: Multibyte char converted to multiple unicode characters.\n" );

			continue;
		}

		mb_to_uni[ *it ] = *wbuf;
		mb_to_uni_by_uni[ *wbuf ].insert( *it );
	}
	
	//----------------------------------------------------------------------------------------
	
	codepage_info cp_info;
	cnv->get_codepage_info( &cp_info );
	cp_info.min_char_len = min_mb_len;
	cp_info.max_char_len = max_mb_len;

	char filename[ 100 ];
//	sprintf( filename, "%s" FILE_SEP_STRING "%s-%s.kum", g_dst_dir, cnv->get_name(), get_os_name()  );
	sprintf( filename, "%s" FILE_SEP_STRING "%s.kum", g_dst_dir, cnv->get_name() );
	FILE* fp = fopen( filename, "wt" );
//	FILE* fp = stdout;

	if( fp == NULL )
	{
		printf( "error: can't open output file \'%s\'.\n", filename );
		exit( 1 );
	}

	output_header( fp, cnv, &cp_info, used_bytes );

	for( uchar = 0; uchar < MAX_UNICODE_VALUE; ++uchar )
	{
		uni_to_mb_map::iterator it = uni_to_mb.find( uchar );
		if( it != uni_to_mb.end() )
		{
			int mtype = MBCS_ROUNDTRIP;
			mb_to_uni_map::iterator mb_it = mb_to_uni.find( it->second );
			if( mb_it == mb_to_uni.end() || mb_it->second != uchar )
			{
				mtype = MBCS_FALLBACK;
			}

			print_mapping_line( fp, uchar, it->second.c_str(), it->second.size(), mtype );
		}

		if( uchar == 0x30fb )
		{
			continue;
		}

		// Generating reverse mapping
		mb_to_uni_by_uni_map::iterator mb_by_uni_it = mb_to_uni_by_uni.find( uchar );
		if( mb_by_uni_it != mb_to_uni_by_uni.end() )
		{
			mb_set& mbset = mb_by_uni_it->second;

			for( mb_set::iterator mb_it = mbset.begin(); mb_it != mbset.end(); ++mb_it  )
			{
				if( it != uni_to_mb.end() && *mb_it == it->second )
				{
					continue;
				}

				print_mapping_line( fp, uchar, mb_it->c_str(), mb_it->size(), MBCS_REVERSE_FALLBACK );
			}
		}		
	}

	output_trail( fp, cnv );
	fclose( fp );
}

static void test_lead_byte( byte_info used_bytes, converter* cnv )
{

    for( int i = 0; i < 0x100; i++ )
    {
        if( used_bytes[ 0 ][ i ] != BYTE_UNUSED )
		{
            continue;
		}
        
        for( int j = 0; j < 0x100; j++ )
        {
			if( used_bytes[ 0 ][ j ] == BYTE_UNUSED )
			{
				continue;
			}
			
			char src[ 2 ];
			ucs_t wbuf[ 16 ];

			src[ 0 ] = i;
			src[ 1 ] = j;
		
			int len = cnv->to_ucs( src, 2, wbuf, sizeof(wbuf) / sizeof(wbuf[0]) );
			if( len > 0 )
			{
                printf( "Adding additional lead byte at 0x%02X.\n", i );

				used_bytes[ 0 ][ i ] = BYTE_CONT;
				break;
			}
		}
	}
}

static void print_mapping_line( FILE* fp, ucs_t uchar, const char* str, int len, int mtype )
{
	if( g_icu_mapping_format )
	{
		static const char* mapping_sign[] = { "|0", "|1", "|3" };

		fprintf( fp, "<U%04X> ", uchar );
		for( int i = 0; i < len; ++i )
		{
			fprintf( fp, "\\x%02X", (uint8_t) str[ i ] );
		}

		fprintf( fp, " %s\n", mapping_sign[mtype] );
	}
	else
	{
		static const char* mapping_sign[] = { "<>", "->", "<-" };
				
		fprintf( fp, "U%04x %s ", uchar, mapping_sign[mtype] );
		print_byte_str( fp, str, len );
		fprintf( fp,  "\n" );
	}
}


static void output_states( FILE* fp, byte_info used_bytes )
{
	uint8_t ( *cur_bytes )[ 0x100 ];
	int s;
	int printed;

	for( int level = 0; level < MAX_MB_LEN; ++level )
	{
		cur_bytes = &used_bytes[ level ];
		printed = 0;

		s = 0;
		for( int i = 0; i < 0x100; ++i )
		{
			if( i != ( 0x100 - 1 ) && (*cur_bytes)[ i ] == (*cur_bytes)[ i + 1 ] )
			{
				continue;
			}
			
			if( (*cur_bytes)[ s ] != BYTE_UNUSED )
			{
				if( printed )
				{
					fprintf( fp, ", " );
				}
				else
				{
					fprintf( fp, "state = " );
					printed = 1;
				}

				fprintf( fp, "%x", s  );
				if( s != i )
				{
					fprintf( fp, "-%x", i );
				}

				if( (*cur_bytes)[ s ] == BYTE_CONT )
				{
					fprintf( fp, ":%d", level + 1 );
				}
			}

			s = i + 1;
		}

		if( printed )
		{
			fprintf( fp, "\n" );
		}
	}
}

static void output_header( FILE* fp, converter* cnv, codepage_info* cp_info, byte_info used_bytes )
{
    time_t cur_time;
    char time_str[ 100 ];


    time( &cur_time );
    strftime( time_str, sizeof(time_str), "%b %d %H:%M %Z %Y", localtime( &cur_time ) );
    
    fprintf( fp, "#\n");
    fprintf( fp, "# Kaspersky Unicode Mapping (KUM) file\n");
    fprintf( fp, "#\n");
    fprintf( fp, "# Created: %s\n", time_str );
    fprintf( fp, "# Created by genkum tool.\n", time_str );
    fprintf( fp, "# Created on %s using %s interface.\n", get_os_name(), cnv->get_interface_name() );
    fprintf( fp, "#\n");
    fprintf( fp, "\n");

//    fprintf( fp, "name = %s-%s\n", cnv->get_name(), get_os_name() );
    fprintf( fp, "name = %s\n", cnv->get_name() );
    fprintf( fp, "min_char_len = %d\n", cp_info->min_char_len );
    fprintf( fp, "max_char_len = %d\n", cp_info->max_char_len );
    fprintf( fp, "sub_char = " ); print_byte_str( fp, cp_info->defchar, strlen( cp_info->defchar ) ); fprintf( fp, "\n" );
    fprintf( fp, "type = " );
	if( cp_info->max_char_len <= 1 )
	{
		fprintf( fp, "SBCS\n" );
	}
	else
	{
		if( cp_info->min_char_len == 2 && cp_info->max_char_len == 2 )
		{
			fprintf( fp, "DBCS\n" );
		}
		else
		{
			fprintf( fp, "MBCS\n" );
		}

		output_states( fp, used_bytes );
	}

    fprintf( fp, "\n");
    fprintf( fp, "BEGIN CHARMAP\n" );
    fprintf( fp, "\n");
	
}

static void output_trail( FILE* fp, converter* cnv )
{
    fprintf( fp, "\n");
    fprintf( fp, "END CHARMAP\n" );
}

