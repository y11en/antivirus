
#undef ntohs
#undef ntohl
#undef htons
#undef htonl

#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include "CNetWatcherTask.h"


#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x)) 
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

typedef DWORD (__stdcall * TGetAdaptersAddresses)(ULONG Family, DWORD Flags, PVOID Reserved, PIP_ADAPTER_ADDRESSES pAdapterAddresses, PULONG pOutBufLen);
void NETWATCH::SetcIPBysockaddr(cIP * IP,sockaddr_in * sockaddr)
{
		if(sockaddr->sin_family==AF_INET) //AF_INET
		{
			IP->SetV4(ntohl((sockaddr->sin_addr.S_un.S_addr)));
		}
		if(sockaddr->sin_family==AF_INET6) //AF_INET6
		{
			IP->SetV6Bytes(((sockaddr_in6*)sockaddr)->sin6_addr.u.Byte);
			IP->m_V6.ntoh6();
			IP->m_V6.m_Zone=0;
		}				
}
void NETWATCH::SetcIPBystr4(cIP * IP,CHAR *str)
{
	IP->SetV4(ntohl(inet_addr(str)));
	if(IP->m_V4==0 || IP->m_V4==INADDR_NONE)
		IP->clear();
}
void CNetWatcherTask::AddZoneAddresses(NETWATCH::Network *netw,PIP_ADAPTER_ADDRESSES in_AdapterAddresses,PIP_ADAPTER_INFO in_AdapterInfo,PMIB_IPFORWARDTABLE pRouteTable)
{

	if(netw->zone_ID==0 || netw->number_of_ipv4+netw->number_of_ipv6==0)
		return;
	
	cNetZone *pZona=FindZoneByID(netw->zone_ID);
	if(!pZona)
		return;
	cIpMask *pIpMask;
	cZonaAddresses * pZonaAddresses=0;
	for(DWORD y=0;y<m_ZonesAddresses.size() && pZonaAddresses==0 ;y++)
	{
		if(m_ZonesAddresses[y].m_nZoneID==netw->zone_ID)
			pZonaAddresses=&m_ZonesAddresses.at(y);	
	}
	if(!pZonaAddresses)
		pZonaAddresses=&m_ZonesAddresses.push_back();
	pZonaAddresses->m_nZoneID=netw->zone_ID;

	if(pRouteTable)
	{
		for(DWORD y=0;y<pRouteTable->dwNumEntries;y++)
		{
			if(netw->m_nIfIndex==pRouteTable->table[y].dwForwardIfIndex && 
				pRouteTable->table[y].dwForwardDest!=0x7f && 
				pRouteTable->table[y].dwForwardDest!=0xE0 && 
				pRouteTable->table[y].dwForwardNextHop!=0x0100007f &&
				pRouteTable->table[y].dwForwardDest!=0xffffffff &&
				!(pRouteTable->table[y].dwForwardDest & 0xff000000) &&
				pRouteTable->table[y].dwForwardType!=MIB_IPROUTE_TYPE_INVALID)
			{
				
				tDWORD Metric=300;
				for(DWORD x=0;x<pRouteTable->dwNumEntries;x++)
				{
					if(x!=y && pRouteTable->table[x].dwForwardDest==pRouteTable->table[y].dwForwardDest &&
						pRouteTable->table[x].dwForwardMetric1<Metric)
						Metric=pRouteTable->table[x].dwForwardMetric1;
				}
				if(Metric>=pRouteTable->table[y].dwForwardMetric1)
				{
					if(pRouteTable->table[y].dwForwardDest==0)
						pZonaAddresses->m_IsDefaultGateway=true;
					else
					{
						pIpMask=&pZonaAddresses->m_vRoutes.push_back();
						pIpMask->SetV4(ntohl(pRouteTable->table[y].dwForwardDest));
						pIpMask->SetV4MaskByIp(ntohl(pRouteTable->table[y].dwForwardMask));
					}
					
				}
			}
		}
	}
	for(DWORD x=0;x<netw->number_of_ipv4;x++)
	{
		pIpMask=&pZonaAddresses->m_vIP.push_back();
		pIpMask->SetV4(netw->IPv4[x]);
		pIpMask->SetV4MaskByIp(netw->Maskv4[x]);
	}
	for(DWORD x=0;x<netw->number_of_ipv6;x++)
	{
		pIpMask=&pZonaAddresses->m_vIP.push_back();
		pIpMask->SetV6Bytes(netw->IPv6[x].O.Bytes);
		pIpMask->m_Mask=netw->Maskv6[x].GetMaskPrefixLength();
		pIpMask->m_V6.m_Zone=netw->Zones[netw->IPv6[x].GetScope()];

	}
	if(pZona)
	{
		if(pZona->m_nSettings & cNetZone::fUptoGateway && pZona->m_vIPNets.size()>0)
		{
			for(DWORD x=0;x<pZona->m_vIPNets.size();x++)
			{
				pZonaAddresses->m_vIP.push_back(pZona->m_vIPNets.at(x));
			}
		}
	}
	cIP IP;
	IP_ADDR_STRING *IPstr;
	if(in_AdapterAddresses)
	{
		PIP_ADAPTER_DNS_SERVER_ADDRESS DnsAddr;
		DnsAddr=in_AdapterAddresses->FirstDnsServerAddress;
		while(DnsAddr)
		{
			NETWATCH::SetcIPBysockaddr(&IP,(sockaddr_in*)DnsAddr->Address.lpSockaddr);
			pZonaAddresses->m_DNS.push_back(IP);
			DnsAddr=DnsAddr->Next;
		}
	}
	if(in_AdapterInfo)
	{
		if(in_AdapterInfo->DhcpEnabled)
		{
			NETWATCH::SetcIPBystr4(&IP,in_AdapterInfo->DhcpServer.IpAddress.String);
			if(IP.IsValid())
				pZonaAddresses->m_DHCP.push_back(IP);
		}
		if(in_AdapterInfo->HaveWins)
		{
			NETWATCH::SetcIPBystr4(&IP,in_AdapterInfo->PrimaryWinsServer.IpAddress.String);
			if(IP.IsValid())
				pZonaAddresses->m_WINS.push_back(IP);
			NETWATCH::SetcIPBystr4(&IP,in_AdapterInfo->SecondaryWinsServer.IpAddress.String);
			if(IP.IsValid())
				pZonaAddresses->m_WINS.push_back(IP);
		}
		IPstr=&in_AdapterInfo->GatewayList;
		while(IPstr)
		{
			NETWATCH::SetcIPBystr4(&IP,IPstr->IpAddress.String);
			if(IP.IsValid())
				pZonaAddresses->m_GW.push_back(IP);
			IPstr=IPstr->Next;
		}		
	}
	/*
	for(tDWORD x=0;x<m_ZonesAddresses.size();x++)
	{
		pZona=FindZoneByID(m_ZonesAddresses.at(x).m_nZoneID);
		pZona->m_vIPNets=m_ZonesAddresses.at(x).m_vIP;
	}
	*/
	// need m_HttpProxy,m_DC.....
}

tERROR  CNetWatcherTask::GetNWdata(cSerializable* data) 
{	
	tERROR err=errNOT_FOUND;
	if(QState()!=TASK_REQUEST_RUN)
		return err;
	Lock_NWdata();
	if(m_NWdata.m_ZonesAddresses.size()>0)
	{
		((cNWdata*)data)->m_ZonesAddresses.assign(m_NWdata.m_ZonesAddresses);
		err=errOK;
	}
	Unlock_NWdata();
	return err;
};

void CNetWatcherTask::CopySettings()
{
	Lock_settings();
	if(m_settings.m_zones!=m_zones || m_settings.m_adapters!=m_adapters || m_settings.m_ARPWatch!=m_ARPWatch)
	{
		m_settings.m_zones.assign(m_zones);
		m_settings.m_adapters.assign(m_adapters);
		m_settings.m_ARPWatch.assign(m_ARPWatch);
		tERROR err=m_task->sysSendMsg(pmc_TASK_SETTINGS_CHANGED, 0, NULL, NULL, 0);
//		RulesApply->UpdatePacketRules(&m_settings,m_ZonesAddresses);
	}
	Unlock_settings();
	Lock_NWdata();
	m_NWdata.m_ZonesAddresses.assign(m_ZonesAddresses);
	Unlock_NWdata();
}
tERROR CNetWatcherTask::SetSettings(const cSerializable* p_settings)
{
	cNWSettings *settings =(cNWSettings *)p_settings;
	Lock();
	m_zones.assign(settings->m_zones);
	m_adapters.assign(settings->m_adapters);
	m_ARPWatch.assign(settings->m_ARPWatch);
	m_SettingChanged=1;
	if(m_zones.size()==0 || m_zones[0].m_nID!=0)
	{
		cNetZone *pZona=&m_zones.insert(0);
		pZona->m_sFrendlyName="Internet";
		pZona->m_sConnectionName="Internet";
		pZona->SetState(fVisible,TRUE);
		pZona->m_nSeverity=cNetZone::znUntrusted;
	}
	if(m_adapters.size()==0 || m_adapters[0].m_nID!=0)
	{
		cNetAdapter *pAdapter=&m_adapters.insert(0);
		pAdapter->m_sAdapterFrendlyName="Other networks";
		pAdapter->SetState(fVisible,TRUE);
	}
	Unlock();
	Lock_settings();
	m_settings.assign(*settings,false);
	Unlock_settings();
	return errOK;
}
tERROR CNetWatcherTask::GetSettings(const cSerializable* p_settings)
{	
	cNWSettings *settings =(cNWSettings *)p_settings;
	Lock_settings();
	settings->assign(m_settings,false);
	Unlock_settings();
	return errOK;
}
tERROR CNetWatcherTask::SetTaskState(tTaskRequestState p_state)
{
	PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: request to change state: incoming value 0x%x", p_state));
	if(QState()==p_state) 
		return warnTASK_ALREADY_IN_STATE ;
	if(p_state!=TASK_REQUEST_RUN && p_state!=TASK_REQUEST_PAUSE && p_state!=TASK_REQUEST_STOP)
	{
		PR_TRACE((g_root, prtERROR, TR "CNetWatcherTask:: Unknown state 0x%x", p_state));
		return errPARAMETER_INVALID;
	}
	SaveState(p_state);
	if(p_state==TASK_REQUEST_RUN && QStateOld()!=TASK_REQUEST_PAUSE)
	{
		DisconnectAll();
		DestroyThread ();
		if(CreateThread ()!=errOK)
			return errTASK_CANNOT_GO_TO_STATE;
	}	
	return warnTASK_STATE_CHANGED;
}
void CNetWatcherTask::PreClose()
{
	DestroyThread ();
	if(m_OldList)
		delete [] (unsigned char * ) m_OldList;
	if(m_old_arp_table)
		FREE(m_old_arp_table);
	if(m_old_route_table)
		FREE(m_old_route_table);
	m_old_route_table=0;
	m_old_arp_table=0;
	//m_OldRouteTable2=0;
	m_OldList=0;
    DeleteCriticalSection(&m_cs);
	DeleteCriticalSection(&m_cs_NWdata);
	DeleteCriticalSection(&m_cs_settings);
}
CNetWatcherTask::CNetWatcherTask ()
{
	m_OldList=0;
	m_OldListSize=0;
	m_threadstarted=false;
	m_thread=0;
	m_level=0;
	m_SettingChanged=0;
	m_zones_changed=0;
	m_adapters_changed=0;
	m_Rules_changed=0;
	m_Time1=0;
	m_old_arp_table=0;
	m_old_route_table=0;
	//m_OldRouteTable2=0;
	m_ARP_Changed=0;
	m_ROUTE_Changed=0;
	m_callback=0;
	m_getAdaptersAddresses=0;
	m_GetIpForwardTable2=0;
	HMODULE hIPLHLPAPI = GetModuleHandle("iphlpapi.dll");

    DWORD dwVersion = GetVersion();

    dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
    dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));

    OSVERSIONINFOEX osvix;
    osvix.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx ((OSVERSIONINFO*)&osvix);

    if (dwWindowsMajorVersion == 5 && dwWindowsMinorVersion == 1 && osvix.wServicePackMajor == 2 || // XP SP2
        dwWindowsMajorVersion == 5 && dwWindowsMinorVersion >= 2 || // XP x64
        dwWindowsMajorVersion >= 6) //Vista+
    {
        m_getAdaptersAddresses = (TGetAdaptersAddresses)GetProcAddress(hIPLHLPAPI, "GetAdaptersAddresses");
		if(!m_getAdaptersAddresses)
		{
			PR_TRACE((g_root, prtERROR, TR "CNetWatcherTask:: GetAdaptersAddresses not found in IPLHLPAPI!"));
		}
    }
	if(dwWindowsMajorVersion >= 6)
	{
		m_GetIpForwardTable2=(TGetIpForwardTable2)GetProcAddress(hIPLHLPAPI, "GetIpForwardTable2");
		if(!m_GetIpForwardTable2)
		{
			PR_TRACE((g_root, prtERROR, TR "CNetWatcherTask:: GetIpForwardTable2 not found in IPLHLPAPI!"));
		}
	}

    InitializeCriticalSection(&m_cs);
	InitializeCriticalSection(&m_cs_settings);
	InitializeCriticalSection(&m_cs_NWdata);

	//log.WriteFormat (_T("[CNetWatcherTask] CNetWatcherTask created"), PEL_INFO);
}

CNetWatcherTask::~CNetWatcherTask ()
{
 
}



void inline CNetWatcherTask::Lock ()
{
    EnterCriticalSection(&m_cs);
}

void inline CNetWatcherTask::Unlock ()
{
    LeaveCriticalSection(&m_cs);
}
void inline CNetWatcherTask::Lock_settings ()
{
    EnterCriticalSection(&m_cs_settings);
}
void inline CNetWatcherTask::Unlock_settings ()
{
    LeaveCriticalSection(&m_cs_settings);
}
void inline CNetWatcherTask::Lock_NWdata ()
{
	EnterCriticalSection(&m_cs_NWdata);
}
void inline CNetWatcherTask::Unlock_NWdata ()
{
	LeaveCriticalSection(&m_cs_NWdata);
}

tERROR CNetWatcherTask::CreateThread ()
{
	if(m_threadstarted)
		return errOK;
	
	PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: Starting thread."));

    DWORD threadid;
    m_event = ::CreateEvent(0, FALSE, FALSE, 0);
    m_thread = ::CreateThread (0, 0, ThreadProc, this, 0, &threadid);
	if(m_thread)
	{
		m_threadstarted = true;
		return errOK;
	}
	else
	{
		::CloseHandle(m_event);
		PR_TRACE((g_root, prtERROR, TR "CNetWatcherTask:: Cant create thread."));
		return errUNEXPECTED;
	}
}


void CNetWatcherTask::DestroyThread ()
{
    if (m_threadstarted)
    {
		PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: Destroying thread."));
		::SetEvent(m_event);

        ::WaitForSingleObject(m_thread, INFINITE);

        ::CloseHandle(m_thread);
        m_thread = 0;

        ::CloseHandle(m_event);
        m_event  = 0;
		m_threadstarted = false;
    }   
}


DWORD WINAPI CNetWatcherTask::ThreadProc ( LPVOID parm )
{
	PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: Thread started."));
    CNetWatcherTask * pthis = (CNetWatcherTask *) parm;
	DWORD res;
	NETWATCH::NETWORKCHANGEPROC callback;
	void *                    aux;
    do
    {
		if(pthis->QState()==TASK_REQUEST_STOP)
			break;
        res = ::WaitForSingleObject(pthis->m_event,2000);
		if(res != WAIT_TIMEOUT)
			break;
		if(pthis->QState()==TASK_REQUEST_RUN)
		{
			pthis->Lock ();
			if (pthis->CheckNetworks () && pthis->m_callback)
			{
				callback = pthis->m_callback;
				aux      = pthis->m_aux;

				pthis->Unlock ();
				callback ( aux );
			}
			else
				pthis->Unlock ();
		}
    }
    while (1);
	PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: Thread exited."));
    return 0;
}
CNetWatcherTaskState::CNetWatcherTaskState()
{
	m_state=TASK_REQUEST_STOP;
	m_state_old=TASK_REQUEST_STOP;
	InitializeCriticalSection(&m_cs_state);
}
CNetWatcherTaskState::~CNetWatcherTaskState()
{
	DeleteCriticalSection(&m_cs_state);
}
void CNetWatcherTaskState::SaveState(DWORD state)
{
	EnterCriticalSection(&m_cs_state);
	m_state_old=m_state;
	m_state=state;
	LeaveCriticalSection(&m_cs_state);
}
DWORD CNetWatcherTaskState::QStateOld()
{
	EnterCriticalSection(&m_cs_state);
	DWORD state=m_state_old;
	LeaveCriticalSection(&m_cs_state);
	return state;
}
DWORD CNetWatcherTaskState::QState()
{
	EnterCriticalSection(&m_cs_state);
	DWORD state=m_state;
	LeaveCriticalSection(&m_cs_state);
	return state;
}
tBOOL CNetWatcherTask::IsLocalArea(const cIpMask *IP)
{
	typedef struct
	{
		tIPv4  nIp;
		tDWORD nPrefixLength;
	} tSubnetAddr;

	tSubnetAddr g_aLocaArea[] = 
	{
		{0x00000000,  8},
		{0x7f000000,  8},
		{0xc0000200, 24},
		{0x0a000000,  8},
		{0xac100000, 12},
		{0xc0a80000, 16},
		{0xa9fe0000, 16},
	};
    if(IP->IsV4())
    {
	    for(tDWORD i = 0; i < countof(g_aLocaArea); i++)
        {
            if( (IP->m_V4 & CKAHUM::IPv4Mask(g_aLocaArea[i].nPrefixLength)) == g_aLocaArea[i].nIp &&
				g_aLocaArea[i].nPrefixLength <= IP->m_Mask )
			    return cTRUE;
        }
    }
	return cFALSE;
}

/*
bool CNetWatcherTask::InitStatus ()
{
    if (m_statusinited) 
    {
        return true;
    }

    if (CheckNetworks ()) 
    {
	    //log.WriteFormat (_T("[CNetWatcherTask] InitStatus, GetNewStatus succeeded"), PEL_INFO);
        m_statusinited = true;
    }
    else if (CheckNetworks ())  // one more time, network status might be in the process of changing
    {
	    //log.WriteFormat (_T("[CNetWatcherTask] InitStatus, GetNewStatus succeeded in second time"), PEL_INFO);
        m_statusinited = true; 
    }
    else
    {
	    //log.WriteFormat (_T("[CNetWatcherTask] InitStatus, GetNewStatus failed twice"), PEL_INFO);
        return false;
    }

    return true;
}
DWORD CNetWatcherTask::ARP_protection(DWORD *x)
{
	Lock ();
	DWORD arp=m_arp_spoof_protection;
	if(x)
		m_arp_spoof_protection=*x;
	Unlock ();
		return arp;
}
NetworkList * CNetWatcherTask::GetNetworkList ()
{
	//log.WriteFormat (_T("[CNetWatcherTask] GetNetworkList"), PEL_INFO);

    NetworkList * list = 0;

    Lock ();

    if ( ! InitStatus () )
    {
        Unlock ();

	    //log.WriteFormat (_T("[CNetWatcherTask] GetNetworkList failed - InitStatus failed"), PEL_INFO);
        return 0;
    }

    if ( ! m_threadstarted )
    {
        CheckNetworks ();
    }

    if ( m_OldListSize == 0 )
    {
        Unlock ();

	    //log.WriteFormat (_T("[CNetWatcherTask] GetNetworkList failed - m_listsize == 0"), PEL_INFO);
        return 0;
    }

    if ( ! ( list = (NetworkList *) new unsigned char [ m_OldListSize ] ) )
    {
        Unlock ();
		
	    //log.WriteFormat (_T("[CNetWatcherTask] GetNetworkList failed - no memory"), PEL_INFO);
        return 0;
    }

    memcpy (list, m_OldList, m_OldListSize); 
	list->FlagsARP=m_FlagsARP;
	m_FlagsARP=0;
    Unlock ();

	//log.WriteFormat (_T("[CNetWatcherTask] GetNetworkList succeeded"), PEL_INFO);
    return list;
}

void CNetWatcherTask::DeleteNetworkList ( NetworkList * list )
{
    delete [] (unsigned char *) list;
}

void CNetWatcherTask::SetCallback ( NETWATCH::NETWORKCHANGEPROC proc, void *aux )
{
	//log.WriteFormat (_T("[CNetWatcherTask] SetCallback begin"), PEL_INFO);

    DestroyThread ();

    if (proc)
    {
        InitStatus ();

        m_callback = proc;
        m_aux = aux;

        CreateThread ();
    }
	//log.WriteFormat (_T("[CNetWatcherTask] SetCallback end"), PEL_INFO);
}


CNetWatcherTask g_NetWatcher;
DWORD NETWATCH::ARP_protection(DWORD *x)
{
	return g_NetWatcher.ARP_protection(x);
}
NETWATCH::OpResult NETWATCH::SetNetworkChangeCallback ( NETWATCH::NETWORKCHANGEPROC NetworkChangeCallback, LPVOID lpContext )
{
    g_NetWatcher.SetCallback ( NetworkChangeCallback, lpContext );
	return NETWATCH::srOK;
}

NETWATCH::OpResult NETWATCH::GetNetworkList ( NETWATCH::HNETWORKLIST * phlist )
{
    if ( ! phlist )
    {
        return NETWATCH::srInvalidArg;
    }

    if ( ! ( (*phlist) = g_NetWatcher.GetNetworkList () ) )
    {
        return NETWATCH::srOpFailed;
    }

	return NETWATCH::srOK;
}

NETWATCH::OpResult NETWATCH::NetworkList_GetSize ( NETWATCH::HNETWORKLIST hlist, int * psize )
{
    if ( ! hlist || ! psize )
    {
        return NETWATCH::srInvalidArg;
    }

    *psize = ((NetworkList*)hlist)->size;
    return NETWATCH::srOK;
}


NETWATCH::OpResult NETWATCH::NetworkList_GetItem ( NETWATCH::HNETWORKLIST hlist, int n, NETWATCH::Network * pnet )
{
    if ( ! hlist || n >= ((NetworkList*)hlist)->size || ! pnet )
    {
        return NETWATCH::srInvalidArg;
    }

    *pnet = ((NetworkList*)hlist)->list[n];
    return NETWATCH::srOK;
}
NETWATCH::OpResult NETWATCH::NetworkList_GetGW ( NETWATCH::HNETWORKLIST hlist, PMIB_IPNETROW  pnet )
{
    
    *pnet = ((NetworkList*)hlist)->IPv4gw;
    return NETWATCH::srOK;
}
NETWATCH::OpResult NETWATCH::GetArpStatus(NETWATCH::HNETWORKLIST hlist,DWORD *flag)
{
	*flag=((NetworkList*)hlist)->FlagsARP;
	 return NETWATCH::srOK;
}
NETWATCH::OpResult NETWATCH::NetworkList_Delete ( NETWATCH::HNETWORKLIST hlist )
{
    g_NetWatcher.DeleteNetworkList((NetworkList*)hlist);
	return NETWATCH::srOK;
}
*/

