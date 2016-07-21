#if ( defined ( _WIN32 ) )

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <Prague/prague.h>

#include "widechar.h"
// ---
extern tBOOL os2k;

#define UNI(a) ((tWCHAR*)(a))
#define MB(a)  ((tCHAR*)(a))


// ---
tERROR CodecWin::mb2wc( 
  const tBYTE * lpMultiByteStr, // string to map
  tUINT         cbMultiByte,    // number of bytes in string
  tBYTE *       lpWideCharStr,  // wide-character buffer
  tUINT         cchWideChar,    // size of buffer
  tUINT*        out_size        // byte size of conwerted symbols
) {
  
  tUINT  ret_val;
  
  if ( out_size )
    *out_size = 0;

  if ( !lpMultiByteStr )
    return errPARAMETER_INVALID;

	if ( lpWideCharStr && (cchWideChar < cbMultiByte) )
		cbMultiByte = cchWideChar;
	
  #if 0
    if ( os2k ) {
      LCID locale = GetThreadLocale();
		  LCID system = GetSystemDefaultLCID();

      if ( locale != system )
        SetThreadLocale( LOCALE_SYSTEM_DEFAULT );
  
      ret_val = MultiByteToWideChar( CP_THREAD_ACP, MB_PRECOMPOSED, reinterpret_cast<const tCHAR *> ( lpMultiByteStr ), cbMultiByte, reinterpret_cast<tWCHAR *> ( lpWideCharStr ), cchWideChar );
      //ret_val = mbstowcs( UNI(dst), src, src_len );
    
      if ( locale != system )
        SetThreadLocale( locale );
		  
    }
    else
  #endif
      ret_val = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, reinterpret_cast<const tCHAR *> ( lpMultiByteStr ), cbMultiByte, reinterpret_cast<tWCHAR *> ( lpWideCharStr ), cchWideChar );
  //ret_val = mbstowcs( UNI(dst), src, src_len );
  
  if ( out_size )
    *out_size = ret_val * sizeof(tWCHAR);

  if ( ret_val ) 
    return errOK;

  switch( GetLastError() ) {
    case ERROR_INSUFFICIENT_BUFFER    : 
			if ( out_size )
				*out_size = cbMultiByte * sizeof(tWCHAR);
			return errBUFFER_TOO_SMALL;
    case ERROR_INVALID_FLAGS          :
    case ERROR_INVALID_PARAMETER      : return errPARAMETER_INVALID;
    case ERROR_NO_UNICODE_TRANSLATION : return errCODEPAGE_NOT_SUPPORTED;
    default                           : return errUNEXPECTED;
  }
}




// ---
tERROR CodecWin::wc2mb(
  const tBYTE * lpWideCharStr,   // wide-character string
  tUINT         cchWideChar,     // number of chars in string
  tBYTE *       lpMultiByteStr,  // buffer for new string
  tUINT         cbMultiByte,     // size of buffer
  tUINT*        out_size          // byte size of conwerted symbols
) {
  
  int ret_val;

  if ( out_size )
    *out_size = 0;

  if ( !lpWideCharStr )
    return errPARAMETER_INVALID;

  if ( !cchWideChar )
    return errOK;

  else {
    #if 0
      if ( os2k ) {
        LCID locale = GetThreadLocale();
		    LCID system = GetSystemDefaultLCID();
        if ( locale != system )
          SetThreadLocale( LOCALE_SYSTEM_DEFAULT );
    
        ret_val = WideCharToMultiByte( CP_THREAD_ACP, WC_COMPOSITECHECK, reinterpret_cast<const tWCHAR *> ( lpWideCharStr ), cchWideChar, reinterpret_cast<tCHAR *> ( lpMultiByteStr ), cbMultiByte, 0, 0 ); 
        // ret_val = wcstombs( lpMultiByteStr, lpWideCharStr, cbMultiByte );
    
        if ( locale != system )
          SetThreadLocale( locale );
      }
      else
    #endif		
        ret_val = WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK, reinterpret_cast<const tWCHAR *> ( lpWideCharStr ), cchWideChar, reinterpret_cast<tCHAR *> ( lpMultiByteStr ), cbMultiByte, 0, 0 );

      if ( out_size )
        *out_size = ret_val;
      
      if ( ret_val ) 
        return errOK;

      switch( GetLastError() ) {
        case ERROR_INSUFFICIENT_BUFFER    : 
					if ( out_size )
						*out_size = cbMultiByte;
					return errBUFFER_TOO_SMALL;
        case ERROR_INVALID_FLAGS          :
        case ERROR_INVALID_PARAMETER      : return errPARAMETER_INVALID;
        default                           : return errUNEXPECTED;
      }
  }
  
  return ret_val;
}

//////////////////////////////////////////////////////////////////////////

bool CodecWin::PrWideCharToUpper ( const tWCHAR * src, tWCHAR * dst, size_t count ) {
	tWCHAR c;
	if ( src != dst ) {
		// fast ASCII convertion src != dst
		while( count ) {
			c = *src;
			if (c >= 0x80)
				break;
			if ( (c >= 'a') && (c <= 'z') )
				c &= ~0x20;
			*dst = c;
			count--;
			src++;
			dst++;
		}
		if ( count )
			memmove( dst, src, count*sizeof(tWCHAR) );
	}
	else {
		// fast ASCII convertion src == dst
		while( count ) {
			c = *dst;
			if ( c >= 0x80 )
				break;
			if ( (c >= 'a') && (c <= 'z') ) {
				c &= ~0x20;
				*dst = c;
			}
			count--;
			dst++;
		}
	}

	if ( 0 == count )
		return true;

	if ( os2k )
		return count == CharUpperBuffW( dst, (DWORD)count );

	if ( 0 == *(dst+count) )
		_wcsupr_s( dst, wcslen(dst) + 1 ), src = dst;
	else {
		tWCHAR c = *(dst+count);
		*(dst+count) = 0;
		_wcsupr_s( dst, wcslen(dst) + 1 ), src = dst;
		*(dst+count) = c;
	}
	return !!src;
}



// ---
bool CodecWin::PrWideCharToLower ( const tWCHAR* src, tWCHAR* dst, size_t count ) {
	tWCHAR c;
	if ( src != dst ) {
		// fast ASCII convertion src != dst
		while( count ) {
			c = *src;
			if (c >= 0x80)
				break;
			if ( (c >= 'A') && (c <= 'Z') )
				c |= 0x20;
			*dst = c;
			count--;
			src++;
			dst++;
		}
		if ( count )
			memmove( dst, src, count*sizeof(tWCHAR) );
	}
	else {
		// fast ASCII convertion src == dst
		while( count ) {
			c = *dst;
			if ( c >= 0x80 )
				break;
			if ( (c >= 'A') && (c <= 'Z') ) {
				c |= 0x20;
				*dst = c;
			}
			count--;
			dst++;
		}
	}

	if ( 0 == count )
		return true;

	if ( os2k )
		return count == CharLowerBuffW( dst, (DWORD)count );

	if ( 0 == *(dst+count) )
		_wcslwr_s( dst, wcslen(dst) + 1 ), src = dst;
	else {
		tWCHAR c = *(dst+count);
		*(dst+count) = 0;
		_wcslwr_s( dst, wcslen(dst) + 1 ), src = dst;
		*(dst+count) = c;
	}
	return !!src;
}



//////////////////////////////////////////////////////////////////////////

// #if ( defined ( _WIN32 ) )
#endif
