#include <Prague/prague.h>
#include <Prague/pr_serializable.h>

#include <windows.h>

#define HK(a)  ((HKEY)(a))

// ---
extern "C" tERROR pr_call _ConvertRegErr( long err );


// ---
extern "C" tERROR pr_call Registry_SetSerVal( tDWORD* werr, tRegKey key, const tCHAR* szValueName, tPTR pValue ) {
	if ( !pValue ) {
		*werr = ::RegSetValueEx( HK(key), szValueName, 0, REG_BINARY, (LPBYTE)0, 0 );
		if ( *werr != ERROR_SUCCESS )
			return _ConvertRegErr( *werr );
		return errOK;
	}

	tDWORD size = 0;
	cChunkBuff buff( ::g_root );
	cERROR err = ::g_root->StreamSerialize( (cSerializable*)pValue, SERID_UNKNOWN, (tBYTE*)buff.ptr(), buff.allocated(), &size );
	if ( err == errBUFFER_TOO_SMALL ) {
		if ( !buff.reserve(size,false) )
			return errNOT_ENOUGH_MEMORY;
		err = ::g_root->StreamSerialize( (cSerializable*)pValue, SERID_UNKNOWN, (tBYTE*)buff.ptr(), buff.allocated(), &size );
	}
	if ( PR_FAIL(err) )
		return err;
	*werr = ::RegSetValueEx( HK(key), szValueName, 0, REG_BINARY, (tBYTE*)buff.ptr(), size );
	if ( *werr != ERROR_SUCCESS )
		return _ConvertRegErr( *werr );
	return errOK;
}



// ---
extern "C" tERROR pr_call Registry_GetSerVal( tDWORD* werr, tRegKey key, const tCHAR* szValueName, DWORD* reg_type, tPTR pValue, tDWORD* out_size ) {
	cERROR err;
	cChunkBuff buff( ::g_root );
	tPTR ptr = (tPTR)buff.ptr();
	
	*reg_type = REG_NONE;
	*out_size = buff.size();
	*werr = ::RegQueryValueEx( HK(key), szValueName, 0, reg_type, (LPBYTE)ptr, (LPDWORD)out_size );
	if ( *werr == ERROR_MORE_DATA ) {
		tPTR ptr = buff.reserve( *out_size, false );
		if ( !ptr ) {
			*werr = ERROR_NOT_ENOUGH_MEMORY;
			return errNOT_ENOUGH_MEMORY;
		}
		*werr = ::RegQueryValueEx( HK(key), szValueName, 0, reg_type, (LPBYTE)ptr, (LPDWORD)out_size );
	}
	if ( *werr != ERROR_SUCCESS )
		return _ConvertRegErr( *werr );
	if ( *reg_type != REG_BINARY )
		return errPARAMETER_INVALID;
	if ( !*out_size ) {
		(*(cSerializable**)&pValue)->clear();
		return err;
	}
	return ::g_root->StreamDeserialize( (cSerializable**)&pValue, (tBYTE*)ptr, *out_size, 0 );
}




// ---
extern "C" tERROR pr_call Registry_GetBufVal( tDWORD* werr, tRegKey key, const tCHAR* szValueName, DWORD* reg_type, tPTR pValue, tDWORD* out_size ) {
	cMemChunk& buf = *(cMemChunk*)pValue;
	const tVOID* ptr = buf.ptr();
	*out_size = buf.allocated();
	*werr = ::RegQueryValueEx( HK(key), szValueName, 0, reg_type, (LPBYTE)ptr, (LPDWORD)out_size );
	if ( (ptr && (*werr == ERROR_MORE_DATA)) || (*out_size > buf.allocated()) ) {
		ptr = buf.reserve( *out_size, false );
		if ( !ptr )
			return errNOT_ENOUGH_MEMORY;
		*werr = ::RegQueryValueEx( HK(key), szValueName, 0, reg_type, (LPBYTE)ptr, (LPDWORD)out_size );
	}
	if ( *werr == ERROR_SUCCESS ) {
		buf.used( *out_size );
		return errOK;
	}
	return _ConvertRegErr( *werr );
}



// ---
extern "C" tERROR pr_call Registry_SetBufVal( tDWORD* werr, tRegKey key, const tCHAR* szValueName, tPTR pValue ) {
	if ( pValue )
		*werr = ::RegSetValueEx( HK(key), szValueName, 0, REG_BINARY, (tBYTE*)((cMemChunk*)pValue)->ptr(), ((cMemChunk*)pValue)->used() );
	else
		*werr = ::RegSetValueEx( HK(key), szValueName, 0, REG_BINARY, (LPBYTE)0, 0 );

	if ( *werr != ERROR_SUCCESS )
		return _ConvertRegErr( *werr );
	return errOK;
}




