#ifndef __JOB_THREAD__
#define __JOB_THREAD__

#include "TaskThread.h"

class CJob : public HookTask, public ThreadParam
{
public:
	CJob(LPVOID pContext) : m_pContext(pContext){}; 
	virtual ~CJob();
	
	bool Start();
	
	void BeforeExit();
	void FiltersChanged(){};
	
	void SingleEvent();
	bool AddFilters();

private:

	void ProcessEvent(PEVENT_TRANSMIT pEvt, PDRV_EVENT_DESCRIBE pDrvEvDescr, PVERDICT pVerdict);

	LPVOID m_pContext;
	OSVERSIONINFO m_OsInfo;


	DrvEventList m_DrvEventList;
};

#endif // __JOB_THREAD__