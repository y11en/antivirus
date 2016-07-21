#if defined (_WIN32)
#pragma once
#endif

#if ( ! defined ( __prague__string__widechar_h__ ) )
#define __prague__string__widechar_h__

namespace CodecWin
{
    tERROR mb2wc( 
      const tBYTE * lpMultiByteStr, // string to map
      tUINT         cbMultiByte,    // number of bytes in string
      tBYTE *       lpWideCharStr,  // wide-character buffer
      tUINT         cchWideChar,    // size of buffer
      tUINT*        out_size        // byte size of conwerted symbols
    );

    tERROR wc2mb(
      const tBYTE * lpWideCharStr,   // wide-character string
      tUINT         cchWideChar,     // number of chars in string
      tBYTE *       lpMultiByteStr,  // buffer for new string
      tUINT         cbMultiByte,     // size of buffer
      tUINT*        out_size         // byte size of conwerted symbols
    );

    bool PrWideCharToUpper ( const tWCHAR * src, tWCHAR * dst, size_t count );
    bool PrWideCharToLower ( const tWCHAR * src, tWCHAR * dst, size_t count );
}

namespace CodecUnix
{
    tERROR mb2wc( 
      const tBYTE * lpMultiByteStr, // string to map
      tUINT         cbMultiByte,    // number of bytes in string
      tBYTE *       lpWideCharStr,  // wide-character buffer
      tUINT         cchWideChar,    // size of buffer
      tUINT*        out_size        // byte size of conwerted symbols
    );

    tERROR wc2mb(
      const tBYTE * lpWideCharStr,   // wide-character string
      tUINT         cchWideChar,     // number of chars in string
      tBYTE *       lpMultiByteStr,  // buffer for new string
      tUINT         cbMultiByte,     // size of buffer
      tUINT*        out_size         // byte size of conwerted symbols
    );

    bool PrWideCharToUpper ( const tWCHAR * src, tWCHAR * dst, size_t count );
    bool PrWideCharToLower ( const tWCHAR * src, tWCHAR * dst, size_t count );
}

#endif
