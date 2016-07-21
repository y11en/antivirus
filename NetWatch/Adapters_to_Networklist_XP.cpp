#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <ws2tcpip.h>
#include "CNetWatcherTask.h"

void  CNetWatcherTask::Adapters_to_Networklist_XP(PIP_ADAPTER_ADDRESSES in_AdapterAddresses,PIP_ADAPTER_INFO in_AdapterInfo,NetworkList * newlist)
{
	if(!in_AdapterAddresses || !in_AdapterInfo || !newlist)
		return;
	int index_ipv4;
	int index_ipv6;
    int entry = 0;
	PIP_ADAPTER_INFO pAdapterInfo=in_AdapterInfo;
	PIP_ADAPTER_ADDRESSES Adapter=in_AdapterAddresses;
	while (Adapter && entry!=MAX_ADAPTERS) 
    {
		//if(Adapter->IfType==IF_TYPE_SOFTWARE_LOOPBACK || Adapter->IfType==IF_TYPE_TUNNEL )
		if(Adapter->IfType==IF_TYPE_TUNNEL )
		{	
			//IF_TYPE_SOFTWARE_LOOPBACK
			Adapter=Adapter->Next;
			continue;
		}
		index_ipv4=0;
		index_ipv6=0;


        wcsncpy_s(newlist->list[entry].AdapterFrendlyName, Adapter->FriendlyName, NETWATCH::ADAPTERNAMELEN);
        newlist->list[entry].AdapterFrendlyName[NETWATCH::ADAPTERNAMELEN - 1] = 0;

        wcsncpy_s(newlist->list[entry].AdapterDesc, Adapter->Description, NETWATCH::ADAPTERDESCLEN);
        newlist->list[entry].AdapterDesc[NETWATCH::ADAPTERDESCLEN - 1] = 0;

        strncpy_s(newlist->list[entry].m_sAdapterWinName,Adapter->AdapterName, NETWATCH::m_sAdapterWinNameLEN);
        newlist->list[entry].m_sAdapterWinName[NETWATCH::m_sAdapterWinNameLEN - 1] = 0;

		wcsncpy_s(newlist->list[entry].DnsSuffix,Adapter->DnsSuffix,NETWATCH::DnsSuffixLen);
		newlist->list[entry].DnsSuffix[NETWATCH::DnsSuffixLen-1]=0;

		newlist->list[entry].m_nFlags=Adapter->Flags;
		newlist->list[entry].m_nIfType=Adapter->IfType;
		newlist->list[entry].m_nIfIndex=Adapter->IfIndex;
		newlist->list[entry].Ipv6IfIndex=Adapter->Ipv6IfIndex;
		newlist->list[entry].OperStatus=Adapter->OperStatus;
		newlist->list[entry].m_nMacLen=min(Adapter->PhysicalAddressLength,sizeof(newlist->list[entry].m_nMac));
        memcpy(&newlist->list[entry].m_nMac, Adapter->PhysicalAddress, newlist->list[entry].m_nMacLen);
        memcpy(newlist->list[entry].Zones, Adapter->ZoneIndices, sizeof(Adapter->ZoneIndices));
		
		PR_TRACE((g_root, prtIMPORTANT, "CNetWatcherTask::Adapters_to_Networklist_XP found adapter %S,%S,%d",Adapter->FriendlyName,Adapter->DnsSuffix,Adapter->IfType));

		if(Adapter->PhysicalAddressLength>sizeof(newlist->list[entry].m_nMac))
		{
			PR_TRACE((g_root, prtERROR, TR "CNetWatcherTask:: MAC address of %S too large.\n",Adapter->Description));
		}
		DWORD IPv4 = 0;
        DWORD Maskv4 = 0;

        bool  HasIPv6 = false;
        CKAHUM::IPv6 IPv6;   IPv6.SetZero();
        CKAHUM::IPv6 Maskv6; Maskv6.SetZero();

        IP_ADAPTER_UNICAST_ADDRESS* UnicastAddress = Adapter->FirstUnicastAddress;
        while (UnicastAddress)
        {
			//if(!(UnicastAddress->Flags & IP_ADAPTER_ADDRESS_DNS_ELIGIBLE))
				
            if (UnicastAddress->Address.lpSockaddr->sa_family == AF_INET &&
				index_ipv4<NETWATCH::MAX_IP_PER_ADAPTER)
            {  
                DWORD ipv4 = ntohl(((sockaddr_in*)UnicastAddress->Address.lpSockaddr)->sin_addr.S_un.S_addr);

                DWORD maxmaskv4 = CKAHUM::IPv4Mask(32);
                DWORD maxlength = 32;

                IP_ADAPTER_PREFIX* Prefix = Adapter->FirstPrefix;
                while (Prefix)
                {
                    if (Prefix->Address.lpSockaddr->sa_family == AF_INET &&
						Prefix->PrefixLength > 0 
						&& Prefix->PrefixLength < maxlength )
                    {
                        DWORD netv4 = ntohl(((sockaddr_in*)Prefix->Address.lpSockaddr)->sin_addr.S_un.S_addr);
                        DWORD maskv4 = CKAHUM::IPv4Mask( Prefix->PrefixLength );
                        if ((netv4 & maskv4) == (ipv4 & maskv4))
                        {
							maxmaskv4 = maskv4;
							maxlength = Prefix->PrefixLength;				
                        }
                    }

                    Prefix = Prefix->Next;
                }
				if(UnicastAddress->PrefixOrigin==IpSuffixOriginDhcp || 
					(UnicastAddress->PrefixOrigin==IpSuffixOriginManual && newlist->list[entry].m_nIP4Net==0))
				{
					newlist->list[entry].m_nIP4Net=ipv4;
					newlist->list[entry].m_nIP4NetMask=maxmaskv4;
				}
				newlist->list[entry].HasIPv4 = TRUE;
				newlist->list[entry].IPv4[index_ipv4] = ipv4;
				newlist->list[entry].Maskv4[index_ipv4] = maxmaskv4;
				newlist->list[entry].PrefixOrigin_v4[index_ipv4] = UnicastAddress->PrefixOrigin;
				index_ipv4++;
			
            }
			else if (UnicastAddress->Address.lpSockaddr->sa_family == AF_INET6 
				&& index_ipv6<NETWATCH::MAX_IP_PER_ADAPTER)
            {
                CKAHUM::IPv6 ipv6;
                ipv6.Set( ((CKAHUM::OWord*)((sockaddr_in6*)UnicastAddress->Address.lpSockaddr)->sin6_addr.u.Byte)->ntoh(),
                          ((sockaddr_in6*)UnicastAddress->Address.lpSockaddr)->sin6_scope_id);

                CKAHUM::IPv6::Scope scope = ipv6.GetScope();
				CKAHUM::IPv6 maxmaskv6;
				maxmaskv6.SetMask(128, 0);
                if (ipv6.GetScope() == CKAHUM::IPv6::ScopeGlobal)
                {		
                    DWORD maxlength = 128;
                    IP_ADAPTER_PREFIX* Prefix = Adapter->FirstPrefix;
                    while (Prefix)
                    {
                        if (Prefix->Address.lpSockaddr->sa_family == AF_INET6 &&
							Prefix->PrefixLength > 0 &&
                            Prefix->PrefixLength < maxlength)
                        {
                            CKAHUM::IPv6 netv6;
                            CKAHUM::IPv6 maskv6;

                            netv6.Set( ((CKAHUM::OWord*)((sockaddr_in6*)Prefix->Address.lpSockaddr)->sin6_addr.u.Byte)->ntoh(),
                                       ((sockaddr_in6*)Prefix->Address.lpSockaddr)->sin6_scope_id );
                            maskv6.SetMask(Prefix->PrefixLength, 0);

                            if ((netv6 & maskv6) == (ipv6 & maskv6))
                            {
                                maxmaskv6= maskv6;
                                maxlength = Prefix->PrefixLength;
                            }
                        }

                        Prefix = Prefix->Next;
                    }
				}
				if(ipv6.IsValid())
				{
					if(UnicastAddress->PrefixOrigin==IpSuffixOriginDhcp || 
						(UnicastAddress->PrefixOrigin==IpSuffixOriginManual && newlist->list[entry].m_nIP4Net==0))
					{
						newlist->list[entry].m_nIP6NetHi=ipv6.O.Hi;
					}
					newlist->list[entry].HasIPv6 = TRUE;
					newlist->list[entry].IPv6[index_ipv6] = ipv6;
					newlist->list[entry].Maskv6[index_ipv6] = maxmaskv6;
					newlist->list[entry].PrefixOrigin_v6[index_ipv6] = UnicastAddress->PrefixOrigin;
					index_ipv6++;
				}  
            }
            UnicastAddress = UnicastAddress->Next;
        }
		newlist->list[entry].number_of_ipv4=index_ipv4;
		newlist->list[entry].number_of_ipv6=index_ipv6;
		m_pAA[entry]=Adapter;
		m_pAI[entry]=0;
		newlist->list[entry].index=entry;
		Adapter=Adapter->Next;	
        entry++;
    }
	newlist->size = entry;
	
	DWORD ip_address_GW; 
	DWORD ip_address_DHCP;

	while(pAdapterInfo)
	{
		for(int x=0;x<entry;x++)
		{
			if(strcmp(pAdapterInfo->AdapterName,newlist->list[x].m_sAdapterWinName)==0)
			{ 
				m_pAI[x]=pAdapterInfo;
				ip_address_GW=ntohl(inet_addr(pAdapterInfo->GatewayList.IpAddress.String));

				if(ip_address_GW!=INADDR_NONE && ip_address_GW!=0)
					newlist->list[x].m_nGwIPv4=ip_address_GW;
				else
					ip_address_GW=0;
				
				if(newlist->list[x].m_nFlags && IP_ADAPTER_DHCP_ENABLED && pAdapterInfo->DhcpEnabled)
				{
					ip_address_DHCP=ntohl(inet_addr(pAdapterInfo->DhcpServer.IpAddress.String));
					if(ip_address_DHCP!=INADDR_NONE && ip_address_DHCP!=0)
						newlist->list[x].m_nDhcpIPv4=ip_address_DHCP;
					else
						newlist->list[x].OperStatus=IfOperStatusDown;

				}
				else
					ip_address_DHCP=0;
				break;
			}
		}
		//if(ip_address_DHCP==0 && newlist->list[x].m_nFlags && IP_ADAPTER_DHCP_ENABLED)
		//	newlist->list[x].OperStatus=IfOperStatusDown;
		pAdapterInfo=pAdapterInfo->Next;
	}
	
	return ;
}