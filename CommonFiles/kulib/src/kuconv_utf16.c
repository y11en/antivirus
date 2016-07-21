
#include "kuconv_int.h"
#include "ku_utf.h"


static int utf16le_to_ucs( kuconv_to_ucs_args* args )
{
	int ret = KU_OK;
	kuconv_t cnv = args->cnv;
	const uint8_t* src = args->src;
	const uint8_t* src_end = args->src_end;
	ucs_t* dst = args->dst;
	const ucs_t* dst_end = args->dst_end;
	unsigned char* to_ucs_bytes = cnv->to_ucs_bytes;

	uint16_t ch, trail;
	int32_t to_ucs_len;

	if( cnv->to_ucs_status )
	{
		*to_ucs_bytes = (uint8_t) cnv->to_ucs_status;
		cnv->to_ucs_len = 1;
		cnv->to_ucs_status = 0;
	}

	to_ucs_len = cnv->to_ucs_len;

	while( src < src_end && dst < dst_end )
	{
		to_ucs_bytes[ to_ucs_len++ ] = *src++;
		
		if( to_ucs_len == 2 )
		{
			ch = to_ucs_bytes[ 1 ] << 8 | to_ucs_bytes[ 0 ];
			if( KU_IS_SINGLE( ch ) )
			{
				*dst++ = ch;
				to_ucs_len = 0;
			}
			else if( KU_IS_SURROGATE_LEAD( ch ) )
			{				
			}
			else
			{
				/* error: unmatched trail surrogate */
				ret = KU_ILLEGAL_CHAR_FOUND;
				break;
			}


		}
		else if( to_ucs_len == 4 )
		{
			ch = to_ucs_bytes[ 1 ] << 8 | to_ucs_bytes[ 0 ];
			trail = to_ucs_bytes[ 3 ] << 8 | to_ucs_bytes[ 2 ];

			if( KU_IS_TRAIL( trail ) )
			{
				*dst++ = KU_GET_SUPPLEMENTARY( ch, trail );
				to_ucs_len = 0;
			}
			else
			{
				if( ( src - (const uint8_t*) args->src ) >= 2 )
				{
					src -= 2;
				}
				else
				{
					cnv->to_ucs_status = 0x100 | to_ucs_bytes[ 2 ];
					--src;
				}

				to_ucs_len = 2;
				ret = KU_ILLEGAL_CHAR_FOUND;
				break;
			}

		}
	}	

	cnv->to_ucs_len = to_ucs_len;

	if( src < src_end && dst >= dst_end && KU_SUCCESS( ret ) )
	{
		ret = KU_BUFFER_OVERFLOW_ERROR;
	}

	args->src = src;
	args->dst = dst;
	return ret;
}

static int utf16le_from_ucs( kuconv_from_ucs_args* args )
{
	int ret = KU_OK;
	kuconv_t cnv = args->cnv;
	const ucs_t* src = args->src;
	const ucs_t* src_end = args->src_end;
	uint8_t* dst = (uint8_t*) args->dst;
	const uint8_t* dst_end = (const uint8_t*) args->dst_end;
	
    ucs_t ch;
	uint16_t lead, trail;

	while( src < src_end && dst < dst_end )
	{
        ch = *(src++);

		if( ch > 0x10ffff )
		{
			cnv->from_ucs_char = ch;
			ret = KU_INVALID_CHAR_FOUND;
			break;
		}

        if( ch < 0x10000 )
        {
			*dst++ = (uint8_t) ch;
			if( dst < dst_end )
			{
				*dst++ = (uint8_t) ( ch >> 8 );
			}
			else
			{
                cnv->from_ucs_overflow_buf[ 0 ] = (uint8_t) ( ch >> 8 );
                cnv->from_ucs_overflow_buf_len = 1;
                ret = KU_BUFFER_OVERFLOW_ERROR;
			}
        }
		else
		{
			lead = KU_LEAD( ch );
			trail = KU_TRAIL( ch );

			*dst++ = (uint8_t) lead;

			switch( dst_end - dst )
			{
			default:
			case 3:
				*dst = (uint8_t) ( lead >> 8 );
				dst[ 1 ] = (uint8_t) trail;
				dst[ 2 ] = (uint8_t) ( trail >> 8 );
				dst += 3;
				break;
			case 2:
				*dst = (uint8_t) ( lead >> 8 );
				dst[ 1 ] = (uint8_t) trail;
				dst += 2;

                *(cnv->from_ucs_overflow_buf) = (uint8_t) ( trail >> 8 );
				cnv->from_ucs_overflow_buf_len = 1;
                ret = KU_BUFFER_OVERFLOW_ERROR;
				break;
			case 1:
				*dst++ = (uint8_t) ( lead >> 8 );

                *(cnv->from_ucs_overflow_buf) = (uint8_t) ( trail );
                cnv->from_ucs_overflow_buf[1] = (uint8_t) ( trail >> 8 );
				cnv->from_ucs_overflow_buf_len = 2;
                ret = KU_BUFFER_OVERFLOW_ERROR;
				break;
			case 0:
                *(cnv->from_ucs_overflow_buf) = (uint8_t) ( lead >> 8 );
                cnv->from_ucs_overflow_buf[1] = (uint8_t) trail;
                cnv->from_ucs_overflow_buf[2] = (uint8_t) ( trail >> 8 );
				cnv->from_ucs_overflow_buf_len = 3;
                ret = KU_BUFFER_OVERFLOW_ERROR;
				break;
			}
		}
	}	
	
	if( src < src_end && dst >= dst_end && KU_SUCCESS( ret ) )
	{
		ret = KU_BUFFER_OVERFLOW_ERROR;
	}

	args->src = src;
	args->dst = dst;
	return ret;
}

static static_conv_info utf16le_static_data =
{
	sizeof( static_conv_info ),
	"utf-8",
	2, 4,
	{ 0xfd, 0xff, 0, 0 }, 2
};


static converter_impl utf16le_impl =
{
	utf16le_to_ucs,
	utf16le_from_ucs,
	NULL /* reset */
};


conv_info utf16le_data =
{
	-1, 0,
	NULL,
	&utf16le_static_data,
	&utf16le_impl,
	2, /* code_unit_size */
	0  /* is_big_endian */
};


