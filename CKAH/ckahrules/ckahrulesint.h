#ifndef __CKAHRULESINT_H__
#define __CKAHRULESINT_H__

#include <CKAH/ckahrules.h>
#include <stuff/cpointer.h>
#include "../ckahum/PoliStorage.h"
#include <CKAH/ckahiptoa.h>
#include <malloc.h>

#define INDENT nIndent, nIndent, _T(" ")

inline PoliType& operator<<(PoliType& that, CKAHUM::IP& ipaddr)
{
    return that << SIMPLE_DATA_SER(ipaddr);
}

// _CEnvelope делает CEnvelope полиморфным (необходимо для RTTI)
template<class T> class _CEnvelope : public CEnvelope<T>
{
public:
    _CEnvelope (T* object) : CEnvelope<T> (object) {}
	virtual ~_CEnvelope () {}
};

struct CBaseMarshal
{
	CBaseMarshal (ULONG nVersion = 1) :
		m_Version (nVersion), m_VersionCur (nVersion)
	{
	}

	virtual ~CBaseMarshal ()
	{
	}

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
        m_Version = m_VersionCur;
		return pt << m_Version;
	}

	ULONG m_Version;
    const ULONG m_VersionCur;
};

IMPLEMENT_MARSHAL (CBaseMarshal)

struct CRulePortBase
{
	virtual ~CRulePortBase ()
	{
	}
	virtual CKAHFW::RulePortType GetType () const = 0;
	virtual PoliType &MarshalImpl (PoliType &pt) = 0;
	virtual void Dump (const CProgramExecutionLog &log, int nIndent = 0) const = 0;
};

IMPLEMENT_MARSHAL (CRulePortBase)

struct CRuleSinglePort : CRulePortBase, CBaseMarshal
{
	CRuleSinglePort () :
		m_Port (0)
	{
	}

	virtual ~CRuleSinglePort ()
	{
	}

	virtual CKAHFW::RulePortType GetType () const
	{
		return CKAHFW::rptSingle;
	}

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		return CBaseMarshal::MarshalImpl (pt) << m_Port;
	}

	USHORT m_Port;

	virtual void Dump (const CProgramExecutionLog &log, int nIndent = 0) const
	{
		log.WriteFormat (_T("%*.*ssingle port: %d"), PEL_INFO, INDENT, m_Port);
	}

};

struct CRulePortRange : CRulePortBase, CBaseMarshal
{
public:
	CRulePortRange () :
		m_LoPort (0),
		m_HiPort (0)
	{
	}

	virtual ~CRulePortRange ()
	{
	}
	
	virtual CKAHFW::RulePortType GetType () const
	{
		return CKAHFW::rptRange;
	}

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		return CBaseMarshal::MarshalImpl (pt) << m_LoPort << m_HiPort;
	}

	USHORT m_LoPort;
	USHORT m_HiPort;

	virtual void Dump (const CProgramExecutionLog &log, int nIndent = 0) const
	{
		log.WriteFormat (_T("%*.*sport range: %d - %d"), PEL_INFO, INDENT, m_LoPort, m_HiPort);
	}

};

typedef _CEnvelope<CRulePortBase> RulePortItem;

//////////////////////////////////////////////////////////////////////////

typedef std::list<RulePortItem> RulePortList;

struct CRulePorts : CBaseMarshal
{
	virtual ~CRulePorts ()
	{
	}

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		CBaseMarshal::MarshalImpl (pt);

		if (pt.is_read_from_pol_ ())
		{
			m_Ports.erase (m_Ports.begin(), m_Ports.end());
			long container_size = 0;
			pt << container_size;

			for (int i = 0; i < container_size; ++i)
			{
				long type = 0;
				pt << type;	
				
				switch (type)
				{
				case CKAHFW::rptSingle:
					{
						RulePortItem item (new CRuleSinglePort);
						pt << *item;
						m_Ports.insert (m_Ports.end (), item);
					}					
					break;
				case CKAHFW::rptRange:
					{
						RulePortItem item (new CRulePortRange);
						pt << *item;
						m_Ports.insert (m_Ports.end (), item);
					}					
					break;
				}	
			}
		}
		else
		{
			long container_size = m_Ports.size ();
			pt << container_size;

			RulePortList::iterator end = m_Ports.end  ();
			for (RulePortList::iterator i = m_Ports.begin  (); i != end; ++i)
			{
				long type = (*i)->GetType ();
				pt << type << **i;
			}
		}

		return pt;
	}

	RulePortList m_Ports;

	void Dump (const CProgramExecutionLog &log, int nIndent = 0) const
	{
		nIndent += 2;
		if (m_Ports.size () > 0)
		{
			RulePortList::const_iterator end = m_Ports.end  ();
			for (RulePortList::const_iterator i = m_Ports.begin  (); i != end; ++i)
				(*i)->Dump (log, nIndent);
		}
		else
			log.WriteFormat (_T("%*.*s<empty>"), PEL_INFO, INDENT);
	}

};

typedef _CEnvelope<CRulePorts> RulePortsItem;

//////////////////////////////////////////////////////////////////////////

struct CRuleAddressBase
{
	virtual ~CRuleAddressBase ()
	{
	}
	
	virtual CKAHFW::RuleAddressType GetType () const = 0;

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		return pt << m_Name;
	}

	virtual void Dump (const CProgramExecutionLog &log, int nIndent = 0) const = 0;

	std::wstring m_Name;
};

IMPLEMENT_MARSHAL (CRuleAddressBase)

struct CRuleAddressRange : CRuleAddressBase, CBaseMarshal
{
	CRuleAddressRange () :
		m_LoAddress (0),
		m_HiAddress (0)
	{
	}

	virtual ~CRuleAddressRange ()
	{
	}

	virtual CKAHFW::RuleAddressType GetType () const
	{
		return CKAHFW::ratRange;
	}

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		CBaseMarshal::MarshalImpl (pt);
		return CRuleAddressBase::MarshalImpl (pt) << m_LoAddress << m_HiAddress;
	}

	CKAHUM::IP m_LoAddress;
	CKAHUM::IP m_HiAddress;

	virtual void Dump (const CProgramExecutionLog &log, int nIndent = 0) const
	{
        log.WriteFormat (_T("%*.*saddress range: %s - %s"), PEL_INFO, INDENT, CKAHUM_IPTOA(m_LoAddress), CKAHUM_IPTOA(m_HiAddress));
	}

};

struct CRuleHostAddress : CRuleAddressBase, CBaseMarshal
{
	virtual ~CRuleHostAddress ()
	{
	}

	virtual CKAHFW::RuleAddressType GetType () const
	{
		return CKAHFW::ratHost;
	}

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		CBaseMarshal::MarshalImpl (pt);
		return CRuleAddressBase::MarshalImpl (pt) << m_HostName << m_IPs;
	}

	std::wstring m_HostName;
	std::vector<CKAHUM::IP> m_IPs;

	virtual void Dump (const CProgramExecutionLog &log, int nIndent = 0) const
	{
		LPTSTR szIp = _T("<empty>");
		
		if (!m_IPs.empty ())
		{
			const unsigned one_ip_size = 2 /* 0x */ + 40 /* 32 hex digits */ + 1 /* space */;
			unsigned size = one_ip_size * m_IPs.size () * sizeof (TCHAR);
			szIp = (LPTSTR )_alloca (size);
			memset (szIp, 0, size);
			for (std::vector<ULONG>::size_type i = 0; i < m_IPs.size (); ++i)
            {
                char ipstr[42];
			    sprintf (ipstr, _T("%s "), CKAHUM_IPTOA(m_IPs[i]));
                strcat(szIp, ipstr);
            }
		}

		log.WriteFormat (_T("%*.*shost address: %ls (%s)"), PEL_INFO, INDENT, m_HostName.c_str (), szIp);
	}

};

struct CRuleAddressSubnet : CRuleAddressBase, CBaseMarshal
{
	CRuleAddressSubnet () :
		m_Address (0),
		m_Mask (0)
	{
	}

	virtual ~CRuleAddressSubnet ()
	{
	}

	virtual CKAHFW::RuleAddressType GetType () const
	{
		return CKAHFW::ratSubnet;
	}

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		CBaseMarshal::MarshalImpl (pt);
		return CRuleAddressBase::MarshalImpl (pt) << m_Address << m_Mask;
	}

	CKAHUM::IP m_Address;
	CKAHUM::IP m_Mask;

	virtual void Dump (const CProgramExecutionLog &log, int nIndent = 0) const
	{
        log.WriteFormat (_T("%*.*ssubnet: %s/%s"), PEL_INFO, INDENT, CKAHUM::IPToA(m_Address), CKAHUM::IPMaskToA(m_Mask));
	}
};

typedef _CEnvelope<CRuleAddressBase> RuleAddressItem;

//////////////////////////////////////////////////////////////////////////

typedef std::list<RuleAddressItem> RuleAddressList;

struct CRuleAddresses : CBaseMarshal
{
	virtual ~CRuleAddresses ()
	{
	}

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		CBaseMarshal::MarshalImpl (pt);

		if (pt.is_read_from_pol_ ())
		{
			m_Addresses.erase (m_Addresses.begin(), m_Addresses.end());
			long container_size = 0;
			pt << container_size;

			for (int i = 0; i < container_size; ++i)
			{
				long type = 0;
				pt << type;	
				
				switch (type)
				{
				case CKAHFW::ratRange:
					{
						RuleAddressItem item (new CRuleAddressRange);
						pt << *item;
						m_Addresses.insert (m_Addresses.end (), item);
					}					
					break;
				case CKAHFW::ratHost:
					{
						RuleAddressItem item (new CRuleHostAddress);
						pt << *item;
						m_Addresses.insert (m_Addresses.end (), item);
					}					
					break;
				case CKAHFW::ratSubnet:
					{
						RuleAddressItem item (new CRuleAddressSubnet);
						pt << *item;
						m_Addresses.insert (m_Addresses.end (), item);
					}					
					break;
				}	
			}
		}
		else
		{
			long container_size = m_Addresses.size ();
			pt << container_size;

			RuleAddressList::iterator end = m_Addresses.end  ();
			for (RuleAddressList::iterator i = m_Addresses.begin  (); i != end; ++i)
			{
				long type = (*i)->GetType ();
				pt << type << **i;
			}
		}

		return pt;
	}

	RuleAddressList m_Addresses;

	void Dump (const CProgramExecutionLog &log, int nIndent = 0) const
	{
		nIndent += 2;
		if (m_Addresses.size () > 0)
		{
			RuleAddressList::const_iterator end = m_Addresses.end  ();
			for (RuleAddressList::const_iterator i = m_Addresses.begin  (); i != end; ++i)
				(*i)->Dump (log, nIndent);
		}
		else
			log.WriteFormat (_T("%*.*s<empty>"), PEL_INFO, INDENT);
	}
};

typedef _CEnvelope<CRuleAddresses> RuleAddressesItem;

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_MARSHAL_ENUM (CKAHFW::RuleTimeElementType)

struct CRuleTimeElement : CBaseMarshal
{
	CRuleTimeElement () :
        m_Type(CKAHFW::rtetSecond),
        m_LoValue(0),
        m_HiValue(0)
	{
	}

	virtual ~CRuleTimeElement ()
	{
	}

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		CBaseMarshal::MarshalImpl (pt);
		return pt << m_Type << m_LoValue << m_HiValue;
	}

    CKAHFW::RuleTimeElementType m_Type;
    ULONG m_LoValue;
    ULONG m_HiValue;

	void Dump (const CProgramExecutionLog &log, int nIndent = 0) const
	{
		TCHAR *szType = "undefined";

        switch(m_Type)
        {
        case CKAHFW::rtetSecond:     szType = _T("seconds");     break;
        case CKAHFW::rtetDaySecond:  szType = _T("day seconds"); break;
        case CKAHFW::rtetMinute:     szType = _T("minutes");     break;
        case CKAHFW::rtetHour:       szType = _T("hours");       break;
        case CKAHFW::rtetDay:        szType = _T("days");        break;
        case CKAHFW::rtetWeekDay:    szType = _T("weekdays");    break;
        case CKAHFW::rtetMonth:      szType = _T("months");      break;
        case CKAHFW::rtetYear:       szType = _T("years");       break;
        }

		log.WriteFormat (_T("%*.*s%s range: %d - %d"), PEL_INFO, INDENT, szType, m_LoValue, m_HiValue);
	}
};

typedef _CEnvelope<CRuleTimeElement> RuleTimeElementItem;

//////////////////////////////////////////////////////////////////////////

typedef std::list<RuleTimeElementItem> RuleTimeElementList;

struct CRuleTime : CBaseMarshal
{
	virtual ~CRuleTime ()
	{
	}

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		CBaseMarshal::MarshalImpl (pt);

		if (pt.is_read_from_pol_ ())
		{
			m_TimeElements.erase (m_TimeElements.begin(), m_TimeElements.end());
			long container_size = 0;
			pt << container_size;

			for (int i = 0; i < container_size; ++i)
			{
				RuleTimeElementItem item (new CRuleTimeElement);
				pt << *item;
				m_TimeElements.insert (m_TimeElements.end (), item);
			}
		}
		else
		{
			long container_size = m_TimeElements.size ();
			pt << container_size;

			RuleTimeElementList::iterator end = m_TimeElements.end  ();
			for (RuleTimeElementList::iterator i = m_TimeElements.begin  (); i != end; ++i)
				pt << **i;
		}

		return pt;
	}

	RuleTimeElementList m_TimeElements;

	void Dump (const CProgramExecutionLog &log, int nIndent = 0) const
	{
		log.WriteFormat (_T("%*.*stime:"), PEL_INFO, INDENT);
		nIndent += 2;
		if (m_TimeElements.size () > 0)
		{
			RuleTimeElementList::const_iterator end = m_TimeElements.end  ();
			for (RuleTimeElementList::const_iterator i = m_TimeElements.begin  (); i != end; ++i)
				(*i)->Dump (log, nIndent);
		}
		else
			log.WriteFormat (_T("%*.*s<empty>"), PEL_INFO, INDENT);
	}
};

typedef _CEnvelope<CRuleTime> RuleTimeItem;

//////////////////////////////////////////////////////////////////////////

typedef std::list<RuleTimeItem> RuleTimeList;

struct CRuleTimes : CBaseMarshal
{
	virtual ~CRuleTimes ()
	{
	}

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		CBaseMarshal::MarshalImpl (pt);

		if (pt.is_read_from_pol_ ())
		{
			m_Times.erase (m_Times.begin(), m_Times.end());
			long container_size = 0;
			pt << container_size;

			for (int i = 0; i < container_size; ++i)
			{
				RuleTimeItem item (new CRuleTime);
				pt << *item;
				m_Times.insert (m_Times.end (), item);
			}
		}
		else
		{
			long container_size = m_Times.size ();
			pt << container_size;

			RuleTimeList::iterator end = m_Times.end  ();
			for (RuleTimeList::iterator i = m_Times.begin  (); i != end; ++i)
				pt << **i;
		}

		return pt;
	}

	RuleTimeList m_Times;

	void Dump (const CProgramExecutionLog &log, int nIndent = 0) const
	{
		log.WriteFormat (_T("%*.*stimes:"), PEL_INFO, INDENT);
		nIndent += 2;
		if (m_Times.size () > 0)
		{
			RuleTimeList::const_iterator end = m_Times.end  ();
			for (RuleTimeList::const_iterator i = m_Times.begin  (); i != end; ++i)
				(*i)->Dump (log, nIndent);
		}
		else
			log.WriteFormat (_T("%*.*s<empty>"), PEL_INFO, INDENT);
	}
};

typedef _CEnvelope<CRuleTimes> RuleTimesItem;

//////////////////////////////////////////////////////////////////////////
//
//	Packet rules
//
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_MARSHAL_ENUM (CKAHFW::PacketDirection)
IMPLEMENT_MARSHAL_ENUM (CKAHFW::StreamDirection)

struct CPacketRule : CBaseMarshal
{
	CPacketRule () :
		CBaseMarshal (4),
		m_bIsEnabled (true),
		m_bIsBlocking (true),
		m_PacketDirection (CKAHFW::pdOutgoing),
        m_StreamDirection (CKAHFW::sdBoth),
		m_Proto (CKAHFW::PROTO_ALL),
		m_RemoteAddresses (new CRuleAddresses),
		m_LocalAddresses (new CRuleAddresses),
		m_TCPUDPRemotePorts (new CRulePorts),
		m_TCPUDPLocalPorts (new CRulePorts),
		m_ICMPType (0xff),
		m_ICMPCode (0xff),
        m_Times (new CRuleTimes),
		m_bNotify (false),
		m_bLog (false)
	{
	}

	virtual ~CPacketRule ()
	{
	}

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		CBaseMarshal::MarshalImpl (pt);

		if (pt.is_read_from_pol_ ())
		{
			switch (m_Version)
			{
			case 1:
				return pt << m_Name << m_bIsEnabled << m_bIsBlocking << m_PacketDirection << m_Proto <<
						*m_RemoteAddresses << *m_LocalAddresses << *m_TCPUDPRemotePorts << *m_TCPUDPLocalPorts <<
						m_ICMPType << m_ICMPCode << m_bNotify << m_UserData;

            case 2:
		        return pt << m_Name << m_bIsEnabled << m_bIsBlocking << m_PacketDirection << m_Proto <<
				        *m_RemoteAddresses << *m_LocalAddresses << *m_TCPUDPRemotePorts << *m_TCPUDPLocalPorts <<
				        m_ICMPType << m_ICMPCode << m_bNotify << m_bLog << m_UserData;

            case 3:
		        return pt << m_Name << m_bIsEnabled << m_bIsBlocking << m_PacketDirection << m_StreamDirection << m_Proto <<
				        *m_RemoteAddresses << *m_LocalAddresses << *m_TCPUDPRemotePorts << *m_TCPUDPLocalPorts << 
				        m_ICMPType << m_ICMPCode << m_bNotify << m_bLog << m_UserData;
			}

		}

		return pt << m_Name << m_bIsEnabled << m_bIsBlocking << m_PacketDirection << m_StreamDirection << m_Proto <<
				*m_RemoteAddresses << *m_LocalAddresses << *m_TCPUDPRemotePorts << *m_TCPUDPLocalPorts << 
				m_ICMPType << m_ICMPCode << *m_Times << m_bNotify << m_bLog << m_UserData;
	}

	std::wstring m_Name;						            // название правила
	bool m_bIsEnabled;							            // разрешено ли правило
	bool m_bIsBlocking;							            // блокирует / пропускает
	CKAHFW::PacketDirection m_PacketDirection;	            // направление
    CKAHFW::StreamDirection m_StreamDirection;              // направление потока 
	UCHAR m_Proto;								            // протокол
	RuleAddressesItem m_RemoteAddresses;		            // список удаленных адресов
	RuleAddressesItem m_LocalAddresses;			            // список локальных адресов
	RulePortsItem m_TCPUDPRemotePorts;			            // список удаленных портов (только для протоколов TCP и UDP)
	RulePortsItem m_TCPUDPLocalPorts;			            // список локальных портов (только для протоколов TCP и UDP)
	UCHAR m_ICMPType;							            // ICMP тип (только для протокола ICMP)
	UCHAR m_ICMPCode;							            // ICMP код (только для протокола ICMP)
    RuleTimesItem m_Times;                                  // список времен срабатывания правила

	bool m_bNotify;								            // признак уведомления
	bool m_bLog;								            // признак записи в журнал
	std::vector<UCHAR> m_UserData;				            // пользовательские данные

	void Dump (const CProgramExecutionLog &log, int nIndent = 0) const
	{
		TCHAR szI[8];

		log.WriteFormat (_T("%*.*spacket rule:"), PEL_INFO, INDENT);
		nIndent += 2;
		log.WriteFormat (_T("%*.*sname: %ls"), PEL_INFO, INDENT, m_Name.empty () ? L"<no name>" : m_Name.c_str ());
		log.WriteFormat (_T("%*.*senabled: %s"), PEL_INFO, INDENT, m_bIsEnabled ? _T("true") : _T("false"));
		log.WriteFormat (_T("%*.*sblocking: %s"), PEL_INFO, INDENT, m_bIsBlocking ? _T("true") : _T("false"));
		log.WriteFormat (_T("%*.*spacketdir: %s"), PEL_INFO, INDENT, m_PacketDirection == CKAHFW::pdIncoming ? _T("incoming") : (m_PacketDirection == CKAHFW::pdOutgoing ? _T("outgoing") : (m_PacketDirection == CKAHFW::pdBoth ? _T("both") : _T("unknown"))));
		log.WriteFormat (_T("%*.*sstreamdir: %s"), PEL_INFO, INDENT, m_StreamDirection == CKAHFW::sdIncoming ? _T("incoming") : (m_StreamDirection == CKAHFW::sdOutgoing ? _T("outgoing") : (m_StreamDirection == CKAHFW::sdBoth ? _T("both") : _T("unknown"))));
		log.WriteFormat (_T("%*.*sprotocol: %s"), PEL_INFO, INDENT, m_Proto == CKAHFW::PROTO_TCP ? _T("TCP") : (m_Proto == CKAHFW::PROTO_UDP ? _T("UDP") : (m_Proto == CKAHFW::PROTO_ICMP ? _T("ICMP") : (m_Proto == CKAHFW::PROTO_ALL ? _T("all") : _itot (m_Proto, szI, 10)))));
		log.WriteFormat (_T("%*.*sremote addresses:"), PEL_INFO, INDENT);
		m_RemoteAddresses->Dump (log, nIndent + 2);
		log.WriteFormat (_T("%*.*slocal addresses:"), PEL_INFO, INDENT);
		m_LocalAddresses->Dump (log, nIndent + 2);
		log.WriteFormat (_T("%*.*sremote ports:"), PEL_INFO, INDENT);
		m_TCPUDPRemotePorts->Dump (log, nIndent + 2);
		log.WriteFormat (_T("%*.*slocal ports:"), PEL_INFO, INDENT);
		m_TCPUDPLocalPorts->Dump (log, nIndent + 2);
		log.WriteFormat (_T("%*.*sICMP type, code: %d, %d"), PEL_INFO, INDENT, m_ICMPType, m_ICMPCode);
        m_Times->Dump(log, nIndent + 2);
		log.WriteFormat (_T("%*.*snotify: %s"), PEL_INFO, INDENT, m_bNotify ? _T("true") : _T("false"));
		log.WriteFormat (_T("%*.*slog: %s"), PEL_INFO, INDENT, m_bLog ? _T("true") : _T("false"));
		log.WriteFormat (_T("%*.*suser data length: %d"), PEL_INFO, INDENT, m_UserData.size ());
	}
};

typedef _CEnvelope<CPacketRule> PacketRuleItem;

//////////////////////////////////////////////////////////////////////////

typedef std::list<PacketRuleItem> PacketRuleList;

struct CPacketRules : CBaseMarshal
{
	virtual ~CPacketRules ()
	{
	}

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		CBaseMarshal::MarshalImpl (pt);

		if (pt.is_read_from_pol_ ())
		{
			m_Rules.erase (m_Rules.begin(), m_Rules.end());
			long container_size = 0;
			pt << container_size;

			for (int i = 0; i < container_size; ++i)
			{
				PacketRuleItem item (new CPacketRule);
				pt << *item;
				m_Rules.insert (m_Rules.end (), item);
			}
		}
		else
		{
			long container_size = m_Rules.size ();
			pt << container_size;

			PacketRuleList::iterator end = m_Rules.end  ();
			for (PacketRuleList::iterator i = m_Rules.begin  (); i != end; ++i)
				pt << **i;
		}

		return pt;
	}

	PacketRuleList m_Rules;

	void Dump (const CProgramExecutionLog &log, int nIndent = 0) const
	{
		log.WriteFormat (_T("%*.*spacket rules:"), PEL_INFO, INDENT);
		nIndent += 2;
		if (m_Rules.size () > 0)
		{
			PacketRuleList::const_iterator end = m_Rules.end  ();
			for (PacketRuleList::const_iterator i = m_Rules.begin  (); i != end; ++i)
				(*i)->Dump (log, nIndent);
		}
		else
			log.WriteFormat (_T("%*.*s<empty>"), PEL_INFO, INDENT);
	}
};

typedef _CEnvelope<CPacketRules> PacketRulesItem;

//////////////////////////////////////////////////////////////////////////
//
//	Application rules
//
//////////////////////////////////////////////////////////////////////////

struct CApplicationRuleElement : CBaseMarshal
{
	CApplicationRuleElement () :
        CBaseMarshal (2),
        m_PacketDirection (CKAHFW::pdBoth),
        m_StreamDirection (CKAHFW::sdBoth),
        m_Proto (CKAHFW::PROTO_ALL),
		m_RemoteAddresses (new CRuleAddresses),
		m_RemotePorts (new CRulePorts),
		m_LocalPorts (new CRulePorts),
		m_bIsActive (false)
	{
	}

	virtual ~CApplicationRuleElement ()
	{
	}

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		CBaseMarshal::MarshalImpl (pt);

		if (pt.is_read_from_pol_ ())
		{
			switch (m_Version)
			{
			case 1:
		        return pt << *m_RemoteAddresses << *m_RemotePorts << *m_LocalPorts << m_bIsActive;
            }
        }

		return pt << m_PacketDirection << m_StreamDirection << m_Proto << 
            *m_RemoteAddresses << *m_RemotePorts << *m_LocalPorts << m_bIsActive;
	}

	CKAHFW::PacketDirection m_PacketDirection;	            // направление
    CKAHFW::StreamDirection m_StreamDirection;              // направление потока 
	UCHAR m_Proto;								            // протокол
	RuleAddressesItem m_RemoteAddresses;	                // список удаленных адресов
	RulePortsItem m_RemotePorts;			                // список удаленных портов
	RulePortsItem m_LocalPorts;				                // список локальных портов
	
	bool m_bIsActive;

	void Dump (const CProgramExecutionLog &log, int nIndent = 0) const
	{
		TCHAR szI[8];

		log.WriteFormat (_T("%*.*sapplication rule element:"), PEL_INFO, INDENT);
		nIndent += 2;
		log.WriteFormat (_T("%*.*sactive: %s"), PEL_INFO, INDENT, m_bIsActive ? _T("true") : _T("false"));
		log.WriteFormat (_T("%*.*spacketdir: %s"), PEL_INFO, INDENT, m_PacketDirection == CKAHFW::pdIncoming ? _T("incoming") : (m_PacketDirection == CKAHFW::pdOutgoing ? _T("outgoing") : (m_PacketDirection == CKAHFW::pdBoth ? _T("both") : _T("unknown"))));
		log.WriteFormat (_T("%*.*sstreamdir: %s"), PEL_INFO, INDENT, m_StreamDirection == CKAHFW::sdIncoming ? _T("incoming") : (m_StreamDirection == CKAHFW::sdOutgoing ? _T("outgoing") : (m_StreamDirection == CKAHFW::sdBoth ? _T("both") : _T("unknown"))));
		log.WriteFormat (_T("%*.*sprotocol: %s"), PEL_INFO, INDENT, m_Proto == CKAHFW::PROTO_TCP ? _T("TCP") : (m_Proto == CKAHFW::PROTO_UDP ? _T("UDP") : (m_Proto == CKAHFW::PROTO_ICMP ? _T("ICMP") : (m_Proto == CKAHFW::PROTO_ALL ? _T("all") : _itot (m_Proto, szI, 10)))));
		log.WriteFormat (_T("%*.*saddresses:"), PEL_INFO, INDENT);
		m_RemoteAddresses->Dump (log, nIndent + 2);
		log.WriteFormat (_T("%*.*sremote ports:"), PEL_INFO, INDENT);
		m_RemotePorts->Dump (log, nIndent + 2);
		log.WriteFormat (_T("%*.*slocal ports:"), PEL_INFO, INDENT);
		m_LocalPorts->Dump (log, nIndent + 2);
	}
};

typedef _CEnvelope<CApplicationRuleElement> CApplicationRuleElementItem;

//////////////////////////////////////////////////////////////////////////

typedef std::list<CApplicationRuleElementItem> CApplicationRuleElementList;

struct CApplicationRule : CBaseMarshal
{
	CApplicationRule () :
		CBaseMarshal (7),
        m_Times (new CRuleTimes),
        m_bIsCommandLine (false),
		m_bIsEnabled (true),
		m_bIsBlocking (true),
		m_bNotify (false),
		m_bLog (false),
        m_bCheckHash (true)
	{
	}

	virtual ~CApplicationRule ()
	{
	}

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		CBaseMarshal::MarshalImpl (pt);

		if (pt.is_read_from_pol_ ())
		{
            if (m_Version < 3)
            {
			    m_Elements.erase (m_Elements.begin(), m_Elements.end());

                CApplicationRuleElementItem
		            TCPIncoming (new CApplicationRuleElement),
		            TCPOutgoing (new CApplicationRuleElement),
		            UDPIncoming (new CApplicationRuleElement),
		            UDPOutgoing (new CApplicationRuleElement);

                TCPIncoming->m_PacketDirection = CKAHFW::pdBoth;
                TCPIncoming->m_StreamDirection = CKAHFW::sdIncoming;
                TCPIncoming->m_Proto = CKAHFW::PROTO_TCP;

                TCPOutgoing->m_PacketDirection = CKAHFW::pdBoth;
                TCPOutgoing->m_StreamDirection = CKAHFW::sdOutgoing;
                TCPOutgoing->m_Proto = CKAHFW::PROTO_TCP;

                UDPIncoming->m_PacketDirection = CKAHFW::pdIncoming;
                UDPIncoming->m_StreamDirection = CKAHFW::sdBoth;
                UDPIncoming->m_Proto = CKAHFW::PROTO_UDP;

                UDPOutgoing->m_PacketDirection = CKAHFW::pdOutgoing;
                UDPOutgoing->m_StreamDirection = CKAHFW::sdBoth;
                UDPOutgoing->m_Proto = CKAHFW::PROTO_UDP;

			    switch (m_Version)
			    {
			    case 1:

				    pt << m_Name << m_ApplicationName << m_bIsEnabled << m_bIsBlocking << 
					    *TCPIncoming << *TCPOutgoing << *UDPIncoming << *UDPOutgoing <<
					    m_bNotify << m_UserData;

                case 2:

                    pt << m_Name << m_ApplicationName << m_bIsEnabled << m_bIsBlocking <<
					    *TCPIncoming << *TCPOutgoing << *UDPIncoming << *UDPOutgoing <<
					    m_bNotify << m_bLog << m_UserData;
                }


                m_Elements.erase (m_Elements.begin(), m_Elements.end());

			    m_Elements.insert (m_Elements.end (), TCPIncoming);
			    m_Elements.insert (m_Elements.end (), TCPOutgoing);
			    m_Elements.insert (m_Elements.end (), UDPIncoming);
			    m_Elements.insert (m_Elements.end (), UDPOutgoing);

                return pt;
            }
            else if (m_Version < 6)
            {
                m_Elements.erase (m_Elements.begin(), m_Elements.end());

			    long container_size = 0;
			    pt << container_size;

			    for (int i = 0; i < container_size; ++i)
			    {
				    CApplicationRuleElementItem item (new CApplicationRuleElement);
				    pt << *item;
				    m_Elements.insert (m_Elements.end (), item);
			    }

			    switch (m_Version)
			    {
                case 3:
		            return pt << m_Name << m_ApplicationName << m_bIsEnabled << m_bIsBlocking << 
					             m_bNotify << m_bLog << m_UserData;

                case 4:
		            return pt << m_Name << m_ApplicationName << m_bIsEnabled << m_bIsBlocking << 
					             *m_Times << m_bNotify << m_bLog << m_UserData;

				case 5:
					return pt << m_Name << m_ApplicationName << m_bIsCommandLine << m_CommandLine <<
								 m_bIsEnabled << m_bIsBlocking << 
								 *m_Times << m_bNotify << m_bLog << m_UserData;

                case 6:
		            return pt << m_Name << m_ApplicationName << m_bIsCommandLine << m_CommandLine <<
                                 m_bIsEnabled << m_bIsBlocking << 
					             *m_Times << m_bNotify << m_bLog << m_UserData << m_AppHash;
                }
            }
        }
            
		if (pt.is_read_from_pol_ ())
		{
            m_Elements.erase (m_Elements.begin(), m_Elements.end());

			long container_size = 0;
			pt << container_size;

			for (int i = 0; i < container_size; ++i)
			{
				CApplicationRuleElementItem item (new CApplicationRuleElement);
				pt << *item;
				m_Elements.insert (m_Elements.end (), item);
			}
		}
		else
		{
			long container_size = m_Elements.size ();
			pt << container_size;

			CApplicationRuleElementList::iterator end = m_Elements.end  ();
			for (CApplicationRuleElementList::iterator i = m_Elements.begin  (); i != end; ++i)
				pt << **i;
        }

		return pt << m_Name << m_ApplicationName << m_bIsCommandLine << m_CommandLine <<
                     m_bIsEnabled << m_bIsBlocking << 
					 *m_Times << m_bNotify << m_bLog << m_UserData << m_bCheckHash << m_AppHash;
	}

	std::wstring m_Name;						// название правила
	std::wstring m_ApplicationName;				// приложение
    bool m_bIsCommandLine;                      // используется ли командная строка
    std::wstring m_CommandLine;                 // командная строка
	bool m_bIsEnabled;							// разрешено ли правило
	bool m_bIsBlocking;							// разрешает / блокирует
    CApplicationRuleElementList m_Elements;
    RuleTimesItem m_Times;                      // список времен срабатывания правила
	bool m_bNotify;								// признак уведомления
	bool m_bLog;								// признак записи в журнал
	std::vector<UCHAR> m_UserData;				// пользовательские данные
	std::vector<UCHAR> m_AppHash;				// md5 hash приложения
    bool m_bCheckHash;                          // признак проверки hash приложения

	void Dump (const CProgramExecutionLog &log, int nIndent = 0) const
	{
		log.WriteFormat (_T("%*.*sapplication rule:"), PEL_INFO, INDENT);
		nIndent += 2;
		log.WriteFormat (_T("%*.*sname: %ls"), PEL_INFO, INDENT, m_Name.empty () ? L"<no name>" : m_Name.c_str ());
		log.WriteFormat (_T("%*.*sapplication: %ls"), PEL_INFO, INDENT, m_ApplicationName.empty () ? L"<no name>" : m_ApplicationName.c_str ());
		log.WriteFormat (_T("%*.*siscmdline: %s"), PEL_INFO, INDENT, m_bIsCommandLine ? _T("true") : _T("false"));
		log.WriteFormat (_T("%*.*scmdline: %ls"), PEL_INFO, INDENT, m_CommandLine.empty () ? L"<no name>" : m_CommandLine.c_str ());
		log.WriteFormat (_T("%*.*senabled: %s"), PEL_INFO, INDENT, m_bIsEnabled ? _T("true") : _T("false"));
		log.WriteFormat (_T("%*.*sblocking: %s"), PEL_INFO, INDENT, m_bIsBlocking ? _T("true") : _T("false"));
		log.WriteFormat (_T("%*.*selements:"), PEL_INFO, INDENT);
		if (m_Elements.size () > 0)
		{
			CApplicationRuleElementList::const_iterator end = m_Elements.end  ();
			for (CApplicationRuleElementList::const_iterator i = m_Elements.begin  (); i != end; ++i)
				(*i)->Dump (log, nIndent+2);
		}
		else
			log.WriteFormat (_T("%*.*s<empty>"), PEL_INFO, INDENT);

		log.WriteFormat (_T("%*.*snotify: %s"), PEL_INFO, INDENT, m_bNotify ? _T("true") : _T("false"));
		log.WriteFormat (_T("%*.*slog: %s"), PEL_INFO, INDENT, m_bLog ? _T("true") : _T("false"));
		log.WriteFormat (_T("%*.*suser data length: %d"), PEL_INFO, INDENT, m_UserData.size ());
		log.WriteFormat (_T("%*.*schk hash: %s"), PEL_INFO, INDENT, m_bCheckHash ? _T("true") : _T("false"));
		log.WriteFormat (_T("%*.*sapp hash: %s"), PEL_INFO, INDENT, m_AppHash.size() == FW_HASH_SIZE ? _T("present") : _T("absent"));
	}
};

typedef _CEnvelope<CApplicationRule> ApplicationRuleItem;

//////////////////////////////////////////////////////////////////////////

typedef std::list<ApplicationRuleItem> ApplicationRuleList;

struct CApplicationRules : CBaseMarshal
{
	virtual ~CApplicationRules ()
	{
	}

	virtual PoliType &MarshalImpl (PoliType &pt)
	{
		CBaseMarshal::MarshalImpl (pt);

		if (pt.is_read_from_pol_ ())
		{
			m_Rules.erase (m_Rules.begin(), m_Rules.end());
			long container_size = 0;
			pt << container_size;

			for (int i = 0; i < container_size; ++i)
			{
				ApplicationRuleItem item (new CApplicationRule);
				pt << *item;
				m_Rules.insert (m_Rules.end (), item);
			}
		}
		else
		{
			long container_size = m_Rules.size ();
			pt << container_size;

			ApplicationRuleList::iterator end = m_Rules.end  ();
			for (ApplicationRuleList::iterator i = m_Rules.begin  (); i != end; ++i)
				pt << **i;
		}

		return pt;
	}

	ApplicationRuleList m_Rules;

	void Dump (const CProgramExecutionLog &log, int nIndent = 0) const
	{
		log.WriteFormat (_T("%*.*sapplication rules:"), PEL_INFO, INDENT);
		nIndent += 2;
		if (m_Rules.size () > 0)
		{
			ApplicationRuleList::const_iterator end = m_Rules.end  ();
			for (ApplicationRuleList::const_iterator i = m_Rules.begin  (); i != end; ++i)
				(*i)->Dump (log, nIndent);
		}
		else
			log.WriteFormat (_T("%*.*s<empty>"), PEL_INFO, INDENT);
	}
};

typedef _CEnvelope<CApplicationRules> ApplicationRulesItem;

//////////////////////////////////////////////////////////////////////////

#endif