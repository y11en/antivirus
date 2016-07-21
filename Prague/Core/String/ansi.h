#if defined (_WIN32)
#pragma once
#endif

#if ( ! defined ( __prague__string__ansi_h__ ) )
#define __prague__string__ansi_h__

namespace CodecWin
{
    bool PrAnsiToUpper ( const void * src, void * dst, size_t len );
    bool PrAnsiToLower ( const void * src, void * dst, size_t len );
}

namespace CodecUnix
{
    bool PrAnsiToUpper ( const void * src, void * dst, size_t len );
    bool PrAnsiToLower ( const void * src, void * dst, size_t len );
}

#endif
