
#ifndef _NetRulesManager_h_
#define _NetRulesManager_h_
#include <Prague/prague.h>
#include "local_inc.h"

#include <AHTasks/iface/i_ipresolver.h>
#include <structs/s_hips.h>


#include "../../CommonFiles\Version\ver_product.h"        //product version definitions
#include <CKAH/CKAHUM.h>
#include <CKAH/CKAHSTAT.h>
#include <CKAH/CKAHPorts.h>
#include <CKAH/ckahrules.h>
#include <CKAH/ckahiptoa.h>

#include <AHTasks/iface/i_ipresolver.h>
#include <AHTasks/plugin/p_resip.h>
#include <CKAH/ipconv.h>


#include <NetWatch/structs/s_netwatch.h>
#include <vector>
#include "NetRMSettings.h"

class CNetRulesTaskState
{
public:
	CNetRulesTaskState();
	~CNetRulesTaskState();
	void SaveState(DWORD);
	void Save_CKAH_State(DWORD);
	DWORD QState();
	DWORD Q_CKAH_State();
	DWORD QStateOld();
private:
	DWORD	m_state;
	DWORD	m_state_old;
	DWORD	m_CKAH_state;
	CRITICAL_SECTION m_cs_state;
};

class CAlock
{
public:
	CAlock(CRITICAL_SECTION * cs)
	{
		m_cs = cs; 
		EnterCriticalSection(m_cs); 
	}
	 ~CAlock() 
	 {
		 LeaveCriticalSection(m_cs);
	 }
private:
	CRITICAL_SECTION  * m_cs;
};
class CNetRulesManager :private CNetRulesTaskState
{
	enum eTargAddr
	{
		fDefault,
		fOnlyHost,
		fOnlyNet,
	};
public:
	CNetRulesManager() {m_inited=0; InitializeCriticalSection(&m_cs);}
	~CNetRulesManager() {DeleteCriticalSection(&m_cs);}
	void SetState( tTaskRequestState);
	void ObjectInitDone(cObject *,hTASKMANAGER		tm);
	void ObjectPreClose();
	void SetSettings(const cSerializable* p_settings);	
	tBOOL NetWatcherSettingsChanged();
	void GeneratePacketRules();
	void GenerateAppRules();

	void ResetAppSeverityRule();
	void SetAppSeverityRule(wchar_t * , tDWORD, tDWORD );
	void ApplyAppSeverityRule();
	CNetRMSettings		m_settings;


private:
	//CKAHFW::HAPPRULES	m_hAppRules;

	hTASKMANAGER		m_tm;
	cObject * 			m_hips;
	cObject*			m_netwatch;

	CKAHFW::FilteringMode		m_FilteringMode;
	CKAHFW::WorkingMode			m_WorkingMode;
	tTaskRequestState			m_TaskState;
	//CHipsSettings *		m_HIPSSettings;
	//cNWSettings *		m_NWsettings;	//cNWSettings
	//cNWdata *	        m_NWdata;		//vZonesAddresses
	


	tDWORD				m_inited;
	cIPRESOLVER*		m_IPResolver;
	tTaskRequestState	m_state;
	CRITICAL_SECTION    m_cs;

	void                Lock (); 
	void                Unlock ();
	
	CKAHUM::OpResult    InitCKAHUM();
	CKAHUM::OpResult	GeneratePacketRuleByHipsPacketRule(cNetZone * pZona,cZonaAddresses * pZonaAddresses,CHipsPacketRule * pHipsPacketRule,cResource *pRes,CKAHFW::HPACKETRULES hRules);
	CKAHUM::OpResult	GeneratePortList(cFwPorts *LocalPorts,cFwPorts *RemotePorts,CKAHFW::HPACKETRULE hRule);
	CKAHUM::OpResult    GenerateAddressesByZone(cNetZone * pZona,cZonaAddresses * pZonaAddresses,CKAHFW::HPACKETRULE hRule);
	CKAHUM::OpResult    GenerateAddressesByZoneApp(cNetZone * pZona,cZonaAddresses * pZonaAddresses,CKAHFW::HAPPRULEELEMENT hRule);
	
	CKAHUM::OpResult	GenerateRuleByWebService(cWebService * pWebService,CKAHFW::HPACKETRULE hRule);
	CKAHUM::OpResult    GenerateAddresses(cIpMasks *,eTargAddr,CKAHFW::HRULEADDRESSES);
	CKAHUM::OpResult    GenerateAddrByIpRange(cIpRange *,CKAHFW::HPACKETRULE );
	CKAHUM::OpResult    GeneratePort(tDWORD,tDWORD, CKAHFW::HRULEPORTS);
	CKAHUM::OpResult    GenerateDirection(tBYTE ,CKAHFW::HPACKETRULE);
	CKAHUM::OpResult    GenerateLocalhostRule(cNetZone * pZona,cZonaAddresses * pZonaAddresses,CKAHFW::HPACKETRULES hRules);
	CKAHUM::OpResult    GenerateBlockAllRule(CKAHFW::HPACKETRULES hRules);
	CKAHUM::OpResult    SetStopProcessFlag(CKAHFW::HPACKETRULE);

};

#endif //_NetRulesManager_h_