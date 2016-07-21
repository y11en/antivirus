


#ifndef __KUCASE_H
#define __KUCASE_H

#include "ku_defs.h"

KU_CDECL_BEGIN

int kucase_to_upper(
		const ucs_t* src, int32_t src_len,
		ucs_t* dst, int32_t dst_len,
		int32_t* needed );

int kucase_to_lower(
		const ucs_t* src, int32_t src_len,
		ucs_t* dst, int32_t dst_len,
		int32_t* needed );

KU_CDECL_END

#endif /* __KUCASE_H */