#include <windows.h>
#include <shlobj.h>
#include <windns.h>
#include "MD5Hash.h"

#pragma warning(disable:4996)		// deprecated string functions
#pragma warning(disable:4390)		// empty control statement

#include "..\hook\idriver.h"
#include "..\hook\hookspec.h"
#include "..\hook\funcs.h"
#include "..\kldefs.h"
#include "..\r3hook\debug.h"
#include "..\r3hook\hookbase64.h"
#include "common.h"
#include "..\plugins\include\fw_ev_api.h"
#include "..\hook\singleparam.h"

extern CLIENT_CONTEXT g_ClientContext;
extern volatile BOOL g_bHookBaseInited;

#define DNSQ_RPORT      53
#define DNSQ_LPORT      0xffff
#define DNSQ_PROTOCOL   0x11
#define DNSQ_STREAM_DIR FW_STREAM_OUT


bool DnsqGetApplicationChecksum (IN LPCWSTR szwApplicationPath, OUT UCHAR Checksum[FW_HASH_SIZE])
{	
    bool bHashCalculated = false;
    HANDLE hFile = INVALID_HANDLE_VALUE;

    // CWow64FsRedirectorDisabler Wow64FsRedirectorDisabler;
    
    hFile = CreateFileW(szwApplicationPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if ( hFile == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    char Buffer[ sizeof(EXTERNAL_DRV_REQUEST) + sizeof(FIDBOX2_REQUEST_GET) + sizeof(FIDBOX2_REQUEST_DATA)];
    memset(Buffer, 0, sizeof(Buffer));

    PEXTERNAL_DRV_REQUEST pRequest = (PEXTERNAL_DRV_REQUEST) Buffer;
    pRequest->m_DrvID		= FLTTYPE_SYSTEM;
    pRequest->m_IOCTL		= _AVPG_IOCTL_FIDBOX2_GET;
    pRequest->m_AppID		= _SYSTEM_APPLICATION;
    pRequest->m_BufferSize	= sizeof(FIDBOX2_REQUEST_GET);

    PFIDBOX2_REQUEST_GET pfd2get = (PFIDBOX2_REQUEST_GET)pRequest->m_Buffer;
    pfd2get->m_hFile		= (DWORD_PTR)hFile;

    ULONG retsize = sizeof( Buffer );


    do {
        if ( IDriverExternalDrvRequest(g_ClientContext.m_hDevice, pRequest, Buffer, &retsize) == TRUE )	
        {
            if ( retsize >= sizeof ( FIDBOX2_REQUEST_DATA ) )
            {
                PFIDBOX2_REQUEST_DATA preqdata = (PFIDBOX2_REQUEST_DATA) Buffer;

                // preqdata - по всему буферу посчитать md5 hash и вернуть его.
                md5_state_t state;

                md5_init( &state );

                md5_append( &state, (UCHAR*)preqdata, retsize );

                md5_finish( &state, (UCHAR*)Checksum );

                bHashCalculated = true;					
            }
        }
        else
        {                
        }
    } while ( FALSE  );
    

    if ( !bHashCalculated )
    {	    
        md5_state_t state;

        md5_init(&state);

        md5_byte_t pBuffer[32 * 1024];

        DWORD dwRead = 0;

        do
        {
            ReadFile(hFile, pBuffer, sizeof (pBuffer), &dwRead, NULL);
            md5_append(&state, pBuffer, dwRead);
        } while(dwRead == sizeof(pBuffer));

        md5_finish(&state, Checksum);

        bHashCalculated = true;
    }

    CloseHandle(hFile);

    if (!bHashCalculated)
    {
        false;
    }

    return true;
}



VERDICT
DnsCheckPktFilters( ULONG rIp )
{
    VERDICT Verdict = Verdict_NotFiltered;
    CHAR Buf[0x1000];
    ZeroMemory(Buf, sizeof(Buf));
    PFILTER_EVENT_PARAM pParam  = (PFILTER_EVENT_PARAM)Buf;
    PSINGLE_PARAM       pSingleParam = (PSINGLE_PARAM)pParam->Params;

    FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_KLICK, FW_NDIS_PACKET_OUT, 0, CheckProcessing, 1);

    SINGLE_PARAM_INIT_SID(pSingleParam);

    ADD_ULONG_PARAM  ( pSingleParam, pParam, FW_ID_REMOTE_IP,			rIp );
    ADD_USHORT_PARAM ( pSingleParam, pParam, FW_ID_REMOTE_PORT,			DNSQ_RPORT	);
    ADD_USHORT_PARAM ( pSingleParam, pParam, FW_ID_LOCAL_PORT,			DNSQ_LPORT	);
    ADD_UCHAR_PARAM	 ( pSingleParam, pParam, FW_ID_PROTOCOL,			DNSQ_PROTOCOL );
    ADD_ULONG_PARAM  ( pSingleParam, pParam, FW_ID_STREAM_DIRECTION,	DNSQ_STREAM_DIR );

    BOOL bResult = IDriverFilterEvent(g_ClientContext.m_hDevice, pParam, FALSE, &Verdict);

    if (!bResult)
        ODS(("IDriverFilterEvent (pkt) failed"));

    return Verdict;
}

WCHAR*
DnsGetCommandLine()
{
    WCHAR* CmdLine = GetCommandLineW();

    if ( CmdLine )
    {
        if ( CmdLine[0] == L'\"' )
        {
            CmdLine = wcschr( CmdLine + 1, L'\"' );
        }

        if ( CmdLine == NULL )
            return NULL;

        CmdLine = wcschr( CmdLine, L' ' ) + 1;
    }

    return CmdLine;
}


VERDICT DnsQuery_Common(PWCHAR wcName, PIP4_ARRAY pServerIps)
{
	VERDICT Verdict = Verdict_NotFiltered;
    ULONG rIp;

	ODS(("querying for %S ..", wcName));

	if (!g_bHookBaseInited)
		return Verdict_NotFiltered;

    rIp = pServerIps && pServerIps->AddrCount ?	pServerIps->AddrArray[0] : 0xffffffff;

    Verdict = DnsCheckPktFilters( rIp );

    switch( Verdict ) 
    {
    case Verdict_Discard:
    case Verdict_Kill:
        
    case Verdict_Default:
    case Verdict_Pass:
    case Verdict_WDog:
    case Verdict_Continue:

        return Verdict;
    }

    FILETIME FileTime;
    SYSTEMTIME SysTime;

    GetSystemTimeAsFileTime(&FileTime);

    FileTimeToSystemTime( &FileTime, &SysTime );

	CHAR Buf[0x10000];		// javaw.exe can have very large command-lines!
	ZeroMemory(Buf, sizeof(Buf));
	PFILTER_EVENT_PARAM pParam  = (PFILTER_EVENT_PARAM)Buf;
	PSINGLE_PARAM       pSingleParam = (PSINGLE_PARAM)pParam->Params;

	FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_KLIN, FW_EVENT_UDP_SEND, 0, PreProcessing, 1);

	SINGLE_PARAM_INIT_SID(pSingleParam);

	try
	{
		WCHAR wcImagePath[1024];
        UCHAR Checksum[FW_HASH_SIZE];

		DWORD dwResult = GetModuleFileNameW(NULL, wcImagePath, _countof(wcImagePath));
		if (dwResult)
		{
			WCHAR wcLongPath[1024];
			PWCHAR wcTruePath = wcImagePath;  

			// try to get long path name..
			dwResult = GetLongPathNameW(wcImagePath, wcLongPath, _countof(wcLongPath));
			if (dwResult && dwResult <= _countof(wcLongPath))
				wcTruePath = wcLongPath;

			CharUpperW(wcTruePath);
			COPY_BINARY_PARAM( pSingleParam, pParam, FW_ID_FULL_APP_PATH,	wcTruePath, (DWORD)sizeof(WCHAR)*(wcslen(wcTruePath)+1) );

            DnsqGetApplicationChecksum( wcTruePath,  Checksum );

            COPY_BINARY_PARAM( pSingleParam, pParam, FW_ID_FILE_CHECKSUM,	Checksum, sizeof( Checksum ) );

            WCHAR* cmdline = DnsGetCommandLine();

            if ( cmdline )
            {
                COPY_BINARY_PARAM( pSingleParam, pParam, FW_ID_COMMAND_LINE,cmdline, (DWORD)sizeof(WCHAR)*(wcslen(cmdline)+1) );

                CharUpperW( (WCHAR*)pSingleParam->ParamValue );
            }
		}

		ADD_ULONG_PARAM  ( pSingleParam, pParam, FW_ID_REMOTE_IP,			rIp );
		ADD_USHORT_PARAM ( pSingleParam, pParam, FW_ID_REMOTE_PORT,			DNSQ_RPORT	);
		ADD_USHORT_PARAM ( pSingleParam, pParam, FW_ID_LOCAL_PORT,			DNSQ_LPORT	);
		ADD_UCHAR_PARAM	 ( pSingleParam, pParam, FW_ID_PROTOCOL,			DNSQ_PROTOCOL);
		ADD_ULONG_PARAM  ( pSingleParam, pParam, FW_ID_STREAM_DIRECTION,	DNSQ_STREAM_DIR );

        COPY_BINARY_PARAM( pSingleParam, pParam, FW_ID_EVENT_TIME,      &FileTime, sizeof (FileTime)  );

        ADD_UCHAR_PARAM  (pSingleParam, pParam, FW_ID_TIME_SECOND,		(UCHAR)SysTime.wSecond		); 
        ADD_UCHAR_PARAM  (pSingleParam, pParam, FW_ID_TIME_MINUTE,		(UCHAR)SysTime.wMinute		); 
        ADD_UCHAR_PARAM  (pSingleParam, pParam, FW_ID_TIME_HOUR,		(UCHAR)SysTime.wHour		); 
        ADD_UCHAR_PARAM  (pSingleParam, pParam, FW_ID_TIME_DAY,		    (UCHAR)SysTime.wDay		    ); 
        ADD_UCHAR_PARAM  (pSingleParam, pParam, FW_ID_TIME_WEEK_DAY,	(UCHAR)SysTime.wDayOfWeek		); 
        ADD_UCHAR_PARAM  (pSingleParam, pParam, FW_ID_TIME_MONTH,		(UCHAR)SysTime.wMonth		); 
        ADD_USHORT_PARAM (pSingleParam, pParam, FW_ID_TIME_YEAR,		SysTime.wYear		); 

        ULONG DaySecond = SysTime.wSecond + SysTime.wMinute * 60 + SysTime.wHour * 3600;
        ADD_ULONG_PARAM  (pSingleParam, pParam, FW_ID_TIME_DAY_SECOND,	DaySecond   );
	}
	catch(...)
	{
	}

	BOOL bResult = IDriverFilterEvent(g_ClientContext.m_hDevice, pParam, FALSE, &Verdict);
	if (!bResult)
		ODS(("IDriverFilterEvent (app) failed"));

	return Verdict;
}

typedef 
DNS_STATUS (WINAPI *t_fDnsQuery_A)(
						   PCSTR lpstrName,
						   WORD wType,
						   DWORD fOptions,
						   PIP4_ARRAY aipServers,
						   PDNS_RECORD* ppQueryResultsSet,
						   PVOID* pReserved
						   );
typedef 
DNS_STATUS (WINAPI *t_fDnsQuery_W)(
								   PCWSTR lpstrName,
								   WORD wType,
								   DWORD fOptions,
								   PIP4_ARRAY aipServers,
								   PDNS_RECORD* ppQueryResultsSet,
								   PVOID* pReserved
								   );
typedef 
DNS_STATUS (WINAPI *t_fDnsQuery_UTF8)(
								   PCSTR lpstrName,
								   WORD wType,
								   DWORD fOptions,
								   PIP4_ARRAY aipServers,
								   PDNS_RECORD* ppQueryResultsSet,
								   PVOID* pReserved
								   );

DNS_STATUS WINAPI Hook_DnsQuery_A(
								   PCSTR lpstrName,
								   WORD wType,
								   DWORD fOptions,
								   PIP4_ARRAY aipServers,
								   PDNS_RECORD* ppQueryResultsSet,
								   PVOID* pReserved
								   )
{
	VERDICT Verdict = Verdict_NotFiltered;
	DNS_STATUS dnsStatus = DNS_ERROR_INVALID_NAME;

	ODS(("DnsQuery_A::"));

	WCHAR wcNameBuf[0x1000];
	DWORD dwResult = MultiByteToWideChar(CP_ACP, 0, lpstrName, -1, wcNameBuf, _countof(wcNameBuf));
	if (dwResult)
		Verdict = DnsQuery_Common(wcNameBuf, aipServers);

	t_fDnsQuery_A fOrigFunc = (t_fDnsQuery_A)HookGetOrigFunc();
	if (fOrigFunc)
	{
		if (_PASS_VERDICT(Verdict))
			dnsStatus = fOrigFunc(lpstrName, wType, fOptions, aipServers, ppQueryResultsSet, pReserved);
	}

	return dnsStatus;
}

DNS_STATUS WINAPI Hook_DnsQuery_W(
								  PCWSTR lpstrName,
								  WORD wType,
								  DWORD fOptions,
								  PIP4_ARRAY aipServers,
								  PDNS_RECORD* ppQueryResultsSet,
								  PVOID* pReserved
								  )
{
	VERDICT Verdict = Verdict_NotFiltered;
	DNS_STATUS dnsStatus = DNS_ERROR_INVALID_NAME;

	ODS(("DnsQuery_W::"));

	Verdict = DnsQuery_Common((PWCHAR)lpstrName, aipServers);

	t_fDnsQuery_W fOrigFunc = (t_fDnsQuery_W)HookGetOrigFunc();
	if (fOrigFunc)
	{
		if (_PASS_VERDICT(Verdict))
			dnsStatus = fOrigFunc(lpstrName, wType, fOptions, aipServers, ppQueryResultsSet, pReserved);
	}

	return dnsStatus;
}

DNS_STATUS WINAPI Hook_DnsQuery_UTF8(
								  PCSTR lpstrName,
								  WORD wType,
								  DWORD fOptions,
								  PIP4_ARRAY aipServers,
								  PDNS_RECORD* ppQueryResultsSet,
								  PVOID* pReserved
								  )
{
	VERDICT Verdict = Verdict_NotFiltered;
	DNS_STATUS dnsStatus = DNS_ERROR_INVALID_NAME;

	ODS(("DnsQuery_UTF8::"));

	WCHAR wcNameBuf[0x1000];
	DWORD dwResult = MultiByteToWideChar(CP_UTF8, 0, lpstrName, -1, wcNameBuf, _countof(wcNameBuf));
	if (dwResult)
		Verdict = DnsQuery_Common(wcNameBuf, aipServers);

	t_fDnsQuery_UTF8 fOrigFunc = (t_fDnsQuery_UTF8)HookGetOrigFunc();
	if (fOrigFunc)
	{
		if (_PASS_VERDICT(Verdict))
			dnsStatus = fOrigFunc(lpstrName, wType, fOptions, aipServers, ppQueryResultsSet, pReserved);
	}

	return dnsStatus;
}

BOOL HookExports()
{
	BOOL bResult = FALSE;

	HMODULE hDnsapi = GetModuleHandleA("dnsapi.dll");
	if (hDnsapi)
	{
		bResult = HookExportInModule(hDnsapi, "DnsQuery_A", Hook_DnsQuery_A, NULL, 0);
		bResult &= HookExportInModule(hDnsapi, "DnsQuery_W", Hook_DnsQuery_W, NULL, 0);
		bResult &= HookExportInModule(hDnsapi, "DnsQuery_UTF8", Hook_DnsQuery_UTF8, NULL, 0);
	}

	return bResult;
}

