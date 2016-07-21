#ifndef __S_REPORT_H
#define __S_REPORT_H

#include <ProductCore/report.h>
#include <ProductCore/plugin/p_reportdb.h>
#include <ProductCore/structs/s_taskmanager.h>
#include <ProductCore/structs/s_ip.h>

enum
{
	rs_cReportEvent = 1,
	rs_cReportEventEx,
	rs_cTaskReportEvent,
	rs_cReportParentIds,
	rs_cReportAttackInfo,
};

//-----------------------------------------------------------------------------

struct cReportEvent : public cSerializable, public tReportEvent
{
	cReportEvent(){ memset((tReportEvent*)this, 0, sizeof(tReportEvent)); }
	
	DECLARE_IID( cReportEvent, cSerializable, PID_REPORTDB, rs_cReportEvent );
};

IMPLEMENT_SERIALIZABLE_BEGIN( cReportEvent, 0 )
	SER_VALUE(m_Timestamp,		tid_QWORD,  "Timestamp" )
	SER_VALUE(m_Severity,		tid_BYTE,   "Severity" )
	SER_VALUE(m_EventID,		tid_WORD,   "EventID" )
	SER_VALUE(m_PID,			tid_QWORD,  "PID" )
	SER_VALUE(m_AppID,			tid_DWORD,  "AppID" )
	SER_VALUE(m_ObjectType,		tid_BYTE,   "ObjectType" )
	SER_VALUE(m_ObjectID,		tid_QWORD,  "ObjectID" )
	SER_VALUE(m_OldObjectID,	tid_QWORD,  "OldObjectID" )
	SER_VALUE(m_Action,			tid_BYTE,   "Action" )
	SER_VALUE(m_TaskGroup,		tid_BYTE,   "TaskGroup" )
	SER_VALUE(m_TaskID,			tid_BYTE,   "TaskID" )
	SER_VALUE(m_Verdict,		tid_WORD,   "Verdict" )
	SER_VALUE(m_DecisionReason,	tid_WORD,   "DecisionReason" )
	SER_VALUE(m_DetectType,		tid_BYTE,   "DetectType" )
	SER_VALUE(m_DetectName,		tid_QWORD,  "DetectName" )
	SER_VALUE(m_DetectDanger,	tid_BYTE,   "DetectDanger" )
	SER_VALUE(m_IsExact,		tid_BYTE,   "IsExact" )
	SER_VALUE(m_Error,			tid_ERROR,  "Error" )
	SER_VALUE(m_ExtraInfo,		tid_QWORD,  "ExtraInfo" )
	SER_VALUE(m_ExtraInfoSub1,	tid_DWORD,  "ExtraInfoSub1" )
	SER_VALUE(m_ExtraInfoSub2,	tid_DWORD,  "ExtraInfoSub2" )
	SER_VALUE(m_dExtraInfo,		tid_DWORD,  "dExtraInfo" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cReportEventEx : public cReportEvent
{
	cReportEventEx() :
		m_nEventId(0)
	{};

	DECLARE_IID( cReportEventEx, cReportEvent, PID_REPORTDB, rs_cReportEventEx );
	
	tQWORD  m_nEventId;
	cStrObj m_sObjectName;
	cStrObj m_sOldObjectName;
	cStrObj m_sDetectName;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cReportEventEx, 0 )
	SER_BASE( cReportEvent, 0 )
	SER_VALUE(m_nEventId,		tid_QWORD,       "EventId" )
	SER_VALUE(m_sObjectName,	tid_STRING_OBJ,  "ObjectNameStr" )
	SER_VALUE(m_sOldObjectName,	tid_STRING_OBJ,  "OldObjectNameStr" )
	SER_VALUE(m_sDetectName,	tid_STRING_OBJ,  "DetectNameStr" )
IMPLEMENT_SERIALIZABLE_END()

struct cTaskReportEvent : public cTaskHeader
{
	DECLARE_IID( cTaskReportEvent, cTaskHeader, PID_REPORTDB, rs_cTaskReportEvent );
	cReportEventEx m_Event;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskReportEvent, 0 )
	SER_BASE( cTaskHeader, 0 )
	SER_VALUE_M(Event,		cReportEventEx::eIID)
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cReportParentIds : public cSerializable
{
	DECLARE_IID( cReportParentIds, cSerializable, PID_REPORTDB, rs_cReportParentIds );

	cVector<tQWORD>  m_vParentIds;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cReportParentIds, 0 )
	SER_VECTOR(m_vParentIds,  tid_QWORD, 0 )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cReportAttackInfo : public cSerializable
{
	cReportAttackInfo() :
		m_Proto(0),
		m_LocalPort(0),
		m_ResponseFlags(0)
	{
	}
	
	DECLARE_IID( cReportAttackInfo, cSerializable, PID_REPORTDB, rs_cReportAttackInfo );

	tBYTE				m_Proto;				// протокол
	tWORD				m_LocalPort;			// порт
	tDWORD				m_ResponseFlags;		// флаги ответа (см. $/windows/Hook/Plugins/Include/attacknotify_api.h)
	cIP				    m_AttackerIP;			// IP-адрес атакующего
};

IMPLEMENT_SERIALIZABLE_BEGIN( cReportAttackInfo, 0 )
	SER_VALUE( m_Proto,				tid_BYTE,		"Proto" )
	SER_VALUE( m_LocalPort,			tid_WORD,		"LocalPort" )
	SER_VALUE( m_ResponseFlags,		tid_DWORD,		"ResponseFlags" )
    SER_VALUE_C( m_AttackerIP,		cIP::eIID,		"AttackerIP", &g_SkipFieldAfterV1_Serializer )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

#endif//  __S_REPORT_H
