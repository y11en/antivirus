#ifndef __prclist_h
#define __prclist_h

#include <windows.h>
#include <prague/prague.h>
#include <Prague/iface/i_string.h>

#include "EvCache.h"

class cProcessingList
{
public:
	cProcessingList( hOBJECT pPragueContext );
	~cProcessingList();

	void Enter( PVOID pDrvContext, PVOID pMessage, cString* hCtx );
	void Leave( cString* hCtx );

public:
	hOBJECT m_pPragueContext;
	CRITICAL_SECTION m_Sync;
	PHashTree m_pHashTree;
};

#endif // __prclist_h