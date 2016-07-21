#ifndef  _KL_DEF_H_
#define  _KL_DEF_H_

#define COMMON_TAG		    '-NeG'
#define GEN_PACKET_MARKER   'gids'

#define __SID_LENGTH	32
#define KL_CALLBACK     __cdecl

#ifdef ISWIN9X
    #include "w9xdef.h"
#else 

#ifdef USER_MODE
#include "ring3def.h"
#else // USER_MODE

#ifdef __CPP

extern "C" {

#ifdef _WIN64
#pragma pack ( push, 8 )
#endif
    #include <ndis.h>
#ifdef _WIN64
#pragma pack ( pop )
#endif

    #include "nt_def.h"
    #include <stdlib.H>
}
#else  // __CPP
    #include <ndis.h>
    #include "nt_def.h"
    #include <stdlib.H>
#endif  // __CPP

#endif // USER_MODE

#endif  // ISWIN9X

#ifndef min
#define min(a,b) ( ((a) <= (b)) ? (a) : (b) )
#endif

#ifndef max
#define max(a,b) ( ((a) >= (b)) ? (a) : (b) )
#endif

#ifndef htons
#define htons(_x_) (USHORT)(((_x_)<<8)|((_x_)>>8))
#endif

#ifndef htonl
#define htonl(_x_) (ULONG) ( (_x_<<24) | (((_x_)<<8)&0x00FF0000)|(((_x_)>>8)&0x0000FF00)|(((_x_)>>24)&0x000000FF) )
#endif

#ifndef ntohs
#define ntohs(_x_) htons(_x_)
#endif

#ifndef ntohl
#define ntohl(_x_) htonl(_x_)
#endif

#ifndef FIELD_OFFSET
#define FIELD_OFFSET(type, field)    ((LONG)&(((type *)0)->field))
#endif

#define localhost	0x100007f

// AddrToStr (ULONG, PCHAR)
#define AddrToStr(_a, _str)												\
{																		\
	PUCHAR	temp = (PUCHAR)&(_a);										\
	sprintf(_str, "%d.%d.%d.%d", temp[0], temp[1], temp[2], temp[3]);	\
}

#define KL_SYSNAME			    "System"
#define KL_SYSNAME_W           L"System"

#define ALLOCATE_STRUCT( _type_ ) (_type_*)KL_MEM_ALLOC( sizeof ( _type_) )

#ifdef _DEBUG
#define _DBG_LINE( x ) x
#else
#define _DBG_LINE( x )
#endif

#endif // _KL_DEF_H_