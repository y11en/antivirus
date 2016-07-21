#ifndef _pdm2rt_task_structs_
#define _pdm2rt_task_structs_

#include <ProductCore/structs/s_taskmanager.h>
#include <AV/structs/s_avs.h>
#include "../plugin/p_pdm2rt.h"

enum PDM2RTActions { pdm2_allow = 0, pdm2_ask, pdm2_block, pdm2_terminate, pdm2_quarantine };

typedef enum enPdm2EventType {
	PDM2_EVENT_UNKNOWN        = 0,	
	PDM2_EVENT_P2P_SC_RDL     = 1,	// pdm - suspicions actions
	PDM2_EVENT_SC_AR          = 2,	// pdm - suspicions actions
	PDM2_EVENT_SC_ARsrc       = 3,	// pdm - suspicions actions
	PDM2_EVENT_SCN            = 4,	// pdm - suspicions actions
	PDM2_EVENT_P2P_SCN        = 5,	// pdm - suspicions actions
	PDM2_EVENT_SC2STARTUP     = 6,	// pdm - suspicions actions
	PDM2_EVENT_SC_MULTIPLE    = 7,	// pdm - suspicions actions
	PDM2_EVENT_RDR            = 8,	// pdm - suspicions actions
	PDM2_EVENT_HIDDEN_OBJ     = 10,	// pdm - found rootkit
	PDM2_EVENT_INVADER        = 11,	// pdm - suspicions actions
	PDM2_EVENT_INVADER_LOADER = 17,	// pdm - load dll in all process using SetWindowsHook
	PDM2_EVENT_STRANGEKEY		= 18,	// pdm - working with strange regestry key
	PDM2_EVENT_SYSCHANGE		= 19,	// pdm - strange sys changes
	PDM2_EVENT_HIDDEN_INSTALL = 20,	// pdm - suspicions actions
	PDM2_EVENT_BUFFEROVERRUN  = 21,	// pdm - suspicions actions
	PDM2_EVENT_DEP			= 22,	// pdm - suspicions actions DEP
	PDM2_EVENT_HOSTS			= 23,	// modifications of hosts file
	PDM2_EVENT_DOWNLOADER		= 25,	// hidden install -> trojan downloader
	PDM2_EVENT_TROJAN_GEN		= 26,	// generic trojan
	PDM2_EVENT_DRIVER_INS		= 28,	// suspicion driver registration
	PDM2_EVENT_HIDDEN_SEND	= 29,	// suspicion data send
	PDM2_EVENT_MAX            = 30,
} tPdm2EventType;

typedef enum enPdm2EventSubType {
	PDM2_SUBEVENT_UNKNOWN     = 0,	
	PDM2_SUBEVENT_CRYPTOR     = 1,	// pdm - suspicions actions : PDM2_EVENT_TROJAN_GEN
	PDM2_SUBEVENT_INFECTOR    = 2,	// pdm - suspicions actions : PDM2_EVENT_TROJAN_GEN
	PDM2_SUBEVENT_ANTI_AV     = 3,	// pdm - suspicions actions : PDM2_EVENT_TROJAN_GEN
	PDM2_SUBEVENT_PSW_STEALER = 4,	// pdm - suspicions actions : PDM2_EVENT_TROJAN_GEN
} tPdm2EventSubType;
//-----------------------------------------------------------------------------
//! Статистика, предоставляемая PDM

struct cPDM2rtStatistics: public cTaskStatistics
{
	cPDM2rtStatistics(): cTaskStatistics(),
		m_nDriverEvents(0),
		m_nProcessedEvents(0),

		m_nTreats(0),
		m_nTreatsAllowed(0),
		m_nTreatsBlocked(0),
		m_nTreatsQuarantined(0),
		m_nTreatsTerminated(0)
	{}

	DECLARE_IID_STATISTICS(cPDM2rtStatistics, cTaskStatistics, PID_PDM2RT);

	tUINT             m_nDriverEvents;               // Number of initial (driver) events
	tUINT             m_nProcessedEvents;            // Number of processed events
	tDATETIME		  m_timeLastEvent;               // Last event time

	tUINT             m_nTreats;                     // Number of detected treats
	tUINT             m_nTreatsAllowed;              // Number of treats allowed to execute
	tUINT             m_nTreatsBlocked;              // Number of blocked treats
	tUINT             m_nTreatsQuarantined;          // Number of quarantined treats
	tUINT             m_nTreatsTerminated;           // Number of terminated programs(processes)
	tDATETIME         m_timeLastTreat;               // Last treat time
};

IMPLEMENT_SERIALIZABLE_BEGIN( cPDM2rtStatistics, 0 )
	SER_BASE( cTaskStatistics, 0 )
	SER_VALUE_M( nDriverEvents, tid_UINT )
	SER_VALUE_M( nProcessedEvents, tid_UINT )
	SER_VALUE_M( timeLastEvent, tid_DATETIME )

	SER_VALUE_M( nTreats, tid_UINT )
	SER_VALUE_M( nTreatsAllowed, tid_UINT )
	SER_VALUE_M( nTreatsBlocked, tid_UINT )
	SER_VALUE_M( nTreatsQuarantined, tid_UINT )
	SER_VALUE_M( nTreatsTerminated, tid_UINT )
	SER_VALUE_M( timeLastTreat, tid_DATETIME )
IMPLEMENT_SERIALIZABLE_END()

//+ ------------------------------------------------------------------------------------------

struct cPDM2rtSettings: public cTaskSettings
{
	cPDM2rtSettings(): 
		cTaskSettings(),
		m_bWatchSystemAccount(cFALSE),
		m_Action(pdm2_ask),
		m_bLog(cTRUE)
	{
	}

	DECLARE_IID_SETTINGS(cPDM2rtSettings, cTaskSettings, PID_PDM2RT);

	tBOOL							m_bWatchSystemAccount;
	PDM2RTActions					m_Action;
	tBOOL							m_bLog;

};

IMPLEMENT_SERIALIZABLE_BEGIN( cPDM2rtSettings, 0 )
	SER_BASE( cTaskSettings, 0 )
	SER_VALUE_M(bWatchSystemAccount,		tid_BOOL)
	SER_VALUE_M(Action,						tid_DWORD)
	SER_VALUE_M(bLog,						tid_BOOL)
IMPLEMENT_SERIALIZABLE_END()

//+ ------------------------------------------------------------------------------------------

struct cPdm2rtProcInfo : public cSerializable 
{
	cPdm2rtProcInfo() : cSerializable(),
		m_ProcessId(0),
		m_ImagePath(),
		m_bHidden(cFALSE)
		{
		}

	DECLARE_IID(cPdm2rtProcInfo, cSerializable, PID_PDM2RT, 3);

	tQWORD			m_ProcessId;
	cStringObj		m_ImagePath;
	cStringObj		m_CommandLine;
	tDATETIME		m_StartTime;
	tDATETIME		m_StopTime;
	tBOOL			m_bHidden;
	cStringObj		m_Version;
	cStringObj		m_Vendor;
	cStringObj		m_Description;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cPdm2rtProcInfo, "Pdm2rtProcList")
	SER_VALUE_M(ProcessId,			tid_QWORD)
	SER_VALUE_M(ImagePath,			tid_STRING_OBJ)
	SER_VALUE_M(CommandLine,		tid_STRING_OBJ)
	SER_VALUE_M(StartTime,			tid_DATETIME)
	SER_VALUE_M(StopTime,			tid_DATETIME)
	SER_VALUE_M(bHidden,			tid_BOOL)
	SER_VALUE_M(Version,			tid_STRING_OBJ)
	SER_VALUE_M(Vendor,				tid_STRING_OBJ)
	SER_VALUE_M(Description,		tid_STRING_OBJ)
IMPLEMENT_SERIALIZABLE_END()

struct cAskObjectAction_PDM2 : public cAskObjectAction
{
	cAskObjectAction_PDM2() : cAskObjectAction(),
		m_EventType(PDM2_EVENT_UNKNOWN),
		m_EventSubType(PDM2_SUBEVENT_UNKNOWN),
		m_nError(errNOT_OK)
	{
	};

	DECLARE_IID(cAskObjectAction_PDM2, cAskObjectAction, PID_PDM2RT, 2);

	enum Actions {
		ASK = eIID,
		DETECT,
		ROLLBACK,
		TERMINATE_FAILED,
		ROLLBACK_RESULT,
		TERMINATE_INFO,
		QUARANTINE_FAILED,
	};
	Actions						m_Action;

	tPdm2EventType				m_EventType;
	tPdm2EventSubType			m_EventSubType;

	cPdm2rtProcInfo				m_CurrentProcess;
	cPdm2rtProcInfo				m_ParentProcess;

	cVector<cPdm2rtProcInfo>	m_ChildProcList;

	cVector<tBYTE>				m_Value;
	cVector<tBYTE>				m_ValueOld;
	cStringObj					m_AdditionalInfo;


	tERROR						m_nError;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cAskObjectAction_PDM2, "cAskObjectAction_PDM2")
	SER_BASE(cAskObjectAction,			0)
	SER_VALUE_M(Action,					tid_DWORD)
	SER_VALUE_M(EventType,				tid_DWORD)
	SER_VALUE_M(EventSubType,			tid_DWORD)
	
	SER_VALUE_M(CurrentProcess,			cPdm2rtProcInfo::eIID)
	SER_VALUE_M(ParentProcess,			cPdm2rtProcInfo::eIID)

	SER_VECTOR_M(ChildProcList,			cPdm2rtProcInfo::eIID)
	SER_VECTOR_M(Value,					tid_BYTE)
	SER_VECTOR_M(ValueOld,				tid_BYTE)
	SER_VALUE_M(nError,					tid_ERROR)
	SER_VALUE_M(AdditionalInfo,			tid_STRING_OBJ)
IMPLEMENT_SERIALIZABLE_END()


#endif // _pdm2rt_task_structs_
