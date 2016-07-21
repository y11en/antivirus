#define  _netwatcher_project_
#include "NetRulesManager.h"
#include <NetWatch/structs/s_netwatch.h>
#include <CKAH/CKAHUM.h>
#include <CKAH/CKAHSTAT.h>
#include <CKAH/CKAHPorts.h>
#include <CKAH/ckahrules.h>
#include <CKAH/ckahiptoa.h>

void CNetRulesManager::SetState( tTaskRequestState p_state )
{
	if(QState()==p_state)
		return;

	tTaskRequestState TaskState;
	tTaskRequestState SettingsState=m_settings.GetSettingsState();
	Lock();
	if(p_state==TASK_REQUEST_UNK)
		TaskState=m_TaskState;
	else
	{
		TaskState=p_state;
		m_TaskState=p_state;
	}
	TaskState=max(TaskState,SettingsState);
	if(QState()==TaskState || (QState()==TASK_REQUEST_STOP && TaskState!=TASK_REQUEST_RUN))
	{
		Unlock();
		return;
	}	
	SaveState(TaskState);
	PR_TRACE((m_hips, prtIMPORTANT, "CNetRulesManager::SetState(%d),old=%d",TaskState,QStateOld()));
	CKAHUM::OpResult opres;
	switch (TaskState)
	{
		case TASK_REQUEST_RUN:
			if(QStateOld()==TASK_REQUEST_STOP)
			{
				opres = InitCKAHUM();
				if(opres!=CKAHUM::srOK)
				{
					PR_TRACE((m_hips, prtERROR, "CNetRulesManager::SetState Cant init CKAHUM, opres=%d\n", opres));
					SaveState(TASK_REQUEST_STOP);
					break;
				}
			}
			if(QStateOld()==TASK_REQUEST_STOP || QStateOld()==TASK_REQUEST_PAUSE)
			{
				//PR_TRACE((m_hips, prtIMPORTANT, "CNetRulesManager:: resume CKAHUM"));
				opres=CKAHFW::Resume();
				if(opres!=CKAHUM::srOK)
				{
					PR_TRACE((m_hips, prtERROR, "CNetRulesManager::SetState Cant resume CKAHUM, opres=%d\n", opres));
					SaveState(TASK_REQUEST_STOP);
					break;
				}
				Unlock();
				//PR_TRACE((m_hips, prtIMPORTANT, "CNetRulesManager:: resume CKAHUM OK"));
				GeneratePacketRules();
				GenerateAppRules();
				Lock();
			}
			break;
		case TASK_REQUEST_PAUSE:
			if(QStateOld()==TASK_REQUEST_RUN)
				CKAHFW::Pause();
			break;
		case TASK_REQUEST_STOP:
			if(QStateOld()==TASK_REQUEST_RUN || QStateOld()==TASK_REQUEST_PAUSE )
			{
				PR_TRACE((m_hips, prtIMPORTANT, "CNetRulesManager:: Stopping CKAHUM"));
				CKAHFW::Stop();
			}
			break;
	}
	Unlock();
} 
void CNetRulesManager::ObjectPreClose()
{
/*
	//if(m_netwatch!=0)
	//	m_tm->ReleaseService(TASKID_TM_ITSELF,(hOBJECT)m_netwatch);
	m_netwatch=0;

	if(m_HIPSSettings)
		delete m_HIPSSettings;
	if(m_NWsettings)
		delete m_NWsettings;
	if(m_NWdata)
		delete m_NWdata;
*/
}
void CNetRulesManager::ObjectInitDone(cObject * pTHIS,hTASKMANAGER		tm)
{	
	m_hips=pTHIS;
	m_tm=0;
/*
	m_NWsettings=new cNWSettings;
	m_NWdata=new cNWdata;
	m_HIPSSettings = new CHipsSettings;
*/
	m_TaskState=TASK_REQUEST_STOP;
	m_netwatch=0;
	m_inited=1;
	m_tm=tm;
	tERROR error = m_hips->sysCreateObjectQuick((cObject**) &m_IPResolver, IID_IPRESOLVER);
}
void  CNetRulesManager::Lock ()
{
    EnterCriticalSection(&m_cs);
}

void  CNetRulesManager::Unlock ()
{
    LeaveCriticalSection(&m_cs);
}
void CNetRulesManager::SetSettings(const cSerializable * NewSettings)
{
	m_settings.SetHIPSSettings((CHipsSettings *)NewSettings);
	tTaskRequestState state=(tTaskRequestState)QState();
	SetState(TASK_REQUEST_UNK);
	if(state==QState())
	{
		GeneratePacketRules();
		GenerateAppRules();
	}
}
tBOOL CNetRulesManager::NetWatcherSettingsChanged()
{
	if(QState()!=TASK_REQUEST_RUN)
		return false;
	tERROR error;
	cProfileBase profNW;
	error = m_tm->GetProfileInfo(AVP_PROFILE_NETWORK_WATCHER, &profNW);
	if(error!=errOK || profNW.m_nState!=PROFILE_STATE_RUNNING)
	{
		m_netwatch=0;
		return false;
	}
	if(m_netwatch==0)
	{
		error = m_tm->GetService(TASKID_TM_ITSELF,(hOBJECT)this, AVP_PROFILE_NETWORK_WATCHER, (hOBJECT*)&m_netwatch);
		if(error!=errOK)
		{
			m_netwatch=0;
			return false;
		}
	}
	cNWdata tmp_NWdata;
	if(((NetWatch*)m_netwatch)->AskAction(1,(&tmp_NWdata)))
		return false;
	m_settings.SetNWdata(&tmp_NWdata);
	cNWSettings tmp_NWsettings;
	error = m_tm->GetProfileInfo(AVP_PROFILE_NETWORK_WATCHER, &tmp_NWsettings);
	if(error==errOK)
	{
		m_settings.SetNWsettings(&tmp_NWsettings);
		return true;
	}
	return false;
//	GeneratePacketRules();
}
CNetRulesTaskState::CNetRulesTaskState()
{
	m_state=TASK_REQUEST_STOP;
	m_state_old=TASK_REQUEST_STOP;
	m_CKAH_state=0;
	InitializeCriticalSection(&m_cs_state);
}
CNetRulesTaskState::~CNetRulesTaskState()
{
	DeleteCriticalSection(&m_cs_state);
}
void CNetRulesTaskState::SaveState(DWORD state)
{
	EnterCriticalSection(&m_cs_state);
	m_state_old=m_state;
	m_state=state;
	LeaveCriticalSection(&m_cs_state);
}
DWORD CNetRulesTaskState::QStateOld()
{
	EnterCriticalSection(&m_cs_state);
	DWORD state=m_state_old;
	LeaveCriticalSection(&m_cs_state);
	return state;
}
DWORD CNetRulesTaskState::QState()
{
	EnterCriticalSection(&m_cs_state);
	DWORD state=m_state;
	LeaveCriticalSection(&m_cs_state);
	return state;
}

