

#ifndef __KUCONV_H
#define __KUCONV_H


#include "kuconv_err.h"

KU_CDECL_BEGIN


#define KUCONV_MAX_CONVERTER_NAME_LENGTH	60		/* Maximum converter name length including terminating null. */

typedef int (*kuconv_to_ucs_callback_proc) (
		const void* context,
		kuconv_to_ucs_args* args,
		const char* err_buf,
		int32_t length,
		int reason,
		int err );

typedef int (*kuconv_from_ucs_callback_proc) (
		const void* context,
		kuconv_from_ucs_args* args,
		/* const ucs_t* err_buf, */
		/* int32_t length, */
		ucs_t err_char,
		int reason,
		int err );


int kuconv_compare_names( const char* name1, const char* name2 );
int kuconv_get_converter_name( const char* alias, const char** converter );
int kuconv_get_aliases_count( uint16_t* paliases_count );


int kuconv_open( const char* name, kuconv_t* pcnv );
int kuconv_close( kuconv_t cnv );


int kuconv_to_ucs( kuconv_t cnv,
		const char** src, const char* src_end,
		ucs_t** dst, const ucs_t* dst_end,
		int flush );

int kuconv_from_ucs( kuconv_t cnv,
		const ucs_t** src, const ucs_t* src_end,
		char** dst, const char* dst_end,
		int flush );


int kuconv_to_ucs_str( kuconv_t cnv,
		const char* src, int32_t src_len,
		ucs_t* dst, int32_t dst_len,
		int32_t* needed_dst_len );

int kuconv_from_ucs_str( kuconv_t cnv,
		const ucs_t* src, int32_t src_len,
		char* dst, int32_t dst_len,
		int32_t* needed_dst_len );


int kuconv_convert( kuconv_t src_cnv, kuconv_t dst_cnv,
		const char* src, int32_t src_len,
		char* dst, int32_t dst_len,
		int32_t* needed_dst_len );


int kuconv_to_ucs_skip( kuconv_t cnv,
		const char** src, const char* src_end,
		int32_t* n,
		int flush );


#define KUCONV_RESET_TO_UCS		0x01
#define KUCONV_RESET_FROM_UCS	0x02
#define KUCONV_RESET_ALL		( KUCONV_RESET_TO_UCS | KUCONV_RESET_FROM_UCS )
void kuconv_reset( kuconv_t cnv, int reset );


void kuconv_get_to_ucs_callback( kuconv_t cnv,
		kuconv_to_ucs_callback_proc* callback, const void** context );

void kuconv_set_to_ucs_callback( kuconv_t cnv,
		kuconv_to_ucs_callback_proc new_callback, const void* new_context,
		kuconv_to_ucs_callback_proc* old_callback, const void** old_context );

void kuconv_get_from_ucs_callback( kuconv_t cnv,
		kuconv_from_ucs_callback_proc* callback, const void** context );

void kuconv_set_from_ucs_callback( kuconv_t cnv,
		kuconv_from_ucs_callback_proc new_callback, const void* new_context,
		kuconv_from_ucs_callback_proc* old_callback, const void** old_context );

int32_t kuconv_flush_cache( void );


int kuconv_get_code_unit_size( kuconv_t cnv );
int kuconv_get_endian( kuconv_t cnv );

KU_CDECL_END

#endif	/* __KUCONV_H */




