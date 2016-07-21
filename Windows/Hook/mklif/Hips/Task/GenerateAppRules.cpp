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
//--------------------------------------------------------------------------------------
void CNetRulesManager::ResetAppSeverityRule()
{
	PR_TRACE((m_hips, prtIMPORTANT, "ResetAppSeverityRule()"));
	m_settings.ClearAppSeverityRules();
	return;
}
//--------------------------------------------------------------------------------------
void CNetRulesManager::SetAppSeverityRule(wchar_t * l_pAppPath, tDWORD l_Severity, tDWORD l_AM)
{
	//PR_TRACE((g_root, prtIMPORTANT, "Add app rule for %S,%d,%d",l_pAppPath,l_Severity,l_AM));

	m_settings.AddAppSeverityRule(l_pAppPath,l_Severity,l_AM);
	return;
}
//--------------------------------------------------------------------------------------
void CNetRulesManager::ApplyAppSeverityRule()
{
	
	//return;
	if(QState()!=TASK_REQUEST_RUN)
		return;
	CAlock local_lock(&m_cs);
	CKAHUM::OpResult opres = CKAHUM::srOK;
	vAppSeverityRules *pAppSeverityRules;
	CHipsSettings     *pHIPSSettings;
	cNWSettings       *pNWsettings;
	cNWdata           *pNWdata;

	if(!m_settings.GetSettingsAndLock(&pHIPSSettings,&pNWsettings,&pNWdata,&pAppSeverityRules))
	{
		if(!pHIPSSettings || !NetWatcherSettingsChanged() || !m_settings.GetSettingsAndLock(&pHIPSSettings,&pNWsettings,&pNWdata,&pAppSeverityRules))
			return;
	}
	if((CKAHFW::WorkingMode)pHIPSSettings->m_FirewallSettings.m_nWorkingMode==CKAHFW::wmBlockAll || 
		(CKAHFW::WorkingMode)pHIPSSettings->m_FirewallSettings.m_nWorkingMode==CKAHFW::wmAllowAll)
	{
		m_settings.Unlock();
		return;
	}
	
	CKAHFW::HAPPRULES hAppRules = CKAHFW::ApplicationRules_Create();
	if(!hAppRules)
	{	
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::SetAppSeverityRule CKAHUM error, cant ApplicationRules_Create()."));
		m_settings.Unlock();
		return;
	}
	if(!(pHIPSSettings->m_FirewallSettings.m_nFlags & cFirewallSettings::FwAppRules))
	{
		CKAHFW::SetApplicationRules(hAppRules);
		CKAHFW::ApplicationRules_Delete(hAppRules);
		m_settings.Unlock();
		return;
	};
	cStrObj AppPath;
	tDWORD AppSeverity,AppAM;
	for(tDWORD AppRuleIndex=0;AppRuleIndex<pAppSeverityRules->size()&& opres == CKAHUM::srOK;AppRuleIndex++)
	{
		AppPath=pAppSeverityRules->at(AppRuleIndex).m_sAppPath;
		AppSeverity=pAppSeverityRules->at(AppRuleIndex).m_nSeverity;
		AppAM=pAppSeverityRules->at(AppRuleIndex).m_nAM;
		PR_TRACE((m_hips, prtIMPORTANT, "Generate apps rule for %S,%d,%d",AppPath.data(),AppSeverity,AppAM));
	
		for(int ZoneIndex=pNWsettings->m_zones.size()-1;ZoneIndex>=0 && opres == CKAHUM::srOK;ZoneIndex--)
		{
			
			cNetZone * pZona=&pNWsettings->m_zones[ZoneIndex];
			if(!pZona->Connected())
				continue;
			cZonaAddresses * pZonaAddresses=pNWdata->FindZoneByID(pZona->m_nID);
			if(pZona->m_nID!=0 && !pZonaAddresses)
			{
				PR_TRACE((m_hips, prtERROR, "CNetRulesManager::SetAppSeverityRule NetWatcher settings error, cant find ZonaAddresses with zone ID=%d.",pZona->m_nID));
				continue;
			}
			if(pZona->m_nSeverity!=AppSeverity)
				continue;
			PR_TRACE((m_hips, prtIMPORTANT, "Generate rule on zone %S",pZona->m_sFrendlyName.data()));
			CKAHFW::HAPPRULE hRule = CKAHFW::ApplicationRule_Create();
			if(!hRule)
			{
				opres = CKAHUM::srOpFailed; 
				PR_TRACE((m_hips, prtERROR, "CNetRulesManager::SetAppSeverityRule CKAHUM error, cant ApplicationRule_Create();."));
				break;
			}
			if(opres == CKAHUM::srOK)
				opres = CKAHFW::ApplicationRule_SetIsCommandLine(hRule,false);
			if(opres == CKAHUM::srOK)
				opres = CKAHFW::ApplicationRule_SetApplicationName(hRule,AppPath.data());
			if(opres == CKAHUM::srOK)
				opres = CKAHFW::ApplicationRule_SetCheckHash (hRule,false);
			if(opres == CKAHUM::srOK)
				opres = CKAHFW::ApplicationRule_SetIsEnabled(hRule, true);
			if(opres == CKAHUM::srOK)
			{
				if(HIPS_GET_AM(AppAM,0)==3)
				{
					opres = CKAHFW::ApplicationRule_SetIsBlocking(hRule, true);
					PR_TRACE((m_hips, prtIMPORTANT, "Generate blocking rule on zone %S",pZona->m_sFrendlyName.data()));
				}
				else
				{
					opres = CKAHFW::ApplicationRule_SetIsBlocking(hRule, false);
					PR_TRACE((m_hips, prtIMPORTANT, "Generate allow rule on zone %S",pZona->m_sFrendlyName.data()));
				}
			}
			if(opres == CKAHUM::srOK)
			{
				CKAHFW::HAPPRULEELEMENT hRuleElement = CKAHFW::ApplicationRuleElement_Create();
				if (hRuleElement)
				{
					if(pZona->m_nID!=0 && opres == CKAHUM::srOK)
					{
						if(pZona->m_nSettings & cNetZone::fUptoGateway ||
							(!(pZona->m_nSettings & cNetZone::fUptoGateway) && !pZonaAddresses->m_IsDefaultGateway))
						opres = GenerateAddressesByZoneApp(pZona,pZonaAddresses,hRuleElement);
					}
				}
				if (CKAHUM::srOK == opres)
				{
					CKAHFW::ApplicationRuleElement_SetIsActive(hRuleElement,true);
					opres = CKAHFW::ApplicationRule_AddElementToBack(hRule, hRuleElement);
				}
				if (hRuleElement)
					CKAHFW::ApplicationRuleElement_Delete(hRuleElement);
				
			}
			if(opres == CKAHUM::srOK)
			{
				if(pZona->m_nID!=0 && (pZona->m_nSettings & cNetZone::fUptoGateway ||
							(!(pZona->m_nSettings & cNetZone::fUptoGateway) && !pZonaAddresses->m_IsDefaultGateway)))
					opres = CKAHFW::ApplicationRules_AddItemToFront(hAppRules, hRule);
				else
					opres = CKAHFW::ApplicationRules_AddItemToBack(hAppRules, hRule);
			}
			if(hRule)
				CKAHFW::ApplicationRule_Delete(hRule);
			if (CKAHUM::srOK != opres)
			{
				PR_TRACE((m_hips, prtERROR, "CNetRulesManager::SetAppSeverityRule error, opres=%d",opres));
			}
			
		}
	}
	if(opres == CKAHUM::srOK)
	{
		opres=CKAHFW::SetApplicationRules(hAppRules);
	}
	else
	{
		PR_TRACE((m_hips, prtERROR, "CNetRulesManager::ApplyAppSeverityRule CKAHUM error, cant SetApplicationRules(m_hAppRules) opres=",opres));
	}
	if(hAppRules)
		CKAHFW::ApplicationRules_Delete(hAppRules);
	m_settings.Unlock();
	return;
}


void CNetRulesManager::GenerateAppRules()
{
	//ResetAppSeverityRule();
	//cStrObj AppNameTmp("C:\\tst\\telnet.exe");
	//SetAppSeverityRule((wchar_t *)AppNameTmp.data(),2,0);
	ApplyAppSeverityRule();
}