#ifndef _cEventHandlerHeurKillAV__H_
#define _cEventHandlerHeurKillAV__H_

#include "../eventmgr/include/eventhandler.h"

class cEventHandlerHeurKillAV : public cEventHandler
{
public:
	cEventHandlerHeurKillAV();
	virtual ~cEventHandlerHeurKillAV();

	_IMPLEMENT( OnProcessCreatePost );
	_IMPLEMENT( OnProcessTerminatePre );
	_IMPLEMENT( OnRegValueSetStrPre );
	_IMPLEMENT( OnKillService );
	_IMPLEMENT( OnAntiAV );
};

#endif // _EventHandlerPDM__H_
