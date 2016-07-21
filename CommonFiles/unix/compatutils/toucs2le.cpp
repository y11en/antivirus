#include "compatutils.h"
#include <kl_byteswap.h>

void* toUCS2LE(void* src, unsigned int size) 
{ 
   	static unsigned short dest[1000];
	wchar_t* source = reinterpret_cast<wchar_t*>(src);
	__builtin_memset(dest, 0, sizeof(dest));
	for(unsigned int i = 0; i < size; ++i) {
		unsigned short tmp = source[i];
		dest[i]=kl_bswap_16_le(tmp);
	}
	return dest;
}
