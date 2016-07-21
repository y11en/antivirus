#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <ws2tcpip.h>
#include "CNetWatcherTask.h"
BOOL CNetWatcherTask::UpdateARPWatch(PMIB_IPNETTABLE ARPTable)
{
	if(!ARPTable)
		return 0;
	DWORD ArpChanged=0;
	PMIB_IPNETROW table=ARPTable->table;
	DWORD dwNumEntries=ARPTable->dwNumEntries;
	DWORD ARPSize=m_ARPWatch.size();
	DWORD last_index=0;
	DWORD zone_ID=0;
	for(DWORD x=0;x<dwNumEntries;x++)
	{
		PMIB_IPNETROW IpMac=&table[x];
		if(IpMac->dwType<3)
			continue;
		if(IpMac->dwIndex!=last_index)
		{
			last_index=IpMac->dwIndex;
			zone_ID=m_ArpWatchZones[last_index];	
		}
		if(zone_ID==0)
			continue;	
		DWORD ip=IpMac->dwAddr;
		DWORD y;
		cARPWatch *row=0;
		for(y=0;y<ARPSize;y++)
		{
			row=&m_ARPWatch[y];
			if(row->m_nTimeDeleted==0 && 
				 zone_ID==row->m_nZoneID && ip==row->m_nIP4)
			{
				if(row->m_nMacLen!=min(IpMac->dwPhysAddrLen,sizeof(row->m_nMac)) ||
				memcmp(&row->m_nMac,IpMac->bPhysAddr,row->m_nMacLen)!=0)
				{
					_time32((__time32_t*)&row->m_nTimeDeleted);
				}
				break;
			}
		}
		if(y==ARPSize || row==0 || row->m_nTimeDeleted)
		{
			ArpChanged=1;
			cARPWatch new_row;
			new_row.m_nIP4=ip;
			new_row.m_nZoneID=zone_ID;
			new_row.m_nMacLen=min(IpMac->dwPhysAddrLen,sizeof(new_row.m_nMac));
			memcpy(&new_row.m_nMac,IpMac->bPhysAddr,new_row.m_nMacLen);
			_time32((__time32_t*)&new_row.m_nTimeCreated);
			if(y!=ARPSize)
			{
				//message to user. ARP changed!
				//old m_ARPWatch[y], new new_row
				m_ARPWatch.push_back(new_row);
				PR_TRACE((g_root, prtIMPORTANT, TR "CNetWatcherTask:: ARP changed"));

			}
			else
			{
				//New ARP! new_row
				m_ARPWatch.push_back(new_row);
			}
			//aSize=m_ARPWatch.size();
		}
		
	}
	return ArpChanged;
}
void CNetWatcherTask::ARP_table_to_Networklist(PMIB_IPNETTABLE ARPTable,NetworkList *newlist)
{
	if(!ARPTable || !newlist)
		return;
	int entry=0;
	DWORD gw;
	DWORD dhcp;
	DWORD ip;
	DWORD dwNumEntries=ARPTable->dwNumEntries;
	PMIB_IPNETROW table=ARPTable->table;
	for(DWORD x=0;x<dwNumEntries;x++)
	{
		if(newlist->IPv4gw.dwAddr==ntohl(table[x].dwAddr) && 
			newlist->IPv4gw.dwIndex==table[x].dwIndex)
		{
			memcpy(newlist->IPv4gw.bPhysAddr,table[x].bPhysAddr,
				min(table[x].dwPhysAddrLen, sizeof(newlist->IPv4gw.bPhysAddr)));
			break;
		}
	}
	while(entry<newlist->size)
	{
		gw=newlist->list[entry].m_nGwIPv4;
		dhcp=newlist->list[entry].m_nDhcpIPv4;
		if(gw==0 && dhcp==0)
		{	
			entry++;
			continue;
		}
		for(DWORD x=0;x<dwNumEntries;x++)
		{
			ip=ntohl(table[x].dwAddr);
			if(gw!=0 && gw==ip)
			{
				newlist->list[entry].m_nGwMacLen=min(table[x].dwPhysAddrLen,sizeof(newlist->list[entry].m_nGwMac));
				memcpy(&newlist->list[entry].m_nGwMac,table[x].bPhysAddr,newlist->list[entry].m_nGwMacLen);
				gw=0;
			}
			if(dhcp!=0 && dhcp==ip)
			{
				newlist->list[entry].m_nDhcpMacLen=min(table[x].dwPhysAddrLen, sizeof(newlist->list[entry].m_nDhcpMac));
				memcpy(&newlist->list[entry].m_nDhcpMac,table[x].bPhysAddr,newlist->list[entry].m_nDhcpMacLen);
				dhcp=0;
			}
			if(dhcp==0 && gw==0)
				break;
		}
		entry++;
	}
}
