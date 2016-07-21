#if defined (_WIN32)
#pragma once
#endif

#if ( ! defined ( __prague__string__oem_h__ ) )
#define __prague__string__oem_h__

namespace CodecWin
{
    bool PrOemToAnsi ( const void * src, void * dst, size_t len );
    bool PrAnsiToOem ( const void * src, void * dst, size_t len );

    bool PrOemToUpper ( const void * src, void * dst, size_t len );
    bool PrOemToLower ( const void * src, void * dst, size_t len );
}

namespace CodecUnix
{
    bool PrOemToAnsi ( const void * src, void * dst, size_t len );
    bool PrAnsiToOem ( const void * src, void * dst, size_t len );

    bool PrOemToUpper ( const void * src, void * dst, size_t len );
    bool PrOemToLower ( const void * src, void * dst, size_t len );
}

#endif
