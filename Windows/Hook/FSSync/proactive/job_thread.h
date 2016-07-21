#ifndef __JOB_THREAD__
#define __JOB_THREAD__

#include "TaskThread.h"
#include "../../hook/fssync.h"

class CJob : public HookTask, public ThreadParam
{
public:
	CJob(DWORD dwContext, _tpfProactivNotify pfnCallback) : 
	  m_dwContext(dwContext),
	  m_pfnCallback(pfnCallback)
	  {
			m_OsInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
			GetVersionEx(&m_OsInfo);		  
	  }; 

	virtual ~CJob();
	
	bool Start();
	
	void BeforeExit();
	void FiltersChanged(){};
	
	void SingleEvent();
	bool AddFilters();

private:
	BOOL ProcessNextInput(PINPUT pInput);
	VERDICT ProcessSendInputs(PINPUT pInputs, DWORD nInputs);

	void ProcessEvent(PEVENT_TRANSMIT pEvt, PDRV_EVENT_DESCRIBE pDrvEvDescr, PVERDICT pVerdict);

	_tpfProactivNotify m_pfnCallback;
	DWORD m_dwContext;

	OSVERSIONINFO m_OsInfo;
	DrvEventList m_DrvEventList;
};

#endif // __JOB_THREAD__