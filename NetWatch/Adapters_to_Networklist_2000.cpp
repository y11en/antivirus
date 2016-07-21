#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <ws2tcpip.h>
#include "ntddndis.h"   
#include "CNetWatcherTask.h"
#include "IoctlNdisQuery.h"
void CNetWatcherTask::Adapters_to_Networklist_2000(PIP_ADAPTER_INFO in_AdapterInfo,NetworkList * newlist)
{
	if(!in_AdapterInfo || !newlist)
		return;
	int entry = 0;
	int index_ipv4;
	PIP_ADAPTER_INFO pAdapterInfo=in_AdapterInfo;
	while(pAdapterInfo)
	{
		/*
		if (pAdapterInfo->Type==MIB_IF_TYPE_LOOPBACK)
		{
			pAdapterInfo=pAdapterInfo->Next;
			continue;
		}
		*/
		index_ipv4=0;
		MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pAdapterInfo->Description,
			sizeof(pAdapterInfo->Description),newlist->list[entry].AdapterDesc,NETWATCH::ADAPTERDESCLEN);
	//	strncpy(newlist->list[entry].AdapterDesc, pAdapterInfo->Description, NETWATCH::ADAPTERDESCLEN);
        newlist->list[entry].AdapterDesc[NETWATCH::ADAPTERDESCLEN - 1] = 0;
		strncpy_s(newlist->list[entry].m_sAdapterWinName,pAdapterInfo->AdapterName, NETWATCH::m_sAdapterWinNameLEN);
        newlist->list[entry].m_sAdapterWinName[NETWATCH::m_sAdapterWinNameLEN - 1] = 0;
		switch(pAdapterInfo->Type)
		{
			case MIB_IF_TYPE_ETHERNET	: newlist->list[entry].m_nIfType=IF_TYPE_ETHERNET_CSMACD;
			case MIB_IF_TYPE_PPP		: newlist->list[entry].m_nIfType=IF_TYPE_PPP;
			case MIB_IF_TYPE_LOOPBACK	: newlist->list[entry].m_nIfType=IF_TYPE_SOFTWARE_LOOPBACK;
		}
		PR_TRACE((g_root, prtIMPORTANT,  "CNetWatcherTask::Adapters_to_Networklist_2000 found adapter %s",pAdapterInfo->Description));

		newlist->list[entry].m_nIfIndex=pAdapterInfo->Index;

//!!		newlist->list[entry].OperStatus=Adapter->OperStatus;
		if(query_dword_oid(newlist->list[entry].m_sAdapterWinName,OID_GEN_MEDIA_CONNECT_STATUS)==NdisMediaStateConnected)
			newlist->list[entry].OperStatus=IfOperStatusUp;
		else
			newlist->list[entry].OperStatus=IfOperStatusDown;

		memcpy(&newlist->list[entry].m_nMac, pAdapterInfo->Address, min(pAdapterInfo->AddressLength, sizeof(newlist->list[entry].m_nMac)));
		
		newlist->list[entry].m_nGwIPv4=ntohl(inet_addr(pAdapterInfo->GatewayList.IpAddress.String));
		if(newlist->list[entry].m_nGwIPv4==INADDR_NONE)
			newlist->list[entry].m_nGwIPv4=0;
		if(pAdapterInfo->DhcpEnabled==1)
		{
			newlist->list[entry].m_nFlags=IP_ADAPTER_DHCP_ENABLED;
			newlist->list[entry].m_nDhcpIPv4=ntohl(inet_addr(pAdapterInfo->DhcpServer.IpAddress.String));
			if(newlist->list[entry].m_nDhcpIPv4==INADDR_NONE)
				newlist->list[entry].m_nDhcpIPv4=0;
		}
		PIP_ADDR_STRING ipaddr=&pAdapterInfo->IpAddressList;
		while(ipaddr && index_ipv4<NETWATCH::MAX_IP_PER_ADAPTER)
		{
			newlist->list[entry].IPv4[index_ipv4]=ntohl(inet_addr(ipaddr->IpAddress.String));
			newlist->list[entry].Maskv4[index_ipv4]=ntohl(inet_addr(ipaddr->IpMask.String));
			newlist->list[entry].HasIPv4 = TRUE;
			index_ipv4++;
			ipaddr=ipaddr->Next;
		}
		entry++;
		pAdapterInfo=pAdapterInfo->Next;
	}
	newlist->size = entry;
}