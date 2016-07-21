#ifndef __JOB_THREAD__
#define __JOB_THREAD__

#include "stdafx.h"
#include "stuff/TaskThread.h"

#include "proclist.h"

#include "RegAutoRuns.h"
#include "ActivityCheck.h"

class CJob : public HookTask, public ThreadParam
{
public:
	CJob(){};
	virtual ~CJob();

	bool Start();

	void BeforeExit();
	void FiltersChanged(){};
	
	void SingleEvent();
	bool AddFilters();

private:
	bool AddProcFilters();
	bool AddRegFilters();
	bool AddFileFilters();

	void ProcessEvent(PEVENT_TRANSMIT pEvt);
	void RegistryEvent(PEVENT_TRANSMIT pEvt);
	void FileEvent(PEVENT_TRANSMIT pEvt);

private:
	CProcessList m_ProcessList;

	CActivityCheck m_ActivityCheck;

	DrvEventList m_DrvEventList;

	_pProcList m_ProcList;

	CRegAutoRuns m_RegCheck;

};

#endif // __JOB_THREAD__