#ifndef _ahfw_task_structs_
#define _ahfw_task_structs_

#include <AHTasks/plugin/p_ahfw.h>
#include <AV/structs/s_avs.h>
#include <ProductCore/structs/s_ip.h>
#include <ProductCore/structs/s_taskmanager.h>

enum _eAHFW_IID
{
    _eiid_fw_stat = 1,
    _eiid_fw_statex,
    _eiid_connections,
    _eiid_applications,
    _eiid_baserule,
    _eiid_portrules,
    _eiid_apprules,
    _eiid_port,
    _eiid_address,
    _eiid_appruleitem,
    _eiid_ask_appevent,
    _eiid_appchanged,
    _eiid_ask_appchanged,
    _eiid_event_report_base,
    _eiid_packet_event_report,
    _eiid_app_event_report,
    _eiid_networks,
    _eiid_base_rule,
    _eiid_packet_rule,
    _eiid_app_rule,
    _eiid_app_rule_ex,
    _eiid_app_rule_preset,
    _eiid_network_changed,
    _eiid_cFwTransmitSize,
    _eiid_cFwHostStat,
    _eiid_cFwNets,
    _eiid_cFwChangeNet
};

typedef enum _eAHFWWorkingMode
{
    _fwwmBlockAll = 0,
    _fwwmRejectNotFiltered,
    _fwwmAskUser,
    _fwwmAllowNotFiltered,
    _fwwmAllowAll
};

typedef enum _eFWWellKnownProtocol
{
    _fwPROTO_ALL	= 0x00,
    _fwPROTO_ICMP	= 0x01,
    _fwPROTO_TCP	= 0x06,
    _fwPROTO_UDP	= 0x11,
    _fwPROTO_ICMPV6 = 0x3a,
};

#define FwInvalidIcmpCode 0xff

typedef enum _eFwDirection
{
    _fwInbound            = 0x01,
    _fwOutbound           = 0x02,
    _fwInboundOutbound    = 0x03,
    _fwInboundStream      = 0x04,
    _fwOutboundStream     = 0x05
};

typedef enum _eFWAddressType
{
    _fwatSymbolic = 0,
    _fwatSubnet = 1,
    _fwatSingleIp = 2,
    _fwatIpRange = 4,
};

typedef enum _eFWWellKnownAddress
{
    _fwaUnknown = 0,
    _fwaDHCP = 1,
    _fwaDNS = 2,
    _fwaWINS = 3,
};

typedef enum _eFWPortType
{
    _fwptSinglePort = 0,
    _fwptPortRange = 1,
};

typedef enum _eAHFWNetworkZone
{
    _fwnzTrusted = 0,
    _fwnzNetBIOS,
    _fwnzUntrusted,
};

enum _eAHFWNetworkType
{
    _fwntIPv4 = 0,
    _fwntIPv6GlobUni,
    _fwntIPv6LinkUni,
    _fwntIPv6IfMulti,
    _fwntIPV6LinkMulti,
};

#define FwHashSize        16


//-----------------------------------------------------------------------------
//! —татистика, предоставл€ема€ AHFirewall

// common
struct cAHFWCommonStatistics: public cTaskStatistics
{
    cAHFWCommonStatistics(): cTaskStatistics(),
        m_PacketsProceed(0),
        m_AppProceed(0),
        m_AppChanges(0)
    {}

    DECLARE_IID_STATISTICS(cAHFWCommonStatistics, cTaskStatistics, PID_AHFW);

    tDWORD			m_PacketsProceed;
    tDWORD			m_AppProceed;
    tDWORD			m_AppChanges;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cAHFWCommonStatistics, 0)
    SER_BASE(cTaskStatistics, 0)
    SER_VALUE_M(PacketsProceed,	tid_DWORD)
    SER_VALUE_M(AppProceed,	tid_DWORD)
    SER_VALUE_M(AppChanges,	tid_DWORD)
IMPLEMENT_SERIALIZABLE_END()

//+ ------------------------------------------------------------------------------------------
// special

struct cAHFWStatistics: public cTaskStatistics
{
    cAHFWStatistics(): cTaskStatistics(),
        m_nActiveNetworkApplications(0),
        m_nOpenPorts(0),
        m_nActiveNetworkConnections(0),
        m_nInBytes(0), m_nOutBytes(0),
        m_nDroppedConnections(0)
    {}

    DECLARE_IID(cAHFWStatistics, cTaskStatistics, PID_AHFW, _eiid_fw_stat);

    tUINT			m_nActiveNetworkApplications;			// количество сетевых приложений
    tUINT			m_nOpenPorts;							// количество открытых портов
    tUINT			m_nActiveNetworkConnections;			// количество сетевых подключений
    tQWORD          m_nInBytes;                             // размер пропущенного вход€щего трафика
    tQWORD          m_nOutBytes;                            // размер пропущенного исход€щего трафика
    tUINT           m_nDroppedConnections;                  // количество дропнутых соединений.
};

IMPLEMENT_SERIALIZABLE_BEGIN(cAHFWStatistics, 0)
    SER_BASE(cTaskStatistics, 0)
    SER_VALUE_M(nActiveNetworkApplications,	tid_UINT)
    SER_VALUE_M(nOpenPorts,					tid_UINT)
    SER_VALUE_M(nActiveNetworkConnections,	tid_UINT)
    SER_VALUE_M(nInBytes,	                tid_QWORD)
    SER_VALUE_M(nOutBytes,	                tid_QWORD)
    SER_VALUE_M(nDroppedConnections,	    tid_UINT)
IMPLEMENT_SERIALIZABLE_END()

//+ ------------------------------------------------------------------------------------------

#define pmc_TASKAHFW_BREAK_CONNECTION        0x1ee66f50 // is a crc32 for "pmc_TASKAHFW_BREAK_CONNECTION"
#define pmc_TASKAHFW_GET_NAGENT_SETTINGS     0x9d055a72 // is a crc32 for "pmc_TASKAHFW_GET_NAGENT_SETTINGS"
#define pmc_TASKAHFW_NAGENT_SETTINGS_CHANGED 0x2a63732e // is a crc32 for "pmc_TASKAHFW_NAGENT_SETTINGS_CHANGED"

struct cAHFWConnection : public cSerializable
{
    cAHFWConnection() : cSerializable(),
        m_Proto(0),
        m_LocalPort(0),
        m_RemotePort(0),
        m_Direction(_fwOutbound),
        m_ActiveTime(0),
        m_InBytes(0),
        m_OutBytes(0)
    {}

    DECLARE_IID(cAHFWConnection, cSerializable, PID_AHFW, _eiid_connections);

    tBYTE           m_Proto;			// протокол
    cIP             m_LocalIP;			// локальный IP-адрес
    tWORD           m_LocalPort;		// локальный порт
    cIP             m_RemoteIP;         // удаленный IP-адрес
    tWORD           m_RemotePort;       // удаленный порт
    _eFwDirection   m_Direction;        // направление
    tDWORD          m_ActiveTime;
    tQWORD          m_InBytes;
    tQWORD          m_OutBytes;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cAHFWConnection, "AHFWConnection")
    SER_VALUE_M(Proto,			tid_BYTE)
    SER_VALUE_M(LocalIP,		cIP::eIID)
    SER_VALUE_M(LocalPort,		tid_WORD)
    SER_VALUE_M(RemoteIP,		cIP::eIID)
    SER_VALUE_M(RemotePort,		tid_WORD)
    SER_VALUE_M(Direction,		tid_DWORD)
    SER_VALUE_M(ActiveTime,     tid_DWORD)
    SER_VALUE_M(InBytes,        tid_QWORD)
    SER_VALUE_M(OutBytes,       tid_QWORD)
IMPLEMENT_SERIALIZABLE_END()

//+ -------

struct cAHFWNetwork : public cSerializable
{
    cAHFWNetwork() : cSerializable(),
        m_IP_(0),
        m_Mask_(0xffffff00),
        m_eZone(_fwnzNetBIOS),
        m_bStealthed(cFALSE),
        m_nMac(0),
        m_bHidden(cFALSE),
        m_eType(_fwntIPv4)
    {}

    DECLARE_IID(cAHFWNetwork, cSerializable, PID_AHFW, _eiid_networks);

    bool              IsMacEqual(cAHFWNetwork &net)         { return m_nMac == net.m_nMac; }
    bool              IsTypeEqual(cAHFWNetwork &net)        { return m_eType == net.m_eType; }

    bool              IsIPEqual(cAHFWNetwork &net)          { return m_IP.m_Version == net.m_IP.m_Version && (m_IP & m_Mask) == (net.m_IP & net.m_Mask); }
    bool              IsIPGreaterEqual(cAHFWNetwork &net)   { return m_IP.m_Version == net.m_IP.m_Version && m_Mask.GetMaskPrefixLength() <= net.m_Mask.GetMaskPrefixLength() && (m_IP & m_Mask) == (net.m_IP & m_Mask); }
    bool              IsIPGreater(cAHFWNetwork &net)        { return IsIPGreaterEqual(net) && !IsIPEqual(net); }

    bool              IsUserDefined()                       { return !m_nMac; }
    bool              IsShown()                             { return !m_bHidden; }

    bool              IsDependent()                         { return m_eType != _fwntIPv4 && m_eType != _fwntIPv6GlobUni; }
    tINT              GetDependencyLevel()                  { return m_eType == _fwntIPv6GlobUni ? 2 : m_eType == _fwntIPv4 ? 1 : 0; }

    void pr_call finalizeStreamDeser( tERROR err, tUINT field_num, const cSerDescriptor* dsc )
    {
        if (field_num == 6 || field_num == 7) // pre-сIP version
        {
            m_IP.SetV4(m_IP_);
            m_Mask.SetV4(m_Mask_);
        }
    }

    void pr_call finalizeRegDeser( tERROR err, tUINT field_num, const cSerDescriptor* dsc )
    {
        if (field_num == 6 || field_num == 7) // pre-сIP version
        {
            m_IP.SetV4(m_IP_);
            m_Mask.SetV4(m_Mask_);
        }
    }

    tIPv4             m_IP_;                // deprecated
    tIPv4             m_Mask_;              // deprecated
    cStringObj        m_sAdapterDesc;       // ќписание адаптера
    tQWORD            m_nMac;				// ћак адрес
    _eAHFWNetworkZone m_eZone;
    tBOOL             m_bStealthed;
    tBOOL             m_bHidden;            // hidden by other networks
    cIP               m_IP;                 // IP-адрес сети
    cIP               m_Mask;               // ћаска сети
    _eAHFWNetworkType m_eType;              // тип адреса сети
};

IMPLEMENT_SERIALIZABLE_BEGIN(cAHFWNetwork, "AHFWNetwork")
    SER_VALUE( m_IP_,           tid_IPv4,        "IP")
    SER_VALUE( m_Mask_,		    tid_IPv4,        "Mask")
    SER_VALUE( m_eZone,	        tid_DWORD,       "Zone" )
    SER_VALUE( m_sAdapterDesc,  tid_STRING_OBJ,  "AdapterDesc" )
    SER_VALUE( m_bStealthed,    tid_BOOL,        "Stealthed" )
    SER_VALUE( m_nMac,          tid_QWORD,       "Mac" )
    SER_VALUE( m_bHidden,       tid_BOOL,        "Hidden" )
    SER_VALUE_C( m_IP,          cIP::eIID,       "IPv",     &g_SkipFieldAfterV1_Serializer )
    SER_VALUE_C( m_Mask,        cIP::eIID,       "Maskv",   &g_SkipFieldAfterV1_Serializer )
    SER_VALUE_C( m_eType,       tid_DWORD,       "Type",    &g_SkipFieldAfterV1_Serializer )
IMPLEMENT_SERIALIZABLE_END()

typedef cVector<cAHFWNetwork> cAHFWNetworks;

//+ -------

struct cFwNets : public cSerializable
{
    DECLARE_IID(cFwNets, cSerializable, PID_AHFW, _eiid_cFwNets);

    cAHFWNetworks     m_aNetworks;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cFwNets, 0)
    SER_VECTOR( m_aNetworks,	cAHFWNetwork::eIID,		"Networks" )
IMPLEMENT_SERIALIZABLE_END()


//+ -------

struct cFwChangeNet : public cSerializable
{
    DECLARE_IID(cFwChangeNet, cSerializable, PID_AHFW, _eiid_cFwChangeNet);

    cFwNets             m_aNetworks;
    tUINT               m_aIndex;
    cAHFWNetwork        m_aNewNet;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cFwChangeNet, 0)
    SER_VALUE( m_aNetworks,	    cFwNets::eIID,		"Networks" )
    SER_VALUE( m_aIndex,	    tid_UINT,		    "Index" )
    SER_VALUE( m_aNewNet,       cAHFWNetwork::eIID,	"NewNet" )
IMPLEMENT_SERIALIZABLE_END()


//+ -------

struct cFwTransmitSize : public cSerializable
{
    cFwTransmitSize() : cSerializable(),
        m_nBytesSent(0),
        m_nBytesRecieved(0)
    {}

    DECLARE_IID(cFwTransmitSize, cSerializable, PID_AHFW, _eiid_cFwTransmitSize);

    tQWORD                              m_nBytesSent;
    tQWORD                              m_nBytesRecieved;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cFwTransmitSize, 0)
    SER_VALUE(  m_nBytesSent,           tid_DWORD,          "BytesSent" )
    SER_VALUE(  m_nBytesRecieved,       tid_DWORD,          "BytesRecieved" )
IMPLEMENT_SERIALIZABLE_END()

//+ -------

struct cFwHostStat : public cSerializable
{
    cFwHostStat() : cSerializable()
    {}

    DECLARE_IID(cFwHostStat, cSerializable, PID_AHFW, _eiid_cFwHostStat);

    cIP                                 m_Host;
    cStringObj                          m_sHostName;
    cFwTransmitSize                     m_Total;
    cFwTransmitSize                     m_Tcp;
    cFwTransmitSize                     m_Udp;
    cFwTransmitSize                     m_Broadcast;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cFwHostStat, 0)
    SER_VALUE(  m_Host,                 cIP::eIID,             "Host" )
    SER_VALUE(  m_sHostName,            tid_STRING_OBJ,        "HostName" )
    SER_VALUE(  m_Total,                cFwTransmitSize::eIID, "Total" )
    SER_VALUE(  m_Tcp,                  cFwTransmitSize::eIID, "Tcp" )
    SER_VALUE(  m_Udp,                  cFwTransmitSize::eIID, "Udp" )
    SER_VALUE(  m_Broadcast,            cFwTransmitSize::eIID, "Broadcast" )
IMPLEMENT_SERIALIZABLE_END()

typedef cVector<cFwHostStat> cFwHostStatistics;

//+ ------------------------------------------------------------------------------------------
struct cAHFWAppConnection : public cSerializable
{
    cAHFWAppConnection() : cSerializable(),
        m_Connections(),
        m_AppName(),
        m_PID(0)
    {
        m_AppName.setCP(cCP_UNICODE);
        m_CmdLine.setCP(cCP_UNICODE);
    }
    
    DECLARE_IID(cAHFWAppConnection, cSerializable, PID_AHFW, _eiid_applications);

    cStringObj					m_AppName;			// название процесса 
    cStringObj					m_CmdLine;			// название процесса 
    tDWORD						m_PID;				// PID процесса
    cVector<cAHFWConnection>	m_Connections;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cAHFWAppConnection, "AHFWAppConnection")
//	SER_BASE(cSerializable, 0)
    SER_VALUE(m_AppName,		tid_STRING_OBJ,		"AppName")
    SER_VALUE(m_CmdLine,		tid_STRING_OBJ,		"CmdLine")
    SER_VALUE_M(PID,			tid_DWORD)
    SER_VECTOR(m_Connections,	cAHFWConnection::eIID, "AppConnections")
IMPLEMENT_SERIALIZABLE_END()

typedef cVector<cAHFWAppConnection> cAHFWAppConnections;

//+ ------------------------------------------------------------------------------------------

struct cAHFWStatisticsEx : public cTaskStatistics
{
    cAHFWStatisticsEx(): cTaskStatistics(),
        m_Applications(),
        m_Connections(),
        m_Networks()
    {}

    DECLARE_IID(cAHFWStatisticsEx, cTaskStatistics, PID_AHFW, _eiid_fw_statex);

    cAHFWAppConnections	m_Applications;
    cAHFWAppConnections	m_Connections;
    cAHFWNetworks       m_Networks;
    cFwHostStatistics   m_HostStat;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cAHFWStatisticsEx, 0)
    SER_BASE(cTaskStatistics, 0)
    SER_VECTOR(m_Applications,  cAHFWAppConnection::eIID,   "Applications")
    SER_VECTOR(m_Connections,   cAHFWAppConnection::eIID,   "Connections")
    SER_VECTOR(m_Networks,      cAHFWNetwork::eIID,         "Networks")
    SER_VECTOR(m_HostStat,      cFwHostStat::eIID,          "HostStat")
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

//+ ------------------------------------------------------------------------------------------

struct cFWPort : public cSerializable
{
    cFWPort() : cSerializable(),
        m_PortLo(0),
        m_PortHi(0)
    {
    }

    DECLARE_IID(cFWPort, cSerializable, PID_AHFW, _eiid_port);

    bool operator == (const cFWPort &P) const
    {
        return (m_PortLo == P.m_PortLo) && (m_PortHi == P.m_PortHi);
    }

    _eFWPortType GetType()
    {
        return m_PortLo == m_PortHi ? _fwptSinglePort : _fwptPortRange;
    }

    bool IsMatch(tWORD Port)
    {
        return m_PortLo <= Port && m_PortHi >= Port;
    }

    static bool IsMatch(tWORD Port, cVector<cFWPort> &Ports)
    {
        for(tDWORD i = 0; i < Ports.size(); i++)
            if( Ports[i].IsMatch(Port) )
                return true;
        return Ports.empty();
    }

    tWORD	m_PortLo;
    tWORD	m_PortHi;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cFWPort, "FWPort")
    SER_VALUE_M(PortLo,		tid_WORD)
    SER_VALUE_M(PortHi,		tid_WORD)
IMPLEMENT_SERIALIZABLE_END()


struct cFWAddress : public cSerializable
{
    cFWAddress() : cSerializable(),
        m_AddrName(),
        m_Address(),
        m_IPLo_(0),
        m_IPHi_(0),
        m_IPMask_(0),
        m_AddressIP_()
    {
        m_AddrName.setCP(cCP_UNICODE);
        m_Address.setCP(cCP_UNICODE);
    }

    DECLARE_IID(cFWAddress, cSerializable, PID_AHFW, _eiid_address);

    bool operator == (const cFWAddress &P) const
    {
        return (m_AddrName == P.m_AddrName) && (m_Address == P.m_Address) && (m_IPLo == P.m_IPLo) && (m_IPHi == P.m_IPHi)  && (m_IPMask == P.m_IPMask);
    }

    _eFWAddressType GetType()
    {
        if(!m_Address.empty())   return _fwatSymbolic;
        if(m_IPMask.IsValid())   return _fwatSubnet;
        if(m_IPLo == m_IPHi)     return _fwatSingleIp;
        return _fwatIpRange;
    }

    bool IsMatch(const cIP &IP)
    {
        switch(GetType())
        {
        case _fwatSymbolic:
            {
                for(tDWORD i = 0; i < m_AddressIP.size(); i++)
                    if( IP == m_AddressIP[i] )
                        return true;
            }
            return false;
        
        case _fwatSubnet:
            return (m_IPLo & m_IPMask) == (IP & m_IPMask);
        }
        return m_IPLo <= IP && m_IPHi >= IP;
    }

    static bool IsMatch(const cIP &IP, cVector<cFWAddress> &Addrs)
    {
        for(tDWORD i = 0; i < Addrs.size(); i++)
            if( Addrs[i].IsMatch(IP) )
                return true;
        return Addrs.empty();
    }

    _eFWWellKnownAddress RecognizeAddress()
    {
        if( !(GetType() == _fwatSymbolic && m_AddressIP.empty()) )
            return _fwaUnknown;
        if( m_Address == L"%dhcp%" ) return _fwaDHCP;
        if( m_Address == L"%dns%"  ) return _fwaDNS;
        if( m_Address == L"%wins%" ) return _fwaWINS;
        return _fwaUnknown;
    }

    void pr_call finalizeStreamDeser( tERROR err, tUINT field_num, const cSerDescriptor* dsc )
    {
        if (field_num == 6)
        {
            m_IPLo.SetV4 (m_IPLo_);
            m_IPHi.SetV4 (m_IPHi_);
            m_IPMask.SetV4(m_IPMask_);
            m_AddressIP.resize(m_AddressIP_.size());
            for(tDWORD i = 0; i < m_AddressIP.size(); i++)
                m_AddressIP[i].SetV4(m_AddressIP_[i]);
        }
    }

    cStringObj					m_AddrName;			// название правила
    cStringObj					m_Address;			// ip name

    cVector<tIPv4>				m_AddressIP_;       // deprecated
    tIPv4						m_IPLo_;            // deprecated
    tIPv4						m_IPHi_;            // deprecated
    tIPv4						m_IPMask_;          // deprecated

    cVector<cIP>                m_AddressIP;
    cIP                         m_IPLo;
    cIP                         m_IPHi;
    cIP                         m_IPMask;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cFWAddress, "FWAddress")
    SER_VALUE_M  (AddrName,			tid_STRING_OBJ)
    SER_VALUE_M  (Address,			tid_STRING_OBJ)
    SER_VALUE    (m_IPLo_,			tid_IPv4, "IPLo")
    SER_VALUE    (m_IPHi_,			tid_IPv4, "IPHi")
    SER_VALUE    (m_IPMask_,		tid_IPv4, "IPMask")
    SER_VECTOR   (m_AddressIP_,		tid_IPv4, "AddressIP")
    SER_VALUE_C  (m_IPLo,			cIP::eIID, "IPvLo",     &g_SkipFieldAfterV1_Serializer)
    SER_VALUE_C  (m_IPHi,			cIP::eIID, "IPvHi",     &g_SkipFieldAfterV1_Serializer)
    SER_VALUE_C  (m_IPMask,			cIP::eIID, "IPvMask",   &g_SkipFieldAfterV1_Serializer)
    SER_VECTOR_C (m_AddressIP,		cIP::eIID, "AddressIPv",&g_SkipFieldAfterV1_Serializer)
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cFwBaseRule : public cSerializable
{
    cFwBaseRule() :
        m_bEnabled(cTRUE),
        m_bBlocking(cFALSE),
        m_bLog(cFALSE),
        m_bWarning(cFALSE),
        m_nTimeFrom(0),
        m_nTimeTill(0),
        m_nProto(_fwPROTO_TCP),
        m_eDirection(_fwInboundOutbound),
        m_nTriggers(0)
    {}
    
    DECLARE_IID( cFwBaseRule, cSerializable, PID_AHFW, _eiid_base_rule );

    enum
    {
        prpFwRemAddr    = 0x01,
        prpFwRemPort    = 0x02,
        prpFwLocAddr    = 0x04,
        prpFwLocPort    = 0x08,
        prpFwIcmpCode   = 0x10,
        prpFwTimeRng    = 0x20,
        prpFwIcmpv6Code = 0x40,
        prpFwEditedNow  = 0x80000000
    };

    bool IsAlike(cFwBaseRule &P) const
    {
        return 
            m_sName            == P.m_sName &&
            m_bBlocking        == P.m_bBlocking &&
            m_nTimeFrom        == P.m_nTimeFrom &&
            m_nTimeTill        == P.m_nTimeTill &&
            m_nProto           == P.m_nProto &&
            m_eDirection       == P.m_eDirection &&
            m_aRemotePorts     == P.m_aRemotePorts &&
            m_aRemoteAddresses == P.m_aRemoteAddresses &&
            m_aLocalPorts      == P.m_aLocalPorts &&
            m_aLocalAddresses  == P.m_aLocalAddresses;
    }
    void    InitProps()                                        { m_nTriggers = GetProps(); }
    tDWORD  GetProps();
    void    CleanupProps();

    cStringObj                          m_sName;
    
    tBOOL                               m_bEnabled;
    tBOOL                               m_bBlocking;
    tBOOL                               m_bLog;
    tBOOL                               m_bWarning;
    tDWORD                              m_nTimeFrom;
    tDWORD                              m_nTimeTill;
        
    tBYTE                               m_nProto;
    _eFwDirection                       m_eDirection;
    cVector<cFWPort>                    m_aRemotePorts;     // for TCP/UDP only
    cVector<cFWAddress>                 m_aRemoteAddresses;
    cVector<cFWPort>                    m_aLocalPorts;      // for TCP/UDP only
    cVector<cFWAddress>                 m_aLocalAddresses;

    tDWORD                              m_nTriggers;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cFwBaseRule, 0 )
    SER_VALUE( m_sName,                 tid_STRING_OBJ,     "Name" )
    SER_VALUE( m_bEnabled,              tid_BOOL,           "Enabled" )
    SER_VALUE( m_bBlocking,             tid_BOOL,           "Blocking" )
    SER_VALUE( m_bLog,                  tid_BOOL,           "Log" )
    SER_VALUE( m_bWarning,              tid_BOOL,           "Warning" )
    SER_VALUE( m_nTimeFrom,             tid_DWORD,          "TimeFrom" )
    SER_VALUE( m_nTimeTill,             tid_DWORD,          "TimeTill" )
    SER_VALUE( m_nProto,                tid_BYTE,           "Proto" )
    SER_VALUE( m_eDirection,            tid_DWORD,          "Direction" )
    SER_VECTOR( m_aRemotePorts,         cFWPort::eIID,      "RemotePorts" )
    SER_VECTOR( m_aRemoteAddresses,     cFWAddress::eIID,   "RemoteAddresses" )
    SER_VECTOR( m_aLocalPorts,          cFWPort::eIID,      "LocalPorts" )
    SER_VECTOR( m_aLocalAddresses,      cFWAddress::eIID,   "LocalAddresses" )
    SER_VALUE( m_nTriggers,             tid_DWORD,          "Triggers" )
IMPLEMENT_SERIALIZABLE_END()

typedef cVector<cFwBaseRule> cFwBaseRules;

//-----------------------------------------------------------------------------

struct cFwPacketRule : public cFwBaseRule
{
    cFwPacketRule() : m_nIcmpCode(FwInvalidIcmpCode) {}
    
    DECLARE_IID( cFwPacketRule, cFwBaseRule, PID_AHFW, _eiid_packet_rule );

    tBYTE                               m_nIcmpCode;   // for ICMP protocol only
};

IMPLEMENT_SERIALIZABLE_BEGIN( cFwPacketRule, 0 )
    SER_BASE(  cFwBaseRule, 0 )
    SER_VALUE( m_nIcmpCode,             tid_BYTE,           "IcmpCode" )
IMPLEMENT_SERIALIZABLE_END()

typedef cVector<cFwPacketRule> cFwPacketRules;

//-----------------------------------------------------------------------------

struct cFwAppRule : public cSerializable
{
    cFwAppRule() :
        m_bEnabled(cTRUE),
        m_bUseCmdLine(cFALSE),
        m_bCheckHash(cTRUE)
    {
        memset(m_nHash, 0, sizeof(m_nHash));
    }
    
    DECLARE_IID( cFwAppRule, cSerializable, PID_AHFW, _eiid_app_rule );

    bool    IsHashEmpty() const                                { for(tDWORD i = 0; i < FwHashSize; i++) if( m_nHash[i] ) return false; return true; }
    bool    IsHashEqual(tBYTE *pHash) const                    { return !memcmp(m_nHash, pHash, FwHashSize); }
    void    CopyHash(tBYTE *pHash)                             { memcpy(m_nHash, pHash, FwHashSize); }
    void    SetEnabled(tBOOL bEnabled)                         { for(tDWORD i = 0; i < m_aRules.size(); i++) m_aRules[i].m_bEnabled = bEnabled; m_bEnabled = bEnabled; }
    void    SetBlocked(tBOOL bBlocked)                         { for(tDWORD i = 0; i < m_aRules.size(); i++) m_aRules[i].m_bBlocking = bBlocked; }
    bool    IsAnyEnabled() const                               { tDWORD i; for( i = 0; i < m_aRules.size() && !m_aRules[i].m_bEnabled; i++); return i < m_aRules.size(); }
    bool    IsItYou(cStringObj &sAppName, tBYTE *pHash, cStringObj &sCmdLine, tBOOL bUseCmdLine) const;
    void    InsertRule(cFwBaseRule &Rule, tSIZE_T pos = 0);
    void    InsertRules(cFwBaseRules &aRules, tSIZE_T pos = 0) { for(tSIZE_T i = 0; i < aRules.size(); i++) InsertRule(aRules[i], pos + i); }
    void    AppendRule(cFwBaseRule &Rule)                      { InsertRule(Rule, m_aRules.size()); }
    void    AppendRules(cFwBaseRules &aRules)                  { for(tSIZE_T i = 0; i < aRules.size(); i++) AppendRule(aRules[i]); }
    void    InitProps()                                        { for(tDWORD i = 0; i < m_aRules.size(); i++) m_aRules[i].InitProps(); }


    
    tBOOL                               m_bEnabled;
    cStringObj                          m_sAppName;
    cStringObj                          m_sCmdLine;
    tBOOL                               m_bUseCmdLine;
    tBYTE								m_nHash[FwHashSize];
    cVector<cFwBaseRule>                m_aRules;
    tBOOL                               m_bCheckHash;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cFwAppRule, 0 )
    SER_VALUE( m_bEnabled,              tid_BOOL,           "Enabled" )
    SER_VALUE( m_sAppName,              tid_STRING_OBJ,     "AppName" )
    SER_VALUE( m_sCmdLine,              tid_STRING_OBJ,     "CmdLine" )
    SER_VALUE( m_bUseCmdLine,           tid_BOOL,           "UseCmdLine" )
    SER_VALUE( m_nHash,                 tid_BINARY,         "Hash" )
    SER_VECTOR(m_aRules,                cFwBaseRule::eIID,  "Rules" )
    SER_VALUE_C( m_bCheckHash,          tid_BOOL,           "CheckHash",   &g_SkipFieldAfterV1_Serializer )
IMPLEMENT_SERIALIZABLE_END()

typedef cVector<cFwAppRule> cFwAppRules;

//-----------------------------------------------------------------------------

struct cFwAppRuleEx : public cFwBaseRule
{
    cFwAppRuleEx() :
        m_bUseCmdLine(cFALSE),
        m_bCheckHash(cTRUE)
    {
        memset(m_nHash, 0, sizeof(m_nHash));
    }
    
    DECLARE_IID( cFwAppRuleEx, cFwBaseRule, PID_AHFW, _eiid_app_rule_ex );
    
    void    CopyHash(tBYTE *pHash)              { memcpy(m_nHash, pHash, FwHashSize); }

    cStringObj                          m_sAppName;
    cStringObj                          m_sCmdLine;
    tBOOL                               m_bUseCmdLine;
    tBYTE								m_nHash[FwHashSize];
    tBOOL                               m_bCheckHash;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cFwAppRuleEx, 0 )
    SER_BASE(  cFwBaseRule, 0 )
    SER_VALUE( m_sAppName,              tid_STRING_OBJ,     "AppName" )
    SER_VALUE( m_sCmdLine,              tid_STRING_OBJ,     "CmdLine" )
    SER_VALUE( m_bUseCmdLine,           tid_BOOL,           "UseCmdLine" )
    SER_VALUE( m_nHash,                 tid_BINARY,         "Hash" )
    SER_VALUE( m_bCheckHash,            tid_BOOL,           "CheckHash" )
IMPLEMENT_SERIALIZABLE_END()

typedef cVector<cFwAppRuleEx> cFwAppRulesEx;

//-----------------------------------------------------------------------------

#define pmc_FW_RULE_ADDED 0xeb75b4a2 //is a crc32 for "pmc_FW_RULE_ADDED"

#define FWPRESETNAME_PACKETRULE         "PacketRule"

struct cFwAppRulePreset : public cSerializable
{
    cFwAppRulePreset() :
        m_bUseCmdLine(cFALSE),
        m_nIcmpCode(FwInvalidIcmpCode),
        m_bCheckHash(cTRUE)
    {}

    DECLARE_IID( cFwAppRulePreset, cSerializable, PID_AHFW, _eiid_app_rule_preset );

    void    InitProps()                                        { for(tDWORD i = 0; i < m_aRules.size(); i++) m_aRules[i].InitProps(); }
    bool    IsPacketPreset()                                   { return m_sName == FWPRESETNAME_PACKETRULE; }

    cStringObj                          m_sName;
    cVector<cStringObj>                 m_aApps;
    cStringObj                          m_sCmdLine;
    tBOOL                               m_bUseCmdLine;
    tBYTE                               m_nIcmpCode;   // for ICMP protocol only
    cVector<cFwBaseRule>                m_aRules;
    tBOOL                               m_bCheckHash;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cFwAppRulePreset, 0 )
    SER_VALUE(  m_sName,                tid_STRING_OBJ,     "Name" )
    SER_VECTOR( m_aApps,                tid_STRING_OBJ,     "Apps" )
    SER_VALUE(  m_sCmdLine,             tid_STRING_OBJ,     "CmdLine" )
    SER_VALUE(  m_bUseCmdLine,          tid_BOOL,           "UseCmdLine" )
    SER_VALUE(  m_nIcmpCode,            tid_BYTE,           "IcmpCode" )
    SER_VECTOR( m_aRules,               cFwBaseRule::eIID,  "Rules" )
    SER_VALUE(  m_bCheckHash,           tid_BOOL,           "CheckHash" )
IMPLEMENT_SERIALIZABLE_END()

typedef cVector<cFwAppRulePreset> cFwAppRulePresets;

//-----------------------------------------------------------------------------

struct cAHFWSettings : public cTaskSettings
{
    cAHFWSettings(): cTaskSettings(),
        m_ProtectionLevel(_fwwmAskUser),
        m_InetStealthed(cTRUE),
        m_NdisCheckTdi(cTRUE),
        m_RefreshIPTimeout(60 * 24),
        m_HashFuncVersion(0)
    {
    }

    DECLARE_IID_SETTINGS(cAHFWSettings, cTaskSettings, PID_AHFW);
    
    cFwAppRule *FindAppRule(cStringObj &sAppName, tBYTE *pHash, cStringObj &sCmdLine, tBOOL bUseCmdLine)
    {
        return FindAppRule(m_AppRules, sAppName, pHash, sCmdLine, bUseCmdLine);
    }

    static cFwAppRule *FindAppRule(cFwAppRules &aRules, cStringObj &sAppName, tBYTE *pHash, cStringObj &sCmdLine, tBOOL bUseCmdLine)
    {
        for(tDWORD i = 0; i < aRules.size(); i++)
            if( aRules[i].IsItYou(sAppName, pHash, sCmdLine, bUseCmdLine) )
                return &aRules[i];
        return NULL;
    }

    _eAHFWWorkingMode		m_ProtectionLevel;
    cFwPacketRules          m_PortRules;
    cFwAppRules	            m_AppRules;
    cAHFWNetworks           m_KnownNetworks;
    tBOOL                   m_InetStealthed;
    tBOOL                   m_NdisCheckTdi;

    tDWORD                  m_RefreshIPTimeout;
    tDWORD					m_HashFuncVersion;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cAHFWSettings, 0)
    SER_BASE(cTaskSettings, 0)
    SER_VALUE_M(ProtectionLevel,	tid_DWORD)

//	SER_VECTOR_M(PortRules,			cFwPacketRule::eIID)
    SER_VECTOR_STREAM(m_PortRules,	cFwPacketRule::eIID, "PortRules")

//	SER_VECTOR_M(AppRules,			cFwAppRule::eIID)
    SER_VECTOR_STREAM(m_AppRules,	cFwAppRule::eIID, "AppRules")

    SER_VECTOR_M(KnownNetworks,     cAHFWNetwork::eIID)
    SER_VALUE_M( InetStealthed,     tid_BOOL)
    SER_VALUE_M( NdisCheckTdi,      tid_BOOL)
    SER_VALUE_M(RefreshIPTimeout,	tid_DWORD)
    SER_VALUE_M(HashFuncVersion,	tid_DWORD)
IMPLEMENT_SERIALIZABLE_END()

//+ ------------------------------------------------------------------------------------------

#define pmc_TASKAHFW_REPORT_EVENT	0x2a7a9cee // is a crc32 for "pmc_TASKAHFW_REPORT_EVENT"
#define pmc_TASKAHFW_WARNING_EVENT	0xbc1d9712 // is a crc32 for "pmc_TASKAHFW_WARNING_EVENT"

struct cAHFWEventReportBase : public cTaskHeader
{
    DECLARE_IID(cAHFWEventReportBase, cTaskHeader, PID_AHFW, _eiid_event_report_base);

    cAHFWEventReportBase() :
        m_New(cFALSE),
        m_Time(-1),
        m_IsBlocking(cFALSE),
        m_Direction(_fwInbound),
        m_Proto(0),
        m_RemoteAddress_(0),
        m_LocalAddress_(0),
        m_RemotePort(0),
        m_LocalPort(0),
        m_Log(cFALSE),
        m_Warning(cFALSE),
        m_App(cFALSE)
    {
        m_RuleName.setCP(cCP_UNICODE);
        m_RemoteHostName.setCP(cCP_UNICODE);
        m_LocalHostName.setCP(cCP_UNICODE);
    }

    void pr_call finalizeStreamDeser( tERROR err, tUINT field_num, const cSerDescriptor* dsc )
    {
        if (field_num == 15) // pre-сIP version
        {
            m_RemoteAddress.SetV4(m_RemoteAddress_);
            m_LocalAddress.SetV4(m_LocalAddress_);
        }
    }

    tBOOL				m_New;                  // правило создано
    __time32_t			m_Time;					// врем€ событи€

    cStringObj			m_RuleName;				// им€ правила
    tBOOL				m_IsBlocking;			// пропущеный / заблокированный
    _eFwDirection		m_Direction;            // направление
    tBYTE				m_Proto;				// ip-based протокол
    tIPv4				m_RemoteAddress_;		// deprecated
    tIPv4				m_LocalAddress_;		// deprecated
    cStringObj			m_RemoteHostName;	    // им€ удаленного хоста
    cStringObj			m_LocalHostName;	    // им€ локального хоста
    tWORD				m_RemotePort;			// удаленный порт (только дл€ протоколов TCP и UDP)
    tWORD				m_LocalPort;			// локальный порт (только дл€ протоколов TCP и UDP)
    tBOOL				m_Log;					// признак уведомлени€
    tBOOL				m_Warning;				// признак уведомлени€
    tBOOL				m_App;					// признак уведомлени€ дл€ приложени€
    cIP				    m_RemoteAddress;		// удаленный IP-адрес
    cIP				    m_LocalAddress;		    // локальный IP-адрес
};

IMPLEMENT_SERIALIZABLE_BEGIN(cAHFWEventReportBase, 0)
    SER_BASE(cTaskHeader, 0)
    SER_VALUE_M(New,        		tid_BOOL)
    SER_VALUE_M(Time,				tid_DWORD)
    SER_VALUE_M(RuleName,			tid_STRING_OBJ)
    SER_VALUE_M(IsBlocking,			tid_BOOL)
    SER_VALUE_M(Direction,			tid_DWORD)
    SER_VALUE_M(Proto,				tid_BYTE)
    SER_VALUE(m_RemoteAddress_,		tid_IPv4,       "RemoteAddress")
    SER_VALUE(m_LocalAddress_,		tid_IPv4,       "LocalAddress")
    SER_VALUE_M(RemoteHostName,		tid_STRING_OBJ)
    SER_VALUE_M(LocalHostName,		tid_STRING_OBJ)
    SER_VALUE_M(RemotePort,			tid_WORD)
    SER_VALUE_M(LocalPort,			tid_WORD)
    SER_VALUE_M(Log,				tid_BOOL)
    SER_VALUE_M(Warning,			tid_BOOL)
    SER_VALUE_M(App,				tid_BOOL)
    SER_VALUE_C(m_RemoteAddress,	cIP::eIID,      "RemoteIPv",    &g_SkipFieldAfterV1_Serializer)
    SER_VALUE_C(m_LocalAddress,		cIP::eIID,      "LocalIPv",     &g_SkipFieldAfterV1_Serializer)
IMPLEMENT_SERIALIZABLE_END()


//-----------------------------------------------------------------------------
// Packet event
struct cAHFWPacketEventReport : public cAHFWEventReportBase
{
    DECLARE_IID(cAHFWPacketEventReport, cAHFWEventReportBase, PID_AHFW, _eiid_packet_event_report);

    cAHFWPacketEventReport() :
        m_IcmpCode(FwInvalidIcmpCode)
    {}

    tBYTE				m_IcmpCode;				// ICMP код (только дл€ протокола ICMP)
};

IMPLEMENT_SERIALIZABLE_BEGIN(cAHFWPacketEventReport, 0)
    SER_BASE(cAHFWEventReportBase, 0)
    SER_VALUE_M(IcmpCode,			tid_BYTE)
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// Application event
struct cAHFWAppEventReport : public cAHFWEventReportBase
{
    DECLARE_IID(cAHFWAppEventReport, cAHFWEventReportBase, PID_AHFW, _eiid_app_event_report);
    
    cAHFWAppEventReport() :
        m_PID(0)
    {
        m_AppName.setCP(cCP_UNICODE);
        m_CmdLine.setCP(cCP_UNICODE);
    }

    cStringObj			    m_AppName;				// им€ приложени€
    cStringObj			    m_CmdLine;				// командна€ строка
    tDWORD				    m_PID;					// PID приложени€
    cFwAppRule	            m_AppRule;              // правило, созданое или отмодифицированоное в результате запроса на обучение
};

IMPLEMENT_SERIALIZABLE_BEGIN(cAHFWAppEventReport, 0)
    SER_BASE(cAHFWEventReportBase, 0)
    SER_VALUE_M(AppName,			tid_STRING_OBJ)
    SER_VALUE_M(CmdLine,			tid_STRING_OBJ)
    SER_VALUE(m_PID,		        tid_DWORD,			"AppPID")
    SER_VALUE_M(AppRule,			cFwAppRule::eIID)
IMPLEMENT_SERIALIZABLE_END()



//+ ------------------------------------------------------------------------------------------
//+ ------------------------------------------------------------------------------------------

struct cAskActionFwLearning : public cAskObjectAction
{
    DECLARE_IID( cAskActionFwLearning, cAskObjectAction, PID_AHFW,  _eiid_ask_appevent);

    enum Actions {LEARNING = eIID};

    cAHFWAppEventReport	m_AppEvent;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAskActionFwLearning, 0 )
    SER_BASE(cAskObjectAction, 0)
    SER_VALUE(m_AppEvent,	cAHFWAppEventReport::eIID, 0)
IMPLEMENT_SERIALIZABLE_END()

//+ ------------------------------------------------------------------------------------------
// событие Application Changed
#define pmc_TASKAHFWAPPCHANGED_REPORT_EVENT	0x25be9a84 /*is a crc32 for "pmc_TASKAHFWAPPCHANGED_REPORT_EVENT"*/

struct cAHFWAppChanged : public cTaskHeader
{
    cAHFWAppChanged() : cTaskHeader(),
        m_ApplicationName(),
        m_PID(0),
        m_Time(-1),
        m_IsBLocked(cTRUE)
    {
        m_ApplicationName.setCP(cCP_UNICODE);
    }

    DECLARE_IID(cAHFWAppChanged, cTaskHeader, PID_AHFW, _eiid_appchanged);

    cStringObj				m_ApplicationName;	// им€ приложени€
    tDWORD					m_PID;				// PID приложени€
    __time32_t				m_Time;				// врем€ событи€
    tBOOL					m_IsBLocked;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cAHFWAppChanged, 0)
    SER_BASE(cTaskHeader, 0)
    SER_VALUE_M(ApplicationName,	tid_STRING_OBJ)
    SER_VALUE(m_PID,		        tid_DWORD,			"AppPID")
    SER_VALUE_M(Time,				tid_DWORD)
    SER_VALUE_M(IsBLocked,			tid_BOOL)
IMPLEMENT_SERIALIZABLE_END()

//+ ------------------------------------------------------------------------------------------

struct cAskActionFwAppChanged : public cAskObjectAction
{
    DECLARE_IID( cAskActionFwAppChanged, cAskObjectAction, PID_AHFW,  _eiid_ask_appchanged);

    enum Actions {APP_CHANGED = eIID};

    cAHFWAppChanged		m_AppChanged;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAskActionFwAppChanged, 0 )
    SER_BASE(cAskObjectAction, 0)
    SER_VALUE_M(AppChanged, cAHFWAppChanged::eIID)
IMPLEMENT_SERIALIZABLE_END()

//+ ------------------------------------------------------------------------------------------

struct cAskActionFwNetChanged : public cAskObjectAction
{
    DECLARE_IID(cAskActionFwNetChanged, cAskObjectAction, PID_AHFW, _eiid_network_changed);
    
    enum Actions {NETWORK_CHANGED = eIID};

    cAHFWNetwork      m_NewNetwork;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cAskActionFwNetChanged, 0)
    SER_BASE(cAskObjectAction, 0)
    SER_VALUE( m_NewNetwork,  cAHFWNetwork::eIID,           "NewNetwork" )
IMPLEMENT_SERIALIZABLE_END()

//+ ------------------------------------------------------------------------------------------

inline tDWORD cFwBaseRule::GetProps()
{
    tDWORD nProp = 0;

    nProp |= m_aRemoteAddresses.empty() ? 0 : prpFwRemAddr;
    nProp |= m_aRemotePorts.empty()     ? 0 : prpFwRemPort;
    nProp |= m_aLocalAddresses.empty()  ? 0 : prpFwLocAddr;
    nProp |= m_aLocalPorts.empty()      ? 0 : prpFwLocPort;
    nProp |= m_nTimeFrom || m_nTimeTill ? prpFwTimeRng : 0;
    if( isBasedOn(cFwPacketRule::eIID) )
    {
        cFwPacketRule *pPortRule = (cFwPacketRule *)this;
        nProp |= pPortRule->m_nIcmpCode == FwInvalidIcmpCode ? 0 : 
            pPortRule->m_nProto == _fwPROTO_ICMP   ? cFwBaseRule::prpFwIcmpCode   : 
            pPortRule->m_nProto == _fwPROTO_ICMPV6 ? cFwBaseRule::prpFwIcmpv6Code : 0;
    }
    return nProp;
}

inline void cFwBaseRule::CleanupProps()
{
    if( !(m_nTriggers & prpFwRemAddr) ) m_aRemoteAddresses.clear();
    if( !(m_nTriggers & prpFwRemPort) ) m_aRemotePorts.clear();
    if( !(m_nTriggers & prpFwLocAddr) ) m_aLocalAddresses.clear();
    if( !(m_nTriggers & prpFwLocPort) ) m_aLocalPorts.clear();
    if( !(m_nTriggers & prpFwTimeRng) ) m_nTimeFrom = m_nTimeTill = 0;
    if( isBasedOn(cFwPacketRule::eIID) )
    {
        cFwPacketRule *pPortRule = (cFwPacketRule *)this;
        if( !(m_nTriggers & prpFwIcmpCode) && !(m_nTriggers & prpFwIcmpv6Code)) 
            pPortRule->m_nIcmpCode = FwInvalidIcmpCode;
    }
}

inline bool cFwAppRule::IsItYou(cStringObj &sAppName, tBYTE *pHash, cStringObj &sCmdLine, tBOOL bUseCmdLine) const
{
    if( pHash && !IsHashEqual(pHash) )
        return false;
    if( m_sAppName.compare(sAppName, fSTRING_COMPARE_CASE_INSENSITIVE ) != cSTRING_COMP_EQ )
        return false;
    if( m_bUseCmdLine != bUseCmdLine )
        return false;
    if( m_bUseCmdLine )
        return m_sCmdLine.compare(sCmdLine, fSTRING_COMPARE_CASE_INSENSITIVE ) == cSTRING_COMP_EQ;
    return true;
}

inline void cFwAppRule::InsertRule(cFwBaseRule &Rule, tSIZE_T pos)
{
    for(tDWORD i = 0; i < m_aRules.size(); i++)
    {
        if( m_aRules[i].IsAlike(Rule) )
        {
            m_bEnabled |= m_aRules[i].m_bEnabled = Rule.m_bEnabled;
            return;
        }
    }
    
    m_aRules.insert(pos, Rule);
    m_bEnabled |= Rule.m_bEnabled;
}


#endif // _ahfw_task_structs_
