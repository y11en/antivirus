
/* Library internal definitions. */

#ifndef __KUCONV_INT_H
#define __KUCONV_INT_H


#include <kulib/kuconv.h>

#include "kuconv_mbcs.h"


#define KUCONV_MAX_CHAR_LEN			8
#define KUCONV_MAX_SUBCHAR_LEN		4 /* KUCONV_MAX_CHAR_LEN */

#define KUCONV_OVERFLOW_BUFFER_LENGTH	32


KU_CDECL_BEGIN


typedef int (*to_ucs_proc) ( kuconv_to_ucs_args* );
typedef int (*from_ucs_proc) ( kuconv_from_ucs_args* );

typedef enum __reset_type {
    RESET_BOTH,
    RESET_TO_UNICODE,
    RESET_FROM_UNICODE
} reset_type_t;

typedef void (*reset_proc) ( kuconv_t cnv, reset_type_t type );


typedef struct __converter_impl
{
	to_ucs_proc to_ucs;
	from_ucs_proc from_ucs;
	reset_proc reset;
} converter_impl;


#define KUCONV_HAS_SUPPLEMENTARY	1
#define KUCONV_HAS_SURROGATES		2


typedef struct __static_conv_info
{
	uint32_t size;		/* size of this struct */

	char name[ KUCONV_MAX_CONVERTER_NAME_LENGTH ];

    int8_t min_bytes_per_char;
    int8_t max_bytes_per_char;

    uint8_t sub_char[ KUCONV_MAX_SUBCHAR_LEN ];
    int8_t sub_char_len;
    uint8_t sub_char1;

    uint8_t has_to_uni_fallback;
    uint8_t has_from_uni_fallback;

    uint8_t uni_flags;

	uint8_t reserved[ 5 ];		/* padding */

    /* total structure size: 80 */
} static_conv_info;


typedef struct __conv_info
{
	int32_t ref_cnt;	/* usage count, -1 - for static info */
	int cashed;			/* 1 - converter info don't destroyed if ref_cnt = 0 */

	const void* data;		/* for kudata_t* from kudata_open */

	const static_conv_info* static_info;

	const converter_impl* impl;

	uint8_t code_unit_size;
	int8_t is_big_endian;		/* -1 if code_unit_size == 1 */

	kuconv_mbcs_data mbcs_data;
} conv_info;


struct __kuconv
{
	conv_info*	info;			/* pointer to converter description */
	
	int8_t is_alloc;			/* 1 - this structure allocated by kuconv */

	int8_t use_fallback;

	/* to ucs */

	kuconv_to_ucs_callback_proc	to_ucs_callback;
	const void* to_ucs_context;

	/* ------------ */

    uint32_t to_ucs_status;
    int32_t mode;

	uint8_t to_ucs_bytes[ KUCONV_MAX_CHAR_LEN - 1 ];	/* keeps the bytes of the current source character */
	int8_t	to_ucs_len;                  				/* number of bytes in to_ucs_bytes */

	char invalid_char_buffer[ KUCONV_MAX_CHAR_LEN ];
	int8_t invalid_char_len;

	ucs_t	to_ucs_overflow_buf[ KUCONV_OVERFLOW_BUFFER_LENGTH ];	/* overlow output buffer */
	int8_t	to_ucs_overflow_buf_len;								/* used length of to_ucs_overflow_buf */



	/* from ucs */

	kuconv_from_ucs_callback_proc	from_ucs_callback;
	const void* from_ucs_context;

    uint8_t sub_char[ KUCONV_MAX_SUBCHAR_LEN ];
    int8_t sub_char_len;
    uint8_t sub_char1;

	/* ------------ */
    ucs_t from_ucs_char;

	/* ucs_t invalid_uchar_buffer[ 1 ]; use from_ucs_char */
	/* int8_t invalid_uchar_len; now always 1 */

	uint8_t from_ucs_overflow_buf[ KUCONV_OVERFLOW_BUFFER_LENGTH ];
	int8_t from_ucs_overflow_buf_len;
};

int kuconv_create_converter( const char* name, kuconv* cnv, kuconv** pcnv );


extern conv_info utf8_data;
extern conv_info utf16le_data;



KU_CDECL_END


#endif	/* __KUCONV_INT_H */
