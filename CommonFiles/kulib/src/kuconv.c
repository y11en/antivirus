

#include "kuconv_int.h"
#include "kualloc.h"
#include "ku_str.h"


#include <memory.h>
#include <string.h>

int kuconv_open( const char* name, kuconv_t* pcnv )
{
    if( pcnv == NULL )
	{
        return KU_INVALID_ARG;
	}

	return kuconv_create_converter( name, NULL, pcnv );
}

#if 0
int kuconv_close( kuconv_t cnv )
{
}
#endif

static int _from_ucs( kuconv_from_ucs_args* args )
{
	int ret;
	kuconv_t cnv = args->cnv;
	from_ucs_proc from_ucs = cnv->info->impl->from_ucs;
    const ucs_t *s;
	char* d;

	for(;;)
	{
		ret = from_ucs( args );

		s = args->src;
		d = args->dst;

		if( KU_SUCCESS( ret ) )
		{
			if( s < args->src_end )
				continue;

			return ret;
		}


		if( ret == KU_BUFFER_OVERFLOW_ERROR /* redundant check for optimization */
			|| ( ret != KU_INVALID_CHAR_FOUND
				&& ret != KU_ILLEGAL_CHAR_FOUND
				&& ret != KU_TRUNCATED_CHAR_FOUND ) )
		{
			return ret;
		}
		

		ret = cnv->from_ucs_callback( cnv->to_ucs_context, args,
			cnv->from_ucs_char,
			( ret == KU_INVALID_CHAR_FOUND || ret == KU_UNSUPPORTED_ESCAPE_SEQUENCE ) ?
				KUCONV_UNASSIGNED : KUCONV_ILLEGAL,
			ret );

		cnv->from_ucs_char = 0;

		if( KU_FAILURE( ret ) )
		{
			break;
		}

	}

	return ret;
}

int kuconv_from_ucs( kuconv_t cnv,
		const ucs_t** src, const ucs_t* src_end,
		char** dst, const char* dst_end,
		int flush )
{
	int ret;
    kuconv_from_ucs_args args;
    const ucs_t* s;
    char* d;

    if( cnv == NULL || src == NULL || dst == NULL )
	{
        return KU_INVALID_ARG;
	}

	s = *src;
	d = *dst;
	if( src_end < s || dst_end < d )
	{
        return KU_INVALID_ARG;
	}

	/* flush overflow buffer */
    if( cnv->from_ucs_overflow_buf_len > 0 )
	{
        char* overflow;
        int32_t i;
		int32_t length;

        overflow = (char*) cnv->from_ucs_overflow_buf;
        length = cnv->from_ucs_overflow_buf_len;
        i = 0;
        do
		{
            if( d == dst_end )
			{
                int32_t j = 0;

                do
				{
                    overflow[ j++ ] = overflow[ i++ ];
                }
				while( i < length );

                cnv->from_ucs_overflow_buf_len = (int8_t) j;
                *dst = d;
                return KU_BUFFER_OVERFLOW_ERROR;
            }

            *d++ = overflow[ i++ ];
        }
		while( i < length );

		/* overflow buffer is completely flushed */
        cnv->from_ucs_overflow_buf_len = 0;
    }

    if( !flush && s == src_end )
	{
		/* no more input */
        *dst = d;
        return KU_OK;
    }

	args.cnv = cnv;
	args.src = s;
	args.src_end = src_end;
	args.dst = d;
	args.dst_end = dst_end;
	args.flush = flush;

	ret = _from_ucs( &args );

	*src = args.src;
	*dst = args.dst;
	return ret;

	return KU_OK;
}

static int _to_ucs( kuconv_to_ucs_args* args )
{
	int ret;
	kuconv_t cnv = args->cnv;
	to_ucs_proc to_ucs = cnv->info->impl->to_ucs;
    const char *s;
	ucs_t* d;
	int8_t error_buf_len;


	for(;;)
	{
		ret = to_ucs( args );

		for(;;)
		{
			s = args->src;
			d = args->dst;

			if( KU_SUCCESS( ret ) )
			{
				if( s < args->src_end )
					break;

				/* s == args->src_end */
				if( args->flush && cnv->to_ucs_len > 0 )
				{
					ret = KU_TRUNCATED_CHAR_FOUND;
				}
				else
				{
					if( args->flush )
					{
						kuconv_reset( cnv, KUCONV_RESET_TO_UCS );
					}

					return ret;
				}
			}


			if( ret == KU_BUFFER_OVERFLOW_ERROR /* redundant check for optimization */
				|| ( ret != KU_INVALID_CHAR_FOUND
					&& ret != KU_ILLEGAL_CHAR_FOUND
					&& ret != KU_TRUNCATED_CHAR_FOUND
					&& ret != KU_ILLEGAL_ESCAPE_SEQUENCE
					&& ret != KU_UNSUPPORTED_ESCAPE_SEQUENCE ) )
			{
				return ret;
			}



			/* Calling callback */

			error_buf_len = cnv->invalid_char_len = cnv->to_ucs_len;
			if( error_buf_len > 0 )
			{
				memcpy( cnv->invalid_char_buffer, cnv->to_ucs_bytes, error_buf_len );
			}
			cnv->to_ucs_len = 0;

			ret = cnv->to_ucs_callback( cnv->to_ucs_context, args,
				cnv->invalid_char_buffer, error_buf_len,
				( ret == KU_INVALID_CHAR_FOUND || ret == KU_UNSUPPORTED_ESCAPE_SEQUENCE ) ?
					KUCONV_UNASSIGNED : KUCONV_ILLEGAL,
				ret );

			if( KU_FAILURE( ret ) )
			{
				return ret;
			}
		}
	}

	return ret;
}

int kuconv_to_ucs( kuconv_t cnv,
		const char** src, const char* src_end,
		ucs_t** dst, const ucs_t* dst_end,
		int flush )
{
	int ret;
    kuconv_to_ucs_args args;
	const char* s;
	ucs_t* d;

    if( cnv == NULL || src == NULL || dst == NULL )
	{
        return KU_INVALID_ARG;
	}

	s = *src;
	d = *dst;
	if( src_end < s || dst_end < d )
	{
        return KU_INVALID_ARG;
	}

	/* flush overflow buffer */
	if( cnv->to_ucs_overflow_buf_len > 0 )
	{
        ucs_t* overflow;
        int i;
		int len;

        overflow = cnv->to_ucs_overflow_buf;
        len = cnv->to_ucs_overflow_buf_len;
        i = 0;
        do
		{
            if( d == dst_end )
			{
                int j = 0;

                do
				{
                    overflow[ j++ ] = overflow[ i++ ];
                }
				while( i < len );

                cnv->to_ucs_overflow_buf_len = (int8_t) j;
                *dst = d;
                return KU_BUFFER_OVERFLOW_ERROR;
            }

            *d++ = overflow[ i++ ];
        }
		while( i < len );

		/* overflow buffer is completely flushed */
        cnv->to_ucs_overflow_buf_len = 0;
	}

    if( !flush && s == src_end )
	{
		/* no more input */
        *dst = d;
        return KU_OK;
    }

	args.cnv = cnv;
	args.src = s;
	args.src_end = src_end;
	args.dst = d;
	args.dst_end = dst_end;
	args.flush = flush;

	ret = _to_ucs( &args );

	*src = args.src;
	*dst = args.dst;
	return ret;
}

int kuconv_to_ucs_str( kuconv_t cnv,
		const char* src, int32_t src_len,
		ucs_t* dst, int32_t dst_len,
		int32_t* needed_dst_len )
{
	int ret;
	const char* src_end;
	ucs_t* saved_dst;
	ucs_t* dst_end;
	int32_t dst_len_ret;

    if( cnv == NULL 
		|| dst_len < 0 || ( dst == NULL && dst_len > 0 )
		|| ( src == NULL && src_len != 0 ) )
	{
        return KU_INVALID_ARG;
	}

	kuconv_reset( cnv, KUCONV_RESET_TO_UCS );

	saved_dst = dst;
	if( src_len < 0 )
	{
		src_len = strlen( src ) + 1; /* TODO: add code unit size handling */
	}

	if( src_len > 0 )
	{
		src_end = src + src_len;
		dst_end = dst + dst_len;

		ret = kuconv_to_ucs( cnv, &src, src_end, &dst, dst_end, 1 );
		dst_len_ret = ( dst - saved_dst );

		if( ret == KU_BUFFER_OVERFLOW_ERROR )
		{
			ucs_t dst_buf[ 0x100 ];
				
			dst_end = dst_buf + sizeof(dst_buf) / sizeof(dst_buf[0]);
			do
			{
				dst = dst_buf;
				ret = kuconv_to_ucs( cnv, &src, src_end, &dst, dst_end, 1 );
				dst_len_ret += ( dst - dst_buf );

			}
			while( ret == KU_BUFFER_OVERFLOW_ERROR );
		}
	}
	else
	{
		ret = KU_OK;
		dst_len_ret = 0;
	}
	
	*needed_dst_len = dst_len_ret;

	return ret;	
}


int kuconv_from_ucs_str( kuconv_t cnv,
		const ucs_t* src, int32_t src_len,
		char* dst, int32_t dst_len,
		int32_t* needed_dst_len )
{
	int ret;
	const ucs_t* src_end;
	char* saved_dst;
	char* dst_end;
	int32_t dst_len_ret;

    if( cnv == NULL 
		|| dst_len < 0 || ( dst == NULL && dst_len > 0 )
		|| ( src == NULL && src_len != 0 ) )
	{
        return KU_INVALID_ARG;
	}

	kuconv_reset( cnv, KUCONV_RESET_FROM_UCS );

	saved_dst = dst;
	if( src_len < 0 )
	{
		src_len = ku_ucslen( src ) + 1;
	}

	if( src_len > 0 )
	{
		src_end = src + src_len;
		dst_end = dst + dst_len;

		ret = kuconv_from_ucs( cnv, &src, src_end, &dst, dst_end, 1 );
		dst_len_ret = ( dst - saved_dst );

		if( ret == KU_BUFFER_OVERFLOW_ERROR )
		{
			char dst_buf[ 0x100 ];
				
			dst_end = dst_buf + sizeof(dst_buf) / sizeof(dst_buf[0]);
			do
			{
				dst = dst_buf;
				ret = kuconv_from_ucs( cnv, &src, src_end, &dst, dst_end, 1 );
				dst_len_ret += ( dst - dst_buf );

			}
			while( ret == KU_BUFFER_OVERFLOW_ERROR );
		}
	}
	else
	{
		ret = KU_OK;
		dst_len_ret = 0;
	}
	
	*needed_dst_len = dst_len_ret;

	return ret;	
}

static int kuconv_convert_int( kuconv_t src_cnv, kuconv_t dst_cnv,
		const char** src, const char* src_end,
		char** dst, const char* dst_end,
		ucs_t* tmp, ucs_t** tmp_src,
		ucs_t** tmp_dst, const ucs_t* tmp_end,
		int flush )
{
	int ret = KU_OK;

	for(;;)
	{
		ret = kuconv_from_ucs( dst_cnv, 
				tmp_src, *tmp_dst, 
				dst, dst_end,
				flush && *src == src_end );
		if( KU_FAILURE(ret) )
		{
			break;
		}

		*tmp_src = *tmp_dst = tmp;

		ret = kuconv_to_ucs( src_cnv, 
				src, src_end, 
				tmp_dst, tmp_end, 
				flush );
		if( ret == KU_BUFFER_OVERFLOW_ERROR )
		{
			/* ret = KU_OK; */
		}
		else if( KU_FAILURE( ret ) || *tmp_dst == tmp )
		{
			break;
		}
	}

	return ret;
}

int kuconv_convert( kuconv_t src_cnv, kuconv_t dst_cnv,
		const char* src, int32_t src_len,
		char* dst, int32_t dst_len,
		int32_t* needed_dst_len )
{
	int ret = KU_OK;

	char* prev_dst;
	int32_t needed = 0;
	
	ucs_t tmp_buf[ 0x100 ];
	ucs_t* tmp_src;
	ucs_t* tmp_dst;

	const char* src_end;
	const char* dst_end;

    if( src_cnv == NULL || dst_cnv == NULL
		|| dst_len < 0 || ( dst == NULL && dst_len > 0 )
		|| ( src == NULL && src_len != 0 ) )
	{
        return KU_INVALID_ARG;
	}

	if( src_len < 0 )
	{
		src_len = strlen( src ) + 1;  /* TODO: add code unit size handling */
	}

	src_end = src + src_len;
	tmp_src = tmp_dst = tmp_buf;


	if( dst_len > 0 )
	{
		dst_end = dst + dst_len;

		prev_dst = dst;
		ret = kuconv_convert_int( src_cnv, dst_cnv,
				&src, src_end,
				&dst, dst_end,
				tmp_buf, &tmp_src, &tmp_dst, tmp_buf + sizeof(tmp_buf) / sizeof(tmp_buf[0]),
				1 );
		needed += ( dst - prev_dst );
		
	}

	if( ret == KU_BUFFER_OVERFLOW_ERROR || dst_len == 0 )
	{
		char dst_buf[ 0x100 ];

		dst_end = dst_buf + sizeof(dst_buf) / sizeof(dst_buf[0]);
		do
		{
			dst = dst_buf;
			ret = kuconv_convert_int( src_cnv, dst_cnv,
					&src, src_end,
					&dst, dst_end,
					tmp_buf, &tmp_src, &tmp_dst, tmp_buf + sizeof(tmp_buf) / sizeof(tmp_buf[0]),
					1 );
			needed += ( dst - dst_buf );
		}
		while( ret == KU_BUFFER_OVERFLOW_ERROR );
	}

	if( needed_dst_len != NULL )
	{
		*needed_dst_len = needed;
	}

	return ret;
}


int kuconv_to_ucs_skip( kuconv_t cnv,
		const char** src, const char* src_end,
		int32_t* skip,
		int flush )
{
	int ret = KU_OK;
	int32_t n;

	ucs_t dst_buf[ 0x100 ];
	ucs_t* dst;
	const ucs_t* dst_end;

	int min_bytes_per_char;
	int max_bytes_per_char;

    if( cnv == NULL || src == NULL )
	{
        return KU_INVALID_ARG;
	}

	if( src_end < *src )
	{
        return KU_INVALID_ARG;
	}

	n = *skip;

	min_bytes_per_char = cnv->info->static_info->min_bytes_per_char;
	max_bytes_per_char = cnv->info->static_info->max_bytes_per_char;
	if( min_bytes_per_char == max_bytes_per_char
		/* && cnv->info->static_info->to_ucs_mappimg_type == MAPPING_1_TO_1 */ ) /* optimization */
	{
		int32_t src_len = ( src_end - *src ) / max_bytes_per_char;
		
		if( src_len > n )
		{
			*src += n * max_bytes_per_char;
			n = 0;
			ret = KU_BUFFER_OVERFLOW_ERROR;
		}
		else
		{
			*src += src_len * max_bytes_per_char;
			n -= src_len;
		}
	}
	else
	{
		do
		{
			dst = dst_buf;
			if( n > ( sizeof(dst_buf) / sizeof(dst_buf[0]) ) )
			{
				dst_end = dst + sizeof(dst_buf) / sizeof(dst_buf[0]);
			}
			else
			{
				dst_end = dst + n;
			}

			ret = kuconv_to_ucs( cnv, 
					src, src_end,
					&dst, dst_end,
					flush );
			
			n -= ( dst - dst_buf );
		}
		while( n > 0 && ret == KU_BUFFER_OVERFLOW_ERROR );
	}


	*skip = n;

	return ret;
}

void kuconv_get_to_ucs_callback( kuconv_t cnv,
		kuconv_to_ucs_callback_proc* callback, const void** context )
{
	*callback = cnv->to_ucs_callback;
	*context = cnv->to_ucs_context;
}

void kuconv_set_to_ucs_callback( kuconv_t cnv,
		kuconv_to_ucs_callback_proc new_callback, const void* new_context,
		kuconv_to_ucs_callback_proc* old_callback, const void** old_context )
{
	if( old_callback ) *old_callback = cnv->to_ucs_callback;
	cnv->to_ucs_callback = new_callback;
	if( old_context ) *old_context = cnv->to_ucs_context;
	cnv->to_ucs_context = new_context;
}

void kuconv_get_from_ucs_callback( kuconv_t cnv,
		kuconv_from_ucs_callback_proc* callback, const void** context )
{
	*callback = cnv->from_ucs_callback;
	*context = cnv->from_ucs_context;
}

void kuconv_set_from_ucs_callback( kuconv_t cnv,
		kuconv_from_ucs_callback_proc new_callback, const void* new_context,
		kuconv_from_ucs_callback_proc* old_callback, const void** old_context )
{
	if( old_callback ) *old_callback = cnv->from_ucs_callback;
	cnv->from_ucs_callback = new_callback;
	if( old_context ) *old_context = cnv->from_ucs_context;
	cnv->from_ucs_context = new_context;
}

void kuconv_reset( kuconv_t cnv, int reset )
{
	if( reset & KUCONV_RESET_TO_UCS )
	{
		cnv->to_ucs_status = 0;
		cnv->mode = 0;
		cnv->to_ucs_len = 0;
		cnv->invalid_char_len = 0;
		cnv->to_ucs_overflow_buf_len = 0;
	}

	if( reset & KUCONV_RESET_FROM_UCS )
	{
		cnv->from_ucs_char = 0;
		cnv->from_ucs_overflow_buf_len = 0;
	}
}

int kuconv_get_code_unit_size( kuconv_t cnv )
{
	return cnv->info->code_unit_size;
}

int kuconv_get_endian( kuconv_t cnv )
{
	return cnv->info->is_big_endian;
}
