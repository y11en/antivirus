#ifndef _ShareExclusiveResource_h_included_lfdasdjlkajsd
#define _ShareExclusiveResource_h_included_lfdasdjlkajsd

///////////////////////////////////////////////////////////
//
// CShareExclusiveResource
//
#include <windows.h>

#include <Prague/prague.h>
#include <Prague/iface/i_csect.h>

#ifdef __cplusplus
	#define RWM_PROC extern "C" 
#else
	#define RWM_PROC
#endif

typedef struct tag_RWM_SubQueue
{
  	struct tag_RWM_SubQueue*	m_pNext;
	tBOOL						m_bFree;

	tDWORD						m_Accessor;
	tDWORD						m_EntersCount;
	tDWORD						m_WriterCount;
} RWM_SubQueue, *PRWM_SubQueue;

typedef struct tag_RWM_Queue
{
	struct tag_RWM_Queue*	m_pNext;
	tBOOL					m_bFree;

	tSHARE_LEVEL  m_AccessLevel;
	HANDLE				m_SyncEvent;

	PRWM_SubQueue			m_pSubList;
} RWM_Queue, *PRWM_Queue;


// The single-writer/multiple-reader guard 
// compound synchronization object
typedef struct tag_RWManager 
{
	CRITICAL_SECTION  m_protector;
#ifdef _DEBUG
  tUINT             m_enters;
  tUINT             m_writers;
#endif
	
	tVOID* (*allocator)(tDWORD);
	tVOID(*liberator)(tVOID*);

	PRWM_Queue			  m_pCirceQueueRoot;
	PRWM_Queue				m_pCirceQueueLast;
	PRWM_Queue				m_pCirceQueueFree;
} RWManager;



////////////////////////////////////////////////////////////
RWM_PROC tBOOL RWM_Initialize(RWManager* rwm, tVOID* (*allocator)(tDWORD), tVOID(*liberator)(tVOID*));
RWM_PROC void RWM_Delete(RWManager* rwm);

// general function readers/writers enter
RWM_PROC tBOOL RWM_Wait(RWManager* prwm, tSHARE_LEVEL access_level);

// A writer thread calls this function to know when
// it can successfully write to the shared data.
#define RWM_WaitToWrite(x)	RWM_Wait(x, SHARE_LEVEL_WRITE)

// A reader thread calls this function to know when 
// it can successfully read the shared data.
#define RWM_WaitToRead(x)	RWM_Wait(x, SHARE_LEVEL_READ)


RWM_PROC tBOOL RWM_Release(RWManager* rwm, tSHARE_LEVEL* prev_level);


#endif
