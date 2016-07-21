#include "custview.h"
#include <iface/i_string.h>



// ---
extern "C" HRESULT WINAPI str_range_format_val( 
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
		
		else if ( dwValue == cSTRING_EMPTY )
			msg = "offs:0, len:0 (cSTRING_EMPTY)";
		
		else if ( dwValue == cSTRING_WHOLE )
			msg = "offs:0, len:up to the end (cSTRING_WHOLE)";
		
		else {
			DWORD val;
			DWORD dwErr;
			pHelper->ReadDebuggeeMemory( pHelper, (DWORD)dwValue, sizeof(val), &val,  &dwErr );

			DWORD offs = STR_RANGE_POS( dwValue );
			DWORD length = STR_RANGE_LEN( dwValue );
			if ( (offs >= 0x1000) && (length >= 0x1000) )
				wsprintf( msg=local_buff, "bad range? (offs:%u, len:%u)", offs, length );
			else if ( length == cSTRING_WHOLE_LENGTH )
				wsprintf( msg=local_buff, "offs:%u, len:up to the end", offs );
			else if ( offs == cSTRING_WHOLE_LENGTH )
				wsprintf( msg=local_buff, "offs:%u, len:%u - special case: right range ", offs, length );
			else
				wsprintf( msg=local_buff, "offs:%u, len:%u", offs, length );
		}
		
		lstrcpyn( pResult, msg, max );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult, "cannot resolve iid, exception" );
	}
	
	return S_OK;
}



// ---
extern "C" HRESULT WINAPI str_range_format( 
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
			lstrcpyn( pResult,"cannot get value", max );
		else
			return str_range_format_val( dwValue, pHelper, nBase, bIgnore, pResult, max, dwReserved );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult,"cannot resolve property id, exception" );
	}
	
	return S_OK;
}




