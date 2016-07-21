#ifndef _cEventHandlerHeurVirus__H_
#define _cEventHandlerHeurVirus__H_

#include "../eventmgr/include/eventhandler.h"
#include <vector>
#include "../eventmgr/include/lock.h"
#include "../eventmgr/include/opcachef.h"

/*
typedef struct tag_MODIFY_PENDING_LIST_ITEM
{
	tcstring fullpath;
	tPid pid;
	FILE_FORMAT_INFO fi;
} MODIFY_PENDING_LIST_ITEM;

typedef std::vector<MODIFY_PENDING_LIST_ITEM> tmodifypendingvector;

*/
class cEventHandlerHeurVirus : public cEventHandler
{
public:
	cEventHandlerHeurVirus();
	virtual ~cEventHandlerHeurVirus();

	_tOnFileCloseModifiedPre iOnFileCloseModified;

	_IMPLEMENT( OnFileCloseModifiedPre );
	_IMPLEMENT( OnFileCloseModifiedPost );

private:
	cOpCacheFile m_OpCacheFileCloseModified;
	cPath m_sProgramFilesMask;
	bool Init();

	bool AnalyzeModifications(cEvent& event, cProcess& pProcess, cFile& file, FILE_FORMAT_INFO* fi_old);
	bool AnalyzeModifications1(cEvent& event, cProcess& pProcess, cFile& file, FILE_FORMAT_INFO* efi_old);
	bool AnalyzeModifications2(cEvent& event, cProcess& pProcess, cFile& file, FILE_FORMAT_INFO* efi_old);
//	bool AnalyzeModifications3(cEvent& event, cProcess& pProcess, cFile& file, FILE_FORMAT_INFO* efi_old);
};

#endif // _EventHandlerPDM__H_
