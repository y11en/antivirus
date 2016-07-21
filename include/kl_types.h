/*
 * (C) 2003 Kaspersky Labs
 */

#ifndef kl_types_h_INCLUDED
#define kl_types_h_INCLUDED 1

#include <kl_platform.h>

#ifdef __cplusplus
/* request additional semantics */
# define __STDC_LIMIT_MACROS
# define __STDC_CONSTANT_MACROS
#endif /* __cplusplus */

#if  defined(__GNUC__) && defined(__KERNEL__) && defined(linux)
# include <linux/types.h>
# include <linux/version.h>
# if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
//FIXME check actual kernel version!
#  if defined __i386__
		typedef int32_t  intptr_t;
		typedef uint32_t uintptr_t;
#  elif defined __x86_64__ 
		typedef int64_t  intptr_t;
		typedef uint64_t uintptr_t;
#  endif
# endif
#elif defined(__GNUC__) && (!defined(__FreeBSD__) || __FreeBSD__ >= 5)
# include <inttypes.h>
#else
# if KL_WORDSIZE == 32 || KL_WORDSIZE == 64
#  include <kl_stdint.h>
# else
#  error Unsupported platform word size
# endif
#endif

#if !defined(__KERNEL__)
#include <limits.h>
#endif

#endif /* kl_types_h_INCLUDED */

