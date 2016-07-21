#ifndef _netwatch_net_structs_
#define _netwatch_net_structs_


#ifdef _netwatcher_project_

#include <prague/prague.h>
#include <Prague/pr_serializable.h>
#include <ProductCore/structs/s_taskmanager.h>
#include <ProductCore/structs/s_ip.h>
//#include "NetWatcher.h"

#include <NetWatch/plugin/p_netwatch.h>
#endif //_netwatcher_project_
//#include "../plugin/p_hips.h"
//#include "AHTasks/structs/s_ahfw.h"
//#include "s_fw.h"

#define NetWatchImpl_PLUGINID 0x5d1

enum eInterfaceTypes
{
	If_other,
	If_LoopBack,
	If_Ethernet,
	If_WiFi,
	If_Tunnel,
	If_PPP,
	If_PPPoE,
	If_VPN,
	If_Modem,
};
enum eNetWatchStructs
{
	eNetZone =1,
	eNetAdapter,
	eNWSettings,
	eARPWatch,
	eZonaAddresses,
	eNWdata,
	eRoute,
	eAskActionNewZone,
};


enum eNetStates
{
	fVisible          = 0x01,
	fDeleted          = 0x02,
	fGateway          = 0x04,
	fConfigured       = 0x08,
	fConnected        = 0x10,
	fConnectedUndef   = 0x20,
	fTouch            = 0x40,
	fChanged          = 0x80,
	fTemporary        = 0x100
};

//////////////////////////////////////////////////////////////////////////
// cARPWatch

struct cARPWatch: public cSerializable
{
	cARPWatch():
	m_nZoneID(0),
	m_nMac(0),
	m_nMacLen(0),
	m_nIP4(0),
	m_nTimeCreated(0),
	m_nTimeDeleted(0)
	{}
	DECLARE_IID(cARPWatch, cSerializable, NetWatchImpl_PLUGINID, eARPWatch);
    tDWORD		 m_nZoneID;
	tQWORD       m_nMac;
	tDWORD       m_nMacLen;
	tIPv4        m_nIP4;
	tDWORD		 m_nTimeCreated;
	tDWORD		 m_nTimeDeleted;
};
IMPLEMENT_SERIALIZABLE_BEGIN(cARPWatch, 0 )
SER_VALUE( m_nZoneID,	          tid_DWORD,      "ZoneID" )
SER_VALUE( m_nMac,	              tid_QWORD,      "Mac" )
SER_VALUE( m_nMacLen,	          tid_DWORD,      "MacLen" )
SER_VALUE( m_nIP4,	              tid_DWORD,      "IP4" )
SER_VALUE( m_nTimeCreated,	      tid_DWORD,      "TimeCreated")
SER_VALUE( m_nTimeDeleted,	      tid_DWORD,      "TimeDeleted" )
IMPLEMENT_SERIALIZABLE_END()

/*
struct cRoute : public cSerializable
{
	cRoute():
	m_Metric(0),
	m_IfIndex(0)
	{}
	DECLARE_IID(cRoute, cSerializable, NetWatchImpl_PLUGINID, eRoute);
	tDWORD     m_Metric; 
	tDWORD     m_IfIndex;
	cIpMask    m_Network;
};
IMPLEMENT_SERIALIZABLE_BEGIN(cRoute, 0)
	SER_VALUE( m_Metric,          tid_DWORD,      "Metric" )
	SER_VALUE( m_IfIndex,         tid_DWORD,      "IfIndex")
	SER_VALUE( m_Network,         cIP::eIID,      "Network")
IMPLEMENT_SERIALIZABLE_END()

typedef cVector<cRoute> vRoutes;
*/

//////////////////////////////////////////////////////////////////////////
// cNetAdapterAddresses

struct cZonaAddresses : public cSerializable
{
	cZonaAddresses():
	m_nZoneID(0),
	m_IsDefaultGateway(false)
	{}

	DECLARE_IID(cZonaAddresses, cSerializable, NetWatchImpl_PLUGINID, eZonaAddresses);
	tDWORD       m_nZoneID;
	cIpMasks	 m_vIP;				//IP addresses of interfaces
	cIpMasks	 m_vRoutes;
	cVector<cIP> m_DHCP;
	cVector<cIP> m_DNS;
	cVector<cIP> m_WINS;
	cVector<cIP> m_GW;				 //gateways
	cVector<cIP> m_DC;               //domain controllers  (PDC,BDC)
	cIP			 m_HttpProxy;
	tBOOL		 m_IsDefaultGateway;
};
IMPLEMENT_SERIALIZABLE_BEGIN(cZonaAddresses, 0)
	SER_VALUE( m_nZoneID,          tid_DWORD,      "ID" )
	SER_VECTOR(m_vRoutes,          cIpMask::eIID,  "Routes")
	SER_VECTOR(m_vIP,              cIpMask::eIID,  "IP")
	SER_VECTOR(m_DHCP,             cIP::eIID,  "DHCP")
	SER_VECTOR(m_DNS,              cIP::eIID,  "DNS")
	SER_VECTOR(m_WINS,             cIP::eIID,  "WINS")
	SER_VECTOR(m_GW,               cIP::eIID,  "GW")
	SER_VECTOR(m_DC,               cIP::eIID,  "DC")
	SER_VALUE(m_HttpProxy,         cIP::eIID,  "HttpProxy")
	SER_VALUE(m_IsDefaultGateway,    tid_BOOL, "IsDefaultGateway")
	
IMPLEMENT_SERIALIZABLE_END()
typedef cVector<cZonaAddresses> vZonesAddresses;

struct cNWdata: public cSerializable
{
	DECLARE_IID( cNWdata, cSerializable, NetWatchImpl_PLUGINID, eNWdata );
	vZonesAddresses		m_ZonesAddresses;
	cZonaAddresses * FindZoneByID (tDWORD id)
	{
		for(tDWORD x=0;x<this->m_ZonesAddresses.size();x++)
		{
			if(this->m_ZonesAddresses[x].m_nZoneID==id)
				return &m_ZonesAddresses[x];
		}
		return 0;
	}
};
IMPLEMENT_SERIALIZABLE_BEGIN( cNWdata, 0 )
	SER_VECTOR(m_ZonesAddresses,             cZonaAddresses::eIID,  "ZonesAddresses")
IMPLEMENT_SERIALIZABLE_END()
//////////////////////////////////////////////////////////////////////////
// cNetAdapter

struct cNetAdapter : public cSerializable
{
	enum eSettings
	{
		fAskWhenNewZones        = 1,
		fAskOnlyWhenSecureZones = 2,
		fTemporaryNewZones      = 4,
		fArpSpoofingProtection  = 8,
	};
	
	cNetAdapter() :
		m_nMac(0),
		m_nMacLen(0),
		m_nID(0),
		m_nIfType(0),
		m_nTypeOfInterface(If_other),
		m_nFlags(0),
		m_nTimeCreated(0),
		m_nTimeConnected(0),
		m_nSettings(1),
		m_nUndefZoneID(0),

		m_nState(0),
		m_nZoneID(0),
		m_nIfIndex(0)
	{}
	void SetState(tDWORD state,tBOOL num)
	{
		if((m_nState & state)==num)
			return;
		if(state!=fTouch)
			m_nState|=fChanged;
		if(num)
			m_nState|=state;
		else
			m_nState-=m_nState & state;
	}
	tBOOL QState(tDWORD state) { return m_nState & state; }
	void SetConnected(tBOOL num) { SetState(fConnected,num); }
	tBOOL Connected() { return m_nState & fConnected; }


	DECLARE_IID(cNetAdapter, cSerializable, NetWatchImpl_PLUGINID, eNetAdapter);

	cStringObj  m_sAdapterFrendlyName;					//realtek
	cStringObj  m_sAdapterWinName;						//{12345-123
	cStringObj	m_sRasDeviceName;
	cStringObj	m_sRasDeviceType;
	tQWORD      m_nMac;
	tDWORD      m_nMacLen;
	tDWORD      m_nID;
	tDWORD      m_nIfType;
	tDWORD		m_nTypeOfInterface;						//eInterfaceTypes
	tDWORD      m_nFlags;
	tDWORD		m_nTimeCreated;
	tDWORD		m_nTimeConnected;
	tDWORD		m_nSettings;							//eSettings
	tDWORD      m_nUndefZoneID;


	// run time state
	
	tDWORD      m_nState;						
	tDWORD      m_nZoneID;
	tDWORD		m_nIfIndex;						//IPv4 adapter index for arp table
};

IMPLEMENT_SERIALIZABLE_BEGIN( cNetAdapter, 0 )
	SER_VALUE( m_sAdapterFrendlyName,tid_STRING_OBJ, "FrendlyName" )
	SER_VALUE( m_sAdapterWinName, tid_STRING_OBJ, "WinName" )
	SER_VALUE( m_sRasDeviceName,  tid_STRING_OBJ, "RasDeviceName")
	SER_VALUE( m_sRasDeviceType,  tid_STRING_OBJ, "RasDeviceType")
	SER_VALUE( m_nMac,            tid_QWORD,      "Mac" )
	SER_VALUE( m_nMacLen,         tid_DWORD,      "MacLen" )
	SER_VALUE( m_nID,             tid_DWORD,      "ID" )
	SER_VALUE( m_nIfType,         tid_DWORD,      "IfType" )
	SER_VALUE( m_nTypeOfInterface,tid_DWORD,      "TypeOfInterface" )
	SER_VALUE( m_nFlags,          tid_DWORD,      "Flags" )
	SER_VALUE( m_nTimeCreated,	  tid_DWORD,	  "TimeCreated" )
	SER_VALUE( m_nTimeConnected,  tid_DWORD,      "TimeConnected" )
	SER_VALUE( m_nSettings,		  tid_DWORD,      "Settings" )
	SER_VALUE( m_nState,          tid_DWORD,      "State" )
	SER_VALUE( m_nZoneID,         tid_DWORD,      "ZoneID" )
	SER_VALUE( m_nUndefZoneID,    tid_DWORD,      "UndefZoneID" )
	SER_VALUE( m_nIfIndex,        tid_DWORD,      "IfIndex" )
/*
	SER_VALUE( m_sConnectionName, tid_STRING_OBJ, "ConnectionName" )
	SER_VALUE( m_nWiFiType,       tid_DWORD,      "WiFiType" )
	SER_VALUE( m_nWiFiSecurity,   tid_DWORD,      "WiFiSecurity" )
	SER_VALUE( m_nWiFiAuthMode,   tid_DWORD,      "WiFiAuthMode")
*/
IMPLEMENT_SERIALIZABLE_END()


//////////////////////////////////////////////////////////////////////////
// cNetZone

struct cNetZone : public cSerializable
{
	enum eSettings
	{
		fPermanent			= 1,
		fConfirmOnConnect	= 2,
		fUptoGateway		= 4,
		fARPWatch           = 8,
		fWarnOnNewMAC		=0x10,
		fWarnOnChangeMAC	=0x20,
		fConnectedProgram	=0x40,
		fDisconnectedprogram=0x80,
		fDefPrinter			=0x100,
		fSavDefPrinter		=0x200,
		fWallPaper			=0x400,

		fAddAllowAll		=0x800,   //добавл€ть последнее правило "–азрешить все" дл€ адресов зоны
		fAddBlockAll		=0x1000,  //тоже, но "«апретить все" 
		
		fAllNetsFirst		=0x2000,  //правила "дл€ всех сетей (severity=0)" ввод€тс€ перед правилами зоны
		fAllNetsLast		=0x4000,  //тоже, но ввод€тс€ после правил зоны

		fStealth            =0x8000,
		fNeedToAsk          =0x10000,  //нова€ зона, нужно спросить юзера о северити
		fCreatedByUser      =0x20000  //
	};
	
	enum eZoneType
	{
		znAll             = 0,
		znTrusted         = 1,
		znNetBIOS		  = 2,
		znUntrusted		  = 3,	
	};
	
	cNetZone() :
		m_nID(0),
		m_nAdapterID(0),
		m_nMac(0),
		m_nMacLen(0),
		m_nIfType(0),
		m_nTypeOfInterface(If_other),
		m_nFlags(0),
		m_nWiFiType(0),
		m_nWiFiSecurity(0),
		m_nWiFiAuthMode(0),
		m_nGwIPv4(0),
		m_nGwMac(0),
		m_nGwMacLen(0),
		m_nDhcpIPv4(0),
		m_nDhcpMac(0),
		m_nDhcpMacLen(0),
		m_nSettings(fPermanent+fUptoGateway+fAllNetsFirst),
		//m_eType(0),
		m_nSeverity(znUntrusted),
		m_nState(0),
		m_nTimeCreated(0),
		m_nTimeConnected(0)
	{}
	void SetState(tDWORD state,tBOOL num)
	{
		if((m_nState & state)==num)
			return;
		if(state!=fTouch)
			m_nState|=fChanged;
		if(num)
			m_nState|=state;
		else
			m_nState-=m_nState & state;
	}
	tBOOL QState(tDWORD state) { return m_nState & state; }
	void SetConnected(tBOOL num) { SetState(fConnected,num); }
	tBOOL Connected() { return m_nState & fConnected; }

	DECLARE_IID( cNetZone, cSerializable, NetWatchImpl_PLUGINID, eNetZone );

	// identification info
	cStringObj  m_sFrendlyName;                  		//user frendly name of zone
	cStringObj  m_sConnectionName;        		//vpn connection name or SSID
	tDWORD      m_nID;
	tDWORD      m_nAdapterID;
	cStringObj  m_sAdapterWinName;           		//{1234-3423..
	cStringObj	m_sDnsSuffix;
	tDWORD		m_nFlags;						//interface flags dhcp?
	tQWORD      m_nMac;
	tDWORD      m_nMacLen;
	tDWORD      m_nIfType;                		//IP_ADAPTER_ADDRESSES.IfType
	tDWORD		m_nTypeOfInterface;				//eInterfaceTypes
	tDWORD      m_nWiFiType;                    //OID_802_11_INFRASTRUCTURE_MODE (adhoc? )
	tDWORD      m_nWiFiSecurity;                //OID_802_11_ENCRYPTION_STATUS (free|wpe|wpa)
	tDWORD      m_nWiFiAuthMode;				//OID_802_11_AUTHENTICATION_MODE               
	cStringObj  m_sRASPeer;						//RASENTRY.szLocalPhoneNumber	
	tDWORD		m_nTimeCreated;
	tDWORD		m_nTimeConnected;
	cStringObj  m_sConnectedProgram;
	cStringObj  m_sDefPrinter;
	cStringObj  m_sWallPaper;

	cIpMasks	m_vIPNets;

	cIpMask		m_IP;

	tIPv4       m_nGwIPv4;
	tQWORD      m_nGwMac;
	tDWORD      m_nGwMacLen;

	tIPv4       m_nDhcpIPv4;
	tQWORD      m_nDhcpMac;
	tDWORD      m_nDhcpMacLen;

	tDWORD      m_nSettings;    					//enum eSettings
	tDWORD      m_nSeverity;

	//temporary
	tDWORD      m_nState;                           //enum eNetStates
};

IMPLEMENT_SERIALIZABLE_BEGIN( cNetZone, 0 )
	SER_VALUE( m_sFrendlyName,        tid_STRING_OBJ, "FrendlyName" )
	SER_VALUE( m_sConnectionName,     tid_STRING_OBJ, "ConnectionName" )
	SER_VALUE( m_nID,	              tid_DWORD,      "ID" )
	SER_VALUE( m_nAdapterID,          tid_DWORD,      "AdapterID" )
	SER_VALUE( m_sAdapterWinName,     tid_STRING_OBJ, "AdapterName" )
	SER_VALUE( m_sDnsSuffix,		  tid_STRING_OBJ, "DnsSuffix")
	SER_VALUE( m_nFlags,              tid_DWORD,      "Flags" )
	SER_VALUE( m_nMac,	              tid_QWORD,      "Mac" )
	SER_VALUE( m_nMacLen,             tid_DWORD,      "MacLen" )
	SER_VALUE( m_nIfType,             tid_DWORD,      "IfType" )
	SER_VALUE( m_nTypeOfInterface,     tid_DWORD,      "TypeOfInterface" )
	SER_VALUE( m_nWiFiType,           tid_DWORD,      "WiFiType" )
	SER_VALUE( m_nWiFiSecurity,       tid_DWORD,      "WiFiSecurity" )
	SER_VALUE( m_nWiFiAuthMode,       tid_DWORD,      "WiFiAuthMode")
	SER_VALUE( m_sRASPeer,    		  tid_STRING_OBJ, "RASPeer")
	SER_VALUE( m_nTimeCreated,	      tid_DWORD,	  "TimeCreated" )
	SER_VALUE( m_nTimeConnected,	  tid_DWORD,      "TimeConnected" )
	SER_VALUE( m_sConnectedProgram,   tid_STRING_OBJ, "ConnectedProgram")
	SER_VALUE( m_sDefPrinter,		  tid_STRING_OBJ, "DefPrinter")
	SER_VALUE( m_sWallPaper,		  tid_STRING_OBJ, "WallPaper")
	SER_VECTOR(m_vIPNets,             cIpMask::eIID,  "IPNets")
	SER_VALUE( m_IP,                  cIpMask::eIID,  "IP")
	SER_VALUE( m_nGwIPv4,             tid_IPv4,       "GwIPv4" )
	SER_VALUE( m_nGwMac,	          tid_QWORD,      "GwMac" )
	SER_VALUE( m_nGwMacLen,           tid_DWORD,      "GwMacLen" )
	SER_VALUE( m_nDhcpIPv4,           tid_IPv4,       "DhcpIPv4" )
	SER_VALUE( m_nDhcpMac,	          tid_QWORD,      "DhcpMac" )
	SER_VALUE( m_nDhcpMacLen,         tid_DWORD,      "DhcpMacLen" )
	SER_VALUE( m_nSettings,           tid_DWORD,      "Settings" )
	SER_VALUE( m_nSeverity,            tid_DWORD,      "Severity" )

	SER_VALUE( m_nState,              tid_DWORD,      "State" )
IMPLEMENT_SERIALIZABLE_END()

typedef  cVector<cNetZone> vNetZones;


//////////////////////////////////////////////////////////////////////////
// cNWSettings

struct cNWSettings: public cSerializable
{
	enum eNWFlags
	{
		fShowAllAdapters	=1,
		fShowAllZones		=2,
		//fARPWatch			=4,
	};
	cNWSettings():
	m_NextAdapterID(1),
	m_NextZoneID(1),
	m_NewZoneLevel(10),
	m_NWFlags(7)
	{}
	DECLARE_IID_SETTINGS(cNWSettings, cSerializable, NetWatchImpl_PLUGINID);
	vNetZones				m_zones;
	cVector<cNetAdapter>	m_adapters;
	tDWORD					m_NextZoneID;
	tDWORD					m_NextAdapterID;
	tDWORD					m_NewZoneLevel;
	tDWORD					m_NWFlags;                //eNWFlags
	cVector<cARPWatch>		m_ARPWatch;
};
IMPLEMENT_SERIALIZABLE_BEGIN(cNWSettings, 0 )
	SER_VECTOR(m_zones,               cNetZone::eIID,   "Zones")
	SER_VECTOR(m_adapters,            cNetAdapter::eIID,"Adapters")
	SER_VALUE( m_NextAdapterID,       tid_DWORD,        "NextAdapterID" )
	SER_VALUE( m_NextZoneID,          tid_DWORD,        "NextZoneID" )
	SER_VALUE( m_NewZoneLevel,        tid_DWORD,        "NewZoneLevel" )
	SER_VALUE( m_NWFlags,			  tid_DWORD,        "NWFlags" )
	SER_VECTOR(m_ARPWatch,cARPWatch::eIID,"ARPWatch")
IMPLEMENT_SERIALIZABLE_END()


//////////////////////////////////////////////////////////////////////////
// cAskActionNewZone

struct cAskActionNewZone : public cTaskAskAction
{
	DECLARE_IID(cAskActionNewZone, cTaskAskAction, NetWatchImpl_PLUGINID, eAskActionNewZone);
	//cZonaAddresses   m_ZonaAddresses;
	cNetAdapter      m_NetAdapter;
	cNetZone         m_NewZone;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cAskActionNewZone, 0)
	SER_BASE(cTaskAskAction, 0)
	SER_VALUE( m_NewZone,       cNetZone::eIID,            "NewZone" )
	SER_VALUE( m_NetAdapter,    cNetAdapter::eIID,         "NetAdapter")
	//SER_VALUE( m_ZonaAddresses, cZonaAddresses::eIID,      "ZonaAddresses")
IMPLEMENT_SERIALIZABLE_END()

#endif // _netwatch_task_structs_

