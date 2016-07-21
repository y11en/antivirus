// diffs.h
//
//

#ifndef diffs_h_INCLUDED
#define diffs_h_INCLUDED

#include "comdefs.h"

#ifdef __cplusplus
extern "C" {
#else
#ifndef bool
#define bool char
#endif
#endif // __cplusplus

#define PACK_FORMAT_NUL 0
#define PACK_FORMAT_DLL 1
#define PACK_FORMAT_KFB 2

////////////////////////////////////////////////////////////////
// packing

bool KLAVPACK_API Diff_DLL_Pack (
			const unsigned char *unp_data,
			size_t unp_size,
			struct Diff_Buffer *pk_buf
		);

bool KLAVPACK_API Diff_KFB_Pack (
			const unsigned char *unp_data,
			size_t unp_size,
			struct Diff_Buffer *pk_buf
		);

////////////////////////////////////////////////////////////////
// unpacking

bool KLAVPACK_API Diff_DLL_IsPacked (
			const unsigned char *data,
			size_t size
		);

bool KLAVPACK_API Diff_DLL_Unpack (
			const unsigned char *pk_data,
			size_t pk_size,
			struct Diff_Buffer *unp_buf
		);

bool KLAVPACK_API Diff_KFB_IsPacked (
			const unsigned char *data,
			size_t size
		);

bool KLAVPACK_API Diff_KFB_Unpack (
			const unsigned char *pk_data,
			size_t pk_size,
			struct Diff_Buffer *unp_buf
		);


////////////////////////////////////////////////////////////////

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // diffs_h_INCLUDED

