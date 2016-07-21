#include "proclist.h"

CSingleProcess::CSingleProcess(CProcessList* pProcessList, ULONG ProcessId, ULONG ProcessHandle, PWCHAR pwchImagePath, ULONG ParentProcessId, CLog* pParentLog)
{
	int len;

	m_ProcessId = ProcessId;
	m_ProcessHandle = ProcessHandle;
	m_ParentProcessId = ParentProcessId;

	GetSystemTime(&m_SysTime);
	m_pwchImagePath = NULL;
	m_pProcessList = pProcessList;
	m_nLockCount = 0;

	PWCHAR pwchLogName;
	len = _WSTR_LEN_B(pwchImagePath);
	m_pwchImagePath = (PWCHAR) CopyBufToNewBlock(pwchImagePath, len);
	if (NULL == m_pwchImagePath)
	{
		DbPrint((DCB_SYSTEM, DL_ERROR, _T("CSingleProcess: can't allocate memory for ImagePath")));
		m_pwchImagePath = L"<NO MEMORY>";
	}
	pwchLogName = (PWCHAR) CopyBufToNewBlock(pwchImagePath, len+100);
	if (NULL == pwchLogName)
	{
		DbPrint((DCB_SYSTEM, DL_ERROR, _T("CSingleProcess: can't allocate memory for LogName")));
	}
	else
	{
		lstrcat(pwchLogName, _T(".det_act.log"));
		m_Log.InitLog(pwchLogName, pParentLog);
		FreeStr(pwchLogName);
	}
};

CSingleProcess::~CSingleProcess()
{
	if (m_nLockCount)
	{
		DbPrint((DCB_SYSTEM, DL_ERROR, _T("CSingleProcess: !!! destructor has locked item!!!")));
	}
	while (m_nLockCount)
	{
		m_pProcessList->m_Sync.UnLockSync();
		InterlockedDecrement(&m_nLockCount);
	}
	FreeStr(m_pwchImagePath);
	FreeStr(m_pwchCmdLine);
};

long CSingleProcess::AddRef()
{
	m_pProcessList->m_Sync.LockSync();
	return InterlockedIncrement(&m_nLockCount);
}

long CSingleProcess::Release()
{
	long count;
	m_pProcessList->m_Sync.UnLockSync();
	count = InterlockedDecrement(&m_nLockCount);
	if (count < 0)		
	{
		DbPrint((DCB_SYSTEM, DL_ERROR, _T("CSingleProcess: Release resulted negative count!!!")));
		count = InterlockedIncrement(&m_nLockCount);
	}
	return count;
}


CSingleProcess* CSingleProcess::GetParent()
{
	if (m_pProcessList == NULL)
		return NULL;
	return m_pProcessList->FindProcess(m_ParentProcessId);
}

//+ ------------------------------------------------------------------------------------------

CProcessList::~CProcessList()
{
	CAutoLock auto_lock(m_Sync);

	for (_pProcList::iterator _it = m_ProcList.begin(); _it != m_ProcList.end(); _it++)
	{
		CSingleProcess* pProcess = *_it;
		delete pProcess;
	}
}

CSingleProcess* CProcessList::FindProcess(ULONG ProcessId)
{
	CAutoLock auto_lock(m_Sync);

	for (_pProcList::iterator _it = m_ProcList.begin(); _it != m_ProcList.end(); _it++)
	{
		CSingleProcess* pProcess = *_it;
		if (pProcess->m_ProcessId == ProcessId)
		{
			pProcess->AddRef();
			return pProcess;
		}
	}

	return NULL;
}


void CProcessList::DeleteProcess(ULONG ProcessId)
{
	CAutoLock auto_lock(m_Sync);

	for (_pProcList::iterator _it = m_ProcList.begin(); _it != m_ProcList.end(); _it++)
	{
		CSingleProcess* pProcess = *_it;
		if (pProcess->m_ProcessId == ProcessId)
		{
			m_ProcList.erase(_it);

			DbPrint((DCB_DRIVER, DL_INFO, _T("Process exit '%s'"), pProcess->m_pwchImagePath));

			delete pProcess;
			return;
		}
	}
}

void CProcessList::UpdateProcessIdByHandle(ULONG ProcessHandle, ULONG ProcessId)
{
	CAutoLock auto_lock(m_Sync);
	for (_pProcList::iterator _it = m_ProcList.begin(); _it != m_ProcList.end(); _it++)
	{
		CSingleProcess* pProcess = *_it;
		if (pProcess->m_ProcessHandle == ProcessHandle)
		{
			pProcess->m_ProcessId = ProcessId;
			
			DbPrint((DCB_DRIVER, DL_INFO, _T("Process: updated process id %#x for '%s'"), 
				ProcessId, pProcess->m_pwchImagePath));
			
			return;
		}
	}
}

bool CProcessList::AddNewProcess(ULONG ParentProcessId, PCHAR pchProcName, ULONG ProcessId, ULONG ProcHandle, PWCHAR pwchImagePath, PSID pSid)
{
	CSingleProcess* pProcess;

	DeleteProcess(ProcessId);

	pProcess = new CSingleProcess(this, ProcessId, ProcHandle, pwchImagePath, ParentProcessId, &m_Log);
	if (pProcess == NULL)
	{
		DbPrint((DCB_SYSTEM, DL_IMPORTANT, _T("CProcessList: can't init pProcess")));
		return false;
	}

	DbPrint((DCB_SYSTEM, DL_NOTIFY, _T("CProcessList: started new process (parent %#x): from '%s'"), ParentProcessId, pwchImagePath));

	{
		CAutoLock auto_lock(m_Sync);
		m_ProcList.insert(m_ProcList.begin(), pProcess);
	}

	return true;
}
