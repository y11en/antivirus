#ifndef __trustpr_h
#define __trustpr_h

#include <windows.h>
#include <prague/prague.h>
#include "EvCache.h"

class cTrustedProcessesCache
{
public:
	cTrustedProcessesCache( hOBJECT pPragueContext );
	~cTrustedProcessesCache();

	void ClearCache();
	bool IsTrusted( ULONG ProcessId );

private:
	void ClearCacheImp();

public:
	hOBJECT m_pPragueContext;
	CRITICAL_SECTION m_Sync;
	PHashTree m_pHashTree;
};

#endif // __trustpr_h