#include"custview.h"
#include <pr_types.h>


/*
#define cCP_NULL                       ((tCODEPAGE)((tDWORD)0))
#define cCP_OEM                        ((tCODEPAGE)(((tDWORD)-1)-10))
#define cCP_ANSI                       ((tCODEPAGE)(((tDWORD)-1)-11))
#define cCP_IBM                        ((tCODEPAGE)(((tDWORD)-1)-12))
#define cCP_UTF8                       ((tCODEPAGE)(((tDWORD)-1)-13))
#define cCP_UNICODE                    ((tCODEPAGE)((tDWORD)1200))
#define cCP_UNICODE_BIGENDIAN          ((tCODEPAGE)((tDWORD)1201))
*/


// ---
tName cps[] = {
	{ cCP_NULL,              "cCP_NULL"                              },
	{ cCP_OEM,               "cCP_OEM"                               },
	{ cCP_ANSI,              "cCP_ANSI"                              },
	{ cCP_IBM,               "cCP_IBM (not supported)"               },
	{ cCP_UTF8,              "cCP_UTF8 (not supported)"              },
	{ cCP_UNICODE,           "cCP_UNICODE"                           },
	{ cCP_UNICODE_BIGENDIAN, "cCP_UNICODE_BIGENDIAN (not supported)" },
};



// ---
HRESULT WINAPI cp_format_val( 
	DWORD dwValue,      // low 32-bits of address
	DEBUGHELPER *pHelper, // callback pointer to access helper functions
	int nBase,            // decimal or hex
	BOOL bIgnore,         // not used
	char *pResult,        // where the result needs to go
	size_t max,           // how large the above buffer is
	DWORD dwReserved      // always pass zero
) {

	__try {
		char* msg;
		if ( dwValue == 0xccccccccl )
			msg ="not initialized";
		else {
			tName* name = find( cps, countof(cps), dwValue );
			if ( !name )
				wsprintf( msg=local_buff,"%u,unknown codepage", dwValue );
			else if ( (name->id == cCP_UNICODE) || (name->id == cCP_UNICODE_BIGENDIAN) )
				wsprintf( msg=local_buff,"%u,%s", dwValue, name->name );
			else
				wsprintf( msg=local_buff,"0x%x,%s", dwValue, name->name );
		}
		lstrcpyn( pResult, msg, max );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult, "cannot resolve cp, exception" );
	}

	return S_OK;
}



// ---
HRESULT WINAPI cp_format( 
	DWORD dwAddr,      // low 32-bits of address
	DEBUGHELPER *pHelper, // callback pointer to access helper functions
	int nBase,            // decimal or hex
	BOOL bIgnore,         // not used
	char *pResult,        // where the result needs to go
	size_t max,           // how large the above buffer is
	DWORD dwReserved      // always pass zero
) {
	__try {
		DWORD dwValue;
		DWORD dwErr;
		HRESULT hr = pHelper->ReadDebuggeeMemory( pHelper, dwAddr, sizeof(dwValue), &dwValue, &dwErr );
		
		if ( FAILED(hr) )
			lstrcpyn( pResult, "cannot get value", max );
		else
			return cp_format_val( dwValue, pHelper, nBase, bIgnore, pResult, max, dwReserved );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult, "cannot resolve cp, exception" );
	}
	
	return S_OK;
}


