#ifndef __PROCESS_LIST__H__
#define __PROCESS_LIST__H__

#include "stdafx.h"
#include "single_process.h"
#include "log.h"


#include <vector>
#include "stuff/mm.h"
#include "log.h"
#include "proclist.h"
#include "PathList.h"

class CSingleProcess;
class CProcessList;

//+ ------------------------------------------------------------------------------------------

typedef std::vector<CSingleProcess*> _pProcList;

//+ ------------------------------------------------------------------------------------------

class CSingleProcess
{
public:
	CSingleProcess(CProcessList* pProcessList, ULONG ProcessId, ULONG ProcessHandle, PWCHAR pwchImagePath, ULONG ParentProcessId, CLog* pParentLog = NULL);
	virtual ~CSingleProcess();
	long AddRef();
	long Release();

	CSingleProcess* GetParent();
	
	//+ ------------------------------------------------------------------------------------------

	ULONG m_ParentProcessId;
	ULONG m_ProcessId;
	ULONG m_ProcessHandle;
	SYSTEMTIME m_SysTime;
	
	PWCHAR m_pwchImagePath;
	PWCHAR m_pwchCmdLine;
	CLog   m_Log;
	CPathList m_SelfCopyList;
private:
	CProcessList* m_pProcessList;
	long m_nLockCount;
};

//+ ------------------------------------------------------------------------------------------

class CProcessList
{
public:
	CProcessList(){};
	virtual ~CProcessList();

	CSingleProcess* FindProcess(ULONG ProcessId);

	bool AddNewProcess(ULONG ParentProcessId, PCHAR pchProcName, ULONG ProcessId, ULONG ProcHandle, PWCHAR pwchImagePath, PSID pSid);
	void UpdateProcessIdByHandle(ULONG ProcessHandle, ULONG ProcessId);
	void DeleteProcess(ULONG ProcessId);

	COwnSync m_Sync;

private:

	_pProcList m_ProcList;
};

#endif // __PROCESS_LIST__H__