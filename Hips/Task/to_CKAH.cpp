#include "NetRulesManager.h"
using namespace CKAHFW;

CKAHUM::OpResult CNetRulesManager::GenerateAddrByIpRange(cIpRange *pIpRange,CKAHFW::HPACKETRULE hRule)
{
	CKAHUM::OpResult opres= CKAHUM::srOK;
	CKAHFW::HRULEADDRESSES hRuleAddresses = 0;
	cIpMasks vIP;
	//generate Local addrs;
	if(vIP.size()>0)
	{
		hRuleAddresses = CKAHFW::RuleAddresses_Create();
		if(!hRuleAddresses)
			opres = CKAHUM::srOpFailed;
		if(opres == CKAHUM::srOK)
			opres = GenerateAddresses(&vIP,fDefault,hRuleAddresses);
		if(opres == CKAHUM::srOK)
			opres = CKAHFW::PacketRule_SetLocalAddresses(hRule, hRuleAddresses);
		if(hRuleAddresses)
			CKAHFW::RuleAddresses_Delete(hRuleAddresses);
	}
	vIP.clear();
	//generate remote addrs;
	if(vIP.size()>0)
	{
		hRuleAddresses = CKAHFW::RuleAddresses_Create();
		if(!hRuleAddresses)
			opres = CKAHUM::srOpFailed;
		if(opres == CKAHUM::srOK)
			opres = GenerateAddresses(&vIP,fDefault,hRuleAddresses);
		if(opres == CKAHUM::srOK)
			opres = CKAHFW::PacketRule_SetRemoteAddresses(hRule, hRuleAddresses);
		if(hRuleAddresses)
			CKAHFW::RuleAddresses_Delete(hRuleAddresses);
	}
	return opres;
}
CKAHUM::OpResult CNetRulesManager::GenerateRuleByWebService(cWebService * pWebService,CKAHFW::HPACKETRULE hRule)
{
	CKAHUM::OpResult opres;
	opres = GenerateDirection(pWebService->m_eDirection,hRule);
	
	if(pWebService->m_nFlags & cWebService::fProtocol)
	{
		if(opres == CKAHUM::srOK)
			opres = CKAHFW::PacketRule_SetProtocol(hRule, pWebService->m_nProto);
		if(opres == CKAHUM::srOK && pWebService->m_nProto==cWebService::pTCP || pWebService->m_nProto==cWebService::pUDP)
			opres=GeneratePortList(&pWebService->m_aLocalPorts,&pWebService->m_aRemotePorts,hRule);
		if(opres == CKAHUM::srOK && pWebService->m_nProto==cWebService::pICMP || pWebService->m_nProto==cWebService::pICMPV6)
		{
			if(opres == CKAHUM::srOK && pWebService->m_nFlags & cWebService::fIcmpType )
			{
				opres = CKAHFW::PacketRule_SetICMPType(hRule, pWebService->m_nIcmpType);
				PR_TRACE((m_hips, prtNOTIFY, "add ICMP type %d",pWebService->m_nIcmpType));
			}		
			if(opres == CKAHUM::srOK && pWebService->m_nFlags & cWebService::fIcmpCode)
			{
				opres = CKAHFW::PacketRule_SetICMPCode(hRule, pWebService->m_nIcmpCode);
				PR_TRACE((m_hips, prtNOTIFY, "add ICMP code %d",pWebService->m_nIcmpCode));
			}
		}
	}
	else
	{
		if(opres == CKAHUM::srOK)
			opres = CKAHFW::PacketRule_SetProtocol(hRule, 0);
	}
	if(opres == CKAHUM::srOK)
		opres = CKAHFW::PacketRule_SetIsEnabled(hRule, true);
	
	if (CKAHUM::srOK != opres)
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GenerateRuleByWebService CKAHFW error %d",opres));
	}
	return opres;
}
CKAHUM::OpResult CNetRulesManager::GenerateAddressesByZone(cNetZone * pZona,cZonaAddresses * pZonaAddresses,CKAHFW::HPACKETRULE hRule)
{
	CKAHUM::OpResult opres= CKAHUM::srOK;
	CKAHFW::HRULEADDRESSES hRuleAddresses = CKAHFW::RuleAddresses_Create();
	if(!hRuleAddresses)
		opres = CKAHUM::srOpFailed;
	if(pZona->m_nSettings & cNetZone::fUptoGateway)
	{
		PR_TRACE((m_hips, prtIMPORTANT, "add as remote addresses:"));
		opres = GenerateAddresses(&pZonaAddresses->m_vIP,fOnlyNet,hRuleAddresses);
	}
	else
	{
		PR_TRACE((m_hips, prtIMPORTANT, "add as local addresses:"));
		opres = GenerateAddresses(&pZonaAddresses->m_vIP,fOnlyHost,hRuleAddresses);
	}

	if(opres == CKAHUM::srOK)
	{
		if(pZona->m_nSettings & cNetZone::fUptoGateway)
			opres = CKAHFW::PacketRule_SetRemoteAddresses(hRule, hRuleAddresses);
		else
			opres = CKAHFW::PacketRule_SetLocalAddresses(hRule, hRuleAddresses);
	}
	if(hRuleAddresses)
		CKAHFW::RuleAddresses_Delete(hRuleAddresses);

	if (CKAHUM::srOK != opres)
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GenerateZoneAddresses error, zone_name=%S,CKAHUM::OpResult=%d",pZona->m_sFrendlyName.data(),opres));
	}
	return opres;
}
CKAHUM::OpResult CNetRulesManager::GenerateAddressesByZoneApp(cNetZone * pZona,cZonaAddresses * pZonaAddresses,CKAHFW::HAPPRULEELEMENT hElement)
{
	CKAHUM::OpResult opres= CKAHUM::srOK;
	CKAHFW::HRULEADDRESSES hRuleAddresses = CKAHFW::RuleAddresses_Create();
	if(!hRuleAddresses)
		opres = CKAHUM::srOpFailed;
	PR_TRACE((m_hips, prtIMPORTANT, "add as remote addresses:"));
	if(pZona->m_nSettings & cNetZone::fUptoGateway)
		opres = GenerateAddresses(&pZonaAddresses->m_vIP,fOnlyNet,hRuleAddresses);
	else
		opres = GenerateAddresses(&pZonaAddresses->m_vRoutes,fOnlyNet,hRuleAddresses);

	if(opres == CKAHUM::srOK)
		opres = CKAHFW::ApplicationRuleElement_SetRemoteAddresses(hElement, hRuleAddresses);
	if(hRuleAddresses)
		CKAHFW::RuleAddresses_Delete(hRuleAddresses);
	if (CKAHUM::srOK != opres)
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GenerateZoneAddresses error, zone_name=%S,CKAHUM::OpResult=%d",pZona->m_sFrendlyName.data(),opres));
	}
	return opres;
}
CKAHUM::OpResult CNetRulesManager::GeneratePortList(cFwPorts * LocalPorts,cFwPorts * RemotePorts,HPACKETRULE hRule)
{
	CKAHUM::OpResult opres = CKAHUM::srOK;
	CKAHFW::HRULEPORTS hRulePorts;
	if(LocalPorts->size()>0)
	{
		hRulePorts = CKAHFW::RulePorts_Create();
		if(!hRulePorts)
			opres = CKAHUM::srOpFailed;
		for(tDWORD PortIndex=0;PortIndex<LocalPorts->size() && opres == CKAHUM::srOK;PortIndex++)
		{
			opres = GeneratePort(LocalPorts->at(PortIndex).m_PortLo,
				LocalPorts->at(PortIndex).m_PortHi,hRulePorts);
		}
		if(opres == CKAHUM::srOK)
			opres = CKAHFW::PacketRule_SetTCPUDPLocalPorts(hRule, hRulePorts);
		if(hRulePorts)
			CKAHFW::RulePorts_Delete(hRulePorts);
	}
	if(RemotePorts->size()>0 && opres == CKAHUM::srOK)
	{
		hRulePorts = CKAHFW::RulePorts_Create();
		if(!hRulePorts)
			opres = CKAHUM::srOpFailed;
		for(tDWORD PortIndex=0;PortIndex<RemotePorts->size() && opres == CKAHUM::srOK;PortIndex++)
		{
			opres = GeneratePort(RemotePorts->at(PortIndex).m_PortLo,
				RemotePorts->at(PortIndex).m_PortHi,hRulePorts);
		}
		if(opres == CKAHUM::srOK)
			opres = CKAHFW::PacketRule_SetTCPUDPRemotePorts(hRule, hRulePorts);
		if(hRulePorts)
			CKAHFW::RulePorts_Delete(hRulePorts);
	}
	if (CKAHUM::srOK != opres)
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GeneratePortList error, opres=%d",opres));
	}
	return opres;
}
CKAHUM::OpResult CNetRulesManager::GenerateAddresses(cIpMasks * vIP ,eTargAddr ta,CKAHFW::HRULEADDRESSES hRuleAddresses)
{
	CKAHUM::IP ckah_IP;
	CKAHUM::IP ckah_Mask;
	cIpMask * ip;
	CKAHUM::OpResult opres = CKAHUM::srOK;
	HRULEADDRESS hRuleAddress_Net = 0;
	HRULEADDRESS hRuleAddress_Host = 0;

	//debug
	tCHAR str1[100];
	tCHAR str_net[1000]="";
	tCHAR str_host[1000]="";
	if(!vIP)
	{
		return opres;
	}
	//--
	for(tDWORD AddrIndex=0;AddrIndex<vIP->size() && opres == CKAHUM::srOK;AddrIndex++)
	{
		ip=&vIP->at(AddrIndex);
		if(!ip->IsValid())
		{
			PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GenerateAddresses error, IP is not valid :(."));
			continue;
		}
		if(ta==fOnlyHost || ip->m_Mask==0 || (ip->IsV4() && ip->m_Mask==32) || (ip->IsV6() &&  ip->m_Mask==128))
		{	
			if(!hRuleAddress_Host)
				hRuleAddress_Host = RuleAddress_Create(ratHost);
			if(!hRuleAddress_Host)
			{
				PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GenerateAddresses CKAHUM cant RuleAddress_Create(ratSubnet)"));
				opres = CKAHUM::srOpFailed;
				break;
			}
			if(ip->IsV4())
				ckah_IP.Setv4(ip->m_V4);
			if(ip->IsV6())
			{
				ckah_IP.v6.O.Hi=ip->m_V6.m_Hi;
				ckah_IP.v6.O.Lo=ip->m_V6.m_Lo;
				ckah_IP.v6.Zone=ip->m_V6.m_Zone;
			}
			//debug 
			ip->ToStr(str1,sizeof(str1));
			strncat_s(str_host,sizeof(str_host),str1,sizeof(str1));
			strncat_s(str_host,sizeof(str_host)," ",1);
			//--
			if(opres == CKAHUM::srOK)
				opres = RuleAddress_AddHostIP(hRuleAddress_Host,&ckah_IP);
			if(opres == CKAHUM::srOK && hRuleAddress_Host)
				opres = RuleAddresses_AddItemToBack(hRuleAddresses, hRuleAddress_Host);
			if(hRuleAddress_Host)
			{
				RuleAddress_Delete(hRuleAddress_Host);
				//PR_TRACE((m_hips, prtIMPORTANT, "add as hosts: %s",str_host));
				hRuleAddress_Host=0;
			}
		}
		else
		{
			if(!hRuleAddress_Net)
				hRuleAddress_Net = RuleAddress_Create(ratSubnet);
			if(!hRuleAddress_Net)
			{
				PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GenerateAddresses CKAHUM cant RuleAddress_Create(ratSubnet)"));
				opres = CKAHUM::srOpFailed;
				break;
			}
			if(ip->IsV4())
			{
				ckah_IP.Setv4(ip->GetV4Net());
				ckah_Mask.Setv4Mask(ip->m_Mask);
			}
			if(ip->IsV6())
			{
				ckah_IP.v6.O.Hi=ip->m_V6.m_Hi;
				ckah_IP.v6.O.Lo=ip->m_V6.m_Lo;
				ckah_IP.v6.Zone=ip->m_V6.m_Zone;	
				ckah_Mask.Setv6Mask(ip->m_Mask,ip->m_V6.m_Zone);
			}
			//debug 
			ip->ToStr(str1,sizeof(str1));
			strncat_s(str_net,sizeof(str_net),str1,sizeof(str1));
			strncat_s(str_net,sizeof(str_net)," ",1);
			//--
			if(opres == CKAHUM::srOK)
				opres = RuleAddress_SetAddressSubnet(hRuleAddress_Net,&ckah_IP,&ckah_Mask);
			if(opres == CKAHUM::srOK)
				opres = RuleAddresses_AddItemToBack(hRuleAddresses, hRuleAddress_Net);
			if(hRuleAddress_Net)
			{
				RuleAddress_Delete(hRuleAddress_Net);
				//PR_TRACE((m_hips, prtIMPORTANT, "add as net: %s",str_net));
				hRuleAddress_Net=0;
			}

		}
	}
	
	if (CKAHUM::srOK != opres)
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GenerateZoneSubnets error, opres=%d",opres));
	}
	if(str_net[0])
	{
		PR_TRACE((m_hips, prtIMPORTANT, "add as net: %s",str_net));
	}
	if(str_host[0])
	{
		PR_TRACE((m_hips, prtIMPORTANT, "add as hosts: %s",str_host));
	}
	
	return opres;
}
CKAHUM::OpResult CNetRulesManager::GenerateDirection(tBYTE Direction,HPACKETRULE hRule)
{
	CKAHUM::OpResult opres = CKAHUM::srOK;
	PacketDirection pd;
	StreamDirection sd;
	switch(Direction)
	{
	case cWebService::dInbound:         pd = pdIncoming; sd = sdBoth;     break;
	case cWebService::dOutbound:        pd = pdOutgoing; sd = sdBoth;     break;
	case cWebService::dInboundOutbound: pd = pdBoth;     sd = sdBoth;     break;
	case cWebService::dInboundStream:   pd = pdBoth;     sd = sdIncoming; break;
	case cWebService::dOutboundStream:  pd = pdBoth;     sd = sdOutgoing; break;
	default:
		opres = CKAHUM::srOpFailed;
	}
	if(opres == CKAHUM::srOK)
		opres = PacketRule_SetPacketDirection(hRule, pd);
	
	if(opres == CKAHUM::srOK)
		opres = PacketRule_SetStreamDirection(hRule, sd);
	if (CKAHUM::srOK != opres)
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GenerateZoneLocalAddress error, opres=%d",opres));
	}
	return opres;
}
CKAHUM::OpResult CNetRulesManager::GeneratePort(tDWORD PortLo,tDWORD PortHi, HRULEPORTS hRulePorts)
{
	CKAHUM::OpResult opres = CKAHUM::srOK;
	RulePortType PortType;
	
	if (PortHi == PortLo)
		PortType = rptSingle;
	else
		PortType = rptRange;

	HRULEPORT hRulePort = RulePort_Create(PortType);
	if (!hRulePort)
		opres = CKAHUM::srOpFailed;

	if (rptSingle == PortType && opres == CKAHUM::srOK)
		opres = RulePort_SetSinglePort(hRulePort, PortLo);
	else
		opres = RulePort_SetPortRange(hRulePort, PortLo, PortHi);
	if(opres == CKAHUM::srOK)
		opres = RulePorts_AddItemToBack(hRulePorts, hRulePort);
	if (hRulePort)
		RulePort_Delete(hRulePort);
	if (CKAHUM::srOK != opres)
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GeneratePort error, opres=%d",opres));
	}
	return opres;
}
void CALLBACK cbPacketRuleNotifyLink(LPVOID lpContext, const PacketRuleNotify *pNotify) {}
RuleCallbackResult CALLBACK cbAppRuleNotifyLink(LPVOID lpContext, const ApplicationRuleNotify *pNotify)
{
	return (RuleCallbackResult)0;
}
RuleCallbackResult CALLBACK cbCheckSumNotifyLink(LPVOID lpContext, const ChecksumChangedNotify *pNotify)
{
	return(RuleCallbackResult)0;
}
void CALLBACK ahfwTrace(CKAHUM::LogLevel Level, LPCSTR szString)
{
	return;
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
CKAHUM::OpResult CNetRulesManager::InitCKAHUM()
{
	
	CKAHUM::OpResult opres;
	cStringObj strManifest("%Bases%\\CKAH.set");
	PR_TRACE((m_hips, prtIMPORTANT, "CNetRulesManager::InitCKAHUM() with %S",strManifest.data()));
	opres = CKAHUM::Initialize(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH_W L"\\CKAHUM", ahfwTrace);
	if (CKAHUM::srOK != opres)
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::InitCKAHUM() CKAHUM::Initialize error, opres=%d",opres));
	}
	if (CKAHUM::srOK == opres)
	{
		m_hips->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strManifest), 0, 0);
		opres = CKAHUM::Reload(strManifest.data());
	}
	if (CKAHUM::srOK != opres)
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::InitCKAHUM() CKAHUM::Reload error, opres=%d strManifest=%S",opres,strManifest.data()));
	}
	if (CKAHUM::srOK == opres)
	{
		opres = CKAHFW::Start(cbPacketRuleNotifyLink, this,cbAppRuleNotifyLink, this,cbCheckSumNotifyLink, this);
	}
	if (CKAHUM::srOK != opres)
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::InitCKAHUM() CKAHFW::Start error, opres=%d",opres));
	}

/*
	CHipsSettings *pHIPSSettings;
	if(!m_settings.GetSettingsAndLock(&pHIPSSettings,0,0,0))
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::InitCKAHUM() Cant get HIPS settings."));
		return opres;
	}
	if (CKAHUM::srOK == opres)
	{
		m_FilteringMode=(CKAHFW::FilteringMode)pHIPSSettings->m_FirewallSettings.m_nFilteringMode;
		opres = SetFilteringMode(m_FilteringMode);
	}
	if (CKAHUM::srOK == opres)
	{
		m_WorkingMode=(CKAHFW::WorkingMode)pHIPSSettings->m_FirewallSettings.m_nWorkingMode;
		opres = SetWorkingMode((CKAHFW::WorkingMode)pHIPSSettings->m_FirewallSettings.m_nWorkingMode);
	}
	m_settings.Unlock();

*/
	return opres;
}

CKAHUM::OpResult CNetRulesManager::SetStopProcessFlag(CKAHFW::HPACKETRULE hRule)
{
	return CKAHFW::PacketRule_SetICMPCode(hRule, 164);
}