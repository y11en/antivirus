// -------------------------------------------
// (C) 2002 Kaspersky Lab 
// 
// \file	info.h
// \author	Mezhuev Pavel
// \date	21.01.2003 17:36:14
// \brief	Information about AVS component
// 
// -------------------------------------------

#ifndef __CONNECTOR__H
#define __CONNECTOR__H

// -------------------------------------------
#include <windows.h>

#include <nagent\conn\connector.h>
#include <nagent\conn\task_settings_synchronizer2.h>
#include <nagent\conn\networklist.h>
#include <nagent\conn\app_firewall.h>
#include <std\thr\sync.h>
#include <std\base\klbaseqi_imp.h>
#include <std\base\klstdobjconv.h>
#include <kca\prts\prxsmacros.h>
#include <transport\ev\ev_general.h>
#include <common\converter.h>
#include <common\kllicinfo.h>

#include <pr_remote.h>
#include <pr_loadr.h>
#include <pr_vector.h>
#include <iface\i_root.h>
#include <iface\i_threadpool.h>
#include <iface\i_productlogic.h>
#include <iface\i_datacodec.h>
#include <iface\i_csect.h>
#include <iface\i_report.h>
#include <iface\i_qb.h>
#include <iface\i_cryptohelper.h>
#include <structs\s_taskmanager.h>
#include <structs\s_profiles.h>
#include <structs\s_scaner.h>
#include <structs\s_bl.h>
#include <structs\s_qb.h>
#include <structs\s_ids.h>
#include <structs\s_ahfw.h>
#include <common\updater.h>
#include <common\tracer.h>

#include <string>
#include <map>

typedef std::wstring wstring;
typedef std::vector<wstring> wstrings;
typedef std::map<wstring,wstring> wstrings_map;
typedef KLSTD::CAutoPtr<KLPAR::Value> PValue;
typedef KLSTD::CAutoPtr<KLPAR::Params> PParams;

typedef std::vector<cBLNotification *> cBLNotifications;

#define sswitch(str)    { const char* __str = str;
#define stcase(str)     if( __str && !strcmp(__str,#str) ) { __str = NULL;
#define scase(str)      if( __str && !strcmp(__str,str) ) { __str = NULL;
#define sdefault()      if( __str ) {
#define sbreak          }
#define send            }

#define __itoa(num, base) _itoa(num, (char *)alloca(33), base)
#define __itoa10(num)     __itoa(num, 10)
#define __itoa16(num)     __itoa(num, 16)

#define __itow(num, base) _itow(num, (wchar_t *)alloca(65), base)
#define __itow10(num)     __itow(num, 10)
#define __itow16(num)     __itow(num, 16)

#define __ultow(num, base) _ultow(num, (wchar_t *)alloca(65), base)
#define __ultow10(num)     __ultow(num, 10)
#define __ultow16(num)     __ultow(num, 16)

class CProductConnector;
extern CProductConnector * g_pProduct;

//////////////////////////////////////////////////////////////////////////

enum _eCONNECTOR_IID
{
	cnst_cConnectorSettings = 0x800,
};

struct cConnectorSettings : public cSerializable
{
	cConnectorSettings() :
		m_tmLastReport(0xffffffff),
		m_nLastReport(0),
		m_nTaskUnique(0)
	{}
	
	DECLARE_IID( cConnectorSettings, cSerializable, PID_BL, cnst_cConnectorSettings );
	
	tDWORD           m_tmLastReport;
	tDWORD           m_nLastReport;
	tDWORD           m_nTaskUnique;
};

//////////////////////////////////////////////////////////////////////////
// CProductConnector

class CProductConnector :
	protected cConnectorSettings,
	protected cThreadPoolBase,
	protected cThreadTaskBase,
	protected cPrTracer,
	public KLParamsController
{
public:
	CProductConnector();
	~CProductConnector();

	bool    Init(KLCONN::Notifier *pNotifier);
	bool    Deinit();

	bool    CheckProductRunning();
	bool    SetProductState(bool bActive);

	bool    GetProfileNames(KLSTD::AKWSTRARR &vecTasks, bool bServives, wstrings *pAdditional);
	bool    AddProfile(cProfile *pProfile, tDWORD flags = 0);
	bool    UpdateProfile(cProfile *pProfile, tDWORD flags = 0);
	bool    GetProfile(const tCHAR *strProfile, cProfile *pProfile);
	bool    DeleteProfile(const tCHAR *strProfile);
	bool    SetProfileState(const tCHAR *strProfile, tTaskRequestState nState);
	bool    GetProfileState(const tCHAR *strProfile, tProfileState &nState);
	bool    SetProfileInfo(const tCHAR *strProfile, cSerializable* info);
	bool    GetProfileInfo(const tCHAR *strProfile, cSerializable* info);
	LPCWSTR GetProfileLocalizedName(cProfileBase *pProfile);

	bool    CryptDecrypt(cStringObj &str, tBOOL bCrypt, tBOOL bRecrypt = cFALSE);
	void    PublishEvents();
	bool    PublishEvent(cBLNotification *pNotification);
	tERROR  PublishKnownEvent(KLPAR::Params *pEvent, cBLNotification *pNotification, const wstring &strEventID);
	tERROR  PackEventStruct(KLPAR::Params *pEvent, cBLNotification *pNotification, const wchar_t **pFreeParams);
	void    NotifyStateChanged(const wchar_t *strProfile, const wchar_t *strType, tProfileState nState, tERROR err);
	void    NotifyQbOperation(tDWORD nMsgID, tDWORD nListID, tOBJECT_ID nObjectID);

	void    ClearPolicyAttr(cProfile *pProfile);

	void    GetComponentsList(wstrings &aComponents);
	void    GetComponentsList(wstrings &aComponents, cProfileEx &pProfile);
	
	int     GetCompletion(cProfile *pProfile);
	cTaskStatistics *GetStaistics(cProfile *pProfile);

	cQB *   QbGetService(const wchar_t *szwListName);
	void    QbGetItemsList(cQB *pQb, KLSTD::klwstrarr_t &vecItemsIds, tDWORD nItemsStartPos = 0, tDWORD nItemsCount = (tDWORD)-1);
	bool    QbScan(cQB *pQb, const KLSTD::AKWSTRARR &vecItemsIds);

    void    SetPortsToOpenList(KLPAR::Params* pData);

	bool	GetLicInfo(cCheckInfoList& keys);


	// iface TasksSynchronizer
	void    SendTaskAction(const wchar_t *szwTaskId, KLCONN::TskAction nAction);

protected:
	tERROR  InitEnvironment();
	tERROR  DeinitEnvironment();
	void    OnConnect();
	void    OnDisconnect();
	
	bool    CheckConnection();
	bool    CheckCanOperate();
	void    do_work();
	
	tERROR  OnEvent(tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD *blen);

public:
	static tERROR pr_call MsgReceive(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD *blen);

protected:
	CPRRemotePrague                m_Prague;
	CPRRemoteProxy<cTaskManager *> m_pTM;
	cReport *                      m_pReport;
	cRegistry *                    m_pReg;
	cRegistry *                    m_pEnvironment;
	cDataCodec *                   m_pBase64Codec;
	cMsgReceiver *                 m_pReciever;
//	cCriticalSection *             m_pLock;
	tMsgHandlerDescr               m_hndList[6];
	wstrings_map                   m_mProfileNames;
	KLCONN::Notifier *             m_pNotifier;
	cFwPacketRules                 m_aRulesForNagent;

	cCryptoHelper *                m_pCrHelp;
	cQB *                          m_pQuarantine;
	cQB *                          m_pBackup;

private:
	volatile bool                  m_bShutdown;

public:
	wstring               CreateNewTaskId(const wchar_t *szwTaskType);
	cStringObj            ExpandEnvStr(const wchar_t *str);
	int                   ExpandEnvInt(const wchar_t *str);
};

//////////////////////////////////////////////////////////////////////////
// CSynchronizerBaseT

template <class T>
class CSynchronizerBaseT : public KLSTD::KLBaseImpl<T, false>
{
public:
	CSynchronizerBaseT() : m_pProduct(NULL) {}
	
	void StartIntegration(CProductConnector *pProduct) { m_pProduct = pProduct; }
	void StopIntegration()                             { m_pProduct = NULL; }

protected:
	CProductConnector* m_pProduct;
};

//////////////////////////////////////////////////////////////////////////
// CAppSynchronizerBaseImpl

template <class T>
class CSettingsSynchronizerBaseImpl : public CSynchronizerBaseT<T>
{
	virtual void LockList(long lMaxWaitTime)                            {KLSTD_THROW(KLSTD::STDE_NOFUNC);}
	virtual void UnlockList()                                           {KLSTD_THROW(KLSTD::STDE_NOFUNC);}
	virtual void LockItem(const wchar_t *szwName, long lMaxWaitTime)    {KLSTD_THROW(KLSTD::STDE_NOFUNC);}
	virtual void UnlockItem(const wchar_t *szwName)                     {KLSTD_THROW(KLSTD::STDE_NOFUNC);}
	virtual unsigned long GetCapabilities()                             {return 0;}
};

//////////////////////////////////////////////////////////////////////////
// CProductSettingsSynchronizerImpl

class CProductSettingsSynchronizerImpl : public CSettingsSynchronizerBaseImpl<KLCONN::SettingsSynchronizer>
{
public:
	KLSTD_SINGLE_INTERAFCE_MAP(KLCONN::SettingsSynchronizer);

	virtual void GetSectionsList(KLSTD::AKWSTRARR &vecSections);
	virtual void GetSection(const wchar_t *szwSection, KLPAR::Params *pFilter, KLPAR::Params **ppSettings);
	virtual void PutSection(const wchar_t *szwSection, KLPAR::Params *pData, KLPAR::Params **ppUnsupported);
	virtual bool DeleteSection(const wchar_t *szwSection);

protected:
	void         AddToUnsupported(const wchar_t *szwSection);
	
	wstrings m_UnknownSections;
};

typedef KLSTD::CAutoPtr<CProductSettingsSynchronizerImpl> CProductSettingsSynchronizerImplPtr;

//////////////////////////////////////////////////////////////////////////
// CPolicySettingsSynchronizerImpl

class CPolicySettingsSynchronizerImpl : public CSettingsSynchronizerBaseImpl<KLCONN::SettingsSynchronizer>
{
public:
	KLSTD_SINGLE_INTERAFCE_MAP(KLCONN::SettingsSynchronizer);

	virtual void GetSectionsList(KLSTD::AKWSTRARR &vecSections);
	virtual void GetSection(const wchar_t *szwSection, KLPAR::Params *pFilter, KLPAR::Params **ppSettings);
	virtual void PutSection(const wchar_t *szwSection, KLPAR::Params *pData, KLPAR::Params **ppUnsupported);
	virtual bool DeleteSection(const wchar_t *szwSection);

protected:
	void         CfgEx2Policy(cCfgEx &pCfgEx, KLPAR::Params **ppData);
	void         Policy2CfgEx(KLPAR::Params *pData, cCfgEx &pCfgEx);
};

typedef KLSTD::CAutoPtr<CPolicySettingsSynchronizerImpl> CPolicySettingsSynchronizerImplPtr;

//////////////////////////////////////////////////////////////////////////
// CTaskSettingsSynchronizerImpl

class CTaskSettingsSynchronizerImpl : public CSettingsSynchronizerBaseImpl<KLCONN::TaskSettingsSynchronizer2>
{
public:
	KLSTD_SINGLE_INTERAFCE_MAP(KLCONN::TaskSettingsSynchronizer2);

	virtual void GetTasksList(KLSTD::AKWSTRARR &vecTasks);
	virtual void GetTask(const wchar_t *szwTaskId, KLSTD::AKWSTR &wstrTaskType, KLSCH::Task **ppTaskSch, KLPAR::Params** ppParams, KLPAR::Params** ppParamsTaskInfo);
	virtual void PutTask(const wchar_t *szwTaskId, const wchar_t *szwTaskType, const KLSCH::Task *pTaskSch, const KLPAR::Params *pParams, const KLPAR::Params *pParamsTaskInfo, KLPAR::Params **ppParamsUnsuported, KLPAR::Params **ppTaskInfoUnsuported);
	virtual bool DeleteTask(const wchar_t *szwTaskId);
	virtual KLSTD::AKWSTR CreateNewTaskId();
	virtual KLSTD::AKWSTR CreateNewTaskId2(const wchar_t *szwTaskType);

protected:
	void         PragueProfile2CscTask(cProfileEx &Profile, KLSTD::AKWSTR &wstrTaskType, KLSCH::Task **ppTaskSch, KLPAR::Params** ppParams, KLPAR::Params** ppParamsTaskInfo);
	void         CscTask2PragueProfile(cProfileEx &Profile, const wchar_t *szwTaskId, const wchar_t *szwTaskType, const KLSCH::Task *pTaskSch, const KLPAR::Params *pParams, const KLPAR::Params *pParamsTaskInfo, KLPAR::Params **ppParamsUnsuported, KLPAR::Params **ppTaskInfoUnsuported, bool& bIsGroupTask);
	void         PragueShedule2CscShedule(KLPAR::Params* pTask, KLPAR::Params* pTaskInfo, const cTaskSchedule &PrShdl, KLSCH::Task *&pCscShdl);
	void         CscShedule2PragueShedule(const KLPAR::Params* pTask, const KLPAR::Params* pTaskInfo, const KLSCH::Task *pCscShdl, cTaskSchedule &PrShdl);
	void         SavePassword(const cTaskRunAs &runas, KLPAR::Params *pParams);
	void         LoadPassword(cTaskRunAs &runas, const KLPAR::Params *pParams);
};

typedef KLSTD::CAutoPtr<CTaskSettingsSynchronizerImpl> CTaskSettingsSynchronizerImplPtr;

//////////////////////////////////////////////////////////////////////////
// CTasksSynchronizerImpl

class CTasksSynchronizerImpl : public CSynchronizerBaseT<KLCONN::TasksSynchronizer>
{
public:
	KLSTD_SINGLE_INTERAFCE_MAP(KLCONN::TasksSynchronizer);

	virtual void GetTaskStatus(const wchar_t *szwTaskId, KLCONN::TskState &nState, KLPAR::Params** ppResult, KLCONN::raise_time_t&   tmRiseTime);
	virtual void GetTaskCompletion(const wchar_t *szwTaskId, int &nPercent, KLCONN::raise_time_t&   tmRiseTime);
	virtual void SendTaskAction(const wchar_t *szwTaskId, KLCONN::TskAction nAction);
};

typedef KLSTD::CAutoPtr<CTasksSynchronizerImpl> CTasksSynchronizerImplPtr;

//////////////////////////////////////////////////////////////////////////
// CNetworkListImpl 

class CNetworkListImpl : public CSynchronizerBaseT<KLCONN::NetworkList>
{
public:
	KLSTD_SINGLE_INTERAFCE_MAP(KLCONN::NetworkList);

	virtual void GetItemInfo(const wchar_t *szwListName, const wchar_t *szwListItemId, KLPAR::Params **ppItemParams);
	virtual void GetItemsList(const wchar_t *szwListName, int nItemsStartPos, int nItemsCount, KLSTD::AKWSTRARR &vecItemsIds);
	virtual void DoItemsAction(const wchar_t *szwListName, const wchar_t *szwActionName, const KLSTD::AKWSTRARR &vecItemsIds);

public:
	static wstring                 id2str(const tOBJECT_ID objID);
	static tOBJECT_ID              str2id(const wchar_t *szwListName);
	static KLCONN_NLST::OBJ_STATUS PragueObjStatus2CscObjStatus(enObjectStatus nDetectStatus);
};

//////////////////////////////////////////////////////////////////////////
// CAppFirewallHookImpl

class CAppFirewallHookImpl : public KLSTD::KLBaseImpl<KLCONN::AppFirewallHook, false>
{
public:
	KLSTD_SINGLE_INTERAFCE_MAP(KLCONN::AppFirewallHook);
    
	virtual void SetPortsToOpenList(KLPAR::Params* pData);
};

//////////////////////////////////////////////////////////////////////////
// CAppSynchronizerImpl

class CAppSynchronizerImpl : public CSynchronizerBaseT<KLCONN::AppSynchronizer>
{
public:
    KLSTD_INTERAFCE_MAP_BEGIN(KLCONN::AppSynchronizer)
        KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLCONN::NetworkList,     m_netList)
        KLSTD_INTERAFCE_MAP_ENTRY_CHILD(KLCONN::AppFirewallHook, m_netSettings)
    KLSTD_INTERAFCE_MAP_END()

    virtual void GetAppStatus(KLCONN::AppState &nState, KLPAR::Params **ppResult, KLCONN::raise_time_t &tmRiseTime);
	virtual void GetAppStatistics(KLPAR::Params *pFilter, KLPAR::Params **ppStatistics);
	virtual void GetApplicationProperties(KLPAR::Params **ppData);
    virtual void SendAppAction(KLCONN::AppAction nAction, KLPAR::Params *pData);
    virtual void GetRtpState(KLCONN::AppRtpState& nRtpState, int& nErrorCode);

	CNetworkListImpl     m_netList;
	CAppFirewallHookImpl m_netSettings;
};

typedef KLSTD::CAutoPtr<CAppSynchronizerImpl> CAppSynchronizerImplPtr;

//////////////////////////////////////////////////////////////////////////
// CConnectorImpl

class CConnectorImpl : public KLSTD::KLBaseImpl<KLCONN::Connector, true>
{
public:
	CConnectorImpl();
	virtual ~CConnectorImpl();

public:
	KLSTD_SINGLE_INTERAFCE_MAP(KLCONN::Connector);

    virtual void StartIntegration(KLCONN::Notifier *pNotifier);
    virtual void StopIntegration() throw();

	virtual void CreateSettingsSynchronizer(KLCONN::SettingsSynchronizer **ppSynchronizer)
	{
		m_pProductSettingsSynchronizer.CopyTo((CProductSettingsSynchronizerImpl **)ppSynchronizer);
	}
	virtual void CreatePolicySynchronizer(KLCONN::SettingsSynchronizer **ppSynchronizer)
	{
		m_pPolicySynchronizer.CopyTo((CPolicySettingsSynchronizerImpl **)ppSynchronizer);
	}
	virtual void CreateTaskSettingsSynchronizer(KLCONN::TaskSettingsSynchronizer **ppSynchronizer)
	{
		m_pTaskSettingsSynchronizer.CopyTo((CTaskSettingsSynchronizerImpl **)ppSynchronizer);
	}
	virtual void CreateTasksSynchronizer(KLCONN::TasksSynchronizer **ppSynchronizer)
	{
		m_pTasksSynchronizer.CopyTo((CTasksSynchronizerImpl **)ppSynchronizer);
	}
	virtual void CreateAppSynchronizer(KLCONN::AppSynchronizer **ppSynchronizer)
	{
		m_pAppSynchronizer.CopyTo((CAppSynchronizerImpl **)ppSynchronizer);
	}


protected:
	CProductConnector                   m_Product;
	CAppSynchronizerImplPtr             m_pAppSynchronizer;
	CProductSettingsSynchronizerImplPtr m_pProductSettingsSynchronizer;
	CPolicySettingsSynchronizerImplPtr  m_pPolicySynchronizer;
	CTaskSettingsSynchronizerImplPtr    m_pTaskSettingsSynchronizer;
	CTasksSynchronizerImplPtr           m_pTasksSynchronizer;
};

typedef KLSTD::CAutoPtr<KLCONN::Connector> CConnectorImplPtr;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

KLCONN::TskState       PragueProfileState2CscTaskState(tProfileState nState);
KLCONN::AppRtpState    PragueProfileState2RtpState(tProfileState nState);
KLEVP::EDetectionType  PragueDetectType2CscDetectType(enDetectType eType);

extern HMODULE g_hModule;


#endif // __CONNECTOR__H

