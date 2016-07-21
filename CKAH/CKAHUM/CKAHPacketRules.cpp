#include "stdafx.h"
#include <CKAH/ckahrules.h>
#include "CKAHPacketRules.h"
#include "../ckahrules/ckahrulesint.h"
#include "winsock.h"
#include "../../windows/Hook/Plugins/Include/fw_ev_api.h"

void ReGetFilterListByID (UUID uid, FLTLIST &ret_list, FLTLIST &in_list)
{
	ret_list.clear ();

	for (FLTLIST::iterator i = in_list.begin (); i != in_list.end ();)
	{
		PFILTER_PARAM pParam = i->FindParam (PF_FILTER_USER_ID);
		
		if (pParam && IsEqualIID (uid, *(UUID*)pParam->m_ParamValue))
		{
			ret_list.push_back (*i);
			i = in_list.erase (i);
		}
		else
		{
			++i;
		}
	}
}

bool FillPacketRuleFromFLTLIST (const FLTLIST &flt_list, CPacketRule &PacketRule)
{
	FLTLIST::const_iterator i;

	PFILTER_PARAM pFilterParam = NULL;

	for (i = flt_list.begin (); i != flt_list.end (); ++i)
	{
		pFilterParam = i->FindParam (PACKET_FILTER_WIZARD_DATA);

		if  (pFilterParam)
			break;			
	}

	if (pFilterParam)
	{
		PoliType (CMemStorage (pFilterParam->m_ParamValue, pFilterParam->m_ParamSize)) << PacketRule;
		return true;
	}
		
	return false;
}

#define ETH_FRAME_TYPE_IP     0x0800
#define ETH_FRAME_TYPE_IPV6   0x86DD

bool ExportRuleIPList (const RuleAddressList &addr_list, FLTLIST &flt_list, bool bIsRemote)
{
    typedef std::list<DWORD> CIPv4List;
    CIPv4List single_ipv4_list;

    typedef std::list<const CKAHUM::IPv6*> CIPv6List;
    typedef std::map<DWORD, CIPv6List> CIPv6ZoneMap;
    CIPv6ZoneMap single_ipv6_zone_map;

    // local ip address is an optional parameter, 
    // because when checking TDI event agains packet filters there might not be a local address.
    DWORD dwFlags = bIsRemote ? _FILTER_PARAM_FLAG_MUSTEXIST : _FILTER_PARAM_FLAG_NONE;
	
	for  ( RuleAddressList::const_iterator i = addr_list.begin (); i != addr_list.end (); ++i)
	{
		CGuardFilter ip_filter;
		
		switch ((*i)->GetType ())
		{
		case CKAHFW::ratHost:
			{
				CRuleHostAddress *pHostAddress = (CRuleHostAddress *)((*i).operator -> ());
				
                for ( std::vector <CKAHUM::IP>::const_iterator j = pHostAddress->m_IPs.begin ();
																  j != pHostAddress->m_IPs.end (); ++j)
				{
                    if (j->Isv4())
                        single_ipv4_list.push_back(j->v4);
                    else if (j->Isv6())
                        single_ipv6_zone_map[j->v6.Zone].push_back(&j->v6);
				}
			}
			break;
		case CKAHFW::ratRange:
			{
				CRuleAddressRange *pRangeAddress = (CRuleAddressRange *)((*i).operator -> ());

                if (pRangeAddress->m_LoAddress.Isv4())
                {
				    ip_filter.AddParam (bIsRemote ? FW_ID_REMOTE_IP : FW_ID_LOCAL_IP, FLT_PARAM_LESS, sizeof(DWORD), &pRangeAddress->m_LoAddress.v4, dwFlags);
				    ip_filter.AddParam (bIsRemote ? FW_ID_REMOTE_IP : FW_ID_LOCAL_IP, FLT_PARAM_MORE, sizeof(DWORD), &pRangeAddress->m_HiAddress.v4, dwFlags);

                    if (!bIsRemote)
                    {
                        USHORT ethtype = ETH_FRAME_TYPE_IP;
                        ip_filter.AddParam ( FW_ID_ETH_FRAME_TYPE, FLT_PARAM_EUA, sizeof(USHORT), &ethtype, _FILTER_PARAM_FLAG_MUSTEXIST);
                    }
                }
                else if (pRangeAddress->m_LoAddress.Isv6())
                {
                    ip_filter.AddParam (bIsRemote ? FW_ID_REMOTE_IPV6 : FW_ID_LOCAL_IPV6, FLT_PARAM_LESS, sizeof(CKAHUM::OWord), pRangeAddress->m_LoAddress.v6.O.Dwords, dwFlags);
                    ip_filter.AddParam (bIsRemote ? FW_ID_REMOTE_IPV6 : FW_ID_LOCAL_IPV6, FLT_PARAM_MORE, sizeof(CKAHUM::OWord), pRangeAddress->m_HiAddress.v6.O.Dwords, dwFlags);

                    if (pRangeAddress->m_LoAddress.v6.Zone != 0)
                    {
                        ip_filter.AddParam (bIsRemote ? FW_ID_REMOTE_IPV6_ZONE : FW_ID_LOCAL_IPV6_ZONE, FLT_PARAM_EUA, sizeof(DWORD), &pRangeAddress->m_LoAddress.v6.Zone, dwFlags);
                    }

                    if (!bIsRemote)
                    {
                        USHORT ethtype = ETH_FRAME_TYPE_IPV6;
                        ip_filter.AddParam ( FW_ID_ETH_FRAME_TYPE, FLT_PARAM_EUA, sizeof(USHORT), &ethtype, _FILTER_PARAM_FLAG_MUSTEXIST);
                    }
                }

				flt_list.insert (flt_list.end (), ip_filter);
			}
			break;
		case CKAHFW::ratSubnet:
			{
				CRuleAddressSubnet *pSubnetAddress = (CRuleAddressSubnet *)((*i).operator -> ());

                if (pSubnetAddress->m_Address.Isv4())
                {
				    DWORD Params[2];
				    Params[0] = pSubnetAddress->m_Address.v4;
				    Params[1] = pSubnetAddress->m_Mask.v4;

				    ip_filter.AddParam (bIsRemote ? FW_ID_REMOTE_IP   : FW_ID_LOCAL_IP,   FLT_PARAM_MASK, sizeof(Params), Params, dwFlags);

                    if (!bIsRemote)
                    {
                        USHORT ethtype = ETH_FRAME_TYPE_IP;
                        ip_filter.AddParam ( FW_ID_ETH_FRAME_TYPE, FLT_PARAM_EUA, sizeof(USHORT), &ethtype, _FILTER_PARAM_FLAG_MUSTEXIST);
                    }
                }
                else if (pSubnetAddress->m_Address.Isv6())
                {
                    CKAHUM::OWord Params[2];
                    Params[0] = pSubnetAddress->m_Address.v6.O;
                    Params[1] = pSubnetAddress->m_Mask.v6.O;

				    ip_filter.AddParam (bIsRemote ? FW_ID_REMOTE_IPV6 : FW_ID_LOCAL_IPV6, FLT_PARAM_MASK, sizeof(Params), Params, dwFlags);

                    if (pSubnetAddress->m_Address.v6.Zone != 0)
                    {
                        ip_filter.AddParam (bIsRemote ? FW_ID_REMOTE_IPV6_ZONE : FW_ID_LOCAL_IPV6_ZONE, FLT_PARAM_EUA, sizeof(DWORD), &pSubnetAddress->m_Address.v6.Zone, dwFlags);
                    }

                    if (!bIsRemote)
                    {
                        USHORT ethtype = ETH_FRAME_TYPE_IPV6;
                        ip_filter.AddParam ( FW_ID_ETH_FRAME_TYPE, FLT_PARAM_EUA, sizeof(USHORT), &ethtype, _FILTER_PARAM_FLAG_MUSTEXIST);
                    }
                }

				flt_list.insert (flt_list.end (), ip_filter);				
			}
			break;

		default:
			assert (0);
			return false;
		}
	}
	
    // IPv4 list
	if   ( !single_ipv4_list.empty () )
	{
		CGuardFilter ip_filter;
		
        std::vector<DWORD> IPBuf;
        IPBuf.reserve(single_ipv4_list.size ());
        IPBuf.insert(IPBuf.begin(), single_ipv4_list.begin (), single_ipv4_list.end ());
		
        ip_filter.AddParam ( bIsRemote ? FW_ID_REMOTE_IP : FW_ID_LOCAL_IP, FLT_PARAM_EQU_LIST, sizeof(DWORD) * single_ipv4_list.size (), &IPBuf[0], dwFlags);

        if (!bIsRemote)
        {
            USHORT ethtype = ETH_FRAME_TYPE_IP;
            ip_filter.AddParam ( FW_ID_ETH_FRAME_TYPE, FLT_PARAM_EUA, sizeof(USHORT), &ethtype, _FILTER_PARAM_FLAG_MUSTEXIST);
        }

		flt_list.insert (flt_list.end (), ip_filter);
	}

    // IPv6 list for each zone
    for (CIPv6ZoneMap::iterator i = single_ipv6_zone_map.begin(); i != single_ipv6_zone_map.end(); ++i)
	{
		CGuardFilter ip_filter;
		
        std::vector<CKAHUM::OWord> IPBuf;
        IPBuf.resize(i->second.size());
		
        int cou = 0;
		for (CIPv6List::const_iterator j = i->second.begin(); j != i->second.end (); ++j)
			IPBuf[cou++] = (*j)->O;
		
        ip_filter.AddParam ( bIsRemote ? FW_ID_REMOTE_IPV6 : FW_ID_LOCAL_IPV6, FLT_PARAM_EQU_LIST, sizeof(CKAHUM::OWord) * i->second.size(), &IPBuf[0], dwFlags);

        if (i->first != 0)
        {
            ip_filter.AddParam ( bIsRemote ? FW_ID_REMOTE_IPV6_ZONE : FW_ID_LOCAL_IPV6_ZONE, FLT_PARAM_EUA, sizeof(DWORD), (LPVOID)&i->first, dwFlags);
        }

        if (!bIsRemote)
        {
            USHORT ethtype = ETH_FRAME_TYPE_IPV6;
            ip_filter.AddParam ( FW_ID_ETH_FRAME_TYPE, FLT_PARAM_EUA, sizeof(USHORT), &ethtype, _FILTER_PARAM_FLAG_MUSTEXIST);
        }

		flt_list.insert (flt_list.end (), ip_filter);
	}

	return true;
}

bool ExportRulePortList (const RulePortList &port_list, FLTLIST &flt_list, bool bIsRemote)
{
	std::list<USHORT> single_port_list;

	ULONG uParamId = bIsRemote ? FW_ID_REMOTE_PORT : FW_ID_LOCAL_PORT;

	for (RulePortList::const_iterator i = port_list.begin(); i != port_list.end(); ++i)
	{
		CGuardFilter port_filter;

		switch((*i)->GetType())
		{
		case CKAHFW::rptSingle:
			{
				CRuleSinglePort *pSinglePort = (CRuleSinglePort *)((*i).operator -> ());
				single_port_list.insert(single_port_list.end(), pSinglePort->m_Port);
			}

			break;
		case CKAHFW::rptRange:
			{
				CRulePortRange *pPortRange = (CRulePortRange *)((*i).operator -> ());

				USHORT sStartPort = pPortRange->m_LoPort;
				USHORT sEndPort = pPortRange->m_HiPort;

				port_filter.AddParam (uParamId, FLT_PARAM_LESS, sizeof(USHORT), &sStartPort);
				port_filter.AddParam (uParamId, FLT_PARAM_MORE, sizeof(USHORT), &sEndPort);

				flt_list.insert (flt_list.end(), port_filter);
			}
			break;

		default:
			assert (0);
			return false;
		}
	}

	if (!single_port_list.empty())
	{
		CGuardFilter port_filter;

		USHORT* sPortBuf = new USHORT[single_port_list.size()];

		if(!sPortBuf)
			return false;

		int cou = 0;

		for (std::list<USHORT>::const_iterator j = single_port_list.begin (); j != single_port_list.end (); ++j)
			sPortBuf[cou++] = *j;

		port_filter.AddParam (uParamId, FLT_PARAM_EQU_LIST, sizeof(USHORT) * single_port_list.size(), sPortBuf);

		flt_list.insert (flt_list.end(), port_filter);

		delete []sPortBuf;
	}

	return true;
}

static bool ExportRuleTimeElementList (const RuleTimeElementList &time_element_list, CGuardFilter &time_filter)
{
	for (RuleTimeElementList::const_iterator i = time_element_list.begin(); i != time_element_list.end(); ++i)
	{
		CRuleTimeElement *pTimeElement = (CRuleTimeElement *)((*i).operator -> ());

        ULONG uParamId = 0;

        UCHAR  ucLo = (UCHAR)pTimeElement->m_LoValue, ucHi = (UCHAR)pTimeElement->m_HiValue; 
        USHORT usLo = (USHORT)pTimeElement->m_LoValue, usHi = (USHORT)pTimeElement->m_HiValue; 
        ULONG  ulLo = pTimeElement->m_LoValue, ulHi = pTimeElement->m_HiValue; 

        LPVOID pLo = 0, pHi = 0;
        int size = 0;

        switch (pTimeElement->m_Type)
        {
        case CKAHFW::rtetSecond:    uParamId = FW_ID_TIME_SECOND;     size = sizeof(UCHAR);  pLo = &ucLo; pHi = &ucHi; break;
        case CKAHFW::rtetDaySecond: uParamId = FW_ID_TIME_DAY_SECOND; size = sizeof(ULONG);  pLo = &ulLo; pHi = &ulHi; break;
        case CKAHFW::rtetMinute:    uParamId = FW_ID_TIME_MINUTE;     size = sizeof(UCHAR);  pLo = &ucLo; pHi = &ucHi; break;
        case CKAHFW::rtetHour:      uParamId = FW_ID_TIME_HOUR;       size = sizeof(UCHAR);  pLo = &ucLo; pHi = &ucHi; break;
        case CKAHFW::rtetDay:       uParamId = FW_ID_TIME_DAY;        size = sizeof(UCHAR);  pLo = &ucLo; pHi = &ucHi; break;
        case CKAHFW::rtetWeekDay:   uParamId = FW_ID_TIME_WEEK_DAY;   size = sizeof(UCHAR);  pLo = &ucLo; pHi = &ucHi; break;
        case CKAHFW::rtetMonth:     uParamId = FW_ID_TIME_MONTH;      size = sizeof(UCHAR);  pLo = &ucLo; pHi = &ucHi; break;
        case CKAHFW::rtetYear:      uParamId = FW_ID_TIME_YEAR;       size = sizeof(USHORT); pLo = &usLo; pHi = &usHi; break;
        }

        if (size != 0)
        {
		    time_filter.AddParam (uParamId, FLT_PARAM_LESS, size, pLo);
		    time_filter.AddParam (uParamId, FLT_PARAM_MORE, size, pHi);
        }
    }

    return true;
}

bool ExportRuleTimeList (const RuleTimeList &time_list, FLTLIST &flt_list)
{
	for (RuleTimeList::const_iterator i = time_list.begin(); i != time_list.end(); ++i)
	{
        CGuardFilter time_filter;

		CRuleTime *pTime = (CRuleTime *)((*i).operator -> ());

        ExportRuleTimeElementList(pTime->m_TimeElements, time_filter);

        flt_list.insert (flt_list.end (), time_filter);
    }

    return true;
}

static bool ExportPacketRule (IN const CPacketRule &rule, IN OUT FLTLIST &flt_list, bool bIsInbound)
{
	FLTLIST fltlist_remote_ip, fltlist_local_ip, fltlist_remote_ports, fltlist_local_ports, fltlist_times;

	CGuardFilter FilterProto;

	if (rule.m_StreamDirection != CKAHFW::sdBoth)
	{
		DWORD stream = FW_STREAM_UNDEFINED;  
        switch (rule.m_StreamDirection)
        {
        case CKAHFW::sdIncoming: stream = FW_STREAM_IN; break;
        case CKAHFW::sdOutgoing: stream = FW_STREAM_OUT; break;
        }
		FilterProto.AddParam (FW_ID_STREAM_DIRECTION, FLT_PARAM_EUA, 4, &stream);
	}

	if (rule.m_Proto != CKAHFW::PROTO_ALL)
	{
		BYTE proto = rule.m_Proto;
		FilterProto.AddParam (FW_ID_PROTOCOL, FLT_PARAM_EUA, 1, &proto);
	}
	else
	{
		BYTE uParams[2];
		uParams[0] = 0;
		uParams[1] = 0;
		
		FilterProto.AddParam (FW_ID_PROTOCOL, FLT_PARAM_MASK, 2, &uParams);
	}

	if (rule.m_RemoteAddresses->m_Addresses.size ())
		ExportRuleIPList (rule.m_RemoteAddresses->m_Addresses, fltlist_remote_ip, true);

	if (rule.m_LocalAddresses->m_Addresses.size ())
		ExportRuleIPList (rule.m_LocalAddresses->m_Addresses, fltlist_local_ip, false);

	if (rule.m_Proto == CKAHFW::PROTO_TCP || rule.m_Proto == CKAHFW::PROTO_UDP)
	{
		if (rule.m_TCPUDPLocalPorts->m_Ports.size ())
			ExportRulePortList (rule.m_TCPUDPLocalPorts->m_Ports, fltlist_local_ports, false);

		if (rule.m_TCPUDPRemotePorts->m_Ports.size ())
			ExportRulePortList (rule.m_TCPUDPRemotePorts->m_Ports, fltlist_remote_ports, true);
	}
	else if (rule.m_Proto == CKAHFW::PROTO_ICMP || rule.m_Proto == CKAHFW::PROTO_ICMPV6)
	{
		if (rule.m_ICMPType != 0xFF)
			FilterProto.AddParam (FW_ID_ICMP_TYPE, FLT_PARAM_EUA, 1, (LPVOID)&rule.m_ICMPType);
		if (rule.m_ICMPCode != 0xFF)
			FilterProto.AddParam (FW_ID_ICMP_CODE, FLT_PARAM_EUA, 1, (LPVOID)&rule.m_ICMPCode);
	}

    if (rule.m_Times->m_Times.size ())
        ExportRuleTimeList (rule.m_Times->m_Times, fltlist_times);

	FLTLIST flt_ret = fltlist_remote_ip * fltlist_local_ip * fltlist_remote_ports * fltlist_local_ports * fltlist_times * FilterProto;

	DWORD flag = (rule.m_bIsBlocking) ? (FLT_A_DEFAULT | FLT_A_DENY) : FLT_A_DEFAULT;
	
	if (rule.m_bLog)
		flag |= FLT_A_INFO | FLT_A_USERLOG;

	if (rule.m_bNotify)
		flag |= FLT_A_INFO | FLT_A_ALERT1;	
	
	flag |= FLT_A_DISPATCH_MIRROR;

	if (rule.m_Proto!=CKAHFW::PROTO_ICMP && rule.m_ICMPCode==164)
	{
		flag|=0x00040000; // FLT_A_STOPPROCESSING;  defined in Windows\Hook\HOOK\avpgcom.h as 0x00040000; q? Alexander.Burt@kaspersky.com
	}
	
	flt_ret.SetOptions (FLTTYPE_KLICK, bIsInbound ? FW_NDIS_PACKET_IN : FW_NDIS_PACKET_OUT, 0, flag);

	flt_list += flt_ret;
	
	return true;
}

bool CreateFLTLISTFromPacketRule (IN const CPacketRule &PacketRule, OUT FLTLIST &flt_list)
{
	bool bExportResult = false;

	switch (PacketRule.m_PacketDirection)
	{
	case CKAHFW::pdBoth:
		bExportResult = ExportPacketRule (PacketRule, flt_list, true) && ExportPacketRule (PacketRule, flt_list, false);
		break;
	case CKAHFW::pdIncoming:
		bExportResult = ExportPacketRule (PacketRule, flt_list, true);
		break;
	case CKAHFW::pdOutgoing:
		bExportResult = ExportPacketRule (PacketRule, flt_list, false);
		break;
	default:
		assert (0);
		return false;
	}

	if (bExportResult && !flt_list.empty ())
	{
		FLTLIST::iterator i = flt_list.begin ();

		CMemStorage mems;
		PoliType (mems, false) << (CPacketRule)PacketRule;

		i->AddParam (PACKET_FILTER_WIZARD_DATA, FLT_PARAM_NOP,
							mems.size_ (), mems.get_lin_space_ (), _FILTER_PARAM_FLAG_NONE);
		
		UUID uuid;
		UuidCreate (&uuid);

		for ( i = flt_list.begin (); i != flt_list.end (); ++i)
		{
			i->AddParam (PF_FILTER_USER_ID, FLT_PARAM_NOP, sizeof (UUID), (void *)&uuid, _FILTER_PARAM_FLAG_MAP_TO_EVENT/*| _FILTER_PARAM_FLAG_CACHABLE*/);

			i->AddParam (PACKET_FILTER_NAME, FLT_PARAM_NOP, (PacketRule.m_Name.size () + 1) * sizeof (wchar_t), (void *)PacketRule.m_Name.c_str (), _FILTER_PARAM_FLAG_MAP_TO_EVENT );
			
			if (PacketRule.m_UserData.size () > 0)
				i->AddParam (PF_FILTER_USER_DATA, FLT_PARAM_NOP, PacketRule.m_UserData.size (), (LPVOID)&PacketRule.m_UserData[0], _FILTER_PARAM_FLAG_MAP_TO_EVENT);

			PacketRule.m_bIsEnabled ? i->Enable () : i->Disable ();
		}
	}
	
	return bExportResult;
}
