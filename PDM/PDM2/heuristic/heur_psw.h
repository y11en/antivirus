#ifndef _cEventHandlerHeurPsw__H_
#define _cEventHandlerHeurPsw__H_

#include "../eventmgr/include/eventhandler.h"

class cEventHandlerHeurPsw : public cEventHandler
{
public:
	cEventHandlerHeurPsw();
	~cEventHandlerHeurPsw();
	bool Init();

	_IMPLEMENT( OnFileCreateFail );
	_IMPLEMENT( OnProtectedStorageAccessPre );

private:
	size_t m_slot_index;
	tstring * psw_expanded_folders;
	void iOnPassAccess(cEvent& event, cFile& file, size_t nFolderIndex);
	void iOnLegitAccess(cEvent& event, cFile& file, size_t nFolderIndex);
};

#endif // _cEventHandlerHeurPsw__H_
