#ifndef _ids_task_structs_
#define _ids_task_structs_

#include <AHTasks/plugin/p_idstask.h>
#include <AV/structs/s_avs.h>
#include <ProductCore/structs/s_ip.h>
#include <ProductCore/structs/s_taskmanager.h>

typedef enum
{
        _ids_SETTINGS = 1,
        _ids_STATISTICS,
		_ids_cIdsBannedHost,
        _ids_LAST
} _ids_t;

//-----------------------------------------------------------------------------

struct cIDSSettings: public cTaskSettings
{
	cIDSSettings(): cTaskSettings(),
		m_bEnableBan(cTRUE),
		m_BanTimeout(60),		// 60 min
		m_bAlertOnDetect(cTRUE)
	{
	}

	DECLARE_IID_SETTINGS(cIDSSettings, cTaskSettings, PID_IDSTASK);

	tBOOL			m_bEnableBan;
	tDWORD			m_BanTimeout;
	tBOOL			m_bAlertOnDetect;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cIDSSettings, 0 )
	SER_BASE( cTaskSettings,  0 )
	SER_VALUE( m_bEnableBan, tid_BOOL, "EnableBan" )
	SER_VALUE( m_BanTimeout, tid_DWORD, "BanTimeout" )
	SER_VALUE( m_bAlertOnDetect, tid_BOOL, "AlertOnDetect" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
//! Статистика, предоставляемая IDS

struct cIdsBannedHost : public cSerializable
{
	cIdsBannedHost() :
		m_IP(0),
		m_Time(0)
	{}
	
	DECLARE_IID(cIdsBannedHost, cSerializable, PID_IDSTASK, _ids_cIdsBannedHost);
	
	cIP m_IP;                                        // banned host ip address
	__time32_t m_Time;                                      // the time then host has been banned
};

typedef cVector<cIdsBannedHost> cIdsBannedHosts;

IMPLEMENT_SERIALIZABLE_BEGIN( cIdsBannedHost, 0 )
    SER_VALUE( m_IP,                  cIP::eIID,            "IP" )
	SER_VALUE( m_Time,                tid_DWORD,            "Time" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

#define pmc_TASKAHIDS_UNBAN_HOST 0xa7afd216 //is a crc32 for "pmc_TASKAHIDS_UNBAN_HOST"

struct cIDSStatistics: public cTaskStatistics
{
	cIDSStatistics(): cTaskStatistics(),
		m_nNumAttacksDetected(0),
		m_timeLastObject(-1)
	{}

	DECLARE_IID_STATISTICS(cIDSStatistics, cTaskStatistics, PID_IDSTASK);

	tUINT             m_nNumAttacksDetected;            // Количество обнаруженных атак
	__time32_t        m_timeLastObject;                 // Время обнаружения последней атаки
	cIdsBannedHosts   m_aBannedHosts;                   // banned hosts
};

IMPLEMENT_SERIALIZABLE_BEGIN( cIDSStatistics, 0 )
	SER_BASE( cTaskStatistics,  0 )
	SER_VALUE( m_nNumAttacksDetected, tid_UINT,             "AttacksDetected" )
	SER_VALUE( m_timeLastObject,      tid_DWORD,            "AttackLastTime" )
	SER_VECTOR(m_aBannedHosts,        cIdsBannedHost::eIID, "BannedHosts" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//! событие IDS
#define pmc_TASKIDS_REPORT_EVENT 0xa663facc /*is a crc32 for "TASKIDS_REPORT_EVENT"*/

struct cIDSEventReport: public cTaskHeader
{
	cIDSEventReport(): cTaskHeader(),
		m_ObjectType(OBJTYPE_NETWORK),
		m_ObjectStatus(OBJSTATUS_UNKNOWN),
		m_DetectType(DETYPE_UNKNOWN),
		m_AttackDescription(),
		m_Proto(0),
		m_AttackerIP_(0),
		m_LocalPort(0),
		m_ResponseFlags(0),
		m_Time(-1)
	{
		m_AttackDescription.setCP(cCP_UNICODE);
	}

	DECLARE_IID(cIDSEventReport, cTaskHeader, PID_IDSTASK, 0x70);

    void pr_call finalizeStreamDeser( tERROR err, tUINT field_num, const cSerDescriptor* dsc )
    {
        if (field_num == 9) // pre-сIP version
        {
            m_AttackerIP.SetV4(m_AttackerIP_);
        }
    }

	enObjectType		m_ObjectType;
	enObjectStatus		m_ObjectStatus;
	enDetectType		m_DetectType;

	cStringObj			m_AttackDescription;	// название атаки (нуль-терминированная юникод-строка)
	tBYTE				m_Proto;				// протокол
	tIPv4				m_AttackerIP_;			// deprecated
	tWORD				m_LocalPort;			// порт
	tDWORD				m_ResponseFlags;		// флаги ответа (см. $/windows/Hook/Plugins/Include/attacknotify_api.h)
	__time32_t			m_Time;					// время атаки (UTC FILETIME)
	cIP				    m_AttackerIP;			// IP-адрес атакующего
};

IMPLEMENT_SERIALIZABLE_BEGIN( cIDSEventReport, 0 )
	SER_BASE( cTaskHeader,  0 )
	SER_VALUE( m_ObjectType,		tid_DWORD,		"ObjectType" )
	SER_VALUE( m_ObjectStatus,		tid_DWORD,		"ObjectStatus" )
	SER_VALUE( m_DetectType,		tid_DWORD,		"DetectType" )
	SER_VALUE( m_AttackDescription,	tid_STRING_OBJ,	"AttackDescription" )
	SER_VALUE( m_Proto,				tid_BYTE,		"Proto" )
	SER_VALUE( m_AttackerIP_,		tid_IPv4,		"AttackerIP" )
	SER_VALUE( m_LocalPort,			tid_WORD,		"LocalPort" )
	SER_VALUE( m_ResponseFlags,		tid_DWORD,		"ResponseFlags" )
	SER_VALUE( m_Time,				tid_DWORD,		"AttackTime" )
    SER_VALUE_C( m_AttackerIP,		cIP::eIID,		"AttackerIPv", &g_SkipFieldAfterV1_Serializer )
IMPLEMENT_SERIALIZABLE_END()


#endif // _ids_task_structs_
