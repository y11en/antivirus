#ifndef _cEventHandlerHeurInstall__H_
#define _cEventHandlerHeurInstall__H_

#include "../eventmgr/include/eventhandler.h"

class cEventHandlerHeurInstall : public cEventHandler
{
public:
	cEventHandlerHeurInstall();
	virtual ~cEventHandlerHeurInstall();

	_IMPLEMENT( OnRegKeyCreatePost );
	_IMPLEMENT( OnRegValueSetPre );
	_IMPLEMENT( OnRegValueSetStrPre );
	_IMPLEMENT( OnFileCloseModifiedPost );
	_IMPLEMENT( OnRegAutoRunRegisterResolved );
	_IMPLEMENT( OnInstallDriver );
	_IMPLEMENT( OnInstall );

	bool Init();

private:
	tPid iFindProcessByDroppedFile(tcstring file);
	void iIsInstallInStream(cEvent& event, cPath& file);
	cPathPidListSafe m_registered_drivers;
	tstring m_drivers_path;
};

#endif // _EventHandlerPDM__H_
