#ifndef _AK_TYPES
#define _AK_TYPES
#include <kl_types.h>

typedef unsigned long FSIZE;

typedef union
{
    uint8_t b[4];
    uint16_t w[2];
    uint32_t d;
} AK_DWORD ;

//#ifndef Malloc
//#define Malloc malloc
//#endif
//
//#ifndef Free
//#define Free free
//#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif
