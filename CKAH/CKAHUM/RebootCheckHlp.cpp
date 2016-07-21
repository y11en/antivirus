#include "stdafx.h"
#include <io.h>
#include "RebootCheckHlp.h"
#include <StuffIO/Ioutil.h>
#include "UnicodeConv.h"
#include "ClientIDs.h"

static class CRebootClientHlp
{
public:
	CRebootClientHlp ()
	{
		/*!TODO
		OSVERSIONINFO OSVer;
		
		OSVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

		if (!GetVersionEx (&OSVer))
			return;

		char szWhistleUp[MAX_PATH];
		char szWFChanged[MAX_PATH];
		
		_snprintf (szWhistleUp, MAX_PATH, "Whistleup%u%u%u", ::GetCurrentProcessId (), ::GetCurrentThreadId (), ::GetTickCount ());
		_snprintf (szWFChanged, MAX_PATH, "WFChanged%u%u%u", ::GetCurrentProcessId (), ::GetCurrentThreadId (), ::GetTickCount ());

		ZeroMemory (&m_AppReg, sizeof (m_AppReg));

		m_AppReg.m_AppID = CLIENT_APP_ID_REBOOTHLP;
		m_AppReg.m_Priority = AVPG_STANDARTRIORITY;
		m_AppReg.m_ClientFlags = _CLIENT_FLAG_WITHOUTWATCHDOG | _CLIENT_FLAG_WITHOUTCASCADE;
		m_AppReg.m_CacheSize = 4096;
		m_AppReg.m_CurProcId = ::GetCurrentProcessId ();
		m_AppReg.m_BlueScreenTimeout = DEADLOCKWDOG_TIMEOUT;

		m_hWhistleup = m_hWFChanged = NULL;

		m_hDevice = RegisterApp (&m_AppReg, &OSVer, &m_hWhistleup, &m_hWFChanged, szWhistleUp, szWFChanged);*/
	}

	~CRebootClientHlp ()
	{
		/* !TODO
		OSVERSIONINFO OSVer;
		OSVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		GetVersionEx (&OSVer);

		UnRegisterApp (m_hDevice, &m_AppReg, FALSE, &OSVer);

		if (m_hWhistleup)
			CloseHandle (m_hWhistleup);

		if (m_hWFChanged)
			CloseHandle (m_hWFChanged);*/
	}

	bool IsRebootQueried ()
	{
		bool bRetValue = false;
		/*!TODO
		DWORD BytesRet;
		DWORD dwFltSize = 1024;
		BYTE pBuf[1024];
		PFILTER_TRANSMIT pFilter = (PFILTER_TRANSMIT)pBuf;

		ZeroMemory (pBuf, sizeof (pBuf));

		pFilter->m_AppID = m_AppReg.m_AppID;
		pFilter->m_FltCtl = FLTCTL_FIRST;

		if (DeviceIoControl ((HANDLE)m_hDevice, IOCTLHOOK_FiltersCtl, pFilter, dwFltSize, pFilter, dwFltSize, &BytesRet, NULL))
			bRetValue = pFilter->m_FilterID != 0;*/

		return bRetValue;
	}

	bool QueryReboot ()
	{
		/*//!TODO
		return IsRebootQueried () || AddFSFilter2 (m_hDevice, m_AppReg.m_AppID, "*", 0, 
					FLT_A_DISABLED,
					FLTTYPE_FLT, 0, 0, 0,
					PreProcessing, NULL, NULL) != 0;*/
		return false;
	}
protected:

	APP_REGISTRATION m_AppReg;
	HANDLE m_hWhistleup, m_hWFChanged, m_hDevice;

} g_RebootClientHlp;

//////////////////////////////////////////////////////////////////////////

bool CRebootCheckHlp::IsRebootQueried ()
{
	return g_RebootClientHlp.IsRebootQueried ();
}

bool CRebootCheckHlp::QueryReboot ()
{
	return g_RebootClientHlp.QueryReboot ();
}
