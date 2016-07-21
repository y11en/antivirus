#ifndef __EVTMGR_P2P_H
#define __EVTMGR_P2P_H

#include "../eventmgr/include/types.h"
#include "../eventmgr/include/envhelper.h"
#include "../eventmgr/include/pathlist.h"
#include "../eventmgr/include/lock.h"

class cPeer2PeerPaths {
public:
	cPeer2PeerPaths();
	~cPeer2PeerPaths();

	bool SearchPeer2PeerPaths(cEnvironmentHelper* pEnvironmentHelper);
	bool AddPeer2PeerPath(cPath& path);
	bool IsPeer2PeerPath(cPath& path);
	bool IsP2PKey(tcstring pwchKeyName, tcstring pwchValueName);

private:
	cPathList m_list;
	cLock m_sync;
};

#endif // __EVTMGR_P2P_H
