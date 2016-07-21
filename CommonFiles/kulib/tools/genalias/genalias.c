
#include <kulib/kuconv.h>
#include <kudata_int.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_LINE_SIZE		8096


#define MAX_CONV_COUNT		0x10000
#define MAX_ALIAS_COUNT		0x10000
#define MAX_LIST_SIZE		0x10000

#define STRING_STORE_SIZE	0x10000		// ( 1 << sizeof(int16_t) )
static char g_string_store[ STRING_STORE_SIZE ];
static int g_string_used;

static int16_t g_converters[ MAX_CONV_COUNT ];
static int g_converters_count;

#define GET_ALIAS_STR( cnv )	( g_string_store + ( cnv ) )



typedef struct __alias_list
{
	int count;
	uint16_t* list;
} alias_list;

static alias_list g_alias_list[ MAX_CONV_COUNT ];
static int g_alias_list_count;

static uint16_t g_all_aliases[ MAX_ALIAS_COUNT ];
static int g_all_aliases_count;

//static uint16_t g_alias_lists[ MAX_LIST_SIZE ];
//static int g_alias_lists_size;



static kudata_header_t g_data_info = 
{
	{ 'K', 'U', 'D', 'T' },
	{
		sizeof( kudata_info_t ),

		KU_IS_BIG_ENDIAN,
		0,

		{ 'c', 'v', 'a', 'l' },
		{ 1, 0, 0, 0 }
	}
};

static int g_verbose = 0;
static int g_line_num;



static int chomp( char* line );
static int is_ws( char c );
static void parse_file( FILE* fp );
static void parse_line( const char* line );

static uint16_t add_converter( uint16_t cnv_str );
static void add_alias( uint16_t alias_str, uint16_t converter );
static void add_to_all_alias( uint16_t alias_str );

static uint16_t alloc_string( const char* s, int length );
static int compare_names( const char* name1, const char* name2 );

static void write_output( FILE* fp );

static void fwrite8( FILE* fp, uint8_t val );
static void fwrite16( FILE* fp, uint16_t val );
static void fwrite32( FILE* fp, uint32_t val );


void help( FILE* fp )
{
	fprintf( fp,
		"usage: genalias -options <input file>\n" 
		"options:\n"
		"    -o=<output_file> - specifies output file name\n"
		);
}

int main( int argc, char* argv[] )
{
	int arg_ind = 1;
	FILE* fp_in;
	FILE* fp_out;
	char* output_filename = NULL;

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
		else if( !strcmp( argv[ arg_ind ], "-v" ) )
		{
			g_verbose = 1;
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

	fwrite( &g_data_info, sizeof(g_data_info), 1, fp_out );

	write_output( fp_out );

	fclose( fp_out );

	return 0;
}

static void parse_file( FILE* fp )
{
	char line[ MAX_LINE_SIZE ];
	int line_size = 0;
	char buf[ MAX_LINE_SIZE ];
	int buf_size = 0;
	int valid_line = 0;


	g_line_num = 0;

	for(;;)
	{

		valid_line = 0;
		while( fgets( buf, MAX_LINE_SIZE, fp ) != NULL )
		{
			buf_size = chomp( buf );

			if( line_size == 0 || ( buf_size != 0 && is_ws( *buf ) ) )
			{
				strcpy( line + line_size, buf );
				line_size += buf_size;
			}
			else if( line_size > 0 )
			{
				valid_line = 1;		
				break;
			}

			g_line_num++;
		}
		
		if( !valid_line )
			break;

		if( is_ws( *line ) )
		{
			fprintf( stderr, "error at line %d: alias can't start with space.\n", g_line_num );			
			exit( 1 );
		}

		parse_line( line );

		
		if( buf_size > 0 )
		{
			strcpy( line, buf );
			line_size = buf_size;
		}
		else
		{
			line_size = 0;
		}

		g_line_num++;
	}
	

}

static void parse_line( const char* line )
{
	const char* start;
	const char* end;
	uint16_t cnv_str;
	uint16_t converter;

	if( *line == '\0' ) return;


	start = line;
	while( *line != '\0' && !is_ws( *line ) ) ++line;
	end = line;

	cnv_str = alloc_string( start, end - start );
//	fprintf( stderr, "%s ", GET_ALIAS_STR( cnv_str ) );
	converter = add_converter( cnv_str );
	add_alias( cnv_str, converter );
	add_to_all_alias( cnv_str );


	for(;;)
	{
		uint16_t alias_str;

		// Skip white space
		while( is_ws( *line ) ) ++line;

		if( *line == '\0' ) break;

		start = line;
		while( *line != '\0' && !is_ws( *line ) ) ++line;
		end = line;
		
		alias_str = alloc_string( start, end - start );
//		fprintf( stderr, "%s ", GET_ALIAS_STR( alias_str ) );
		add_alias( alias_str, converter );
		add_to_all_alias( alias_str );
	}
	
//	fprintf( stderr, "\n" );
}

static void add_alias( uint16_t alias_str, uint16_t converter )
{
	int	idx;
	int idx2;
	char* alias;
	alias_list* list;

	// TODO: Check alias name.

	// Find dup.
	alias = GET_ALIAS_STR( alias_str );
	for( idx = 0; idx < g_converters_count; ++idx )
	{
		for( idx2 = 0; idx2 < g_alias_list[ idx ].count ; ++idx2 )
		{
			uint16_t cur_alias_str = g_alias_list[ idx ].list[ idx2 ];

			if( compare_names( alias, GET_ALIAS_STR( cur_alias_str ) ) == 0 )
			{
				fprintf( stderr, "error at line %d: duplicate alias '%s' found.\n", g_line_num, alias );
				exit( 1 );
			}
		}
	}

	
	list = g_alias_list + converter;
	list->list = realloc( list->list, ( list->count + 1 ) * sizeof(list->list[0]) );

	list->list[ list->count++ ] = alias_str;
}

static void add_to_all_alias( uint16_t alias_str )
{
    if( g_all_aliases_count >= MAX_ALIAS_COUNT )
	{
		fprintf( stderr, "error at line %d: too many aliases defined.\n", g_line_num );
        exit( 1 );
    }

    g_all_aliases[ g_all_aliases_count++ ] = alias_str;
}


static uint16_t add_converter( uint16_t cnv_str )
{
	int i;
	char* converter;

    if( g_converters_count >= MAX_CONV_COUNT )
	{
		fprintf( stderr, "error at line %d: too many converters.\n", g_line_num );
        exit( 1 );
    }

	converter = GET_ALIAS_STR( cnv_str );
	for( i = 0; i < g_converters_count; ++i )
	{
		if( compare_names( converter, GET_ALIAS_STR( g_converters_count ) ) == 0 )
		{
			fprintf( stderr, "error at line %d: duplicate converter name found.\n", g_line_num );
			exit( 1 );
		}
	}

	g_converters[ g_converters_count ] = cnv_str;

	return g_converters_count++;
}

static uint16_t alloc_string( const char* s, int length )
{
	int used;
	uint16_t ret;


	if( length < 0 )
		length = strlen( s );

	used = g_string_used + ( length + 1 );
	
	if( used > STRING_STORE_SIZE )
	{
		fprintf( stderr, "error at line %d: string store full.\n", g_line_num );			
		exit( 1 );
	}


	memcpy( g_string_store + g_string_used, s, length );
	g_string_store[ g_string_used + length ] = '\0';

	// TODO: Check string for invalid chars

	ret = g_string_used;
	g_string_used = used;

	return ret;
}

static int compare_aliases( const void* alias1, const void* alias2 )
{
    int result = compare_names( GET_ALIAS_STR( *(uint16_t*) alias1 ), GET_ALIAS_STR( *(uint16_t*) alias2 ) );

	return result;
}


static uint16_t get_alias_conv( uint16_t alias )
{
	int idx;
	int idx2;

	for( idx = 0; idx < g_converters_count; ++idx )
	{
		for( idx2 = 0; idx2 < g_alias_list[ idx ].count; ++idx2 )
		{
			if( g_alias_list[ idx ].list[ idx2 ] == alias )
				return idx;	
		}
	}

	return idx;
}

static void resolve_aliases( uint16_t* alias_to_conv )
{
	int idx;

	for( idx = 0; idx < g_all_aliases_count; ++idx )
	{
//		fprintf( stderr, "%s\n", GET_ALIAS_STR( g_all_aliases[ idx ] ) );
		alias_to_conv[ idx ] = get_alias_conv( g_all_aliases[ idx ] );
	}
}


//static void create_

static void print_aliases( void )
{
	int idx;
	int idx2;

//	fprintf( stderr, "-----\n" );
	for( idx = 0; idx < g_converters_count; ++idx )
	{
		for( idx2 = 0; idx2 < g_alias_list[ idx ].count; ++idx2 )
		{
//			fprintf( stderr, "%s ", GET_ALIAS_STR( g_alias_list[ idx ].list[ idx2 ] ) );
		}
//		fprintf( stderr, "\n" );
	}

//	fprintf( stderr, "-----\n" );
}

static void write_output( FILE* fp )
{
	uint16_t* alias_to_conv = malloc( g_all_aliases_count * sizeof(uint16_t) );

	qsort( g_all_aliases, g_all_aliases_count, sizeof(g_all_aliases[0]), compare_aliases );
	
//	print_aliases();

	resolve_aliases( alias_to_conv );
	

	fwrite32( fp, 4 );

	fwrite32( fp, g_converters_count * sizeof(uint16_t) );
	fwrite32( fp, g_all_aliases_count * sizeof(uint16_t) );
	fwrite32( fp, g_all_aliases_count * sizeof(uint16_t) );

	fwrite32( fp, g_string_used );
	
	
	
	fwrite( g_converters, sizeof(uint16_t), g_converters_count, fp );
	
	fwrite( g_all_aliases, sizeof(uint16_t), g_all_aliases_count, fp );
	fwrite( alias_to_conv, sizeof(uint16_t), g_all_aliases_count, fp );
	

	fwrite( g_string_store, sizeof(g_string_store[0]), g_string_used, fp );


	free( alias_to_conv );
}


static int chomp( char* line )
{
	char* s = line;
	char* last_non_space = NULL;

	while( *s != 0 ) 
	{
		if( *s == '\r' || *s == '\n' || *s == '#' )
		{
			*s = '\0';
			break;
		}

		if( !is_ws( *s ) )
		{
			last_non_space = s;
		}

		++s;
	}

	if( last_non_space != NULL )
	{

		*(++last_non_space) = '\0';
		s = last_non_space;
	}

	return s - line;
}

static int is_ws( char c )
{
	return c == ' ' || c == '\t';
}


static char __tolower( char c )
{
	if( 'A' <= c && c <= 'Z' )
		return c + ( 'a' - 'A' );
	
	return c;
}

static int compare_names( const char* name1, const char* name2 ) 
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

static void fwrite8( FILE* fp, uint8_t val )
{
	fwrite( &val, sizeof(val), 1, fp );
}

static void fwrite16( FILE* fp, uint16_t val )
{
	fwrite( &val, sizeof(val), 1, fp );
}

static void fwrite32( FILE* fp, uint32_t val )
{
	fwrite( &val, sizeof(val), 1, fp );
}
