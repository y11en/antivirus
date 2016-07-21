#ifndef _cEventHandlerHeurDumb__H_
#define _cEventHandlerHeurDumb__H_

#include "../eventmgr/include/eventhandler.h"
#include "../eventmgr/include/pathlist.h"

class cEventHandlerHeurDumb : public cEventHandler
{
public:
	_IMPLEMENT( OnRegValueSetStrPre );
	_IMPLEMENT( OnRegValueSetPre );
	_IMPLEMENT( OnFindWindowFail );

public:
	bool Init();

private:
	size_t m_slot_index;
};

#endif // _cEventHandlerHeurDumb__H_
