#include"custview.h"



// ---
HRESULT WINAPI pid_format_val( 
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
		else
			wsprintf( msg=local_buff,"%u,%s", dwValue, find_name(pids,pid_count,dwValue,"unknown") );
		lstrcpyn( pResult, msg, max );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult, "cannot resolve pid, exception" );
	}

	return S_OK;
}



// ---
HRESULT WINAPI pid_format( 
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
			return pid_format_val( dwValue, pHelper, nBase, bIgnore, pResult, max, dwReserved );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult, "cannot resolve pid, exception" );
	}
	
	return S_OK;
}




// ---
#define DEF_IDENTIFIER(name, val)	{val, #name},
tName pids[] = {
	#include "../../include/pr_d_pid.h"
};

// ---
tUINT pid_count = countof(pids);


