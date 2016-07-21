// Processes.h: interface for the CProcesses class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSES_H__6421F9E4_60F9_4D53_A9D8_E26524287035__INCLUDED_)
#define AFX_PROCESSES_H__6421F9E4_60F9_4D53_A9D8_E26524287035__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProcesses  
{
public:
	CProcesses();
	virtual ~CProcesses();

	bool Start();
	void Stop();
private:
	Job_Process m_JobProcesess;
	Job_Registry m_JobRegistry;
	Job_Files m_Files;
	Job_Self m_Self;
};

#endif // !defined(AFX_PROCESSES_H__6421F9E4_60F9_4D53_A9D8_E26524287035__INCLUDED_)
