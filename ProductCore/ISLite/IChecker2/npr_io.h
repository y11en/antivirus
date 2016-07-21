#ifndef _NPR_IO_H_
#define _NPR_IO_H_

#if !defined(_NO_PRAGUE_)

#define CALL_IO_GetName(io, psize, name, size) CALL_SYS_PropertyGetStr(io, psize, pgOBJECT_NAME, name, size, cCP_ANSI)

#else // _NO_PRAGUE__

#include <stdio.h>
#if defined (__unix__)
#include <limits.h>
#ifdef __OpenBSD__ 
#include <sys/syslimits.h> 
#endif
#undef _MAX_PATH
#define _MAX_PATH PATH_MAX
#undef MAX_PATH
#define MAX_PATH PATH_MAX
#else
#include <io.h>
#endif
#include <stdlib.h>

typedef struct tag_IO
{
   int handle;
   int size;
   char filename[_MAX_PATH];
} * hIO;

tERROR CALL_IO_SeekRead(  hIO io, tDWORD* pdwResult, tQWORD qwOffset, void* pBuffer, tDWORD dwSize );

#define CALL_IO_GetSize(io, psize, type) \
	( *(psize) = (io)->size, errOK )

#define CALL_IO_GetName(io, psize, name, size) \
	( strncpy((name),(io)->filename,(size)), \
          (name)[(size)-1] = 0, \
          (psize) ? *(psize) = strlen((io)->filename) : (0), \
          errOK \
        )   

#endif // _NO_PRAGUE_

#endif //_NPR_IO_H_
