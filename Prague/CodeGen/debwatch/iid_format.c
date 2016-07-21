#include "custview.h"



// ---
HRESULT WINAPI iid_format_val( 
	DWORD dwValue,        // low 32-bits of address
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
			msg = "not initialized";

		else if ( dwValue == IID_ANY )
			msg = "0,IID_ANY";

		else if ( dwValue == IID_NONE )
			msg = "0xffffffff,IID_NONE";

		else
			wsprintf( msg=local_buff, "%u,%s", dwValue, find_name(iids,iid_count,dwValue,"unk") );

		lstrcpyn( pResult, msg, max );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult, "cannot resolve iid, exception" );
	}

	return S_OK;
}



// ---
HRESULT WINAPI iid_format( 
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
			return iid_format_val( dwValue, pHelper, nBase, bIgnore, pResult, max, dwReserved );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult, "cannot resolve iid, exception" );
	}

	return S_OK;
}


