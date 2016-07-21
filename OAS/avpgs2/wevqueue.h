#ifndef __wevqueue
#define __wevqueue

#include <windows.h>
#include <Prague/prague.h>
#include <Prague/iface/i_string.h>
#include <prague/pr_oid.h>

//#include "wavpg.h"
#include "EvCache.h"

class cDelayedContext
{
public:
	cDelayedContext( hOBJECT pPragueContext, cString* hCtx, int Timeout );
	~cDelayedContext();

	bool IsValid();

public:
	hOBJECT m_pPragueContext;
	tPTR m_ObjectName;
	tDWORD m_NameLength;
	ULONG m_ProcessId;
	HANDLE m_Impersonate;
	__int64 m_Timeout;
	tORIG_ID m_Origin;
};

// 
class cEventQueue
{
public:
	cEventQueue( hOBJECT pPragueContext );
	~cEventQueue();

	bool Add( cDelayedContext* pDelayedContext );
	void SkipItem( cString* hCtx );
	bool DeleteNotTheSame( cString* hCtx );

	cDelayedContext* Next();

public:
	hOBJECT m_pPragueContext;
	CRITICAL_SECTION m_Sync;
	PHashTree m_pHashTree;
};

#endif // wevqueue