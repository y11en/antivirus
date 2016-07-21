#ifndef __procmonm
#define __procmonm

#include <Prague/pr_serializable.h>
#include <ProductCore/structs/s_ip.h>
#include "s_trmod.h"

#define pmc_PROCESS_MONITOR 0xCC540336
#define pm_PROCESS_MONITOR_SCHEDULE_GET_PREFETCH	0xCC540337
#define pm_PROCESS_MONITOR_ADD_ACTIVE_IMAGE			0xCC540338
#define pm_PROCESS_MONITOR_IS_ACTIVE_IMAGE			0xCC540339

#define pm_PROCESS_MONITOR_PROCESS_START		    0x010
#define pm_PROCESS_MONITOR_PROCESS_EXIT			    0x011
#define pm_PROCESS_MONITOR_PROCESS_GETACTIONMASK    0x012
#define pm_PROCESS_MONITOR_CHECKINTASKLIST			0x013
#define pm_PROCESS_MONITOR_PROCESSUPDATE			0x014
#define pm_PROCESS_MONITOR_GETINFO					0x015
#define pm_PROCESS_MONITOR_PROCESS_START_WITH_RATING 0x016


struct cPRCMRequest
{
	enum _eprcmAction
	{
		_ePrcmn_ExclOpenFiles = 1,
		_ePrcmn_ExclNet       = 2,
		_ePrcmn_ExclBehavior  = 3,
		_ePrcmn_ExclRegistry  = 4,
		_ePrcmn_ExclTraffic   = 5,
	};
	
	tDWORD			m_ProcessId;
	_eprcmAction	m_Request;
// additional data
	cIP        m_Host;         // IP-адрес сервера
	tWORD      m_nPort;        // Порт сервера
};

struct cProcMonNotify : public cSerializable
{
	cProcMonNotify() :
		m_ProcessId(0),
		m_ParentProcessId(0),
		m_tStart(0),
		m_tExit(0)
	{
		m_sImagePath.setCP(cCP_UNICODE);
	}
	
	DECLARE_IID( cProcMonNotify, cSerializable, PID_PROCESSMONITOR, 990 );

	cStringObj	m_sImagePath;   // Полное имя файла
	tDWORD		m_ProcessId;
	tDWORD		m_ParentProcessId;
	time_t		m_tStart;
	time_t		m_tExit;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cProcMonNotify, 0 )
	SER_VALUE_M( sImagePath,        tid_STRING_OBJ )
	SER_VALUE_M( ProcessId,			tid_DWORD )
	SER_VALUE_M( ParentProcessId,   tid_DWORD )
	SER_VALUE_M( tStart,			tid_DWORD )
	SER_VALUE_M( tStart,			tid_DWORD )
IMPLEMENT_SERIALIZABLE_END();

struct cProcMonNotifyEx : public cProcMonNotify
{
	cProcMonNotifyEx() : cProcMonNotify() {}
	
	DECLARE_IID( cProcMonNotifyEx, cProcMonNotify, PID_PROCESSMONITOR, pmst_cProcMonNotifyEx );

	cSecurityRating m_SecRating;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cProcMonNotifyEx, 0 )
	SER_BASE(cProcMonNotify, 0)
	SER_VALUE(m_SecRating, cSecurityRating::eIID, 0)
IMPLEMENT_SERIALIZABLE_END();


#endif
