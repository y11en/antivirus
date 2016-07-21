// UncontrolledFiles.cpp: implementation of the CUncontrolledFiles class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UncontrolledFiles.h"
#include "AutoPTCHAR.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUncontrolledFiles::CUncontrolledFiles()
{
	HKEY hKeyEventsLog = NULL;

	TCHAR keyname[] = _T("SYSTEM\\CurrentControlSet\\Services\\Eventlog");
	CString strValue;
	CString strEnumName;
	CString strExpanded;

	CString strtmp;

	if(ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, keyname, &hKeyEventsLog))
	{
		AutoPTCHAR auptchValue(&strValue, 4096);
		AutoPTCHAR auptchExpanded(&strExpanded, 4096);
		AutoPTCHAR auptchTmpName(&strEnumName, 1024);

		if (auptchValue.IsValid() && auptchTmpName.IsValid() && auptchExpanded.IsValid())
		{
			HKEY hKeyTmp;
			DWORD dwIndex = 0;
			DWORD TypeStr = REG_SZ;
			
			DWORD dwlen;

			PTCHAR SubkeyName = auptchTmpName;
			DWORD dwSubkeyName = auptchTmpName.GetBufferLength();
			
			CString KeyNameTmp;
			
			while (RegEnumKey(hKeyEventsLog, dwIndex++, SubkeyName, dwSubkeyName) == ERROR_SUCCESS)
			{
				KeyNameTmp = keyname;
				KeyNameTmp += _T("\\");
				KeyNameTmp += SubkeyName;

				if(ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, KeyNameTmp, &hKeyTmp))
				{
					dwlen = auptchValue.GetBufferLength() * sizeof(TCHAR);
					if (ERROR_SUCCESS == RegQueryValueEx(hKeyTmp, _T("File"), 0, &TypeStr, auptchValue, &dwlen))
					{
						ExpandEnvironmentStrings(auptchValue, auptchExpanded, auptchExpanded.GetBufferLength());

						strtmp = auptchExpanded.GetPTCH();
						strtmp.MakeUpper();
						m_UncotrolledFileList.push_back(strtmp);
					}
					RegCloseKey(hKeyTmp);
				}
			}

			//+ ------------------------------------------------------------------------------------------
			PushEnvironmentString(_T("%SystemRoot%\\system32\\config\\software.LOG"));
			PushEnvironmentString(_T("%SystemRoot%\\system32\\config\\system.LOG"));
			PushEnvironmentString(_T("%SystemRoot%\\system32\\Config\\SAM.LOG"));
			PushEnvironmentString(_T("%SystemRoot%\\system32\\Config\\SAM"));
			PushEnvironmentString(_T("%SystemRoot%\\system32\\Config\\SOFTWARE"));
			PushEnvironmentString(_T("%SystemRoot%\\system32\\Config\\Security"));
			PushEnvironmentString(_T("%SystemRoot%\\system32\\Config\\SECURITY.LOG"));
			PushEnvironmentString(_T("%SystemRoot%\\system32\\Config\\DEFAULT"));
			PushEnvironmentString(_T("%SystemRoot%\\system32\\Config\\DEFAULT.LOG"));

			PushEnvironmentString(_T("%SystemRoot%\\system32\\Config\\SYSTEM"));
			PushEnvironmentString(_T("%SystemRoot%\\System32\\Wbem\\Logs\\wbemess.log"));

			PushEnvironmentString(_T("%SystemRoot%\\security\\logs\\diagnosis.log"));
			PushEnvironmentString(_T("%SystemRoot%\\security\\logs\\winlogon.log"));
			
			PushEnvironmentString(_T("%SystemRoot%\\Windows Update.log"));
		}

		RegCloseKey(hKeyEventsLog);
	}
}

CUncontrolledFiles::~CUncontrolledFiles()
{

}

void CUncontrolledFiles::PushEnvironmentString(PTCHAR ptchString)
{
	CString strtmp;
	CString strExpanded;
	{
		AutoPTCHAR auptchExpanded(&strtmp, 4096);
		if (auptchExpanded.IsValid())
		{
			ExpandEnvironmentStrings(ptchString, auptchExpanded, auptchExpanded.GetBufferLength());
			strExpanded = auptchExpanded.GetPTCH();
			strExpanded.MakeUpper();
			m_UncotrolledFileList.push_back(strExpanded);
		}
	}
}
