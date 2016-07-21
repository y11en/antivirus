


#ifndef __KUCONV_ERR_H
#define __KUCONV_ERR_H


#include "ku_defs.h"


struct __kuconv;
typedef struct __kuconv kuconv;
typedef kuconv* kuconv_t;
/* typedef void* kuconv_t; */

enum __kuconv_callback_reason
{
	KUCONV_UNASSIGNED = 0,
	KUCONV_ILLEGAL,
	KUCONV_IRREGULAR
};

typedef struct __kuconv_from_ucs_args
{
    kuconv_t		cnv;
    const ucs_t*	src;
    const ucs_t*	src_end;
    char*			dst;
    const char*		dst_end;
    int				flush;
} kuconv_from_ucs_args;

typedef struct __kuconv_to_ucs_args
{
    kuconv_t		cnv;
    const char*		src;
    const char*		src_end;
    ucs_t*			dst;
    const ucs_t* 	dst_end;
    int 			flush;
} kuconv_to_ucs_args;


int KUCONV_TO_UCS_CALLBACK_STOP(
		const void* context,
		kuconv_to_ucs_args* args,
		const char* err_buf,
		int32_t length,
		int reason,
		int err );

int KUCONV_TO_UCS_CALLBACK_SKIP(
		const void* context,
		kuconv_to_ucs_args* args,
		const char* err_buf,
		int32_t length,
		int reason,
		int err );

int KUCONV_TO_UCS_CALLBACK_SUBSTITUTE(
		const void* context,
		kuconv_to_ucs_args* args,
		const char* err_buf,
		int32_t length,
		int reason,
		int err );


int KUCONV_FROM_UCS_CALLBACK_STOP(
		const void* context,
		kuconv_from_ucs_args* args,
		ucs_t err_char,
		int reason,
		int err );

int KUCONV_FROM_UCS_CALLBACK_SKIP(
		const void* context,
		kuconv_from_ucs_args* args,
		ucs_t err_char,
		int reason,
		int err );

int KUCONV_FROM_UCS_CALLBACK_SUBSTITUTE(
		const void* context,
		kuconv_from_ucs_args* args,
		ucs_t err_char,
		int reason,
		int err );



#endif /* __KUCONV_ERR_H */

