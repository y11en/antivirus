#ifndef __HOOK_JOBS
#define __HOOK_JOBS

#include "CheckWindow.h"
#include "TaskThread.h"
#include "RegAutoRuns.h"
#include "AppSettings.h"
#include "Job_SandBox_Warnings.h"

#include "strhistory.h"
#include "servicestuff.h"

#include "ProcessRecognizer.h"
#include "KnownProcessList.h"

#include "ThNewProcess.h"
#include "ThModifiedProcess.h"
#include "ThCheckSelfCopy.h"
#include "ThCMD_Remote.h"

#include "FilePlacement.h"
#include "UncontrolledFiles.h"

#include "FileLog.h"

#include <vector>

//+ ------------------------------------------------------------------------------------------

class CSingleProcess : public CProcessRecognizer
{
public:
	CSingleProcess(PWCHAR pwchImagePath) : CProcessRecognizer(pwchImagePath)
	{
		m_ProcessId = m_ParentId = 0;
		m_ProcessHandle = 0;
		m_bHasInvisibleWindows = false;
		m_bSandBoxed = false;
		m_ImagePath = pwchImagePath;
		m_SandBoxFilterId_FileActivity = 0;
		m_bWasSelfOpen = false;
		m_bIsCmd = false;
		m_bAutorunModified = false;
		m_bSelfCopy = false;
		m_bAlive = true;
	};
	virtual ~CSingleProcess()
	{
	};

	bool IsSendBoxed()
	{
		return m_bSandBoxed;
	}

	void SetSandBoxed(bool bSandBoxed)
	{
		m_bSandBoxed = bSandBoxed;
	}

	CString* GetImagePath()
	{
		return &m_ImagePath;
	}

	bool m_bAlive;
	SYSTEMTIME m_SysTime;
	ULONG m_ProcessId;
	ULONG m_ProcessHandle;
	ULONG m_ParentId;
	CString m_ProcName;
	CString m_ParentProcName;

	bool m_bHasInvisibleWindows;

	CDrvHistory m_DrvRegHistory;
	CDrvHistory m_DrvFileHistory;
	CDrvHistory m_DrvSelfHistory;

	bool m_bSandBoxed;
	ULONG m_SandBoxFilterId_FileActivity;

	bool m_bWasSelfOpen;

	bool m_bIsCmd;
	bool m_bAutorunModified;
	bool m_bSelfCopy;
private:
	CString m_ImagePath;
};

typedef std::vector<CSingleProcess*> _pProcList;

//+ ------------------------------------------------------------------------------------------
#define _PROCLIST_MODIFIED_NONE		0x000
#define _PROCLIST_MODIFIED_ITEMS	0x001
#define _PROCLIST_MODIFIED_PARAMS	0x002

void CurrentTimeToString(CString* pstrtime);

class CProcessList : public CCheckWindow, public COwnSync, public CFilePlacement, public CGenericSettings
{
public:
	CProcessList();
	virtual ~CProcessList();

	bool StartLoop();
	void StopLoop();
	bool ActivateProtection(bool bActivate);

	void EnvironmentChanged();
	virtual void GS_SettingsChanged();

	bool AddNewProcess(ULONG ParentProcessId, PCHAR pchProcName, ULONG ProcessId, ULONG ProcHandle, PWCHAR pwchImagePath, PSID pSid);
	void UpdateProcessIdByHandle(ULONG ProcessHandle, ULONG ProcessId);
	void DeleteProcess(ULONG ProcessId);
	bool IsSandBoxed(ULONG ProcessId);

	void RegistryActivity(ULONG ProcessId, PWCHAR pwchRegUrl, bool bAutoRun);
	void SysFileActivity(ULONG ProcessId, PWCHAR pwchFileName);
	void CheckSelfCopy(ULONG ProcessId, PWCHAR pwchFileName);
	void CheckSelfOpen(ULONG ProcessId, PWCHAR pwchFileName);

	bool IsExistPermisionForCreateProcessing(ULONG CreatorProcessId);
	bool IsAllowedStart_Modified(PWCHAR pwchImagePath);
	bool IsAllowedFileActivity(ULONG ProcessId, PWCHAR pwchFileName);
	
	void PushAlertMessage_CreateProcess(ULONG CreatorProcessId, PWCHAR pwchImagePath);
	void PushAlertMessage_NotAllowedFileActivity(ULONG CreatorProcessId, PCHAR pchProcName, PWCHAR pwchFileName);
	void PushAlertMessage_WriteToRegistry(ULONG ProcessId, PWCHAR pwchRegistryKeyName);
	void PushAlertMessage_ChangeAutoRunKey(ULONG ProcessId, PCHAR pchProcName, PWCHAR pwchRegistryKeyName);
	void PushAlert_SelfCopy(ULONG ProcessId, CString *pProcName, CString* pImagePath, CString* pNewFileName);
	void PushAlert_CMD_Remote(ULONG ProcessId, PWCHAR pwchCmdLine);
	void PushAlert_TrojanActivity(ULONG ProcessId, CString *pProcName, CString* pImagePath);

	void TimeoutStep();

	void DeleteHistoryFor(ULONG ProcessId, CString* pImagePath);

	void AddTask_CheckSelfCopy(ULONG ProcessId, CHashContainer& HashC, CString *pProcName, CString* pImagePath, PWCHAR pwchNewFileName);

	_eStartNewProcess_Verdict PushAlertMessage_NewProcess(ULONG CreatorProcessId, PWCHAR pwchImagePath, CProcessRecognizer* pRecognizer);

	_pProcList* AcquireProcList()
	{
		LockSync();
		return &m_ProcList;
	};
	void ReleaseProcList()
	{
		m_Modified = _PROCLIST_MODIFIED_NONE;
		UnLockSync();
	}

	void SetModifiedFlag(unsigned int modified_flag)
	{
		if (modified_flag == _PROCLIST_MODIFIED_NONE)
			m_Modified = _PROCLIST_MODIFIED_NONE;
		else
			m_Modified |= modified_flag;
	}

	unsigned int GetModifiedFlags()
	{
		return m_Modified;
	}

	CGenericSettings* GetGenericSettings(){return this;}
	CSandBoxSettings* GetSandBoxSettings(){return &m_SandBoxSettings;}

	CSingleProcess* FindProcess(ULONG ProcessId);
	CSingleProcess* FindProcessByTime(LPSYSTEMTIME pSystime);

	Job_SandBox_Warnings m_JobSandBoxWarnings;
	
	CKnownProcessList m_KnownProcessList;
	CThNewProcess m_ThNewProcess;
	CThModifiedProcess m_ThModifiedProcess;
	CThCheckSelfCopy m_ThCheckSelfCopy;
	CThCMD_Remote m_ThCMD_Remote;

	HANDLE m_hDevice;
	ULONG m_AppID_JobFiles;
	ULONG m_AppID_Processes;

private:
	void ChangeFilterForSystemProcess(bool bActivate);
	void AddSystemProcessFilter(ULONG ProcessId);
	void InitList();
	virtual void AlertInvisilbe(ULONG ProcessId);

	void CheckTrojanActivity(CSingleProcess* pSingle);

	ULONG SandBoxFilters_Add(ULONG ProcessId);
	bool SandBoxFilters_Delete(CSingleProcess* pSingle);

	_pProcList m_ProcList;
	unsigned int m_Modified;	

	CSandBoxSettings m_SandBoxSettings;
	CFileLog m_FileLog;
};

//+ ------------------------------------------------------------------------------------------

class Job_Process : public HookTask, public ThreadParam
{
public:
	Job_Process(){};
	virtual ~Job_Process(){};

	bool Start()
	{
		return HookTask::Start(this);
	}

	void BeforeExit();
	void FiltersChanged(){};
	
	void SingleEvent();
	bool AddFilters();

private:
	DrvEventList m_DrvEventList;
};

//+ ------------------------------------------------------------------------------------------

class Job_Registry : public HookTask, public ThreadParam, private CRegAutoRuns
{
public:
	Job_Registry(){};
	virtual ~Job_Registry(){};

	bool Start()
	{
		return HookTask::Start(this);
	}
	
	void BeforeExit(){};
	void FiltersChanged(){};
	
	void SingleEvent();
	bool AddFilters();

private:
	DrvEventList m_DrvEventList;
};

//+ ------------------------------------------------------------------------------------------

class Job_Files : public HookTask, public ThreadParam
{
public:
	Job_Files(){};
	virtual ~Job_Files(){};

	bool Start()
	{
		return HookTask::Start(this);
	}
	
	void BeforeExit(){};
	void FiltersChanged(){};
	
	void SingleEvent();
	bool AddFilters();

private:
	void AddInvisibleFile(PTCHAR ptchFileName);

	DrvEventList m_DrvEventList;
};

//+ ------------------------------------------------------------------------------------------

class Job_Self : public HookTask, public ThreadParam
{
public:
	Job_Self(){};
	virtual ~Job_Self(){};

	bool Start()
	{
		return HookTask::Start(this);
	}
	
	void BeforeExit(){};
	void FiltersChanged(){};
	
	void SingleEvent();
	bool AddFilters();

//+ ------------------------------------------------------------------------------------------
private:
	DrvEventList m_DrvEventList;
};

#endif //__HOOK_JOBS