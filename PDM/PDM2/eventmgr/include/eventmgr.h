#ifndef _cEventMgr_H_
#define _cEventMgr_H_

#include "eventhandler.h"
#include "opcache.h"
#include "../../heuristic/proclist.h"
#include "envhelper.h"
#include "cregkey.h"

class cEventMgr : public cEventHandler
{
public:
	cEventMgr(cEnvironmentHelper* pEnvironmentHelper);
	~cEventMgr();

	void __OnFileCreatePre(cEvent& event, cFile& file, uint32_t desired_access, uint32_t share_mode, uint32_t creation_disposition);
	bool RegisterHandler(cEventHandler* pHandler, bool bDelete);
	bool UnregisterHandler(cEventHandler* pHandler);
	
	_IMPLEMENT(OnProcessCreatePre);
	_IMPLEMENT(OnProcessCreatePost);
	_IMPLEMENT(OnProcessExitPre);
	_IMPLEMENT(OnProcessExitPost);
	_IMPLEMENT(OnThreadCreatePre);
	_IMPLEMENT(OnThreadCreatePost);
	_IMPLEMENT(OnThreadTerminatePost);

	_IMPLEMENT( OnFileCreatePre );
	_IMPLEMENT( OnFileCreatePost );
	_IMPLEMENT( OnFileCreateFail );

	_IMPLEMENT( OnFileCreateNewPre );
	_IMPLEMENT( OnFileCreateNewPost );
	_IMPLEMENT( OnFileCreateNewFail );

	_IMPLEMENT( OnFileModifyPre );
	_IMPLEMENT( OnFileModifyPost );
	_IMPLEMENT( OnFileModifyFail );

	_IMPLEMENT( OnFileDeletePre );
	_IMPLEMENT( OnFileDeletePost );
	_IMPLEMENT( OnFileDeleteFail );

	_IMPLEMENT( OnFileRenamePre );
	_IMPLEMENT( OnFileRenamePost );
	_IMPLEMENT( OnFileRenameFail );

	_IMPLEMENT( OnWriteProcessMemoryPre );

private:
};


#endif
