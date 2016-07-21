#include "custview.h"


// ---
tName hrs[] = {
	{ 0x0,        "S_OK" },
	{ 0x1,        "S_FALSE" },
	{ 0x8000FFFF, "E_UNEXPECTED" },
	{ 0x80004001, "E_NOTIMPL" },
	{ 0x8007000E, "E_OUTOFMEMORY" },
	{ 0x80070057, "E_INVALIDARG" },
	{ 0x80004002, "E_NOINTERFACE" },
	{ 0x80004003, "E_POINTER" },
	{ 0x80070006, "E_HANDLE" },
	{ 0x80004004, "E_ABORT" },
	{ 0x80004005, "E_FAIL" },
	{ 0x80070005, "E_ACCESSDENIED" },
	{ 0x80004001, "E_NOTIMPL" },
	{ 0x8007000E, "E_OUTOFMEMORY" },
	{ 0x80070057, "E_INVALIDARG" },
	{ 0x80004002, "E_NOINTERFACE" },
	{ 0x80004003, "E_POINTER" },
	{ 0x80070006, "E_HANDLE" },
	{ 0x80004004, "E_ABORT" },
	{ 0x80004005, "E_FAIL" },
	{ 0x80070005, "E_ACCESSDENIED" },
	{ 0x8000000A, "E_PENDING" },
	{ 0x80040E00, "DB_E_BADACCESSORHANDLE" },
	{ 0x80040E01, "DB_E_ROWLIMITEXCEEDED" },
	{ 0x80040E02, "DB_E_READONLYACCESSOR" },
	{ 0x80040E03, "DB_E_SCHEMAVIOLATION" },
	{ 0x80040E04, "DB_E_BADROWHANDLE" },
	{ 0x80040E05, "DB_E_OBJECTOPEN" },
	{ 0x80040E06, "DB_E_BADCHAPTER" },
	{ 0x80040E07, "DB_E_CANTCONVERTVALUE" },
	{ 0x80040E08, "DB_E_BADBINDINFO" },
	{ 0x80040E09, "DB_SEC_E_PERMISSIONDENIED" },
	{ 0x80040E0A, "DB_E_NOTAREFERENCECOLUMN" },
	{ 0x80040E0B, "DB_E_LIMITREJECTED" },
	{ 0x80040E0C, "DB_E_NOCOMMAND" },
	{ 0x80040E0D, "DB_E_COSTLIMIT" },
	{ 0x80040E0E, "DB_E_BADBOOKMARK" },
	{ 0x80040E0F, "DB_E_BADLOCKMODE" },
	{ 0x80040E10, "DB_E_PARAMNOTOPTIONAL" },
	{ 0x80040E11, "DB_E_BADCOLUMNID" },
	{ 0x80040E12, "DB_E_BADRATIO" },
	{ 0x80040E13, "DB_E_BADVALUES" },
	{ 0x80040E14, "DB_E_ERRORSINCOMMAND" },
	{ 0x80040E15, "DB_E_CANTCANCEL" },
	{ 0x80040E16, "DB_E_DIALECTNOTSUPPORTED" },
	{ 0x80040E17, "DB_E_DUPLICATEDATASOURCE" },
	{ 0x80040E18, "DB_E_CANNOTRESTART" },
	{ 0x80040E19, "DB_E_NOTFOUND" },
	{ 0x80040E1A, "DB_E_CANNOTFREE" },
	{ 0x80040E1B, "DB_E_NEWLYINSERTED" },
	{ 0x80040E1C, "DB_E_GOALREJECTED" },
	{ 0x80040E1D, "DB_E_UNSUPPORTEDCONVERSION" },
	{ 0x80040E1E, "DB_E_BADSTARTPOSITION" },
	{ 0x80040E1F, "DB_E_NOQUERY" },
	{ 0x80040E20, "DB_E_NOTREENTRANT" },
	{ 0x80040E21, "DB_E_ERRORSOCCURRED" },
	{ 0x80040E22, "DB_E_NOAGGREGATION" },
	{ 0x80040E23, "DB_E_DELETEDROW" },
	{ 0x80040E24, "DB_E_CANTFETCHBACKWARDS" },
	{ 0x80040E25, "DB_E_ROWSNOTRELEASED" },
	{ 0x80040E26, "DB_E_BADSTORAGEFLAG" },
	{ 0x80040E27, "DB_E_BADCOMPAREOP" },
	{ 0x80040E28, "DB_E_BADSTATUSVALUE" },
	{ 0x80040E29, "DB_E_CANTSCROLLBACKWARDS" },
	{ 0x80040E2A, "DB_E_BADREGIONHANDLE" },
	{ 0x80040E2B, "DB_E_NONCONTIGUOUSRANGE" },
	{ 0x80040E2C, "DB_E_INVALIDTRANSITION" },
	{ 0x80040E2D, "DB_E_NOTASUBREGION" },
	{ 0x80040E2E, "DB_E_MULTIPLESTATEMENTS" },
	{ 0x80040E2F, "DB_E_INTEGRITYVIOLATION" },
	{ 0x80040E30, "DB_E_BADTYPENAME" },
	{ 0x80040E31, "DB_E_ABORTLIMITREACHED" },
	{ 0x80040E32, "DB_E_ROWSETINCOMMAND" },
	{ 0x80040E33, "DB_E_CANTTRANSLATE" },
	{ 0x80040E34, "DB_E_DUPLICATEINDEXID" },
	{ 0x80040E35, "DB_E_NOINDEX" },
	{ 0x80040E36, "DB_E_INDEXINUSE" },
	{ 0x80040E37, "DB_E_NOTABLE" },
	{ 0x80040E38, "DB_E_CONCURRENCYVIOLATION" },
	{ 0x80040E39, "DB_E_BADCOPY" },
	{ 0x80040E3A, "DB_E_BADPRECISION" },
	{ 0x80040E3B, "DB_E_BADSCALE" },
	{ 0x80040E3C, "DB_E_BADID" },
	{ 0x80040E3D, "DB_E_BADTYPE" },
	{ 0x80040E3E, "DB_E_DUPLICATECOLUMNID" },
	{ 0x80040E3F, "DB_E_DUPLICATETABLEID" },
	{ 0x80040E40, "DB_E_TABLEINUSE" },
	{ 0x80040E41, "DB_E_NOLOCALE" },
	{ 0x80040E42, "DB_E_BADRECORDNUM" },
	{ 0x80040E43, "DB_E_BOOKMARKSKIPPED" },
	{ 0x80040E44, "DB_E_BADPROPERTYVALUE" },
	{ 0x80040E45, "DB_E_INVALID" },
	{ 0x80040E46, "DB_E_BADACCESSORFLAGS" },
	{ 0x80040E47, "DB_E_BADSTORAGEFLAGS" },
	{ 0x80040E48, "DB_E_BYREFACCESSORNOTSUPPORTED" },
	{ 0x80040E49, "DB_E_NULLACCESSORNOTSUPPORTED" },
	{ 0x80040E4A, "DB_E_NOTPREPARED" },
	{ 0x80040E4B, "DB_E_BADACCESSORTYPE" },
	{ 0x80040E4C, "DB_E_WRITEONLYACCESSOR" },
	{ 0x80040E4D, "DB_SEC_E_AUTH_FAILED" },
	{ 0x80040E4E, "DB_E_CANCELED" },
	{ 0x80040E4F, "DB_E_CHAPTERNOTRELEASED" },
	{ 0x80040E50, "DB_E_BADSOURCEHANDLE" },
	{ 0x80040E51, "DB_E_PARAMUNAVAILABLE" },
	{ 0x80040E52, "DB_E_ALREADYINITIALIZED" },
	{ 0x80040E53, "DB_E_NOTSUPPORTED" },
	{ 0x80040E54, "DB_E_MAXPENDCHANGESEXCEEDED" },
	{ 0x80040E55, "DB_E_BADORDINAL" },
	{ 0x80040E56, "DB_E_PENDINGCHANGES" },
	{ 0x80040E57, "DB_E_DATAOVERFLOW" },
	{ 0x80040E58, "DB_E_BADHRESULT" },
	{ 0x80040E59, "DB_E_BADLOOKUPID" },
	{ 0x80040E5A, "DB_E_BADDYNAMICERRORID" },
	{ 0x80040E5B, "DB_E_PENDINGINSERT" },
	{ 0x80040E5C, "DB_E_BADCONVERTFLAG" },
	{ 0x80040E5D, "DB_E_BADPARAMETERNAME" },
	{ 0x80040E5E, "DB_E_MULTIPLESTORAGE" },
	{ 0x80040E5F, "DB_E_CANTFILTER" },
	{ 0x80040E60, "DB_E_CANTORDER" },
	{ 0x40EC0,    "DB_S_ROWLIMITEXCEEDED" },
	{ 0x40EC1,    "DB_S_COLUMNTYPEMISMATCH" },
	{ 0x40EC2,    "DB_S_TYPEINFOOVERRIDDEN" },
	{ 0x40EC3,    "DB_S_BOOKMARKSKIPPED" },
	{ 0x40EC5,    "DB_S_NONEXTROWSET" },
	{ 0x40EC6,    "DB_S_ENDOFROWSET" },
	{ 0x40EC7,    "DB_S_COMMANDREEXECUTED" },
	{ 0x40EC8,    "DB_S_BUFFERFULL" },
	{ 0x40EC9,    "DB_S_NORESULT" },
	{ 0x40ECA,    "DB_S_CANTRELEASE" },
	{ 0x40ECB,    "DB_S_GOALCHANGED" },
	{ 0x40ECC,    "DB_S_UNWANTEDOPERATION" },
	{ 0x40ECD,    "DB_S_DIALECTIGNORED" },
	{ 0x40ECE,    "DB_S_UNWANTEDPHASE" },
	{ 0x40ECF,    "DB_S_UNWANTEDREASON" },
	{ 0x40ED0,    "DB_S_ASYNCHRONOUS" },
	{ 0x40ED1,    "DB_S_COLUMNSCHANGED" },
	{ 0x40ED2,    "DB_S_ERRORSRETURNED" },
	{ 0x40ED3,    "DB_S_BADROWHANDLE" },
	{ 0x40ED4,    "DB_S_DELETEDROW" },
	{ 0x40ED5,    "DB_S_TOOMANYCHANGES" },
	{ 0x40ED6,    "DB_S_STOPLIMITREACHED" },
	{ 0x40ED8,    "DB_S_LOCKUPGRADED" },
	{ 0x40ED9,    "DB_S_PROPERTIESCHANGED" },
	{ 0x40EDA,    "DB_S_ERRORSOCCURRED" },
	{ 0x40EDB,    "DB_S_PARAMUNAVAILABLE" },
	{ 0x40EDC,    "DB_S_MULTIPLECHANGES" },
};




// ---
HRESULT WINAPI hr_format_val( 
	DWORD dwValue,      // low 32-bits of address
	DEBUGHELPER *pHelper, // callback pointer to access helper functions
	int nBase,            // decimal or hex
	BOOL bIgnore,         // not used
	char *pResult,        // where the result needs to go
	size_t max,           // how large the above buffer is
	DWORD dwReserved      // always pass zero
) {

	__try {
		char* name;
		char* msg;
		
		if ( dwValue == 0xccccccccl )
			msg = "not initialized";

		else if ( 0 != (name=find_name(hrs,countof(hrs),dwValue,0)) )
			wsprintf( msg=local_buff, "0x%08x,%s", dwValue, name );

		else if ( FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,dwValue,0,src,sizeof(src),0))
			wsprintf( msg=local_buff, "0x%08x,%s", dwValue, src );

		else
			wsprintf( msg=local_buff, "0x%08x(%u) - cannot parse HRESULT", dwValue, dwValue );

		lstrcpyn( pResult, msg, max );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult, "cannot resolve hr, exception" );
	}

	return S_OK;
}



// ---
HRESULT WINAPI hr_format( 
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
			return hr_format_val( dwValue, pHelper, nBase, bIgnore, pResult, max, dwReserved );
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		lstrcpy( pResult, "cannot resolve hr, exception" );
	}
	
	return S_OK;
}


