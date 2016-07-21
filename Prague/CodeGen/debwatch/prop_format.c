#include"custview.h"
#include <pr_prop.h>

// ---
extern tName props[];
extern tDWORD prop_count;
	
extern tName types[];
extern tDWORD type_count;

extern tName ranges[];
extern tDWORD range_count;


// ---
HRESULT WINAPI prop_format_val( 
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
			msg ="not initialized";

		else { 
			char* fmt;
			char* prop_name = find_name( props, prop_count, dwValue, 0 );
			char* range_name = find_name( ranges, range_count, dwValue & pRANGE_MASK, "unk range" );
			char* type_name = find_name( types, type_count, dwValue & pTYPE_MASK, "unk type" );
			tDWORD id = dwValue & pNUMBER_MASK;
			if ( !prop_name )
				wsprintf( prop_name=src, "0x%08x", dwValue );
			if ( ((dwValue & pRANGE_MASK) == pRANGE_CUSTOM) && (dwValue & pCUSTOM_HERITABLE) )
				fmt = "%s,%s(heritable),%s,%x";
			else
				fmt = "%s,%s,%s,%x";
			wsprintf( msg=local_buff, fmt, prop_name, range_name, type_name, id );
		}
		lstrcpyn( pResult, msg, max );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult,"cannot resolve property id, exception" );
	}
	
	return S_OK;
}



// ---
HRESULT WINAPI prop_format( 
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
			return prop_format_val( dwValue, pHelper, nBase, bIgnore, pResult, max, dwReserved );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult,"cannot resolve property id, exception" );
	}
	
	return S_OK;
}




// ---
tName props[] = {
  #define DEF_IDENTIFIER(name, val)	{val, #name},
  #include "../../include/pr_d_pg.h"
  #undef DEF_IDENTIFIER
};
tDWORD prop_count = countof(props);


// ---
tName types[] = {
	{ pTYPE_NOTHING      ,"nothing" },
	{ pTYPE_BYTE         ,"byte" },
	{ pTYPE_WORD         ,"word" },
	{ pTYPE_DWORD        ,"dword" },
	{ pTYPE_QWORD        ,"qword" },
	{ pTYPE_STRING       ,"string" },
	{ pTYPE_CODEPAGE     ,"codepage" },
	{ pTYPE_LCID         ,"lcid" },
	{ pTYPE_DATETIME     ,"datetime" },
	{ pTYPE_BINARY       ,"binary" },
	{ pTYPE_INT          ,"int" },
	{ pTYPE_IFACEPTR     ,"ifaceptr" },
	{ pTYPE_PTR          ,"ptr" },
	{ pTYPE_OBJECT       ,"object" },
	{ pTYPE_BOOL         ,"bool" },
	{ pTYPE_CHAR         ,"char" },
	{ pTYPE_WCHAR        ,"wchar" },
	{ pTYPE_WSTRING      ,"wstring" },
	{ pTYPE_ERROR        ,"error" },
	{ pTYPE_UINT         ,"uint" },
	{ pTYPE_SBYTE        ,"sbyte" },
	{ pTYPE_SHORT        ,"short" },
	{ pTYPE_LONG         ,"long" },
	{ pTYPE_LONGLONG     ,"longlong" },
	{ pTYPE_IID          ,"iid" },
	{ pTYPE_PID          ,"pid" },
	{ pTYPE_ORIG_ID      ,"orig_id" },
	{ pTYPE_OS_ID        ,"os_id" },
	{ pTYPE_VID          ,"vid" },
	{ pTYPE_PROPID       ,"propid" },
	{ pTYPE_VERSION      ,"version" },
	{ pTYPE_DATA         ,"data" },
	{ pTYPE_FUNC_PTR     ,"func_ptr" },
	{ pTYPE_EXPORT       ,"export" },
	{ pTYPE_IMPORT       ,"import" },
	{ pTYPE_TRACE_LEVEL  ,"trace_level" },
};
tDWORD type_count = countof(types);



// ---
tName ranges[] = {
	{ pRANGE_SYSTEM,  "system" },
	{ pRANGE_GLOBAL,  "global" },
	{ pRANGE_LOCAL,   "local" },
	{ pRANGE_PRIVATE, "private" },
	{ pRANGE_CUSTOM,  "custom" },
};
tDWORD range_count = countof(ranges);
	

