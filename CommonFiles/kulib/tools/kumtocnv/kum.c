


#include "kum.h"
#include "utils.h"
#include <kuconv_int.h>
#include <ku_utf.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#define MAX_LINE_SIZE		8096

static void print_mapping( FILE *fp, kum_mapping* m, uint8_t* char_bytes ) 
{
	int32_t i;
	static const char* map_str[] = { "<>", "->", "<-" };

	fprintf( fp, "U%04lX %s ", (long) m->u, map_str[ m->f ] );

	for( i = 0; i < m->blen; ++i ) 
	{
		fprintf( fp, "\\x%02X", char_bytes[ i ] );
	}

	fputs( "\n", fp );
}

int8_t kum_parse_bytes(
		uint8_t char_bytes[ KUCONV_MAX_CHAR_LEN ],
		const char* line,
		const char** ps )
{
    const char *s = *ps;
    char *end;
    uint8_t byte;
    int8_t blen = 0;

    for(;;)
	{
        if( blen > 0 && *s == '+' )
		{
            ++s;
        }

        if( *s != '\\' ) 
		{
            break;
        }

        if( s[ 1 ] != 'x' ||
            ( byte = (uint8_t) strtoul( s + 2, &end, 16 ), end ) != s + 4 ) 
		{
            fprintf( stderr, "kum error: byte must be formatted as \\xXX (2 hex digits) - \'%s\'\n", line );
            return -1;
        }

        if( blen == KUCONV_MAX_CHAR_LEN ) 
		{
            fprintf(stderr, "kum error: too many bytes on \'%s\'\n", line );
            return -1;
        }

        char_bytes[ blen++ ] = byte;
        s = end;
    }

    *ps = s;
    return blen;
}

static int parse_header_line( char* line, char** pkey, char** pvalue )
{
	char* s;
	char* end;
	char* key_end;
	char c;

	s = prepare_line( line, &end );
	if( *s == '\0' )
	{
		return 1;
	}

	*pkey = s;
	while( ( c = *s ) != '=' && c != ' ' && c != '\t' )
	{
		if( c == '\0' )
		{
			fprintf( stderr, "kum error: invalid header format.\n" );
			exit( 1 );			
		}

		++s;
	}

	key_end = s;
	s = (char*) skip_ws( s );
	if( key_end == *pkey || *s != '=' )
	{
		fprintf( stderr, "kum error: invalid header format.\n" );
		exit( 1 );			
	}

	*key_end = '\0';
	++s;

	s = (char*) skip_ws( s );

	if( *s != '\"' && *s != '\'' )
	{
		*pvalue = s;
	}
	else
	{
		/* Removing quotes */
		*pvalue = s + 1;
		
		if( end > *pvalue && *(end - 1) == *s )
		{
			*--end = '\0';
		}
	}
	
	return 1;
}


static void kum_read_header( kum_file* kum, FILE* fp )
{
	char line[ MAX_LINE_SIZE ];
	char* key;
	char* value;
	char* s;

	while( fgets( line, MAX_LINE_SIZE, fp ) != NULL )
	{
		s = prepare_line( line, NULL );
		if( *s == '\0' )
		{
			continue;
		}

		
		if( strcmp( s, "BEGIN CHARMAP" ) == 0 )
		{
			break;
		}


		parse_header_line( line, &key, &value );
		
//		fprintf( stderr, "\'%s\' = \'%s\'\n", key, value );

		if( strcmp( "name", key ) == 0 )
		{
			strcpy( kum->name, value );
		}
		else if( strcmp( "type", key ) == 0 )
		{
			if( strcmp( value, "SBCS" ) == 0 )
			{
				kum->conv_type = KUM_SBCS;
			}
			else if( strcmp( value, "DBCS" ) == 0 )
			{
				kum->conv_type = KUM_DBCS;
			}
			else if( strcmp( value, "MBCS" ) == 0 )
			{
				kum->conv_type = KUM_MBCS;
			}
			else
			{
				fprintf( stderr, "kum error: invalid 'type' value\n." );
				exit( 1 );				
			}
		}
		else if( strcmp( "min_char_len", key ) == 0 )
		{
			char* end;
			kum->min_char_len = strtol( value, &end, 10 );
			if( kum->min_char_len <= 0 || *end != '\0' )
			{
				fprintf( stderr, "kum warning: invalid value of 'min_char_len'.\n" );
			}
		}
		else if( strcmp( "max_char_len", key ) == 0 )
		{
			char* end;
			kum->max_char_len = strtol( value, &end, 10 );
			if( kum->max_char_len <= 0 || *end != '\0' )
			{
				fprintf( stderr, "kum warning: invalid value of 'max_char_len'.\n" );
			}
		}
		else if( strcmp( "sub_char", key ) == 0 )
		{
			uint8_t bytes[ KUCONV_MAX_CHAR_LEN ];
			int8_t len;

			s = value;
			len = kum_parse_bytes( bytes, line, &s );
			if( len < 1 || len > 4 || *s != '\0' )
			{
				fprintf( stderr, "kum error: invalid value of 'sub_char'.\n" );
				exit( 1 );
			}
			
			kum->sub_char_len = len;
			memcpy( kum->sub_char, bytes, len );
		}
		else if( strcmp( "sub_char1", key ) == 0 )
		{
			uint8_t bytes[ KUCONV_MAX_CHAR_LEN ];
			int8_t len;

			s = value;
			len = kum_parse_bytes( bytes, line, &s );
			if( len != 1 || *s != '\0' )
			{
				fprintf( stderr, "kum error: invalid value of 'sub_char'.\n" );
				exit( 1 );
			}
			
			kum->sub_char1 = bytes[ 0 ];
		}
		else if( strcmp( "state", key ) == 0 )
		{
			switch( kum->conv_type )
			{
			case KUM_SBCS:
			case KUM_DBCS:
				kum->conv_type = KUM_MBCS;
				break;
			case KUM_MBCS:
				break;
			default:
				fprintf( stderr, "kum error: 'state' entry before the 'type'.\n");
				exit( 1 );
			}

			kum_add_state( &kum->states, value );
		}
		else
		{
			fprintf( stderr, "kum warning: unknown header value '%s'.\n", key );
		}
		
	}
}

int parse_mapping_line( 
		kum_mapping* m, 
		ucs4_t* uchar, 
		uint8_t char_bytes[ KUCONV_MAX_CHAR_LEN ],
		const char* line )
{
	const char* s;
	char* end;
	uint8_t f;
	int8_t blen;

	s = skip_ws( line );

	if( *s != 'U' )
	{
		fprintf( stderr, "kum error: invalid mapping line format '%s'\n", line );
		return 0;
	}
	++s;

	*uchar = (ucs4_t) strtoul( s, &end, 16 );
	if( s == end )
	{
		fprintf( stderr, "kum error: invalid mapping line format '%s'\n", line );
		return 0;
	}
	s = skip_ws( end );

	m->u = *uchar;

	if( *s == '<' && s[ 1 ] == '>' )
	{
		f = MBCS_ROUNDTRIP;
	}
	else if( *s == '-' && s[ 1 ] == '>' )
	{
		f = MBCS_FALLBACK;
	}
	else if( *s == '<' && s[ 1 ] == '-' )
	{
		f = MBCS_REVERSE_FALLBACK;
	}
	else
	{
		fprintf( stderr, "kum error: invalid mapping line format '%s'\n", line );
		return 0;
	}

	s += 2;
	s = skip_ws( s );

	blen = kum_parse_bytes( char_bytes, line, &s );
	if( blen <= 0 )
	{
		if( blen == 0 )
		{
			fprintf( stderr, "kum error: no bytes on \'%s\'\n", line );
		}
		return 0;
	}

	if( blen <= 4 )
	{
        memcpy( m->b.bytes, char_bytes, blen );	
	}

    m->blen = blen;
    m->f = f;
	return 1;
}

static void add_mapping( kum_table* table, kum_mapping* m, uint8_t char_bytes[ KUCONV_MAX_CHAR_LEN ] )
{
    int32_t index;

    if( table->map_len >= table->map_allocated ) 
	{
        if( table->map_allocated == 0 ) 
		{
            table->map_allocated=1000;
        } else 
		{
            table->map_allocated *= 10;
        }
        table->map = (kum_mapping*) realloc( table->map, table->map_allocated * sizeof(kum_mapping) );
        if( table->map == NULL ) 
		{
            fprintf( stderr, "kum error: unable to allocate %d kum_mappings\n",
                            (int) table->map_allocated  );
            exit( 1 );
        }

        if( table->reverse_map != NULL ) 
		{
            free( table->reverse_map );
            table->reverse_map = NULL;
        }
    }

    if( m->blen > 4 && table->bytes_allocated == 0 ) 
	{
        table->bytes_allocated = 10000;
        table->bytes = (uint8_t*) malloc( table->bytes_allocated );
        if( table->bytes == NULL )
		{
            fprintf( stderr, "kum error: unable to allocate %d bytes\n",
                            (int) table->bytes_allocated );
            exit( 1 );
        }
    }
	
    if( m->blen > 4 ) 
	{
        index = table->bytes_len;
        table->bytes_len += m->blen;
        if( table->bytes_len > table->bytes_allocated )
		{
            fprintf( stderr, "kum error: too many bytes in mappings with >4 charset bytes\n" );
            exit( 1 );
        }

        memcpy( table->bytes + index, char_bytes, m->blen );
        m->b.index = index;
    }


	if( m->u >= 0x10000 )
	{
		table->unicode_mask |= KUCONV_HAS_SUPPLEMENTARY;
	}
	else if( KU_IS_SURROGATE( m->u ) )
	{
		table->unicode_mask |= KUCONV_HAS_SURROGATES;
	}

    memcpy( table->map + table->map_len++, m, sizeof(kum_mapping) );
    table->sorted = 0;
}

static int add_mapping_line( kum_file* kum, const char* line )
{
	kum_mapping m = { 0 };
	ucs4_t uchar;
	uint8_t char_bytes[ KUCONV_MAX_CHAR_LEN ];
	int32_t count;

	if( !parse_mapping_line( &m, &uchar, char_bytes, line ) )
	{
		return 0;
	}


	count = kum_count_chars( &kum->states, char_bytes, m.blen );
    if( count < 1 ) 
	{
		print_mapping( stderr, &m, char_bytes );
        return 0;
    }

    if( count > 1 ) 
	{
		fprintf( stderr, "kum error: too long multibyte sequence (>1).\n" );
        return 0;
    }

	
	add_mapping( kum->table, &m, char_bytes );

	
	return 1;
}


static void kum_read_table( kum_file* kum, FILE* fp )
{
	char line[ MAX_LINE_SIZE ];
	char* s;

	for(;;)
	{
		if( fgets( line, MAX_LINE_SIZE, fp ) == NULL )
		{
			fprintf( stderr, "kum error: incomplete mapping table\n" );
			exit( 1 );
		}

		s = prepare_line( line, NULL );
		if( *s == '\0' )
		{
			continue;
		}
		
		if( strcmp( s, "END CHARMAP" ) == 0 )
		{
			break;
		}

		if( !add_mapping_line( kum, line ) )
		{
			exit( 1 );
		}
		
	}
}

static int kum_read( kum_file* kum, FILE* fp )
{
	kum_read_header( kum, fp );

	kum_process_states( kum );

	kum_read_table( kum, fp );

	return 1;
}

kum_table* ucm_open_table() 
{
    kum_table* table= (kum_table*) malloc( sizeof(kum_table) );
    if( table == NULL )
	{
        fprintf(stderr, "kum error: unable to allocate a kum_table\n");
        exit( 1 );
    }

    memset( table, 0, sizeof(kum_table) );
    return table;
}

static void kum_init( kum_file* kum )
{
	memset( kum, 0, sizeof(kum_file) );

	kum->table = ucm_open_table();

	kum->states.flags[ 0 ] = KUM_STATE_FLAG_DIRECT;
	kum->conv_type = KUM_UNDEFINED_CONVERTER;
	kum->min_char_len = kum->max_char_len = 1;
}

kum_file* kum_open( const char* filename )
{
	kum_file* kum;
	FILE* fp;

	kum = (kum_file*) malloc( sizeof( kum_file ) );
	if( kum == NULL )
	{
		return NULL;
	}
	
	kum_init( kum );

	fp = fopen( filename, "rt" );
	if( fp == NULL )
	{
		fprintf( stderr, "kum error: can't open file \'%s\'\n", filename );

		free( kum );
		return NULL;
	}


	if( !kum_read( kum, fp ) )
	{
		free( kum );
		kum = NULL;
	}

	fclose( fp );

	return kum;
}