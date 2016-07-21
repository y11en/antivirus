
#include <kulib/kuconv_err.h>
#include "kuconv_int.h"

static int to_ucs_write_u_chars( kuconv_to_ucs_args* args, const ucs_t* uchars, int32_t len )
{
    ucs_t* dst = args->dst;
    const ucs_t* dst_end = args->dst_end;

	while( len > 0 && dst< dst_end )
	{
		*dst++ = *uchars++;
		--len;
	}
    args->dst = dst;

    if( len > 0 )
	{
		dst = args->cnv->to_ucs_overflow_buf;
		args->cnv->to_ucs_overflow_buf_len = (int8_t) len;
            
		do 
		{
			*dst++ = *uchars++;
		}
		while( --len > 0 );

        return KU_BUFFER_OVERFLOW_ERROR;
    }

	return KU_OK;
}


static int to_ucs_write_sub( kuconv_to_ucs_args* args )
{
	static const ucs_t sub_char = 0xfffd;
	static const ucs_t sub_char1 = 0x1a;

	if( args->cnv->invalid_char_len == 1 && args->cnv->sub_char1 != 0 )
	{
		return to_ucs_write_u_chars( args, &sub_char1, 1 );
	}
	else
	{
		return to_ucs_write_u_chars( args, &sub_char, 1 );
	}
}

int KUCONV_TO_UCS_CALLBACK_STOP(
		const void* context,
		kuconv_to_ucs_args* args,
		const char* err_buf,
		int32_t length,
		int reason,
		int err )
{
	return err;	
}

int KUCONV_TO_UCS_CALLBACK_SKIP(
		const void* context,
		kuconv_to_ucs_args* args,
		const char* err_buf,
		int32_t length,
		int reason,
		int err )
{
    if( context == NULL )
    {
        if( reason <= KUCONV_IRREGULAR )
        {
			return KU_OK;
        }
    }
    else if( *( (const char*) context ) == 'i' )
    {
        if( reason == KUCONV_UNASSIGNED )
        {
			return KU_OK;
        }
    }

	return err;	
}


int KUCONV_TO_UCS_CALLBACK_SUBSTITUTE (
		const void* context,
		kuconv_to_ucs_args* args,
		const char* err_buf,
		int32_t length,
		int reason,
		int err )
{
    if( context == NULL )
    {
        if( reason <= KUCONV_IRREGULAR )
        {
			return to_ucs_write_sub( args );
        }
    }
    else if( *( (const char*) context ) == 'i' )
    {
        if( reason == KUCONV_UNASSIGNED )
        {
			return to_ucs_write_sub( args );
        }
    }

	return err;
}


static int from_ucs_write_bytes( kuconv_from_ucs_args* args, const char* src, int32_t len )
{
    char* dst = args->dst;
    const char* dst_end = args->dst_end;

	while( len > 0 && dst < dst_end )
	{
		*dst++ = *src++;
		--len;
	}
    args->dst = dst;

    if( len > 0 )
	{
		dst = args->cnv->from_ucs_overflow_buf;
		args->cnv->from_ucs_overflow_buf_len = (int8_t) len;
            
		do 
		{
			*dst++ = *src++;
		}
		while( --len > 0 );

        return KU_BUFFER_OVERFLOW_ERROR;
    }

	return KU_OK;
}

static int from_ucs_write_sub( kuconv_from_ucs_args* args )
{
	kuconv_t cnv = args->cnv;
	
	if( args->cnv-> from_ucs_char < 0x80 && args->cnv->sub_char1 != 0 )
	{
		return from_ucs_write_bytes( args, &cnv->sub_char1, 1 );
	}
	else
	{
		return from_ucs_write_bytes( args, cnv->sub_char, cnv->sub_char_len );
	}
}

int KUCONV_FROM_UCS_CALLBACK_STOP(
		const void* context,
		kuconv_from_ucs_args* args,
		ucs_t err_char,
		int reason,
		int err )
{
	return err;
}

int KUCONV_FROM_UCS_CALLBACK_SKIP(
		const void* context,
		kuconv_from_ucs_args* args,
		ucs_t err_char,
		int reason,
		int err )
{
    if( context == NULL )
    {
        if( reason <= KUCONV_IRREGULAR )
        {
			return KU_OK;
        }
    }
    else if( *( (const char*) context ) == 'i' )
    {
        if( reason == KUCONV_UNASSIGNED )
        {
			return KU_OK;
        }
    }

	return err;
}

int KUCONV_FROM_UCS_CALLBACK_SUBSTITUTE(
		const void* context,
		kuconv_from_ucs_args* args,
		ucs_t err_char,
		int reason,
		int err )
{
    if( context == NULL )
    {
        if( reason <= KUCONV_IRREGULAR )
        {
			return from_ucs_write_sub( args );
        }
    }
    else if( *( (const char*) context ) == 'i' )
    {
        if( reason == KUCONV_UNASSIGNED )
        {
			return from_ucs_write_sub( args );
        }
    }

	return err;
}
