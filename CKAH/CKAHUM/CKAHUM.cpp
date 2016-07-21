#include "stdafx.h"
#include <CKAH/ckahum.h>
#include "../ckahrules/ckahmain.h"
#include "Stuff/RegStorage.h"
#include "CKAHStorage.h"
#include "CKAHMain.h"

volatile long g_refcnt;

Guard g_guardStorage;

CExternalLogger g_externalLogger;

LPCKAHSTORAGE g_lpStorage = NULL;

CProgramExecutionLog log;

HANDLE g_hStopEvent;

bool g_bIsNT = true;

void GetWindowsVersion ()
{
	OSVERSIONINFO osvi;

	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

	if (!GetVersionEx (&osvi))
		return;

	g_bIsNT = (osvi.dwPlatformId >= VER_PLATFORM_WIN32_NT);
}

CExternalLogger::CExternalLogger()
{
}

CExternalLogger::~CExternalLogger()
{
}

void CExternalLogger::AddLogger(CKAHUM::EXTERNALLOGGERPROC logger)
{
    if ( logger )
    {
        Guard::Lock lock(m_guardLogger);

        if (std::find(m_logger.begin(), m_logger.end(), logger) == m_logger.end())
        {
            m_logger.push_back(logger);
        }
    }
}

void CExternalLogger::RemoveLogger(CKAHUM::EXTERNALLOGGERPROC logger)
{
    if ( logger )
    {
        Guard::Lock lock(m_guardLogger);

        EXTERNALLOGGERLIST::iterator i = std::find(m_logger.begin(), m_logger.end(), logger);
        if (i != m_logger.end())
        {
            m_logger.erase(i);
        }
    }
}

void CExternalLogger::LoggerFunc (DWORD dwEventType, LPCSTR szString)
{
    Guard::Lock lock(m_guardLogger);

    for (EXTERNALLOGGERLIST::iterator i = m_logger.begin(); i != m_logger.end (); ++i)
    {
		(*i) (dwEventType == PEL_ERROR ? CKAHUM::lError : (dwEventType == PEL_WARNING ? CKAHUM::lWarning : CKAHUM::lInfo), szString);
    }
}

static void CALLBACK LoggerFunc (DWORD dwEventType, LPCSTR szString)
{
    g_externalLogger.LoggerFunc(dwEventType, szString);
}


BOOL APIENTRY DllMain (HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		GetWindowsVersion ();
		g_hStopEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
		log.Initialize (LoggerFunc);
		CkahComm_SetExternalLogger (LoggerFunc);
        CkahRules_SetExternalLogger (LoggerFunc);
		DisableThreadLibraryCalls ((HMODULE)hModule);
		break;
	case DLL_PROCESS_DETACH:
		SetEvent (g_hStopEvent);
		CloseHandle (g_hStopEvent);
		g_hStopEvent = NULL;

		CKAHUM::StopAll ();
		log.Initialize (NULL);
        CkahRules_SetExternalLogger (NULL);
		CkahComm_SetExternalLogger (NULL);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}

    return TRUE;
}
