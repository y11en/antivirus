/*
 * (C) Kaspersky Labs
 */

#ifndef kl_platform_h_INCLUDED
#define kl_platform_h_INCLUDED 1

#include <stddef.h>

/* Microsoft Visual C++ or compatible */
#if defined (_MSC_VER)

/* Windows platforms */
/* Win32 is defined for both Win32 and Win64 */
# if defined(_WIN32)
#  define KL_PLATFORM_WINDOWS
#  if !defined(_WIN64)
/* Win32 */
#   define KL_PLATFORM_WIN32
#   define KL_WORDSIZE 32
#	define KL_MAX_WORDSIZE 64	
#   define KL_MODEL_ILP32
/* Win32+WinCE */
#   if defined (_WIN32_WCE)
#    define KL_PLATFORM_WINCE
#   endif
#  else
/* Win64 */
#   define KL_PLATFORM_WIN64
#   define KL_WORDSIZE 64
#	define KL_MAX_WORDSIZE 64
#   define KL_MODEL_LLP64
#   define KL_UNALIGNED_SUPPORTED
#   define KL_UNALIGNED __unaligned
#  endif

/* determine processor architecture */
#  if defined (_M_IX86)
#   define KL_ARCH_X86
#  elif defined (_M_AMD64)
#   define KL_ARCH_AMD64
#  elif defined (_M_IA64)
#   define KL_ARCH_IA64

#  elif defined (_M_ARM) || defined (ARM)
#	define KL_ARCH_ARM
#   define KL_NEED_ALIGNMENT

#  elif defined MIPS			// by ArtemK
#	define KL_ARCH_MIPS
#   define KL_NEED_ALIGNMENT

#  else
#   error Unknown processor architecture
#  endif

#  ifndef _INTEGRAL_MAX_BITS
#   error _INTEGRAL_MAX_BITS not defined
#  endif

// for all Microsoft platforms...
#  define KL_INT64_T __int64
#  define KL_WCHAR_T_SIZE 16
#  define KL_LITTLE_ENDIAN

# else
#  error Unsupported platform
# endif

#ifdef __cplusplus
# define KL_NO_VTABLE __declspec(novtable)
#else
# define KL_NO_VTABLE
#endif

#define KL_CDECL __cdecl

/* GNU C */
#elif defined (__GNUC__)

/* Unix platforms */
# if defined(__unix__)
#  define KL_PLATFORM_UNIX
#  if defined(__linux__)
#   define KL_PLATFORM_LINUX
#  elif defined(__APPLE__)
#   define KL_PLATFORM_MACOSX
#  endif
# endif

/* determine processor architecture */
# if defined __i386__
#  define KL_ARCH_X86
#  define KL_WORDSIZE 32
#  define KL_MAX_WORDSIZE 64
#  define KL_MODEL_ILP32
#  define KL_INT64_T long long
#  define KL_LITTLE_ENDIAN
# elif defined __ia64__
#  define KL_ARCH_IA64
#  define KL_WORDSIZE 64
#  define KL_MAX_WORDSIZE 64
#  define KL_MODEL_LP64
#  define KL_INT64_T long
#  define KL_LITTLE_ENDIAN
# elif defined __x86_64__
#  define KL_ARCH_AMD64
#  define KL_WORDSIZE 64
#  define KL_MAX_WORDSIZE 64
#  define KL_MODEL_LP64
#  define KL_INT64_T long
#  define KL_LITTLE_ENDIAN
# elif defined __sparc__
#  define KL_ARCH_SPARC
#  define KL_WORDSIZE 32
#  define KL_MAX_WORDSIZE 64
#  define KL_MODEL_ILP32
#  define KL_INT64_T long long
#  define KL_NEED_ALIGNMENT
#  define KL_BIG_ENDIAN
# elif defined __arm__
#  define KL_ARCH_ARM
#  define KL_WORDSIZE 32
#  define KL_MAX_WORDSIZE 64
#  define KL_MODEL_ILP32
#  define KL_INT64_T long long
#  define KL_NEED_ALIGNMENT
#  if defined (__ARMEL__)
#    define KL_LITTLE_ENDIAN
#  elif defined (__ARMEB__)
#    define KL_BIG_ENDIAN
#  else
#    error "Unknown byte order"     
#  endif
# elif defined (__ppc__) || defined (__PPC__) || defined (__powerpc__)
#  define KL_ARCH_PPC
#  define KL_WORDSIZE 32
#  define KL_MAX_WORDSIZE 64
#  define KL_MODEL_ILP32
#  define KL_INT64_T long long
#  define KL_NEED_ALIGNMENT
#  if defined (__LITTLE_ENDIAN__)
#    define KL_LITTLE_ENDIAN
#  elif defined (__BIG_ENDIAN__)
#    define KL_BIG_ENDIAN
#  else
#    error "Unknown byte order"     
#  endif
# else 
#  error Unknown processor architecture
# endif

# ifndef _INTEGRAL_MAX_BITS
#  define _INTEGRAL_MAX_BITS 64
# endif

//# define KL_WCHAR_T_SIZE 32
#if (__WCHAR_MAX__ == 0x7fffffff) || (__WCHAR_MAX__ == 0xffffffff)
# define KL_WCHAR_T_SIZE 32
#elif (__WCHAR_MAX__ == 0x7fff) || (__WCHAR_MAX__ == 0xffff)
# define KL_WCHAR_T_SIZE 16
#else
#error Could not define sizeof(wchar_t)
#endif

# define KL_CDECL
# define KL_NO_VTABLE

/* unknown compiler */
#else
# error Unsupported compiler
#endif

#if defined(KL_BIG_ENDIAN)
# define KL_INT64_PAIR(HI,LO) HI, LO
#elif defined(KL_LITTLE_ENDIAN)
# define KL_INT64_PAIR(HI,LO) LO, HI
#else
# error Undefined byte order
#endif

#if !defined (KL_UNALIGNED)
#   define KL_UNALIGNED
#endif

#ifndef KL_EXTERN_C
# if defined(__cplusplus)
#  define KL_EXTERN_C extern "C"
# else
#  define KL_EXTERN_C
# endif
#endif

#endif /* kl_platform_h_INCLUDED */

