#if defined (__unix__)

#include <wctype.h>
#include <errno.h>
#include <string.h>
#include <wchar.h>
#include <Prague/prague.h>
#include <stdlib.h>
#include "widechar.h"

tERROR CodecUnix::mb2wc ( const tBYTE * lpMultiByteStr, // string to map
                          tUINT         cbMultiByte,    // number of bytes in string
                          tBYTE *       lpWideCharStr,  // wide-character buffer
                          tUINT         cchWideChar,    // size of buffer
                          tUINT*        out_size )      // byte size of conwerted symbols 
{
  size_t  ret_val;
  
  tUINT size = 0;
  if ( !out_size )
    out_size = &size;
  *out_size = 0;

  const char* multiByteStr = reinterpret_cast<const char *> ( lpMultiByteStr );

  if ( cbMultiByte <= cchWideChar ) {
    mbstate_t state;
    memset ( &state, 0, sizeof ( state ) );
    
    ret_val = mbsrtowcs ( reinterpret_cast<wchar_t *> ( lpWideCharStr ), 
                          &multiByteStr, 
                          cbMultiByte,
                          &state );
  }
  else {
    *out_size = cbMultiByte * sizeof ( wchar_t );
    return errBUFFER_TOO_SMALL;
  }

  if ( ret_val == static_cast <size_t> ( -1 ) ) {
    *out_size = ( multiByteStr - reinterpret_cast<const char *> ( lpMultiByteStr ) ) * sizeof ( wchar_t );    
    return errUNEXPECTED;
  }
  *out_size = ret_val * sizeof ( wchar_t );
  return errOK;
}

tERROR CodecUnix::wc2mb ( const tBYTE * lpWideCharStr,   // wide-character string
                          tUINT         cchWideChar,     // number of chars in string
                          tBYTE *       lpMultiByteStr,  // buffer for new string
                          tUINT         cbMultiByte,     // size of buffer
                          tUINT*        out_size )       // byte size of conwerted symbols
{
  size_t  ret_val;
  
  tUINT size = 0;
  if ( !out_size )
    out_size = &size;
  *out_size = 0;

  const wchar_t* wideCharStr = reinterpret_cast<const wchar_t *> ( lpWideCharStr );

  if ( cbMultiByte >= cchWideChar ) {
    mbstate_t state;
    memset ( &state, 0, sizeof ( state ) );
    ret_val = wcsrtombs ( reinterpret_cast<char *> ( lpMultiByteStr ), 
                          &wideCharStr,
                          cchWideChar,
                          &state );
  }
  else {
    *out_size = cchWideChar;
    return errBUFFER_TOO_SMALL;
  }

  if ( ret_val == static_cast <size_t> ( -1 ) ) {
    *out_size = ( wideCharStr - reinterpret_cast<const wchar_t *> ( lpWideCharStr ) ) / sizeof ( wchar_t );    
    return errUNEXPECTED;
  }
  *out_size = ret_val;
  return errOK;
}


//////////////////////////////////////////////////////////////////////////

bool CodecUnix::PrWideCharToUpper ( const tWCHAR * src, tWCHAR * dst, size_t count )
{
    if ( src != dst )
    {
        memmove ( dst, src, count * sizeof ( tWCHAR ) );
    }

    for ( unsigned i = 0; i < count; ++i )
    {
        dst[i] = towupper ( dst[i] );
    }

    return true;
}

bool CodecUnix::PrWideCharToLower ( const tWCHAR * src, tWCHAR * dst, size_t count )
{
    if ( src != dst )
    {
        memmove ( dst, src, count * sizeof ( tWCHAR ) );
    }

    for ( unsigned i = 0; i < count; ++i )
    {
        dst[i] = towlower ( dst[i] );
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////

#endif
