#define __kernel_h
#include "custview.h"
#include "../../string/pstring.h"

// ---
//typedef struct tag_hi_String {
//	void*       vtbl; // pointer to the "String" virtual table
//	void*       sys;  // pointer to the "SYSTEM" virtual table
//	StringData* data; // pointer to the "String" data structure
//} *hi_String;

typedef struct tag_hi_String {
	const iStringVtbl* vtbl; // pointer to the "String" virtual table
	const iSYSTEMVTBL* sys;  // pointer to the "SYSTEM" virtual table
	StringData*        data; // pointer to the "String" data structure
} *hi_String;


// ---
HRESULT WINAPI phString( DWORD address, DEBUGHELPER* pHelper, int base, BOOL ignore, char* pOutputStr, size_t dwMaxSize, DWORD reserved ) {

	__try {
		struct tag_hi_String handle;
		StringData    data;
		DWORD         dwErr;
		DWORD         len;
		HRESULT       hr;
		char*         ptr;
		char          tmp_buff[20];

		memset( pOutputStr, 0, dwMaxSize );

		if ( ((DWORD)address) == 0 ) {
			lstrcpyn( pOutputStr, "null pointer", dwMaxSize );
			return S_OK;
		}
		if ( ((DWORD)address) == 0xccccccccl ) {
			lstrcpyn( pOutputStr, "not initialized", dwMaxSize );
			return S_OK;
		}
		
		hr = pHelper->ReadDebuggeeMemory( pHelper, address, sizeof(struct tag_hi_String), &handle, &dwErr );
		if ( FAILED(hr) ) {
			lstrcpyn( pOutputStr, "??? cannot read debuggee memory", dwMaxSize );
			return S_OK;
		}

		pHelper->ReadDebuggeeMemory( pHelper, (DWORD)handle.data, sizeof(StringData), &data,  &dwErr );
		if ( FAILED(hr) ) {
			lstrcpyn( pOutputStr, "??? cannot read debuggee memory", dwMaxSize );
			return S_OK;
		}
		
		if ( !data.len ) {
			int offs;
			switch( data.cp ) {
				case cCP_OEM     : offs = 2; lstrcpy( pOutputStr, "O:" );  break;
				case cCP_ANSI    : offs = 2; lstrcpy( pOutputStr, "A:" ); break;
				case cCP_UNICODE : offs = 2; lstrcpy( pOutputStr, "U:" );  break;
				default          : offs = wsprintf( pOutputStr, "UNK:0x%x:", data.cp ); break;
			}
			lstrcpyn( pOutputStr+offs, "empty string", dwMaxSize-offs );
			return S_OK;
		}

		memset( src, 0, sizeof(src) );
		if ( data.len > sizeof(src)-1 )
			len = sizeof(src)-1;
		else
			len = data.len;

		pHelper->ReadDebuggeeMemory( pHelper, (DWORD)data.buff, len, src, &dwErr );
		if ( FAILED(hr) ) {
			lstrcpyn( pOutputStr, "??? cannot read debuggee memory", dwMaxSize );
			return S_OK;
		}
		
		len = 2;
    memset( local_buff, 0, sizeof(local_buff) );
    switch( data.cp ) {
			case cCP_OEM  :
				ptr = "O:";
				OemToCharBuff( (char*)src, local_buff, sizeof(local_buff) );
				break;
			case cCP_ANSI :
				ptr = "A:";
				lstrcpyn( local_buff, (char*)src, sizeof(local_buff) );
				break;
			case cCP_UNICODE :
				ptr = "U:";
				WideCharToMultiByte( CP_ACP, WCFL, (tWCHAR*)src, data.len/sizeof(tWCHAR), local_buff, sizeof(local_buff), 0, 0 );
				break;
			default:
				len = wsprintf( ptr=tmp_buff, "UNK:0x%x", data.cp );
				break;
		}
		memcpy( pOutputStr, ptr, len );
		pOutputStr += len;
		dwMaxSize -= len;

		len = lstrlen( local_buff );
		if ( len <= dwMaxSize )
			ptr = local_buff;
		else {
			memcpy( pOutputStr, "...", 3 );
			pOutputStr += 3;
			dwMaxSize -= 3;
			ptr = local_buff + len - dwMaxSize;
		}
		lstrcpyn( pOutputStr, ptr, dwMaxSize );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpyn( pOutputStr, "cannot parse address", dwMaxSize );
	}
	return S_OK;
}



