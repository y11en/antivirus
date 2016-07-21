#ifndef _DB_TYPES_H
#define _DB_TYPES_H

#define IN
#define OUT
#define OPTIONAL

typedef unsigned char    byte;
typedef unsigned int     uint;
typedef unsigned __int8  uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

typedef uint32 tUniqPid;
typedef uint64 tNativePid;
typedef uint32 tEventId;

#define AUTO_LOCK_W()
#define AUTO_LOCK_R()

/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#endif // _DB_TYPES_H
