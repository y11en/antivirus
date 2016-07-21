#ifndef __cEventHandler__H_
#define __cEventHandler__H_

#include <assert.h>
#include "types.h"

class cEventHandler;
class cEventMgr;
class cEnvironmentHelper;
class cFile;
class cPath;
class cRegKey;

#include "../../heuristic/proclist.h"

#include <vector>
typedef struct tag_cEventHandlerDescr {
	cEventHandler* pEventHandler;
	bool bDelete;
} cEventHandlerDescr;
typedef std::vector<cEventHandlerDescr> tEventHandlerList;

#if !defined(INIT_EVENT)
#define INIT_EVENT(event) \
	if (m_pEventMgr) \
	{ \
		event.m_pEventMgr = m_pEventMgr; \
		if (event.m_tid) \
			m_pProcessList->GetRemoteThreadCreator(event.m_tid, &event.m_pid); \
	}
#endif

// MS compiler v.1200 assigns to function declared with typedef __cdecl calling convention. 
// This leading to colling convention conflict at link time (__cdecl vs __thiscall).
// As workaround we declare handlers with __fastcall calling convention
#if defined(_MSC_VER)
#if _MSC_VER <= 1200
#define __HANDLER_CALL __fastcall
#else // _MSC_VER > 1200
#define __HANDLER_CALL 
#endif // _MSC_VER
#else // _MSC_VER not defined
#define __HANDLER_CALL 
#endif


#define DECLARE_EVENT(name, params, short_params) \
	typedef void __HANDLER_CALL _t##name params; \
	virtual void __HANDLER_CALL name params; // { CALL_NEXT(name short_params); };

/*
#define DECLARE_HANDLER3(name, params, short_params) \
	DECLARE_EVENT(name##Pre,  params, short_params) \
	DECLARE_EVENT(name##Post, params, short_params) \
	DECLARE_EVENT(name##Fail, params, short_params)
*/

#define _IMPLEMENT(handler) _t##handler handler;

class cEvent
{
public:
	tPid m_pid;
	tTid m_tid;
//	tPid m_primary_event_pid;
	void* m_context;
	cProcessListItem* m_pProcess;
	cEventHandler* m_pEventMgr;
	bool m_bForwardCall;
	bool m_bSkipForward;
	uint64_t m_time;
	tVerdict m_verdict;
	cEvent*  m_primary_event;

	cEvent(void* context=0, tPid pid=0, tTid tid=0) : 
		m_pid(pid),
		m_tid(tid), 
		//m_caller_pid(0), 
		m_context(context),
		m_pProcess(0),
		m_pEventMgr(0),
		m_bSkipForward(false),
		m_bForwardCall(false),
		m_time(0),
		m_verdict(evtVerdict_Default),
		m_primary_event(0)
	{
		m_primary_event = this;
	}
	cEvent(cEvent & original_event, tPid new_pid) :
		m_pid(new_pid), 
		m_tid(0), 
		//m_caller_pid(0), 
		m_context(original_event.m_context),
		m_pProcess(0),
		m_pEventMgr(original_event.m_pEventMgr),
		m_bSkipForward(false),
		m_bForwardCall(false),
		m_time(original_event.m_time),
		m_verdict(evtVerdict_Default),
		m_primary_event(original_event.m_primary_event ? original_event.m_primary_event : &original_event)
	{
	}		
	~cEvent();
	cProcess GetProcess();
	uint64_t GetTime();
	void SetTime(uint64_t time) { m_time = time; };
	tVerdict SetVerdict(tVerdict verdict) { m_verdict = verdict; return m_verdict; };
	tVerdict GetVerdict() const { return m_verdict; };
	tPid     GetPrimaryEventPid() const { return (m_primary_event ? m_primary_event->m_pid : m_pid); }
};

class cEventHandler
{
public:
	cEventHandler() : m_pEnvironmentHelper(NULL), m_pSubHandlersList(NULL), m_pSubHandlersListLock(NULL) {};
	virtual ~cEventHandler() {};
	
	virtual bool Init() { return true; } ;
	virtual void Done() { } ;
	
#include "events.h"

public:
	cEventMgr*                m_pEventMgr;
	cProcessList*             m_pProcessList;
	cEnvironmentHelper*       m_pEnvironmentHelper;
	tEventHandlerList*        m_pSubHandlersList;
	cLock*                    m_pSubHandlersListLock;
};

#undef DECLARE_EVENT
#undef __HANDLER_CALL

#include "envhelper.h"
#include "eventmgr.h"
#include "cpath.h"
#include "cregkey.h"

#endif // __EVENTS__H_
