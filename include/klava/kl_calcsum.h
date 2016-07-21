// kl_calcsum.h
//

#ifndef kl_calcsum_h_INCLUDED
#define kl_calcsum_h_INCLUDED

// C++-only version

#include <kl_types.h>

#if defined(_MSC_VER)
__forceinline
#else
inline
#endif
uint32_t kl_calcsum_init (const uint8_t *& p)
{
	uint32_t n = *p++;
	n ^=  (n << 8) ^ *p++;
	n ^=  (n << 8) ^ *p++;
	n ^=  (n << 8) ^ *p++;
	return n;
}

#if defined(_MSC_VER)
__forceinline
#else
inline
#endif
uint32_t kl_calcsum_byte (const uint8_t *& p, uint32_t b)
{
	return (b << 1) ^ *p++;
}

#if defined(_MSC_VER)
__forceinline
#else
inline
#endif
uint32_t kl_calcsum_cont (const uint8_t *& p, const uint8_t * pe, uint32_t acc)
{
	while (p != pe)
		acc ^= kl_calcsum_byte (p, acc);
	return acc;
}

#if defined(_MSC_VER)
__forceinline
#else
inline
#endif
uint32_t kl_calcsum (const uint8_t * p, size_t cnt)
{
	if (cnt <= 4)
	{
		uint32_t s = *p++;
		for (size_t i = 1; i < cnt; i++){
			s ^= s << 8;
			s ^= *p++;
		}
		return s;
	} else
	{
		const uint8_t * pe = p + cnt;
		uint32_t sum = kl_calcsum_init (p);
		return kl_calcsum_cont (p, pe, sum);
	}
}

#endif // kl_calcsum_h_INCLUDED

