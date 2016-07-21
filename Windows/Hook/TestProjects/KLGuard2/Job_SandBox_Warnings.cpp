// Job_SandBox_Warnings.cpp: implementation of the Job_SandBox_Warnings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Job_SandBox_Warnings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//+ ------------------------------------------------------------------------------------------

Job_SandBox_Warnings::Job_SandBox_Warnings()
{
	m_onshow = 0;
}

Job_SandBox_Warnings::~Job_SandBox_Warnings()
{
}

//+ ------------------------------------------------------------------------------------------

bool Job_SandBox_Warnings::IsExistEvents()
{
	if (m_History.GetEventCount() != 0)
		return true;
	
	return false;
}

void Job_SandBox_Warnings::ShowWarnigns()
{
	if (m_History.GetEventCount() == 0)
		return;

	if (InterlockedIncrement(&m_onshow) != 1)
	{
		InterlockedDecrement(&m_onshow);
		return;
	}

	CDSandBoxWarnings DSandBoxWarnings(&m_History, NULL);
	DSandBoxWarnings.DoModal();

	InterlockedDecrement(&m_onshow);
}

void Job_SandBox_Warnings::PushEvent(CString* pstrEvent)
{
	m_History.PushEvent(pstrEvent);
}