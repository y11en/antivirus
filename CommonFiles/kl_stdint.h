/* 
 * <stdint.h> replacement for known platforms
 *
 * (C) 2003 Kaspersky Labs
 *
 * DO NOT include this file directly. It will be included, if needed, by kl_types.h
 *
 */

#ifndef kl_stdint_h_INCLUDED
#define kl_stdint_h_INCLUDED 1

#include "kl_platform.h"
#include <wchar.h>

#ifndef KL_WORDSIZE
# error KL_WORDSIZE not defined
#endif

#ifndef KL_INT64_T
# error KL_INT64_T not defined
#endif

#ifndef KL_WCHAR_T_SIZE
# error KL_WCHAR_T_SIZE not defined
#endif


/* sized types */
typedef	signed char int8_t;
typedef	short int   int16_t;
typedef	int         int32_t;
typedef	KL_INT64_T  int64_t;

typedef	unsigned char       uint8_t;
typedef	unsigned short int  uint16_t;
typedef	unsigned int        uint32_t;
typedef	unsigned KL_INT64_T uint64_t;

/* minimum-width types */
typedef signed char int_least8_t;
typedef short int   int_least16_t;
typedef int         int_least32_t;
typedef KL_INT64_T  int_least64_t;

typedef unsigned char      uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int       uint_least32_t;
typedef unsigned KL_INT64_T uint_least64_t;

/* fastest minimum-width types */
typedef signed char int_fast8_t;
#if KL_WORDSIZE >= 64
 typedef KL_INT64_T  int_fast16_t;
 typedef KL_INT64_T  int_fast32_t;
#else
 typedef int         int_fast16_t;
 typedef int         int_fast32_t;
#endif
typedef KL_INT64_T  int_fast64_t;

typedef unsigned char       uint_fast8_t;
#if KL_WORDSIZE >= 64
 typedef unsigned KL_INT64_T uint_fast16_t;
 typedef unsigned KL_INT64_T uint_fast32_t;
#else
 typedef unsigned int        uint_fast16_t;
 typedef unsigned int        uint_fast32_t;
#endif
typedef unsigned KL_INT64_T uint_fast64_t;

/* maximum-width types */
typedef KL_INT64_T          intmax_t;
typedef unsigned KL_INT64_T uintmax_t;

/* pointer-width types */
#if KL_WORDSIZE >= 64
 typedef KL_INT64_T          intptr_t;
 typedef unsigned KL_INT64_T uintptr_t;
#else
 typedef int          intptr_t;
 typedef unsigned int uintptr_t;
#endif

#ifdef _MSC_VER
# define KL_INT64_C(c)  c ## I64
# define KL_UINT64_C(c) c ## UI64
#else
# define KL_INT64_C(c)  c ## LL
# define KL_UINT64_C(c) c ## ULL
#endif

/* For C++, include this conditionally, as specified in C99 */
#if !defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS)

# define INT8_C(c)	c
# define INT16_C(c)	c
# define INT32_C(c)	c
# define INT64_C(c)	KL_INT64_C(c)

# define UINT8_C(c)	c##U
# define UINT16_C(c)	c##U
# define UINT32_C(c)	c##U 
# define UINT64_C(c)	KL_UINT64_C(c)

# define INTMAX_C(c)	INT64_C(c)
# define UINTMAX_C(c)	UINT64_C(c)

#endif	/* C++ */

/* For C++, include this conditionally, as specified in C99 */
#if !defined(__cplusplus) || defined(__STDC_LIMIT_MACROS)

/* Various limits  */
#define INT8_MIN     (-128)
#define INT16_MIN    (-32767-1)
#define INT32_MIN    (-2147483647-1)
#define INT64_MIN    (KL_INT64_C(-9223372036854775807)-1)

#define INT8_MAX     (127)
#define INT16_MAX    (32767)
#define INT32_MAX    (2147483647)
#define INT64_MAX    (KL_INT64_C(9223372036854775807))

#define UINT8_MAX     (255)
#define UINT16_MAX    (65535)
#define UINT32_MAX    (4294967295U)
#define UINT64_MAX    (KL_UINT64_C(18446744073709551615))

#define INT_LEAST8_MIN     INT8_MIN
#define INT_LEAST16_MIN    INT16_MIN
#define INT_LEAST32_MIN    INT32_MIN
#define INT_LEAST64_MIN    INT64_MIN

#define INT_LEAST8_MAX     INT8_MAX
#define INT_LEAST16_MAX    INT16_MAX
#define INT_LEAST32_MAX    INT32_MAX
#define INT_LEAST64_MAX    INT64_MAX

#define UINT_LEAST8_MAX    UINT8_MAX
#define UINT_LEAST16_MAX   UINT16_MAX
#define UINT_LEAST32_MAX   UINT32_MAX
#define UINT_LEAST64_MAX   UINT64_MAX

#define INT_FAST8_MIN      INT8_MIN
#define INT_FAST16_MIN     INT32_MIN
#define INT_FAST32_MIN     INT32_MIN
#define INT_FAST64_MIN     INT64_MIN

#define INT_FAST8_MAX      INT8_MAX
#define INT_FAST16_MAX     INT32_MAX
#define INT_FAST32_MAX     INT32_MAX
#define INT_FAST64_MAX     INT64_MAX

#define UINT_FAST8_MAX     UINT8_MAX
#define UINT_FAST16_MAX    UINT32_MAX
#define UINT_FAST32_MAX    UINT32_MAX
#define UINT_FAST64_MAX    UINT64_MAX

#define INTPTR_MIN         INT32_MIN
#define INTPTR_MAX         INT32_MAX
#define UINTPTR_MAX        UINT32_MAX

#define INTMAX_MIN         INT64_MIN
#define INTMAX_MAX         INT64_MAX

#define UINTMAX_MAX        UINT64_MAX

#if KL_WORDSIZE >= 64
 #define PTRDIFF_MIN     INT64_MIN
 #define PTRDIFF_MAX     INT64_MAX
#else
 #define PTRDIFF_MIN     INT32_MIN
 #define PTRDIFF_MAX     INT32_MAX
#endif

#define SIG_ATOMIC_MIN   INT32_MIN
#define SIG_ATOMIC_MAX   INT32_MAX

#ifndef SIZE_MAX
# if KL_WORDSIZE >= 64
#  define SIZE_MAX   UINT64_MAX
# else
#  define SIZE_MAX   UINT32_MAX
# endif
#endif

#ifndef WCHAR_MIN
# if KL_WCHAR_T_SIZE == 16
#  define WCHAR_MIN  (0U)
#  define WCHAR_MAX  UINT16_MAX
# elif KL_WCHAR_T_SIZE == 32
#  define WCHAR_MIN  (0U)
#  define WCHAR_MAX  UINT32_MAX
# else
#  error Unsupported KL_WCHAR_T_SIZE value
# endif
#endif

#ifndef WINT_MIN
# define WINT_MIN  WCHAR_MIN
# define WINT_MAX  WCHAR_MAX
#endif

#endif	/* C++ */

#endif /* kl_stdint_h_INCLUDED */


