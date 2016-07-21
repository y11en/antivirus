/*
 * (C) 2003 Kaspersky Labs
 */

#ifndef kl_types_h_INCLUDED
#define kl_types_h_INCLUDED 1

#include "kl_platform.h"

#ifdef __cplusplus
/* request additional semantics */
# define __STDC_LIMIT_MACROS
# define __STDC_CONSTANT_MACROS
#endif /* __cplusplus */

#if defined(__GNUC__)
# include <inttypes.h>
#else
# if KL_WORDSIZE == 32 || KL_WORDSIZE == 64
#  include "kl_stdint.h"
# else
#  error Unsupported platform word size
# endif
#endif

#include <limits.h>

#endif /* kl_types_h_INCLUDED */

