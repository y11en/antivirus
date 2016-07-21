#ifndef _RDB_TYPES_H
#define _RDB_TYPES_H

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif

#ifndef __PR_TYPES_H
typedef long tERROR;
typedef unsigned int tDWORD;
#endif
#include <prague/pr_err.h>
#include <stddef.h>
typedef unsigned char    byte;
typedef unsigned int     uint;
typedef unsigned __int8  uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

typedef uint64 tObjectHash;
typedef uint64 tObjectId;
typedef uint64 tDateTime;
typedef uint32 tUniqPid;
typedef uint64 tNativePid;
typedef uint64 tEventId;
typedef uint32 tAppId;
typedef tObjectId tUserId;

typedef uint32 tShortObjectId;
#define GET_SEGMENT_FROM_ID(nLongObjectId) ((uint32)(((tObjectId)nLongObjectId) >> 32))
#define MAKE_LONG_OBJECT_ID(nShortObjectId) ((((tObjectId)m_nSegment)<<32) | (nShortObjectId))
#define MAKE_SHORT_OBJECT_ID(nLongObjectId) ((tShortObjectId)(nLongObjectId))

#define SIGN_BIT_MASK(value) (((value | 1) & 1) << (sizeof(value)*8-1))
#define IS_SIGN_BIT(value) ((((unsigned)value) & SIGN_BIT_MASK(value)) != 0)
#define DROP_SIGN_BIT(value) (((unsigned)value) & ~SIGN_BIT_MASK(value))

/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#endif // _RDB_TYPES_H
