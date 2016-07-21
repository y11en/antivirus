#ifndef _cEventHandlerHeurKillFiles__H_
#define _cEventHandlerHeurKillFiles__H_

#include "../eventmgr/include/eventhandler.h"
#include "../eventmgr/include/lock.h"


class cEventHandlerHeurKillFiles : public cEventHandler
{
public:
	//cEventHandlerHeurKillFiles();
	//virtual ~cEventHandlerHeurKillFiles();

	//bool Init();

	_IMPLEMENT( OnFileDeletePost );
	//_IMPLEMENT( OnFileCloseModifiedPost );

private:
	//long m_nModifiedFiles;
	//long m_nDeletedSystemFiles;
};

#endif // _EventHandlerPDM__H_
