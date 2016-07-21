#ifndef _STATE_MANAGER_H_
#define _STATE_MANAGER_H_

#include <Prague/prague.h>
#include <Prague/plugin/p_win32loader.h>

#include <ProductCore/iface/i_taskmanager.h>
#include <ProductCore/iface/i_taskmanager.h>

#include <Prague/iface/i_csect.h> 

#include "threadimp.h"
#include "trafficprotocoller.h"
#include <ProductCore/structs/s_profiles.h>

/*
template<class T>
class CStateManager : public CThreadImp
{
public:
	CStateManager(T* pTask) : m_pTask(pTask)  {;}
	virtual ~CStateManager(){;};
	virtual tERROR _Run()
	{
		m_pTask->SetStateEx((tTaskRequestState)m_p_state);
		return errOK;
	}
	tERROR SetState(tDWORD p_state)   
	{ 
		if ( p_state == TASK_REQUEST_RUN )
			Stop(cFALSE);
		m_p_state = p_state;   
		return Start(); 
	};
private:
	T* m_pTask;
	tDWORD m_p_state;
};
*/
#define RELEASE_SERVICE( error, service )	\
{	\
	if ( service && PR_SUCC(_this->sysCheckObject((hOBJECT)service)) )	\
	{	\
		/*error =*/ m_tm->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)service);	\
		service = NULL;	\
	}	\
}

template<typename T>
tERROR PrepareObject(
					 T* _this,
					 tDWORD& m_dwSessionCount, 
					 hCRITICAL_SECTION *pSessionCS,
					 hOBJECT& m_traffic_monitor,
					 hTASKMANAGER& m_tm,
					 T* = 0)
{
	cERROR error = errOK;
	m_dwSessionCount = 0;
	if(pSessionCS)
		*pSessionCS = NULL;
	m_traffic_monitor = NULL;
	if (PR_SUCC(error))
		error = _this->sysCreateObjectQuick( (hOBJECT*)pSessionCS,
		IID_CRITICAL_SECTION, PID_WIN32LOADER_2LEVEL_CSECT );
	if (PR_SUCC(error))
		error = _this->sysRegisterMsgHandler(pmcTRAFFIC_PROTOCOLLER, rmhDECIDER, 
		m_tm, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
	return error;
}

template<typename T>
tERROR ReleaseObject(
					 T* _this,
					 hCRITICAL_SECTION *pSessionCS,
					 hOBJECT& m_traffic_monitor,
					 hTASKMANAGER& m_tm,
					 T* = 0)
{
	cERROR error = errOK;
	if ( pSessionCS && *pSessionCS )
	{
		(*pSessionCS)->sysCloseObject();
		*pSessionCS = NULL;
	}
	if ( m_tm )
		_this->sysUnregisterMsgHandler(pmcTRAFFIC_PROTOCOLLER, m_tm);
	if ( m_traffic_monitor && PR_SUCC(_this->sysCheckObject((hOBJECT)m_traffic_monitor)) )	
	{	
		m_tm->ReleaseService(TASKID_TM_ITSELF, (hOBJECT)m_traffic_monitor);	
		m_traffic_monitor = NULL;	
	}	
	return error;
}
#endif//_STATE_MANAGER_H_