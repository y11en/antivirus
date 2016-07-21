#ifndef _cEventHandlerHeurDelSelf__H_
#define _cEventHandlerHeurDelSelf__H_

#include "../eventmgr/include/eventhandler.h"

class cEventHandlerHeurDelSelf : public cEventHandler
{
public:
	_IMPLEMENT( OnProcessCreatePre );

private:
	void iParseBat(cEvent& event, cFile& bat_file, tcstring cmd_line, bool bParseCmdLine);
};

#endif // _cEventHandlerHeurDelSelf__H_
