// ThCheckSelfCopy.h: interface for the CThCheckSelfCopy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THCHECKSELFCOPY_H__8513957D_D663_4269_AA22_273F6D7A543F__INCLUDED_)
#define AFX_THCHECKSELFCOPY_H__8513957D_D663_4269_AA22_273F6D7A543F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TaskThread.h"

typedef struct _tRequest_CheckSelfCopy
{
	DWORD m_dwID;
	ULONG m_ProcessId;
	CHashContainer m_HashC;
	CString m_ProcName;
	CString m_ImagePath;
	CString m_NewFileName;
}Request_CheckSelfCopy, *PRequest_CheckSelfCopy;

typedef std::vector<PRequest_CheckSelfCopy> _CheckSelfCopyQueue;

class CCheckSelfCopyQueue : public COwnSync
{
public:
	CCheckSelfCopyQueue(){};
	~CCheckSelfCopyQueue(){};

	void PushEvent(PRequest_CheckSelfCopy pRequest);
	PRequest_CheckSelfCopy PopEvent();
private:
	_CheckSelfCopyQueue m_SelfCopyQueue;
};

class CThCheckSelfCopy : public HookTask
{
public:
	CThCheckSelfCopy();
	virtual ~CThCheckSelfCopy();

	bool StartLoop();
	void StopLoop();

	void CheckSelfCopy(ULONG ProcessId, CHashContainer& HashC, CString *pProcName, CString* pImagePath, 
		PWCHAR pwchNewFileName);

	void ShowTrojanActivity(ULONG ProcessId, CString *pProcName, CString* pImagePath);

	HANDLE Init();
	void BeforeExit();

	CCheckSelfCopyQueue m_CheckSelfCopyQueue;
private:
	HANDLE m_hNewEvent;
	bool m_bThreadExist;
};

#endif // !defined(AFX_THCHECKSELFCOPY_H__8513957D_D663_4269_AA22_273F6D7A543F__INCLUDED_)
