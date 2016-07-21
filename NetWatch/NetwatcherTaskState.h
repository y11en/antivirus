#include <prague/prague.h>
#include <Prague/pr_serializable.h>
#include <windows.h>
class CNetWatcherTaskState
{
public:
	CNetWatcherTaskState();
	~CNetWatcherTaskState();
	void SaveState(tDWORD);
	tDWORD QState();
	tDWORD QStateOld();
private:
	tDWORD	m_state;
	tDWORD	m_state_old;
	CRITICAL_SECTION m_cs_state;
};