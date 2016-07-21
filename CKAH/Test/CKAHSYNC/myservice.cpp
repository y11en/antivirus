// myservice.cpp

#include "NTServApp.h"
#include "myservice.h"
#include "../../ckahum/Utils.h"

#ifndef _DEBUG
	#define DebugMsg
#endif

CMyService::CMyService() : CNTService("CKAHSync")
{
	DebugMsg("CMyService::CMyService");

	m_hStopEvent = NULL;
}

CMyService::~CMyService()
{
	if(m_hStopEvent)
		CloseHandle(m_hStopEvent);
}

BOOL CMyService::OnInit()
{
	// Read the registry parameters
    // Try opening the registry key:
    // HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\<AppName>\Parameters

    HKEY hkey;
	char szKey[1024];
	strcpy(szKey, "SYSTEM\\CurrentControlSet\\Services\\");
	strcat(szKey, m_szServiceName);
	strcat(szKey, "\\Parameters");
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                     szKey,
                     0,
                     KEY_QUERY_VALUE,
                     &hkey) == ERROR_SUCCESS) {

        RegCloseKey(hkey);
    }

	if(!m_hStopEvent)
		m_hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	return TRUE;
}

void CMyService::Run()
{
	if(!m_hStopEvent)
	{
		DebugMsg("CKAHSync: Run cannot start");	
		return;
	}

	DWORD dwWaitResult;
	HANDLE pHandles[] = { m_hStopEvent };

	DebugMsg("CKAHSync: Running...");

	do
	{
		dwWaitResult = WaitForMultipleObjects(sizeof (pHandles) / sizeof (pHandles[0]), pHandles, FALSE, INFINITE);

	} 
	while(dwWaitResult != WAIT_OBJECT_0);

	DebugMsg("CKAHSync: Quitting run");
}

void CMyService::OnStop()
{
	DebugMsg("CKAHSync: Stop received");
	SetEvent(m_hStopEvent);
}

//////////////////////////////////////////////////////////////////////////

const WCHAR *szNetBasesRootFolder = L"\\\\vintik\\Updates\\release\\";
const WCHAR *szLocIDSBasesFolder = L"o:\\ckah\\";
const WCHAR *szLocMergedBasesRootFolder = L"o:\\ckah\\MergedUpdates\\";
const WCHAR *szLocTempMergedBasesRootFolder = L"o:\\ckah\\Temp\\";

const WCHAR *szBasesFolders[] = { L"updates", L"updates_ext", L"updates_x" };

void CMyService::PerformBasesMerge ()
{
	DebugMsg("CKAHSync: Performing bases merge...");

	for (int bs = 0; bs < sizeof (szBasesFolders) / sizeof (szBasesFolders[0]); ++bs)
	{
		DebugMsg("CKAHSync: Processing '%ls' bases set", szBasesFolders[bs]);
		std::wstring sLocTempMergedBasesFolder = szLocTempMergedBasesRootFolder;
		sLocTempMergedBasesFolder += szBasesFolders[bs];
		if (!CreateCleanTempIDSFolder (sLocTempMergedBasesFolder.c_str ()))
		{
			DebugMsg("CKAHSync: Failed to create empty folder '%ls'", sLocTempMergedBasesFolder.c_str ());
			continue;
		}






		DeleteTempIDSFolder (sLocTempMergedBasesFolder.c_str ());
	}

	DeleteTempIDSFolder (szLocTempMergedBasesRootFolder);

	DebugMsg("CKAHSync: Bases merge done");
}
