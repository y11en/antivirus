/*!
 * (C) 2007 Kaspersky Lab 
 * 
 * \file	winsessions.h
 * \author	Andrew Lashenkov
 * \date	26.09.2007
 * \brief	
 * 
 */
#ifdef _WIN32
#ifndef __KL_WINSESSIONS_H__
#define __KL_WINSESSIONS_H__

#include <std/win32/klos_win32v40.h>
#include <string>
#include <vector>

namespace KLWSSN{

struct SessionInfo{
	DWORD m_dwId;
	std::basic_string<TCHAR> m_strWinStationName;
	SessionInfo(DWORD dwId = 0, LPCTSTR pWinStationName = NULL)
	:
		m_dwId(dwId)
	{
		if( !pWinStationName || !*pWinStationName )
		{
			m_strWinStationName = _T("WinSta0");
		}
		else
		{
			m_strWinStationName = pWinStationName;
		}
	}
};

typedef std::vector<KLWSSN::SessionInfo> SessionsInfo;

bool GetSessions(SessionsInfo& vecSessions);

void CreateProcessForSession(
	DWORD dwSessionId, 
	LPWSTR lpCommandLine, 
	STARTUPINFOW& si, 
	PROCESS_INFORMATION& pi);

};

#endif //__KL_WINSESSIONS_H__
#endif //_WIN32
