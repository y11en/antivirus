#include "NetRulesManager.h"
//#include "../NetWatch/cnetwatchertask.h"
//#include <Windows.h>
#include <NetWatch/structs/s_netwatch.h>
#include "../../CommonFiles\Version\ver_product.h"        //product version definitions

#include <CKAH/CKAHUM.h>
#include <CKAH/CKAHSTAT.h>
//using namespace CKAHFW;
#include <CKAH/CKAHPorts.h>
#include <CKAH/ckahrules.h>
#include <CKAH/ckahiptoa.h>

#include <AHTasks/iface/i_ipresolver.h>
#include <AHTasks/plugin/p_resip.h>
#include <CKAH/ipconv.h>

void CNetRulesManager::GeneratePacketRules()
{
	//return;
	if(QState()!=TASK_REQUEST_RUN)
		return;
	CAlock local_lock(&m_cs);
	CHipsSettings *pHIPSSettings;
	cNWSettings * pNWsettings;
	cNWdata *	pNWdata;
	if(!m_settings.GetSettingsAndLock(&pHIPSSettings,&pNWsettings,&pNWdata,0))
	{
		if(!pHIPSSettings || !NetWatcherSettingsChanged() || !m_settings.GetSettingsAndLock(&pHIPSSettings,&pNWsettings,&pNWdata,0))
			return;
	}
	CKAHUM::OpResult opres = CKAHUM::srOK;

	if(m_FilteringMode!=(CKAHFW::FilteringMode)pHIPSSettings->m_FirewallSettings.m_nFilteringMode)
	{
		m_FilteringMode=(CKAHFW::FilteringMode)pHIPSSettings->m_FirewallSettings.m_nFilteringMode;
		opres = CKAHFW::SetFilteringMode(m_FilteringMode);
		PR_TRACE((m_hips, prtIMPORTANT, "CNetRulesManager::GeneratePacketRules Set FilteringMode=%d,opres=%d",m_FilteringMode,opres));
	}
	if(m_WorkingMode!=(CKAHFW::WorkingMode)pHIPSSettings->m_FirewallSettings.m_nWorkingMode)
	{
		m_WorkingMode=(CKAHFW::WorkingMode)pHIPSSettings->m_FirewallSettings.m_nWorkingMode;
		opres = CKAHFW::SetWorkingMode(m_WorkingMode);
		PR_TRACE((m_hips, prtIMPORTANT, "CNetRulesManager::GeneratePacketRules Set WorkingMode=%d,opres=%d",m_WorkingMode,opres));
	}
	if((CKAHFW::WorkingMode)pHIPSSettings->m_FirewallSettings.m_nWorkingMode==CKAHFW::wmBlockAll || 
		(CKAHFW::WorkingMode)pHIPSSettings->m_FirewallSettings.m_nWorkingMode==CKAHFW::wmAllowAll)
	{
		m_settings.Unlock();
		return;
	}
	CKAHFW::HPACKETRULES hRules =CKAHFW::PacketRules_Create();
	if(!hRules)
	{	
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GeneratePacketRules CKAHUM error, cant PacketRules_Create()."));
		m_settings.Unlock();
		return;
	}
	if(!(pHIPSSettings->m_FirewallSettings.m_nFlags & cFirewallSettings::FwPacketRules))
	{
		CKAHFW::SetPacketRules(hRules);
		CKAHFW::PacketRules_Delete(hRules);
		m_settings.Unlock();
		return;
	};
	for(int ZoneIndex=pNWsettings->m_zones.size()-1;ZoneIndex>=0 && opres == CKAHUM::srOK;ZoneIndex--)
	//for(DWORD ZoneIndex=0;ZoneIndex<pNWsettings->m_zones.size() && opres == CKAHUM::srOK;ZoneIndex++)
	{
		cNetZone * pZona=&pNWsettings->m_zones[ZoneIndex];
		if(!pZona->Connected())
			continue;
		cZonaAddresses * pZonaAddresses=pNWdata->FindZoneByID(pZona->m_nID);
		if(pZona->m_nID!=0 && !pZonaAddresses)
		{
			PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GeneratePacketRules NetWatcher settings error, cant find ZonaAddresses with zone ID=%d.",pZona->m_nID));
			continue;
		}		
		if(pZona->m_nTypeOfInterface==If_LoopBack && pHIPSSettings->m_FirewallSettings.m_nFlags & cFirewallSettings::FwLocalhostAllowAll)
		{
			GenerateLocalhostRule(pZona,pZonaAddresses,hRules);
			continue;
		}
		if(pHIPSSettings->m_FirewallSettings.m_nFlags & cFirewallSettings::FwBlockAll)
			continue;
		if(pZona->m_nID==0 && pHIPSSettings->m_FirewallSettings.m_nFlags & cFirewallSettings::FwBlockInternet)
			continue;
		PR_TRACE((m_hips, prtIMPORTANT, "CNetRulesManager::GeneratePacketRules Creating rules for %S ...",pZona->m_sFrendlyName.data()));
		for(int PriIndex=0;PriIndex<3;PriIndex++)
		{
			tDWORD severity=pZona->m_nSeverity;
			if(PriIndex==0)
			{
				if(pZona->m_nSettings & cNetZone::fAllNetsFirst)
					severity=0;
				else
					continue;
			}
			if(PriIndex==2)
			{
				if(pZona->m_nSettings & cNetZone::fAllNetsLast)
					severity=0xffff;
				else
					continue;
			}
			PR_TRACE((m_hips, prtIMPORTANT, "CNetRulesManager::GeneratePacketRules Creating rules with severity=%d",severity));
			for(tDWORD RuleIndex=0;RuleIndex<pHIPSSettings->m_PacketRules.size() && opres == CKAHUM::srOK;RuleIndex++)
			{
				CHipsPacketRule * pHipsPacketRule=&pHIPSSettings->m_PacketRules.at(RuleIndex);
				if(pHipsPacketRule->m_Severity!=severity || pHipsPacketRule->m_RuleState==0)
					continue;
				cResource *pRes=pHIPSSettings->m_Resource.Find(pHipsPacketRule->m_ResIdEx);
				if(!pRes)
				{
					PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GeneratePacketRules HIPSSettings error, cant find Resource with ID=%d.",pHIPSSettings->m_PacketRules[RuleIndex].m_ResIdEx));
					break;
				}
				opres = GeneratePacketRuleByHipsPacketRule(pZona,pZonaAddresses,pHipsPacketRule,pRes,hRules);
			}
		}
		//stealth rule
		if(pZona->m_nSettings & cNetZone::fStealth)
		{
			PR_TRACE((m_hips, prtIMPORTANT, "CNetRulesManager::GeneratePacketRules Creating stealth rules for %S ...",pZona->m_sFrendlyName.data()));
			for(tDWORD ruleIndex=0;ruleIndex<3 && opres == CKAHUM::srOK;ruleIndex++)
			{
				tBYTE proto;
				CKAHFW::HPACKETRULE hRule = CKAHFW::PacketRule_Create();
				if(hRule)
				{
					if(ruleIndex==0)
						proto=CKAHFW::PROTO_TCP;
					if(ruleIndex==1)
						proto=CKAHFW::PROTO_UDP;
					if(ruleIndex==2)
					{
						proto=CKAHFW::PROTO_ICMP;
						opres = CKAHFW::PacketRule_SetICMPType(hRule,8);
						if(opres == CKAHUM::srOK)
							opres = PacketRule_SetPacketDirection(hRule,CKAHFW::pdIncoming);
					}
					else
					{
						if(opres == CKAHUM::srOK)
							opres = PacketRule_SetStreamDirection(hRule,CKAHFW::sdIncoming);
					}
					if(opres == CKAHUM::srOK)
						opres = CKAHFW::PacketRule_SetProtocol(hRule,proto);
					if(opres == CKAHUM::srOK)
						opres = CKAHFW::PacketRule_SetIsEnabled(hRule, true);
					if(pZona->m_nID!=0 && opres == CKAHUM::srOK)
						GenerateAddressesByZone(pZona,pZonaAddresses,hRule);
					if(opres == CKAHUM::srOK)
						opres = CKAHFW::PacketRule_SetIsBlocking(hRule,true);
					if (CKAHUM::srOK == opres)
						opres = CKAHFW::PacketRules_AddItemToBack(hRules, hRule);
					else
					{
						PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GeneratePacketRules CKAHUM error, opres=%d\n", opres));
					}
					CKAHFW::PacketRule_Delete(hRule);
				}
				else
					break;
			}

		}
		//last STOP_PROCESSING rule
		if(pZona->m_nID!=0)
		{
			CKAHFW::HPACKETRULE hRule = CKAHFW::PacketRule_Create();
			if(hRule)
			{
				PR_TRACE((m_hips, prtIMPORTANT, "CNetRulesManager::GeneratePacketRules Last STOP_PROCESSING rule for zone %S.",pZona->m_sFrendlyName.data()));
				opres = CKAHFW::PacketRule_SetName(hRule, pZona->m_sFrendlyName.data());
				if(opres == CKAHUM::srOK)
					opres = CKAHFW::PacketRule_SetProtocol(hRule,CKAHFW::PROTO_ALL);
				if(opres == CKAHUM::srOK)
					opres = SetStopProcessFlag(hRule);
				if(opres == CKAHUM::srOK)
					opres = PacketRule_SetPacketDirection(hRule, CKAHFW::pdBoth);
				if(opres == CKAHUM::srOK)
					opres = PacketRule_SetStreamDirection(hRule, CKAHFW::sdBoth);
				if(pZona->m_nID!=0 && opres == CKAHUM::srOK)
					GenerateAddressesByZone(pZona,pZonaAddresses,hRule);
				if (CKAHUM::srOK == opres)
					opres = CKAHFW::PacketRules_AddItemToBack(hRules, hRule);
				CKAHFW::PacketRule_Delete(hRule);
			}
		}
	}
	if(pHIPSSettings->m_FirewallSettings.m_nFlags & cFirewallSettings::FwBlockAll ||
	   pHIPSSettings->m_FirewallSettings.m_nFlags & cFirewallSettings::FwBlockInternet)
		opres=GenerateBlockAllRule(hRules);
	if (CKAHUM::srOK == opres)
		CKAHFW::SetPacketRules(hRules);
	else
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GeneratePacketRules CKAHUM error, opres=%d\n", opres));
	}
	CKAHFW::PacketRules_Delete(hRules);
	m_settings.Unlock();
}
CKAHUM::OpResult CNetRulesManager::GenerateLocalhostRule(cNetZone * pZona,cZonaAddresses * pZonaAddresses,CKAHFW::HPACKETRULES hRules)
{
	PR_TRACE((m_hips, prtIMPORTANT, "CNetRulesManager::GeneratePacketRules Creating allow all rule for loopback."));
	CKAHUM::OpResult opres = CKAHUM::srOK;
	CKAHFW::HPACKETRULE hRule = CKAHFW::PacketRule_Create();
	if(!hRule)
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GenerateLocalhostRule CKAHUM error, cant PacketRule_Create()."));
		return CKAHUM::srOpFailed;
	}
	if(opres == CKAHUM::srOK)
		opres = CKAHFW::PacketRule_SetIsBlocking(hRule,false);
	if(opres == CKAHUM::srOK)
		GenerateAddressesByZone(pZona,pZonaAddresses,hRule);
	if(opres == CKAHUM::srOK)
		opres = CKAHFW::PacketRules_AddItemToBack(hRules, hRule);
	if(opres != CKAHUM::srOK)
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GenerateLocalhostRule error opres=%d",opres));
	}
	CKAHFW::PacketRule_Delete(hRule);
	return opres;

}
CKAHUM::OpResult CNetRulesManager::GeneratePacketRuleByHipsPacketRule(cNetZone * pZona,cZonaAddresses * pZonaAddresses,CHipsPacketRule * pHipsPacketRule,cResource *pRes,CKAHFW::HPACKETRULES hRules)
{
	CKAHUM::OpResult opres = CKAHUM::srOK;
	cWebService * pWebService=(cWebService *)(cSer *)pRes->m_pData;
	if( !pWebService || !pWebService->isBasedOn(cWebService::eIID))
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GeneratePacketRuleByHipsPacketRule HIPSSettings error, cant find pWebService with Resource name=%S.",pRes->m_sName.data()));
		return CKAHUM::srOpFailed; 
	}
	CKAHFW::HPACKETRULE hRule = CKAHFW::PacketRule_Create();
	if(!hRule)
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GeneratePacketRuleByHipsPacketRule CKAHUM error, cant PacketRule_Create()."));
		return CKAHUM::srOpFailed;
	}

	if(HIPS_GET_AM(pHipsPacketRule->m_AccessFlag,0)==3)
	{
		PR_TRACE((m_hips, prtIMPORTANT, "CNetRulesManager::GeneratePacketRuleByHipsPacketRule Create blocking rule with service %S",pRes->m_sName.data()));
		opres = CKAHFW::PacketRule_SetIsBlocking(hRule,true);
	}
	if(HIPS_GET_AM(pHipsPacketRule->m_AccessFlag,0)==0)
	{
		PR_TRACE((m_hips, prtIMPORTANT, "CNetRulesManager::GeneratePacketRuleByHipsPacketRule Create allow rule with service %S",pRes->m_sName.data()));
		opres = CKAHFW::PacketRule_SetIsBlocking(hRule,false);
	}
	if(HIPS_GET_AM(pHipsPacketRule->m_AccessFlag,0)==2)
	{
		PR_TRACE((m_hips, prtIMPORTANT, "CNetRulesManager::GeneratePacketRuleByHipsPacketRule Create skip rule with service %S",pRes->m_sName.data()));
		opres = SetStopProcessFlag(hRule);
	}
	
	if(opres == CKAHUM::srOK)
		opres = CKAHFW::PacketRule_SetName(hRule, pRes->m_sName.data());			
	if(opres == CKAHUM::srOK)
		GenerateRuleByWebService(pWebService,hRule);
	if(pZona->m_nID!=0 && opres == CKAHUM::srOK)
		GenerateAddressesByZone(pZona,pZonaAddresses,hRule);
	if(opres == CKAHUM::srOK)
		opres = CKAHFW::PacketRules_AddItemToBack(hRules, hRule);
	if(opres != CKAHUM::srOK)
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::GeneratePacketRuleByHipsPacketRule error, zone_name=%S,resource name=%S,opres=%d",pZona->m_sFrendlyName.data(),pRes->m_sName.data(),opres));
	}
	CKAHFW::PacketRule_Delete(hRule);
	return opres;

}
CKAHUM::OpResult CNetRulesManager::GenerateBlockAllRule(CKAHFW::HPACKETRULES hRules)
{
	CKAHUM::OpResult opres = CKAHUM::srOK;
	CKAHFW::HPACKETRULE hRule = CKAHFW::PacketRule_Create();
	if(hRule)
	{
		PR_TRACE((m_hips, prtIMPORTANT, "CNetRulesManager Block All Rule."));
		if(opres == CKAHUM::srOK)
			opres = CKAHFW::PacketRule_SetIsBlocking(hRule,true);
		if(opres == CKAHUM::srOK)
			opres = CKAHFW::PacketRule_SetProtocol(hRule,CKAHFW::PROTO_ALL);
		if(opres == CKAHUM::srOK)
			opres = PacketRule_SetPacketDirection(hRule, CKAHFW::pdBoth);
		if(opres == CKAHUM::srOK)
			opres = PacketRule_SetStreamDirection(hRule, CKAHFW::sdBoth);
		if (CKAHUM::srOK == opres)
			opres = CKAHFW::PacketRules_AddItemToBack(hRules, hRule);
		CKAHFW::PacketRule_Delete(hRule);
	}
	return opres;
}