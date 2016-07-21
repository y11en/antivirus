#ifndef __SysWatchEventHandler_H_
#define __SysWatchEventHandler_H_

#include <../../PDM/PDM2/eventmgr/include/eventhandler.h>
#include <ProductCore/plugin/p_syswatch.h>
#include <ProductCore/iface/i_task.h>

class cSysWatchEventHandler : public cEventHandler
{
public:
	cSysWatchEventHandler(hTASK hTask, hREPORTDB hDB);
	~cSysWatchEventHandler();

	_IMPLEMENT(OnFileCreatePost);
	_IMPLEMENT(OnFileModifyPost);
	_IMPLEMENT(OnFileDeletePost);
	_IMPLEMENT(OnRegValueSetPost);
	_IMPLEMENT(OnRegValueSetStrPost);
private:
	hTASK m_hTask;
	hREPORTDB m_hDB;

};

#endif __SysWatchEventHandler_H_

