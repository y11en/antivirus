// crc32.h
//

#ifndef crc32_h_INCLUDED
#define crc32_h_INCLUDED

#include <kl_types.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint32_t KlavCRC32 (const void *data, size_t size, uint32_t oldCRC);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // crc32_h_INCLUDED



