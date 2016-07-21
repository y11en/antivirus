#include "NetRulesManager.h"
#include <NetWatch/structs/s_netwatch.h>
void   CNetRMSettings::AddAppSeverityRule(wchar_t * l_pAppPath, DWORD l_Severity, DWORD l_AM)
{
	Lock();
	PR_TRACE((g_root, prtIMPORTANT, "Add app rule for %S,%d,%d",l_pAppPath,l_Severity,l_AM));
	sAppSeverityRule * pAppSeverityRule=&m_AppSeverityRules.push_back();
	pAppSeverityRule->m_sAppPath=l_pAppPath;
	pAppSeverityRule->m_nSeverity=l_Severity;
	pAppSeverityRule->m_nAM=l_AM;
	Unlock();
}

tBOOL CNetRMSettings::Changed()
{
	Lock();
	tBOOL x=m_changed;
	Unlock();
	return x;
}
tDWORD CNetRMSettings::SetHIPSSettings(CHipsSettings *param)
{
	Lock();
	m_changed=true;
	param->copy(m_HIPSSettings_s,0);
	PR_TRACE((g_root, prtIMPORTANT, "CNetRMSettings::SetHIPSSettings m_HIPSSettings_s.m_NetworkState=%d",m_HIPSSettings_s.m_NetworkState));
	switch(m_HIPSSettings_s.m_NetworkState)
	{
		case 0:
			m_HIPSSettings_s.m_FirewallSettings.m_nWorkingMode=CKAHFW::wmAllowAll;
			//m_HIPSSettings_s.m_FirewallSettings.m_State=TASK_REQUEST_STOP;
			break;
		case 1:
			m_HIPSSettings_s.m_FirewallSettings.m_nWorkingMode=CKAHFW::wmBlockAll;
			break;
		case 2:
			m_HIPSSettings_s.m_FirewallSettings.m_nWorkingMode=CKAHFW::wmAllowNotFiltered;
			break;
	}
	Unlock();
	return 1;
}

tDWORD CNetRMSettings::SetNWsettings(cNWSettings * param)
{
	Lock();
	if(m_changed || m_NWsettings_s.m_zones!=param->m_zones ||  
		m_NWsettings_s.m_adapters!=param->m_adapters)
	{
		param->copy(m_NWsettings_s,0);
		m_changed=true;
	}
	else
		m_changed=false;
	Unlock();
	return 1;
}

tDWORD CNetRMSettings::SetNWdata(cNWdata * param)
{
	Lock();
	if(m_changed || m_NWdata_s!=*param)
	{
		param->copy(m_NWdata_s,0);
		m_changed=true;
	}
	else
		m_changed=false;
	Unlock();
	return 1;
}

tTaskRequestState CNetRMSettings::GetSettingsState()
{
	Lock();
	tTaskRequestState res=(tTaskRequestState) m_HIPSSettings_s.m_FirewallSettings.m_State;
	Unlock();
	return res;

}

tBOOL CNetRMSettings::GetSettingsAndLock(CHipsSettings **HIPSsettings,cNWSettings **NWsettings,cNWdata **NWdata,vAppSeverityRules ** AppSeverityRules)
{
	Lock();
	tBOOL result=true;
	if(HIPSsettings)
	{
		if(m_HIPSSettings_s.m_PacketRules.size()!=0)
			*HIPSsettings=&m_HIPSSettings_s;
		else
		{
			result=false;
			*HIPSsettings=0;
		}
	}
	if(NWsettings)
	{
		if(m_NWdata_s.m_ZonesAddresses.size()!=0)
			*NWsettings=&m_NWsettings_s;
		else
		{	
			result=false;
			*NWsettings=0;
		}
	}

	if(NWdata)
	{
		if(m_NWdata_s.m_ZonesAddresses.size()!=0)
				*NWdata=&m_NWdata_s;
		else 
		{
			result=false;
			*NWdata=0;
		}
	}

	if(AppSeverityRules)
		*AppSeverityRules=&m_AppSeverityRules;
	if(result==false)
	{
		Unlock();
		return false;
	}
	return true;
}


CNetRMSettings::CNetRMSettings()
{
	m_changed=false;
	InitializeCriticalSection(&m_cs);
}

CNetRMSettings::~CNetRMSettings()
{
	DeleteCriticalSection(&m_cs);
}
void inline CNetRMSettings::Lock ()
{
    EnterCriticalSection(&m_cs);
}

void inline CNetRMSettings::Unlock ()
{
    LeaveCriticalSection(&m_cs);
}