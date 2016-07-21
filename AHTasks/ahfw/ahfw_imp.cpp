#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>

#include "ahfw_imp.h"
#include <CKAH/ipconv.h>
#include <ProductCore/iptoa.h>

void CALLBACK ahfwTrace(CKAHUM::LogLevel Level, LPCSTR szString)
{
	switch (Level)
	{
	case CKAHUM::lError:
		PR_TRACE((g_root, prtDANGER, "AH\t%s", szString));
		break;
	case CKAHUM::lWarning:
		PR_TRACE((g_root, prtERROR, "AH\t%s", szString));
		break;
	default:
		PR_TRACE((g_root, prtIMPORTANT, "AH\t%s", szString));
	}
}

void FWGetUnique(cCriticalSection* pSync, tQWORD* pCurrentVal, tQWORD* pUnique)
{
	cAutoCS autoCS(pSync, true);
	*pUnique = *pCurrentVal;
	*pCurrentVal++;
}

tDWORD GetSessionIDForProcess(ULONG ProcessId)
{
	DWORD dwSessionId = (DWORD) -1;
	if (gfnProcessToSession && gfnProcessToSession(ProcessId, &dwSessionId))
		return (tDWORD) dwSessionId;

	return (tDWORD) -1;
}
#include <iptypes.h>
#include <stdio.h>

static tDWORD Str2Ipv4(char *sIp)
{
	tDWORD nIp[4] = {0,};
	sscanf(sIp, "%u.%u.%u.%u", &nIp[3], &nIp[2], &nIp[1], &nIp[0]);
	tDWORD Ip;
	tBYTE *pIp = (tBYTE *)&Ip;
	pIp[0] = nIp[0];
	pIp[1] = nIp[1];
	pIp[2] = nIp[2];
	pIp[3] = nIp[3];

	return Ip;
}

static tERROR AddIPv4String(cVector<cIP> &aHosts, PIP_ADDR_STRING pIpv4AddrString)
{
	for(; pIpv4AddrString; pIpv4AddrString = pIpv4AddrString->Next)
	{
		cIP ip;
        ip.SetV4(Str2Ipv4(pIpv4AddrString->IpAddress.String));

		tDWORD i;
		for(i = 0; (i < aHosts.size()) && (aHosts[i] != ip); i++);
		if( i == aHosts.size() )
			aHosts.push_back(ip);
	}
	return errOK;
}

static tERROR AddSockaddr(cVector<cIP> &aHosts, SOCKET_ADDRESS& socket_address)
{
    cIP ip;
    CKAHUM::IPv6 ckahumipv6;
    switch (socket_address.lpSockaddr->sa_family)
    {
    case AF_INET:
        ip.SetV4(ntohl(((sockaddr_in*)socket_address.lpSockaddr)->sin_addr.S_un.S_addr));
        break;
    case AF_INET6:
        ckahumipv6.Set(((CKAHUM::OWord*)((sockaddr_in6*)socket_address.lpSockaddr)->sin6_addr.u.Byte)->ntoh(),
                       ((sockaddr_in6*)socket_address.lpSockaddr)->sin6_scope_id );
        ip.SetV6(CKAHUMIPv6_cIPv6(ckahumipv6));
        break;
    }

    if (ip.IsValid())
    {
		tDWORD i;
		for(i = 0; (i < aHosts.size()) && (aHosts[i] != ip); i++);
		if( i == aHosts.size() )
			aHosts.push_back(ip);
    }
    return errOK;
}

// the followings structures are ripped from WDK (with slight modifiactions):
// ===========================================================================
            typedef struct _IP_ADAPTER_WINS_SERVER_ADDRESS_LH {
                union {
                    ULONGLONG Alignment;
                    struct {
                        ULONG Length;
                        DWORD Reserved;
                    };
                };
                struct _IP_ADAPTER_WINS_SERVER_ADDRESS_LH *Next;
                SOCKET_ADDRESS Address;
            } IP_ADAPTER_WINS_SERVER_ADDRESS_LH, *PIP_ADAPTER_WINS_SERVER_ADDRESS_LH;

            typedef struct _IP_ADAPTER_GATEWAY_ADDRESS_LH {
                union {
                    ULONGLONG Alignment;
                    struct {
                        ULONG Length;
                        DWORD Reserved;
                    };
                };
                struct _IP_ADAPTER_GATEWAY_ADDRESS_LH *Next;
                SOCKET_ADDRESS Address;
            } IP_ADAPTER_GATEWAY_ADDRESS_LH, *PIP_ADAPTER_GATEWAY_ADDRESS_LH;

            #pragma warning(push)
            #pragma warning(disable:4214) // bit field types other than int
            typedef union _NET_LUID_LH
            {
                ULONG64     Value;
                struct
                {
                    ULONG64     Reserved:24;
                    ULONG64     NetLuidIndex:24;
                    ULONG64     IfType:16;                  // equal to IANA IF type
                }Info;
            } NET_LUID_LH, *PNET_LUID_LH;
            #pragma warning(pop)
            typedef NET_LUID_LH IF_LUID;

            typedef UINT32 NET_IF_COMPARTMENT_ID, *PNET_IF_COMPARTMENT_ID;
            typedef GUID NET_IF_NETWORK_GUID, *PNET_IF_NETWORK_GUID;

            typedef enum _NET_IF_CONNECTION_TYPE
            {
               NET_IF_CONNECTION_DEDICATED = 1,
               NET_IF_CONNECTION_PASSIVE = 2,
               NET_IF_CONNECTION_DEMAND = 3,
               NET_IF_CONNECTION_MAXIMUM = 4
            } NET_IF_CONNECTION_TYPE, *PNET_IF_CONNECTION_TYPE;

            typedef enum {
                TUNNEL_TYPE_NONE = 0,
                TUNNEL_TYPE_OTHER = 1,
                TUNNEL_TYPE_DIRECT = 2,
                TUNNEL_TYPE_6TO4 = 11,
                TUNNEL_TYPE_ISATAP = 13,
                TUNNEL_TYPE_TEREDO = 14,
            } TUNNEL_TYPE, *PTUNNEL_TYPE;

            #define MAX_DHCPV6_DUID_LENGTH          130 // RFC 3315.

            typedef struct _IP_ADAPTER_ADDRESSES_LH {
                union {
                    ULONGLONG Alignment;
                    struct {
                        ULONG Length;
                        DWORD IfIndex;
                    };
                };
                struct _IP_ADAPTER_ADDRESSES_LH *Next;
                PCHAR AdapterName;
                PIP_ADAPTER_UNICAST_ADDRESS FirstUnicastAddress;
                PIP_ADAPTER_ANYCAST_ADDRESS FirstAnycastAddress;
                PIP_ADAPTER_MULTICAST_ADDRESS FirstMulticastAddress;
                PIP_ADAPTER_DNS_SERVER_ADDRESS FirstDnsServerAddress;
                PWCHAR DnsSuffix;
                PWCHAR Description;
                PWCHAR FriendlyName;
                BYTE PhysicalAddress[MAX_ADAPTER_ADDRESS_LENGTH];
                ULONG PhysicalAddressLength;
                union {
                    ULONG Flags;
                    struct {
                        ULONG DdnsEnabled : 1;
                        ULONG RegisterAdapterSuffix : 1;
                        ULONG Dhcpv4Enabled : 1;
                        ULONG ReceiveOnly : 1;
                        ULONG NoMulticast : 1;
                        ULONG Ipv6OtherStatefulConfig : 1;
                        ULONG NetbiosOverTcpipEnabled : 1;
                        ULONG Ipv4Enabled : 1;
                        ULONG Ipv6Enabled : 1;
                        ULONG Ipv6ManagedAddressConfigurationSupported : 1;
                    };
                };
                ULONG Mtu;
                DWORD IfType;
                IF_OPER_STATUS OperStatus;
                DWORD Ipv6IfIndex;
                ULONG ZoneIndices[16];
                PIP_ADAPTER_PREFIX FirstPrefix;

                ULONG64 TransmitLinkSpeed;
                ULONG64 ReceiveLinkSpeed;
                PIP_ADAPTER_WINS_SERVER_ADDRESS_LH FirstWinsServerAddress;
                PIP_ADAPTER_GATEWAY_ADDRESS_LH FirstGatewayAddress;
                ULONG Ipv4Metric;
                ULONG Ipv6Metric;
                IF_LUID Luid;
                SOCKET_ADDRESS Dhcpv4Server;
                NET_IF_COMPARTMENT_ID CompartmentId;
                NET_IF_NETWORK_GUID NetworkGuid;
                NET_IF_CONNECTION_TYPE ConnectionType;    
                TUNNEL_TYPE TunnelType;
                //
                // DHCP v6 Info.
                //
                SOCKET_ADDRESS Dhcpv6Server;
                BYTE Dhcpv6ClientDuid[MAX_DHCPV6_DUID_LENGTH];
                ULONG Dhcpv6ClientDuidLength;
                ULONG Dhcpv6Iaid;
            } IP_ADAPTER_ADDRESSES_LH, 
             *PIP_ADAPTER_ADDRESSES_LH;
// ===========================================================================

tERROR GetNetCfg(cVector<cIP> *pDhcp, cVector<cIP> *pDns, cVector<cIP> *pWins)
{
    HMODULE hIPLHLPAPI = 0;

    typedef DWORD (__stdcall * TGetAdaptersAddresses)(ULONG Family, DWORD Flags, PVOID Reserved, PIP_ADAPTER_ADDRESSES pAdapterAddresses, PULONG pOutBufLen);
    TGetAdaptersAddresses pGetAdaptersAddresses = 0;

    DWORD dwVersion = GetVersion();
    DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
    DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));

    if(dwVersion < 0x80000000 && dwWindowsMajorVersion >= 5 && dwWindowsMinorVersion >= 1) // this function is used on XP\Vista+
    {
	    HMODULE hIPLHLPAPI = GetModuleHandle("iphlpapi.dll");
        pGetAdaptersAddresses = (TGetAdaptersAddresses)GetProcAddress(hIPLHLPAPI, "GetAdaptersAddresses");
    }

	if( pDhcp || pWins )
	{

		if( !(dwVersion & 0x80000000) )
		{
			DWORD dwRetAddr, ulOutBufLen = 0;
			PIP_ADAPTER_INFO pAdapterInfo = NULL;
			if( (dwRetAddr = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == ERROR_BUFFER_OVERFLOW )
				pAdapterInfo = (PIP_ADAPTER_INFO)malloc(ulOutBufLen);
				
			if( dwRetAddr == ERROR_SUCCESS || GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_SUCCESS)
			{
				for(PIP_ADAPTER_INFO pAdapter = pAdapterInfo; pAdapter; pAdapter = pAdapter->Next)
				{
					if( pDhcp && pAdapter->DhcpEnabled )
					{
						AddIPv4String(*pDhcp, &pAdapter->DhcpServer);
					}    
					if( pWins && pAdapter->HaveWins )
					{
						AddIPv4String(*pWins, &pAdapter->PrimaryWinsServer);
						AddIPv4String(*pWins, &pAdapter->SecondaryWinsServer);
					}
				}
			}
			if( pAdapterInfo )
				free(pAdapterInfo);
		}
	}

	if( pDns )
	{
		DWORD nSize = 0, nRet = 0;
		FIXED_INFO *pInfo = NULL;
		if( (nRet = GetNetworkParams(pInfo, &nSize)) == ERROR_BUFFER_OVERFLOW )
			pInfo = (FIXED_INFO *)malloc(nSize);
		if( nRet == ERROR_SUCCESS || GetNetworkParams(pInfo, &nSize) == ERROR_SUCCESS )
			AddIPv4String(*pDns, &pInfo->DnsServerList);
		if( pInfo )
			free(pInfo);
	}


    if (pGetAdaptersAddresses)
    {
        #define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x)) 
        #define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

        PIP_ADAPTER_ADDRESSES AdapterAddresses = NULL;
        ULONG OutBufferLength = 0;
        ULONG RetVal = 0, i;    

        for (i = 0; i < 5; i++) 
        {
            RetVal = pGetAdaptersAddresses(AF_INET6, 0, NULL, AdapterAddresses, &OutBufferLength);
            
            if (RetVal != ERROR_BUFFER_OVERFLOW) 
                break;

            if (AdapterAddresses != NULL) { FREE(AdapterAddresses); }
            
            AdapterAddresses = (PIP_ADAPTER_ADDRESSES) MALLOC(OutBufferLength);
            if (AdapterAddresses == NULL)
            {
                RetVal = GetLastError();
                break;
            }
        }
        
        if (RetVal == NO_ERROR) 
        {
            for (PIP_ADAPTER_ADDRESSES Adapter = AdapterAddresses; Adapter; Adapter = Adapter->Next)
            {
                if (pDns)
                {
                    for (PIP_ADAPTER_DNS_SERVER_ADDRESS DnsAddress = Adapter->FirstDnsServerAddress;
                         DnsAddress; DnsAddress = DnsAddress->Next)
                        AddSockaddr(*pDns, DnsAddress->Address);
                }

                if (Adapter->Length >= sizeof (IP_ADAPTER_ADDRESSES_LH))
                {
                    PIP_ADAPTER_ADDRESSES_LH AdapterLH =  (PIP_ADAPTER_ADDRESSES_LH)Adapter;

                    if (pWins)
                    {
                        for (PIP_ADAPTER_WINS_SERVER_ADDRESS_LH WinsAddress = AdapterLH->FirstWinsServerAddress;
                             WinsAddress; WinsAddress = WinsAddress->Next)
                            AddSockaddr(*pWins, WinsAddress->Address);
                    }

                    /*
                    if (pDhcp)
                    {
                        AddSockaddr(*pDhcp, AdapterLH->Dhcpv6Server);
                    }
                    */
                }
            }

        }
        if (AdapterAddresses != NULL) 
        {
            FREE(AdapterAddresses);
        }
    }

	return errOK;
}

// service func
//+ ------------------------------------------------------------------------------------------
CKAHUM::OpResult GenerateAddr(cFWAddress* pSingleAddress, HRULEADDRESSES hRuleAddresses, cIPRESOLVER* pIPResolver)
{
	CKAHUM::OpResult opres = CKAHUM::srOK;

	RuleAddressType adrrtype;

	cVector<cIP> AddressIP;
	cVector<cIP> *pAddressIP = &pSingleAddress->m_AddressIP;

	if (!pSingleAddress->m_Address.empty())
		adrrtype = ratHost;
	else
	{
		if (pSingleAddress->m_IPMask.IsValid())
			adrrtype = ratSubnet;
		else if (pSingleAddress->m_IPLo == pSingleAddress->m_IPHi)
        {
            AddressIP.push_back(pSingleAddress->m_IPLo);
            pAddressIP = &AddressIP;
            adrrtype = ratHost;
        }
        else
			adrrtype = ratRange;
	}

	_eFWWellKnownAddress eAddr = pSingleAddress->RecognizeAddress();
	if( eAddr != _fwaUnknown )
	{
		adrrtype = ratHost;
		pAddressIP = &AddressIP;

		switch(eAddr)
		{
		case _fwaDHCP: GetNetCfg(&AddressIP, NULL, NULL); break;
		case _fwaDNS:  GetNetCfg(NULL, &AddressIP, NULL); break;
		case _fwaWINS: GetNetCfg(NULL, NULL, &AddressIP); break;
		}
		if( AddressIP.empty() )
			return CKAHUM::srOpFailed;
	}
	
	HRULEADDRESS hRuleAddress = RuleAddress_Create(adrrtype);
	if (!hRuleAddress)
		return CKAHUM::srOpFailed;

	switch(adrrtype)
	{
	case ratRange:
	    opres = RuleAddress_SetAddressRange(hRuleAddress, &cIP_CKAHUMIP(pSingleAddress->m_IPLo), &cIP_CKAHUMIP(pSingleAddress->m_IPHi));
		break;
	case ratSubnet:
		opres = RuleAddress_SetAddressSubnet(hRuleAddress, &cIP_CKAHUMIP(pSingleAddress->m_IPLo), &cIP_CKAHUMIP(pSingleAddress->m_IPMask));
		break;
	case ratHost:
		for (tDWORD cou = 0; cou < pAddressIP->size(); cou++)
			RuleAddress_AddHostIP(hRuleAddress, &cIP_CKAHUMIP(pAddressIP->at(cou)));
		break;
	}
	
	opres = RuleAddress_SetName(hRuleAddress, pSingleAddress->m_AddrName.data());
	
	opres = RuleAddresses_AddItemToBack(hRuleAddresses, hRuleAddress);

	opres = RuleAddress_Delete(hRuleAddress);

	return opres;
}

CKAHUM::OpResult GenerateTimeRange(cFwBaseRule * pRule, HRULETIMES* phRuleTimes)
{
	CKAHUM::OpResult opres = CKAHUM::srOK;
	
	*phRuleTimes = RuleTimes_Create();
	if(*phRuleTimes)
	{
		HRULETIME hRuleTime = RuleTime_Create();
		if(hRuleTime)
		{
			HRULETIMEELEMENT hRuleTimeElement = RuleTimeElement_Create();
			if(hRuleTimeElement)
			{
				if(opres == CKAHUM::srOK)
				{
					opres = RuleTimeElement_SetType(hRuleTimeElement, rtetDaySecond);
				}
				if(opres == CKAHUM::srOK)
				{
					RuleTimeElement_SetRange(hRuleTimeElement, pRule->m_nTimeFrom, pRule->m_nTimeTill);
				}
				if(opres == CKAHUM::srOK)
				{
					opres = RuleTime_AddItemToBack(hRuleTime, hRuleTimeElement);
				}
				RuleTimeElement_Delete(hRuleTimeElement);
			}
			else
				opres = CKAHUM::srOpFailed;
			
			if(opres == CKAHUM::srOK)
			{
				opres = RuleTimes_AddItemToBack(*phRuleTimes, hRuleTime);
			}
			
			RuleTime_Delete(hRuleTime);
		}
		else
			opres = CKAHUM::srOpFailed;
	}
	else
		opres = CKAHUM::srOpFailed;

	if(opres != CKAHUM::srOK && *phRuleTimes)
	{
		RuleTimes_Delete(*phRuleTimes);
		*phRuleTimes = NULL;
	}
	return opres;
}

CKAHUM::OpResult GenerateAddrList(cVector<cFWAddress>* pList, HRULEADDRESSES* phRuleAddresses, cIPRESOLVER* pIPResolver)
{
	CKAHUM::OpResult opres = CKAHUM::srOK;
	
	int cou_addr_all = pList->count();
	if (!cou_addr_all)
	{
		*phRuleAddresses = 0;
		return CKAHUM::srOK;
	}

	*phRuleAddresses = RuleAddresses_Create();
	if (!*phRuleAddresses)
		return CKAHUM::srOpFailed;
		
	for (int cou_addr = 0; cou_addr < cou_addr_all && CKAHUM::srOK == opres; cou_addr++)
	{
		cFWAddress& SingleAddress = pList->at(cou_addr);
		if (CKAHUM::srOK == opres)
			opres = GenerateAddr(&SingleAddress, *phRuleAddresses, pIPResolver);
	}
	
	if (CKAHUM::srOK == opres)
		return CKAHUM::srOK;
	
	RuleAddresses_Delete(*phRuleAddresses);
	*phRuleAddresses = 0;

	return CKAHUM::srOpFailed;
}


CKAHUM::OpResult GeneratePort(cFWPort* pSinglePort, HRULEPORTS hRulePorts)
{
	CKAHUM::OpResult opres = CKAHUM::srOK;
	RulePortType PortType;
	
	if (pSinglePort->m_PortHi == pSinglePort->m_PortLo)
		PortType = rptSingle;
	else
		PortType = rptRange;

	HRULEPORT hRulePort = RulePort_Create(PortType);
	if (!hRulePort)
		return CKAHUM::srOpFailed;

	if (rptSingle == PortType)
		opres = RulePort_SetSinglePort(hRulePort, pSinglePort->m_PortLo);
	else
		opres = RulePort_SetPortRange(hRulePort, pSinglePort->m_PortLo, pSinglePort->m_PortHi);

	opres = RulePorts_AddItemToBack(hRulePorts, hRulePort);
	opres = RulePort_Delete(hRulePort);
	
	return opres;
}

CKAHUM::OpResult GeneratePortList(cVector<cFWPort>* pList, HRULEPORTS* phRulePorts)
{
	CKAHUM::OpResult opres = CKAHUM::srOK;

	int cou_port_all = pList->count();	
	if (!cou_port_all)
	{
		*phRulePorts = 0;
		return CKAHUM::srOK;
	}
	
	*phRulePorts = RulePorts_Create();
	if (!*phRulePorts)
		return CKAHUM::srOpFailed;

	for (int cou_port = 0; cou_port < cou_port_all && CKAHUM::srOK == opres; cou_port++)
	{
		cFWPort& SinglePort = pList->at(cou_port);
		if (CKAHUM::srOK == opres)
			opres = GeneratePort(&SinglePort, *phRulePorts);
	}
	
	if (CKAHUM::srOK == opres)
		return CKAHUM::srOK;

	RulePorts_Delete(*phRulePorts);
	*phRulePorts = 0;

	return CKAHUM::srOpFailed;
}

CKAHUM::OpResult GenerateElement(cFwBaseRule * pSingleEl, HAPPRULEELEMENT hRuleElement, cIPRESOLVER* pIPResolver)
{
	CKAHUM::OpResult opres;
	
	opres = ApplicationRuleElement_SetIsActive(hRuleElement, !!pSingleEl->m_bEnabled);

	if (CKAHUM::srOK == opres)
		opres =	ApplicationRuleElement_SetProtocol(hRuleElement, pSingleEl->m_nProto);
	
	if(CKAHUM::srOK == opres)
	{
		PacketDirection pd;
		StreamDirection sd;
		switch(pSingleEl->m_eDirection)
		{
		case _fwInbound:         pd = pdIncoming; sd = sdBoth;     break;
		case _fwOutbound:        pd = pdOutgoing; sd = sdBoth;     break;
		case _fwInboundOutbound: pd = pdBoth;     sd = sdBoth;     break;
		case _fwInboundStream:   pd = pdBoth;     sd = sdIncoming; break;
		case _fwOutboundStream:  pd = pdBoth;     sd = sdOutgoing; break;
		default:
			opres = CKAHUM::srOpFailed;
		}
		if(opres == CKAHUM::srOK)
			opres = ApplicationRuleElement_SetPacketDirection(hRuleElement, pd);
		
		if(opres == CKAHUM::srOK)
			opres = ApplicationRuleElement_SetStreamDirection(hRuleElement, sd);
	}

	HRULEPORTS hRulePorts;
	
	if (CKAHUM::srOK == opres)
		opres = GeneratePortList(&pSingleEl->m_aLocalPorts, &hRulePorts);

	if (CKAHUM::srOK == opres)
	{
		if (hRulePorts)
		{
			opres = ApplicationRuleElement_SetLocalPorts(hRuleElement, hRulePorts);
			RulePorts_Delete(hRulePorts);
			hRulePorts = 0;
		}
	}
	
	if (CKAHUM::srOK == opres)
		opres = GeneratePortList(&pSingleEl->m_aRemotePorts, &hRulePorts);
	
	if (CKAHUM::srOK == opres)
	{
		if (hRulePorts)
		{
			opres = ApplicationRuleElement_SetRemotePorts(hRuleElement, hRulePorts);
			RulePorts_Delete(hRulePorts);
			hRulePorts = 0;
		}
	}
	
	if (CKAHUM::srOK == opres)
	{
		HRULEADDRESSES hRuleAddresses;

		opres = GenerateAddrList(&pSingleEl->m_aRemoteAddresses, &hRuleAddresses, pIPResolver);
		if (CKAHUM::srOK == opres)
		{
			if (hRuleAddresses)
			{
				opres = ApplicationRuleElement_SetRemoteAddresses(hRuleElement, hRuleAddresses);
				RuleAddresses_Delete(hRuleAddresses);
				hRuleAddresses = 0;
			}
		}
	}

	return opres;
}

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

tBOOL IsLocalArea(const cIP& Ip, const cIP& Mask)
{
    if( Mask.IsV4())
    {
        if( Mask.m_V4 == tIPv4Mask(32) )
		    return cFALSE;

	    tDWORD nPrefixLength = Mask.GetMaskPrefixLength();

	    for(tDWORD i = 0; i < countof(g_aLocaArea); i++)
        {
            if( (Ip.m_V4 & CKAHUM::IPv4Mask(g_aLocaArea[i].nPrefixLength)) == g_aLocaArea[i].nIp &&
			    g_aLocaArea[i].nPrefixLength <= nPrefixLength )
			    return cTRUE;
        }
    }
	return cFALSE;
}

tUINT AHFWNetworks_FindNetwork(cAHFWNetworks &aNetworks, cAHFWNetwork &aNet)
{
    tUINT i;
    for (i = 0; i < aNetworks.size(); ++i)
    {
        if (aNetworks[i].IsTypeEqual(aNet) && aNetworks[i].IsMacEqual(aNet) )
            break;
    }
    return i;
}

tUINT AHFWNetworks_FindDependsOn(cAHFWNetworks &aNetworks, cAHFWNetwork &aNet)
{
    // for each not user-defined and independent network
    tINT level = 0;
    tUINT dependson = aNetworks.size();
    if (aNet.IsDependent())
    {
        for (tUINT i = 0; i < aNetworks.size(); i++)
        {
            cAHFWNetwork &Net = aNetworks[i];
            if (!Net.IsUserDefined()&&
                Net.IsMacEqual(aNet) &&
                Net.GetDependencyLevel() > level)
            {
                level = Net.GetDependencyLevel();
                dependson = i;
            }
        }
    }
    return dependson;
}

//
tBOOL AHFWNetworks_GetEffectiveNetwork(cAHFWNetworks &aNetworks, tUINT aIndex, cAHFWNetwork& aEffectiveNet)
{
    if (aIndex >= 0 && aIndex < aNetworks.size())
    {
        aEffectiveNet = aNetworks[aIndex];
        if (!aEffectiveNet.IsShown())
        {
            // for each shown network
	        for(tUINT i = 0; i < aNetworks.size(); i++)
	        {
		        cAHFWNetwork &Net = aNetworks[i];
                if (Net.IsShown() && 
                    Net.IsIPGreaterEqual(aEffectiveNet))
                {
                    AHFWNetworks_CopyNetworkSettings(aEffectiveNet, Net);
                    break;
                }
            }
        }
        else if (aEffectiveNet.IsDependent())
        {
            tUINT dependson = AHFWNetworks_FindDependsOn(aNetworks, aEffectiveNet);
            if (dependson != aNetworks.size())
            {
                AHFWNetworks_CopyNetworkSettings(aEffectiveNet, aNetworks[dependson]);
            }
        }
        return cTRUE;
    }
    else
        return cFALSE;
}


void AHFWNetworks_HideNetworks(cAHFWNetworks& aNetworks, cAHFWNetwork &aNewNet, tUINT aSkipIndex)
{
    for(tUINT i = 0; i < aNetworks.size(); i++)
    {
	    cAHFWNetwork &Net = aNetworks[i];
        if (i != aSkipIndex &&
            ( aNewNet.IsUserDefined() ? aNewNet.IsIPGreaterEqual(Net) :
                                        aNewNet.IsIPGreater(Net) ))
        {
            if (Net.IsUserDefined())
            {
                aNetworks.remove(i--); // delete, if user defined
                PR_TRACE((g_root, prtIMPORTANT, "FW\tAddNetwork: deleting user defined network. mode:%s, subnet: %s/%s, mac:%I64x, zone: %d, adapter: %S", Net.m_bHidden?"hidden":"shown", cIPToA(Net.m_IP), cIPMaskToA(Net.m_Mask), Net.m_nMac, Net.m_eZone, Net.m_sAdapterDesc.data()));
            }
            else if (Net.IsShown())
            {
                Net.m_bHidden = cTRUE; // hide, if detected
                AHFWNetworks_CopyNetworkSettings(Net, aNewNet);
                PR_TRACE((g_root, prtIMPORTANT, "FW\tAddNetwork: hiding network. mode:%s, subnet: %s/%s, mac:%I64x, zone: %d, adapter: %S", Net.m_bHidden?"hidden":"shown", cIPToA(Net.m_IP), cIPMaskToA(Net.m_Mask), Net.m_nMac, Net.m_eZone, Net.m_sAdapterDesc.data()));

                // change all dependent as well, if any
                AHFWNetworks_ChangeDependentNetworks(aNetworks, Net);
            }
            else
            {
                // copy setting to already hidden
                AHFWNetworks_CopyNetworkSettings(Net, aNewNet);
                PR_TRACE((g_root, prtIMPORTANT, "FW\tAddNetwork: copying settings to hidden network. mode:%s, subnet: %s/%s, mac:%I64x, zone: %d, adapter: %S", Net.m_bHidden?"hidden":"shown", cIPToA(Net.m_IP), cIPMaskToA(Net.m_Mask), Net.m_nMac, Net.m_eZone, Net.m_sAdapterDesc.data()));

                // change all dependent as well, if any
                AHFWNetworks_ChangeDependentNetworks(aNetworks, Net);
            }
        }
    }
}

// AddNetwork
// return value:
// cTRUE - added network is shown
// cFALSE - added network is not shown
tBOOL AHFWNetworks_AddNetwork(cAHFWNetworks &aNetworks, cAHFWNetwork &aNewNet, tUINT aInsertIndex)
{
    bool addhidden = false;
    tUINT greater = 0;
    bool adddependent = false;
    tUINT dependson = 0;

    if (!aNewNet.IsUserDefined())
    {
        // for each shown network
	    for(tUINT i = 0; i < aNetworks.size(); i++)
	    {
		    cAHFWNetwork &Net = aNetworks[i];
            if (Net.IsShown() && 
                Net.IsIPGreaterEqual(aNewNet))
            {
                addhidden = true;
                greater = i;
                break;
            }
        }

        if (!addhidden && 
            aNewNet.IsDependent())
        {
            // find a network whom this one depends on
            dependson = AHFWNetworks_FindDependsOn(aNetworks, aNewNet);
            if (dependson != aNetworks.size())
                adddependent = true;
        }
    }

    if (addhidden)
    {
        // network is covered by some other network, hide it
        aNewNet.m_bHidden = cTRUE;
        // and copy it's settings
        AHFWNetworks_CopyNetworkSettings(aNewNet, aNetworks[greater]);

        // propagate settings to dependent networks
        AHFWNetworks_ChangeDependentNetworks(aNetworks, aNewNet);
    }
    else 
    {
        aNewNet.m_bHidden = cFALSE;

        if (adddependent)
        {
            // network depends on some other network
            AHFWNetworks_CopyNetworkSettings(aNewNet, aNetworks[dependson]);
        }
        else
        {
            // propagate settings to dependent networks
            AHFWNetworks_ChangeDependentNetworks(aNetworks, aNewNet);
        }

        // network can cover some other network, find them
        AHFWNetworks_HideNetworks(aNetworks, aNewNet);
    }

    if (aInsertIndex != (tUINT)(-1))  
        aNetworks.insert(aInsertIndex, aNewNet);
    else                              
        aNetworks.push_back(aNewNet);

    PR_TRACE((g_root, prtIMPORTANT, "FW\tAddNetwork: adding network. mode:%s, subnet: %s/%s, mac:%I64x, zone: %d, adapter: %S", aNewNet.m_bHidden?"hidden":"shown", cIPToA(aNewNet.m_IP), cIPMaskToA(aNewNet.m_Mask), aNewNet.m_nMac, aNewNet.m_eZone, aNewNet.m_sAdapterDesc.data()));

    return addhidden || adddependent ? cFALSE : cTRUE;
}

void AHFWNetworks_UnhideNetworks(cAHFWNetworks &aNetworks, cAHFWNetwork& aDeletedNet, tUINT aSkipIndex)
{
    if (aDeletedNet.IsShown())
    {
        // find a hidden network which must become shown
        for(tUINT i = 0; i < aNetworks.size(); i++)
        {
            cAHFWNetwork &Neti = aNetworks[i];
            if (i != aSkipIndex &&
                !Neti.IsShown() && 
                aDeletedNet.IsIPGreaterEqual(Neti)) // a network, which hidden by deleted network
            {
                bool show = true;
                // for each 
                for (tUINT j = 0; j < aNetworks.size(); j++)
                {
                    if (j != aSkipIndex)
                    {
                        cAHFWNetwork &Netj = aNetworks[j];
                        if (Netj.IsShown())
                        {
                            // a network must not be hidden by another shown network
                            if (Netj.IsIPGreaterEqual(Neti)) 
                                show = false;
                        }
                        else
                        {
                            // a network must not be hidden by another hidden network
                            if (Netj.IsIPGreater(Neti)) 
                                show = false;
                        }
                    }
                }
                if (show)
                {
                    // show 
                    Neti.m_bHidden = cFALSE;

                    tUINT dependson = AHFWNetworks_FindDependsOn(aNetworks, Neti);
                    if (dependson != aNetworks.size())
                    {
                        // copy settings from whom this one depends on
                        AHFWNetworks_CopyNetworkSettings(Neti, aNetworks[dependson]);
                    }
                    else
                    {
                        // copy settings from the unhiding one
                        AHFWNetworks_CopyNetworkSettings(Neti, aDeletedNet);
                        // change all dependent as well, if any
                        AHFWNetworks_ChangeDependentNetworks(aNetworks, Neti);
                    }
                    PR_TRACE((g_root, prtIMPORTANT, "FW\tUnhideNetwork: show network. mode:%s, subnet: %s/%s, mac:%I64x, zone: %d, adapter: %S", Neti.m_bHidden?"hidden":"shown", cIPToA(Neti.m_IP), cIPMaskToA(Neti.m_Mask), Neti.m_nMac, Neti.m_eZone, Neti.m_sAdapterDesc.data()));
                }
            }
        }
    }
}

tBOOL AHFWNetworks_RedependNetworks(cAHFWNetworks& aNetworks, cAHFWNetwork& aNet)
{
    if (!aNet.IsUserDefined())
    {
        // find a network with greater dependency level than aNet
        tINT level = 0;
        for(tUINT i = 0; i < aNetworks.size(); i++)
        {
            cAHFWNetwork &Net = aNetworks[i];
            if (Net.IsMacEqual(aNet) &&
                Net.GetDependencyLevel() > aNet.GetDependencyLevel())
            {
                // if found, we must transfer the settings from the network found
                // to the dependent networks

                for (tUINT i = 0; i < aNetworks.size(); i++)
                {
                    cAHFWNetwork &Neti = aNetworks[i];
                    if (Neti.IsDependent() &&
                        Neti.IsMacEqual(Net))
                    {
                        AHFWNetworks_CopyNetworkSettings(Neti, Net);
                    }
                }
                return cTRUE;
            }
        }
    }
    return cFALSE;
}

void AHFWNetworks_DeleteDependentNetworks(cAHFWNetworks& aNetworks, cAHFWNetwork& aDeletedNet)
{
    if (!aDeletedNet.IsUserDefined() &&
        !aDeletedNet.IsDependent())
    {
        if (!AHFWNetworks_RedependNetworks(aNetworks, aDeletedNet))
        {
            // find dependent networks which must be deleted
            for(tUINT i = 0; i < aNetworks.size(); i++)
            {
                cAHFWNetwork &Net = aNetworks[i];
                if (Net.IsDependent() &&
                    Net.IsMacEqual(aDeletedNet))
                {
                    aNetworks.remove(i--);
                }
            }
        }
    }
}

void AHFWNetworks_DeleteNetwork(cAHFWNetworks &aNetworks, tUINT aIndex, tBOOL dependent, tBOOL hidden)
{
    if (aIndex >= 0 && aIndex < aNetworks.size())
    {
        cAHFWNetwork DeletedNet = aNetworks[aIndex];
        aNetworks.remove(aIndex);

        PR_TRACE((g_root, prtIMPORTANT, "FW\tDeleteNetwork: deleting network. mode:%s, subnet: %s/%s, mac:%I64x, zone: %d, adapter: %S", DeletedNet.m_bHidden?"hidden":"shown", cIPToA(DeletedNet.m_IP), cIPMaskToA(DeletedNet.m_Mask), DeletedNet.m_nMac, DeletedNet.m_eZone, DeletedNet.m_sAdapterDesc.data()));

        if (hidden)
        {
            if (DeletedNet.IsShown())
            {
                // find hidden networks
                tUINT i = 0; 
                while(i < aNetworks.size())
                {
	                cAHFWNetwork &Neti = aNetworks[i];
                    if (!Neti.IsShown() && 
                        DeletedNet.IsIPGreaterEqual(Neti)) // a network, which hidden by deleted network
                    {
                        PR_TRACE((g_root, prtIMPORTANT, "FW\tDeleteNetworkWithHidden: deleting hidden network. mode:%s, subnet: %s/%s, mac:%I64x, zone: %d, adapter: %S", Neti.m_bHidden?"hidden":"shown", cIPToA(Neti.m_IP), cIPMaskToA(Neti.m_Mask), Neti.m_nMac, Neti.m_eZone, Neti.m_sAdapterDesc.data()));

                        cAHFWNetwork DeletedHiddenNet = aNetworks[i];
                        aNetworks.remove(i);
                        if (dependent)
                            AHFWNetworks_DeleteDependentNetworks(aNetworks, DeletedHiddenNet);
                        else
                            AHFWNetworks_RedependNetworks(aNetworks, DeletedHiddenNet);

                        i = 0;
                    }
                    else
                        ++i;
                }
            }
        }
        else
            AHFWNetworks_UnhideNetworks(aNetworks, DeletedNet);

        if (dependent)
            AHFWNetworks_DeleteDependentNetworks(aNetworks, DeletedNet);
        else
            AHFWNetworks_RedependNetworks(aNetworks, DeletedNet);
    }
    else
    {
        PR_TRACE((g_root, prtIMPORTANT, "FW\tDeleteNetwork: invalid index %d", aIndex));
    }
}
void AHFWNetworks_ChangeDependentNetworks(cAHFWNetworks &aNetworks, cAHFWNetwork &aNet)
{
    if (!aNet.IsUserDefined() &&
        !aNet.IsDependent())
    {
        tUINT i = 0;
        // find if any network has greater dependency level
        for (; i < aNetworks.size(); i++)
        {
            cAHFWNetwork& Net = aNetworks[i];
            if (Net.IsMacEqual(aNet) &&
                Net.GetDependencyLevel() > aNet.GetDependencyLevel())
            {
                break;
            }
        }
        // if no
        if (i == aNetworks.size())
        {
            // then find dependent (and not hidden) networks and change them
            for (i = 0; i < aNetworks.size(); ++i)
            {
                cAHFWNetwork& Net = aNetworks[i];
                if (Net.IsDependent() &&
                    Net.IsMacEqual(aNet) &&
                    Net.IsShown())
                {
                    AHFWNetworks_CopyNetworkSettings(aNetworks[i], aNet);
                }
            }
        }
    }
}
void AHFWNetworks_ChangeNetwork(cAHFWNetworks &aNetworks, tUINT aIndex, cAHFWNetwork &aNewNet)
{
    if (aIndex >= 0 && aIndex < aNetworks.size())
    {
        cAHFWNetwork &ChangingNet = aNetworks[aIndex];

        if (ChangingNet.IsIPEqual(aNewNet))
        {
            for (tUINT i = 0; i < aNetworks.size(); i++)
            {
                if (i != aIndex)
                {
                    if (ChangingNet.IsIPGreaterEqual(aNetworks[i]))
                    {
                        AHFWNetworks_CopyNetworkSettings(aNetworks[i], aNewNet);
                        // change dependants, if any
                        AHFWNetworks_ChangeDependentNetworks(aNetworks, aNetworks[i]);
                    }
                }
            }
            ChangingNet = aNewNet;
        }
        else
        {
            AHFWNetworks_UnhideNetworks(aNetworks, ChangingNet, aIndex);
            ChangingNet = aNewNet;
            AHFWNetworks_HideNetworks(aNetworks, ChangingNet, aIndex);
        }

        AHFWNetworks_ChangeDependentNetworks(aNetworks, aNewNet);
    }
}

// UpdateNetworks
// return value:
// cTRUE - network list is updated
// cFALSE - network list is not updated
tBOOL AHFWNetworks_UpdateNetworks(cAHFWNetworks &aNetworks, cAHFWNetworks& aNewNetworks)
{
    PR_TRACE((g_root, prtIMPORTANT, "FW\tUpdateNetworks"));
    aNewNetworks.clear();

    cAHFWNetworks DetectedNetworks; AHFWNetworks_GetNetworks(DetectedNetworks);

    tBOOL updated = cFALSE;

    for (tUINT i = 0; i < DetectedNetworks.size(); ++i)
    {
        cAHFWNetwork &DetectedNet = DetectedNetworks[i];
        
        tUINT j = AHFWNetworks_FindNetwork(aNetworks, DetectedNet);

        if (j != aNetworks.size()) // found 
        {
            PR_TRACE((g_root, prtIMPORTANT, "FW\tUpdateNetworks: detected network found. mode:%s, subnet: %s/%s, mac:%I64x, zone: %d, adapter: %S", DetectedNet.m_bHidden?"hidden":"shown", cIPToA(DetectedNet.m_IP), cIPMaskToA(DetectedNet.m_Mask), DetectedNet.m_nMac, DetectedNet.m_eZone, DetectedNet.m_sAdapterDesc.data()));

            if (!aNetworks[j].IsIPEqual(DetectedNet))
            {
                // mac is equal, IP is changed
                cAHFWNetwork ChangingNet;
                AHFWNetworks_GetEffectiveNetwork(aNetworks, j, ChangingNet);
                AHFWNetworks_DeleteNetwork(aNetworks, j, cFALSE, cFALSE);
                ChangingNet.m_IP = DetectedNet.m_IP;
                ChangingNet.m_Mask = DetectedNet.m_Mask;
                AHFWNetworks_AddNetwork(aNetworks, ChangingNet);

                updated = cTRUE;
            }
        }
        else // not found, new network
        {
            PR_TRACE((g_root, prtIMPORTANT, "FW\tUpdateNetworks: detected network not found. mode:%s, subnet: %s/%s mac:%I64x, zone: %d, adapter: %S", DetectedNet.m_bHidden?"hidden":"shown", cIPToA(DetectedNet.m_IP), cIPMaskToA(DetectedNet.m_Mask), DetectedNet.m_nMac, DetectedNet.m_eZone, DetectedNet.m_sAdapterDesc.data()));

            if (AHFWNetworks_AddNetwork(aNetworks, DetectedNet))
            {
                // new network added as shown, we should ask user about it
                aNewNetworks.push_back(DetectedNet);
            }
            updated = cTRUE;
        }
    }
    return updated;
}

void AHFWNetworks_RefreshNetworks(cAHFWNetworks &aNetworks)
{
    PR_TRACE((g_root, prtIMPORTANT, "FW\tRefreshNetworks"));
    cAHFWNetworks DetectedNetworks; AHFWNetworks_GetNetworks(DetectedNetworks);

    cAHFWNetworks RefreshedNetworks;

    // copy user defined networks
    for (tUINT i = 0; i < aNetworks.size(); ++i)
    {
        if (aNetworks[i].IsUserDefined())
            AHFWNetworks_AddNetwork(RefreshedNetworks, aNetworks[i]);
    }

    for (tUINT i = 0; i < DetectedNetworks.size(); ++i)
    {
        cAHFWNetwork &DetectedNet = DetectedNetworks[i];
        tUINT j = AHFWNetworks_FindNetwork(aNetworks, DetectedNet);
        if (j != aNetworks.size())
        {
            // copy settings
            if (aNetworks[j].IsShown())
                AHFWNetworks_CopyNetworkSettings(DetectedNet, aNetworks[j]);
        }
        AHFWNetworks_AddNetwork(RefreshedNetworks, DetectedNet);
    }
    aNetworks.clear();
    aNetworks = RefreshedNetworks;
}

void AHFWNetworks_CopyNetworkSettings(cAHFWNetwork & ToNetwork, cAHFWNetwork & FromNetwork)
{
    ToNetwork.m_eZone = FromNetwork.m_eZone;
    ToNetwork.m_bStealthed = FromNetwork.m_bStealthed;
}

tERROR AHFWNetworks_GetNetworks(cAHFWNetworks &aNetworks)
{
	CKAHUM::OpResult opres = CKAHUM::srOK;

	aNetworks.clear();
	
	PR_TRACE((g_root, prtIMPORTANT, "FW\tGetNetworks. start working..."));

	HNETWORKLIST hnetworks;
	if( (opres = GetNetworkList(&hnetworks)) == CKAHUM::srOK )
	{
		int networks_count = 0;
		if( (opres = NetworkList_GetSize(hnetworks, &networks_count)) == CKAHUM::srOK && networks_count )
		{
			Network network;
			for(int cou = 0; cou < networks_count; cou++)
			{
				if( (opres = NetworkList_GetItem(hnetworks, cou, &network)) == CKAHUM::srOK )
				{
                    bool isunicast = false;
                    _eAHFWNetworkZone eZone = _fwnzUntrusted;

                    // IPv4
                    if (network.HasIPv4 && 
                        network.IPv4 != 0x7f000001) // 127.0.0.1 localhost
                    {
				        cAHFWNetwork & net = aNetworks.push_back();
                        net.m_IP.SetV4(network.IPv4 & network.Maskv4);
				        net.m_Mask.SetV4(network.Maskv4);
			            net.m_sAdapterDesc = network.AdapterDesc;
                        net.m_nMac = *(tQWORD*)network.Phys;
                        if (net.m_nMac == 0) net.m_nMac = 0x0000FFFFFFFFFFFF;
                        net.m_eType = _fwntIPv4;
    					
			            net.m_eZone = eZone = IsLocalArea(net.m_IP, net.m_Mask) ? _fwnzNetBIOS : _fwnzUntrusted;
			            net.m_bStealthed = net.m_eZone == _fwnzUntrusted;

                        isunicast = true;

                        PR_TRACE((g_root, prtIMPORTANT, "FW\tGetNetworks. subnet: %s/%s, mac:%I64x, zone: %d, adapter: %S", cIPToA(net.m_IP), cIPMaskToA(net.m_Mask), net.m_nMac, net.m_eZone, net.m_sAdapterDesc.data()));
                    }

                    // IPv6 global unicast
                    if (network.HasIPv6 && 
                        network.IPv6.IsValid() &&                              // [::]  - uninitialized
                        ! (network.IPv6.O.Hi == 0 && network.IPv6.O.Lo == 1) ) // [::1] - localhost
                    {
			            cAHFWNetwork & net = aNetworks.push_back();
                        cIPv6 netip = CKAHUMIPv6_cIPv6(network.IPv6),
                              netmask = CKAHUMIPv6_cIPv6(network.Maskv6);
                        net.m_IP.SetV6(netip & netmask);
			            net.m_Mask.SetV6(netmask);
		                net.m_sAdapterDesc = network.AdapterDesc;
                        net.m_nMac = *(tQWORD*)network.Phys;
                        if (net.m_nMac == 0) net.m_nMac = 0x0000FFFFFFFFFFFFULL;
                        net.m_eType = _fwntIPv6GlobUni;
    					
		                net.m_eZone = eZone = IsLocalArea(net.m_IP, net.m_Mask) ? _fwnzNetBIOS : _fwnzUntrusted;
		                net.m_bStealthed = net.m_eZone == _fwnzUntrusted;

                        isunicast = true;

                        PR_TRACE((g_root, prtIMPORTANT, "FW\tGetNetworks. subnet: %s/%s, mac:%I64x, zone: %d, adapter: %S", cIPToA(net.m_IP), cIPMaskToA(net.m_Mask), net.m_nMac, net.m_eZone, net.m_sAdapterDesc.data()));
                    }

                    if (network.HasIPv6 && isunicast)
                    {
                        // IPv6 link unicast 
                        // fe80::%zone/64
                        {
			                cAHFWNetwork & net = aNetworks.push_back();
                            
                            net.m_IP.SetV6ScopePrefix(_ipv6Scope_Link, false, network.Zones[CKAHUM::IPv6::ScopeLink]);
			                net.m_Mask.SetV6Mask(64, 0);
		                    net.m_sAdapterDesc = network.AdapterDesc;
                            net.m_nMac = *(tQWORD*)network.Phys;
                            if (net.m_nMac == 0) net.m_nMac = 0x0000FFFFFFFFFFFFULL;
                            net.m_eType = _fwntIPv6LinkUni;
        					
		                    net.m_eZone = eZone;
		                    net.m_bStealthed = net.m_eZone == _fwnzUntrusted;

                            PR_TRACE((g_root, prtIMPORTANT, "FW\tGetNetworks. subnet: %s/%s, mac:%I64x, zone: %d, adapter: %S", cIPToA(net.m_IP), cIPMaskToA(net.m_Mask), net.m_nMac, net.m_eZone, net.m_sAdapterDesc.data()));
                        }

                        // IPv6 interface multicast 
                        // ff01::%zone/16
                        {
			                cAHFWNetwork & net = aNetworks.push_back();
                            
                            net.m_IP.SetV6ScopePrefix(_ipv6Scope_Interface, true, network.Zones[CKAHUM::IPv6::ScopeInterface]);
			                net.m_Mask.SetV6Mask(16, 0);
		                    net.m_sAdapterDesc = network.AdapterDesc;
                            net.m_nMac = *(tQWORD*)network.Phys;
                            if (net.m_nMac == 0) net.m_nMac = 0x0000FFFFFFFFFFFFULL;
                            net.m_eType = _fwntIPv6IfMulti;
        					
		                    net.m_eZone = eZone;
		                    net.m_bStealthed = net.m_eZone == _fwnzUntrusted;

                            PR_TRACE((g_root, prtIMPORTANT, "FW\tGetNetworks. subnet: %s/%s, mac:%I64x, zone: %d, adapter: %S", cIPToA(net.m_IP), cIPMaskToA(net.m_Mask), net.m_nMac, net.m_eZone, net.m_sAdapterDesc.data()));
                        }

                        // IPv6 link multicast 
                        // ff02::%zone/16
                        {
			                cAHFWNetwork & net = aNetworks.push_back();
                            
                            net.m_IP.SetV6ScopePrefix(_ipv6Scope_Link, true, network.Zones[CKAHUM::IPv6::ScopeLink]);
			                net.m_Mask.SetV6Mask(16, 0);
		                    net.m_sAdapterDesc = network.AdapterDesc;
                            net.m_nMac = *(tQWORD*)network.Phys;
                            if (net.m_nMac == 0) net.m_nMac = 0x0000FFFFFFFFFFFFULL;
                            net.m_eType = _fwntIPV6LinkMulti;
        					
		                    net.m_eZone = eZone;
		                    net.m_bStealthed = net.m_eZone == _fwnzUntrusted;

                            PR_TRACE((g_root, prtIMPORTANT, "FW\tGetNetworks. subnet: %s/%s, mac:%I64x, zone: %d, adapter: %S", cIPToA(net.m_IP), cIPMaskToA(net.m_Mask), net.m_nMac, net.m_eZone, net.m_sAdapterDesc.data()));
                        }
                    }
				}
			}
		}
		NetworkList_Delete(hnetworks);
	}

	PR_TRACE((g_root, prtIMPORTANT, "FW\tGetNetworks. finished."));

	return opres == CKAHUM::srOK ? errOK : errNOT_OK;
}
