#if !defined(_CNetWatcherTask_H__INCLUDED_)
#define _CNetWatcherTask_H__INCLUDED_
#define _netwatcher_project_
#include <winsock2.h>
#include <WS2TCPIP.H>
#include <windows.h>
#include <map>
#include <NetWatch/structs/s_netwatch.h>
#include <Iphlpapi.h>
//#include <Netioapi.h>
#include <CKAH/CKAHUM.h>
#include <CKAH/CKAHSTAT.h>
using namespace CKAHFW;
#include <CKAH/CKAHPorts.h>
#include <CKAH/ckahrules.h>
typedef unsigned __int64   tQWORD;
#include <ckah\ckahip.h>
namespace NETWATCH
{
	void SetcIPBysockaddr(cIP * ,sockaddr_in * );
	void SetcIPBystr4(cIP * ,CHAR *);

    const int ADAPTERNAMELEN = 256;
    const int ADAPTERDESCLEN = 256;
  //  const int PHYSLEN = 8;
	const int m_sAdapterWinNameLEN = 256;
	const int MAX_IP_PER_ADAPTER=16;
	const int DnsSuffixLen=100;
    struct Network
    {
		DWORD			number_of_ipv4;
		DWORD			number_of_ipv6;
        BOOL            HasIPv4;                            // наличие IPv4
        DWORD           IPv4[MAX_IP_PER_ADAPTER];                               // IPv4-адрес сети
        DWORD           Maskv4[MAX_IP_PER_ADAPTER];			// IPv4-маска сети
		DWORD			PrefixOrigin_v4[MAX_IP_PER_ADAPTER];
        BOOL            HasIPv6;                            // наличие IPv6
        CKAHUM::IPv6    IPv6[MAX_IP_PER_ADAPTER];                               // IPv6-адрес сети
        CKAHUM::IPv6    Maskv6[MAX_IP_PER_ADAPTER];                             // IPv6-маска сети
        DWORD			PrefixOrigin_v6[MAX_IP_PER_ADAPTER];
		DWORD           Zones[CKAHUM::IPv6::MaxScopes];     // IPv6 zone indices для всех scopes
        WCHAR           AdapterFrendlyName[ADAPTERNAMELEN];        // Имя адаптера
		WCHAR           AdapterDesc[ADAPTERDESCLEN];        // Описание адаптера
		WCHAR			DnsSuffix[DnsSuffixLen];		

		tQWORD			m_nMac;
		DWORD			m_nMacLen;                      // MAC-адрес адаптера
				
		DWORD			m_nIfIndex;
		DWORD			Ipv6IfIndex;
		DWORD			OperStatus; 
		DWORD			m_nFlags;								//adapter settings mask(DHCP,IPV4,IPV6,...)
		DWORD			m_nIfType;								//interface type (Ethrnet,PPP,TUNNEL,...)
		CHAR			m_sAdapterWinName[m_sAdapterWinNameLEN];  //AdapterName ( "{12345-12345...}"
		
		tIPv4			m_nIP4Net;
		tIPv4			m_nIP4NetMask;

		tQWORD			m_nIP6NetHi;

		tIPv4			m_nGwIPv4;
		tQWORD			m_nGwMac;
		tDWORD			m_nGwMacLen;
		//BOOL			DefGW;
		DWORD			inRouteTable;

		tIPv4			m_nDhcpIPv4;
		tQWORD			m_nDhcpMac;
		tDWORD			m_nDhcpMacLen;

		WCHAR			m_sRASPeer[RAS_MaxPhoneNumber + 1];	//RASENTRY.szLocalPhoneNumber
		WCHAR			m_sRasDeviceType[RAS_MaxDeviceType+1];
		WCHAR			m_sRasDeviceName[RAS_MaxDeviceName+1];
		
		DWORD			m_nWiFiType;                    //OID_802_11_INFRASTRUCTURE_MODE (adhoc? )
		DWORD			m_nWiFiSecurity;                //OID_802_11_ENCRYPTION_STATUS (free|wpe|wpa)
		DWORD			m_nWiFiAuthMode;				//OID_802_11_AUTHENTICATION_MODE               

		CHAR			SSID[33];
		ULONG			SsidLength;
		DWORD			index;
		DWORD			ARPWatch;
		DWORD			adapter_ID;
		DWORD			zone_ID;
		DWORD			Changed;


    };

    typedef LPVOID HNETWORKLIST;

	// callback-функция уведомления об изменении конфигурации сети
	typedef VOID (CALLBACK *NETWORKCHANGEPROC) (LPVOID lpContext);

    // конфигурация сети


    // установить callback для уведомления об изменении конфигурации сети
//	NETWATCH::OpResult SetNetworkChangeCallback ( NETWORKCHANGEPROC NetworkChangeCallback, LPVOID lpContext );

    // получить список сетей
//	NETWATCH::OpResult GetNetworkList ( HNETWORKLIST * phlist );

    // получить размер списка сетей
//   NETWATCH::OpResult NetworkList_GetSize ( HNETWORKLIST hlist, int * psize );

    // получить элемент списока сетей
//    NETWATCH::OpResult NetworkList_GetItem ( HNETWORKLIST hlist, int n, Network * pnet );

	//получить default gateway
//	NETWATCH::OpResult NetworkList_GetGW ( HNETWORKLIST hlist, PMIB_IPNETROW  pnet );

    // удалить список сетей
 //   NETWATCH::OpResult NetworkList_Delete ( HNETWORKLIST hlist );
	
	//получить статус изменения ARP таблицы
//	NETWATCH::OpResult GetArpStatus(HNETWORKLIST hlist,DWORD *flag);

//	DWORD  ARP_protection(DWORD *);

};



struct NetworkList
{
	DWORD				Flags;
    int                 size;
	MIB_IPNETROW		IPv4gw;
#pragma warning(disable:4200)
    NETWATCH::Network     list[];
};

typedef DWORD (__stdcall * TGetAdaptersAddresses)(ULONG Family, DWORD Flags, PVOID Reserved, PIP_ADAPTER_ADDRESSES  pAdapterAddresses, PULONG pOutBufLen);
typedef DWORD (__stdcall * TGetIpForwardTable2)(ULONG  Family,PVOID Table);

class CNetWatcherTaskState
{
public:
	CNetWatcherTaskState();
	~CNetWatcherTaskState();
	void SaveState(DWORD);
	DWORD QState();
	DWORD QStateOld();
private:
	DWORD	m_state;
	DWORD	m_state_old;
	CRITICAL_SECTION m_cs_state;
};


class CNetWatcherTask :public CNetWatcherTaskState  //:private cNWSettings
{
public:
	iObj * m_task;
//	cRulesApply * RulesApply;
	CNetWatcherTask ();
    ~CNetWatcherTask ();
	tERROR SetSettings(const cSerializable*);
	tERROR GetSettings(const cSerializable*);
	tERROR GetNWdata(cSerializable *);
	tERROR SetTaskState(tTaskRequestState p_state);
	void  PreClose();
private:
	const static int MAX_ADAPTERS=30;
	PIP_ADAPTER_ADDRESSES		m_pAA[MAX_ADAPTERS];
	PIP_ADAPTER_INFO			m_pAI[MAX_ADAPTERS];
	DWORD						m_level;
	cNWSettings					m_settings;
	DWORD						m_NWFlags;
	DWORD						m_NewZoneLevel;
	DWORD						m_SettingChanged;
	cVector<cNetZone>			m_zones;
	cVector<cNetAdapter>		m_adapters;
	cVector<cARPWatch>			m_ARPWatch;				
	BOOL						m_zones_changed;
	BOOL						m_adapters_changed;
	BOOL						m_Rules_changed;
	vZonesAddresses				m_ZonesAddresses;
	cNWdata						m_NWdata;

	std::map<DWORD,DWORD>		m_ArpWatchZones;
	DWORD						dwWindowsMajorVersion;
	DWORD						dwWindowsMinorVersion;

	NetworkList *               m_OldList;
	int							m_OldListSize;
	NetworkList *				m_NewList;
	int							m_NewListSize;


	//PMIB_IPNETTABLE				m_new_arp_table;
	PMIB_IPNETTABLE				m_old_arp_table;
	PMIB_IPFORWARDTABLE			m_old_route_table;
	//PMIB_IPFORWARD_TABLE2		m_OldRouteTable2;
	BOOL						m_ARP_Changed;
	BOOL						m_ROUTE_Changed;
	time_t						m_Time1;			//when networklist changed;

    HANDLE                      m_thread;
	DWORD						m_threadid;
    HANDLE                      m_event;
    CRITICAL_SECTION            m_cs;
	CRITICAL_SECTION            m_cs_settings;
	CRITICAL_SECTION			m_cs_NWdata;
    NETWATCH::NETWORKCHANGEPROC   m_callback;
    void *                      m_aux;

	TGetAdaptersAddresses		m_getAdaptersAddresses;
	TGetIpForwardTable2			m_GetIpForwardTable2;

  //  bool                        m_statusinited;
    bool                        m_threadstarted;




    void Lock ();
    void Unlock ();

	void Lock_settings ();
    void Unlock_settings ();
	void Lock_NWdata ();
    void Unlock_NWdata ();
	void CopySettings();

    tERROR CreateThread ();
    void DestroyThread ();

    //bool InitStatus ();
	bool CheckNetworks ();

    NetworkList * GetNetworkList ();
	DWORD ARP_protection(DWORD *);
    void DeleteNetworkList ( NetworkList * list );

    void SetCallback ( NETWATCH::NETWORKCHANGEPROC proc, void *aux );
    

    static DWORD WINAPI ThreadProc ( LPVOID parm );


	void Adapters_to_Networklist_XP(PIP_ADAPTER_ADDRESSES,PIP_ADAPTER_INFO,NetworkList *);
    void Adapters_to_Networklist_2000(PIP_ADAPTER_INFO,NetworkList *);
	void ARP_table_to_Networklist(PMIB_IPNETTABLE,NetworkList *);
	void Route_table_to_Networklist(PMIB_IPFORWARDTABLE,NetworkList *);
	BOOL UpdateARPWatch(PMIB_IPNETTABLE);
	void wifi_adapter_query(NETWATCH::Network *);
	void query_adapter_description(NETWATCH::Network *);
	void ppp_adapter_query(NETWATCH::Network *);

	DWORD FindZoneByNet(NETWATCH::Network *);
	cNetZone * FindZoneByID(DWORD); 
	void  ConnectZone(cNetZone *,NETWATCH::Network *);
	void  DisconnectZone(cNetZone *);
	cNetZone * NewZoneByNet(NETWATCH::Network *);
	void  UnTouchZones();
	void  DisconnectAll();

	cNetAdapter *FindAdapterByNet(NETWATCH::Network *);
	cNetAdapter *FindAdapterByID(DWORD);
	void UnTouchAdapters();
	void AddZoneAddresses(NETWATCH::Network *,PIP_ADAPTER_ADDRESSES,PIP_ADAPTER_INFO,PMIB_IPFORWARDTABLE);
	//void SetFirewallRulesByZones();
	tBOOL IsLocalArea(const cIpMask *);

};

#endif
