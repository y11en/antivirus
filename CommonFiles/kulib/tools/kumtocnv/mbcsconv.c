

#include "mbcsconv.h"
#include <kuconv_int.h>
#include <kuconv_mbcs.h>

#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <search.h>



enum
{
	MBCS_STAGE_2_BLOCK_SIZE = 0x40, /* 64 = 1 << 6 */
	MBCS_STAGE_2_BLOCK_SIZE_SHIFT = 6,
	MBCS_STAGE_1_SIZE = 0x440, /* 0x110000 >> 10 */
	MBCS_STAGE_2_SIZE = 0xfbc0, /* 0x10000 - MBCS_STAGE_1_SIZE */
	MBCS_MAX_STAGE_2_USED = MBCS_STAGE_2_SIZE,
	MBCS_STAGE_2_MAX_BLOCKS = MBCS_STAGE_2_SIZE>>MBCS_STAGE_2_BLOCK_SIZE_SHIFT,

	MBCS_STAGE_2_ALL_UNASSIGNED_INDEX = 0, /* stage 1 entry for the all-unassigned stage 2 block */
	MBCS_STAGE_2_FIRST_ASSIGNED = MBCS_STAGE_2_BLOCK_SIZE, /* start of the first stage 2 block after the all-unassigned one */

	MBCS_STAGE_3_BLOCK_SIZE = 16, /* 16 = 1 << 4 */
	MBCS_STAGE_3_FIRST_ASSIGNED = MBCS_STAGE_3_BLOCK_SIZE, /* start of the first stage 3 block after the all-unassigned one */

	MBCS_MAX_FALLBACK_COUNT = 8192
};

struct mbcsconv
{
	kum_file* kum;

    uint8_t has_to_uni_fallback;
    uint8_t has_from_uni_fallback;

	/* to uni data */
    _mbcs_to_u_fallback to_u_fallbacks[ MBCS_MAX_FALLBACK_COUNT ];
    int32_t count_to_u_fallbacks;
	uint16_t* unicode_code_units;

	/* from uni data */
    uint16_t stage1[ MBCS_STAGE_1_SIZE ];
    uint32_t stage2[ MBCS_STAGE_2_SIZE ];

    uint8_t *from_u_bytes;
    uint32_t stage2_used, stage3_used;
};

static int add_table( mbcsconv* cnv );

mbcsconv* mbcsconv_create( kum_file* kum )
{
	mbcsconv* cnv;

	cnv = malloc( sizeof( mbcsconv ) );
	memset( cnv, 0, sizeof( mbcsconv ) );

	cnv->kum = kum;
	
	add_table( cnv );

	return cnv;
}

static char digit_to_hex( int d )
{
    return d < 10 ? (char) ( '0' + d ) : (char) ( 'a' - 10 + d );
}

static char* bytes_to_str( char* buf, const uint8_t* bytes, int len )
{
    char* s = buf;
    while( len-- )
	{
        *s++ = '\\';
        *s++ = 'x';
        *s++ = digit_to_hex( *bytes >> 4 );
        *s++ = digit_to_hex( *bytes & 0xf );
        ++bytes;
    }

    *s = '\0';
    return buf;
}

static int add_table_prepare( mbcsconv* cnv )
{
	int32_t to_unicode_units_count = cnv->kum->states.to_unicode_units_count;
	int32_t i;
    int32_t sum;

	cnv->unicode_code_units = malloc( to_unicode_units_count * sizeof(uint16_t) );
	if( cnv->unicode_code_units == NULL )
	{
        fprintf( stderr, "error: out of memory, unable to allocate %ld 16-bit code units\n",
				(long) to_unicode_units_count );

		return 0;
	}

	for( i = 0; i < to_unicode_units_count; ++i )
	{
		cnv->unicode_code_units[ i ] = 0xfffe;		
	}


	/* to uni */

    cnv->stage2_used = MBCS_STAGE_2_FIRST_ASSIGNED;
    cnv->stage3_used = MBCS_STAGE_3_BLOCK_SIZE * cnv->kum->max_char_len;

    for( i = 0; i < MBCS_STAGE_1_SIZE; ++i ) 
	{
        cnv->stage1[ i ] = MBCS_STAGE_2_ALL_UNASSIGNED_INDEX;
    }

	sum = 0x100000 * cnv->kum->max_char_len;
   
	cnv->from_u_bytes = (uint8_t *) malloc( sum );
    if( cnv->from_u_bytes == NULL )
	{
        fprintf( stderr, "error: unable to allocate %ld of memory for mapping from unicode\n", (long) sum );
        return 0;
    }

	/* initialize stage3 unassigned block */
    memset( cnv->from_u_bytes, 0, MBCS_STAGE_3_BLOCK_SIZE * cnv->kum->max_char_len );
	
	return 1;	
}


static int compare_fallbacks( const void* fb1, const void* fb2 ) 
{
    return ( (const _mbcs_to_u_fallback*) fb1 )->offset - ( (const _mbcs_to_u_fallback*) fb2 )->offset;
}

static void optimize_states(
		kum_states* states,
		_mbcs_to_u_fallback *to_u_fallbacks, int32_t count_to_u_fallbacks )
{
    int32_t state, cell, entry;

    for( state=0; state < states->states_count; ++state )
	{
		for( cell = 0; cell < 256; ++cell )
		{
			entry = states->states[ state ][ cell ];

			if( MBCS_ENTRY_SET_STATE( entry, 0 ) == MBCS_ENTRY_FINAL( 0, MBCS_STATE_VALID_DIRECT_16, 0xfffe ) ) 
			{
				states->states[ state ][ cell ] = MBCS_ENTRY_FINAL_SET_ACTION( entry, MBCS_STATE_UNASSIGNED );
			}
		}
    }

    if( count_to_u_fallbacks > 0 ) {
        qsort( to_u_fallbacks, count_to_u_fallbacks, sizeof(_mbcs_to_u_fallback), compare_fallbacks );
    }
}

static void add_table_postprocess( mbcsconv* cnv )
{
	optimize_states( &cnv->kum->states,
			cnv->to_u_fallbacks, cnv->count_to_u_fallbacks );

}

static int32_t find_fallback( _mbcs_to_u_fallback* to_u_fallbacks, int32_t count_to_u_fallbacks,
                 uint32_t offset) {
    int32_t i;

    if( count_to_u_fallbacks == 0 )
	{
        return -1;
    }

    for( i = 0; i < count_to_u_fallbacks; ++i )
	{
        if( offset == to_u_fallbacks[ i ].offset )
		{
            return i;
        }
    }
	
    return -1;
}

static int set_fallback( mbcsconv* cnv, uint32_t offset, ucs4_t c )
{
    int32_t i = find_fallback( cnv->to_u_fallbacks, cnv->count_to_u_fallbacks, offset );
    if( i >= 0 )
	{
        cnv->to_u_fallbacks[ i ].code_point = c;
        return 1;
    }
	else
	{
        i = cnv->count_to_u_fallbacks;
        if( i >= MBCS_MAX_FALLBACK_COUNT )
		{
            fprintf( stderr, "error: too many to_unicode fallbacks, currently at: U+%x\n", (int) c );
            return 0;
        }
		else 
		{
            cnv->to_u_fallbacks[ i ].offset = offset;
            cnv->to_u_fallbacks[ i ].code_point = c;
            cnv->count_to_u_fallbacks = i + 1;
            return 1;
        }
    }
}

static int32_t remove_fallback( mbcsconv* cnv, uint32_t offset )
{
    int32_t i = find_fallback( cnv->to_u_fallbacks, cnv->count_to_u_fallbacks, offset );
    if( i >= 0 )
	{
        _mbcs_to_u_fallback* to_u_fallbacks;
        int32_t limit, old;

        to_u_fallbacks = cnv->to_u_fallbacks;
        limit = cnv->count_to_u_fallbacks;
        old = (int32_t) to_u_fallbacks[ i ].code_point;

        to_u_fallbacks[ i ].offset = to_u_fallbacks[ limit - 1 ].offset;
        to_u_fallbacks[ i ].code_point = to_u_fallbacks[ limit - 1 ].code_point;
        cnv->count_to_u_fallbacks = limit - 1;
        return old;
    } 
	else
	{
        return -1;
    }
}


static int add_to_uni( mbcsconv* cnv, const uint8_t* bytes, int blen, ucs4_t uchar, int f )
{
	char buf[ 17 ];
	kum_states* states = &cnv->kum->states;
	int i = 0;
	int32_t entry;
	uint8_t state = 0;
    uint32_t offset = 0;
	int32_t old;


	for(;;)
	{
		entry = states->states[ state ][ bytes[ i++ ] ];
        if( MBCS_ENTRY_IS_TRANSITION( entry ) )
		{
            if( i == blen )
			{
                fprintf( stderr, "error: byte sequence too short, ends in non-final.\n" );
                return 0;
            }
            state = (uint8_t) MBCS_ENTRY_TRANSITION_STATE( entry );
            offset += MBCS_ENTRY_TRANSITION_OFFSET( entry );
        }
		else
		{
            if( i < blen )
			{
                fprintf( stderr, "error: byte sequence too long\n" );
                return 0;
            }

            switch( MBCS_ENTRY_FINAL_ACTION( entry ) )
			{
            case MBCS_STATE_ILLEGAL:
                fprintf( stderr, "error: byte sequence ends in illegal state at U+%04x<->%s\n",
						(int) uchar, bytes_to_str( buf, bytes, blen ) );
                return 0;
            case MBCS_STATE_CHANGE_ONLY:
                fprintf( stderr, "error: byte sequence ends in state-change-only at U+%04x<->%s\n",
						(int) uchar, bytes_to_str( buf, bytes, blen ) );
                return 0;
            case MBCS_STATE_UNASSIGNED:
                fprintf( stderr, "error: byte sequence ends in unassigned state at U+%04x<->%s\n",
						(int) uchar, bytes_to_str( buf, bytes, blen ) );
                return 0;
				
            case MBCS_STATE_FALLBACK_DIRECT_16:
            case MBCS_STATE_VALID_DIRECT_16:
            case MBCS_STATE_FALLBACK_DIRECT_20:
            case MBCS_STATE_VALID_DIRECT_20:
                if( MBCS_ENTRY_SET_STATE( entry, 0 ) != MBCS_ENTRY_FINAL( 0, MBCS_STATE_VALID_DIRECT_16, 0xfffe ) ) 
				{
                    if( MBCS_ENTRY_FINAL_ACTION( entry ) == MBCS_STATE_VALID_DIRECT_16 
						|| MBCS_ENTRY_FINAL_ACTION( entry ) == MBCS_STATE_FALLBACK_DIRECT_16 ) 
					{
                        old = MBCS_ENTRY_FINAL_VALUE( entry );
                    }
					else
					{
                        old = 0x10000 + MBCS_ENTRY_FINAL_VALUE( entry );
                    }

                    fprintf( stderr, "error: duplicate codepage byte sequence at U+%04x<->%s see U+%04x\n",
							(int) uchar, bytes_to_str( buf, bytes, blen ), (int) old );
                    return 0;
                }

                /* reassign the correct action code */
                entry = MBCS_ENTRY_FINAL_SET_ACTION( entry, 
						( f == MBCS_ROUNDTRIP ? MBCS_STATE_VALID_DIRECT_16 : MBCS_STATE_FALLBACK_DIRECT_16 ) 
						+ ( uchar >= 0x10000 ? 1 : 0 ) 
					);

                if( uchar >= 0x10000 )
				{
                    entry = MBCS_ENTRY_FINAL_SET_VALUE( entry, uchar - 0x10000 );
                }
				else
				{
                    entry = MBCS_ENTRY_FINAL_SET_VALUE( entry, uchar );
                }

                states->states[ state ][ bytes[ i - 1 ] ] = entry;
                break;
				
            case MBCS_STATE_VALID_16:
                offset += MBCS_ENTRY_FINAL_VALUE_16( entry );

                if( ( old = cnv->unicode_code_units[offset] ) != 0xfffe || ( old = remove_fallback( cnv, offset ) ) != -1 ) 
				{
                    fprintf( stderr, "error: duplicate codepage byte sequence at U+%04x<->%s see U+%04x\n",
							(int) uchar, bytes_to_str( buf, bytes, blen ), (int) old );
					return 0;
                }

                if( uchar >= 0x10000 )
				{
                    fprintf( stderr, "error: code point does not fit into valid-16-bit state\n." );
                    return 0;
                }

                if( f != MBCS_ROUNDTRIP )
				{
                    /* assign only if there is no precise mapping */
                    if( cnv->unicode_code_units[ offset ] == 0xfffe )
					{
                        return set_fallback( cnv, offset, uchar );
                    }
                }
				else
				{
                    cnv->unicode_code_units[ offset ] = (uint16_t) uchar;
                }
                break;
				
            case MBCS_STATE_VALID_16_PAIR:
                offset += MBCS_ENTRY_FINAL_VALUE_16( entry );

                old = cnv->unicode_code_units[ offset ];
                if( old < 0xfffe )
				{
                    int32_t real;
                    if( old < 0xd800 )
					{
                        real = old;
                    }
					else if( old <= 0xdfff )
					{
                        real = 0x10000 + ( ( old & 0x3ff ) << 10 ) + ( ( cnv->unicode_code_units[ offset + 1 ] ) & 0x3ff );
                    }
					else /* old<=0xe001 */ 
					{
                        real = cnv->unicode_code_units[ offset + 1 ];
                    }

                    fprintf( stderr, "error: duplicate codepage byte sequence at U+%04x<->%s see U+%04x\n",
							(int) uchar, bytes_to_str( buf, bytes, blen ), (int) old );

                    return 0;
                }

                if( f != MBCS_ROUNDTRIP )
				{
                    /* assign only if there is no precise mapping */
                    if( old <= 0xdbff || old == 0xe000 ) 
					{
                        /* do nothing */
                    } 
					else if( uchar < 0x10000 ) 
					{
                        /* set a BMP fallback code point as a pair with 0xe001 */
                        cnv->unicode_code_units[ offset++ ]= 0xe001;
                        cnv->unicode_code_units[ offset ] = (uint16_t) uchar;
                    } else {
                        /* set a fallback surrogate pair with two second surrogates */
                        cnv->unicode_code_units[ offset++ ] = (uint16_t) ( 0xdbc0 + ( uchar >> 10 ) );
                        cnv->unicode_code_units[ offset ] = (uint16_t) ( 0xdc00 + ( uchar & 0x3ff ) );
                    }
                }
				else
				{
                    if( uchar < 0xd800 ) 
					{
                        /* set a BMP code point */
                        cnv->unicode_code_units[ offset ] = (uint16_t) uchar;
                    }
					else if( uchar < 0x10000 )
					{
                        /* set a BMP code point above 0xd800 as a pair with 0xe000 */
                        cnv->unicode_code_units[ offset++] = 0xe000;
                        cnv->unicode_code_units[ offset ] = (uint16_t) uchar;
                    }
					else
					{
                        /* set a surrogate pair */
                        cnv->unicode_code_units[ offset++ ] = (uint16_t) ( 0xd7c0 + ( uchar >> 10 ) );
                        cnv->unicode_code_units[ offset ] = (uint16_t) ( 0xdc00 + ( uchar & 0x3ff ) );
                    }
                }
                break;
				
            default:
                fprintf( stderr, "internal error: byte sequence reached reserved action code.\n" );
                return 0;
            }

			return 1;
		}
	}
}

static int add_from_uni( mbcsconv* cnv, const uint8_t* bytes, int blen, ucs4_t uchar, int f )
{
	char buf[ 17 ];
	uint32_t index;
    const uint8_t* pb;
    uint8_t *p;
    uint32_t b;
    uint32_t old;
	int max_char_len = cnv->kum->max_char_len;



    if( f != MBCS_ROUNDTRIP && blen == 1 && *bytes == 0 )
	{
        fprintf( stderr, "error: unable to encode a fallback from U+%04x to 0x%02x\n", (int) uchar, *bytes );

        return 0;
    }


    index = uchar >> 10;
    if( cnv->stage1[ index ] == MBCS_STAGE_2_ALL_UNASSIGNED_INDEX )
	{
		cnv->stage1[ index ] = (uint16_t) cnv->stage2_used;
		cnv->stage2_used += MBCS_STAGE_2_BLOCK_SIZE;

        if( cnv->stage2_used > MBCS_MAX_STAGE_2_USED )
		{
            fprintf( stderr, "error: too many stage 2 entries at U+%04x<->0x%s\n",
					(int) uchar, bytes_to_str( buf, bytes, blen ) );
            return 0;
        }
    }

    index = cnv->stage1[ index ] + ( ( uchar >> 4 ) &0x3f );
    if( cnv->stage2[ index ] == 0 )
	{
        cnv->stage2[index] = ( cnv->stage3_used / 16 ) / max_char_len;
        memset( cnv->from_u_bytes + cnv->stage3_used, 0, 16 * max_char_len );
        cnv->stage3_used += 16 * cnv->kum->max_char_len;

        if( cnv->stage3_used > ( 0x100000 * (uint32_t) max_char_len ) )
		{
            fprintf(stderr, "error: too many code points at U+%04x<->0x%s\n",
					(int) uchar, bytes_to_str( buf, bytes, blen ) );
            return 0;
        }
    }
	

    pb = bytes;
    b = 0;
    switch( blen )
	{
    case 4:
        b = *pb++;
    case 3:
        b = ( b << 8 ) | *pb++;
    case 2:
        b = ( b << 8 ) | *pb++;
    case 1:
    default:
        b = ( b << 8 ) | *pb++;
        break;
    }
	

    old = 0;
    p = cnv->from_u_bytes + ( 16 * (uint32_t) (uint16_t) cnv->stage2[ index ] + ( uchar & 0xf ) ) * max_char_len;
    switch( max_char_len )
	{
    case 1:
        old = *(uint8_t*) p;
        *(uint8_t*) p = (uint8_t) b;
        break;

    case 2:
        old = *(uint16_t*) p;
        *(uint16_t*) p = (uint16_t) b;
        break;
    case 3:
        old = (uint32_t) *p << 16;
        *p++ = (uint8_t) ( b >> 16 );
        old |= (uint32_t) *p << 8;
        *p++ = (uint8_t) ( b >> 8 );
        old |= *p;
        *p = (uint8_t) b;
        break;
    case 4:
        old = *(uint32_t*) p;
        *(uint32_t *) p = b;
        break;
    default:
        /* will never occur */
        break;
    }

    if( ( cnv->stage2[ index ] & ( 1UL << ( 16 + ( uchar & 0xf ) ) ) ) != 0 || old != 0 ) 
	{
        fprintf( stderr, "error: duplicate unicode code point at U+%04x<->0x%s see 0x%02x\n",
            (int) uchar, bytes_to_str( buf, bytes, blen ), (int)old );
        return 0;
    }
	
    if( f == MBCS_ROUNDTRIP )
	{
        cnv->stage2[ index ] |= ( 1UL << ( 16 + ( uchar & 0xf ) ) );
    }


	return 1;
}

static int add_table( mbcsconv* cnv )
{
	int32_t i;
	kum_table* table = cnv->kum->table;
	kum_mapping* m;
	int ret = 1;

	if( !add_table_prepare( cnv ) )
	{
		return 0;
	}

	m = table->map;
	for( i = 0; i < table->map_len; ++i, ++m )
	{
		
		switch( m->f )
		{
		case MBCS_ROUNDTRIP:
			ret &= add_to_uni( cnv, m->b.bytes, m->blen, m->u, m->f );
			ret &= add_from_uni( cnv, m->b.bytes, m->blen, m->u, m->f );
			break;

		case MBCS_FALLBACK:
			ret &= add_from_uni( cnv, m->b.bytes, m->blen, m->u, m->f );
			cnv->has_from_uni_fallback = 1;
			break;

		case MBCS_REVERSE_FALLBACK:
			ret &= add_to_uni( cnv, m->b.bytes, m->blen, m->u, m->f );
			cnv->has_to_uni_fallback = 1;
			break;
		}

	}

	add_table_postprocess( cnv );

	return ret;
}



void mbcsconv_write( mbcsconv* cnv, FILE* fp )
{
	static_conv_info info;
	_mbcs_header header;

	int32_t i;
    int32_t stage1_size;


	memset( &info, 0, sizeof(info) );

	info.size = sizeof(static_conv_info);
	
	strcpy( info.name, cnv->kum->name );

	info.min_bytes_per_char = cnv->kum->min_char_len;
	info.max_bytes_per_char = cnv->kum->max_char_len;
	
	info.sub_char_len = cnv->kum->sub_char_len;
	memcpy( info.sub_char, cnv->kum->sub_char, cnv->kum->sub_char_len );
	info.sub_char1 = cnv->kum->sub_char1;

    info.has_to_uni_fallback = cnv->has_to_uni_fallback;
    info.has_from_uni_fallback = cnv->has_from_uni_fallback;

	info.uni_flags = cnv->kum->table->unicode_mask;

	fwrite( &info, sizeof(static_conv_info), 1, fp );
	
	/* ----------------------------------------- */

	if( cnv->kum->table->unicode_mask & KUCONV_HAS_SUPPLEMENTARY )
	{
		stage1_size = MBCS_STAGE_1_SIZE;
	}
	else
	{
		stage1_size = 0x40;
	}
	
	for( i = 0; i < stage1_size; ++i )
	{
		cnv->stage1[ i ] += (uint16_t) stage1_size / 2;
	}

    cnv->stage2_used *= 4;

	/* round to 4 bytes */
//    cnv->stage2_used = (cnv->stage2_used + 3 ) & ~3;
//    cnv->stage3_used = (cnv->stage3_used + 3 ) & ~3;




	memset( &header, 0, sizeof(_mbcs_header) );
	header.type = cnv->kum->max_char_len - 1;

	header.count_states = cnv->kum->states.states_count;
	header.count_to_u_fallbacks = cnv->count_to_u_fallbacks;
	header.offset_to_u_code_units = sizeof(_mbcs_header) 
			+ cnv->kum->states.states_count * 0x100 * sizeof(int32_t)
			+ cnv->count_to_u_fallbacks * sizeof(_mbcs_to_u_fallback);
	
	header.offset_from_u_table = header.offset_to_u_code_units 
			+ cnv->kum->states.to_unicode_units_count * sizeof(uint16_t);
	header.offset_from_u_bytes = header.offset_from_u_table
			+ stage1_size * sizeof(uint16_t) + cnv->stage2_used;
	header.from_u_bytes_len = cnv->stage3_used;

	fwrite( &header, sizeof(_mbcs_header), 1, fp );

	fwrite( cnv->kum->states.states, cnv->kum->states.states_count * 0x100 * sizeof(int32_t), 1, fp );
	fwrite( cnv->to_u_fallbacks, cnv->count_to_u_fallbacks * sizeof(_mbcs_to_u_fallback), 1, fp );
	fwrite( cnv->unicode_code_units, cnv->kum->states.to_unicode_units_count * sizeof(uint16_t), 1, fp );

    fwrite( cnv->stage1, stage1_size * sizeof(uint16_t), 1, fp );
	fwrite( cnv->stage2, cnv->stage2_used, 1, fp );
    fwrite( cnv->from_u_bytes, cnv->stage3_used, 1, fp );
}


