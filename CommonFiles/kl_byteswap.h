// kl_byteswap.h
//
// byte-swapping primitives
//
// (C) 2004-2006 Kaspersky labs.
//
// Latest revision: 30-11-2006
//

#ifndef kl_byteswap_h_INCLUDED
#define kl_byteswap_h_INCLUDED

#include "kl_types.h"

#if defined(KL_PLATFORM_LINUX)

# include <byteswap.h>

# define kl_bswap_16(x) bswap_16(x)
# define kl_bswap_32(x) bswap_32(x)
# define kl_bswap_64(x) bswap_64(x)

#elif defined(KL_PLATFORM_MACOSX)

# include <CoreServices/CoreServices.h>
# define kl_bswap_16(x) Endian16_Swap(x)
# define kl_bswap_32(x) Endian32_Swap(x)
# define kl_bswap_64(x) Endian64_Swap(x)

#else // platform

// generic byte-swapping code

#define	kl_bswap_16(x)	\
	((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))

#define	kl_bswap_32(x)	\
	((((x) & UINT32_C(0xff000000)) >> 24) | (((x) & UINT32_C(0x00ff0000)) >>  8) | \
	(((x) & UINT32_C(0x0000ff00)) <<  8) | (((x) & UINT32_C(0x000000ff)) << 24))

#if KL_MAX_WORDSIZE >= 64

#define kl_bswap_64(x) \
    ((((x) & UINT64_C(0xFF00000000000000)) >> 56) \
   | (((x) & UINT64_C(0x00FF000000000000)) >> 40) \
   | (((x) & UINT64_C(0x0000FF0000000000)) >> 24) \
   | (((x) & UINT64_C(0x000000FF00000000)) >> 8)  \
   | (((x) & UINT64_C(0x00000000FF000000)) << 8)  \
   | (((x) & UINT64_C(0x0000000000FF0000)) << 24) \
   | (((x) & UINT64_C(0x000000000000FF00)) << 40) \
   | (((x) & UINT64_C(0x00000000000000FF)) << 56))

#endif // KL_MAX_WORDSIZE >= 64

#endif // platform

#ifdef __cplusplus

template<typename T>
T kl_bswap_t (T x);

template<>
inline int8_t kl_bswap_t (int8_t x)
{ return x; }

template<>
inline uint8_t kl_bswap_t (uint8_t x)
{ return x; }

template<>
inline int16_t kl_bswap_t (int16_t x)
{ return kl_bswap_16 (x); }

template<>
inline uint16_t kl_bswap_t (uint16_t x)
{ return kl_bswap_16 (x); }

template<>
inline int32_t kl_bswap_t (int32_t x)
{ return kl_bswap_32 (x); }

template<>
inline uint32_t kl_bswap_t (uint32_t x)
{ return kl_bswap_32 (x); }

template<typename T>
inline void kl_bswap_array_t(T * begin, size_t count)
{
	for (size_t i = 0; i < count; ++i)
	{
		begin [i] = kl_bswap_t (begin [i]);
	}
}

#if KL_MAX_WORDSIZE >= 64

template <>
inline int64_t kl_bswap_t (int64_t x)
{ return kl_bswap_64 (x); }

template <>
inline uint64_t kl_bswap_t (uint64_t x)
{ return kl_bswap_64 (x); }

#endif // KL_MAX_WORDSIZE >= 64

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// pointer-based primitives for 64-bit data

#if KL_MAX_WORDSIZE >= 64

#define	kl_bswap_64p(ps, pd) \
		do { register uint32_t t = kl_bswap_32(((const uint32_t *)(ps))[0]); \
		  ((uint32_t *)(pd))[0] = kl_bswap_32(((const uint32_t *)(ps))[1]); \
		  ((uint32_t *)(pd))[1] = t; } while(0)

#endif // KL_MAX_WORDSIZE >= 64

////////////////////////////////////////////////////////////////
// converters to-from little-endian format

#ifdef KL_BIG_ENDIAN

// big-endian platform

#define kl_bswap_16_le(x) kl_bswap_16(x)
#define kl_bswap_32_le(x) kl_bswap_32(x)

#define kl_bswap_16_be(x) (x)
#define kl_bswap_32_be(x) (x)

#if KL_MAX_WORDSIZE >= 64

#define kl_bswap_64_le(x)      kl_bswap_64(x)
#define kl_bswap_64p_le(ps,pd) kl_bswap_64p(ps,pd)

#define kl_bswap_64_be(x) (x)
#define kl_bswap_64p_be(ps,pd) \
	do { if (ps != pd) *pd = ps; } while (0)

#endif // KL_MAX_WORDSIZE >= 64

#else // KL_BIG_ENDIAN

// little-endian platform

#define kl_bswap_16_le(x) (x)
#define kl_bswap_32_le(x) (x)

#define kl_bswap_16_be(x) kl_bswap_16(x)
#define kl_bswap_32_be(x) kl_bswap_32(x)

#if KL_MAX_WORDSIZE >= 64

#define kl_bswap_64_le(x) (x)
#define kl_bswap_64p_le(ps,pd) \
	do { if (ps != pd) *pd = ps; } while (0)

#define kl_bswap_64_be(x)      kl_bswap_64(x)
#define kl_bswap_64p_be(ps,pd) kl_bswap_64p(ps,pd)

#endif // KL_MAX_WORDSIZE >= 64

#endif // KL_BIG_ENDIAN

////////////////////////////////////////////////////////////////
// macros for unaligned access

#ifdef KL_NEED_ALIGNMENT

# define kl_get_unaligned_16_le_ptr(p) \
	((uint16_t)(p)[0] | ((p)[1] << 8))
	
# define kl_get_unaligned_16_be_ptr(p) \
	(((uint16_t)(p)[0] << 8) | (p)[1])

# define kl_get_unaligned_32_le_ptr(p) \
	((uint32_t)(p)[0] | ((p)[1] << 8) | ((p)[2] << 16) | ((p)[3] << 24))

# define kl_get_unaligned_32_be_ptr(p) \
	(((uint32_t)(p)[0] << 24) | ((p)[1] << 16) | ((p)[2] << 8) | (p)[3])

# define kl_get_unaligned_16_le(p) \
	kl_get_unaligned_16_le_ptr ((const uint8_t *)(p))

# define kl_get_unaligned_16_be(p) \
	kl_get_unaligned_16_be_ptr ((const uint8_t *)(p))

# define kl_get_unaligned_32_le(p) \
	kl_get_unaligned_32_le_ptr ((const uint8_t *)(p))

# define kl_get_unaligned_32_be(p) \
	kl_get_unaligned_32_be_ptr ((const uint8_t *)(p))

# define kl_set_unaligned_16_le_ptr(p, v) do {\
		(p)[0] = (uint8_t)(v); \
		(p)[1] = (uint8_t)((v) >> 8); \
	} while (0)

# define kl_set_unaligned_16_be_ptr(p, v) do {\
		(p)[0] = (uint8_t)((v) >> 8); \
		(p)[1] = (uint8_t)(v); \
	} while (0)

# define kl_set_unaligned_32_le_ptr(p, v) do {\
		(p)[0] = (uint8_t)(v); \
		(p)[1] = (uint8_t)((v) >> 8); \
		(p)[2] = (uint8_t)((v) >> 16); \
		(p)[3] = (uint8_t)((v) >> 24); \
	} while (0)

# define kl_set_unaligned_32_be_ptr(p, v) do {\
		(p)[0] = (uint8_t)((v) >> 24); \
		(p)[1] = (uint8_t)((v) >> 16); \
		(p)[2] = (uint8_t)((v) >> 8); \
		(p)[3] = (uint8_t)(v); \
	} while (0)

# define kl_set_unaligned_16_le(p,v) \
	kl_set_unaligned_16_le_ptr (((uint8_t *)(p)), ((uint16_t)(v)))

# define kl_set_unaligned_16_be(p,v) \
	kl_set_unaligned_16_be_ptr (((uint8_t *)(p)), ((uint16_t)(v)))

# define kl_set_unaligned_32_le(p,v) \
	kl_set_unaligned_32_le_ptr (((uint8_t *)(p)), ((uint32_t)(v)))

# define kl_set_unaligned_32_be(p,v) \
	kl_set_unaligned_32_be_ptr (((uint8_t *)(p)), ((uint32_t)(v)))


# if KL_MAX_WORDSIZE >= 64

#  define kl_get_unaligned_64_le_ptr(p) \
	((uint64_t)(p)[0] | ((p)[1] << 8) | ((p)[2] << 16) | ((p)[3] << 24) \
	| ((p)[4] << 32) | ((p)[5] << 40) | ((p)[6] << 48) | ((p)[7] << 56))

#  define kl_get_unaligned_64_be_ptr(p) \
	(((uint64_t)(p)[0] << 56) | ((p)[1] << 48) | ((p)[2] << 40) | ((p)[3] << 32) \
	| ((p)[4] << 24) | ((p)[5] << 26) | ((p)[6] << 16) | (p)[7])

#  define kl_get_unaligned_64_le(p) \
	kl_get_unaligned_64_le_ptr ((const uint8_t *)(p))

#  define kl_get_unaligned_64_be(p) \
	kl_get_unaligned_64_be_ptr ((const uint8_t *)(p))

#  define kl_set_unaligned_64_le_ptr(p, v) do {\
		(p)[0] = (uint8_t)(v); \
		(p)[1] = (uint8_t)((v) >> 8); \
		(p)[2] = (uint8_t)((v) >> 16); \
		(p)[3] = (uint8_t)((v) >> 24); \
		(p)[4] = (uint8_t)((v) >> 32); \
		(p)[5] = (uint8_t)((v) >> 40); \
		(p)[6] = (uint8_t)((v) >> 48); \
		(p)[7] = (uint8_t)((v) >> 56); \
	} while (0)

#  define kl_set_unaligned_64_be_ptr(p, v) do {\
		(p)[0] = (uint8_t)((v) >> 56); \
		(p)[1] = (uint8_t)((v) >> 48); \
		(p)[2] = (uint8_t)((v) >> 40); \
		(p)[3] = (uint8_t)((v) >> 32); \
		(p)[4] = (uint8_t)((v) >> 24); \
		(p)[5] = (uint8_t)((v) >> 16); \
		(p)[6] = (uint8_t)((v) >> 8); \
		(p)[7] = (uint8_t)(v); \
	} while (0)

#  define kl_set_unaligned_64_le(p,v) \
	kl_set_unaligned_64_le_ptr ((uint8_t *)(p), (uint64_t)(v))

#  define kl_set_unaligned_64_be(p,v) \
	kl_set_unaligned_64_be_ptr ((uint8_t *)(p), (uint64_t)(v))

# endif // KL_MAX_WORDSIZE >= 64

#else // KL_NEED_ALIGNMENT

# define kl_get_unaligned_16_le(p) \
	kl_bswap_16_le (*((uint16_t *)(p)))

# define kl_get_unaligned_16_be(p) \
	kl_bswap_16_be (*((uint16_t *)(p)))

# define kl_get_unaligned_32_le(p) \
	kl_bswap_32_le (*((uint32_t *)(p)))

# define kl_get_unaligned_32_be(p) \
	kl_bswap_32_be (*((uint32_t *)(p)))

# define kl_set_unaligned_16_le(p,v) \
	(*((uint16_t *)(p)) = kl_bswap_16_le (v))

# define kl_set_unaligned_16_be(p,v) \
	(*((uint16_t *)(p)) = kl_bswap_16_be (v))

# define kl_set_unaligned_32_le(p,v) \
	(*((uint32_t *)(p)) = kl_bswap_32_le (v))

# define kl_set_unaligned_32_be(p,v) \
	(*((uint32_t *)(p)) = kl_bswap_32_be (v))

# if KL_MAX_WORDSIZE >= 64

#  define kl_get_unaligned_64_le(p) \
	kl_bswap_64_le (*((uint64_t *)(p)))

#  define kl_get_unaligned_64_be(p) \
	kl_bswap_64_be (*((uint64_t *)(p)))

#  define kl_set_unaligned_64_le(p,v) \
	(*((uint64_t *)(p)) = kl_bswap_64_le (v))

#  define kl_set_unaligned_64_be(p,v) \
	(*((uint64_t *)(p)) = kl_bswap_64_be (v))

# endif // KL_MAX_WORDSIZE >= 64

#endif // KL_NEED_ALIGNMENT


#ifdef KL_BIG_ENDIAN

# define kl_get_unaligned_16(p)   kl_get_unaligned_16_be(p)
# define kl_get_unaligned_32(p)   kl_get_unaligned_32_be(p)

# define kl_set_unaligned_16(p,v) kl_set_unaligned_16_be(p,v)
# define kl_set_unaligned_32(p,v) kl_set_unaligned_32_be(p,v)

#else // KL_BIG_ENDIAN

# define kl_get_unaligned_16(p)   kl_get_unaligned_16_le(p)
# define kl_get_unaligned_32(p)   kl_get_unaligned_32_le(p)

# define kl_set_unaligned_16(p,v) kl_set_unaligned_16_le(p,v)
# define kl_set_unaligned_32(p,v) kl_set_unaligned_32_le(p,v)

#endif // KL_BIG_ENDIAN

#if KL_MAX_WORDSIZE >= 64

# ifdef KL_BIG_ENDIAN

#  define kl_get_unaligned_64(p)   kl_get_unaligned_64_be(p)
#  define kl_set_unaligned_64(p,v) kl_set_unaligned_64_be(p,v)

# else // KL_BIG_ENDIAN

#  define kl_get_unaligned_64(p)   kl_get_unaligned_64_le(p)
#  define kl_set_unaligned_64(p,v) kl_set_unaligned_64_le(p,v)

# endif // KL_BIG_ENDIAN

#endif // KL_MAX_WORDSIZE >= 64


#endif // kl_byteswap_h_INCLUDED


