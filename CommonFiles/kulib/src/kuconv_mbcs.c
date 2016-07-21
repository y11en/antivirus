

#include "kuconv_mbcs.h"

#include "kuconv_int.h"

#include "ku_utf.h"

#define KUCONV_TO_U_USE_FALLBACK( cnv )		1

#define IS_PRIVATE_USE( c ) ( (uint32_t) ( (c) - 0xe000 ) < 0x1900 || (uint32_t) ( (c) - 0xf0000 ) < 0x20000 )
#define FROM_U_USE_FALLBACK( use_fallback, c ) ( (use_fallback) || IS_PRIVATE_USE( c ) )
#define KUCONV_FROM_U_USE_FALLBACK( cnv, c ) FROM_U_USE_FALLBACK( (cnv)->use_fallback, c )


int mbcs_load( conv_info* cnv_info, const uint8_t* raw_data )
{
	const _mbcs_header* header = (const _mbcs_header*) raw_data;
	kuconv_mbcs_data* mbcs_data = &cnv_info->mbcs_data;

	cnv_info->code_unit_size = 1;
	cnv_info->is_big_endian = -1;

    mbcs_data->type = (uint8_t) header->type;

	mbcs_data->count_states = header->count_states;
	mbcs_data->count_to_u_fallbacks = header->count_to_u_fallbacks;
	mbcs_data->states = (const int32_t (*)[ 256 ]) ( raw_data + sizeof(_mbcs_header) );
	mbcs_data->to_u_fallbacks = (const _mbcs_to_u_fallback*) ( mbcs_data->states + header->count_states );
	mbcs_data->unicode_code_units = (const uint16_t*) ( raw_data + header->offset_to_u_code_units );

	mbcs_data->from_u_table = (const uint16_t*) ( raw_data + header->offset_from_u_table );
	mbcs_data->from_u_bytes = (const uint8_t*) ( raw_data + header->offset_from_u_bytes );
	mbcs_data->from_u_bytes_len = header->from_u_bytes_len;
	
	mbcs_data->uni_flags = cnv_info->static_info->uni_flags;

	return KU_OK;
}



static ucs4_t mbcs_get_fallback( kuconv_mbcs_data* mbcs_data, uint32_t offset )
{
    const _mbcs_to_u_fallback* to_u_fallbacks;
    uint32_t i, start, limit;

    limit = mbcs_data->count_to_u_fallbacks;
    if( limit > 0 )
	{
        to_u_fallbacks = mbcs_data->to_u_fallbacks;
        start = 0;
        while( start < limit - 1 ) 
		{
            i = ( start + limit ) / 2;
            if( offset < to_u_fallbacks[ i ].offset ) 
			{
                limit = i;
            }
			else
			{
                start = i;
            }
        }

        if( offset == to_u_fallbacks[ start ].offset ) 
		{
            return to_u_fallbacks[ start ].code_point;
        }
    }

    return 0xfffe;
}

static int to_ucs( kuconv_to_ucs_args* args )
{
	int ret = KU_OK;

    kuconv_t cnv;
    const uint8_t* src;
    const uint8_t* src_end;
    ucs_t* dst;
    const ucs_t* dst_end;

    const int32_t (*states) [ 0x100 ];
    const uint16_t* unicode_code_units;

    uint32_t offset;
    uint8_t state;
    int8_t byte_index;
    uint8_t* bytes;

    int32_t entry;
    uint8_t action;
	ucs_t c;



	cnv = args->cnv;
	src = (const uint8_t*) args->src;
	src_end = (const uint8_t*) args->src_end;
	dst = args->dst;
	dst_end = args->dst_end;

	states = cnv->info->mbcs_data.states;
    unicode_code_units = cnv->info->mbcs_data.unicode_code_units;
	
    offset = cnv->to_ucs_status;
    byte_index = cnv->to_ucs_len;
    bytes = cnv->to_ucs_bytes;

    state = (uint8_t) cnv->mode;


    while( src < src_end )
	{
		if( dst >= dst_end )
		{
			ret = KU_BUFFER_OVERFLOW_ERROR;
			break;
		}

		entry = states[ state ][ bytes[ byte_index++ ] = *src++ ];

        if( MBCS_ENTRY_IS_TRANSITION( entry ) )
		{
            state = (uint8_t) MBCS_ENTRY_TRANSITION_STATE( entry );
            offset += MBCS_ENTRY_TRANSITION_OFFSET( entry );
            continue;
        }

        cnv->mode = state;

        state = (uint8_t) MBCS_ENTRY_FINAL_STATE( entry );

        action = (uint8_t) ( MBCS_ENTRY_FINAL_ACTION( entry ) );
        if( action == MBCS_STATE_VALID_16 )
		{
            offset += MBCS_ENTRY_FINAL_VALUE_16( entry );
            c = unicode_code_units[ offset ];
            if( c < 0xfffe )
			{
                *dst++ = c;
                byte_index = 0;
            }
			else if( c == 0xfffe )
			{
                if( KUCONV_TO_U_USE_FALLBACK( cnv ) 
					&& ( entry=(int32_t) mbcs_get_fallback( &cnv->info->mbcs_data, offset ) ) != 0xfffe ) 
				{
                    *dst++ = (ucs_t) entry;
                    byte_index = 0;
                }
            }
			else
			{
                ret = KU_ILLEGAL_CHAR_FOUND;
            }
        }
		else if( action == MBCS_STATE_VALID_DIRECT_16 )
		{
            *dst++ = MBCS_ENTRY_FINAL_VALUE_16( entry );
            byte_index = 0;
        }
		else if( action == MBCS_STATE_VALID_16_PAIR )
		{
            offset += MBCS_ENTRY_FINAL_VALUE_16( entry );
            c = unicode_code_units[ offset++ ];
            if( c < 0xd800 )
			{
                *dst++ = c;
                byte_index = 0;
            }
			else if( KUCONV_TO_U_USE_FALLBACK( cnv ) ? c <= 0xdfff : c <= 0xdbff )
			{
                *dst++ = KU_GET_SUPPLEMENTARY( c & 0xdbff, unicode_code_units[ offset ] );
                byte_index = 0;
            }
			else if( KUCONV_TO_U_USE_FALLBACK( cnv ) ? ( c & 0xfffe ) == 0xe000 : c == 0xe000 )
			{
                *dst++ = unicode_code_units[ offset ];
                byte_index=0;
            }
			else if( c == 0xffff )
			{
                ret = KU_ILLEGAL_CHAR_FOUND;
            }
        }
		else if( action == MBCS_STATE_VALID_DIRECT_20 
				|| ( action == MBCS_STATE_FALLBACK_DIRECT_20 && KUCONV_TO_U_USE_FALLBACK( cnv ) ) )
		{
            *dst++ = MBCS_ENTRY_FINAL_VALUE( entry );
            byte_index = 0;
        }
		else if( action == MBCS_STATE_CHANGE_ONLY ) 
		{
			byte_index = 0;
        }
		else if( action == MBCS_STATE_FALLBACK_DIRECT_16 )
		{
            if( KUCONV_TO_U_USE_FALLBACK( cnv ) )
			{
                *dst++ = MBCS_ENTRY_FINAL_VALUE_16( entry );
                byte_index = 0;
            }
        }
		else if( action == MBCS_STATE_UNASSIGNED )
		{
        }
		else if( action == MBCS_STATE_ILLEGAL )
		{
            ret = KU_ILLEGAL_CHAR_FOUND;
        }
		else
		{
            byte_index = 0;
        }

        offset = 0;

        if( byte_index == 0 )
		{

        }
		else if( KU_FAILURE( ret ) ) 
		{
            break;
        }
		else /* byteIndex > 0 - unassigned sequence  */
		{
			ret = KU_INVALID_CHAR_FOUND;
            break;
        }
    }

    cnv->to_ucs_status = offset;
    cnv->mode = state;
    cnv->to_ucs_len = byte_index;

    args->src = (const char*) src;
    args->dst = dst;

	return ret;
}

static int from_ucs( kuconv_from_ucs_args* args )
{
	int ret = KU_OK;

    kuconv_t cnv;
    const ucs_t* src;
    const ucs_t* src_end;
    uint8_t* dst;
/*    const uint8_t* dst_end; */
    int32_t target_capacity;

    const uint16_t* table;
    const uint8_t* bytes;
    const uint8_t* p;

    uint8_t type;
    uint8_t flags;

	ucs_t c;

    uint32_t stage2_entry;
    uint32_t value;
    int32_t length;

	uint32_t max_src;

	cnv = args->cnv;

    type = cnv->info->mbcs_data.type;
    flags = cnv->info->mbcs_data.uni_flags;

    src = args->src;
    src_end = args->src_end;
    dst = (uint8_t*) args->dst;
    target_capacity = args->dst_end - args->dst;

    table = cnv->info->mbcs_data.from_u_table;
	bytes = cnv->info->mbcs_data.from_u_bytes;


	max_src = ( cnv->info->mbcs_data.uni_flags & KUCONV_HAS_SUPPLEMENTARY ) ? 0x10ffff : 0xffff;

    while( src < src_end )
	{
        if( target_capacity > 0 )
		{
            c = *src++;

/*			
			if( c > 0x10ffff )
			{
				ret = KU_INVALID_CHAR_FOUND;
				break;
			}
*/
			
			if( c > max_src )
			{
				ret = KU_INVALID_CHAR_FOUND;
				/* ret = KU_ILLEGAL_CHAR_FOUND; */
				break;
			}

            stage2_entry = MBCS_STAGE_2_FROM_U( table, c );

            switch( type ) 
			{
            case MBCS_OUTPUT_1:
                value = MBCS_VALUE_1_FROM_STAGE_2( bytes, stage2_entry, c );
				length = 1;
				break;
            case MBCS_OUTPUT_2:
                value = MBCS_VALUE_2_FROM_STAGE_2( bytes, stage2_entry, c );
                if( value <= 0xff )
				{
                    length = 1;
                }
				else
				{
					length = 2;
                }
                break;
            case MBCS_OUTPUT_3:
                p = MBCS_POINTER_3_FROM_STAGE_2( bytes, stage2_entry, c );
                value =( (uint32_t) *p << 16 ) | ( (uint32_t) p[ 1 ] << 8 ) | p[ 2 ];
                if( value <= 0xff )
				{
                    length = 1;
                }
				else if( value <= 0xffff )
				{
                    length = 2;
                }
				else
				{
                    length = 3;
                }
                break;
            case MBCS_OUTPUT_4:
                value = MBCS_VALUE_4_FROM_STAGE_2( bytes, stage2_entry, c );
                if( value <= 0xff )
				{
                    length = 1;
                }
				else if( value <= 0xffff ) 
				{
                    length = 2;
                }
				else if( value <= 0xffffff )
				{
                    length = 3;
                }
				else
				{
                    length = 4;
                }
                break;
            default:
                /* never occur */
                break;
            }

            if( !( MBCS_FROM_U_IS_ROUNDTRIP( stage2_entry, c )
					|| ( KUCONV_FROM_U_USE_FALLBACK( cnv, c ) && value != 0 ) )
			) 
			{
				ret = KU_ILLEGAL_CHAR_FOUND;
                break;
            }


            if( length <= target_capacity )
			{
				switch(length)
				{
				case 4:
					*dst++ = (uint8_t) ( value >> 24 );
				case 3:
					*dst++ = (uint8_t) ( value >> 16 );
				case 2:
					*dst++ = (uint8_t) ( value >> 8 ) ;
				case 1:
					*dst++ = (uint8_t) value;
				default:
					/* never occur */
					break;
				}
                
				target_capacity -= length;
            }
			else
			{
                uint8_t* overflow_buf;

                length -= target_capacity;
                overflow_buf = (uint8_t *) cnv->from_ucs_overflow_buf;
                switch( length )
				{
                case 3:
                    *overflow_buf++ = (uint8_t) ( value >> 16 );
                case 2:
                    *overflow_buf++ = (uint8_t) ( value >> 8 );
                case 1:
                    *overflow_buf = (uint8_t) value;
                default:
                    /* never occur */
                    break;
                }
                cnv->from_ucs_overflow_buf_len = (int8_t) length;

                value >>= 8*length;
                switch( target_capacity )
				{
                case 3:
                    *dst++ = (uint8_t) ( value >> 16 );
                case 2:
                    *dst++ = (uint8_t) ( value >> 8 );
                case 1:
                    *dst++ = (uint8_t) value;
                default:
                    /* never occur */
                    break;
                }

                target_capacity = 0;
                ret = KU_BUFFER_OVERFLOW_ERROR;
                break;
            }

            continue;
        } else {
            ret = KU_BUFFER_OVERFLOW_ERROR;
            break;
        }
    }


//	if( KU_FAILURE(ret) && ret != KU_BUFFER_OVERFLOW_ERROR )
		cnv->from_ucs_char = c;


    args->src = src;
    args->dst = (char*) dst;

	return ret;
}

static void reset( kuconv_t cnv, reset_type_t type )
{

}




static converter_impl mbcs_impl =
{
	to_ucs,
	from_ucs,
	reset
};

conv_info mbcs_data =
{
	1, 0,
	NULL,
	NULL,
	&mbcs_impl
};
