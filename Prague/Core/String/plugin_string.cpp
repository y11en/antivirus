// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  23 September 2005,  15:09 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- petrovitch
// File Name   -- plugin_string.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
// AVP Prague stamp end



#if defined (_WIN32)
#include <windows.h>
#endif


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pstring.h"
// AVP Prague stamp begin( Plugin extern and export declaration,  )
extern tERROR pr_call String_Register( hROOT root );

#define  IMPEX_EXPORT_LIB
#include <Prague/plugin/p_string.h>

#define  IMPEX_TABLE_CONTENT
EXPORT_TABLE( string_export_table )
#include <Prague/plugin/p_string.h>
EXPORT_TABLE_END
// AVP Prague stamp end



#undef  IMPEX_EXPORT_LIB
#undef  IMPEX_TABLE_CONTENT
#define IMPEX_IMPORT_LIB
#include <Prague/iface/e_ktrace.h>

#define IMPEX_TABLE_CONTENT
IMPORT_TABLE(string_import_table)
	#include <Prague/iface/e_ktrace.h>
IMPORT_TABLE_END


extern "C" hROOT g_root;

#include "codec.h"

#include "ansi.h"
#include "widechar.h"

#if defined ( _WIN32 )
  using namespace CodecWin;
#elif defined (__unix__)
  using namespace CodecUnix;
#include <locale.h>
#include <wchar.h>
#endif


#ifndef min
#define min(a, b)  (((a) < (b)) ? (a) : (b)) 
#endif

tBOOL os2k = cFALSE;
tUINT g_maxANSICharSize;
extern "C" {
	tCODEPAGE g_cp_system_default;
}

const int StackBufferSize = 256;

#define _MAP_CP(p) \
  switch( p ) { \
    case cCP_NULL: p = cCP_ANSI; break; \
    case cCP_STRING_DEFAULT: p = cCP_UNICODE; break; \
    case cCP_SYSTEM_DEFAULT: p = g_cp_system_default; break; \
  }


// ---
extern "C" tERROR pr_call _init_string_iface( hROOT root ) {

	tDWORD os_ver;
	::g_root = root;
	::g_maxANSICharSize = 2;
	::os2k = cFALSE;
	::g_cp_system_default = cCP_ANSI;
	
	#if defined (_WIN32)

		os_ver = GetVersion();
		if ( 0x80000000 & os_ver )
			g_cp_system_default = cCP_ANSI;
		else
			g_cp_system_default = cCP_UNICODE;

		if ( LOBYTE(HIWORD(os_ver)) >= 5 ) // os resolve
			::os2k = cTRUE;
		else
			::os2k = cFALSE;
		
		CPINFO cpinfo;
		if ( GetCPInfo(CP_ACP,&cpinfo) )
			g_maxANSICharSize = cpinfo.MaxCharSize;
		else
			g_maxANSICharSize = 2;
	#else
		{
			setlocale ( LC_ALL, "" );
			g_cp_system_default = cCP_ANSI;
			g_maxANSICharSize = 2;
		}        
	#endif

	return PR_SUCC(String_Register(::g_root));
}


// ---
extern "C" tERROR pr_call _init_string_impex() {
  tDWORD count = 0;
	cERROR err = CALL_Root_RegisterExportTable( ::g_root, &count, string_export_table, PID_STRING );
	if ( PR_FAIL(err) )
		return err;
	return CALL_Root_ResolveImportTable( ::g_root, &count, string_import_table, PID_STRING );
}


#if !defined( ITS_LOADER )
// AVP Prague stamp begin( Plugin definitions,  )
PR_MAKE_TRACE_FUNC;


extern "C" tBOOL __stdcall DllMain( tPTR hInstance, tDWORD dwReason, tERROR* pERROR ) {
  switch( dwReason ) {

		case DLL_PROCESS_ATTACH:
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH :
    case DLL_THREAD_DETACH :
      break;

    case PRAGUE_PLUGIN_LOAD :
			*pERROR = _init_string_iface( (hROOT)hInstance );
			if ( PR_SUCC(*pERROR) )
				return cFALSE;
			*pERROR = _init_string_impex();
			return PR_SUCC(*pERROR);

    case PRAGUE_PLUGIN_UNLOAD :
      // free system resources
      // unregister my custom property ids -- you can drop it, kernel do it by itself
      // release    my imports		         -- you can drop it, kernel do it by itself
      // unregister my exports		         -- you can drop it, kernel do it by itself
      // unregister my interfaces          -- you can drop it, kernel do it by itself
      break;
  }
  return cTRUE;
}
// AVP Prague stamp end



#endif // ITS_LOADER
// ---
static tERROR pr_call CalcAddLen( tUINT len, tUINT flags, tDWORD* length ) {
  *length = 0;
  switch( flags & mSTRING_PR_FORMAT ) {
  case cSTRING_BLEN : 
    if ( len > cMAX_BYTE ) 
      return errBUFFER_TOO_SMALL; 
    else
      *length = 1;
    break;
  case cSTRING_WLEN :
    if ( len > cMAX_WORD ) 
      return errBUFFER_TOO_SMALL; 
    else
      *length = 2;
    break;
  case cSTRING_DLEN : 
    *length = 4;
    break;
  default : 
    break;
  }
  if ( flags & cSTRING_Z )
    (*length)++;
  if ( flags & cSTRING_LINE )
    (*length) += 2;
  return errOK;
}




// AVP Prague stamp begin( Plugin export method implementation, CalcExportLen )
// Parameters are:
tERROR CalcExportLen( const tPTR src_str, tDWORD src_len, tCODEPAGE src_cp, tCODEPAGE dst_cp, tDWORD dst_flags, tDWORD* dst_len ) {
	
	tUINT  p;
	tUINT  c;
	tERROR e;
	tDWORD dl;
	
	if ( !dst_len )
		dst_len = &dl;

  _MAP_CP(src_cp);
  _MAP_CP(dst_cp);	
	switch( dst_cp ) {
		
		case cCP_NULL:
			return errCODEPAGE_NOT_SUPPORTED;
			
		case cCP_OEM:
		case cCP_ANSI:
			if ( src_cp == cCP_UNICODE ) {
				if ( !src_str || (src_len && !*UNI(src_str)) )
					c = 0;
				else {
					register tUINT maxANSICharSize = g_maxANSICharSize;
					if ( (0 == src_len) && !(dst_flags&cSTRING_STRICT_SIZE) ) {
						c=0;
						while( *UNI(src_str) ) {
							c+=maxANSICharSize;
							++UNIR(src_str);
						}
					}
					else {
						tWCHAR* str_end = (tWCHAR*)src_str + src_len/2;
						for( c=0; src_str<str_end && *UNI(src_str); c+=maxANSICharSize,++UNIR(src_str))
							;
					}
				}
			}
			else if ( !src_str || (src_len && !*MB(src_str)) )
				c = 0;
			else {
				if ( (0 == src_len) && !(dst_flags&cSTRING_STRICT_SIZE) )
					src_len = (tDWORD)-1;
				for( c=0; c<src_len && *MB(src_str); c++,MBR(src_str)++ )
					;
			}

			if ( PR_SUCC(e=CalcAddLen(c,dst_flags,dst_len)) ) {
				*dst_len += c;
				return errOK;
			}
			return e;
			
		case cCP_UNICODE:
			if ( (src_cp == cCP_OEM) || (src_cp == cCP_ANSI) ) {
				if ( !src_str || (src_len && !*MB(src_str)) )
					c = 0;
				else {
					if ( (0 == src_len) && !(dst_flags&cSTRING_STRICT_SIZE) )
						src_len = (tDWORD)-1;
					for( c=0; c<src_len && *MB(src_str); ++c,++MBR(src_str) )
						;
				}
				if ( PR_SUCC(e=CalcAddLen(c,dst_flags,dst_len)) ) {
					*dst_len += c;
					if ( !(dst_flags & fSTRING_SYMBOLS) )
						*dst_len *= sizeof(tWCHAR);
					return errOK;
				}
				return e;
			}

			if ( !src_str || (src_len && !*UNI(src_str)) ) {
				p = 0;
				c = 0;
			}
			else {
				if ( (0 == src_len) && !(dst_flags&cSTRING_STRICT_SIZE) )
					src_len = (tDWORD)-1;
				for( p=0,c=0; (p<src_len) && *UNI(src_str); ++c,p+=sizeof(tWCHAR),++UNIR(src_str) )
					;
			}
			if ( PR_SUCC(e=CalcAddLen(p,dst_flags,dst_len)) ) {
				*dst_len += c;
				if ( !(dst_flags & fSTRING_SYMBOLS) )
					*dst_len *= sizeof(tWCHAR);
				return errOK;
			}
			return e;
			
		case cCP_IBM:
		case cCP_UTF8:
		case cCP_UNICODE_BIGENDIAN:
		default:
			return errCODEPAGE_NOT_SUPPORTED;
	}
	
}
// AVP Prague stamp end



// ---
static void add_crlf( tPTR dst, tDWORD* len ) {
	
	tUINT  l = *len;
	tCHAR* e = MB(dst) + l;
	
	if ( l && (*(e-1) == 0) ) {
		e--;
		l--;
	}
	
	if ( !l )
		;
	
	else if ( (l>1) && (*UNI(e-2) == MAKEWORD('\r','\n')) ) // already set -- nothing to do
		return;
	
	else if ( (l>1) && (*UNI(e-2) == MAKEWORD('\n','\r')) ) { // already set by reversing -- replace it
		e -= 2;
		l -= 2;
	}
	
	else if ( (*(e-1) == L'\r') || (*(e-1) == L'\n') ) { // single crlf symbol -- replace it
		e--;
		l--;
	}
	
	*UNI(e) = MAKEWORD( '\r', '\n' );
	*len = l + 2;
}




// ---
static void add_crlf_w( tPTR dst, tDWORD* len ) {
  
  tUINT   l = *len;
  tWCHAR* e = UNI(MB(dst) + l);
  
  if ( (l >= sizeof(tWCHAR)) && (*(e-1) == 0) ) {
    e -= 1;
    l -= sizeof(tWCHAR);
  }
  
  if ( !l )
    ;
  
  else if ( (l>sizeof(tWCHAR)) && (*((tDWORD*)(e-2)) == MAKELONG(L'\r',L'\n')) ) // already set -- nothing to do
    return;
  
  else if ( (l>sizeof(tWCHAR)) && (*((tDWORD*)(e-2)) == MAKELONG(L'\n',L'\r')) ) { // already set by reversing -- replace it
    e -= 2;
    l -= 2 * sizeof(tWCHAR);
  }
  
  else if ( (*(e-1) == L'\r') || (*(e-1) == L'\n') ) { // single crlf symbol -- replace it
    e -= 1;
    l -= sizeof(tWCHAR);
  }
  
  *((tDWORD*)e) = MAKELONG( L'\r', L'\n' );
  *len = l + 2;
}




// AVP Prague stamp begin( Plugin export method implementation, CopyTo )
// Parameters are:
tERROR CopyTo( tPTR dst_str, tDWORD dst_len, tCODEPAGE dst_cp, const tPTR src_str, tDWORD src_len, tCODEPAGE src_cp, tDWORD flags, tDWORD* output_len ) {

  tPTR  beg;
  tUINT wlen, postfix_len;
  tUINT dst_dec;
	tERROR err = errOK;

  _MAP_CP(src_cp);
  _MAP_CP(dst_cp);
	if ( !dst_str )
		return CalcExportLen( src_str, src_len, src_cp, dst_cp, flags, output_len );
	
  beg = dst_str;
  dst_dec = 0;
	postfix_len = 0;

  if ( output_len )
    *output_len = 0;

	if ( !src_str )
		src_len = 0;
	else if ( (src_cp == cCP_UNICODE) && (src_len % 2) )
		src_len -= 1;

  switch( flags & mSTRING_PR_FORMAT ) {
    case cSTRING_BLEN :
      (*(tBYTE**)&dst_str)++;
      dst_len -= sizeof(tBYTE);
      wlen = sizeof(tBYTE);
      break;

    case cSTRING_WLEN :
      (*(tWORD**)&dst_str)++;
      dst_len -= sizeof(tWORD);
      wlen = sizeof(tWORD);
      break;

    case cSTRING_DLEN :
      (*(tDWORD**)&dst_str)++;
      dst_len -= sizeof(tDWORD);
      wlen = sizeof(tDWORD);
      break;

    default:
      wlen = 0;
      break;
  }

  if ( flags & cSTRING_Z ) {
    switch( dst_cp ) {
      case cCP_OEM:
      case cCP_ANSI:
        dst_len -= (dst_dec += sizeof(tCHAR));
				postfix_len = sizeof(tCHAR);
        break;
      case cCP_UNICODE :
        dst_len -= (dst_dec += sizeof(tWCHAR));
				postfix_len = sizeof(tWCHAR);
        break;
    }
  }

  if ( flags & cSTRING_LINE ) {
    switch( dst_cp ) {
      case cCP_OEM:
      case cCP_ANSI:
        dst_len -= (dst_dec += 2 * sizeof(tCHAR));
				postfix_len = 2 * sizeof(tCHAR);
        break;
      case cCP_UNICODE :
        dst_len -= (dst_dec += 2 * sizeof(tWCHAR));
				postfix_len = 2 * sizeof(tWCHAR);
        break;
    }
  }

  if ( ((src_len==0) && !(flags&cSTRING_STRICT_SIZE)) && src_str ) {
    if ( src_cp == cCP_UNICODE ) {
      if ( *UNI(src_str) != 0 )
        src_len = _toui32(wcslen((tWCHAR*)src_str)*sizeof(tWCHAR));
    }
    else if ( (*MB(src_str)) != 0 )
      src_len = _toui32(strlen(MB(src_str)));
  }

  if ( flags & cSTRING_Z ) {
    switch( src_cp ) {
      case cCP_OEM:
      case cCP_ANSI:
        while( src_len && !*(MB(src_str)+src_len-1) )
          src_len--;
        break;
      case cCP_UNICODE :
        while ( (((tINT)src_len)>0) && !*(UNI(MB(src_str)+src_len)-1) )
          src_len -= sizeof(tWCHAR);
        break;
    }
  }

	bool calc_buff_len = false;
	tUINT divider = 1;
	tUINT multiplier = 1;

  switch ( src_cp ) {
		case cCP_OEM:
    case cCP_ANSI:
      switch ( dst_cp ) {
        case cCP_OEM     :
        case cCP_ANSI    : break;
				case cCP_UNICODE : divider = sizeof(tWCHAR)/sizeof(tCHAR); break;
      }
      break;

    case cCP_UNICODE:
      switch ( dst_cp ) {
				case cCP_OEM     :
				case cCP_ANSI    : 
					divider = g_maxANSICharSize;
					multiplier = sizeof(tWCHAR);
					break;
        case cCP_UNICODE : break;
      }
      break;
  }

	tUINT convert_len;
	if ( (src_len*divider) > (dst_len*multiplier) ) {
		calc_buff_len = true;
		convert_len = dst_len*multiplier/divider;
	}
	else
		convert_len = src_len;

	tUINT ReqOutBufLen = 0;
  if ( src_str && src_len ) {
		if ( src_cp == dst_cp ) {
			memcpy( dst_str, src_str, convert_len );
			ReqOutBufLen = src_len;
			if ( calc_buff_len )
				err = errBUFFER_TOO_SMALL;
		}
		else {
			err = PrStringCodec ( (tBYTE*)src_str, convert_len, src_cp, (tBYTE*)dst_str, dst_len, dst_cp, &ReqOutBufLen );
			convert_len = ReqOutBufLen;
			if ( PR_SUCC(err) && calc_buff_len ) {
				PrStringCodec ( (tBYTE*)src_str, src_len, src_cp, 0, 0, dst_cp, &ReqOutBufLen );
				err = errBUFFER_TOO_SMALL;
			}
		}
		src_len = convert_len;
  }
  else
    src_len = 0;

	if ( flags != cSTRING_CONTENT_ONLY ) {
    dst_len += dst_dec;
		switch( flags & mSTRING_PR_FORMAT ) {
			case cSTRING_BLEN :
				if ( src_len > cMAX_BYTE )
					return errBUFFER_TOO_SMALL;
				*(tBYTE*)beg = (tBYTE)src_len;
				break;
				
			case cSTRING_WLEN :
				if ( src_len > cMAX_WORD )
					return errBUFFER_TOO_SMALL;
				*(tWORD*)beg = (tWORD)src_len;
				break;
				
			case cSTRING_DLEN :
				*(tDWORD*)beg = (tDWORD)src_len;
				break;
		}
		switch( dst_cp ) {
			case cCP_OEM :
			case cCP_ANSI:
				if ( flags & cSTRING_LINE )
					add_crlf( dst_str, &src_len );
        if ( (flags & cSTRING_Z) && (!src_len || (0 != *(MB(dst_str)+(src_len-1)))) ) {
					*(MB(dst_str) + src_len) = 0;
					ReqOutBufLen += sizeof(tCHAR);
				}
        break;

      case cCP_UNICODE:
      case cCP_UNICODE_BIGENDIAN:
        if ( flags & cSTRING_LINE )
          add_crlf_w( dst_str, &src_len );
        if ( (flags & cSTRING_Z) && (!src_len || (0 != *(UNI(MB(dst_str)+src_len)-1))) ) {
          *UNI(MB(dst_str)+src_len) = 0;
          ReqOutBufLen += sizeof(tWCHAR);
        }
        break;
    }
  }

  if ( output_len )
    *output_len = wlen + ReqOutBufLen;
  return err;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, CopyToEx )
// Parameters are:
tERROR CopyToEx( tPTR dst_str, tDWORD dst_len, tCODEPAGE dst_cp, tDWORD dst_flags, const tPTR src_str, tDWORD src_len, tCODEPAGE src_cp, tDWORD src_flags, tDWORD* output_len ) {

  tPTR  beg;
  tUINT wlen;
  tUINT dst_dec;

  _MAP_CP(src_cp);
  _MAP_CP(dst_cp);
	if ( !dst_str )
		return CalcExportLen( src_str, src_len, src_cp, dst_cp, dst_flags, output_len );
	
  beg = dst_str;
  dst_dec = 0;

  if ( output_len )
    *output_len = 0;

	if ( !src_str )
		src_len = 0;
	
	switch( dst_flags & mSTRING_PR_FORMAT ) {
    case cSTRING_BLEN :
      (*(tBYTE**)&dst_str)++;
      dst_len -= sizeof(tBYTE);
      wlen = sizeof(tBYTE);
      break;

    case cSTRING_WLEN :
      (*(tWORD**)&dst_str)++;
      dst_len -= sizeof(tWORD);
      wlen = sizeof(tWORD);
      break;

    case cSTRING_DLEN :
      (*(tDWORD**)&dst_str)++;
      dst_len -= sizeof(tDWORD);
      wlen = sizeof(tDWORD);
      break;

    default:
      wlen = 0;
      break;
  }

  if ( dst_flags & cSTRING_Z ) {
    switch( dst_cp ) {
      case cCP_OEM:
      case cCP_ANSI:
        dst_len -= (dst_dec += sizeof(tCHAR));
        break;
      case cCP_UNICODE :
        dst_len -= (dst_dec += sizeof(tWCHAR));
        break;
    }
  }

  if ( dst_flags & cSTRING_LINE ) {
    switch( dst_cp ) {
      case cCP_OEM:
      case cCP_ANSI:
        dst_len -= (dst_dec += 2 * sizeof(tCHAR));
        break;
      case cCP_UNICODE :
        dst_len -= (dst_dec += 2 * sizeof(tWCHAR));
        break;
    }
  }

  if ( !src_len && !(src_flags&cSTRING_STRICT_SIZE) && src_str ) {
    if ( src_cp == cCP_UNICODE ) {
      if ( *UNI(src_str) != 0 )
        src_len = _toui32(wcslen((wchar_t*)src_str)*sizeof(tWCHAR));
    }
    else if ( (*MB(src_str)) != 0 )
      src_len = _toui32(strlen(MB(src_str)));
  }

  if ( src_flags & cSTRING_Z ) {
    switch( src_cp ) {
      case cCP_OEM:
      case cCP_ANSI:
        while( src_len && !*(MB(src_str)+src_len-1) )
          src_len--;
        break;
      case cCP_UNICODE :
        while ( (((tINT)src_len)>0) && !*(UNI(MB(src_str)+src_len)-1) )
          src_len -= sizeof(tWCHAR);
        break;
    }
  }

  switch ( dst_cp ) {
    case cCP_ANSI:
    case cCP_OEM:
      switch ( src_cp ) {
        case cCP_OEM:
        case cCP_ANSI:
          if ( src_len > dst_len )
            src_len = dst_len;
          break;
      }
      break;

    case cCP_UNICODE:
      switch ( src_cp ) {
        case cCP_UNICODE:
          if ( src_len > dst_len )
            src_len = dst_len;
          break;
      }
      break;
  }

  if ( src_str && src_len ) {
    tUINT ReqOutBufLen;
    tERROR err = PrStringCodec ( (tBYTE*)src_str, src_len, src_cp, (tBYTE*)dst_str, dst_len, dst_cp, & ReqOutBufLen );
    if ( PR_FAIL(err) )
      return err;
    src_len = ReqOutBufLen;
  }
	else
		src_len = 0;

	if ( dst_flags != cSTRING_CONTENT_ONLY ) {
    dst_len += dst_dec;
		switch( dst_flags & mSTRING_PR_FORMAT ) {
			case cSTRING_BLEN :
				if ( src_len > cMAX_BYTE )
					return errBUFFER_TOO_SMALL;
				*(tBYTE*)beg = (tCHAR)src_len;
				break;
				
			case cSTRING_WLEN :
				if ( dst_len > cMAX_WORD )
					return errBUFFER_TOO_SMALL;
				*(tWORD*)beg = (tWORD)src_len;
				break;
				
			case cSTRING_DLEN :
				*(tDWORD*)beg = (tDWORD)src_len;
				break;
		}
		switch( dst_cp ) {
			case cCP_OEM :
			case cCP_ANSI:
				if ( dst_flags & cSTRING_LINE )
					add_crlf( dst_str, &src_len );
				if ( dst_flags & cSTRING_Z ) {
					if ( dst_len > src_len ) {
						*(MB(dst_str) + src_len) = 0;
						src_len += sizeof(tCHAR);
					}
				}
        break;

      case cCP_UNICODE:
      case cCP_UNICODE_BIGENDIAN:
        if ( dst_flags & cSTRING_LINE )
          add_crlf_w( dst_str, &src_len );
        if ( dst_flags & cSTRING_Z ) {
          if ( *(UNI(MB(dst_str)+src_len)-1) != 0 ) {
            *UNI(MB(dst_str)+src_len) = 0;
            src_len += sizeof(tWCHAR);
          }
        }
        break;
    }
  }

  if ( output_len )
    *output_len = wlen + src_len;
  return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, ConvertLen )
// Parameters are:
tERROR ConvertLen( tDWORD src_len, tDWORD* dst_len, tCODEPAGE src_cp, tCODEPAGE dst_cp ) {

  tERROR error = errOK;

  PR_TRACE_A3( 0, "Enter ConvertLen(len=%u,src_cp=0x%x,dst_cp=0x%x) method", src_len, src_cp, dst_cp );
  _MAP_CP(src_cp);
  _MAP_CP(dst_cp);
  switch( src_cp ) {
		case cCP_OEM :
    case cCP_ANSI:
      switch( dst_cp ) {
        case cCP_OEM     :
        case cCP_ANSI    : break;
        case cCP_UNICODE : src_len *= sizeof(tWCHAR);         break;
        default          : error = errCODEPAGE_NOT_SUPPORTED; break;
      }
      break;
    case cCP_UNICODE :
      switch( dst_cp ) {
        case cCP_OEM     :
        case cCP_ANSI    : src_len = (src_len * g_maxANSICharSize) / sizeof(tWCHAR); break;
        case cCP_UNICODE : break;
        default          : error = errCODEPAGE_NOT_SUPPORTED;	 break;
      }
      break;
    default :	
      error = errCODEPAGE_NOT_SUPPORTED;	
      break;
  }
  if ( dst_len )
    *dst_len = src_len;
  PR_TRACE_A1( 0, "Leave %d = ConvertLen() method", src_len );
  return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, ImportFromProp )
// Parameters are:
tERROR ImportFromProp( hOBJECT src_obj, tPROPID src_propid, tPTR dst_buff, tDWORD dst_len, tDWORD dst_cp, tDWORD* result_len ) {
	tERROR    error;
  tDWORD    len = 0;
	tCODEPAGE from_cp;
  cObj*     sobj = (cObj*)src_obj;
  PR_TRACE_A0( sobj, "Enter ImportFromProp method" );

  _MAP_CP(dst_cp);

  if ( ((src_propid&pTYPE_MASK) != pTYPE_STRING) && ((src_propid&pTYPE_MASK) != pTYPE_WSTRING) )
    error = errPARAMETER_INVALID;

  else if ( !dst_buff != !dst_len )
    error = errPARAMETER_INVALID;

  else if ( !KNOWN_CP(dst_cp) )
    error = errCODEPAGE_NOT_SUPPORTED;

  else if ( PR_SUCC(error=sobj->propGetStrCP(&from_cp,src_propid)) ) {
    switch( from_cp ) {
      case cCP_OEM:
      case cCP_ANSI:
      case cCP_UNICODE:
        if ( dst_buff ) {
          if ( from_cp != dst_cp ) {
            tPTR tmp_buff = 0;
            if ( PR_FAIL(error=sobj->propGetStr(&len,src_propid,0,0,from_cp)) )
              ;
            else if ( PR_FAIL(error=CALL_SYS_ObjHeapAlloc(::g_root,&tmp_buff,len)) )
              ;
            else if ( PR_FAIL(error=sobj->propGetStr(&len,src_propid,tmp_buff,len,from_cp)) )
              ;
            else
              error = CopyTo( dst_buff, dst_len, dst_cp, tmp_buff, len, from_cp, cSTRING_Z, &len );
            if ( tmp_buff )
              CALL_SYS_ObjHeapFree( ::g_root, tmp_buff );
          }
          else
            error = sobj->propGetStr( &len, src_propid, dst_buff, len, from_cp );
        }
        else if ( PR_FAIL(error=sobj->propGetStr(&len,src_propid,0,0,from_cp)) )
          ;
        else if ( from_cp != dst_cp ) {
          switch( from_cp ) {
            case cCP_OEM :
            case cCP_ANSI    :
              if ( dst_cp == cCP_UNICODE )
                len *= sizeof(tWCHAR);
              break;
            case cCP_UNICODE :
              len /= sizeof(tWCHAR);
              break;
          }
        }
        break;

      case cCP_NULL:
      case cCP_IBM:
      case cCP_UTF8:
      case cCP_UNICODE_BIGENDIAN:
      default:
        error = errCODEPAGE_NOT_SUPPORTED;
        break;
    }
  }

  if ( result_len )
    *result_len = len;
  PR_TRACE_A2( 0, "Leave ImportFromProp method, ret tDWORD = %u, %terr", len, error );
  return error;
}
// AVP Prague stamp end



#define CMP( meth, s1, s2 ) ( !(meth)((s1),(s2)) ? errOK : errNOT_MATCHED )
// AVP Prague stamp begin( Plugin export method implementation, CmpStr )
// Parameters are:
tERROR CmpStr( const tVOID* p_str1, tDWORD p_str1_len, tCODEPAGE p_str1_cp, const tVOID* p_str2, tDWORD p_str2_len, tCODEPAGE p_str2_cp, tDWORD p_flags ) {

	if ( !p_str1 != !p_str2 )
		return errNOT_MATCHED;

	if ( !p_str1 )
		return errOK;
  _MAP_CP(p_str1_cp);
  _MAP_CP(p_str2_cp);
	tERROR err;
	if ( !(p_flags & cSTRING_STRICT_SIZE) ) {
		if ( !p_str1_len && PR_FAIL(err=CalcImportLen(p_str1,p_str1_cp,p_flags,0,&p_str1_len)) )
			return err;
		if ( !p_str2_len && PR_FAIL(err=CalcImportLen(p_str2,p_str1_cp,p_flags,0,&p_str2_len)) )
			return err;
	}

	if ( p_str1_cp == p_str2_cp ) {
		if ( fSTRING_COMPARE_CASE_INSENSITIVE == (p_flags & fSTRING_COMPARE_CASE_INSENSITIVE) ) {
			if ( (p_str1_cp == cCP_ANSI) || (p_str1_cp == cCP_OEM) ) {
				#if defined (_WIN32)
					if ( CSTR_EQUAL == CompareString(LOCALE_USER_DEFAULT,NORM_IGNORECASE|SORT_STRINGSORT,(LPCSTR)p_str1,p_str1_len,(LPCSTR)p_str2,p_str2_len) )
				#else
					if (!strnicmp ((LPCSTR)p_str1,(LPCSTR)p_str2,p_str1_len))
				#endif
						return errOK;
				return errNOT_MATCHED;
			}
			return CMP( _wcsicmp, (tWCHAR*)p_str1, (tWCHAR*)p_str2 );
		}
		if ( (p_str1_cp == cCP_ANSI) || (p_str1_cp == cCP_OEM) )
			return CMP( lstrcmp, (LPCSTR)p_str1, (LPCSTR)p_str2 );
		return CMP( wcscmp, (tWCHAR*)p_str1, (tWCHAR*)p_str2 );
	}
	else {
		tDWORD       len;
		tPTR         mem = 0;
		const tVOID* ps1;
		const tVOID* ps2;
		tDWORD       ps1len;
		tDWORD       ps2len;
		tCODEPAGE    ps1cp;
		tCODEPAGE    ps2cp;

		if ( p_str1_cp == cCP_UNICODE ) {
			ps1 = p_str1; ps1len = p_str1_len; ps1cp = p_str1_cp;
			ps2 = p_str2; ps2len = p_str2_len; ps2cp = p_str2_cp;
		}
		else {
			ps1 = p_str2; ps1len = p_str2_len; ps1cp = p_str2_cp;
			ps2 = p_str1; ps2len = p_str1_len; ps2cp = p_str1_cp;
		}

		err = CalcExportLen( const_cast<tVOID*>(ps2), ps2len, ps2cp, ps1cp, cSTRING_Z, &len );
		if ( PR_SUCC(err) )
			err = CALL_SYS_ObjHeapAlloc( g_root, &mem, len );
		if ( PR_SUCC(err) )
			err = CopyToEx( mem, len, ps2cp, cSTRING_Z, const_cast<tVOID*>(ps1), ps1len, ps1cp, cSTRING_Z, &len );
		if ( PR_SUCC(err) )
			err = CmpStr( ps1, ps1len, ps1cp, mem, len, ps2cp, p_flags );
		if ( mem )
			CALL_SYS_ObjHeapFree( g_root, mem );
		return err;
	}
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, GetSlashSymbol )
// Parameters are:
tSYMBOL GetSlashSymbol() {
#if defined (_WIN32)
	return '\\';
#else
	return '/';
#endif
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, ToUpperCase )
// Parameters are:
tERROR ToUpperCase( tVOID* p_str, tDWORD p_len, tCODEPAGE p_cp ) {
  _MAP_CP(p_cp);
	switch ( p_cp ) {
	case cCP_UNICODE:
		if ( PrWideCharToUpper ( reinterpret_cast <tWCHAR*> ( p_str ), reinterpret_cast <tWCHAR*> ( p_str ), p_len / sizeof ( tWCHAR )))
			return errOK;
		break;
	case cCP_ANSI:
		if ( PrAnsiToUpper ( p_str, p_str, p_len ) )
			return errOK;
		break;
	default:
		return errCODEPAGE_NOT_SUPPORTED;
	}
	return errUNEXPECTED;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, ToLowerCase )
// Parameters are:
tERROR ToLowerCase( tVOID* p_str, tDWORD p_len, tCODEPAGE p_cp ) {
  _MAP_CP(p_cp);
	switch ( p_cp ) {
	case cCP_UNICODE:
		if ( PrWideCharToLower ( reinterpret_cast <tWCHAR*> ( p_str ), reinterpret_cast <tWCHAR*> ( p_str ), p_len / sizeof ( tWCHAR )))
			return errOK;
		break;
	case cCP_ANSI:
		if ( PrAnsiToLower ( p_str, p_str, p_len ) )
			return errOK;
		break;
	default:
		return errCODEPAGE_NOT_SUPPORTED;
	}
	return errUNEXPECTED;
}
// AVP Prague stamp end



tERROR CompareWideStr( const tWCHAR* p_str1, tDWORD p_len1, const tWCHAR* p_str2, tDWORD p_len2, tDWORD p_flags, tSTR_COMPARE* p_result ) {
  if ( !p_len1 || !p_len2 ) {
    if ( p_len1 > p_len2 )
      *p_result = cSTRING_COMP_GT;
    else if ( p_len1 < p_len2 )
      *p_result = cSTRING_COMP_LT;
    else
      *p_result = cSTRING_COMP_EQ;
    return errOK;
  }

  if ( ( p_str1 == p_str2 ) || ( !p_str1 && !p_str2 ) ) {
    *p_result = cSTRING_COMP_EQ;
    return errOK;
  }

  int l_res = 0;
  int l_len = ( p_len1 < p_len2 ) ? p_len1 : p_len2;
  if ( ( p_flags & fSTRING_COMPARE_CASE_INSENSITIVE ) || ( p_flags & fSTRING_COMPARE_SLASH_INSENSITIVE ) ) {
    for ( int i = 0; i < l_len; ++i ) {
      tWCHAR l_ch1 = p_str1 [i];
      tWCHAR l_ch2 = p_str2 [i];
      if ( p_flags & fSTRING_COMPARE_CASE_INSENSITIVE ) {
        PrWideCharToLower ( &l_ch1, &l_ch1, 1 );
        PrWideCharToLower ( &l_ch2, &l_ch2, 1 );
      }
      if ( p_flags & fSTRING_COMPARE_SLASH_INSENSITIVE ) {
        if ( l_ch1 == L'/' )
          l_ch1 = L'\\';
        if ( l_ch2 == L'/' )
          l_ch2 = L'\\';
      }
      l_res = l_ch1 - l_ch2;
      if ( l_res )
        break;
    }
  }
  else
    l_res = wcsncmp ( p_str1, p_str2, l_len );

  if ( l_res > 0 )
    *p_result = cSTRING_COMP_GT;
  else if ( l_res < 0 )
    *p_result = cSTRING_COMP_LT;
  else if ( p_len1 > p_len2 )
    *p_result = cSTRING_COMP_GT;
  else if ( p_len1 < p_len2 )
    *p_result = cSTRING_COMP_LT;
  else
    *p_result = cSTRING_COMP_EQ;
  return errOK;
}

tERROR FindWideStr( const tWCHAR* p_str, tDWORD p_len, const tWCHAR* p_sub_str, tDWORD p_sub_len, tDWORD p_flags, tDWORD* p_result ) {
  if ( !p_str || !p_len || !p_sub_str || !p_sub_len )
    return errPARAMETER_INVALID;

  if ( p_len < p_sub_len )
    return errOK;
  tDWORD l_len = p_len - p_sub_len;
  tDWORD i = ( p_flags & fSTRING_FIND_BACKWARD ) ? l_len : 0;
  if ( ( p_flags & fSTRING_COMPARE_CASE_INSENSITIVE ) || ( p_flags & fSTRING_COMPARE_SLASH_INSENSITIVE ) ) {
    tWCHAR l_ch1 = p_sub_str [0];
    if ( p_flags & fSTRING_COMPARE_CASE_INSENSITIVE ) 
      PrWideCharToLower ( &l_ch1, &l_ch1, 1 );

    if ( ( p_flags & fSTRING_COMPARE_SLASH_INSENSITIVE ) && ( l_ch1 == L'/' ) )
      l_ch1 = L'\\';
    while ( i <= l_len ) {
      tWCHAR l_ch2 = p_str [i];
      if ( p_flags & fSTRING_COMPARE_CASE_INSENSITIVE ) 
        PrWideCharToLower ( &l_ch2, &l_ch2, 1 );

      if ( ( p_flags & fSTRING_COMPARE_SLASH_INSENSITIVE ) && (l_ch2 == L'/' ) )
        l_ch2 = L'\\';
      if ( l_ch1 == l_ch2 ) {
        tSTR_COMPARE l_res = cSTRING_COMP_UNDEF;
        tERROR l_err = CompareWideStr ( p_str + i, p_sub_len, p_sub_str, p_sub_len, p_flags, &l_res );
        if ( PR_FAIL ( l_err ) )
          return l_err;
        if ( l_res == cSTRING_COMP_EQ ) {
          *p_result = i;
          break;
        }
      }
      ( p_flags & fSTRING_FIND_BACKWARD ) ? --i : ++i;
    }
  }
  else
    while ( ( i >= 0 ) && ( i <= l_len ) ) {
      if ( ( p_str [ i ] == p_sub_str [ 0 ] ) &&
         ( wcsncmp ( p_str + i, p_sub_str, p_sub_len ) == 0 ) ) {
        *p_result = i;
        break;
      }
      ( p_flags & fSTRING_FIND_BACKWARD ) ? --i : ++i;
    }
  return errOK;
}

tERROR FindOneOfWideStr( const tWCHAR* p_str, tDWORD p_len, const tWCHAR* p_sub_str, tDWORD p_sub_len, tDWORD p_flags, bool p_is_included, tDWORD* p_result ) {
  if ( !p_str || !p_len || !p_sub_str || !p_sub_len )
    return errPARAMETER_INVALID;

  tDWORD i = ( p_flags & fSTRING_FIND_BACKWARD ) ? p_len - 1: 0;

  while ( i < p_len ) {
    bool l_found = false;
    for ( tDWORD j = 0; j < p_sub_len; j++ ) {
      tWCHAR l_ch1 = p_str [ i ];
      tWCHAR l_ch2 = p_sub_str [ j ];
      if ( p_flags & fSTRING_COMPARE_CASE_INSENSITIVE ) {
        PrWideCharToLower ( &l_ch1, &l_ch1, 1 );
        PrWideCharToLower ( &l_ch2, &l_ch2, 1 );
      }
      if ( p_flags & fSTRING_COMPARE_SLASH_INSENSITIVE ) {
        if ( l_ch1 == L'/' )
          l_ch1 = L'\\';
        if ( l_ch2 == L'/' )
          l_ch2 = L'\\';
      }

      if ( l_ch1 == l_ch2 ) {
        l_found = true;
        break;
      }
    }
    if ( ( l_found && p_is_included ) || ( !l_found && !p_is_included ) ) {
      *p_result = i;
      return errOK;
    }
    ( p_flags & fSTRING_FIND_BACKWARD ) ? --i : ++i;
  }
  return errOK;
}

inline tERROR checkRange ( const sString& p_str, tDWORD& p_pos, tDWORD& p_size ) {
  if ( p_pos > p_str.m_len )
    return errSTR_OUT_OF_RANGE;
  if ( p_size == cSTRING_WHOLE_LENGTH )
    p_size = p_str.m_len - p_pos;
  if ( ( p_size + p_pos ) > p_str.m_len )
    return errSTR_OUT_OF_RANGE;
  return errOK;
}


inline tERROR checkRange ( const StringData& p_str, tDWORD& p_pos, tDWORD& p_size ) {
  if ( p_str.cp == cCP_UNICODE ) 
    p_pos *= sizeof ( tWCHAR );
  if ( p_pos > p_str.len )
    return errSTR_OUT_OF_RANGE;
  if ( p_size == cSTRING_WHOLE_LENGTH )
    p_size = p_str.len - p_pos;
  else if ( p_str.cp == cCP_UNICODE )
    p_size *= sizeof ( tWCHAR );
  if ( ( p_size + p_pos ) > p_str.len )
    return errSTR_OUT_OF_RANGE;
  return errOK;  
}

#define CONVERT_BUFFER(_str,_len,_cp,_buff) \
  _MAP_CP(_cp); \
  tWCHAR l_str [ StackBufferSize ] = {0}; \
  _buff = l_str; \
  if ( _cp != cCP_UNICODE ) { \
      tDWORD l_len = sizeof ( l_str ) - 1; \
      l_err = CopyTo ( _buff, l_len, cCP_UNICODE, const_cast <tVOID*> ( _str ), _len, _cp, cSTRING_CONTENT_ONLY, &l_len ); \
      if ( PR_FAIL ( l_err ) ) { \
        if ( l_err != errBUFFER_TOO_SMALL ) \
          return l_err; \
        l_err = g_root->heapAlloc ( reinterpret_cast<tVOID**> ( &_buff ), l_len ); \
        if ( PR_FAIL ( l_err ) ) \
          return l_err; \
        l_err = CopyTo ( _buff, l_len, cCP_UNICODE, const_cast <tVOID*> ( _str ), _len, _cp, cSTRING_CONTENT_ONLY, 0 ); \
        if ( PR_FAIL ( l_err ) ) \
          return l_err; \
      } \
      _len = l_len; \
    } \
  else \
    _buff = reinterpret_cast<tWCHAR*> ( const_cast <tVOID*> ( _str ) );

#define FREE_BUFFER(_buff,_str) \
  if ( ( _buff != _str ) && ( _buff != l_str ) ) \
    g_root->heapFree ( _buff );



// AVP Prague stamp begin( Plugin export method implementation, CompareBuff )
// Parameters are:
tERROR CompareBuff( const sString* p_str, tDWORD p_pos, tDWORD p_size, const tVOID* p_other_str, tDWORD p_len, tCODEPAGE p_cp, tDWORD p_flags, tSTR_COMPARE* p_result ) {
  if ( !p_str ) 
    return errPARAMETER_INVALID;
  tERROR l_err = checkRange ( *p_str, p_pos, p_size );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  tWCHAR* l_buff = 0;
  CONVERT_BUFFER(p_other_str,p_len,p_cp,l_buff)
  tSTR_COMPARE l_result;
  if ( !p_result )
    p_result = &l_result;
  *p_result  = cSTRING_COMP_UNDEF;
  if ( p_len || PR_SUCC ( l_err=CalcImportLen( l_buff, p_cp, p_flags, 0, &p_len ) ) )
    l_err = CompareWideStr ( p_str->m_data + p_pos, p_size, l_buff, p_len / sizeof ( tWCHAR ), p_flags, p_result );
  FREE_BUFFER(l_buff,p_other_str)
  return l_err;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, FindBuff )
// Parameters are:
tERROR FindBuff( const sString* p_str, tDWORD p_pos, tDWORD p_size, const tVOID* p_other_str, tDWORD p_len, tCODEPAGE p_cp, tDWORD p_flags, tDWORD* p_result ) {
  if ( !p_str  )
    return errPARAMETER_INVALID;
  tERROR l_err = checkRange ( *p_str, p_pos, p_size );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  tWCHAR* l_buff = 0;
  CONVERT_BUFFER(p_other_str,p_len,p_cp,l_buff)
  tDWORD l_result;
  if ( !p_result )
    p_result = &l_result;
  *p_result = cSTRING_EMPTY_LENGTH;
  if ( p_len || PR_SUCC ( l_err=CalcImportLen( l_buff, p_cp, p_flags, 0, &p_len ) ) )
    l_err = FindWideStr ( p_str->m_data + p_pos, p_size, l_buff, p_len / sizeof ( tWCHAR ), p_flags, p_result );
  if ( PR_SUCC ( l_err ) && ( *p_result != cSTRING_EMPTY_LENGTH ) )
    *p_result += p_pos;
  FREE_BUFFER(l_buff,p_other_str)
  return l_err;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, ReplaceBuff )
// Parameters are:
tERROR ReplaceBuff( sString* p_str, const tVOID* p_pattern, tDWORD p_pattern_len, tCODEPAGE p_pattern_cp, tDWORD p_pattern_flags, const tVOID* p_replace_with, tDWORD p_replace_with_len, tCODEPAGE p_replace_with_cp ) {
  if ( !p_str )
    return errPARAMETER_INVALID;
  tERROR l_err = errOK;
  tWCHAR* l_buff = 0;
  CONVERT_BUFFER(p_pattern,p_pattern_len,p_pattern_cp,l_buff)
  tDWORD l_pos = cSTRING_EMPTY_LENGTH;
  if ( p_pattern_len || PR_SUCC ( l_err=CalcImportLen( l_buff, p_pattern_cp, 0, 0, &p_pattern_len ) ) )
    l_err = FindBuff ( p_str, 0, cSTRING_WHOLE_LENGTH,  l_buff, p_pattern_len, cCP_UNICODE, p_pattern_flags, &l_pos );
  FREE_BUFFER(l_buff,p_pattern)
  if ( PR_SUCC ( l_err ) )
    l_err = ReplaceBuffInPos ( p_str, l_pos, p_pattern_len / sizeof ( tWCHAR ), p_replace_with, p_replace_with_len, p_replace_with_cp );
  return l_err;

}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, GetSystemDefaultCp )
// Parameters are:
tCODEPAGE GetSystemDefaultCp() {
	return g_cp_system_default;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, GetSystemDefaultCp )
// Parameters are:
tDWORD GetMaxANSICharSize() {
	return g_maxANSICharSize;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, ReplaceBuffInPos )
// Parameters are:
tERROR ReplaceBuffInPos( sString* p_str, tDWORD p_pos, tDWORD p_size, const tVOID* p_other_str, tDWORD p_len, tCODEPAGE p_cp ) {
  if ( !p_str || !p_other_str )
    return errPARAMETER_INVALID;
  tERROR l_err = checkRange ( *p_str, p_pos, p_size );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  tWCHAR* l_buff = 0;
  CONVERT_BUFFER(p_other_str,p_len,p_cp,l_buff)
  if ( p_len || PR_SUCC ( l_err=CalcImportLen( l_buff, p_cp, 0, 0, &p_len ) ) ) {
    p_len /= sizeof ( tWCHAR );
    tDWORD l_size = p_str->m_len - p_size + p_len;
    if ( l_size > p_str->m_size ) {
      tVOID* l_res = 0;
      l_err = g_root->heapRealloc ( &l_res, p_str->m_data, ( l_size + 1 ) * sizeof (tWCHAR ));
      if ( PR_SUCC ( l_err ) ) {
        p_str->m_data = reinterpret_cast <tWCHAR*> ( l_res );
        p_str->m_size = l_size;
      }
    }
    if ( PR_SUCC ( l_err ) ) {
      memmove ( p_str->m_data + p_pos + p_len , p_str->m_data + p_pos + p_size, ( p_str->m_len - p_pos - p_size ) * sizeof ( tWCHAR ) );
      memmove ( p_str->m_data + p_pos, l_buff, p_len * sizeof ( tWCHAR ) );
      p_str->m_len = l_size;
      memset ( p_str->m_data + p_str->m_len, 0, ( p_str->m_size - p_str->m_len )  * sizeof ( tWCHAR ) );
    }
  }
  FREE_BUFFER(l_buff,p_other_str)
  return l_err;
}
// AVP Prague stamp end

// AVP Prague stamp begin( Plugin export method implementation, CompareStr )
// Parameters are:
tERROR CompareStr( const sString* p_str, tDWORD p_pos, tDWORD p_size, const hSTRING p_other_str, tDWORD p_other_pos, tDWORD p_other_size, tDWORD p_flags, tSTR_COMPARE* p_result ) {
  StringData* l_str = 0;
  if ( !p_other_str || ( ( l_str = CUST_TO_StringData(p_other_str) ) == 0 ) )
    return errPARAMETER_INVALID;
  tERROR l_err = checkRange ( *l_str, p_other_pos, p_other_size );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  return CompareBuff ( p_str, p_pos, p_size, l_str->buff + p_other_pos, p_other_size, l_str->cp, p_flags, p_result );
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, FindStr )
// Parameters are:
tERROR FindStr( const sString* p_str, tDWORD p_pos, tDWORD p_size, hSTRING p_other_str, tDWORD p_other_pos, tDWORD p_other_size, tDWORD p_flags, tDWORD* p_result ) {
  StringData* l_str = 0;
  if ( !p_other_str || ( ( l_str = CUST_TO_StringData(p_other_str) ) == 0 ) )
    return errPARAMETER_INVALID;

  tERROR l_err = checkRange ( *l_str, p_other_pos, p_other_size );
  if ( PR_FAIL ( l_err ) )
    return l_err;

  return FindBuff ( p_str, p_pos, p_size, l_str->buff + p_other_pos, p_other_size, l_str->cp, p_flags, p_result );
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, ReplaceStrInPos )
// Parameters are:
tERROR ReplaceStrInPos( sString* p_str, tDWORD p_pos, tDWORD p_size, hSTRING p_other_str, tDWORD p_other_pos, tDWORD p_other_size ) {
  StringData* l_str = 0;
  if ( !p_other_str || ( ( l_str = CUST_TO_StringData(p_other_str) ) == 0 ) )
    return errPARAMETER_INVALID;

  tERROR l_err = checkRange ( *l_str, p_other_pos, p_other_size );
  if ( PR_FAIL ( l_err ) )
    return l_err;

  return ReplaceBuffInPos ( p_str, p_pos, p_size, l_str->buff + p_other_pos, p_other_size, l_str->cp );
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, ReplaceStr )
// Parameters are:
tERROR ReplaceStr( sString* p_str, hSTRING p_pattern, tDWORD p_pattern_pos, tDWORD p_pattern_size, tDWORD p_pattern_flags, hSTRING p_replace_with, tDWORD p_replace_with_pos, tDWORD p_replace_with_size ) {
  StringData* l_pattern_str = 0;
  if ( !p_pattern || ( ( l_pattern_str = CUST_TO_StringData(p_pattern) ) == 0 ) )
    return errPARAMETER_INVALID;
  StringData* l_replace_str = 0;
  if ( !p_replace_with || ( ( l_replace_str = CUST_TO_StringData(p_replace_with) ) == 0 ) )
    return errPARAMETER_INVALID;

  tERROR l_err = checkRange ( *l_pattern_str, p_pattern_pos, p_pattern_size );
  if ( PR_FAIL ( l_err ) )
    return l_err;

  l_err = checkRange ( *l_replace_str, p_replace_with_pos, p_replace_with_size );
  if ( PR_FAIL ( l_err ) )
    return l_err;

  return ReplaceBuff ( p_str,
                       l_pattern_str->buff + p_pattern_pos, p_pattern_size, l_pattern_str->cp, p_pattern_flags,
                       l_replace_str->buff + p_replace_with_pos, p_replace_with_size, l_replace_str->cp );
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, FindOneOfBuff )
// Parameters are:
tERROR FindOneOfBuff( const sString* p_str, tDWORD p_pos, tDWORD p_size, const tVOID* p_other_str, tDWORD p_len, tCODEPAGE p_cp, tDWORD p_flags, tBOOL p_is_included, tDWORD* p_result ) {
  if ( !p_str || !p_other_str )
    return errPARAMETER_INVALID;
  tERROR l_err = checkRange ( *p_str, p_pos, p_size );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  tWCHAR* l_buff = 0;
  CONVERT_BUFFER(p_other_str,p_len,p_cp,l_buff)
  tDWORD l_result;
  if ( !p_result )
    p_result = &l_result;
  *p_result = cSTRING_EMPTY_LENGTH;
  if ( p_len || PR_SUCC ( l_err=CalcImportLen( l_buff, p_cp, 0, 0, &p_len ) ) )
      l_err = FindOneOfWideStr ( p_str->m_data + p_pos, p_size, l_buff, p_len / sizeof ( tWCHAR ), p_flags, p_is_included == cTRUE, p_result );
  if ( PR_SUCC ( l_err ) && ( *p_result != cSTRING_EMPTY_LENGTH ) )
    *p_result += p_pos;

  FREE_BUFFER(l_buff,p_other_str)
  return l_err;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin export method implementation, FindOneOfStr )
// Parameters are:
tERROR FindOneOfStr( const sString* p_str, tDWORD p_pos, tDWORD p_size, hSTRING p_other_str, tDWORD p_other_pos, tDWORD p_other_size, tDWORD p_flags, tBOOL p_is_included, tDWORD* p_result ) {
  StringData* l_str = 0;
  if ( !p_other_str || ( ( l_str = CUST_TO_StringData(p_other_str) ) == 0 ) )
    return errPARAMETER_INVALID;

  tERROR l_err = checkRange ( *l_str, p_other_pos, p_other_size );
  if ( PR_FAIL ( l_err ) )
    return l_err;

  return FindOneOfBuff ( p_str, p_pos, p_size, l_str->buff + p_other_pos, p_other_size, l_str->cp, p_flags, p_is_included, p_result );
}
// AVP Prague stamp end




extern "C" tERROR pr_call i_CalcImportLen( tPTR* p_from, tCODEPAGE p_cp, tDWORD p_flags, tDWORD p_len, tDWORD* p_olen ) {
  tUINT ml;
  tVOID* p = *p_from;

  if ( !p ) {
    *p_olen = 0;
    return errOK;
  }

  switch( p_flags & mSTRING_PR_FORMAT ) {
    case cSTRING_BLEN : p_len = p_len ? min(p_len-sizeof(tBYTE),*(tBYTE*)*p_from)   : *(tBYTE*)*p_from;  (*(tBYTE**)p_from)++;  break;
    case cSTRING_WLEN : p_len = p_len ? min(p_len-sizeof(tWORD),*(tWORD*)*p_from)   : *(tWORD*)*p_from;  (*(tWORD**)p_from)++;  break;
    case cSTRING_DLEN : p_len = p_len ? min(p_len-sizeof(tDWORD),*(tDWORD*)*p_from) : *(tDWORD*)*p_from; (*(tDWORD**)p_from)++; break;
  }

  switch( p_flags & mSTRING_TERM_FORMAT ) {
    case cSTRING_Z :
      ml = p_len ? p_len : cMAX_WORD;
      if ( (p_cp == cCP_ANSI) || (p_cp == cCP_OEM) ) {
        for( p_len=0; (p_len<ml) && (*MB(p)!=0); p_len++,MBR(p)++ )
          ;
      }
      else if ( p_cp == cCP_UNICODE || p_cp == cCP_UNICODE_BIGENDIAN ) {
        for( p_len=0; (p_len<ml) && (*UNI(p)!=0); p_len+=sizeof(tWCHAR),UNIR(p)++ )
          ;
      }
      else
        return errCODEPAGE_NOT_SUPPORTED;
      break;

    case cSTRING_LINE :
      ml = p_len ? p_len : cMAX_WORD;
      if ( (p_cp == cCP_ANSI) || (p_cp == cCP_OEM) ) {
        for( p_len=0; (p_len<ml) && ((*MB(p) != '\r') && (*MB(p) != '\n')); p_len++,MBR(p)++ )
          ;
      }
      else if ( p_cp == cCP_UNICODE ) {
        for( p_len=0; (p_len<ml) && ((*UNI(p) != L'\r') && (*UNI(p) != L'\n')); p_len+=sizeof(tWCHAR),UNIR(p)++ )
          ;
      }
      else if ( p_cp == cCP_UNICODE_BIGENDIAN ) {
        for( p_len=0; (p_len<ml) && ((*UNI(p) != ('\r'<<8)) && (*UNI(p) != ('\n'<<8))); p_len+=sizeof(tWCHAR),UNIR(p)++ )
          ;
      }
      else
        return errCODEPAGE_NOT_SUPPORTED;
      break;

    case (cSTRING_Z|cSTRING_LINE) :
    default                       :
      ml = p_len ? p_len : cMAX_WORD;
      if ( (p_cp == cCP_ANSI) || (p_cp == cCP_OEM) ) {
        for( p_len=0; (p_len<ml) && ((*MB(p) != '\0') /*&& (*MB(p) != '\r') && (*MB(p) != '\n')*/); p_len++,MBR(p)++ )
          ;
      }
      else if ( p_cp == cCP_UNICODE ) {
        for( p_len=0; (p_len<ml) && ((*UNI(p) != L'\0') /*&& (*UNI(p) != L'\r') && (*UNI(p) != L'\n')*/); p_len+=sizeof(tWCHAR),UNIR(p)++ )
          ;
      }
      else if ( p_cp == cCP_UNICODE_BIGENDIAN ) {
        for( p_len=0; (p_len<ml) && ((*UNI(p) != L'\0') /*&&(*UNI(p) != ('\r'<<8)) && (*UNI(p) != ('\n'<<8))*/); p_len+=sizeof(tWCHAR),UNIR(p)++ )
          ;
      }
      else
        return errCODEPAGE_NOT_SUPPORTED;
      break;
  }

  *p_olen = p_len;
  return errOK;
}


// AVP Prague stamp begin( Plugin export method implementation, CalcImportLen )
// Parameters are:

tERROR CalcImportLen( const tVOID* p_from, tCODEPAGE p_cp, tDWORD p_flags, tDWORD p_len, tDWORD* p_olen ) {
  if ( ((p_flags & ~cSTRING_Z) & mSTRING_PR_FORMAT) ) 
		return errPARAMETER_INVALID;
	return i_CalcImportLen( const_cast<tVOID**> ( &p_from ), p_cp, p_flags, p_len, p_olen );
}
// AVP Prague stamp end



