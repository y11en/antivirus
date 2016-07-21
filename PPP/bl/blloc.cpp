// -------------------------------------------

#include "bl.h"
#include <iface/i_token.h>

#include "Holders.h"

extern tPTR g_hModule;

#if defined (_WIN32)
#include "blmsg.h"
#include <lm.h>
#elif defined (__unix__)
#include <unistd.h>
#define GetComputerNameA(a,b) gethostname (a,*b)
#define GetModuleFileName(a,b,c)
#define RegisterEventSource(a,b) 0
#define DeregisterEventSource(a)
#define ReportEvent(a,b,c,d,e,f,g,h,i)
#define MSG_ERROR 0
#define EVENTLOG_ERROR_TYPE 0
#define EVENTLOG_WARNING_TYPE 1
#define EVENTLOG_INFORMATION_TYPE 2
#warning "Consideration is needed!"
#endif
// -------------------------------------------

bool cBlLoc::init(bool bPreInit)
{
	if( bPreInit )
	{
		tCHAR szMsgDll[MAX_PATH] = "";
		GetModuleFileName((HINSTANCE)g_hModule, szMsgDll, sizeof(szMsgDll));
		m_strPath = szMsgDll;
		m_bl->sysCreateObjectQuick((hOBJECT*)&m_lock, IID_CRITICAL_SECTION);
		m_hEventLog = NULL;
		m_nLastEventTime = 0;
		return true;
	}

	cAutoCS cs(m_lock, true);
	
	if( m_bFailed )
		return false;

	if( CreateAndInitRoot(g_root, GUIFLAG_CFGSTG_RO, this, NULL, false) )
		return true;

	m_bFailed = cTRUE;
	return false;
}

bool cBlLoc::done()
{
	clear_caches(true);

	Unload();

	if( m_hEventLog )
	{
		DeregisterEventSource(m_hEventLog);
		m_hEventLog = NULL;
	}
	return true;
}

void cBlLoc::clear_caches(bool bAll)
{
	cAutoCS cs(m_lock, true);
	if( bAll )
	{
		m_mapEventNames.clear();
		m_mapProfileNames.clear();
	}
	else if( PrGetTickCount() - m_nLastEventTime < 3000 )
		return;
	
	for(tEventBindings::iterator it = m_mapEventBindings.begin(); it != m_mapEventBindings.end(); it++)
	{
		tEventBinding &eb = it->second;
		eb.first->Destroy();
		eb.second->Destroy();
	}
	m_mapEventBindings.clear();
}

bool cBlLoc::send_mail(CSendMailInfo& pInfo)
{
	if( init() && PR_SUCC(m_pRoot->SendMail(pInfo)) )
		return true;

	return false;
}

bool cBlLoc::get_mail_header(cStrObj& strText)
{
	if( !init() )
		return false;

	tString strProductInfo; m_pRoot->GetFormatedText(strProductInfo, m_pRoot->GetString(tString(), PROFILE_LOCALIZE, NULL, "ProductNameDetailed"), (cSerializable *)NULL);
	tString strOSVer; m_pRoot->GetSysVersion(strOSVer);
	
	cStrObj strComputer;
	{
		tCHAR szCompName[512] = {0};
		tULONG dwSize = countof(szCompName);
		GetComputerNameA(szCompName, &dwSize);

		if( szCompName[0] )
			strComputer.format(cCP_ANSI, "Computer: %s\r\n", szCompName);
	}

	cStrObj strDomain;
#if defined (_WIN32)
	{
		if( HINSTANCE hNetApi = ::LoadLibrary("netapi32.dll") )
		{
			typedef NET_API_STATUS (WINAPI f_NetWkstaGetInfo)(LPWSTR servername, DWORD level, LPBYTE * bufptr);
			typedef NET_API_STATUS (WINAPI f_NetApiBufferFree)(LPVOID Buffer);
			
			f_NetWkstaGetInfo * pNetWkstaGetInfo = (f_NetWkstaGetInfo *)::GetProcAddress(hNetApi, "NetWkstaGetInfo");
			f_NetApiBufferFree * pNetApiBufferFree = (f_NetApiBufferFree *)::GetProcAddress(hNetApi, "NetApiBufferFree");
			
			if( pNetWkstaGetInfo && pNetApiBufferFree )
			{
				WKSTA_INFO_100 * pWkstaInfo = NULL;
				if( pNetWkstaGetInfo(NULL, 100, (LPBYTE *)&pWkstaInfo) == NERR_Success )
				{
					if( pWkstaInfo )
					{
						if( pWkstaInfo->wki100_langroup && *((LPWSTR)pWkstaInfo->wki100_langroup) )
							strDomain.format(cCP_ANSI, "Domain: %S\r\n", pWkstaInfo->wki100_langroup);
						pNetApiBufferFree(pWkstaInfo);
					}
				}
			}
			
			::FreeLibrary(hNetApi);
		}
	}
#endif	
	strText.format(cCP_ANSI,
		"Product: %s\r\n"
		"Operation system: %s\r\n"
		"%S"
		"%S"
		"\r\nNotifications:\r\n",
		strProductInfo.c_str(), strOSVer.c_str(), strComputer.data(), strDomain.data());
	
	if( m_strMailSubject.empty() )
		m_strMailSubject = "Kaspersky Anti-Virus notification";
	return true;
}

bool cBlLoc::get_event_text(cBLNotification& pNotification, cStrObj& strText, cStrObj* strType)
{
/*	if( !init() )
		return false;
	
	if( m_bl->m_bIsStopping )
		return false;

	cAutoCS cs(m_lock, true);

	if( pNotification.m_pData )
	{
		tDWORD nSerId = pNotification.m_pData->getIID();

		tEventBindings::iterator it = m_mapEventBindings.find(nSerId);
		if( it == m_mapEventBindings.end() )
		{
			CFieldsBinder *pBinder = m_pRoot->CreateFieldsBinder();
			pBinder->AddDataSource(&pNotification);
			pBinder->AddDataSource(pNotification.m_pData);

			cNode *pNode = m_pRoot->CreateNode(m_pRoot->GetString(tString(), PROFILE_LOCALIZE, NULL, "ProductNotificationSection"), pBinder);

			strText = m_pRoot->GetFormatedText(tString(), pNode);

			pBinder->ClearSources(false);

			m_mapEventBindings.insert(tEventBindings::value_type(nSerId, tEventBinding(pNode, pBinder)));
		}
		else
		{
			tEventBinding &eb = it->second;
			cNode *pNode = eb.first;
			CFieldsBinder *pBinder = eb.second;

			pBinder->AddDataSource(&pNotification);
			pBinder->AddDataSource(pNotification.m_pData);

			strText = m_pRoot->GetFormatedText(tString(), pNode);
			
			pBinder->ClearSources(false);
		}
		m_nLastEventTime = PrGetTickCount();
	}

	if( strType )
	{
		tEventID eventID(pNotification.m_EventID, pNotification.m_Severity);
		tEventNames::iterator it = m_mapEventNames.find(eventID);
		if( it == m_mapEventNames.end() )
		{
			cStrObj str; m_pRoot->GetFormatedText(str, "$(EventID,EventTList)", &pNotification);
			it = m_mapEventNames.insert(tEventNames::value_type(eventID, str.data())).first;
		}
		*strType = it->second.c_str();
	}
*/	return true;
}

bool cBlLoc::get_profile_name(cSerString& strText)
{
	if( !init() )
		return false;

	if( m_bl->m_bIsStopping )
		return false;

	cAutoCS cs(m_lock, true);

	cStrObj &strProfileId = strText.m_str;
	tProfileNames::iterator it = m_mapProfileNames.find(strProfileId.data());
	if( it == m_mapProfileNames.end() )
	{
		cProfileBase pProfile;
		if( PR_FAIL(m_bl->m_tm->GetProfileInfo(strProfileId.c_str(cCP_ANSI), &pProfile)) )
		{
			if( strProfileId != AVP_PROFILE_SELFPROT &&
				strProfileId != AVP_PROFILE_REGGUARD2 &&
				strProfileId != AVP_PROFILE_AIC2 )
				return false;
			pProfile.m_sName = strProfileId;
		}
		
		if( !pProfile.m_sDescription.empty() )
		{
			strProfileId = pProfile.m_sDescription;
			return true;
		}

		cStrObj str; m_pRoot->GetFormatedText(str, m_pRoot->LoadLocString(tString(), "ProfileDescription"), &pProfile);
		it = m_mapProfileNames.insert(tProfileNames::value_type(strProfileId.data(), str.data())).first;
	}
	strProfileId = it->second.c_str();
	return true;
}

bool cBlLoc::localize_info(cSerializable* pInfo)
{
/*	if( !pInfo || !init() )
		return false;

	if( pInfo->isBasedOn(cBLLocalizedNotification::eIID) )
	{
		cBLLocalizedNotification* pNotification = (cBLLocalizedNotification*)pInfo;
		
		get_event_text(*pNotification, pNotification->m_sEventText, &pNotification->m_sEventType);

		cSerString strProfileName;
		cSerializable *pData = pNotification->m_pData;
		if( pData && pData->isBasedOn(cAskObjectAction_REG::eIID) )
			strProfileName.m_str = AVP_PROFILE_REGGUARD2;
		else
		if( pData && pData->isBasedOn(cAskObjectAction_PDM_A::eIID) )
		{
			cAskObjectAction_PDM_A &action = *(cAskObjectAction_PDM_A *)pData;
			if( action.m_EventType == REPORT_EVENT_APPG || 
				action.m_EventType == REPORT_EVENT_APPG_CHANGED ||
				action.m_EventType == REPORT_EVENT_IMG_CHANGED ||
				action.m_EventType == REPORT_EVENT_APPG_ASCHILD
				)
				strProfileName.m_str = AVP_PROFILE_AIC2;
			else
				strProfileName.m_str = AVP_PROFILE_PDM;
		}
		else
		if( pData && pData->isBasedOn(cTaskHeader::eIID) )
			strProfileName.m_str = ((cTaskHeader *)pData)->m_strProfile;
		else
		if( pData && pData->isBasedOn(cProfileBase::eIID) )
			strProfileName.m_str = ((cProfileBase *)pData)->m_sName;
		else
			strProfileName.m_str = AVP_PROFILE_PROTECTION;

		if( get_profile_name(strProfileName) )
			pNotification->m_sTaskName = strProfileName.m_str;
	}
*/	return true;
}

void cBlLoc::init_event_log(bool bSystem)
{
#if defined (_WIN32)
	tCHAR  szBuf[MAX_PATH];
	const tCHAR* szLogName = bSystem ? "Application" : "Kaspersky Event Log";
	const tCHAR* szAppName = bSystem ? VER_PRODUCTNAME_STR : "avp";

	HANDLE hToken = NULL;
	OpenThreadToken(GetCurrentThread(), TOKEN_QUERY|TOKEN_IMPERSONATE|TOKEN_DUPLICATE, TRUE, &hToken);
	if( hToken )
		RevertToSelf();

	sprintf(szBuf, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s", szLogName, szAppName); 

	HKEY hk; DWORD dwDisp; 
	RegCreateKeyEx(HKEY_LOCAL_MACHINE, szBuf, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hk, &dwDisp);
	RegSetValueEx(hk, "EventMessageFile", 0, REG_EXPAND_SZ, (LPBYTE)(tSTRING)m_strPath.c_str(cCP_ANSI), (DWORD)m_strPath.size()+1);
	RegCloseKey(hk); 

	if( m_hEventLog )
		DeregisterEventSource(m_hEventLog), m_hEventLog = NULL;

	m_hEventLog = RegisterEventSource(NULL, szAppName);
	m_bSystemLog = bSystem;

	if( hToken )
	{
		SetThreadToken(NULL, hToken);
		CloseHandle(hToken);
	}
#endif
}

bool cBlLoc::report_to_system_log(enNotifySeverity nSeverity, cStrObj& strText, bool bSystem)
{
	if( !m_hEventLog || m_bSystemLog != bSystem )
	{
		if( !m_hEventLog )
			init_event_log(false);
		init_event_log(bSystem);
	}

	if( !m_hEventLog )
		return false;

	cStrBuff strTemp = strText.c_str(cCP_ANSI);
	LPCSTR str = (LPCSTR)strTemp;
	tDWORD wType = EVENTLOG_ERROR_TYPE;
	switch(nSeverity)
	{
	case eNotifySeverityCritical:     wType = EVENTLOG_WARNING_TYPE; break;
	case eNotifySeverityImportant:    wType = EVENTLOG_WARNING_TYPE; break;
	case eNotifySeverityNotImportant: wType = EVENTLOG_INFORMATION_TYPE; break;
	}

	ReportEvent(m_hEventLog, wType, 0, MSG_ERROR, NULL, 1, 0, &str, NULL);
	return true;
}

bool cBlLoc::send_net(cDetectNotifyInfo& pInfo)
{
#if defined (_WIN32)
	if( pInfo.m_strMachineName.empty() )
		return false;

	if( !init() )
		return false;

	static NET_API_STATUS (NET_API_FUNCTION * fnNetMessageBufferSend)(LPCWSTR servername, LPCWSTR msgname, LPCWSTR fromname, LPBYTE buf, DWORD buflen);
	if( !fnNetMessageBufferSend )
	{
		HMODULE hMod = LoadLibrary("netapi32.dll");
	if( hMod )
			*(tPTR*)&fnNetMessageBufferSend = GetProcAddress(hMod, "NetMessageBufferSend");

		if( !fnNetMessageBufferSend )
			*(tPTR*)&fnNetMessageBufferSend = (tPTR)-1;
	}

	if( (tPTR)fnNetMessageBufferSend == (tPTR)-1 )
		return false;

	cStrObj strTemplate = pInfo.m_strTemplate;
	strTemplate.replace_all(L"#(", 0, L"$(");

	cStrObj strMessage;
	m_pRoot->GetFormatedText(strMessage, m_pRoot->LocalizeStr(TOR(tString,()), strTemplate.data(), strTemplate.size()), &pInfo);

	fnNetMessageBufferSend(NULL, pInfo.m_strMachineName.data(), NULL, (LPBYTE)strMessage.data(), strMessage.memsize(cCP_UNICODE));
#endif
	return true;
}

tERROR cBlLoc::svrmgr_request(cSvrMgrRequest* pRequest)
{
	if( !init() )
		return errNOT_OK;

	cAutoObj<cToken> hToken;
	m_bl->sysCreateObjectQuick(hToken, IID_TOKEN, PID_ANY);
	if( !!hToken )
		hToken->Revert();

	return m_pRoot->SendSrvMgrRequest(pRequest->m_Service.data(), (tSrvMgrRequest)pRequest->m_Request);
}

// -------------------------------------------

cSerializable* cBlLoc::GetGlobals()
{
	return &m_bl->m_settings;
}

void cBlLoc::GetLocalizationId(tString& strLocId)
{
	cStringObj str = "%Localization%";
	if( m_bl->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(str), 0, 0) == errOK_DECIDED )
		strLocId = str.c_str(cCP_ANSI); 
	else
		strLocId = "en";
}

// -------------------------------------------
