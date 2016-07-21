#include "../include/eventhandler.h"

cEvent::~cEvent() {
	if (m_verdict != evtVerdict_Default && m_primary_event)
		m_primary_event->SetVerdict(m_verdict);
	if (m_pProcess)
		m_pProcess->ReleaseRef();
}

cProcess cEvent::GetProcess()
{
	if (!m_pProcess)
	{
		assert(m_pEventMgr);
		cProcess proc = _iFindProcess(m_pEventMgr, m_pid);
		if (proc)
		{
			m_pProcess = proc;
			m_pProcess->AddRef();
		}
	}
	if (m_pProcess)
	{
		m_pProcess->AddRef();
		m_pProcess->Lock();
	}
	return m_pProcess;
}

uint64_t cEvent::GetTime()
{
	if (!m_time && m_pEventMgr)
		m_time = m_pEventMgr->m_pEnvironmentHelper->GetTimeLocal();
	return m_time;
}

#define CALL_NEXT(call) \
	if (!m_pSubHandlersList)  \
		return;  \
	INIT_EVENT(event);  \
	cAutoLock _lock(*m_pSubHandlersListLock, false); \
	if (!m_pProcessList->IsInterpreteur(event.m_pid))  \
	{  \
		for (tEventHandlerList::iterator _it = m_pSubHandlersList->begin(); _it != m_pSubHandlersList->end(); _it++)  \
			_it->pEventHandler->call;  \
	}  \
	if (event.m_bSkipForward) \
		return; \
	tPid caller_pid = event.m_pid; \
	cEvent* pOrigEvent = &event; \
	if (m_pProcessList->GetCaller(caller_pid, &caller_pid)) \
	{  \
		cEvent event(*pOrigEvent, caller_pid);  \
		INIT_EVENT(event);  \
		event.m_bForwardCall = true; \
		m_pEventMgr->call;  \
	};  \
	return;

#define DECLARE_EVENT(name, params, short_params) \
	void cEventHandler::name params { CALL_NEXT(name short_params); };


#include "../include/events.h"
