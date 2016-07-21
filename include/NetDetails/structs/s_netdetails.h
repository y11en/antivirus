#ifndef _sniffer_task_structs_
#define _sniffer_task_structs_

#include <ProductCore/structs/s_taskmanager.h>
#include <ProductCore/report.h>
#include <ProductCore/structs/s_ip.h>
#include <NetDetails/plugin/p_netdetails.h>

enum _eNetDetails_IID
{
	snfr_Settings = 1,
	snfr_AskAction,
	snfr_FrameReport,
	snfr_FrameSummary,
	snfr_FrameDetails,
};

typedef cVector<tBYTE> cSnifferData;

//////////////////////////////////////////////////////////////
//	struct cNetDetailsSettings - настройки задачи NetDetails (прототип)
////////////////////////////////////////////////////////////////

struct cNetDetailsSettings: public cTaskSettings
{
	cNetDetailsSettings(): cTaskSettings()	{}

	DECLARE_IID_SETTINGS(cNetDetailsSettings, cTaskSettings, PID_NETDETAILS);
};
//-----------------------------------------------------------------------
IMPLEMENT_SERIALIZABLE_BEGIN( cNetDetailsSettings, 0 )
	SER_BASE( cTaskSettings,  0 )
IMPLEMENT_SERIALIZABLE_END()

//////////////////////////////////////////////////////
//	struct cSnifferAskAction - формат показа деталей 1-го репорта
//////////////////////////////////////////////////////

struct cSnifferAskAction: public cSerializable
{
	cSnifferAskAction() : cSerializable(),	m_pData(0), m_pDetails(0) {}
	DECLARE_IID(cSnifferAskAction, cSerializable, PID_NETDETAILS, snfr_AskAction);

	tPTR	m_pData;	// sniffer frame details blob
	tPTR	m_pDetails;	// sniffer output cSnifferFrameDetailsItem
};
//-----------------------------------------------------------------------
IMPLEMENT_SERIALIZABLE_BEGIN(cSnifferAskAction, 0)
	SER_VALUE(m_pDetails,	tid_PTR,	0)
	SER_VALUE(m_pData,		tid_PTR,	0 )
IMPLEMENT_SERIALIZABLE_END()


//////////////////////////////////////////////////////
//	struct tSnifferFrameReport
//////////////////////////////////////////////////////

struct tIPv6
{
	tIPv6() :  m_Lo(0), m_Hi(0), m_Zone(0) {}

/*	union
    {
        struct {
			tQWORD m_Lo;
			tQWORD m_Hi;
        };
        tDWORD  m_Dwords[4];
        tWORD   m_Words[8];
        tBYTE   m_Bytes[16];
    };
*/
    tQWORD m_Lo;
    tQWORD m_Hi;
    tDWORD  m_Zone;
};

////////////////////////////////////////////////////////////
//	struct tSnifferFrameReport - формат хранения в ReportDB
////////////////////////////////////////////////////////////

struct tSnifferFrameReport
{
	enum { eIPVer4 = 4, eIPVer6 = 6 };

	tSnifferFrameReport() :
		m_Timestamp(-1),
		m_Proto(0xff),
		m_HighProto(0xff),
		m_EType(0xffff),
		m_IPVersion(4),
		m_ObjectID(0)
	{}

	__time32_t  m_Timestamp;	// current time
	tBYTE       m_Proto;		// протокол
	tBYTE       m_HighProto;	// протокол верхнего уровня
    tWORD       m_EType;		// Ethernet type
	tBYTE       m_IPVersion;	// тип IP (4 = IPv4/ 6 = IPv6)
	tIPv6		m_IPSrc;		// IP address источника (IPv4/IPv6), если m_IPVersion = 4 используем m_Zone
	tIPv6		m_IPTgt;		// IP address (IPv4/IPv6) источника
	tObjectId   m_ObjectID;		// object id -> ObjectDB
};

//////////////////////////////////////////////////////////////
//	struct cSnifferFrameReport - формат для показа в ListView alias(netdetails) 
////////////////////////////////////////////////////////////////

struct cSnifferFrameReport : public cSerializable, public tSnifferFrameReport
{
	cSnifferFrameReport() :
		m_IPSource(0),
		m_IPTarget(0)
	{ }

	DECLARE_IID( cSnifferFrameReport, cSerializable, PID_NETDETAILS, snfr_FrameReport );

	cIP	m_IPSource;		// IP address (IPv4/IPv6) источника
	cIP	m_IPTarget;		// IP address (IPv4/IPv6) источника

	// конвертор tIPv6 в cIP
	void UpdateIP()
	{
		if( m_IPVersion == eIPVer4 )
		{
			m_IPSource.SetV4((tIPv4)m_IPSrc.m_Zone);
			m_IPTarget.SetV4((tIPv4)m_IPTgt.m_Zone);
		}
		else if( m_IPVersion == eIPVer6 )
		{
			cIPv6 ip;

			ip.m_Lo = m_IPSrc.m_Lo;
			ip.m_Hi = m_IPSrc.m_Hi;
			ip.m_Zone = m_IPSrc.m_Zone;
			m_IPSource.SetV6(ip);

			ip.m_Lo = m_IPTgt.m_Lo;
			ip.m_Hi = m_IPTgt.m_Hi;
			ip.m_Zone = m_IPTgt.m_Zone;
			m_IPTarget.SetV6(ip);
		}
	}
};
//-----------------------------------------------------------------------
IMPLEMENT_SERIALIZABLE_BEGIN( cSnifferFrameReport, 0 )
	SER_VALUE( m_Timestamp,		tid_DWORD, "Timestamp" )
	SER_VALUE( m_Proto,			tid_BYTE,  "Protocol" )
	SER_VALUE( m_HighProto,		tid_BYTE,  "HighProtocol" )
	SER_VALUE( m_EType,			tid_WORD,  "EType" )
	SER_VALUE( m_IPVersion,		tid_BYTE,  "IPVersion" )
	SER_VALUE(m_ObjectID,		tid_QWORD, "ObjectID" )
    SER_VALUE( m_IPSource,		cIP::eIID, "IPSource")
    SER_VALUE( m_IPTarget,		cIP::eIID, "IPTarget")
IMPLEMENT_SERIALIZABLE_END()


////////////////////////////////////////////////////////////////////////////////
//	struct cSnifferFrameSummary - формат, посылаемый для помещения в ReportDB
//								  tSnifferFrameReport	- это событие отчета
//								  cSnifferData m_Data - это данные привязанные к событию по m_ObjectID;
//////////////////////////////////////////////////////////////////////////////

struct cSnifferFrameSummary : public cSnifferFrameReport
{
	cSnifferFrameSummary() : 
		cSnifferFrameReport()
	{}

	DECLARE_IID( cSnifferFrameSummary, cSnifferFrameReport, PID_NETDETAILS, snfr_FrameSummary );

	cSnifferData m_Data; // sniffer frame details
};
//-----------------------------------------------------------------------
IMPLEMENT_SERIALIZABLE_BEGIN( cSnifferFrameSummary, 0 )
	SER_BASE( cSnifferFrameReport,  0 )
	SER_VALUE( m_Data,	tid_BYTE, 0 )
IMPLEMENT_SERIALIZABLE_END()

//////////////////////////////////////////////////////
//	struct cSnifferFrameDetailsItem - формат показа деталей 1-го репорта
//////////////////////////////////////////////////////

struct FmtItem
{
	tDWORD		m_id;
	cStringObj	m_value;
	cStringObj	m_value2;
	cStringObj	m_value3;
	tDWORD		m_offData;
	tDWORD		m_lenData;

	FmtItem() : m_id(0), m_value(""), m_value2(""), m_value3(""), m_offData(0), m_lenData(0) {}
	void clear() { m_id = 0; m_value = ""; m_value2 = ""; m_value3 = ""; m_offData = 0; m_lenData = 0; }
};

//////////////////////////////////////////////////////
//	struct cSnifferFrameDetailsItem - формат показа деталей 1-го репорта
//////////////////////////////////////////////////////

struct cSnifferFrameDetailsItem: public cSerializable
{
	cSnifferFrameDetailsItem() : cSerializable(), m_vChild() {}

	DECLARE_IID(cSnifferFrameDetailsItem, cSerializable, PID_NETDETAILS, snfr_FrameDetails);

	FmtItem								m_fmt_item;
	cVector<cSnifferFrameDetailsItem>	m_vChild;	//	child groups

	void clearAll()
	{
		m_fmt_item.clear();

		for(size_t i = 0; i < m_vChild.size(); i++)		{
			m_vChild[i].clearAll();
		}
		
		m_vChild.clear();
		
	}
};
//-----------------------------------------------------------------------
IMPLEMENT_SERIALIZABLE_BEGIN(cSnifferFrameDetailsItem, 0)
	SER_VALUE(m_fmt_item.m_id,		tid_DWORD,		"id")
	SER_VALUE(m_fmt_item.m_value,	tid_STRING_OBJ, "str1")
	SER_VALUE(m_fmt_item.m_value2,	tid_STRING_OBJ, "str2")
	SER_VALUE(m_fmt_item.m_value3,	tid_STRING_OBJ, "str3")
	SER_VECTOR(m_vChild, cSnifferFrameDetailsItem::eIID, "vChild")
IMPLEMENT_SERIALIZABLE_END()





#endif // _sniffer_task_structs_
