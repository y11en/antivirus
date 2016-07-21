

#ifndef __KU_DEFS_H
#define __KU_DEFS_H

#include "ku_platform.h"

#include <stddef.h>



#ifdef __cplusplus
#   define KU_CFUNC extern "C"
#   define KU_CDECL_BEGIN extern "C" {
#   define KU_CDECL_END   }
#else
#   define KU_CFUNC extern
#   define KU_CDECL_BEGIN
#   define KU_CDECL_END
#endif



typedef uint16_t ucs2_t;
typedef uint32_t ucs4_t;
typedef ucs4_t ucs_t;


enum
{
	KU_OK = 0,					/* No error, OK */
	KU_INVALID_ARG,					/* Incorrect function args. */
	KU_MEMORY_ALLOCATION_ERROR,
	KU_INVALID_FORMAT_ERROR,
	KU_FILE_ACCESS_ERROR,

	/* Conversion errors */
	KU_INVALID_CHAR_FOUND	= 0x100,		/* Unmappable input sequence. */
	KU_TRUNCATED_CHAR_FOUND,	       		/* Incomplete input sequence. */
	KU_ILLEGAL_CHAR_FOUND,		       		/* Illegal input sequence/combination of input units. */
	KU_BUFFER_OVERFLOW_ERROR,	       		/* Output (destination) buffer overflow. */

	KU_ILLEGAL_ESCAPE_SEQUENCE,	       		/* ISO-2022 illegal escape sequence. */
	KU_UNSUPPORTED_ESCAPE_SEQUENCE,			/* ISO-2022 unsupported escape sequence. */
};


#define KU_SUCCESS( e ) ( ( e ) <= KU_OK )
#define KU_FAILURE( e ) ( ( e ) > KU_OK )


#endif __KU_DEFS_H