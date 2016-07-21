#ifndef _EventHandlerPDM__H_
#define _EventHandlerPDM__H_

#include "../eventmgr/include/eventhandler.h"

class cEventHandlerPDM : public cEventHandler
{
public:
	cEventHandlerPDM();
	virtual ~cEventHandlerPDM();

	_IMPLEMENT(OnRegAvpImageFileExecOptions);
	_IMPLEMENT(OnSelfCopyMultiple);
	_IMPLEMENT(OnSelfCopyToNetwork);
	_IMPLEMENT(OnSelfCopyToPeerToPeerFolder);
	_IMPLEMENT(OnSelfCopyToStartup);
	_IMPLEMENT(OnSelfCopyAutoRun);

private:
};

#endif // _EventHandlerPDM__H_