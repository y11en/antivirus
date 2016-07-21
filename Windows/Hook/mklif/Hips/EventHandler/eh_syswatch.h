#ifndef __SYSTEM_WATCHER__EVENT_HANDLER
#define __SYSTEM_WATCHER__EVENT_HANDLER

#include <syswatch.h>
#include <eventmgr\include\eventhandler.h>

class cEHSysWatch : public cSystemWatcher , public cEventHandler
{
public:
	bool Init();

	_IMPLEMENT(OnProcessCreatePost);	
	_IMPLEMENT(OnProcessExitPost);

	_IMPLEMENT(OnFileCreateNewPost);
	_IMPLEMENT(OnFileModifyPost);
	_IMPLEMENT(OnFileDeletePost);
	_IMPLEMENT(OnFileRenamePost);

	_IMPLEMENT(OnRegKeyCreatePost);
	_IMPLEMENT(OnRegKeyRenamePost);
	_IMPLEMENT(OnRegKeyDeletePost);

	_IMPLEMENT(OnRegValueSetPost);
	_IMPLEMENT(OnRegValueDeletePost);

private:
};


#endif __SYSTEM_WATCHER__EVENT_HANDLER
