#include "stdafx.h"
#include "resource.h"
#include "Sound.h"
#include "mmsystem.h"
#include "../../Windows/Personal 4.6/exenames.h"
#include "../KLUtil/UnicodeConv.h"

struct KAVSoundEventDescription
{
	KAVSound::Sounds sound;
	const TCHAR *szEventLabel;
	UINT idResEventLabel;
	const TCHAR *szWavFileName;
};

static KAVSoundEventDescription g_KAVSoundEventDescription[] =
{
	{ KAVSound::eInfected, _T("KAV50_Infected"), IDS_COMMON_SOUND_EVENT_INFECTED, _T("infected.wav") },
	{ KAVSound::eAttackDetected, _T("KAV50_AttackDetected"), IDS_COMMON_SOUND_EVENT_ATTACK_DETECTED, _T("infected.wav") },
	
	{ KAVSound::ePasswordRequest, _T("KAV50_PasswordRequest"), IDS_COMMON_SOUND_EVENT_PASSWORD_REQUEST, _T("password.wav") },
	
	{ KAVSound::eProductStart, _T("KAV50_Start"), IDS_COMMON_SOUND_EVENT_PRODUCT_START, NULL },
	{ KAVSound::eProductFinish, _T("KAV50_Finish"), IDS_COMMON_SOUND_EVENT_PRODUCT_FINISH, NULL },

	{ KAVSound::eODSTaskStartedByUser, _T("KAV50_ODSTaskStartedByUser"), IDS_COMMON_SOUND_EVENT_ODS_TASK_STARTED_BY_USER, NULL },
	{ KAVSound::eODSTaskStartedByScheduler, _T("KAV50_ODSTaskStartedByScheduler"), IDS_COMMON_SOUND_EVENT_ODS_TASK_STARTED_BY_SCHEDULER, NULL },
	{ KAVSound::eODSTaskCompleted, _T("KAV50_ODSTaskCompleted"), IDS_COMMON_SOUND_EVENT_ODS_TASK_COMPLETED, _T("finish.wav") },
	{ KAVSound::eODSTaskFailed, _T("KAV50_ODSTaskFailed"), IDS_COMMON_SOUND_EVENT_ODS_TASK_FAILED, _T("finish.wav") },

	{ KAVSound::eUpdaterTaskStartedByUser, _T("KAV50_UpdaterTaskStartedByUser"), IDS_COMMON_SOUND_EVENT_UPDATER_TASK_STARTED_BY_USER, NULL },
	{ KAVSound::eUpdaterTaskStartedByScheduler, _T("KAV50_UpdaterTaskStartedByScheduler"), IDS_COMMON_SOUND_EVENT_UPDATER_TASK_STARTED_BY_SCHEDULER, NULL },
	{ KAVSound::eUpdaterTaskCompleted, _T("KAV50_UpdaterTaskCompleted"), IDS_COMMON_SOUND_EVENT_UPDATER_TASK_COMPLETED, _T("finish.wav") },
	{ KAVSound::eUpdaterTaskFailed, _T("KAV50_UpdaterTaskFailed"), IDS_COMMON_SOUND_EVENT_UPDATER_TASK_FAILED, _T("finish.wav") },

	{ KAVSound::eRTPTaskStarted, _T("KAV50_RTPTaskStarted"), IDS_COMMON_SOUND_EVENT_RTP_TASK_STARTED, NULL },
	{ KAVSound::eRTPTaskCompleted, _T("KAV50_RTPTaskCompleted"), IDS_COMMON_SOUND_EVENT_RTP_TASK_COMPLETED, _T("finish.wav") },
	{ KAVSound::eRTPTaskFailed, _T("KAV50_RTPTaskFailed"), IDS_COMMON_SOUND_EVENT_RTP_TASK_FAILED, _T("finish.wav") },
};

static WCHAR *g_KAVSoundObsoleted[] =
{
	L"KAV50_TaskCompleted"
};

static void ClearRegistryKey(HKEY hKey)
{
	TCHAR szValue[0x1000];
	DWORD dwSize;

	while (RegEnumValue (hKey, 0, szValue, &(dwSize = sizeof (szValue)), NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
	{
		if (RegDeleteValue (hKey, szValue) != ERROR_SUCCESS)
			break;
	}
}

static bool RecurClearRegistryKeyA(HKEY hKey, LPCSTR szSubKey)
{
	HKEY hSubKey = NULL;
	
	if (RegOpenKeyExA (hKey, szSubKey, 0, KEY_READ | KEY_WRITE, &hSubKey) != ERROR_SUCCESS)
		return false;
	
	CHAR szKey[0x200];
	DWORD dwKeySize;
	FILETIME ft;
	
	while (RegEnumKeyExA (hSubKey, 0, szKey, &(dwKeySize = sizeof (szKey) / sizeof (szKey[0])), NULL, NULL, NULL, &ft) == ERROR_SUCCESS)
	{
		if (!RecurClearRegistryKeyA (hSubKey, szKey))
			break;
	}
	
	ClearRegistryKey (hSubKey);
	RegCloseKey (hSubKey);
	
	return RegDeleteKeyA (hKey, szSubKey) == ERROR_SUCCESS;
}

static bool RecurClearRegistryKeyW(HKEY hKey, LPCWSTR szSubKey)
{
	HKEY hSubKey = NULL;
	
	if (RegOpenKeyExW (hKey, szSubKey, 0, KEY_READ | KEY_WRITE, &hSubKey) != ERROR_SUCCESS)
		return false;
	
	WCHAR szKey[0x200];
	DWORD dwKeySize;
	FILETIME ft;
	
	while (RegEnumKeyExW (hSubKey, 0, szKey, &(dwKeySize = sizeof (szKey) / sizeof (szKey[0])), NULL, NULL, NULL, &ft) == ERROR_SUCCESS)
	{
		if (!RecurClearRegistryKeyW (hSubKey, szKey))
			break;
	}
	
	ClearRegistryKey (hSubKey);
	RegCloseKey (hSubKey);
	
	return RegDeleteKeyW (hKey, szSubKey) == ERROR_SUCCESS;
}

static void RecurClearRegistryKey(HKEY hKey, LPCWSTR szSubKey)
{
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof(os);
	GetVersionEx(&os);
	if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
		RecurClearRegistryKeyW(hKey, szSubKey);
	else
		RecurClearRegistryKeyA(hKey, __LPCSTR(szSubKey));
}

static bool CreateSoundEventLabels ()
{
	// event labels
	HKEY hEventLabelsKey = NULL;

	if (RegOpenKeyEx (HKEY_CURRENT_USER, _T("AppEvents\\EventLabels"), 0, KEY_ALL_ACCESS, &hEventLabelsKey) != ERROR_SUCCESS)
		return false;

	int i;
	for (i = 0; i < sizeof (g_KAVSoundObsoleted) / sizeof (g_KAVSoundObsoleted[0]); ++i)
	{
		RecurClearRegistryKey(hEventLabelsKey, g_KAVSoundObsoleted[i]);
	}
	for (i = 0; i < sizeof (g_KAVSoundEventDescription) / sizeof (g_KAVSoundEventDescription[0]); ++i)
	{
		HKEY hEventLabelKey = NULL;
		if (RegCreateKeyEx (hEventLabelsKey, g_KAVSoundEventDescription[i].szEventLabel, 0, NULL,
								REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hEventLabelKey, NULL) != ERROR_SUCCESS)
		{
			continue;
		}

		CString strEventLabel ((LPCTSTR)g_KAVSoundEventDescription[i].idResEventLabel);

		if (!strEventLabel.IsEmpty ())
		{
			RegSetValueEx (hEventLabelKey, NULL, 0, REG_SZ, (CONST BYTE *)(LPCTSTR)strEventLabel, strEventLabel.GetLength () + sizeof ('\0'));
		}
		
		RegCloseKey (hEventLabelKey);
	}

	if (hEventLabelsKey)
		RegCloseKey (hEventLabelsKey);

	return true;
}

bool KAVSound::CheckCurrentUserSoundSchema ()
{
	if (!CreateSoundEventLabels ())
		return false;

	HKEY hAppEventsKey = NULL;

	if (RegOpenKeyEx (HKEY_CURRENT_USER, _T("AppEvents\\Schemes\\Apps"), 0, KEY_ALL_ACCESS, &hAppEventsKey) != ERROR_SUCCESS)
		return false;

	CString strProductName ((LPCTSTR)IDS_COMMON_PRODUCT_NAME);

	if (strProductName.IsEmpty ())
		return false;

	CString strCurrentDirectory;
	if (::GetCurrentDirectory (MAX_PATH, strCurrentDirectory.GetBufferSetLength (MAX_PATH)) == 0)
		return false;
	strCurrentDirectory.ReleaseBuffer ();
	if (strCurrentDirectory.GetAt(strCurrentDirectory.GetLength () - 1) != _T('\\'))
		strCurrentDirectory += _T('\\');

	HKEY hProductEventsKey = NULL;

	if (RegCreateKeyEx (hAppEventsKey, KAV_EXENAME_GUI, 0, NULL,
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hProductEventsKey, NULL) == ERROR_SUCCESS)
	{
		RegSetValueEx (hProductEventsKey, NULL, 0, REG_SZ, 
			(CONST BYTE *)(LPCTSTR)strProductName, strProductName.GetLength () + sizeof ('\0'));

		int i;
		for (i = 0; i < sizeof (g_KAVSoundObsoleted) / sizeof (g_KAVSoundObsoleted[0]); ++i)
		{
			RecurClearRegistryKey(hProductEventsKey, g_KAVSoundObsoleted[i]);
		}
		for (i = 0; i < sizeof (g_KAVSoundEventDescription) / sizeof (g_KAVSoundEventDescription[0]); ++i)
		{
			HKEY hEventLabelKey = NULL;
			if (RegCreateKeyEx (hProductEventsKey, g_KAVSoundEventDescription[i].szEventLabel, 0, NULL,
									REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hEventLabelKey, NULL) != ERROR_SUCCESS)
			{
				continue;
			}
			
			HKEY hDefEventLabelKey = NULL;
			DWORD dwDisposition = 0;
			if (RegCreateKeyEx (hEventLabelKey, _T(".Current"), 0, NULL,
									REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hDefEventLabelKey, &dwDisposition) == ERROR_SUCCESS)
			{
				if (dwDisposition == REG_CREATED_NEW_KEY && g_KAVSoundEventDescription[i].szWavFileName)
				{
					CString strDefValue = strCurrentDirectory + g_KAVSoundEventDescription[i].szWavFileName;
					RegSetValueEx (hDefEventLabelKey, NULL, 0, REG_SZ, 
						(CONST BYTE *)(LPCTSTR)strDefValue, strDefValue.GetLength () + sizeof ('\0'));
				}
			
				RegCloseKey (hDefEventLabelKey);
			}
			
			RegCloseKey (hEventLabelKey);
		}

		RegCloseKey (hProductEventsKey);
	}

	if (hAppEventsKey)
		RegCloseKey (hAppEventsKey);

	return true;
}

bool KAVSound::PlayKAVSound (KAVSound::Sounds sound, bool bWait)
{
	for (int i = 0; i < sizeof (g_KAVSoundEventDescription) / sizeof (g_KAVSoundEventDescription[0]); ++i)
	{
		if (sound == g_KAVSoundEventDescription[i].sound)
		{
			return ::PlaySound (g_KAVSoundEventDescription[i].szEventLabel, NULL,
				SND_APPLICATION | SND_NODEFAULT | SND_NOSTOP | (bWait ? SND_SYNC : SND_ASYNC)) != FALSE;
		}
	}

	return false;
}