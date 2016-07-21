
#include "kuconv_int.h"


#define UTF8_IS_LEAD( c )  ( (uint8_t) ( ( c ) - 0xc0 ) < 0x3e )
#define UTF8_IS_TRAIL( c ) ( ( ( c ) & 0xc0 ) == 0x80 )

#define UTF_IS_SURROGATE( c ) ( ( ( c ) & 0xfffff800 ) == 0xd800 )

#define MAXIMUM_UTF 0x0010FFFF

static const int8_t first_byte_to_length[ 256 ] =
{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 0, 0
};

static const uint32_t offsets[ 7 ] =
{
	0, 0, 0x00003080, 0x000E2080, 0x03C82080, 0xFA082080, 0x82082080
};

static const uint32_t min_value[ 7 ] =
{
	0, 0, 0x80, 0x800, 0x10000, 0xffffffff, 0xffffffff
};


static int to_ucs( kuconv_to_ucs_args* args )
{
	int ret = KU_OK;
	kuconv_t cnv = args->cnv;
	const unsigned char* src = args->src;
	const unsigned char* src_end = args->src_end;
	ucs_t* dst = args->dst;
	const ucs_t* dst_end = args->dst_end;
	unsigned char* to_ucs_bytes = cnv->to_ucs_bytes;

	uint32_t ch, ch2;
	int32_t i, length;

	if( cnv->to_ucs_status && dst < dst_end )
	{
		length = cnv->mode;
		i = cnv->to_ucs_len;

		ch = cnv->to_ucs_status;
		cnv->to_ucs_status = 0;
		goto CONTINUE;
	}


	while( src < src_end && dst < dst_end )
	{
		ch = *src++;
		if( ch < 0x80 )
		{
			*dst++ = (ucs_t) ch;
			continue;
		}

		*to_ucs_bytes = (uint8_t) ch;
		length = first_byte_to_length[ ch ];
		i = 1;
CONTINUE:
		while( i < length )
		{
			if( src < src_end )
			{
				to_ucs_bytes[ i ] = (uint8_t) ( ch2 = *src );
				if( !UTF8_IS_TRAIL( ch2 ) )
				{
					break;
				}
				ch = ( ch << 6 ) + ch2;
				++src;
				++i;
			}
			else
			{
				cnv->to_ucs_status = ch;
				cnv->mode = length;
				cnv->to_ucs_len = (int8_t) i;
				goto DONE;
			}
		}

		ch -= offsets[ length ];

		if( i == length
			&& ch <= MAXIMUM_UTF
			&& ch >= min_value[ length ]
			&& !UTF_IS_SURROGATE( ch ) )
		{
			cnv->to_ucs_len = 0;
			*dst++ = (ucs_t) ch;
		}
		else
		{
			cnv->to_ucs_len = (int8_t) i;
			ret = KU_ILLEGAL_CHAR_FOUND;
			break;
		}

	}

DONE:
	if( src < src_end && dst >= dst_end && KU_SUCCESS( ret ) )
	{
		ret = KU_BUFFER_OVERFLOW_ERROR;
	}

	args->src = src;
	args->dst = dst;
	return ret;
}

static int from_ucs( kuconv_from_ucs_args* args )
{
	int ret = KU_OK;
	kuconv_t cnv = args->cnv;
	const ucs_t* src = args->src;
	const ucs_t* src_end = args->src_end;
	char* dst = args->dst;
	const char* dst_end = args->dst_end;
	
    char temp[ 4 ];
	int index_to_write;
    ucs_t ch;

	while( src < src_end && dst < dst_end )
	{
        ch = *(src++);

        if( ch < 0x80 )
        {
            *(dst++) = (char) ch;
        }
#if 1
        else if( ch < 0x800 )
        {
            *(dst++) = (char) ( ( ch >> 6 ) | 0xc0 );
            if( dst < dst_end )
            {
                *(dst++) = (char) ( ( ch & 0x3f ) | 0x80 );
            }
            else
            {
                cnv->from_ucs_overflow_buf[ 0 ] = (char) ( ( ch & 0x3f ) | 0x80 );
                cnv->from_ucs_overflow_buf_len = 1;
                ret = KU_BUFFER_OVERFLOW_ERROR;
            }
        }
        else
		{
			if( ch < 0x10000 )
            {
                index_to_write = 2;
                temp[ 2 ] = (char) ( ( ch >> 12 ) | 0xe0 );
            }
            else if( ch <= 0x10ffff )
            {
                index_to_write = 3;
                temp[ 3 ] = (char) ( ( ch >> 18 ) | 0xf0 );
                temp[ 2 ] = (char) ( ( ( ch >> 12 ) & 0x3f ) | 0x80 );
            }
			else
			{
				cnv->from_ucs_char = ch;
				ret = KU_INVALID_CHAR_FOUND;
				break;
			}

            temp[ 1 ] = (char) ( ( ( ch >> 6 ) & 0x3f ) | 0x80 );
            temp[ 0 ] = (char) ( ( ch & 0x3f ) | 0x80 );

            for( ; index_to_write >= 0; --index_to_write )
            {
                if( dst < dst_end )
                {
                    *(dst++) = temp[ index_to_write ];
                }
                else
                {
                    cnv->from_ucs_overflow_buf[ cnv->from_ucs_overflow_buf_len++ ] = temp[ index_to_write ];
                    ret = KU_BUFFER_OVERFLOW_ERROR;
                }
            }
        }
#else
		else 
		{
			if( ch < 0x800 )
			{
                index_to_write = 1;
			}
			else if( ch < 0x10000 )
            {
                index_to_write = 2;
            }
            else if( ch <= 0x10ffff )
            {
                index_to_write = 3;
            }
			else
			{
				cnv->from_ucs_char = ch;
				ret = KU_INVALID_CHAR_FOUND;
				break;
			}

			switch( index_to_write )
			{
			case 3:
                temp[ 3 ] = (char) ( ( ch >> 18 ) | 0xf0 );
				ch &= 0x3ffff;
			case 2:
                temp[ 2 ] = (char) ( ( ch >> 12 ) | 0x80 );
				ch &= 0xfff;
			case 1:
				temp[ 1 ] = (char) ( ( ch >> 6 ) | 0x80 );
				temp[ 0 ] = (char) ( ( ch & 0x3f ) | 0x80 );
			}

            for( ; index_to_write >= 0; --index_to_write )
            {
                if( dst < dst_end )
                {
                    *(dst++) = temp[ index_to_write ];
                }
                else
                {
                    cnv->from_ucs_overflow_buf[ cnv->from_ucs_overflow_buf_len++ ] = temp[ index_to_write ];
                    ret = KU_BUFFER_OVERFLOW_ERROR;
                }
            }
		}
#endif
	
	}	
	
	if( src < src_end && dst >= dst_end && KU_SUCCESS( ret ) )
	{
		ret = KU_BUFFER_OVERFLOW_ERROR;
	}

	args->src = src;
	args->dst = dst;
	return ret;
}

static static_conv_info utf8_static_data =
{
	sizeof( static_conv_info ),
	"utf-8",
	1, 4,
	{ 0xef, 0xbf, 0xbd, 0 }, 3
};


static converter_impl utf8_impl =
{
	to_ucs,
	from_ucs,
	NULL /* reset */
};


conv_info utf8_data =
{
	-1, 0,
	NULL,
	&utf8_static_data,
	&utf8_impl,
	1, /* code_unit_size */
	-1 /* is_big_endian */
};


