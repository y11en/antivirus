

#include <kulib/kuconv.h>
#include <kudata_int.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#if defined(WIN32)
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

#define MAX_FILES 10000

typedef struct __file_info
{
	char* name;
	int name_len;
	uint32_t name_offset;

	char* file_name;
	int32_t file_size;
	uint32_t file_offset;

} file_info_t;

static file_info_t g_files[ MAX_FILES ];
static int g_file_count;


static int32_t g_names_total_size;


static kudata_header_t g_data_info = 
{
	{ 'K', 'U', 'D', 'T' },
	{
		sizeof( kudata_info_t ),

		KU_IS_BIG_ENDIAN,
		0,

		{ 'd', 'a', 't', 'a' },
		{ 1, 0, 0, 0 }
	}
};


static int compare_files( const void *file1, const void *file2 );

static void parse_file( FILE* fp );
static void add_file( char* file );

static const char* get_base_name( const char* filename );
static char* get_full_path( const char* filename );


static void write_output( FILE* fp );
static void fwrite32( FILE* fp, uint32_t val );


static int g_verbose;
static char* g_source_dir = ".";

void help( FILE* fp )
{
	fprintf( fp,
		"usage: gendata -options <input file>\n" 
		"options:\n"
		"    -o=<output_file> - specifies output file name\n"
		"    -s=<source_dir> - specifies direcory of source files\n"
		);
}


int main( int argc, char* argv[] )
{
	int arg_ind = 1;
	char* output_filename = NULL;
	FILE* fp_in;
	FILE* fp_out;


	if( argc == 1 )
	{
		help( stdout );
		exit( 0 );
	}

	for( ; arg_ind < argc; ++arg_ind )
	{
		if( *argv[ arg_ind ] != '-' )
			break;

		if( !strncmp( argv[ arg_ind ], "-o=", 3 ) )
		{
			output_filename = argv[ arg_ind ] + 3;
		}
		else if( !strncmp( argv[ arg_ind ], "-s=", 3 ) )
		{
			g_source_dir = argv[ arg_ind ] + 3;
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
		else
		{
			fprintf( stderr, "error: unknown option.\n" );
			help( stderr );
			exit( 1 );
		}
	}
	
	if( arg_ind == argc )
	{
		fprintf( stderr, "error: input file name not specified.\n" );
		help( stderr );
		exit( 1 );
	}

	if( output_filename == NULL )
	{
		fprintf( stderr, "error: output file name not specified.\n" );
		help( stderr );
		exit( 1 );
	}



	fp_in = fopen( argv[ arg_ind ], "r" );
	if( fp_in == NULL )
	{
		fprintf( stderr, "error: can't open input file '%s'.\n", argv[ arg_ind ] );
		exit( 1 );
	}

	parse_file( fp_in );

	fclose( fp_in );

	
	
	

	fp_out = fopen( output_filename, "w+b" );
	if( fp_out == NULL )
	{
		fprintf( stderr, "error: can't open output file '%s'.\n", output_filename );
		exit( 1 );
	}

	write_output( fp_out );

	fclose( fp_out );

	return 0;
}

static int compare_files( const void *file1, const void *file2 )
{
	return strcmp( ( (file_info_t*) file1 )->name, ( (file_info_t*) file2 )->name );
}

static const char* skip_ws( const char* s ) 
{
    while( *s == ' ' || *s == '\t' )
	{
        ++s;
    }

    return s;
}

static char* prepare_line( char* line, char** pend )
{
	char* end;
	char c;

	for( end = line; ( c = *end ) != '\0'; ++end )
	{
		if( c == '#' || c == '\r' || c == '\n' )
		{
			break;
		}
	}

	while( end > line && ( ( c = *( end - 1 ) ) == ' ' || c == '\t' ) ) 
	{
		--end;
	}
	*end = '\0';

	if( pend != NULL )
	{
		*pend = end;
	}

	return (char*) skip_ws( line );
}

static void parse_file( FILE* fp )
{
	char line[ 4096 ];
	char* s;


	while( fgets( line, sizeof(line), fp ) != NULL )
	{
		s = prepare_line( line, NULL );

		if( *s == '\0' )
		{
			continue;
		}

#if FILE_SEP_CHAR != FILE_ALT_SEP_CHAR
		{
			char* t;
			while( t = strchr( s, FILE_ALT_SEP_CHAR ) )
			{
				*t = FILE_SEP_CHAR;
			}
		}
#endif
		
		add_file( s );
				
	}
	
}


static void add_file( char* filename )
{
	int len;
	char* s;
	const char* base;
	char* full_path;
	
	FILE* fp;
	int32_t file_size;

	printf( "Adding \'%s\'.\n", filename );

	if( g_file_count == MAX_FILES )
	{
        fprintf( stderr, "gendata: too many source data files, maximum is %d\n", MAX_FILES );
        exit( 1 );
	}


	base = get_base_name( filename );
	len = strlen( base ) + 1;	
	s = (char*) malloc( len );
	memcpy( s, base, len );

	g_files[ g_file_count ].name = s;
	g_files[ g_file_count ].name_len = len;

	g_names_total_size += len;

	full_path = get_full_path( filename );
	g_files[ g_file_count ].file_name = full_path;

    /* try to open the file */
    fp = fopen( full_path, "rb" );
    if( fp == NULL ) 
	{
        fprintf( stderr, "gendata: can't open file from list '%s'\n", full_path );
        exit( 1 );
    }

	if( fseek( fp, 0, SEEK_END ) != 0 
		|| ( file_size = ftell( fp ) ) == -1L )
	{
        fprintf( stderr, "gendata: can't get size of file from list '%s'\n", full_path );
        exit( 1 );
	}

	fclose( fp );

	g_files[ g_file_count ].file_size = file_size;

	++g_file_count;
}


static const char* get_base_name( const char* filename )
{
    const char* basename;
		
	basename = strrchr( filename, FILE_SEP_CHAR );

    if( basename != NULL ) 
        filename = basename + 1;

#if FILE_ALT_SEP_CHAR != FILE_SEP_CHAR
	basename = strrchr( filename, FILE_ALT_SEP_CHAR );
    if( basename != NULL ) 
        filename = basename + 1;
#endif

	return filename;
}

static char* get_full_path( const char* filename )
{
	char* full_path;
	int len;

	len = strlen( g_source_dir ) + 1 + strlen( filename ) + 1;
	full_path = (char*) malloc( len );

	strcpy( full_path, g_source_dir );
	strcat( full_path, FILE_SEP_STRING );
	strcat( full_path, filename );

	return full_path;
}

static void write_output( FILE* fp )
{
	int i;
	uint32_t name_offset;
	uint32_t file_offset;
	uint32_t file_size;


	qsort( g_files, g_file_count, sizeof(g_files[0]), compare_files );

	name_offset = sizeof(uint32_t) + sizeof(uint32_t) * 2 * g_file_count;
	file_offset = ( name_offset + g_names_total_size + 0xf ) & ~0xf;
	for( i = 0; i < g_file_count; ++i )
	{
		g_files[ i ].name_offset = name_offset;
		name_offset += g_files[ i ].name_len;
		
		g_files[ i ].file_offset = file_offset;
		file_offset += ( g_files[ i ].file_size + 0xf ) & ~0xf;
	}

	fwrite( &g_data_info, sizeof(g_data_info), 1, fp );
	
	fwrite32( fp, g_file_count );
	for( i = 0; i < g_file_count; ++i )
	{
		fwrite32( fp, g_files[ i ].name_offset );
		fwrite32( fp, g_files[ i ].file_offset );
	}

	for( i = 0; i < g_file_count; ++i )
	{
		fwrite( g_files[ i ].name, g_files[ i ].name_len, 1, fp );
	}

	file_size = sizeof(uint32_t) + sizeof(uint32_t) * 2 * g_file_count + g_names_total_size;
	for( i = 0; i < g_file_count; ++i )
	{
		int32_t nread;
		FILE* cur_fp;
		uint8_t buf[ 4096 ];
			
		file_size &= 0xf;
		if( file_size )
		{
			int n = 0x10 - file_size;
			char b = 0;
			while( n-- ) fwrite( &b, 1, 1, fp );
		}

		cur_fp = fopen( g_files[ i ].file_name, "rb" );
		if( cur_fp == NULL ) 
		{
			fprintf( stderr, "gendata: can't open file from list '%s'\n", g_files[ i ].file_name );
			exit( 1 );
		}


		for( nread = 0;;)
		{
			size_t n = fread( buf, 1, sizeof(buf), cur_fp );
			if( n <= 0 )
			{
				break;
			}
			
			nread += n;
			fwrite( buf, 1, n, fp );
		}

		fclose( cur_fp );

		file_size = g_files[ i ].file_size;

		if( nread != g_files[ i ].file_size )
		{
			fprintf( stderr, "gendata: can't read all data from file '%s'\n", g_files[ i ].file_name );
			exit( 1 );
		}
	}

	file_size &= 0xf;
	if( file_size )
	{
		int n = 0x10 - file_size;
		char b = 0;
		while( n-- ) fwrite( &b, 1, 1, fp );
	}

}

static void fwrite32( FILE* fp, uint32_t val )
{
	fwrite( &val, sizeof(val), 1, fp );
}

/*
static char* alloc_string( int len )
{
	int used = g_string_store_used + len;
	char* ret;

	if( used > STRING_STORE_SIZE )
	{
        fprintf( stderr, "gendata: can't alloc string, out of memory\n" );
        exit( 1 );
	}

	ret = g_string_store + g_string_store_used;
	g_string_store_used = used;

	return ret;
}
*/