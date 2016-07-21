#if !defined(_COMMON_)
#define _COMMON_

//Bitfield order
#ifdef _MSC_VER
#ifdef	_M_ALPHA
#ifndef __LITTLE_ENDIAN_BITFIELD
#define __LITTLE_ENDIAN_BITFIELD
#endif
#elif defined(_M_IX86) 
#ifndef __LITTLE_ENDIAN_BITFIELD
#define __LITTLE_ENDIAN_BITFIELD
#endif
#elif defined(_M_MPPC)
#ifndef __BIG_ENDIAN_BITFIELD
#define __BIG_ENDIAN_BITFIELD
#endif
#elif defined(_M_MRX000)
#error "Please specify little endian or Big endian format for MIPS platform manually"
#elif defined(_M_PPC)
#error "Please specify little endian or Big endian format for Power PC platform manually"
#endif //_M_ALPHA
#else //non _MSC_VER compiler
#if !defined(__BIG_ENDIAN_BITFIELD) && !defined(__LITTLE_ENDIAN_BITFIELD) 
#error "You use non Microsoft compiler, please specify bitfield order manualy"
#endif
#endif //_MSC_VER
#if defined(__BIG_ENDIAN_BITFIELD) && defined(__LITTLE_ENDIAN_BITFIELD) 
#error "Little endian and Big endian format was specified simultaneously!"
#endif

#endif //_COMMON_

