#include"custview.h"
#include <pr_err.h>


// ---
extern tName errs[];
extern tDWORD err_count;


// ---
HRESULT WINAPI err_format_val( 
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

		else if ( dwValue == errOK )
			msg = "errOK";

		else {
			tErrFacility facility = PR_ERR_FAC(dwValue);
			tUINT  id       = PR_ERR_ID(dwValue);
			tUINT  code     = PR_ERR_CODE(dwValue);
			char*  scope;
			char*  name;
			char*  owner = 0;
			char   tmp_err[MAX_PATH];
			BOOL   is_prague = FALSE;

			switch( facility ) {
				case fac_PREDECLARED:
					name = find_name( errs, err_count, dwValue, 0 );
					if ( !name || !*name )
						wsprintf( name=tmp_err, "0x%08x", dwValue );
					else if ( id == 0 )
						owner = "sys";
					else
						owner = find_name( iids, iid_count, id, "unk iface" );
					is_prague = TRUE;
					break;
				case fac_IMPLEMENTATION_DECLARED:
					name = find_name( errs, err_count, dwValue, 0 );
					if ( !name || !*name )
						wsprintf( name=tmp_err, "0x%08x", dwValue );
					else
						owner = find_name( pids, pid_count, id, "unk plugin" );
					is_prague = TRUE;
					break;
				case fac_NATIVE_OS:
					owner  = "win";
					FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 0, code, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), name=tmp_err, sizeof(tmp_err), 0 );
					break;
				default:
					owner = "unk";
					name = find_name( errs, err_count, dwValue, 0 );
					if ( !name || !*name )
						wsprintf( name=tmp_err, "0x%08x", dwValue );
					else
						is_prague = TRUE;
					break;
			}
			if ( is_prague && PR_SUCC(dwValue) )
				scope = "(warn)";
			else
				scope = "";
			wsprintf( msg=local_buff, "%s%s:%s", owner, scope, name );
		}
		lstrcpyn( pResult, msg, max );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult,"cannot resolve error, exception" );
	}

	return S_OK;
}




// ---
HRESULT WINAPI err_format( 
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
			return err_format_val( dwValue, pHelper, nBase, bIgnore, pResult, max, dwReserved );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult,"cannot resolve error, exception" );
	}
	
	return S_OK;
}




// ---
tName errs[] = {
	
#undef DEF_IDENTIFIER
#define DEF_IDENTIFIER(name, val)	{val, #name},
#include "../../include/pr_d_err.h"
};

tDWORD err_count = countof(errs);
