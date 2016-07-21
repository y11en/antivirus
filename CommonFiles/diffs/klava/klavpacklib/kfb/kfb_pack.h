// kfb_pack.h
//
//

#ifndef kfb_pack_h_INCLUDED
#define kfb_pack_h_INCLUDED

#include "../klavpack_encode.h"
#include "../klavpack_decode.h"

#include <klava/kfb.h>

typedef std::vector<KFB2_SECTION_TABLE_RECORD> SectionTable;

void allocSectionSpace (
		uint32_t size,
		uint32_t alignment,
		SectionTable& tbl,
		uint32_t *off,
		uint32_t* idx,
		uint32_t* totalSize
	);

#ifdef KL_LITTLE_ENDIAN
# define KFB_NATIVE_ENDIAN 0
#else
# define KFB_NATIVE_ENDIAN 1
#endif

extern uint32_t KlavCRC32 (const void *data, size_t size, uint32_t oldCRC);

#endif // kfb_pack_h_INCLUDED

