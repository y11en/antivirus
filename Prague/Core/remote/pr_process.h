/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	pr_client.h
 * \author	Pavel Mezhuev
 * \date	09.12.2002 17:34:41
 * \brief	Спецификация интерфейса для Prague-proxies.
 * 
 */
//************************************************************************

extern HANDLE g_module;

#ifndef __pr_process_h
#define __pr_process_h

#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <pdh.h>

#if defined (_M_IA64)
	#define RETURN_BACK _M_IA64
	#undef _M_IA64
#endif

#include <winternl.h>

#if defined(RETURN_BACK)
	#define _M_IA64 RETURN_BACK
	#undef RETURN_BACK
#endif

#include <psapi.h>
#include <wtsapi32.h>

//************************************************************************

class CAutoToken
{
public:
	CAutoToken(HANDLE hToken)
	{
		m_hOldToken = NULL;
		OpenThreadToken(GetCurrentThread(), TOKEN_QUERY|TOKEN_IMPERSONATE|TOKEN_DUPLICATE, TRUE, &m_hOldToken);
		if( hToken )
		{
			RevertToSelf();
//			SetThreadToken(NULL, hToken);
			GetLastError();
		}
	}

	~CAutoToken()
	{
		if( m_hOldToken )
		{
			SetThreadToken(NULL, m_hOldToken);
			CloseHandle(m_hOldToken);
		}
	}

private:
	HANDLE m_hOldToken;
};

//************************************************************************

struct WTSApi
{
	WTSApi() : m_bInited(FALSE){}

	void Init()
	{
		if( m_bInited ) return;
		HMODULE hMod = LoadLibrary("wtsapi32.dll");
		*(void**)&fnEnumerateSessions = hMod ? GetProcAddress(hMod, "WTSEnumerateSessionsA") : NULL;
		*(void**)&fnEnumerateProcesses = hMod ? GetProcAddress(hMod, "WTSEnumerateProcessesA") : NULL;
		*(void**)&fnFreeMemory = hMod ? GetProcAddress(hMod, "WTSFreeMemory") : NULL;
		*(void**)&fnQueryUserToken = hMod ? GetProcAddress(hMod, "WTSQueryUserToken") : NULL;

		hMod = LoadLibrary("advapi32.dll");
		*(void**)&fnOpenProcessToken = hMod ? GetProcAddress(hMod, "OpenProcessToken") : NULL;
		*(void**)&fnCreateProcessAsUser = hMod ? GetProcAddress(hMod, "CreateProcessAsUserA") : NULL;
		*(void**)&fnImpersonateLoggedOnUser = hMod ? GetProcAddress(hMod, "ImpersonateLoggedOnUser") : NULL;
		*(void**)&fnRevertToSelf = hMod ? GetProcAddress(hMod, "RevertToSelf") : NULL;

		hMod = LoadLibrary("ntdll.dll");
		*(void**)&fnNtQuerySystemInformation = hMod ? GetProcAddress(hMod, "NtQuerySystemInformation") : NULL;

		hMod = LoadLibrary("userenv.dll");
		*(void**)&fnCreateEnvironmentBlock = hMod ? GetProcAddress(hMod, "CreateEnvironmentBlock") : NULL;
		*(void**)&fnDestroyEnvironmentBlock = hMod ? GetProcAddress(hMod, "DestroyEnvironmentBlock") : NULL;

		AdjustProcessPrivileges();
		m_bInited = TRUE;
	}

	bool CreateProcess(tSESSION session_id, tSTRING exec_path, tSTRING exec_args, tPROCESS *process, bool bWait = false, bool bAsUser = false)
	{
		Init();

		if( session_id == PR_SESSION_ALL )
		{
			PWTS_SESSION_INFO pSInfo; DWORD nCount;
			if( fnEnumerateSessions && fnEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSInfo, &nCount) )
			{
				bool res = true;
				for(DWORD i = 0; i < nCount; i++)
					res &= CreateProcess(pSInfo[i].SessionId, exec_path, exec_args, process, bWait, true);

				fnFreeMemory(pSInfo);
				return res;
			}

			session_id = 0, bAsUser = true;
		}

		tCHAR path[MAX_PATH];
		GetModuleFileName((HINSTANCE)g_module, path, sizeof(path));

		tCHAR* file_name = strrchr(path, '\\');
		if( !file_name )
			return false;

		file_name++;
		strcpy_s(file_name, path + countof(path) - file_name, exec_path);

		PROCESS_INFORMATION pi;
		memset(&pi, 0, sizeof(pi));

		STARTUPINFO si;
		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);

		tCHAR args[MAX_PATH] = "";
		sprintf_s(args, countof(args), "%s %s", path, exec_args);

		BOOL bRetVal = FALSE;
		if( session_id != PR_SESSION_LOCAL && fnCreateProcessAsUser && !(GetVersion() & 0x80000000) )
		{
			HANDLE token = GetSessionToken(session_id);
			if( token )
			{
				CAutoToken autotoken(NULL);

				bRetVal = fnImpersonateLoggedOnUser(token);
				if( bRetVal )
				{
					si.lpDesktop = TEXT("winsta0\\default");

					LPVOID pEnv = NULL;
					fnCreateEnvironmentBlock(&pEnv, token, FALSE);

					bRetVal = fnCreateProcessAsUser(token, path, args,
						NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT, pEnv, NULL, &si, &pi);

					if( pEnv )
						fnDestroyEnvironmentBlock(pEnv);

					if( !bRetVal )
						PR_TRACE((g_root, prtERROR, "PR_PROCESS: CreateProcessAsUser failed (%x) !!!!!!", GetLastError()));

				}
				fnRevertToSelf();
				CloseHandle(token);
			}

			if( !bRetVal /*&& ((session_id == PR_SESSION_ACTIVE) || bAsUser)*/ )
				return false;
		}

		if( !bRetVal )
			bRetVal = ::CreateProcess(path, args, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

		if( bRetVal && bWait )
			WaitForSingleObject(pi.hProcess, INFINITE);

		if( pi.hProcess )
			CloseHandle(pi.hProcess);

		if( pi.hThread )
			CloseHandle(pi.hThread);

		if( bRetVal && process )
			*process = pi.dwProcessId;

		return !!bRetVal;
	}

	tSESSION GetActiveSessionId()
	{
		tSESSION session_id = 0;

		PWTS_SESSION_INFO pSInfo; DWORD nCount;
		if( !fnEnumerateSessions || !fnEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSInfo, &nCount) )
			return session_id;

		for(DWORD i = 0; i < nCount; i++)
			if( pSInfo[i].State == WTSActive )
			{
				session_id = pSInfo[i].SessionId;
				break;
			}

		fnFreeMemory(pSInfo);
		return session_id;
	}

	bool IsSystemToken(HANDLE hToken)
	{
		SID_IDENTIFIER_AUTHORITY authWorld = SECURITY_NT_AUTHORITY;
		PSID pSID = NULL;
		PTOKEN_USER pInfo = NULL;
		DWORD size = 0;

		BOOL bRet = AllocateAndInitializeSid(&authWorld, 1, SECURITY_LOCAL_SYSTEM_RID, 0, 0, 0, 0, 0, 0, 0, &pSID);

		if( bRet )
			GetTokenInformation(hToken, TokenUser, NULL, 0, &size);

		if( bRet && size )
		{
			pInfo = (PTOKEN_USER)malloc(size);
			bRet = GetTokenInformation(hToken, TokenUser, pInfo, size, &size);
		}

		if( bRet && pInfo )
			bRet = EqualSid(pSID, pInfo->User.Sid);

		if( pInfo )
			free(pInfo);

		if( pSID )
			FreeSid(pSID);

		return !!bRet;
	}

	bool GetUserToken(HANDLE& hTokenNew, HANDLE& hTokenOld)
	{
		Init();

		hTokenOld = NULL;
		OpenThreadToken(GetCurrentThread(), TOKEN_QUERY|TOKEN_IMPERSONATE|TOKEN_DUPLICATE, TRUE, &hTokenOld);
		if( !hTokenOld )
			OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY|TOKEN_IMPERSONATE|TOKEN_DUPLICATE, &hTokenOld);

		if( hTokenOld && !IsSystemToken(hTokenOld) )
			return true;

		HANDLE hUserToken = GetSessionToken(PR_SESSION_ACTIVE);
		if( !hUserToken )
		{
			PR_TRACE((g_root, prtERROR, "PR_PROCESS: Cannot get session token !!!!!!!"));
			return false;
		}

		DuplicateToken(hUserToken, SecurityImpersonation, &hTokenNew);
		if( !hTokenNew )
			PR_TRACE((g_root, prtERROR, "PR_PROCESS: Cannot duplicate token !!!!!!!"));

		CloseHandle(hUserToken);
		return true;
	}

	bool ImpersonateToUser(HANDLE& hTokenNew, HANDLE& hTokenOld)
	{
		if( !GetUserToken(hTokenNew, hTokenOld) )
			return false;

		if( hTokenNew && !SetThreadToken(NULL, hTokenNew) )
			PR_TRACE((g_root, prtERROR, "PR_PROCESS: Cannot set thread token !!!!!!!"));

		return true;
	}

	HANDLE GetSystemToken(bool bLocal = true)
	{
		Init();

		HANDLE hToken = NULL;
		OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY|TOKEN_IMPERSONATE|TOKEN_DUPLICATE, &hToken);

		if( IsSystemToken(hToken) )
			return hToken;
        if(hToken)
            CloseHandle(hToken);

		return bLocal ? NULL : GetProcessToken(0, "winlogon.exe");
	}

	HANDLE GetSessionToken(tSESSION session_id)
	{
		HANDLE token = NULL;

		if( session_id == PR_SESSION_BY_USER_TOKEN )
		{
			HANDLE hToken = NULL, hProcToken = NULL;

			if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY|TOKEN_DUPLICATE, &hProcToken) && hProcToken)
			{
				if( !IsSystemToken(hProcToken) )
				{
					CloseHandle(hProcToken);
					return NULL;
				}
				CloseHandle(hProcToken);
			}

			OpenThreadToken(GetCurrentThread(), TOKEN_QUERY|TOKEN_DUPLICATE, TRUE, &hToken);

			if( hToken && !IsSystemToken(hToken) )
				DuplicateTokenEx(hToken, TOKEN_QUERY|TOKEN_IMPERSONATE|TOKEN_DUPLICATE|TOKEN_ASSIGN_PRIMARY,
					NULL, SecurityImpersonation, TokenPrimary, &token);

			if( hToken )
				CloseHandle(hToken);

			if( token )
				return token;

			session_id = PR_SESSION_ACTIVE;
		}

		if( session_id == PR_SESSION_ACTIVE )
			session_id = GetActiveSessionId();

		if( fnQueryUserToken && fnQueryUserToken(session_id, &token) )
			return token;

		return GetProcessToken(session_id, "explorer.exe");
	}

	HANDLE GetProcessToken(tSESSION session_id, LPCSTR szAppName)
	{
		HANDLE hProcess = NULL;
		HANDLE hToken = NULL;
		PVOID pInfo = NULL;
		DWORD dwResult, dwSize = 0x1000, dwNameLen = (tDWORD)strlen(szAppName), j;

		if( !fnNtQuerySystemInformation )
			return hToken;

		dwResult = 0x80000005L;
		for(int i = 0; i < 100 && dwResult; i++, dwSize *= 2)
		{
			pInfo = realloc(pInfo, dwSize);
			dwResult = fnNtQuerySystemInformation(SystemProcessInformation, pInfo, dwSize, NULL);
			if( dwResult != 0x80000005L && dwResult != 0xC0000004L )
				break;
		}

		if( dwResult )
			return hToken;

		for(PSYSTEM_PROCESS_INFORMATION pi = (PSYSTEM_PROCESS_INFORMATION)pInfo; pi;
					pi = (PSYSTEM_PROCESS_INFORMATION)((PBYTE)pi + pi->NextEntryOffset))
		{
			WCHAR* szName = (WCHAR*)((DWORD*)pi->Reserved1)[14];
			for(j = 0; szName && j < dwNameLen; j++)
				if( !szName[j] || toupper(szName[j]) != toupper(szAppName[j]) )
					break;

			if( j == dwNameLen ) // found
			{
				hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pi->UniqueProcessId);
				break;
			}

			if( !pi->NextEntryOffset || pi->NextEntryOffset == -1 )
				break;
		}

		if( hProcess )
		{
			fnOpenProcessToken(hProcess, TOKEN_DUPLICATE|TOKEN_IMPERSONATE|TOKEN_QUERY|TOKEN_ASSIGN_PRIMARY, &hToken);
			if( !hToken )
				PR_TRACE((g_root, prtERROR, "PR_PROCESS: Cannot open %s token (%x) !!!!!!", szAppName, GetLastError()));
			CloseHandle(hProcess);
		}
		else
			PR_TRACE((g_root, prtERROR, "PR_PROCESS: Cannot open %s process !!!!!!", szAppName));

		if( pInfo )
			free(pInfo);

		return hToken;
	}

	void EnablePrivilege(HANDLE hToken, LPCSTR szName)
	{
		TOKEN_PRIVILEGES stNewPrivilegesState;
		stNewPrivilegesState.Privileges[ 0 ].Attributes = SE_PRIVILEGE_ENABLED;
		stNewPrivilegesState.PrivilegeCount = 1;
		
		if( !LookupPrivilegeValue(NULL, szName, &stNewPrivilegesState.Privileges[ 0 ].Luid) )
		{
			PR_TRACE((g_root, prtERROR, "PR_PROCESS: LookupPrivilegeValue(%s) failed(%d)", szName, GetLastError()));
			return;
		}
		if( !AdjustTokenPrivileges(hToken, FALSE, &stNewPrivilegesState, 0, NULL, 0) )
		{
			PR_TRACE((g_root, prtERROR, "PR_PROCESS: AdjustTokenPrivileges(%s) failed(%d)", szName, GetLastError()));
			return;
		}
		if (GetLastError() != ERROR_SUCCESS) // != ERROR_NOT_ALL_ASSIGNED
			PR_TRACE((g_root, prtIMPORTANT, "PR_PROCESS: AdjustTokenPrivileges(%s) failed(%d)", szName, GetLastError()));
	}

	void AdjustProcessPrivileges()
	{
		HANDLE hToken = NULL;
		fnOpenProcessToken(GetCurrentProcess(), TOKEN_DUPLICATE|TOKEN_IMPERSONATE|TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES, &hToken);
		if( !hToken )
			return;

		EnablePrivilege(hToken, SE_INCREASE_QUOTA_NAME);
		EnablePrivilege(hToken, SE_SECURITY_NAME);
		EnablePrivilege(hToken, SE_SYSTEM_PROFILE_NAME);
		EnablePrivilege(hToken, SE_DEBUG_NAME);
		EnablePrivilege(hToken, SE_MACHINE_ACCOUNT_NAME);
		EnablePrivilege(hToken, SE_CREATE_TOKEN_NAME);
		EnablePrivilege(hToken, SE_ASSIGNPRIMARYTOKEN_NAME);
		EnablePrivilege(hToken, SE_TCB_NAME);
		EnablePrivilege(hToken, SE_AUDIT_NAME);
		CloseHandle(hToken);
	}

private:
	BOOL   (WINAPI *fnEnumerateSessions)(HANDLE hServer, DWORD Reserved, DWORD Version, PWTS_SESSION_INFO* ppSessionInfo, DWORD* pCount);
	BOOL   (WINAPI *fnEnumerateProcesses)(HANDLE hServer, DWORD Reserved, DWORD Version, PWTS_PROCESS_INFO* ppProcessInfo, DWORD* pCount);
	BOOL   (WINAPI *fnQueryUserToken)(ULONG SessionId, PHANDLE phToken);
	void   (WINAPI *fnFreeMemory)(PVOID pMemory);

	LONG   (WINAPI *fnNtQuerySystemInformation)(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

	BOOL   (WINAPI *fnOpenProcessToken)(HANDLE ProcessHandle,DWORD DesiredAccess, PHANDLE TokenHandle);
	BOOL   (WINAPI *fnImpersonateLoggedOnUser)(HANDLE hToken);
	BOOL   (WINAPI *fnRevertToSelf)();
	BOOL   (WINAPI *fnCreateProcessAsUser)(HANDLE hToken, LPCTSTR lpApplicationName, LPTSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
		BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCTSTR lpCurrentDirectory, LPSTARTUPINFO lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);

	BOOL   (WINAPI *fnCreateEnvironmentBlock)(LPVOID* lpEnvironment, HANDLE hToken, BOOL bInherit);
	BOOL   (WINAPI *fnDestroyEnvironmentBlock)(LPVOID lpEnvironment);

	BOOL    m_bInited;
};

//************************************************************************

#endif // __pr_process_h