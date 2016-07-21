#include "stdafx.h"
#include "jobs.h"

void CurrentTimeToString(CString* pstrtime)
{
	SYSTEMTIME SysTime;
	GetSystemTime(&SysTime);

	pstrtime->Format(_T("%02d-%02d %02d:%02d:%02d"),
		SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
}

#include <tlhelp32.h>
BOOL GetProcessModule (DWORD dwPID, DWORD dwModuleID, LPMODULEENTRY32 lpMe32, DWORD cbMe32) 
{ 
    BOOL          bRet        = FALSE; 
    BOOL          bFound      = FALSE; 
    HANDLE        hModuleSnap = NULL; 
    MODULEENTRY32 me32        = {0}; 
 
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID); 
    if (hModuleSnap == INVALID_HANDLE_VALUE) 
        return (FALSE); 
 
    me32.dwSize = sizeof(MODULEENTRY32); 
 
    if (Module32First(hModuleSnap, &me32)) 
    { 
        do 
        { 
            if (me32.th32ModuleID == dwModuleID) 
            { 
                CopyMemory (lpMe32, &me32, cbMe32); 
                bFound = TRUE; 
            } 
        } 
        while (!bFound && Module32Next(hModuleSnap, &me32)); 
 
        bRet = bFound;
    } 
    else 
        bRet = FALSE;

    CloseHandle (hModuleSnap); 
 
    return (bRet); 
}

//+ ------------------------------------------------------------------------------------------

CProcessList::CProcessList()
{
	m_Modified = _PROCLIST_MODIFIED_NONE;
	m_hDevice = NULL;
	m_AppID_JobFiles = _SYSTEM_APPLICATION;
	m_AppID_Processes = _SYSTEM_APPLICATION;

	if (m_KnownProcessList.GetKnownCount() == 0)
	{
		if (m_bStartOnlyKnownProcesses == true)
		{
			MessageBox(NULL, _T("Known process list == 0. Start unknown process is enabled now"), 
				_T("KLGuard v2. Warning:"), MB_ICONWARNING);
			m_bStartOnlyKnownProcesses = false;
			GS_SaveSettings();
		}
	}
};

CProcessList::~CProcessList()
{
	for (_pProcList::iterator _it = m_ProcList.begin(); _it != m_ProcList.end(); ++_it)
	{
		CSingleProcess* pSingle = *_it;
		delete pSingle;
	}
};

bool CProcessList::StartLoop()
{
	bool bStarted = m_ThNewProcess.StartLoop();
	if (bStarted)
		bStarted = m_ThModifiedProcess.StartLoop();
	if (bStarted)
		bStarted = m_ThCheckSelfCopy.StartLoop();
	if (bStarted)
		bStarted = m_ThCMD_Remote.StartLoop();

	if (bStarted)
		InitList();

	return bStarted;
}

void CProcessList::StopLoop()
{
	m_ThNewProcess.StopLoop();
	m_ThModifiedProcess.StopLoop();
	m_ThCheckSelfCopy.StopLoop();
	m_ThCMD_Remote.StopLoop();

	m_FileLog.AddString(_T("Session stopped\n"));
}

void CProcessList::EnvironmentChanged()
{
	ReloadEnvironment();
}

void CProcessList::TimeoutStep()
{
	CAutoLock auto_lock(&m_Sync);
	for (_pProcList::iterator _it = m_ProcList.begin(); _it != m_ProcList.end(); ++_it)
	{
		CSingleProcess* pSingle = *_it;
		if (pSingle->m_bHasInvisibleWindows)
		{
			pSingle->m_bHasInvisibleWindows = HasVisibleWindow(pSingle->m_ProcessId);
			if (pSingle->m_bHasInvisibleWindows == false)
				SetModifiedFlag(_PROCLIST_MODIFIED_ITEMS);
		}
		else
			CheckWindows(pSingle->m_ProcessId);
	}
}

void CProcessList::GS_SettingsChanged()
{
	static bool bRegisteredInvisible = false;
	if (!bRegisteredInvisible)
	{
		IDriverRegisterInvisibleThread(m_hDevice, m_AppID_Processes);
		bRegisteredInvisible = true;
	}
	
	ActivateProtection(m_bProtectSelf);
	
	ChangeFilterForSystemProcess(m_bDontControlWriteBySystemProcess);

	SetSandBoxPalcement(GetSandBoxSettings()->GetSandBoxPathUpper());
	if (!m_FileLog.InitLog(&m_LogFileName, false))
	{
		//! error - log file not created;
	}
	else
	{
		m_FileLog.AddString(_T("Session started\n"));
	}
}

bool CProcessList::ActivateProtection(bool bActivate)
{
	if (bActivate)
		return !!IDriverProProtRequest(m_hDevice, m_AppID_Processes, TRUE, PROPROT_FLAG_OPEN | PROPROT_FLAG_AUTOACTIVATE);

	return !!IDriverProProtRequest(m_hDevice, m_AppID_Processes, TRUE, PROPROT_FLAG_OPEN);
}

void CProcessList::ChangeFilterForSystemProcess(bool bActivate)
{
	//!
	// enum filters with _PARAM_USER_FILTER_DESCRIPTION and enable/disable it
}

void CProcessList::AddSystemProcessFilter(ULONG ProcessId)
{
	BYTE Buf1[sizeof(FILTER_PARAM) + sizeof(ULONG)];
	BYTE Buf2[sizeof(FILTER_PARAM) + sizeof(ULONG)];
	PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;
	PFILTER_PARAM Param2 = (PFILTER_PARAM) Buf2;

	Param1->m_ParamFlags = _FILTER_PARAM_FLAG_SPECIAL_THISISPROCID;
	Param1->m_ParamFlt = FLT_PARAM_NOP;
	Param1->m_ParamID = _PARAM_OBJECT_SOURCE_ID;
	*(ULONG*) Param1->m_ParamValue = ProcessId;
	Param1->m_ParamSize = sizeof(ULONG);

	Param2->m_ParamFlags = _FILTER_PARAM_FLAG_NONE;
	Param2->m_ParamFlt = FLT_PARAM_NOP;
	Param2->m_ParamID = _PARAM_USER_FILTER_DESCRIPTION;
	*(ULONG*) Param2->m_ParamValue = ProcessId;
	Param2->m_ParamSize = sizeof(ULONG);

	DWORD dwSite = 0;

	DWORD FltFlag = FLT_A_DEFAULT;
	if (false == m_bDontControlWriteBySystemProcess)	// not need
		FltFlag |= FLT_A_DISABLED;

	AddFSFilter2(m_hDevice, m_AppID_JobFiles, "*", DEADLOCKWDOG_TIMEOUT, FltFlag, FLTTYPE_NFIOR, 
			IRP_MJ_WRITE, 0, 0, PreProcessing, &dwSite, Param1, Param2, NULL);
}

void CProcessList::InitList()
{
	CAutoLock auto_lock(&m_Sync);

    HANDLE hProcessSnap = NULL; 
    PROCESSENTRY32 pe32      = {0}; 

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	
    if (hProcessSnap == INVALID_HANDLE_VALUE) 
        return; 
	
    pe32.dwSize = sizeof(PROCESSENTRY32); 

	CSingleProcess *pSingle;
	FILETIME CreationTime, ExitTime, KernelTime, UserTime;
	
    if (Process32First(hProcessSnap, &pe32)) 
    { 
		HANDLE hProcess;
		BOOL bGotModule = FALSE; 
		MODULEENTRY32 me32 = {0}; 
        do 
        {
			if (pe32.th32ProcessID == 0)
				continue;

			hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
			if (hProcess != NULL)
			{
				GetProcessTimes(hProcess, &CreationTime, &ExitTime, &KernelTime, &UserTime);
				CloseHandle(hProcess);
			}
			else
			{
				memset(&CreationTime, 0, sizeof(CreationTime));
				AddSystemProcessFilter(pe32.th32ProcessID);
			}

			bGotModule = GetProcessModule(pe32.th32ProcessID, pe32.th32ModuleID, &me32, sizeof(MODULEENTRY32)); 
			if (!bGotModule)
				bGotModule = GetProcessModule(pe32.th32ProcessID, 1, &me32, sizeof(MODULEENTRY32)); 

			if (!bGotModule)
				pSingle = new CSingleProcess(pe32.szExeFile);
			else
				pSingle = new CSingleProcess(me32.szExePath);

			if (pSingle != NULL)
			{
				pSingle->m_ParentId = pe32.th32ParentProcessID;
				pSingle->m_ProcessId = pe32.th32ProcessID;
				pSingle->m_ProcName = pe32.szExeFile;

				FileTimeToSystemTime(&CreationTime, &pSingle->m_SysTime);

				m_ProcList.insert(m_ProcList.begin(), pSingle);
			}
        } while (Process32Next(hProcessSnap, &pe32));
    } 

	AddSystemProcessFilter(4);	//! for system process
	
    CloseHandle (hProcessSnap); 
	SetModifiedFlag(_PROCLIST_MODIFIED_ITEMS);
}

CSingleProcess* CProcessList::FindProcess(ULONG ProcessId)
{
	for (_pProcList::iterator _it = m_ProcList.begin(); _it != m_ProcList.end(); ++_it)
	{
		CSingleProcess* pSingle = *_it;
		if (pSingle->m_ProcessId == ProcessId && pSingle->m_bAlive)
			return pSingle;
	}

	return NULL;
}

CSingleProcess* CProcessList::FindProcessByTime(LPSYSTEMTIME pSystime)
{
	for (_pProcList::iterator _it = m_ProcList.begin(); _it != m_ProcList.end(); ++_it)
	{
		CSingleProcess* pSingle = *_it;
		if (0 == memcmp(&pSingle->m_SysTime, pSystime, sizeof(SYSTEMTIME)))
			return pSingle;
	}

	return NULL;
}

void CProcessList::UpdateProcessIdByHandle(ULONG ProcessHandle, ULONG ProcessId)
{
	CAutoLock auto_lock(&m_Sync);
	for (_pProcList::iterator _it = m_ProcList.begin(); _it != m_ProcList.end(); ++_it)
	{
		CSingleProcess* pSingle = *_it;
		if (pSingle->m_ProcessHandle == ProcessHandle)
		{
			pSingle->m_ProcessId = ProcessId;
			SetModifiedFlag(_PROCLIST_MODIFIED_ITEMS);
			return;
		}
	}
}

bool CProcessList::AddNewProcess(ULONG ParentProcessId, PCHAR pchProcName, ULONG ProcessId, ULONG ProcHandle, PWCHAR pwchImagePath, PSID pSid)
{
	DeleteProcess(ProcessId);

	CSingleProcess *pSingle = new CSingleProcess(pwchImagePath);
	if (pSingle == NULL)
		return false;

	GetSystemTime(&pSingle->m_SysTime);
	pSingle->m_ParentId = ParentProcessId;
	pSingle->m_ProcessId = ProcessId;
	pSingle->m_ProcessHandle = ProcHandle;

	{
		char msg[MAX_PATH];
		wsprintfA(msg, "new process with pid %d\n", ProcessId);
		OutputDebugStringA(msg);
	}

	CProcessRecognizer* pCmd = m_KnownProcessList.GetCmd();
	pSingle->m_bIsCmd = pCmd->IsEQU(pSingle->GetHash());

	CString newProcName;
	CString strmp = pwchImagePath;
	int pos = strmp.ReverseFind(L'\\');
	if (pos != -1)
		pSingle->m_ProcName = strmp.Right(strmp.GetLength() -  pos - 1);
	else
		pSingle->m_ProcName = pwchImagePath;

	newProcName = pSingle->m_ProcName;
	
	pSingle->m_ParentProcName = pchProcName;

	if (m_SandBoxSettings.IsStartFromBox(pSingle->GetImagePath()))
	{
		pSingle->SetSandBoxed(true);
		pSingle->m_SandBoxFilterId_FileActivity = SandBoxFilters_Add(ProcessId);
	}
	else
	{
		if (m_bEducationMode)
			m_KnownProcessList.RegisterNewProcess(pwchImagePath, pSingle);
	}

	{
		CAutoLock auto_lock(&m_Sync);
		m_ProcList.insert(m_ProcList.begin(), pSingle);
	}

	SetModifiedFlag(_PROCLIST_MODIFIED_ITEMS);

	CString strtmp;
	strtmp.Format(_T("Process '%S' %#x (%d) started new process %s %#x (%d) from '%s'\n"), 
		pchProcName, ParentProcessId, ParentProcessId, 
		newProcName, ProcessId, ProcessId, pwchImagePath);
	m_FileLog.AddString(strtmp);

	return true;
}

void CProcessList::DeleteHistoryFor(ULONG ProcessId, CString* pImagePath)
{
	CAutoLock auto_lock(&m_Sync);
	CSingleProcess* pSingle = FindProcess(ProcessId);
	if (pSingle == NULL)
		return;

	if (*pSingle->GetImagePath() != *pImagePath)
		return;

	pSingle->m_DrvFileHistory.clearall();
	pSingle->m_DrvRegHistory.clearall();
	pSingle->m_DrvSelfHistory.clearall();

	SetModifiedFlag(_PROCLIST_MODIFIED_ITEMS);
}

void CProcessList::DeleteProcess(ULONG ProcessId)
{
	CAutoLock auto_lock(&m_Sync);

	for (_pProcList::iterator _it = m_ProcList.begin(); _it != m_ProcList.end(); ++_it)
	{
		CSingleProcess* pSingle = *_it;
		if (pSingle->m_ProcessId == ProcessId && pSingle->m_bAlive)
		{
//			m_ProcList.erase(_it);

			SandBoxFilters_Delete(pSingle);
//			delete pSingle;
			pSingle->m_bAlive = false;
	
			SetModifiedFlag(_PROCLIST_MODIFIED_ITEMS);
			return;
		}
	}
}

bool CProcessList::IsSandBoxed(ULONG ProcessId)
{
	CAutoLock auto_lock(&m_Sync);

	CSingleProcess* pSingle = FindProcess(ProcessId);
	if (pSingle == NULL)
		return false;

	return pSingle->IsSendBoxed();
}

void CProcessList::CheckTrojanActivity(CSingleProcess* pSingle)
{
	if (!pSingle->m_bSelfCopy || !pSingle->m_bAutorunModified)
		return;

	m_ThCheckSelfCopy.ShowTrojanActivity(pSingle->m_ProcessId, &pSingle->m_ProcName, pSingle->GetImagePath());
}

void CProcessList::PushAlert_TrojanActivity(ULONG ProcessId, CString *pProcName, CString* pImagePath)
{
	CString strtmp;
	strtmp.Format(_T("Bad activity: Self copy and modify autorun registry.\n\tStarted from:'%s'\n\tProcess %s %#x (%d)"),
		 *pImagePath, *pProcName, ProcessId, ProcessId);

	m_FileLog.AddString(strtmp);
	GlobalAlert();

	::MessageBox(NULL, strtmp, _T("KLGuard2: Malware detected!"), MB_ICONWARNING);
}

void CProcessList::RegistryActivity(ULONG ProcessId, PWCHAR pwchRegUrl, bool bAutoRun)
{
	CAutoLock auto_lock(&m_Sync);

	CSingleProcess* pSingle = FindProcess(ProcessId);
	if (pSingle == NULL)
		return;

	if (bAutoRun)
	{
		pSingle->m_bAutorunModified = true;
		CheckTrojanActivity(pSingle);
	}

	SetModifiedFlag(_PROCLIST_MODIFIED_PARAMS);

	CString strtime;
	CurrentTimeToString(&strtime);
	
	CString strtmp;
	strtmp.Format(_T("%s Registry: modify '%s'"), strtime, pwchRegUrl);

	pSingle->m_DrvRegHistory.push_back(strtmp);
	
	strtmp.Format(_T("Process %#x (%d)modify registry value '%s'\n"), ProcessId, ProcessId, pwchRegUrl);
	m_FileLog.AddString(strtmp);

	GlobalAlert();
}

void CProcessList::SysFileActivity(ULONG ProcessId, PWCHAR pwchFileName)
{
	if (IsFolderPrefetch(pwchFileName))
		return;

	CString strtmp;
	{
		CAutoLock auto_lock(&m_Sync);

		CSingleProcess* pSingle = FindProcess(ProcessId);
		if (pSingle == NULL)
			return;

		SetModifiedFlag(_PROCLIST_MODIFIED_PARAMS);

		CString strtime;
		CurrentTimeToString(&strtime);
		
		strtmp.Format(_T("%s modify file '%s'"), strtime, pwchFileName);

		pSingle->m_DrvFileHistory.push_back(strtmp);

		strtmp.Format(_T("Process %s %#x (%d) modified file '%s'\n"), 
			pSingle->m_ProcName, ProcessId, ProcessId, pwchFileName);
	}

	m_FileLog.AddString(strtmp);
}

void CProcessList::CheckSelfCopy(ULONG ProcessId, PWCHAR pwchFileName)
{
	CAutoLock auto_lock(&m_Sync);
	
	CSingleProcess* pSingle = FindProcess(ProcessId);
	if (pSingle == NULL)
	{
		DbPrint(DCB_NOCAT, DL_WARNING, (_T("CheckSelfCopy - process not found. Pid %d"), ProcessId));
		return;
	}

	if (pSingle->m_bIsCmd)
		pSingle = FindProcess(pSingle->m_ParentId);
	
	if (pSingle == NULL)
	{
		DbPrint(DCB_NOCAT, DL_WARNING, (_T("CheckSelfCopy - was cmd: process not found. Pid %d"), ProcessId));
		return;
	}

	if (!pSingle->m_bWasSelfOpen)
	{
		//return;
		DbPrint(DCB_NOCAT, DL_WARNING, (_T("CheckSelfCopy - process not opened self. Pid %d"), ProcessId));
	}

	AddTask_CheckSelfCopy(ProcessId, CHashContainer(pSingle->GetHash(), pSingle->GetObjectSize()), &pSingle->m_ProcName, pSingle->GetImagePath(), pwchFileName);
}

void CProcessList::AddTask_CheckSelfCopy(ULONG ProcessId, CHashContainer& HashC, CString *pProcName, CString* pImagePath, PWCHAR pwchNewFileName)
{
	m_ThCheckSelfCopy.CheckSelfCopy(ProcessId, HashC, pProcName, pImagePath, pwchNewFileName);
}

bool CProcessList::IsAllowedFileActivity(ULONG ProcessId, PWCHAR pwchFileName)
{
	bool bRet = false;

	_eFilePlacement file_placement = GetFilePlacement(pwchFileName);

	switch (file_placement)
	{
	case _placement_other:
		if (lstrlen(pwchFileName) == 0)	//!
			bRet = true;
		break;
	case _placement_system_folder:
		if (m_SandBoxSettings.m_bAllowWrite_SystemFolder)
			bRet = true;
		break;
	case _placement_program_files:
		if (m_SandBoxSettings.m_bAllowWrite_ProgramFiles)
			bRet = true;
		break;
	case _placement_temp:
		if (m_SandBoxSettings.m_bAllowWrite_TempFolder)
			bRet = true;
		break;
	case _placement_sandbox:
		if (m_SandBoxSettings.m_bAllowWrite_SandBoxFolder)
			bRet = true;
		break;
	}

	return bRet;
}

void CProcessList::CheckSelfOpen(ULONG ProcessId, PWCHAR pwchFileName)
{
	CAutoLock auto_lock(&m_Sync);

	CSingleProcess* pSingle = FindProcess(ProcessId);
	if (pSingle == NULL)
		return;

	if (*pSingle->GetImagePath() != pwchFileName)
		return;

	pSingle->m_bWasSelfOpen = true;
	SetModifiedFlag(_PROCLIST_MODIFIED_PARAMS);

	CString strtime;
	CurrentTimeToString(&strtime);

	CString strtmp;
	strtmp.Format(_T("%s Self open"), strtime);

	pSingle->m_DrvSelfHistory.push_back(strtmp);
	CheckWindows(ProcessId);
}

bool CProcessList::IsExistPermisionForCreateProcessing(ULONG CreatorProcessId)
{
	CheckWindows(CreatorProcessId);

	if (!m_SandBoxSettings.m_bAllowCreateProcess)
	{
		CAutoLock auto_lock(&m_Sync);
		CSingleProcess* pSingle = FindProcess(CreatorProcessId);
		if (pSingle == NULL)
			return true;

		if (pSingle->IsSendBoxed())
			return false;
	}
	return true;
}

bool CProcessList::IsAllowedStart_Modified(PWCHAR pwchImagePath)
{
	return m_ThModifiedProcess.IsAllowStart(pwchImagePath);
}

void CProcessList::PushAlertMessage_CreateProcess(ULONG CreatorProcessId, PWCHAR pwchImagePath)
{
	if (!m_SandBoxSettings.m_bShowWarning)
		return;

	CAutoLock auto_lock(&m_Sync);
	CSingleProcess* pSingle = FindProcess(CreatorProcessId);
	if (pSingle == NULL)
	{
		//! very strange!
		return;
	}

	CString strtime;
	CurrentTimeToString(&strtime);

	CString strtmp;
	strtmp.Format(_T("%s process %#x (%d) started from '%s' is trying to start process from '%s'. Operation is disabled."), 
		strtime, CreatorProcessId, CreatorProcessId, *pSingle->GetImagePath(), pwchImagePath);
	
	m_FileLog.AddString(strtmp + _T("\n"), false);

	m_JobSandBoxWarnings.PushEvent(&strtmp);
}

void CProcessList::PushAlertMessage_NotAllowedFileActivity(ULONG CreatorProcessId, PCHAR pchProcName, PWCHAR pwchFileName)
{
	CString strtmp;
	
	{
		CAutoLock auto_lock(&m_Sync);
		CSingleProcess* pSingle = FindProcess(CreatorProcessId);

		if (pSingle == NULL)
		{
			strtmp.Format(_T("Process %S %#x (%d) trying to modify file '%s'. Operation disabled\n"),
				pchProcName, CreatorProcessId, CreatorProcessId, pwchFileName);
		}
		else
			strtmp.Format(_T("Process %s %#x (%d) started from ('%s') trying to modify file '%s'. Operation disabled\n"),
				pSingle->m_ProcName, CreatorProcessId, CreatorProcessId, *pSingle->GetImagePath(), pwchFileName);
	}
	m_FileLog.AddString(strtmp);
}

void CProcessList::PushAlertMessage_ChangeAutoRunKey(ULONG ProcessId, PCHAR pchProcName, PWCHAR pwchRegistryKeyName)
{
	CString strtmp;
	
	{
		CAutoLock auto_lock(&m_Sync);
		CSingleProcess* pSingle = FindProcess(ProcessId);

		if (pSingle == NULL)
		{
			strtmp.Format(_T("Process %S %#x (%d) trying to change autorun key value '%s'. Operation disabled\n"),
				pchProcName, ProcessId, ProcessId, pwchRegistryKeyName);
		}
		else
			strtmp.Format(_T("Process %s %#x (%d) started from ('%s') trying to change autorun key value '%s'. Operation disabled\n"),
				pSingle->m_ProcName, ProcessId, ProcessId, *pSingle->GetImagePath(), pwchRegistryKeyName);
	}
	m_FileLog.AddString(strtmp);
}

void CProcessList::PushAlert_SelfCopy(ULONG ProcessId, CString *pProcName, CString* pImagePath, CString* pNewFileName)
{
	CString strtmp;

	strtmp.Format(_T("process %s %#x (%d) started from '%s' copied self to '%s'\n"), 
				*pProcName, ProcessId, ProcessId, *pImagePath, *pNewFileName);

	m_FileLog.AddString(strtmp);

	{
		CAutoLock auto_lock(&m_Sync);
		CSingleProcess* pSingle = FindProcess(ProcessId);
		if (pSingle != NULL)
		{
			pSingle->m_DrvFileHistory.push_back(strtmp);
			pSingle->m_bSelfCopy = true;
			CheckTrojanActivity(pSingle);
		}
	}

	::MessageBox(NULL, strtmp, _T("KLGuard2: Selfcopy detected!"), MB_ICONWARNING);
}

void CProcessList::PushAlertMessage_WriteToRegistry(ULONG ProcessId, PWCHAR pwchRegistryKeyName)
{
	CString strtmp;

	CString strtime;
	CurrentTimeToString(&strtime);

	{
		CAutoLock auto_lock(&m_Sync);
		CSingleProcess* pSingle = FindProcess(ProcessId);
		if (pSingle == NULL)
		{
			strtmp.Format(_T("%s process %#x (%d) modified registry: '%s'. Operation is disabled."), 
				strtime, ProcessId, ProcessId, pwchRegistryKeyName);

		}
		else
			strtmp.Format(_T("%s process %#x (%d) started from '%s' is trying to write in registry: '%s'. Operation is disabled."), 
				strtime, ProcessId, ProcessId, *pSingle->GetImagePath(), pwchRegistryKeyName);
	}
	
	m_FileLog.AddString(strtmp + _T("\n"), false);

	if (m_SandBoxSettings.m_bShowWarning)
		m_JobSandBoxWarnings.PushEvent(&strtmp);
}

_eStartNewProcess_Verdict CProcessList::PushAlertMessage_NewProcess(ULONG CreatorProcessId, PWCHAR pwchImagePath, 
																	CProcessRecognizer* pRecognizer)
{
	CString strtmp;
	bool bSanBoxed = false;
	{
		CAutoLock auto_lock(&m_Sync);
		CSingleProcess* pSingle = FindProcess(CreatorProcessId);

		CString strtime;
		CurrentTimeToString(&strtime);

		if (pSingle != 0)
		{
			strtmp.Format(_T("%s process %#x (%d) '%s' is starting unknow process\n"), 
				strtime, CreatorProcessId, CreatorProcessId, pSingle->m_ProcName);
		}
		else
		{
			strtmp.Format(_T("%s process %#x (%d) <unknown> is starting unknow process\n"), 
				strtime, CreatorProcessId, CreatorProcessId);
		}

		m_FileLog.AddString(strtmp, false);
	}

	strtmp = pwchImagePath;
	bool bIsStartFromSandBox = m_SandBoxSettings.IsStartFromBox(&strtmp);
	_eStartNewProcess_Verdict start_verdict = m_ThNewProcess.IsAllowStart(&strtmp, pwchImagePath, bIsStartFromSandBox);

	if (start_verdict == _start_allowed)
	{
		if (!bIsStartFromSandBox)
			m_KnownProcessList.RegisterNewProcess(pwchImagePath, pRecognizer);
	}

	return start_verdict;
}

void CProcessList::PushAlert_CMD_Remote(ULONG ProcessId, PWCHAR pwchCmdLine)
{
	CAutoLock auto_lock(&m_Sync);
	CSingleProcess* pSingle = FindProcess(ProcessId);

	CString strtmp;
	CString strtime;
	CurrentTimeToString(&strtime);

	if (pSingle == 0 || !pSingle->m_bIsCmd)
		return;

	CSingleProcess* pSingleTmp = FindProcess(pSingle->m_ParentId);
	if (pSingleTmp != NULL)
			pSingle = pSingleTmp;

	strtmp.Format(_T("%s Alert! Process %#x (%d) '%s' (started form '%s') started process cmd with redirected input and output!\nCommand line '%s'"), 
		strtime, ProcessId, ProcessId, pSingle->m_ProcName, *pSingle->GetImagePath(), pwchCmdLine);

	strtmp.Replace(_T("\t"), _T(" "));
	m_FileLog.AddString(strtmp + _T("\n"), false);

	m_ThCMD_Remote.ShowWarning(&strtmp);
	GlobalAlert();
}

void CProcessList::AlertInvisilbe(ULONG ProcessId)
{
	CSingleProcess* pSingle = FindProcess(ProcessId);
	if (pSingle == NULL)
		return;

	pSingle->m_bHasInvisibleWindows = true;

	//!
	GlobalAlert();
}

ULONG CProcessList::SandBoxFilters_Add(ULONG ProcessId)
{
	BYTE Buf1[sizeof(FILTER_PARAM) + sizeof(ULONG)];
	PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;

	Param1->m_ParamFlags = _FILTER_PARAM_FLAG_SPECIAL_THISISPROCID;
	Param1->m_ParamFlt = FLT_PARAM_NOP;
	Param1->m_ParamID = _PARAM_OBJECT_SOURCE_ID;
	*(ULONG*) Param1->m_ParamValue = ProcessId;
	Param1->m_ParamSize = sizeof(ULONG);

	DWORD dwSite = 0;

	return AddFSFilter2(m_hDevice, m_AppID_JobFiles, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP, FLTTYPE_NFIOR, 
			IRP_MJ_WRITE, 0, 0, PreProcessing, &dwSite, Param1, NULL);
}

bool CProcessList::SandBoxFilters_Delete(CSingleProcess* pSingle)
{
	if (pSingle->m_SandBoxFilterId_FileActivity == 0)
		return true;

	FILTER_TRANSMIT FilterTransmit;
	FilterTransmit.m_AppID = m_AppID_JobFiles;
	FilterTransmit.m_FltCtl = FLTCTL_DEL;
	FilterTransmit.m_FilterID = pSingle->m_SandBoxFilterId_FileActivity;
	
	return !!IDriverFilterTransmit(m_hDevice, &FilterTransmit, &FilterTransmit, sizeof(FilterTransmit), sizeof(FilterTransmit));
}

//+ ------------------------------------------------------------------------------------------
	bool Job_Process::AddFilters()
{
	bool bRet = true;

	if (!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP, FLTTYPE_SYSTEM, 
			MJ_SYSTEM_CREATE_PROCESS, 0, 0, AnyProcessing, NULL, NULL)
		||
		!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP, FLTTYPE_SYSTEM, 
			MJ_SYSTEM_CREATE_PROCESS_EX, 0, 0, AnyProcessing, NULL, NULL)
		||
		!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO, FLTTYPE_SYSTEM, 
			MJ_EXIT_PROCESS, 0, 0, PostProcessing, NULL, NULL)
//		||
//		!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO, FLTTYPE_SYSTEM, 
//			MJ_SYSTEM_TERMINATE_PROCESS, 0, 0, PostProcessing, NULL, NULL)

		||
		!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO | FLT_A_DELETE_ON_MARK, FLTTYPE_SYSTEM, 
			MJ_CREATE_PROCESS_NOTIFY_EXTEND, 0, 0, PreProcessing, NULL, NULL))
	{
		ResetFilters();
		return false;
	}

	AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP, FLTTYPE_SYSTEM, 
		MJ_SYSTEM_CREATE_THREAD, 0, 0, PostProcessing, NULL, NULL);

	return true;
}

void Job_Process::BeforeExit()
{
	IDriverProProtRequest(m_hDevice, m_AppID, FALSE, PROPROT_FLAG_OPEN);
}


void Job_Process::SingleEvent()
{
	VERDICT Verdict = Verdict_Default;
	BYTE* pEventData = NULL;
	ULONG Mark;

	if (GetEventData(&m_DrvEventList, m_hDevice, m_AppID, &pEventData, &Mark))
	{
		PEVENT_TRANSMIT pEvt;

		PSINGLE_PARAM pParamProcId;
		PSINGLE_PARAM pParamParentProcId;
		PSINGLE_PARAM pParamImagePath;
		PSINGLE_PARAM pParamProcHandle;
		PSINGLE_PARAM pParamSid;

		pEvt = (PEVENT_TRANSMIT) pEventData;
		switch (pEvt->m_FunctionMj)
		{
		case MJ_SYSTEM_CREATE_PROCESS:
		case MJ_SYSTEM_CREATE_PROCESS_EX:
			{
				pParamParentProcId = IDriverGetEventParam(pEvt, _PARAM_OBJECT_SOURCE_ID);
				pParamImagePath = IDriverGetEventParam(pEvt, _PARAM_OBJECT_URL_W);
				pParamProcHandle = IDriverGetEventParam(pEvt, _PARAM_OBJECT_OBJECTHANDLE);
				
				if (pParamImagePath->ParamSize == 0 || pParamProcHandle == 0)
					break;

				if (pEvt->m_EventFlags & _EVENT_FLAG_POSTPROCESSING)
				{
					pParamProcId = IDriverGetEventParam(pEvt, _PARAM_OBJECT_DEST_ID);
					pParamSid = IDriverGetEventParam(pEvt, _PARAM_OBJECT_SID);
					
					ULONG ProcID = pParamProcId == NULL ? (ULONG) -1 : *(ULONG*)pParamProcId->ParamValue;
					
					IDriverYieldEvent(m_hDevice, m_AppID, Mark, 5);
					
					gpProcList->AddNewProcess(*(ULONG*)pParamParentProcId->ParamValue, pEvt->m_ProcName, 
						ProcID, *(ULONG*)pParamProcHandle->ParamValue, (PWCHAR) pParamImagePath->ParamValue, (PSID) pParamSid->ParamValue);
				}
				else
				{
					if (!gpProcList->IsExistPermisionForCreateProcessing(*(ULONG*)pParamParentProcId->ParamValue))
					{
						Verdict = Verdict_Discard;
						gpProcList->PushAlertMessage_CreateProcess(*(ULONG*)pParamParentProcId->ParamValue, 
							(PWCHAR) pParamImagePath->ParamValue);
					}
					else
					{
						CProcessRecognizer recognizer((PWCHAR) pParamImagePath->ParamValue);

						bool bKnown = gpProcList->m_KnownProcessList.IsKnownProcess(&recognizer);
						if (!bKnown && gpProcList->m_KnownProcessList.ExistByName((PWCHAR) pParamImagePath->ParamValue))
						{
							IDriverYieldEvent(m_hDevice, m_AppID, Mark, _TIMEOUT_SHOWMODIFIED_WND);
							if (gpProcList->IsAllowedStart_Modified((PWCHAR) pParamImagePath->ParamValue))
							{
								gpProcList->m_KnownProcessList.RegisterNewProcess((PWCHAR) pParamImagePath->ParamValue, &recognizer);
								bKnown = true;
							}
							else
								Verdict = Verdict_Discard;
						}

						if (gpProcList->GetGenericSettings()->m_bStartOnlyKnownProcesses)
						{
							if (!bKnown)
								Verdict = Verdict_Discard;
						}
						else
						{
							if (!bKnown && Verdict != Verdict_Discard)
							{
								if (!gpProcList->GetGenericSettings()->m_bEducationMode)
								{
									IDriverYieldEvent(m_hDevice, m_AppID, Mark, 300);
									switch(gpProcList->PushAlertMessage_NewProcess(*(ULONG*)pParamParentProcId->ParamValue, 
										(PWCHAR) pParamImagePath->ParamValue, &recognizer))
									{
									case _start_disabled:
										Verdict = Verdict_Discard;
										break;
									case _start_allowed_once:
									case _start_allowed:
										break;
									}
								}
							}
						}
					}
				}
			}
			break;
		case MJ_EXIT_PROCESS:
			{
				pParamProcId = IDriverGetEventParam(pEvt, _PARAM_OBJECT_SOURCE_ID);
				ULONG ProcID = pParamProcId == NULL ? (ULONG) -1 : *(ULONG*)pParamProcId->ParamValue;
				
				gpProcList->DeleteProcess(ProcID);
			}
			break;
		case MJ_SYSTEM_TERMINATE_PROCESS:
			{
				pParamProcId = IDriverGetEventParam(pEvt, _PARAM_OBJECT_DEST_ID);
				ULONG ProcID = pParamProcId == NULL ? (ULONG) -1 : *(ULONG*)pParamProcId->ParamValue;
				
				gpProcList->DeleteProcess(ProcID);
			}
			break;
		case MJ_CREATE_PROCESS_NOTIFY_EXTEND:
			{
				PSINGLE_PARAM pContext = IDriverGetEventParam(pEvt, _PARAM_OBJECT_CONTEXT_FLAGS);
				PSINGLE_PARAM pComandLine = IDriverGetEventParam(pEvt, _PARAM_OBJECT_URL_PARAM_W);
				if (pContext != NULL && pComandLine != NULL)
				{
					if (*(ULONG*)pContext->ParamValue & (CREATE_PROCESS_NOTIFY_FLAG_STDIN_REMOTE | CREATE_PROCESS_NOTIFY_FLAG_STDOUT_REMOTE))
					{
						gpProcList->PushAlert_CMD_Remote(pEvt->m_ProcessID, (PWCHAR)pComandLine->ParamValue);
					}
				}
			}
			break;
		case MJ_SYSTEM_CREATE_THREAD:
			{
				PSINGLE_PARAM pParamProcHandle = IDriverGetEventParam(pEvt, _PARAM_OBJECT_OBJECTHANDLE);
				PSINGLE_PARAM pParamProcId = IDriverGetEventParam(pEvt, _PARAM_OBJECT_CLIENTID1);
				if (pParamProcHandle != NULL && pParamProcId != NULL)
				{
					if (*(ULONG*)pParamProcHandle->ParamValue != (ULONG)-1)
					{
						gpProcList->UpdateProcessIdByHandle(*(ULONG*)pParamProcHandle->ParamValue, *(ULONG*)pParamProcId->ParamValue);
					}
				}
			}
			break;
		default:
			break;
		}

		if (Verdict == Verdict_Discard || Verdict == Verdict_Kill)
			GlobalAlert();

		if (!(pEvt->m_EventFlags & _EVENT_FLAG_DELETED_ON_MARK))
			IDriverSetVerdict(m_hDevice, m_AppID, Verdict, Mark, 0);

		m_DrvEventList.Free(pEventData);
	}
}

//+ ------------------------------------------------------------------------------------------
#define _REG_MAXPATHLEN	MAXPATHLEN + 30

bool Job_Registry::AddFilters()
{
	bool bRet = true;

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	if (!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_POPUP, FLTTYPE_REGS, 
			Interceptor_SetValueKey, 0, 0, PreProcessing, NULL, NULL))
	{
		ResetFilters();
		return false;
	}
	
	return true;
}

void Job_Registry::SingleEvent()
{
	VERDICT Verdict = Verdict_Default;
	BYTE* pEventData = NULL;
	ULONG Mark;

	if (GetEventData(&m_DrvEventList, m_hDevice, m_AppID, &pEventData, &Mark))
	{
		PEVENT_TRANSMIT pEvt;

		PSINGLE_PARAM pParamUrl;

		pEvt = (PEVENT_TRANSMIT) pEventData;
		switch (pEvt->m_FunctionMj)
		{
		case Interceptor_SetValueKey:
			{
				pParamUrl = IDriverGetEventParam(pEvt, _PARAM_OBJECT_URL);

				CString strtmp = (PCHAR)pParamUrl->ParamValue;
				CString keyname = strtmp;
				keyname.MakeUpper();
				unsigned long len = (keyname.GetLength() + 1) * sizeof(WCHAR);
				LPTSTR keyname_buff = keyname.GetBuffer(0);

				if (gpProcList->IsSandBoxed(pEvt->m_ProcessID))
				{
					if (!gpProcList->GetSandBoxSettings()->m_bAllowWrite_Registry)
					{
						Verdict = Verdict_Discard;
						gpProcList->PushAlertMessage_WriteToRegistry(pEvt->m_ProcessID, keyname_buff);
					}
				}

				if (IsAutoRunsKey(keyname_buff, len))
				{
					LPTSTR strtmp_buff = strtmp.GetBuffer(0);
					if (gpProcList->GetGenericSettings()->m_bProtectAutoRun)
					{
						gpProcList->PushAlertMessage_ChangeAutoRunKey(pEvt->m_ProcessID, pEvt->m_ProcName, strtmp_buff);
						Verdict = Verdict_Discard;
					}
					else
						gpProcList->RegistryActivity(pEvt->m_ProcessID, strtmp_buff, true);

					strtmp.ReleaseBuffer();
				}
				else if (IsShellExKey(keyname_buff, len))
				{
					LPTSTR strtmp_buff = strtmp.GetBuffer(0);
					gpProcList->RegistryActivity(pEvt->m_ProcessID, strtmp_buff, false);
					strtmp.ReleaseBuffer();
				}
				
				keyname.ReleaseBuffer();
			}
			break;
		default:
			break;
		}

		if (Verdict == Verdict_Discard || Verdict == Verdict_Kill)
			GlobalAlert();

		IDriverSetVerdict(m_hDevice, m_AppID, Verdict, Mark, 0);
		m_DrvEventList.Free(pEventData);
	}
}

//+ ------------------------------------------------------------------------------------------
void Job_Files::AddInvisibleFile(PTCHAR ptchFileName)
{
	DWORD dwSite = 0;
	AddFSFilterW(m_hDevice, m_AppID, ptchFileName, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_DEFAULT, FLTTYPE_NFIOR, IRP_MJ_WRITE, 0, 0, PreProcessing, &dwSite);
}

bool Job_Files::AddFilters()
{
	bool bRet = true;

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

#define _env_length 0x200
	BYTE Buf1[sizeof(FILTER_PARAM) + _env_length];
	PFILTER_PARAM Param1 = (PFILTER_PARAM) Buf1;

	//+ ------------------------------------------------------------------------------------------
	//+ first filter (iopen file)

	Param1->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST | _FILTER_PARAM_FLAG_CACHABLE;
	Param1->m_ParamFlt = FLT_PARAM_WILDCARD;
	Param1->m_ParamID = _PARAM_OBJECT_URL_W;
	GetEnvironmentVariable(_T("windir"), (PWCHAR) Param1->m_ParamValue, _env_length / sizeof(WCHAR));
	lstrcat((PWCHAR) Param1->m_ParamValue, _T("*"));

	Param1->m_ParamSize = (lstrlen((PWCHAR)Param1->m_ParamValue) + 1 ) * sizeof(WCHAR);

	if (!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO, FLTTYPE_NFIOR, 
			IRP_MJ_WRITE, 0, 0, PreProcessing, NULL, Param1, NULL))
	{
		ResetFilters();
		return false;
	}
	
	//+ ------------------------------------------------------------------------------------------
	//+ second filter (cleanup for modified files)
	
	Param1->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST | _FILTER_PARAM_FLAG_INVERS_OP;
	Param1->m_ParamFlt = FLT_PARAM_WILDCARD;
	Param1->m_ParamID = _PARAM_OBJECT_URL_W;
	lstrcpy((PWCHAR) Param1->m_ParamValue, _T("*\\"));
	Param1->m_ParamSize = (lstrlen((PWCHAR)Param1->m_ParamValue) + 1 ) * sizeof(WCHAR);

	BYTE Buf2[sizeof(FILTER_PARAM) + sizeof(DWORD)];
	PFILTER_PARAM Param2 = (PFILTER_PARAM) Buf2;

	Param2->m_ParamFlags = _FILTER_PARAM_FLAG_MUSTEXIST;
	Param2->m_ParamFlt = FLT_PARAM_AND;
	Param2->m_ParamID = _PARAM_OBJECT_CONTEXT_FLAGS;
	Param2->m_ParamSize = sizeof(DWORD);
	*(DWORD*)Param2->m_ParamValue = _CONTEXT_OBJECT_FLAG_MODIFIED;

	if (!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO, FLTTYPE_NFIOR, 
			IRP_MJ_CLEANUP, 0, 0, PostProcessing, NULL, Param2, Param1, NULL))
	{
		ResetFilters();
		return false;
	}

	//- end setting job filters
	//- ------------------------------------------------------------------------------------------


	CUncontrolledFiles UncontrolledFiles;
	{
		_StrVector::iterator _it;

		CString *pstrtmp;
		for (_it = UncontrolledFiles.m_UncotrolledFileList.begin(); _it != UncontrolledFiles.m_UncotrolledFileList.end(); ++_it)
		{
			pstrtmp = _it;
			AddInvisibleFile(pstrtmp->GetBuffer(0));
			pstrtmp->ReleaseBuffer();
		}
	}

	AddInvisibleFile(_T("*:KAVICHS"));
	
	return true;
}

void Job_Files::SingleEvent()
{
	VERDICT Verdict = Verdict_Default;
	BYTE* pEventData = NULL;
	ULONG Mark;

	if (GetEventData(&m_DrvEventList, m_hDevice, m_AppID, &pEventData, &Mark))
	{
		PEVENT_TRANSMIT pEvt;

		PSINGLE_PARAM pParamUrl;

		pEvt = (PEVENT_TRANSMIT) pEventData;

		pParamUrl = IDriverGetEventParam(pEvt, _PARAM_OBJECT_URL_W);

		if(pEvt->m_EventFlags &_EVENT_FLAG_POPUP)
		{
			if (!gpProcList->IsAllowedFileActivity(pEvt->m_ProcessID, (PWCHAR)pParamUrl->ParamValue))
			{
				Verdict = Verdict_Discard;
				gpProcList->PushAlertMessage_NotAllowedFileActivity(pEvt->m_ProcessID, pEvt->m_ProcName, 
					(PWCHAR)pParamUrl->ParamValue);
			}
		}
		else
		{
			if (pEvt->m_FunctionMj == IRP_MJ_WRITE)
				gpProcList->SysFileActivity(pEvt->m_ProcessID, (PWCHAR)pParamUrl->ParamValue);
			else
				gpProcList->CheckSelfCopy(pEvt->m_ProcessID, (PWCHAR)pParamUrl->ParamValue);
		}

		m_DrvEventList.Free(pEventData);

		if (Verdict == Verdict_Discard || Verdict == Verdict_Kill)
			GlobalAlert();

		IDriverSetVerdict(m_hDevice, m_AppID, Verdict, Mark, 0);
	}
}

//+ ------------------------------------------------------------------------------------------

bool Job_Self::AddFilters()
{
	bool bRet = true;

	if (!AddFSFilter2(m_hDevice, m_AppID, "*", DEADLOCKWDOG_TIMEOUT, FLT_A_INFO, FLTTYPE_NFIOR, 
			IRP_MJ_CREATE, 0, 0, PreProcessing, NULL, NULL))
	{
		ResetFilters();
		return false;
	}

	return true;
}

void Job_Self::SingleEvent()
{
	VERDICT Verdict = Verdict_Default;
	BYTE* pEventData = NULL;
	ULONG Mark;

	if (GetEventData(&m_DrvEventList, m_hDevice, m_AppID, &pEventData, &Mark))
	{
		PEVENT_TRANSMIT pEvt;

		PSINGLE_PARAM pParamUrl;

		pEvt = (PEVENT_TRANSMIT) pEventData;

		pParamUrl = IDriverGetEventParam(pEvt, _PARAM_OBJECT_URL_W);

		gpProcList->CheckSelfOpen(pEvt->m_ProcessID, (PWCHAR)pParamUrl->ParamValue);

		if (Verdict == Verdict_Discard || Verdict == Verdict_Kill)
			GlobalAlert();

		IDriverSetVerdict(m_hDevice, m_AppID, Verdict, Mark, 0);

		m_DrvEventList.Free(pEventData);
	}
}