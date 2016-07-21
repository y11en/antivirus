// tree_crc32.h
//

#ifndef tree_crc32_h_INCLUDED
#define tree_crc32_h_INCLUDED

#include "common.h"

///////////////////////////////////////////////////////

EXTERN_C uint32_t CountCRC32 (uint32_t size, const uint8_t* ptr, uint32_t old_crc);

extern uint32_t crc_32_tab[];

#define UPD_CRC32(octet,crc) (crc_32_tab[((crc) ^ (octet)) & 0xff] ^ ((crc) >> 8))

///////////////////////////////////////////////////////

#endif // tree_crc32_h_INCLUDED

