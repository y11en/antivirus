#include "NetWatcherTaskState.h"
#include <productcore\iface\i_taskmanager.h>
CNetWatcherTaskState::CNetWatcherTaskState()
{
	m_state=TASK_REQUEST_STOP;
	m_state_old=TASK_REQUEST_STOP;
	InitializeCriticalSection(&m_cs_state);
}
CNetWatcherTaskState::~CNetWatcherTaskState()
{
	DeleteCriticalSection(&m_cs_state);
}
void CNetWatcherTaskState::SaveState(tDWORD state)
{
	EnterCriticalSection(&m_cs_state);
	m_state_old=m_state;
	m_state=state;
	LeaveCriticalSection(&m_cs_state);
}
tDWORD CNetWatcherTaskState::QStateOld()
{
	EnterCriticalSection(&m_cs_state);
	tDWORD state=m_state_old;
	LeaveCriticalSection(&m_cs_state);
	return state;
}
tDWORD CNetWatcherTaskState::QState()
{
	EnterCriticalSection(&m_cs_state);
	tDWORD state=m_state;
	LeaveCriticalSection(&m_cs_state);
	return state;
}