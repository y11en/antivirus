#ifndef __TYPES__H
#define __TYPES__H

#include <kl_types.h>
#include "tstring.h"

#if !defined(_SIZE_T_DEFINED) && !defined(_SIZE_T_DECLARED)
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#define _SIZE_T_DECLARED
#endif

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

//typedef unsigned __int64 uint64_t;
//typedef          __int64  int64;
//typedef unsigned __int32 uint32_t;
//typedef          __int32  int32;
//typedef unsigned __int16 uint16;
//typedef          __int16  int16;
//typedef unsigned __int8  uint8_t;
//typedef          __int8   int8;

typedef int64_t  tHANDLE;
typedef uint64_t tRegistryKey;
typedef uint64_t tPid;
typedef uint64_t tTid;
typedef uint64_t tAddr;
typedef uint32_t tVerdict;

#define evtVerdict_Default          0x0000
#define evtVerdict_Pending          0x0001
#define evtVerdict_Allow            0x0002
#define evtVerdict_Deny             0x0004
#define evtVerdict_TerminateProcess 0x0008
#define evtVerdict_TerminateThread  0x0010


#ifndef countof
#define countof(array) (sizeof(array) / sizeof((array)[0]))
#endif


#endif // __TYPES__H
