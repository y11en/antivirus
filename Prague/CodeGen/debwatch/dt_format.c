#include"custview.h"
#include <pr_types.h>
#include <time.h>

tERROR IMPEX_FUNC(DTGet)( const tDT *dt, tDWORD *year, tDWORD *month, tDWORD *day, tDWORD *hour, tDWORD *minute, tDWORD *second, tDWORD *ns );


// ---
HRESULT WINAPI dt_format( 
	DWORD dwAddr,      // low 32-bits of address
	DEBUGHELPER *pHelper, // callback pointer to access helper functions
	int nBase,            // decimal or hex
	BOOL bIgnore,         // not used
	char *pResult,        // where the result needs to go
	size_t max,           // how large the above buffer is
	DWORD dwReserved      // always pass zero
) {
	__try {
		tDATETIME dtValue;
		DWORD dwErr;
		HRESULT hr = pHelper->ReadDebuggeeMemory( pHelper, dwAddr, sizeof(dtValue), &dtValue, &dwErr );
		
		if ( FAILED(hr) )
			lstrcpyn( pResult, "cannot get value", max );
		else if ( (*(tLONGLONG*)&dtValue) == 0xcccccccccccccccc )
			lstrcpyn( pResult, "not initialized", max );
		else {
			tDWORD year, month, day, hour, minute, sec, nsec;
			DTGet( (tDT*)&dtValue, &year, &month, &day, &hour, &minute, &sec, &nsec );
			wsprintf( pResult, "%02u.%02u.%04u %02u:%02u:%02u.%03u", day, month, year, hour, minute, sec, nsec/1000000 );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult, "cannot resolve tDATETIME, exception" );
	}
	
	return S_OK;
}



// ---
static void resolve_local_time( char* pResult, size_t max, time_t time ) {
	if ( -1 == time )
		lstrcpyn( pResult, "{(-1), time just initialized}", max );
	else {
		struct tm* p = localtime( &time );
		wsprintf( pResult, "{%02u.%02u.%04u %02u:%02u:%02u}", p->tm_mday, 1+p->tm_mon, 1900+p->tm_year, p->tm_hour, p->tm_min, p->tm_sec );
	}
}



// ---
static void resolve_gmt_time( char* pResult, size_t max, time_t time ) {
	if ( -1 == time )
		lstrcpyn( pResult, "{(-1), time just initialized}", max );
	else {
		struct tm* p = gmtime( &time );
		wsprintf( pResult, "{%02u.%02u.%04u %02u:%02u:%02u}", p->tm_mday, 1+p->tm_mon, 1900+p->tm_year, p->tm_hour, p->tm_min, p->tm_sec );
	}
}



// ---
HRESULT WINAPI local_time_format_val( 
	DWORD dwVal,      // low 32-bits of address
	DEBUGHELPER *pHelper, // callback pointer to access helper functions
	int nBase,            // decimal or hex
	BOOL bIgnore,         // not used
	char *pResult,        // where the result needs to go
	size_t max,           // how large the above buffer is
	DWORD dwReserved      // always pass zero
) {
	__try {
		resolve_local_time( pResult, max, dwVal );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult, "cannot resolve time_t, exception" );
	}
	
	return S_OK;
}


// ---
HRESULT WINAPI gmt_time_format_val( 
	DWORD dwVal,      // low 32-bits of address
	DEBUGHELPER *pHelper, // callback pointer to access helper functions
	int nBase,            // decimal or hex
	BOOL bIgnore,         // not used
	char *pResult,        // where the result needs to go
	size_t max,           // how large the above buffer is
	DWORD dwReserved      // always pass zero
) {
	__try {
		resolve_gmt_time( pResult, max, dwVal );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult, "cannot resolve time_t, exception" );
	}
	
	return S_OK;
}


// ---
HRESULT WINAPI local_time_format( 
	DWORD dwAddr,      // low 32-bits of address
	DEBUGHELPER *pHelper, // callback pointer to access helper functions
	int nBase,            // decimal or hex
	BOOL bIgnore,         // not used
	char *pResult,        // where the result needs to go
	size_t max,           // how large the above buffer is
	DWORD dwReserved      // always pass zero
) {
	__try {
		time_t dwVal;
		DWORD dwErr;
		HRESULT hr = pHelper->ReadDebuggeeMemory( pHelper, dwAddr, sizeof(dwVal), &dwVal, &dwErr );
		
		if ( FAILED(hr) )
			lstrcpyn( pResult, "cannot get value", max );
		else
			resolve_local_time( pResult, max, dwVal );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult, "cannot resolve time_t, exception" );
	}
	
	return S_OK;
}




// ---
HRESULT WINAPI gmt_time_format( 
	DWORD dwAddr,      // low 32-bits of address
	DEBUGHELPER *pHelper, // callback pointer to access helper functions
	int nBase,            // decimal or hex
	BOOL bIgnore,         // not used
	char *pResult,        // where the result needs to go
	size_t max,           // how large the above buffer is
	DWORD dwReserved      // always pass zero
) {
	__try {
		time_t dwVal;
		DWORD dwErr;
		HRESULT hr = pHelper->ReadDebuggeeMemory( pHelper, dwAddr, sizeof(dwVal), &dwVal, &dwErr );
		
		if ( FAILED(hr) )
			lstrcpyn( pResult, "cannot get value", max );
		else
			resolve_gmt_time( pResult, max, dwVal );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult, "cannot resolve time_t, exception" );
	}
	
	return S_OK;
}


