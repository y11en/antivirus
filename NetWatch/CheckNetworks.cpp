#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <ws2tcpip.h>

#include "CNetWatcherTask.h"
#include "adapters.h"

bool CNetWatcherTask::CheckNetworks()
{
	m_NewListSize = 0;
    m_NewList = 0;
	DWORD entries=0;
	C_ArpTable ArpTable;
	C_GetIpForwardTable RouteTable;
	RouteTable.reload();
	if(RouteTable.error())
	{
		m_level=0;
		PR_TRACE((g_root, prtERROR, TR "CNetWatcherTask:: Error calling RouteTable."));
		return false;
	}
	m_ROUTE_Changed=RouteTable.ROUTEcmp(m_old_route_table);
	if(m_ARP_Changed==1 && m_old_arp_table && m_ArpWatchZones.size()>0 && UpdateARPWatch(m_old_arp_table))
	{		
		CopySettings();
	}
	if(m_ROUTE_Changed==0 && m_ARP_Changed==0 && m_level<2)
	{
		m_level++;
		return false;
	}
	if(m_ROUTE_Changed==1)
	{	
		if(m_old_route_table)
			FREE(m_old_route_table);
		RouteTable.dont_FREE();
		m_old_route_table=RouteTable.Get_data_pointer();
	}
	ArpTable.reload();
	if(ArpTable.error())
	{
		m_level=0;
		PR_TRACE((g_root, prtERROR, TR "CNetWatcherTask:: Error calling ArpTable."));
		return false;
	}
	m_ARP_Changed=ArpTable.ARPmemcmp(m_old_arp_table);
	if(m_ROUTE_Changed==0 && m_ARP_Changed==0 && m_level<3)
	{
		m_level++;
		return false;
	}
	if(m_ARP_Changed)
	{
		if(m_old_arp_table)
			FREE(m_old_arp_table);
		ArpTable.dont_FREE();
		m_old_arp_table=ArpTable.Get_data_pointer();
	}
	m_level=0;

	C_GetAdaptersInfo GetAdaptersInfo;
	PIP_ADAPTER_INFO pAdapterInfo=GetAdaptersInfo.Get_data_pointer();
	if(GetAdaptersInfo.error())
	{
		PR_TRACE((g_root, prtERROR, TR "CNetWatcherTask:: Error calling GetAdaptersInfo."));
		return false;
	}
	PIP_ADAPTER_ADDRESSES AdapterAddresses;
	C_GetAdaptersAddresses GetAdaptersAddresses(m_getAdaptersAddresses,AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX);
	if(m_getAdaptersAddresses)
	{
		AdapterAddresses = GetAdaptersAddresses.Get_data_pointer();
		if(GetAdaptersAddresses.error())
		{
			PR_TRACE((g_root, prtERROR, TR "CNetWatcherTask:: Error calling GetAdaptersAddresses."));
			return false;
		}
		entries=GetAdaptersAddresses.Number_of_Entries();
		m_NewListSize = sizeof(NETWATCH::Network)*entries + sizeof(int)+sizeof(MIB_IPNETROW)+sizeof(DWORD);	
		m_NewList = (NetworkList *) new unsigned char [ m_NewListSize ];
		memset(m_NewList, 0, m_NewListSize);
		Adapters_to_Networklist_XP(AdapterAddresses,pAdapterInfo,m_NewList);
	}
	if(!m_NewList)
	{
		entries=GetAdaptersInfo.Number_of_Entries();
		m_NewListSize = sizeof(NETWATCH::Network)*entries + sizeof(int)+sizeof(MIB_IPNETROW)+sizeof(DWORD);
		m_NewList = (NetworkList *) new unsigned char [ m_NewListSize ];
		memset(m_NewList, 0, m_NewListSize);
		Adapters_to_Networklist_2000(pAdapterInfo,m_NewList);
	}
	if(!m_NewList)
		return false;

	if(m_OldList && m_NewList->size==0 && m_OldList->size==0)
	{
		//no network adapters 
		delete [] (unsigned char *) m_NewList;
		return false;
	}
	if(m_NewList->size>0)
	{
		DWORD index=0;
		DWORD ip4gw=RouteTable.get_IP4_gateway(&index);
		if(ip4gw)
		{
				m_NewList->IPv4gw.dwAddr=ip4gw;
				m_NewList->IPv4gw.dwIndex=index;
		}
		ARP_table_to_Networklist(m_old_arp_table,m_NewList);
		for(int x=0;m_old_route_table && x<m_NewList->size;x++)
		{
			for(DWORD y=0;y<m_old_route_table->dwNumEntries;y++)
			{
				if(m_NewList->list[x].m_nIfIndex==m_old_route_table->table[y].dwForwardIfIndex)
					m_NewList->list[x].inRouteTable++;
			}
		}
	}
	if (m_SettingChanged || m_NewListSize != m_OldListSize || 
         memcmp(m_OldList, m_NewList, m_NewListSize ) !=0 )
    {
		m_SettingChanged=0;
		if(m_OldList)
			delete [] (unsigned char *) m_OldList;

        m_OldList     = m_NewList;
        m_OldListSize = m_NewListSize;

		PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: list changed."));
		NetworkList * list2;
		list2 = (NetworkList *) new unsigned char [ m_OldListSize ];
		memcpy (list2, m_OldList, m_OldListSize); 
		NETWATCH::Network * netw;
		m_ArpWatchZones.clear();
		m_ZonesAddresses.clear();
		for(int entry=0;entry<list2->size;entry++)
		{
			netw=&list2->list[entry];
			if((dwWindowsMajorVersion>=6 && netw->m_nIfType==IF_TYPE_IEEE80211) || 
		        (dwWindowsMajorVersion <6 && netw->m_nIfType==IF_TYPE_ETHERNET_CSMACD)
			)
			{
				wifi_adapter_query(netw);
			}
			if(netw->m_nIfType==IF_TYPE_PPP)
				ppp_adapter_query(netw);
			FindAdapterByNet(netw);
			FindZoneByNet(netw);
			AddZoneAddresses(netw,m_pAA[netw->index],m_pAI[netw->index],m_old_route_table);
			if(netw->m_nIfType!=IF_TYPE_PPP && netw->HasIPv4 && netw->ARPWatch)
				m_ArpWatchZones[netw->m_nIfIndex]=netw->zone_ID;
		}
		UnTouchZones();
		UnTouchAdapters();
		delete [] (unsigned char *) list2;
		time(&m_Time1);
		if(m_NewList->Flags==0)
		{
			CopySettings();
			return true;
		}
    }
    else
    {
		delete [] (unsigned char *) m_NewList;
    }
	return false;
}
