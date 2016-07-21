#ifndef __wusers
#define __wusers

#include <windows.h>
#include <prague/prague.h>
#include <prague/iface/i_threadpool.h>
#include <ProductCore/common/userban.h>

#include "EvCache.h"

class cWUsers
{
public:
	cWUsers( hOBJECT pPragueContext );
	~cWUsers();

	bool FillInfo( PLUID pLuid, cUserInfo* pUser );
	void ResolveLuid( PLUID pLuid );

	tERROR ResetBanList();
	tERROR AddBan( cUserInfo* pUserInfo );
	bool IsBanned( PLUID pLuid );

public:
	cThreadPool* m_ThPoolWork;
	CRITICAL_SECTION m_Sync;
	PHashTree m_pHashTree;
};

#endif // __wusers