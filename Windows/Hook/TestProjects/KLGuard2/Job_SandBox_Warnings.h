// Job_SandBox_Warnings.h: interface for the Job_SandBox_Warnings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JOB_SANDBOX_WARNINGS_H__B30A4207_6132_4090_AA2B_3DD9CC3B3584__INCLUDED_)
#define AFX_JOB_SANDBOX_WARNINGS_H__B30A4207_6132_4090_AA2B_3DD9CC3B3584__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TaskThread.h"
#include "DSandBoxWarnings.h"
#include "StrHistory.h"

class Job_SandBox_Warnings
{
public:
	Job_SandBox_Warnings();
	virtual ~Job_SandBox_Warnings();

	bool IsExistEvents();

	void PushEvent(CString* pstrEvent);
	void ShowWarnigns();
private:
	LONG m_onshow;
	CStrHistory m_History;
};

#endif // !defined(AFX_JOB_SANDBOX_WARNINGS_H__B30A4207_6132_4090_AA2B_3DD9CC3B3584__INCLUDED_)
