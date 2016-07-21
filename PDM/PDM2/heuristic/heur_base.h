#ifndef _cEventHandlerHeuristic__H_
#define _cEventHandlerHeuristic__H_

#include "../eventmgr/include/eventhandler.h"
#include "../eventmgr/include/opcachef.h"

#include "sclist.h"
#include "p2p.h"
#include "regar.h"

class cEventHandlerHeuristic : public cEventHandler
{
public:
	cEventHandlerHeuristic();
	~cEventHandlerHeuristic();

	bool Init();

	_tOnFileCloseModifiedPre iOnFileCloseModified;

	_IMPLEMENT( OnFileCloseModifiedPre );
	_IMPLEMENT( OnFileCloseModifiedPost );
	_IMPLEMENT( OnSelfCopy );
	_IMPLEMENT( OnRegValueSetStrPre );
	_IMPLEMENT( OnRegValueSetStrPost );
	_IMPLEMENT( OnRegAutoRunRegisterResolved );
	_IMPLEMENT( OnFileRenamePost );
	_IMPLEMENT( OnProcessCreatePre );
	_IMPLEMENT( OnRegKeyCreatePost );
	_IMPLEMENT( OnCopyFilePre );
	_IMPLEMENT( OnOpenSectionPre );
	_IMPLEMENT( OnFileDeletePost );

private:
	void CheckAutorunInf(cEvent & evt, cFile & file);

private:
	cPathPidListSafe m_selfcopy_list;
	cPeer2PeerPaths  m_Peer2PeerPaths;
	cPathList        m_StartupPaths;
	cRegAutoRunList  m_RegAutoRuns;
	cOpCacheFile     m_OpCacheFileCloseModified;
};

#endif // _EventHandlerPDM__H_
