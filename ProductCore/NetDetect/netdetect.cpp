// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  11 January 2006,  16:22 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- PPP
// Purpose     -- Not defined
// Author      -- Pavlushchik
// File Name   -- netdetect.cpp
// -------------------------------------------
// AVP Prague stamp end



#define PR_IMPEX_DEF

// AVP Prague stamp begin( Interface version,  )
#define NetDetect_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/pr_time.h>
#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_root.h>

#include <ProductCore/plugin/p_netdetect.h>
// AVP Prague stamp end



#include <windows.h>
#include <Iprtrmib.h>

//extern HANDLE g_hNotifyAddrChangeEvent;

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable NetDetect : public cNetDetect {
private:
// Internal function declarations
	tERROR pr_call ObjectInit();
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();

// Property function declarations

public:
// External function declarations
	tERROR pr_call Detect();
	tERROR pr_call InternetGetConnectedState( tDWORD* p_pdwFlags );
	tERROR pr_call GetLastIpAddrChange( tDWORD* p_pdwLastChangeTime );

// Data declaration
// AVP Prague stamp end

	hCRITICAL_SECTION m_hCS;

    typedef BOOL (WINAPI *typeInternetGetConnectedState)(LPDWORD lpdwFlags, DWORD dwReserved);
    typedef int (PASCAL FAR *typeWSAStartup)(IN WORD wVersionRequired, OUT LPWSADATA lpWSAData);
    typedef int (PASCAL FAR *typeWSACleanup)(void);
    typedef struct hostent FAR * ( PASCAL FAR * typeGethostbyname)(IN const char FAR * name);
	typedef DWORD (PASCAL FAR * typeNotifyAddrChange)( PHANDLE Handle, LPOVERLAPPED overlapped );
	typedef DWORD (PASCAL FAR * typeGetIpAddrTable)(PMIB_IPADDRTABLE pIpAddrTable, PULONG pdwSize, BOOL bOrder);
    
    typeInternetGetConnectedState iInternetGetConnectedState;
    typeWSAStartup WSAStartup;
    typeWSACleanup WSACleanup;
    typeGethostbyname gethostbyname;
	typeNotifyAddrChange NotifyAddrChange;
	typeGetIpAddrTable GetIpAddrTable;
    
    WSADATA wsaData;
    HMODULE hWinInetDll, hWs2Dll, hIpHlpApi;

	time_t m_nLastNotifyAddrChangeTime;
	
	PMIB_IPADDRTABLE m_pIpTable;
	DWORD m_dwIpTableSize;

	tERROR NetDetect::DumpTable(PMIB_IPADDRTABLE pIpTable, char* szName, time_t tTime);
	tERROR NetDetect::FixTable(PMIB_IPADDRTABLE pIpTable);
	
	tDWORD m_dwWindowsMajorVersion;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(NetDetect)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "NetDetect". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//    Kernel notifies an object about successful creating of object
// Behaviour comment
//    Initializes internal object data
// Result comment
//    Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR NetDetect::ObjectInit()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "NetDetect::ObjectInit method" );

	m_dwWindowsMajorVersion = (tDWORD)(LOBYTE(LOWORD(GetVersion())));

    hWs2Dll = LoadLibrary ( "ws2_32.dll" );
	if (!hWs2Dll)
		return errOBJECT_CANNOT_BE_INITIALIZED;

    WSAStartup = (typeWSAStartup)GetProcAddress(hWs2Dll, "WSAStartup");
    WSACleanup = (typeWSACleanup)GetProcAddress(hWs2Dll, "WSACleanup");
    gethostbyname = (typeGethostbyname)GetProcAddress(hWs2Dll, "gethostbyname");

    if ( ! WSAStartup || ! WSACleanup || ! gethostbyname )
		return errOBJECT_CANNOT_BE_INITIALIZED;

    if ( WSAStartup ( MAKEWORD(1,1), & wsaData ) != 0 )
		return errOBJECT_CANNOT_BE_INITIALIZED;

    hWinInetDll = LoadLibrary ( "wininet.dll" );
    if ( hWinInetDll == NULL )
		return errOBJECT_CANNOT_BE_INITIALIZED;

    iInternetGetConnectedState = (typeInternetGetConnectedState) GetProcAddress(hWinInetDll, "InternetGetConnectedState");

    if ( ! iInternetGetConnectedState )
		return errOBJECT_CANNOT_BE_INITIALIZED;

//	if (g_hNotifyAddrChangeEvent)
	hIpHlpApi = LoadLibrary ( "iphlpapi.dll" );
	if (hIpHlpApi)
		GetIpAddrTable = (typeGetIpAddrTable)GetProcAddress(hIpHlpApi, "GetIpAddrTable");
//	if (hIpHlpApi)
//		NotifyAddrChange = (typeNotifyAddrChange)GetProcAddress(hIpHlpApi, "NotifyAddrChange");
//	if (NotifyAddrChange)
//	{
//		OVERLAPPED overlapped;
//		HANDLE dummy = NULL;
//		memset(&overlapped, 0, sizeof(overlapped));
//		overlapped.hEvent = g_hNotifyAddrChangeEvent;
//		DWORD error = NotifyAddrChange(&dummy, &overlapped);
//		if (error != ERROR_IO_PENDING && error != NO_ERROR)
//			NotifyAddrChange = NULL;
//	}
//
	error = sysCreateObjectQuick((hOBJECT*)&m_hCS, IID_CRITICAL_SECTION);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//    Notification that all necessary properties have been set and object must go to operation state
// Result comment
//    Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR NetDetect::ObjectInitDone()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "NetDetect::ObjectInitDone method" );

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//    Kernel warns object it is going to close all children
// Behaviour comment
//    Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR NetDetect::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "NetDetect::ObjectPreClose method" );

    if ( hWinInetDll ) FreeLibrary ( hWinInetDll );
    if ( wsaData.wHighVersion || wsaData.wVersion ) WSACleanup();
    if ( hWs2Dll ) FreeLibrary ( hWs2Dll );
	if (hIpHlpApi) FreeLibrary(hIpHlpApi);

	if (m_pIpTable) heapFree(m_pIpTable);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Detect )
// Behaviour comment
//    If internet connection present method returns success error code.
// Parameters are:
tERROR NetDetect::Detect()
{
	tDWORD dwFlags;
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "NetDetect::Detect method" );

    error = InternetGetConnectedState(&dwFlags);

    if ( PR_FAIL(error) && dwFlags == 0 )
		return errNOT_OK;

    if ( dwFlags & cINTERNET_CONNECTION_MODEM )
        return error;

    if ( dwFlags & ( cINTERNET_CONNECTION_LAN | cINTERNET_CONNECTION_PROXY ) )
    {
        if ( gethostbyname ( "kaspersky.com" ) != NULL ||
			 gethostbyname ( "kaspersky.ru"  ) != NULL )
            return errOK;
		return errNOT_OK;
    }

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, InternetGetConnectedState )
// Parameters are:
tERROR NetDetect::InternetGetConnectedState( tDWORD* p_pdwFlags )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "NetDetect::InternetGetConnectedState method" );

	if (p_pdwFlags == NULL)
		return errPARAMETER_INVALID;
	*p_pdwFlags = 0;
	if (!iInternetGetConnectedState((DWORD*)p_pdwFlags, 0))
		error = errNOT_OK;
	
	return error;
}
// AVP Prague stamp end

tERROR NetDetect::DumpTable(PMIB_IPADDRTABLE pIpTable, char* szName, time_t tTime)
{
	DWORD i;
	PR_TRACE(( this, prtNOTIFY, "ndet\tIP addtess table (%s at %d)", szName, tTime));
	if (!pIpTable)
	{
		PR_TRACE(( this, prtNOTIFY, "ndet\t\ttable empty"));
		return errOK;
	}
	for (i=0; i<pIpTable->dwNumEntries; i++)
	{
		MIB_IPADDRROW* pAddr = &pIpTable->table[i];
		PR_TRACE(( this, prtNOTIFY, "ndet\t\taddr:%08X index:%08X mask:%08X caddr:%08X rasm:%08X u1:%04X type:%04X", pAddr->dwAddr, pAddr->dwIndex, pAddr->dwMask, pAddr->dwBCastAddr, pAddr->dwReasmSize, pAddr->unused1, pAddr->wType));
	}
	return errOK;
}

tERROR NetDetect::FixTable(PMIB_IPADDRTABLE pIpTable)
{
	DWORD i;
	if (!pIpTable)
		return errOK;
	for (i=0; i<pIpTable->dwNumEntries; i++)
	{
		MIB_IPADDRROW* pAddr = &pIpTable->table[i];
		if (m_dwWindowsMajorVersion < 5) // Win9x, NT4
		{
			pAddr->unused1 = 0;
			pAddr->wType = 0;
		}
		else
		{
			pAddr->wType &= MIB_IPADDR_DISCONNECTED;
		}
	}
	return errOK;
}

// AVP Prague stamp begin( External (user called) interface method implementation, GetLastIpAddrChange )
// Parameters are:
tERROR NetDetect::GetLastIpAddrChange( tDWORD* p_pdwLastChangeTime ) {
	tERROR error = errOK;
	BYTE   buff[0x200];
	PMIB_IPADDRTABLE pIpTable = (PMIB_IPADDRTABLE)buff;
	DWORD dwIpTableSize = sizeof(buff);
	PR_TRACE_FUNC_FRAME( "NetDetect::GetLastIpAddrChange method" );

//	if( GetVersion() & 0x80000000 ) // Win9x hard fix
//	{
//		if( p_pdwLastChangeTime )
//			*p_pdwLastChangeTime = 0;
//		return errOK;
//	}

	if (!GetIpAddrTable)
	{
		PR_TRACE((this, prtERROR, "ndet\tGetLastIpAddrChange: function not supported, errNOT_SUPPORTED"));
		return errNOT_SUPPORTED;
	}
	if (!p_pdwLastChangeTime)
	{
		PR_TRACE((this, prtERROR, "ndet\tGetLastIpAddrChange: !p_pdwLastChangeTime, errPARAMETER_INVALID"));
		return errPARAMETER_INVALID;
	}

	DWORD dwError = GetIpAddrTable(pIpTable, &dwIpTableSize, FALSE);
	if (ERROR_INSUFFICIENT_BUFFER == dwError)
	{
		if (PR_SUCC(error = heapAlloc((tPTR*)&pIpTable, dwIpTableSize)))
			dwError = GetIpAddrTable(pIpTable, &dwIpTableSize, FALSE);
	}

	if (NO_ERROR == dwError)
	{
		bool bChanged = false;
		
		FixTable(pIpTable); // fix new table
		
		cAutoCS(m_hCS, true);
		if (!m_pIpTable)
		{
			PR_TRACE((this, prtIMPORTANT, "ndet\tGetIpAddrTable ok, first query size=%d, entries=%d", dwIpTableSize, pIpTable->dwNumEntries));
			bChanged = true;
		}
		else if (m_pIpTable->dwNumEntries != pIpTable->dwNumEntries)
		{
			PR_TRACE((this, prtIMPORTANT, "ndet\tGetIpAddrTable ok, ENTRIES COUNT changed was=%d new=%d", m_pIpTable->dwNumEntries, pIpTable->dwNumEntries));
			bChanged = true;
		}
		else if (0!=memcmp(pIpTable->table, m_pIpTable->table, pIpTable->dwNumEntries * sizeof(pIpTable->table[0])))
		{
			PR_TRACE((this, prtIMPORTANT, "ndet\tGetIpAddrTable ok, DATA changed"));
			bChanged = true;
		}
		else
		{
			PR_TRACE((this, prtNOTIFY, "ndet\tGetIpAddrTable ok, entries=%d", pIpTable->dwNumEntries));
		}
		if (bChanged)
		{
			DumpTable(m_pIpTable, "old", m_nLastNotifyAddrChangeTime);
			m_nLastNotifyAddrChangeTime = cDateTime::now_utc();
			DumpTable(pIpTable, "new", m_nLastNotifyAddrChangeTime);
			error = heapRealloc((tPTR*)&m_pIpTable, m_pIpTable, dwIpTableSize);
			if (PR_FAIL(error))
			{
				PR_TRACE((this, prtERROR, "ndet\tGetLastIpAddrChange: realloc failed, %terr", error));
				m_pIpTable = NULL;
				m_dwIpTableSize = 0;
			}
			else
			{
				memcpy(m_pIpTable, pIpTable, dwIpTableSize);
				m_dwIpTableSize = dwIpTableSize;
			}
		}
		PR_TRACE((this, prtNOTIFY, "ndet\tGetLastIpAddrChange: success, changed=%s", bChanged ? "true" : "false"));
	}
	else
	{
		switch (dwError)
		{
		case ERROR_INSUFFICIENT_BUFFER: error = errBUFFER_TOO_SMALL; break;
		case ERROR_NOT_SUPPORTED:       error = errNOT_SUPPORTED; break;
		case ERROR_INVALID_PARAMETER:   error = errPARAMETER_INVALID; break;
		default:                        error = errUNEXPECTED; break;
		}
		PR_TRACE((this, prtERROR, "ndet\tGetLastIpAddrChange: failed, w32error=%d, %terr", dwError, error));
	}

	if (pIpTable != (PMIB_IPADDRTABLE)buff)
		heapFree(pIpTable);

	*p_pdwLastChangeTime = (tDWORD)m_nLastNotifyAddrChangeTime;
	return error;
}
// AVP Prague stamp end


// AVP Prague stamp begin( C++ class regitration,  )
// Interface "NetDetect". Register function
tERROR NetDetect::Register( hROOT root ) {
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(NetDetect_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, NetDetect_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "", 1 )
mpPROPERTY_TABLE_END(NetDetect_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(NetDetect)
	mINTERNAL_METHOD(ObjectInit)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
mINTERNAL_TABLE_END(NetDetect)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(NetDetect)
	mEXTERNAL_METHOD(NetDetect, Detect)
	mEXTERNAL_METHOD(NetDetect, InternetGetConnectedState)
	mEXTERNAL_METHOD(NetDetect, GetLastIpAddrChange)
mEXTERNAL_TABLE_END(NetDetect)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "NetDetect::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_NETDETECT,                          // interface identifier
		PID_NETDETECT,                          // plugin identifier
		0x00000000,                             // subtype identifier
		NetDetect_VERSION,                      // interface version
		VID_MIKE,                               // interface developer
		&i_NetDetect_vtbl,                      // internal(kernel called) function table
		(sizeof(i_NetDetect_vtbl)/sizeof(tPTR)),// internal function table size
		&e_NetDetect_vtbl,                      // external function table
		(sizeof(e_NetDetect_vtbl)/sizeof(tPTR)),// external function table size
		NetDetect_PropTable,                    // property table
		mPROPERTY_TABLE_SIZE(NetDetect_PropTable),// property table size
		sizeof(NetDetect)-sizeof(cObjImpl),     // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"NetDetect(IID_NETDETECT) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call NetDetect_Register( hROOT root ) { return NetDetect::Register(root); }
// AVP Prague stamp end



