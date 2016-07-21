#ifndef _cEventHandlerHeurInvader__H_
#define _cEventHandlerHeurInvader__H_

#include "../eventmgr/include/eventhandler.h"
#include "mwlist.h"

class cEventHandlerHeurInvader : public cEventHandler
{
public:
	cEventHandlerHeurInvader();
	virtual ~cEventHandlerHeurInvader();

	_IMPLEMENT( OnProcessExitPost );
	_IMPLEMENT( OnThreadCreatePre );
	_IMPLEMENT( OnThreadSetContextPre );
	_IMPLEMENT( OnWriteProcessMemoryPre );
	_IMPLEMENT( OnMapViewOfSectionPost );
	_IMPLEMENT( OnSetWindowsHook );

private:
	bool IsInvader(cEvent& event, tPid dest_pid, tAddr addr);
	bool IsAddressNotExecutable(tPid src_pid, tPid dest_pid, tAddr addr);
	bool IsSelfInvader(cEvent& event, tPid dest_pid);
	cMemWriteList m_MemWriteList;
};

#endif // _EventHandlerPDM__H_
