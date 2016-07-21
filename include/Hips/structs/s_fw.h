#ifndef _hips_net_structs_
#define _hips_net_structs_

#include <ProductCore/structs/s_taskmanager.h>
#include <ProductCore/structs/s_ip.h>
#include "../plugin/p_hips.h"


enum
{
	eFwPort = 30,
	eFwAddress,
	eWebService,
	eIpRange,
};

//////////////////////////////////////////////////////////////////////////
// cFwPort

struct cFwPort : public cSerializable
{
    cFwPort() :
        m_PortLo(0),
        m_PortHi(0)
    {}

    DECLARE_IID(cFwPort, cSerializable, PID_HIPS, eFwPort);

    bool operator == (const cFwPort &P) const
    {
        return (m_PortLo == P.m_PortLo) && (m_PortHi == P.m_PortHi);
    }

    bool IsMatch(tWORD Port) const
    {
        return m_PortLo <= Port && m_PortHi >= Port;
    }

    static bool IsMatch(tWORD Port, cVector<cFwPort> &Ports)
    {
        for(tDWORD i = 0; i < Ports.size(); i++)
            if( Ports[i].IsMatch(Port) )
                return true;
        return Ports.empty();
    }

    tWORD	m_PortLo;
    tWORD	m_PortHi;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cFwPort, 0)
    SER_VALUE_M(PortLo,		tid_WORD)
    SER_VALUE_M(PortHi,		tid_WORD)
IMPLEMENT_SERIALIZABLE_END()

typedef cVector<cFwPort> cFwPorts;

//////////////////////////////////////////////////////////////////////////
// cFwAddress

struct cFwAddress : public cSerializable
{
	enum eType
	{
		tSymbolic = 0,
		tSubnet = 1,
		tSingleIp = 2,
	};

	enum eWellKnownAddress
	{
		aUnknown = 0,
		aDHCP = 1,
		aDNS = 2,
		aWINS = 3,
		aGW =4,			//all gateways in route table
		aDEF_GW = 5,	//default gateway
		aDC = 6,		//domain controller
		aHPROXY= 7,		//http proxy in internet exploder settings
	};


    DECLARE_IID(cFwAddress, cSerializable, PID_HIPS, eFwAddress);

    bool operator == (const cFwAddress &P) const
    {
        return (m_Address == P.m_Address) && (m_IP == P.m_IP);
    }

    eType GetType() const
    {
        if(!m_Address.empty()) return tSymbolic;
        if(m_IP.m_Mask)        return tSubnet;
        return tSingleIp;
    }

    bool IsMatch(const cIP &IP) const
    {
        switch(GetType())
        {
        case tSymbolic:
            for(tDWORD i = 0; i < m_AddressIP.size(); i++)
                if( m_AddressIP[i] == IP )
                    return true;
            return false;
        
        case tSubnet:
            return m_IP.IsMatch(IP);
        }
        return m_IP == IP;
    }

    static bool IsMatch(const cIP &IP, cVector<cFwAddress> &Addrs)
    {
        for(tDWORD i = 0; i < Addrs.size(); i++)
            if( Addrs[i].IsMatch(IP) )
                return true;
        return Addrs.empty();
    }

    eWellKnownAddress RecognizeAddress() const
    {
        if( !(GetType() == tSymbolic && m_AddressIP.empty()) )
            return aUnknown;
        if( m_Address == L"%dhcp%" )       return aDHCP;
        if( m_Address == L"%dns%"  )       return aDNS;
        if( m_Address == L"%wins%" )       return aWINS;
		if( m_Address == L"%gw%" )         return aGW;
		if( m_Address == L"%def_gw%" )     return aDEF_GW;
		if( m_Address == L"%dc%" )         return aDC;
		if( m_Address == L"%http_proxy%" ) return aHPROXY;
        return aUnknown;
    }

    cStringObj   m_Address;
    cVector<cIP> m_AddressIP;
    cIpMask      m_IP;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cFwAddress, 0)
    SER_VALUE_M  (Address,			tid_STRING_OBJ)
    SER_VALUE_M  (IP,				cIpMask::eIID)
    SER_VECTOR_M (AddressIP,		cIP::eIID)
IMPLEMENT_SERIALIZABLE_END()

typedef cVector<cFwAddress> cFwAddresses;

//////////////////////////////////////////////////////////////////////////
//	cWebService

struct cWebService : public cSerializable
{
	enum eWellKnownProtocol
	{
		pICMP	= 0x01,
		pTCP	= 0x06,
		pUDP	= 0x11,
		pICMPV6 = 0x3a,
	};

	enum eDirection
	{
		dInbound            = 0x01,
		dOutbound           = 0x02,
		dInboundOutbound    = 0x03,
		dInboundStream      = 0x04,
		dOutboundStream     = 0x05,
	};

	enum eFlags
	{
		fProtocol   = 0x01,
		fIcmpCode   = 0x02,
		fIcmpType   = 0x04,
	};

	cWebService() :
		m_nProto(pTCP),
		m_eDirection(dInboundOutbound),
		m_nIcmpType(0),
		m_nIcmpCode(0),
		m_nFlags(0/*fProtocol*/)
	{}

	DECLARE_IID( cWebService, cSerializable, PID_HIPS, eWebService );

	tBYTE         m_nProto;
	tBYTE         m_eDirection;
	tBYTE         m_nIcmpType;        // for ICMP only
	tBYTE         m_nIcmpCode;        // for ICMP only
	tDWORD        m_nFlags;           // eFlags
	cFwPorts      m_aRemotePorts;     // for TCP/UDP only
	cFwPorts      m_aLocalPorts;      // for TCP/UDP only
};

IMPLEMENT_SERIALIZABLE_BEGIN( cWebService, 0 )
	SER_VALUE( m_nProto,                tid_BYTE,           "Proto" )
	SER_VALUE( m_eDirection,            tid_BYTE,           "Direction" )
	SER_VALUE( m_nIcmpType,             tid_BYTE,           "IcmpType" )
	SER_VALUE( m_nIcmpCode,             tid_BYTE,           "IcmpCode" )
	SER_VALUE( m_nFlags,                tid_DWORD,          "Flags" )
	SER_VECTOR( m_aRemotePorts,         cFwPort::eIID,      "RemotePorts" )
	SER_VECTOR( m_aLocalPorts,          cFwPort::eIID,      "LocalPorts" )
IMPLEMENT_SERIALIZABLE_END()

//////////////////////////////////////////////////////////////////////////
//	cIpRange

struct cIpRange : public cSerializable
{
	DECLARE_IID( cIpRange, cSerializable, PID_HIPS, eIpRange );

	cFwAddresses  m_aRemoteAddresses;
    cFwAddresses  m_aLocalAddresses;  // for packet rules only
};

IMPLEMENT_SERIALIZABLE_BEGIN( cIpRange, 0 )
    SER_VECTOR( m_aRemoteAddresses,     cFwAddress::eIID,   "RemoteAddresses" )
    SER_VECTOR( m_aLocalAddresses,      cFwAddress::eIID,   "LocalAddresses" )
IMPLEMENT_SERIALIZABLE_END()

#endif // _hips_task_structs_

