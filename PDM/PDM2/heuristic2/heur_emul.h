#ifndef _cEventHandlerHeurEmul__H_
#define _cEventHandlerHeurEmul__H_

#include "../eventmgr/include/eventhandler.h"

class cEventHandlerHeurEmulVirus : public cEventHandler
{
public:
	_IMPLEMENT( OnFileModifyPost );
};

class cEventHandlerHeurEmulDownloader : public cEventHandler
{
public:
	_IMPLEMENT( OnDownloadFile );
	_IMPLEMENT( OnProcessCreatePre );

private:
	cPathPidListSafe m_downloaded_list;
};

#endif // _cEventHandlerHeurEmul__H_
