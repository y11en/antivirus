#ifndef _EventHandlerPDM__H_
#define _EventHandlerPDM__H_

#include "../pdm2/eventmgr/include/eventhandler.h"
#include <ProductCore/plugin/p_sfdb.h>

class cEventHandlerPDM : public cEventHandler
{
public:
	cEventHandlerPDM();
	virtual ~cEventHandlerPDM();

	_IMPLEMENT( OnSelfCopy );
	_IMPLEMENT( OnRegAvpImageFileExecOptions);

	_IMPLEMENT( OnSelfCopyMultiple );
	_IMPLEMENT( OnSelfCopyToNetwork );
	_IMPLEMENT( OnSelfCopyToPeerToPeerFolder );
	_IMPLEMENT( OnSelfCopyToStartup );
	
	_IMPLEMENT( OnSelfCopyAutoRun );


	//<<
	hSECUREFILEDB	m_hSFDB_KL;	//	pointer to "white list" base
	cObject*		m_pThis;	//	pointer to prague component (for access to sys-functions)
	//>>
	bool CheckExcludeEvent(cEvent& event, unsigned int l_EventType);


private:
	bool MakeRequest (
		cEvent& event,
		unsigned int EventType,
		unsigned int EventSubType = 0
		);

};

#endif // _EventHandlerPDM__H_