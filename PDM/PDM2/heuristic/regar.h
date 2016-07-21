#ifndef _EVTMGR_REGAR_H
#define _EVTMGR_REGAR_H

class cEventHandler;
class cRegAutoRunList;

#include <vector>
#include "../eventmgr/include/types.h"
#include "../eventmgr/include/cpath.h"
#include "../eventmgr/include/pathlist.h"
#include "../eventmgr/include/tstring.h"
#include "../eventmgr/include/lock.h"
#include "../eventmgr/include/cregkey.h"
#include "../eventmgr/include/eventmgr.h"

#define ARF_CLSID_VALUEDATA 0x01
#define ARF_CLSID_VALUENAME 0x02
#define ARF_CLSID_KEY       0x04
#define ARF_MULTI_SZ        0x08

class cRegAutoRun {
public:
	cRegAutoRun();
	~cRegAutoRun();
	cRegAutoRun& operator =(cRegAutoRun&);
	bool resolve(cEnvironmentHelper* pEnvironmentHelper);
	void clear();

public:
	cRegKey key;
	tstring value_name;
	tstring value_data;
	tstring value_data_part;
	tstring filename;
};

typedef std::vector<cRegAutoRun*> tautorunvector;

class cRegAutoRunList {
public:
	cRegAutoRunList();
	~cRegAutoRunList();
	bool Init(cEnvironmentHelper* pEnvironmentHelper, cEventMgr* pEventMgr);
	
	void Register(cEvent& event, const cRegKey& sKey, tcstring sValueName, tcstring sValueData, uint32_t nARFlags);
	void ResolveAll(cEvent& event);
	bool Unregister(cPath& path);
	bool IsRegistered(cEvent& event, cPath& path, cRegAutoRun* pRegAutoRun);
	bool IsAutoRun(const cRegKey& sKey, tcstring sValueName, uint32_t* pnARFlags);

private:
	cEnvironmentHelper* m_pEnvironmentHelper;
	cEventMgr*          m_pEventMgr;
	tautorunvector m_list;
	cLock m_sync;
};


#endif // _EVTMGR_REGAR_H
