

#include "kum.h"
#include "mbcsconv.h"
#include <kudata_int.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>


static kudata_header_t g_data_info = 
{
	{ 'K', 'U', 'D', 'T' },
	{
		sizeof( kudata_info_t ),

		KU_IS_BIG_ENDIAN,
		0,

		{ 'c', 'o', 'n', 'v' },
		{ 1, 0, 0, 0 }
	}
};


typedef struct __converter_data
{
	kum_file* kum;

} converter_data;

void converter_data_init( converter_data* data )
{
	memset( data, 0, sizeof(converter_data) );
}

static void read_file( converter_data* data, const char* filename );
static void read_converter( converter_data* data, const char* filename );


static void help( FILE* fp )
{
	fprintf( fp,
		"usage: kumtocnv -options <input file>\n" 
		"options:\n"
		"    -o=<output_file> - specifies output file name\n"
		);
}

int main( int argc, char* argv[] )
{
	int arg_ind = 1;
	char* output_filename = NULL;
	converter_data cnv_data;

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


	converter_data_init( &cnv_data );
	read_converter( &cnv_data, argv[ 1 ] );
	
	return 0;
}


static void read_converter( converter_data* data, const char* filename )
{
	FILE* fp_out;
	char output_filename[ 1024 ];

	kum_file* kum;
	mbcsconv* cnv;
		
	kum = kum_open( filename );
	if( kum == NULL )
	{
//		fprintf( stderr, "error: can't open input file '%s'.\n", filename );
		exit( 1 );
	}

	cnv = mbcsconv_create( kum );


	strcpy( output_filename, kum->name );
	strcat( output_filename, ".cnv" );
		
	fp_out = fopen( output_filename, "w+b" );
	if( fp_out == NULL )
	{
		fprintf( stderr, "error: can't open output file '%s'.\n", output_filename );
		exit( 1 );
	}

	fwrite( &g_data_info, sizeof(g_data_info), 1, fp_out );
	mbcsconv_write( cnv, fp_out );

	fclose( fp_out );	
}