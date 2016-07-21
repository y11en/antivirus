#include <stdlib.h>

#include <Prague/prague.h>
#include <Prague/pr_cpp.h>
#include <Prague/iface/e_ktrace.h>

#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_loader.h>

// ---
template <class T>
inline tERROR _s2u( const tCHAR* str, T& value ) {
	tCHAR* p = 0;
	value = (T)strtoul( str, &p, 10 );
	if ( !p )
		return errUNEXPECTED;
	return errOK;
}

// ---
template <class T>
inline tERROR _s2l( const tCHAR* str, T& value ) {
	tCHAR* p = 0;
	value = (T)strtol( str, &p, 10 );
	if ( !p )
		return errUNEXPECTED;
	return errOK;
}

// ---
template <class T>
inline tERROR _s2c( const tCHAR* str, const tCHAR* cmp, tUINT len, T& value ) {
	if ( memcmp(str,cmp,len) )
		return errUNEXPECTED;
	tCHAR* p = 0;
	value = (T)strtol( str+len, &p, 10 );
	if ( !p )
		return errUNEXPECTED;
	return errOK;
}



// ---
extern "C" tERROR IMPEX_FUNC(pr_val2str)( tCHAR* buff, tUINT len, void* value, tTYPE_ID type ) {
	switch( type ) {
		case tid_VOID  : return errBAD_TYPE;
		case tid_BYTE  : pr_sprintf( buff, len, "%02x", *(tBYTE*)value ); return errOK;
		case tid_WORD  : pr_sprintf( buff, len, "%04x", *(tWORD*)value ); return errOK;
		case tid_DWORD : pr_sprintf( buff, len, "%08x", *(tDWORD*)value ); return errOK;
		case tid_QWORD : return errNOT_SUPPORTED; //pr_sprintf( buff, len, "%I64x", *(tQWORD*)value ); return errOK;
		case tid_BOOL  :
			if ( *(bool*)value )
				memcpy( buff, "true", sizeof("true") );
			else
				memcpy( buff, "false", sizeof("false") );
			return errOK;

		case tid_CHAR     : pr_sprintf( buff, len, "%03u", *(tCHAR*)value ); return errOK;
		case tid_WCHAR    : pr_sprintf( buff, len, "%05u", *(tWCHAR*)value ); return errOK;
		case tid_STRING   : return errNOT_SUPPORTED; //pr_sprintf( buff, len, "%s", *(const tCHAR**)value ); return errOK;
		case tid_WSTRING  : return errNOT_SUPPORTED; //pr_sprintf( buff, len, "%S", *(const tWCHAR**)value ); return errOK;
		case tid_ERROR    : pr_sprintf( buff, len, "%08u", *(tERROR*)value ); return errOK;
		case tid_PTR      : pr_sprintf( buff, len, "%08u", *(tPTR*)value ); return errOK;
		case tid_INT      : pr_sprintf( buff, len, "%d", *(tINT*)value ); return errOK;
		case tid_UINT     : pr_sprintf( buff, len, "%u", *(tUINT*)value ); return errOK;
		case tid_SBYTE    : pr_sprintf( buff, len, "%03u", *(tCHAR*)value ); return errOK;
		
		case tid_SHORT    : pr_sprintf( buff, len, "%d", *(tSHORT*)value ); return errOK;
		case tid_LONG     : pr_sprintf( buff, len, "%ld", *(tLONG*)value ); return errOK;
		case tid_LONGLONG : return errNOT_SUPPORTED; //pr_sprintf( buff, len, "%I64d", *(tLONGLONG*)value ); return errOK;
		case tid_IID      : pr_sprintf( buff, len, "iid%u", *(tIID*)value ); return errOK;
		case tid_PID      : pr_sprintf( buff, len, "pid%u", *(tPID*)value ); return errOK;
		case tid_ORIG_ID  : pr_sprintf( buff, len, "orig%u", *(tORIG_ID*)value ); return errOK;
		case tid_OS_ID    : pr_sprintf( buff, len, "os%u", *(tOS_ID*)value ); return errOK;
		case tid_VID      : pr_sprintf( buff, len, "vid%u", *(tVID*)value ); return errOK;
		case tid_PROPID   : pr_sprintf( buff, len, "prop%u", *(tPROPID*)value ); return errOK;
		case tid_VERSION  : pr_sprintf( buff, len, "version%u", *(tVERSION*)value ); return errOK;
		case tid_CODEPAGE : pr_sprintf( buff, len, "codepage%u", *(tCODEPAGE*)value ); return errOK;
		case tid_LCID     : pr_sprintf( buff, len, "lcid%u", *(tLCID*)value ); return errOK;
		case tid_DATA     : pr_sprintf( buff, len, "%lu", *(tDATA*)value ); return errOK;
		case tid_DATETIME :
		case tid_FUNC_PTR :
		
		// additional types
		case tid_BINARY      :
		case tid_IFACEPTR    :
		case tid_OBJECT      :
		case tid_EXPORT      :
		case tid_IMPORT      :
		case tid_TRACE_LEVEL : return errNOT_SUPPORTED;
		case tid_TYPE_ID     : pr_sprintf( buff, len, "%04u", *(tTYPE_ID*)value ); return errOK;
		default              : return errNOT_SUPPORTED;
	}
}


// ---
extern "C" tERROR IMPEX_FUNC(pr_str2val)( tVOID* value, tUINT len, tTYPE_ID type, const tCHAR* str ) {

	if ( !str || !*str )
		return errUNEXPECTED;
	
	//tUINT strLen;

	switch( type ) {
		case tid_VOID  : return errBAD_TYPE;
		case tid_BYTE  : return (len < sizeof(tBYTE))  ? errBUFFER_TOO_SMALL : _s2u( str, *(tBYTE*)value );
		case tid_WORD  : return (len < sizeof(tWORD))  ? errBUFFER_TOO_SMALL : _s2u( str, *(tWORD*)value );
		case tid_DWORD : return (len < sizeof(tDWORD)) ? errBUFFER_TOO_SMALL : _s2u( str, *(tDWORD*)value );
		case tid_QWORD : return errNOT_SUPPORTED;
		case tid_BOOL  :
			if ( len < sizeof(bool) )
				return errBUFFER_TOO_SMALL;
			if ( !memcmp(str,"true",4) )
				*(bool*)value = true;
			else if ( !memcmp(str,"false",5) )
				*(bool*)value = false;
			//else if ( !memcmp(str,"cTRUE",5) )
			//	*(tBOOL*)value = cTRUE;
			//else if ( !memcmp(str,"cFALSE",6) )
			//	*(tBOOL*)value = cFALSE;
			else
				return errUNEXPECTED;
			return errOK;
			
		case tid_CHAR     : return (len < sizeof(tCHAR))     ? errBUFFER_TOO_SMALL : _s2l( str, *(tCHAR*)value );
		case tid_WCHAR    : return (len < sizeof(tWCHAR))    ? errBUFFER_TOO_SMALL : _s2u( str, *(tWCHAR*)value );
		case tid_STRING   :
			return errNOT_SUPPORTED;
			//strLen = !str ? strlen(str)+1 : 1;
			//if ( len < strLen )
			//	return errBUFFER_TOO_SMALL;
			//memcpy( value, str, strLen );
			//return errOK;
		case tid_WSTRING  :
			return errNOT_SUPPORTED;
			//strLen = !str ? sizeof(tWCHAR) * (wcslen((const tWCHAR*)str)+1) : sizeof(tWCHAR);
			//if ( len < strLen )
			//	return errBUFFER_TOO_SMALL;
			//memcpy( value, str, strLen );
			//return errOK;
		case tid_ERROR    : return (len < sizeof(tERROR))    ? errBUFFER_TOO_SMALL : _s2u( str, *(tERROR*)value );
		case tid_PTR      : return (len < sizeof(tPTR))      ? errBUFFER_TOO_SMALL : _s2u( str, *(tPTR*)value );
		case tid_INT      : return (len < sizeof(tINT))      ? errBUFFER_TOO_SMALL : _s2l( str, *(tINT*)value );
		case tid_UINT     : return (len < sizeof(tUINT))     ? errBUFFER_TOO_SMALL : _s2u( str, *(tUINT*)value );
		case tid_SBYTE    : return (len < sizeof(tCHAR))     ? errBUFFER_TOO_SMALL : _s2u( str, *(tCHAR*)value );
			
		case tid_SHORT    : return (len < sizeof(tSHORT))    ? errBUFFER_TOO_SMALL : _s2l( str, *(tSHORT*)value );
		case tid_LONG     : return (len < sizeof(tLONG))     ? errBUFFER_TOO_SMALL : _s2l( str, *(tLONG*)value );
		case tid_LONGLONG : return errNOT_SUPPORTED; //_s2l( str, "%I64d", *(tLONGLONG*)value );
		case tid_IID      : return (len < sizeof(tIID))      ? errBUFFER_TOO_SMALL : _s2c( str, "iid", 3, *(tIID*)value );
		case tid_PID      : return (len < sizeof(tPID))      ? errBUFFER_TOO_SMALL : _s2c( str, "pid", 3, *(tPID*)value );
		case tid_ORIG_ID  : return (len < sizeof(tORIG_ID))  ? errBUFFER_TOO_SMALL : _s2c( str, "orig", 4, *(tORIG_ID*)value );
		case tid_OS_ID    : return (len < sizeof(tOS_ID))    ? errBUFFER_TOO_SMALL : _s2c( str, "os", 2, *(tOS_ID*)value );
		case tid_VID      : return (len < sizeof(tVID))      ? errBUFFER_TOO_SMALL : _s2c( str, "vid", 3, *(tVID*)value );
		case tid_PROPID   : return (len < sizeof(tPROPID))   ? errBUFFER_TOO_SMALL : _s2c( str, "prop", 4, *(tPROPID*)value );
		case tid_VERSION  : return (len < sizeof(tVERSION))  ? errBUFFER_TOO_SMALL : _s2c( str, "version", 7, *(tVERSION*)value );
		case tid_CODEPAGE : return (len < sizeof(tCODEPAGE)) ? errBUFFER_TOO_SMALL : _s2c( str, "codepage", 8, *(tCODEPAGE*)value );
		case tid_LCID     : return (len < sizeof(tLCID))     ? errBUFFER_TOO_SMALL : _s2c( str, "lcid", 4, *(tLCID*)value );
		case tid_DATA     : return (len < sizeof(tDATA))     ? errBUFFER_TOO_SMALL : _s2l( str, *(tDATA*)value );
		case tid_DATETIME :
		case tid_FUNC_PTR :
			
			// additional types
		case tid_BINARY      :
		case tid_IFACEPTR    :
		case tid_OBJECT      :
		case tid_EXPORT      :
		case tid_IMPORT      :
		case tid_TRACE_LEVEL : return errNOT_SUPPORTED;
		case tid_TYPE_ID     : return (len < sizeof(tTYPE_ID)) ? errBUFFER_TOO_SMALL : _s2u( str, *(tTYPE_ID*)value );
		default              : return errNOT_SUPPORTED;
	}
}



// ---
tINT IMPEX_FUNC(pr_strtol)( const tCHAR* str, tCHAR** endpoint, tUINT radix ) {
	return strtol( str, endpoint, radix );
}


// ---
tUINT IMPEX_FUNC(pr_strtoul)( const tCHAR* str, tCHAR** endpoint, tUINT radix ) {
	return strtoul( str, endpoint, radix );
}


