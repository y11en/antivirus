#if ( defined ( _WIN32 ) )

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "ansi.h"

bool CodecWin::PrAnsiToUpper ( const void * src, void * dst, size_t len )
{
    if ( src != dst )
    {
        memmove ( dst, src, len );
    }

    return CharUpperBuffA ( static_cast<LPSTR> ( dst ), (DWORD)len ) == len;
}

bool CodecWin::PrAnsiToLower ( const void * src, void * dst, size_t len )
{
    if ( src != dst )
    {
        memmove ( dst, src, len );
    }

    return CharLowerBuffA ( static_cast<LPSTR> ( dst ), (DWORD)len ) == len;
}

#endif
