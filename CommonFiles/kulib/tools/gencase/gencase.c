
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "utils.h"
#include "casetbl.h"
#include <kudata_int.h>


static kudata_header_t g_data_info = 
{
	{ 'K', 'U', 'D', 'T' },
	{
		sizeof( kudata_info_t ),

		KU_IS_BIG_ENDIAN,
		0,

		{ 'c', 'a', 's', 'e' },
		{ 1, 0, 0, 0 }
	}
};

typedef struct __case_file
{
	int upper;
	casetbl upper_tbl;
	int lower;
	casetbl lower_tbl;
} case_file;


case_file g_file;


void case_file_init( case_file* file )
{
	file->upper = 0;
	casetbl_init( &file->upper_tbl );
	file->lower = 0;
	casetbl_init( &file->lower_tbl );
}

static void read_tables( const char* filename, case_file* file );
static void read_table( casetbl* tbl, int upper, FILE* fp );
static void write_table( case_file* file, const char* output_filename );

static void help( FILE* fp )
{
	fprintf( fp,
		"usage: gencase -options <input file>\n" 
		"options:\n"
		"    -o=<output_file> - specifies output file name\n"
		);
}

int main( int argc, char* argv[] )
{
	int arg_ind = 1;
	char* output_filename = "case.kud";

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

	case_file_init( &g_file );

	read_tables( argv[ arg_ind ], &g_file );
	write_table( &g_file, output_filename );	


	return 0;
}


#define MAX_LINE_SIZE 8096

static void read_tables( const char* filename, case_file* file )
{
	FILE* fp;
	char line[ MAX_LINE_SIZE ];
	char* s;

	fp = fopen( filename, "rt" );
	if( fp == NULL )
	{
		fprintf( stderr, "error: can't open input file '%s'\n", filename );
		exit( 1 );
	}
	

	for(;;)
	{
		if( fgets( line, MAX_LINE_SIZE, fp ) == NULL )
		{
			break;
		}

		s = prepare_line( line, NULL );
		if( *s == '\0' )
		{
			continue;
		}
		
		if( strcmp( s, "BEGIN UPPERCASE" ) == 0 )
		{
			if( file->upper )
			{
				fprintf( stderr, "error: too many upper case tables.\n");
				exit( 1 );
			}
			read_table( &file->upper_tbl, 1, fp );
			file->upper = 1;
		}
		else if( strcmp( s, "BEGIN LOWERCASE" ) == 0 )
		{
			if( file->lower )
			{
				fprintf( stderr, "error: too many lower case tables.\n");
				exit( 1 );
			}

			read_table( &file->lower_tbl, 0, fp );
			file->lower = 1;
		}
		else
		{
			fprintf( stderr, "error: invalid line '%s'\n", line );
			exit( 1 );
		}

	}


	fclose( fp );
	
	
}

static int parse_mapping_line( const char* line, ucs4_t* from_uchar, ucs4_t* to_uchar )
{
	const char* s;
	char* end;

	s = skip_ws( line );

	if( *s != 'U' )
	{
		fprintf( stderr, "error: invalid mapping line format '%s'\n", line );
		return 0;
	}
	++s;

	*from_uchar = (ucs4_t) strtoul( s, &end, 16 );
	if( s == end || ( *end != ' ' && *end != '\t' ) )
	{
		fprintf( stderr, "error: invalid mapping line format '%s'\n", line );
		return 0;
	}
	s = skip_ws( end );
	
	if( *s != 'U' )
	{
		fprintf( stderr, "error: invalid mapping line format '%s'\n", line );
		return 0;
	}
	++s;
	
	*to_uchar = (ucs4_t) strtoul( s, &end, 16 );
	if( s == end || *skip_ws( end ) != '\0' )
	{
		fprintf( stderr, "error: invalid mapping line format '%s'\n", line );
		return 0;
	}


	return 1;
}


static int add_mapping_line( casetbl* tbl, char* line )
{
	ucs4_t from_uchar, to_uchar;

	if( !parse_mapping_line( line, &from_uchar, &to_uchar ) )
	{
		return 0;
	}

	
//	fprintf( stderr, "%lx %lx\n", from_uchar, to_uchar );

	return casetbl_add( tbl, from_uchar, to_uchar );
}

static void read_table( casetbl* tbl, int upper, FILE* fp )
{
	char line[ MAX_LINE_SIZE ];
	char* s;

	for(;;)
	{
		if( fgets( line, MAX_LINE_SIZE, fp ) == NULL )
		{
			fprintf( stderr, "error: incomplete mapping table\n" );
			exit( 1 );
		}

		s = prepare_line( line, NULL );
		if( *s == '\0' )
		{
			continue;
		}
		
		if( strcmp( s, upper ? "END UPPERCASE" : "END LOWERCASE" ) == 0 )
		{
			break;
		}

		if( !add_mapping_line( tbl, line ) )
		{
			exit( 1 );
		}
		
	}

}


static void write_table( case_file* file, const char* output_filename )
{
	FILE* fp;
	uint32_t upper_size;
	uint32_t lower_size;

	if( !file->upper )
	{
		fprintf( stderr, "error: upper case table not defined.\n");
		exit( 1 );
	}

	if( !file->lower )
	{
		fprintf( stderr, "error: lower case table not defined.\n");
		exit( 1 );
	}


	fp = fopen( output_filename, "w+b" );
	if( fp == NULL )
	{
		fprintf( stderr, "error: can't open output file '%s'.\n", output_filename );
		exit( 1 );
	}

	fwrite( &g_data_info, sizeof(g_data_info), 1, fp );
	
	upper_size = casetbl_size( &file->upper_tbl );
	lower_size = casetbl_size( &file->lower_tbl );

	fwrite( &upper_size, sizeof(upper_size), 1, fp );
//	fwrite( &lower_size, sizeof(lower_size), 1, fp );

	casetbl_write( &file->upper_tbl, fp );
	casetbl_write( &file->lower_tbl, fp );
	
	fclose( fp );
}
