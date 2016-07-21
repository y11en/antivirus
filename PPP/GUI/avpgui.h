// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Thursday,  30 October 2003,  11:27 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- gui.cpp
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __gui_cpp__c14e8ba1_9225_423c_a433_749e7f769e03 )
#define __gui_cpp__c14e8ba1_9225_423c_a433_749e7f769e03
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <plugin/p_gui.h>
#include <plugin/p_qb.h>
#include <iface/i_root.h>
#include <iface/i_reg.h>
// AVP Prague stamp end

#include <iface/e_loader.h>

#include "RootSink.h"
#include "TrayIcon.h"
#include "StatusWindow.h"

#define AVP_SOURCE_GUI            "Gui"
#define AVP_SOURCE_REPORTS        "Reports"
#define AVP_SOURCE_QUARANTINE     "Quarantine"
#define AVP_SOURCE_BACKUP         "Backup"

#define AVP_STATE_HIDDEN          (tDWORD)-1

#ifdef _DEBUG
	#define DebugSleep(_n)	PrSleep(_n)
#else
	#define DebugSleep(_n)
#endif

#define pmPROCPROTECTION_IFNO              (pmc_PROCPROTECTION + pm_PROCPROTECTION_IFNO)
#define pmLICENSING_DATA_CHANGED           (pmc_LICENSING + pm_DATA_CHANGED)

//***************************************************************************

#define GUI_PROCEVT_PROCESS		0x00000001
#define GUI_PROCEVT_FORCESYNC	0x00000002

enum ePasswordConfirmReason
{
	pwdc_CheckAlways = -1,
	pwdc_OpenWindow = 0,
	pwdc_SaveSettings,
	pwdc_StopTask,
	pwdc_Exit,
	pwdc_StopService,
	pwdc_Activate,
	pwdc_SaveStateAndSettings,
};

// AVP Prague stamp begin( C++ class declaration,  )
struct GUIImpl :
	public cTask,
	public CKav6RootSink,
	public CPRRemotePrague
{
public:
	typedef CKav6RootSink TBase;
	GUIImpl();
	~GUIImpl();

	SINK_MAP_DECLARE();
	cNode *CreateOperator(const char *name, cNode **args, cAlloc *al);

private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call SetSettings( const cSerializable* p_settings );
	tERROR pr_call GetSettings( cSerializable* p_settings );
	tERROR pr_call AskAction( tActionId p_actionId, cSerializable* p_actionInfo );
	tERROR pr_call SetState( tTaskRequestState p_state );
	tERROR pr_call GetStatistics( cSerializable* p_statistics );

// Data declaration
	tCHAR  m_user_settings[256]; // --
	tDWORD m_session_id;         // --
// AVP Prague stamp end

public:
	void OnDestroy();
	void OnEvent(tDWORD nEventId, cSerializable* pData);
	void OnTimerRefresh(tDWORD nTimerSpin);
	void OnActivate(CItemBase* pItem, bool bActivate);
	bool CanClearResources() { return true; }
	void OnHotkey(tDWORD nVK);

	BIND_MEMBER_MAP_BEGIN()
		BIND_VALUE(m_nTaskManager, tid_DWORD, "TaskManagerState")
		BIND_VALUE(m_nStateId,     tid_DWORD, "ProdStateId")
	BIND_MEMBER_MAP_END(CKav6RootSink)

public:
	tERROR OpenRequest(tDWORD p_request_id, tDWORD p_request_type, cSerializable* p_params);
	tERROR CloseRequest(tDWORD p_request_id);
	void Disconnect();
	bool CheckConnection();

	bool Exit(CItemBase * pParent, bool bAskCloseWnd = false);
	void ShowTray(bool bShow = true);
	bool SetProfileState(LPCSTR strProfile, tTaskRequestState nState, CItemBase * pParent);
	bool SetProfileLevel(LPCSTR strProfile, tDWORD nLevel);
	bool EnableProfile(LPCSTR strProfile, tBOOL bEnable);
	bool SaveAsProfile(LPCSTR strProfile, CItemBase * pParent);
	bool RenameProfile(LPCSTR strProfile, CItemBase * pParent);
	bool DeleteProfile(LPCSTR strProfile, CItemBase * pParent);
	bool UpdateProgramState();
	bool AskScanObjects(cAskScanObjects* pAskInfo);
	void UpdateInternalProfile(LPCSTR strProfile = NULL);
	
	void    GetProductStatesOrder(tKeys& vecStates, tQWORD * pStatesMask, LPCSTR strSection);
	tDWORD  GetProductStateId(tKeys& vecStates, tDWORD nStateId = 0, int nDir = 0, bool bFilter = true);
	void    RemoveProductState(tDWORD nStateId);
	void    RestoreProductStates(tQWORD nStateMask = -1);
	bool    IsProductStateHidden(tDWORD nStateId = 0, tQWORD nStateMask = -1);
	bool    IsProductStateAvailable(tDWORD nStateId = 0);
	
	cStringObj GetProductNewsItemId(const cStringObj& Id, int nDir = 0, bool bSkipHidden = true) const;
	bool GetProductNewsItem(const cStringObj& Id, cGuiNewsItem& NewsItem) const;
	void HideProductNewsItem(const cStringObj& Id);
	bool IsProductNewsItemHidden(const cStringObj* pId = NULL) const;

	void GetReservedKeyInfo();
	
	tDWORD  GetInitFlags() { return m_settings ? m_settings->m_Ins_InitMode : 0; }

	cGuiVariables * GetGuiVariables() { return &m_Vars; }
	cStringObj&     GetUserId()       { return m_sUserId; }
	cStringObj&     GetUserName()     { return m_sUserName; }
	tDWORD          GetOSVersion()    { return m_nOSVersion; }
	cObject *       Obj()             { return *this; }
	cCryptoHelper * GetCryptoHelper(bool& bSimple);
	cIPRESOLVER *   GetIpResolver();

	bool ProcessEvent(tDWORD nEventId, cSerializable* pData = NULL, CItemBase * pParent = NULL, tDWORD nFlags = GUI_PROCEVT_PROCESS, bool * pSync = NULL);

	bool IsTransparentEnabled();
	bool IsGuiSimple();
	tBYTE GetDlgTransparency();
	void ReportError(tTRACE_LEVEL level, LPCSTR strInfo);

	void OpenMailURL(CItemBase* pItem, LPCSTR strCommand);
	bool CheckPassword(CItemBase* pItem, ePasswordConfirmReason nReason);
	bool CheckCanShowAlert(const cStringObj &strProfile, bool bCheckProfileState = true);
	void CheckAndCloseAlert(cProfileBase *pProfile);
	void StoreAlertInfo(CDialogSink *pSink, const cStringObj &strProfile);
	bool TryInstallAutoLicKey();

	bool ReadNews();
	void UpdateNewsTray();
	void ShowNewsTray(bool bShow);

	bool DeleteKey(CItemBase* pItem, bool bOnlyReserved);

	bool OpenPersonalCabinet(bool req, CItemBase* pItem);
	bool BuyProduct(bool renew, CItemBase* pItem);
	
	bool SetRegSettings(const tCHAR *sPath, cSerializable* pStruct);
	
	void CheckAndSendDumps();

	cProfile* GetCachedProfile(tTaskId nTaskId);

	bool QBObjectRestore(cQBObject * pInfo, CItemBase * pParent, bool bFirstSaveAs);

	tMsgHandlerDescr m_hndList[11];

#if defined (_DEBUG) || defined (_AVPGUI_TEST)
	void TestGui(tDWORD nId, cSerializable* pData);
#endif // _DEBUG || _AVPGUI_TEST

public:
	CGuiLoader      m_Gui;
	cLicensing *    m_pLic;
	cQB *           m_pQuarantine;
	cQB *           m_pBackup;
	cAVS *          m_pAvs;
	cProcessMonitor * m_pPM;
    cCryptoHelper * m_pCrHelp;
	cReportDB *     m_pReport;
	cIPRESOLVER *   m_pIpRes;
	CItemBase *     m_pTray;
	CTrayFlash      m_TrayNews;
	unsigned        m_fShowTray : 1;
	unsigned        m_fWithoutSysShell : 1;
	tBOOL           m_bHasServiceAccessRights;

	tDWORD          m_nTaskManager;
	tDWORD          m_nStateId;

	cSerObj<cBLSettings>   m_settings;
	cSerObj<cBLStatistics> m_statistics;

	std::vector<cProfile*> m_vProfiles;

private:
	cStringObj      m_sUserId;
	cStringObj      m_sUserName;
	cGuiVariables   m_Vars;
	tKeys           m_vecStates;
	CStatusBaloon   m_pBallons;
	tBOOL           m_bPasswordChecked;
	tPROCESS        m_Host;
	tBOOL           m_bExiting;
	tString         m_strAlertItemId;
	cStringObj      m_strAlertProfileId;
	tBOOL           m_bHasDelayedDumps;
	tBOOL           m_bClosingSessionWindows;

	MEMBER_NODE(cNodeExit)       m_nodeExit;
	MEMBER_NODE(cNodeActivate)   m_nodeActivate;
	MEMBER_NODE(cNodeRemoveActiveKey) m_nodeRemoveActiveKey;
	MEMBER_NODE(cNodeThreatAll)  m_nodeThreatAll;
	MEMBER_NODE(cNodeRestartApp) m_nodeRestartApp;
	MEMBER_NODE(cNodeRestartOs)  m_nodeRestartOs;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION_NO_DELETE(GUIImpl)
};
// AVP Prague stamp end

//***************************************************************************
	
struct cAttachFiles : public cSerializable
{
	cAttachFiles() : m_pItem(NULL) {}

	tPTR             m_pItem;
	tAttachedFiles * m_aAattaches;
	tERROR           m_err;
};
	
struct cGuiRequest : public cSerializable
{
	DECLARE_IID( cGuiRequest, cSerializable, PID_GUI, 104 );

	cSerObj<cSerializable> m_pData;
	tDWORD                 m_request_id;
};

void AntispamShowTrainingWizard(CItemBase* pParent = NULL);
void Reg_Bin2Str(tDWORD nRegType, tBYTE * pData, tDWORD nSize, tString& strRes);
	
//***************************************************************************

extern GUIImpl *  g_hGui;
extern CProduct * g_pProduct;

#include "globals.h"

//***************************************************************************

// AVP Prague stamp begin( Header endif,  )
#endif // gui_cpp
// AVP Prague stamp end

