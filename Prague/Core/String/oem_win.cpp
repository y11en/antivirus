#if ( defined ( _WIN32 ) )

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "oem.h"

bool CodecWin::PrOemToAnsi ( const void * src, void * dst, size_t len )
{
    return ::OemToCharBuffA ( reinterpret_cast<LPCSTR> ( src ), reinterpret_cast<LPSTR> ( dst ), (DWORD)len ) == TRUE;
}

bool CodecWin::PrAnsiToOem ( const void * src, void * dst, size_t len )
{
    return ::CharToOemBuffA ( reinterpret_cast<LPCSTR> ( src ), reinterpret_cast<LPSTR> ( dst ), (DWORD)len ) == TRUE;
}

bool CodecWin::PrOemToUpper ( const void * src, void * dst, size_t len )
{
    if ( src != dst )
    {
        memmove ( dst, src, len );
    }

    return      PrOemToAnsi ( src, dst, len )
            &&  CharUpperBuffA ( static_cast<LPSTR> ( dst ), (DWORD)len ) == len
            &&  PrAnsiToOem ( dst, dst, len );
}

bool CodecWin::PrOemToLower ( const void * src, void * dst, size_t len )
{
    if ( src != dst )
    {
        memmove ( dst, src, len );
    }

    return      PrOemToAnsi ( src, dst, len )
            &&  CharLowerBuffA ( static_cast<LPSTR> ( dst ), (DWORD)len ) == len
            &&  PrAnsiToOem ( dst, dst, len );
}

#endif
