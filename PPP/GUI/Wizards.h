// TrainingWizard.h
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __WIZARDS_H__
#define __WIZARDS_H__

#pragma once
#pragma warning ( disable: 4786 )

#include "ListsView.h"
//#include "ServicesView.h"
#include "SettingsDlgs.h"
//#include "SettingsWindow.h"

#include <plugin/p_as_trainsupport.h>
//#include "Kav50Settings/ss.h"

class CAvzWizard : public CTaskProfileView
{
public:
	CAvzWizard(CSinkCreateCtx * pCtx);

protected:
	BIND_MEMBER_MAP_BEGIN()
		BIND_VECTOR(m_vTroubles, cGuiAvzScript::eIID, "Troubles")
	BIND_MEMBER_MAP_END(CTaskProfileView)

protected:
	void OnInit();
	void OnEvent(tDWORD nEventId, cSerializable* pData);
	void OnCustomizeData(cSerializable * pData, bool bIn);
	
	void LoadSection();
	void ReadSection(tString section);

	virtual bool IsMyTask(cProfileBase *pProfile) { return m_pProfile && m_pProfile->m_sName == pProfile->m_sName; }
	virtual void OnTaskComplete(CItemBase *pPage, cProfileBase *pProfile);

protected:
	typedef cVector<cGuiAvzScript> cAvzScripts;

	cAvzScripts m_vTroubles;
};

class CAvzSecurityAnalyzer : public CAvzWizard
{
public:
	CAvzSecurityAnalyzer(CSinkCreateCtx * pCtx);

protected:
	BIND_MEMBER_MAP_BEGIN()
		BIND_VALUE(m_nDetectDanger, tid_DWORD,                 "DetectDanger")
		BIND_VECTOR(m_vThreats,     cInfectedObjectInfo::eIID, "Threats")
	BIND_MEMBER_MAP_END(CAvzWizard)
	
protected:
	bool OnSelect(CItemBase* pItem);
	bool LoadSectionEx();
	
	virtual bool IsMyTask(cProfileBase *pProfile);
	virtual void OnTaskComplete(CItemBase *pPage, cProfileBase *pProfile);	

protected:
	typedef cVector<cInfectedObjectInfo> cThreats;

	CProfile *m_pScanVulnerab;
	cThreats  m_vThreats;
	tDWORD    m_nDetectDanger;
};


/////////////////////////////////////////////////////////////////////////////
// CExecFinder

class CExecFinder : public CRootTaskBase
{
public:
	CExecFinder(CRootItem * pRoot);
	~CExecFinder();
	
	bool   Begin(CItemBase * pParent, tDWORD nEvent);
	void   Stop();

protected:
	void   do_work();
	void   on_exit();
	
	tERROR Find(const cStringObj &sRootDir, cProcessMonitor *pPM, tDWORD nDeep = 0);
	void   ReportCompleted(tDWORD nPercent);

public:
	volatile unsigned m_aborted : 1;
	volatile unsigned m_working : 1;

protected:
	CItemBase *       m_pParent;
	tDWORD            m_nEvent;
	tDWORD            m_TotalFiles;
	tERROR            m_err;
};

//////////////////////////////////////////////////////////////////////
// CAddProductOnlineKey

class CAddProductOnlineKey : public CRootTaskBase
{
public:
	CAddProductOnlineKey(CRootItem * pRoot);
	~CAddProductOnlineKey();
	
	bool CheckActiveKey(cCheckInfo& pInfo);
	bool Begin(const cUserData& data, CItemBase * pParent);
	bool Submit(const tString &strHeaders, tCPTR pPostData, tSIZE_T nPostDataLen);
	void Stop();
	bool Finish(cCheckInfo& pInfo, cStringObj& htmlForm, cLicErrorInfo& licErrInfo);

protected:
	static tERROR pr_call _MsgReceive(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD* blen)
	{
		CAddProductOnlineKey * pId = (CAddProductOnlineKey *)_this->propGetPtr(pgRECEIVE_CLIENT_ID);
		return pId->OnMsg(msg_cls, msg_id, send_point, ctx, receive_point, pbuff, blen);
	}

	void do_work();
	void on_exit() {}
	tERROR OnMsg(tDWORD msg_cls, tDWORD msg_id, hOBJECT send_point, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD* blen);

	CItemBase *       m_pParent;
	cSerObj<cUserData> m_pData;
	tERROR            m_err;
	tDWORD            m_dwOLAErrCode;
	tDWORD            m_dwOLAErrCodeSpecific;
	cCheckInfo        m_Info;
	hSTRING           m_hForm;
	cPostData         m_PostData;
};

/////////////////////////////////////////////////////////////////////////////
// CConfigureWizard

enum EKav46Type { None, Personal, Pro, Wks, Sos };

enum enWisardTriggers
{
	wzNone                 = 0x00000000,
	wzResetArea            = 0x00000001,
	wzActivationBegin      = 0x00000002,
	wzActivationForm       = 0x00000004,
	wzActivationProgress   = 0x00000008,
	wzActivationChooseKey  = 0x00000010,
	wzActivationEnd        = 0x00000020,
	wzUpdate               = 0x00000040,
	wzAntivirus            = 0x00000080,
	wzPassword             = 0x00000100,
	wzPdmRules             = 0x00000200,
	wzPdmRulesCollect      = 0x00000400,
	wzFwNetworks           = 0x00000800,
	wzFwRules              = 0x00001000,
	wzFwAdditional         = 0x00002000,
	wzInherit              = 0x00004000,
	wzInitFinish           = 0x00008000,

	wzMaskActivation       = wzActivationBegin | wzActivationForm | wzActivationProgress | wzActivationChooseKey | wzActivationEnd,
	wzMaskActivationOnline = wzActivationBegin | wzActivationForm | wzActivationProgress | wzActivationEnd,
	wzMaskActivationKey    = wzActivationBegin | wzActivationChooseKey | wzActivationEnd,
	wzMaskSettings         = wzUpdate | wzAntivirus | wzPassword | wzPdmRules | wzPdmRulesCollect | wzFwNetworks | wzFwRules | wzFwAdditional,
	wzMaskAll              = wzMaskActivation | wzMaskSettings,
};

enum enInheritTriggers
{
	wzitNone               = 0x00000000,
	wzitSettings           = 0x00000001,
	wzitQuarantine         = 0x00000002,
	wzitBackup             = 0x00000004,
	wzitLicense            = 0x00000008,
	wzitAll                = wzitSettings|wzitQuarantine|wzitBackup|wzitLicense,
};

class CConfigureWizard : public CTaskProfileView
{
public:
	typedef CTaskProfileView TBase;

	enum eWizardFlags {fActivation = 0x01, fSettings = 0x02};

	CConfigureWizard(tDWORD nFlags = fActivation|fSettings, CProfile * pDst = NULL);
	~CConfigureWizard();

	void DoSilent();

protected:
// 	SINK_MAP_BEGIN()
// 		SINK_STATIC_LIST("NetList", m_NetList)
// 	SINK_MAP_END(TBase)

	BIND_MEMBER_MAP_BEGIN()
		BIND_VALUE(m_nPiwFlags, tid_DWORD, "PiwFlags")

		BIND_VALUE( m_eTriggers,                tid_DWORD,                   "eTriggers")
		BIND_VALUE( m_eLicTriggers,             tid_DWORD,                   "eLicTriggers")
		BIND_VALUE( m_bAlreadyTrialled,         tid_BOOL,                    "bAlreadyTrialled")
		BIND_VALUE( m_strActivationCode,        tid_STRING_OBJ,              "strActivationCode")
		BIND_VALUE( m_strUserName,              tid_STRING_OBJ,              "strUserName")
		BIND_VALUE( m_strUserEmail,             tid_STRING_OBJ,              "strUserEmail")
		BIND_VALUE( m_strCountry,               tid_STRING_OBJ,              "strCountry")
		BIND_VALUE( m_strCity,                  tid_STRING_OBJ,              "strCity")
		BIND_VALUE( m_strKeyPath,               tid_STRING_OBJ,              "strKeyPath")
		BIND_VALUE( m_LicInfo,                  cCheckInfo::eIID,            "LicInfo")
		BIND_VALUE( m_bActivationNeeded,        tid_BOOL,                    "bActivationNeeded")
		BIND_VALUE( m_bConnected,               tid_BOOL,                    "bConnected")
		BIND_VALUE( m_bSentCode,                tid_BOOL,                    "bSentCode")
		BIND_VALUE( m_bRetrievedKey,            tid_BOOL,                    "bRetrievedKey")
		BIND_VALUE( m_nNetworkState,            tid_DWORD,                   "nNetworkState")
		BIND_VALUE( m_bFwRulesAdding,           tid_BOOL,                    "bFwRulesAdding")
		BIND_VALUE( m_Progress,                 tid_DWORD,                   "Progress")
		BIND_VALUE( m_bInteractiveExtendedMode, tid_BOOL,                    "bInteractiveExtendedMode")
		BIND_VALUE( m_bInteractiveAhLearn,      tid_BOOL,                    "bInteractiveAhLearn")
		BIND_VALUE( m_bInteractiveInvader,      tid_BOOL,                    "bInteractiveInvader")
		BIND_VALUE( m_bInteractivePdmRegistry,  tid_BOOL,                    "bInteractivePdmRegistry")
		BIND_VALUE( m_bInteractiveIntegrity,    tid_BOOL,                    "bInteractiveIntegrity")
		BIND_VALUE( m_bIsDnsEnabled,            tid_BOOL,                    "bIsDnsEnabled")
		BIND_VALUE( m_bEnableDns,               tid_BOOL,                    "bEnableDns")
		BIND_VALUE( m_eInheritTriggers,         tid_DWORD,                   "eInheritTriggers")
		BIND_VALUE( m_eInheritTriggersAvail,    tid_DWORD,                   "eInheritTriggersAvail")
		BIND_VALUE( m_bReboot,                  tid_BOOL,                    "bReboot")
		BIND_VALUE( m_bStartProduct,            tid_BOOL,                    "bStartProduct")
		BIND_VALUE( m_strCustomerId,            tid_STRING_OBJ,              "strCustomerId")
		BIND_VALUE( m_strCustomerPassword,      tid_STRING_OBJ,              "strCustomerPassword")
		BIND_VALUE( m_bExpress,                 tid_BOOL,                    "bExpress")
		BIND_VALUE( m_bSilent,                  tid_BOOL,                    "bSilent")
	BIND_MEMBER_MAP_END(CTaskProfileView)									 
																			 
	bool ReInit(CDialogSink * pFrom);										 
	void OnInit();															 
	void AddKeysFromProductRoot();											 
	bool OnAddDataSection(LPCTSTR strSect);									 
	bool OnSelect(CItemBase* pItem);
	void OnEvent(tDWORD nEventId, cSerializable* pData);
	bool OnClicked(CItemBase* pItem);
	bool OnNext();
	bool OnPrev();
	void OnChangedData(CItemBase* pItem);
	bool OnApply();
	bool OnClose(tDWORD& nResult);
	void OnTimerRefresh(tDWORD nTimerSpin);
	bool OnNavigate(CItemBase * pItem, const tString &strURL, int Flags, tCPTR pPostData, tSIZE_T nPostDataLen, const tString &strHeaders, bool &bCancel);
	
	bool ActivationBegin(bool bTrial = false);
	bool InheritLic(bool bCheckOnly);
	void InheritConfiguration(tDWORD nInheritTriggers);
	void UpdateLicensingTriggers();
	void UpdateInheritTriggers();
	void FillRegistryGuardInitialRules();
	void SetUserInfo();
	bool UpgradeSettingsFromHiddenProfiles();

	tERROR LoadCfg(LPCSTR strCfgFile);

	tDWORD               m_nPiwFlags;
	enWisardTriggers     m_eInitTriggers;
	cSerObj<CProfile>    m_pDst;
	CAddProductOnlineKey m_activation;
//	CFwNetworkList       m_NetList;
	CExecFinder          m_ExecFinder;
	CSettingsReseter     m_Reseter;
	EKav46Type			 m_Kav46Type;
	cStrObj				 m_Kav46SettingsFile;
	cConfigureProductDiscartedKeys m_KeysDiscarted;

	unsigned             m_bLicActivating : 1;
	unsigned             m_bInitMode : 1;
	unsigned             m_bLoadPervSettings : 1;
	unsigned             m_bPervSettingsLoaded : 1;
	unsigned             m_bUpgrade : 1;

	enWisardTriggers    m_eTriggers;
	enWisardTriggers    m_eLicTriggers;
	tBOOL               m_bAlreadyTrialled;
	cStrObj             m_strActivationCode;
	cStrObj             m_strUserName;
	cStrObj             m_strUserEmail;
	cStrObj             m_strCountry;
	cStrObj             m_strCity;
	cStrObj             m_strCustomerId;
	cStrObj             m_strCustomerPassword;
	cStrObj             m_strKeyPath;
	cCheckInfo          m_LicInfo;
	
	tBOOL               m_bActivationNeeded;
	tBOOL               m_bConnected;
	tBOOL               m_bSentCode;
	tBOOL               m_bRetrievedKey;
	
	tDWORD              m_nNetworkState;
	
	tBOOL               m_bFwRulesAdding;
	
	tDWORD              m_Progress;

	tBOOL               m_bInteractiveExtendedMode;
	tBOOL               m_bInteractiveAhLearn;
	tBOOL               m_bInteractiveInvader;
	tBOOL               m_bInteractivePdmRegistry;
	tBOOL               m_bInteractiveIntegrity;
	
	tBOOL               m_bIsDnsEnabled;
	tBOOL               m_bEnableDns;
	
	enInheritTriggers   m_eInheritTriggers;
	enInheritTriggers   m_eInheritTriggersAvail;

	tBOOL               m_bReboot;
	tBOOL               m_bStartProduct;

	tBOOL               m_bExpress;
	tBOOL               m_bSilent;
};

/////////////////////////////////////////////////////////////////////////////
// CMapiFolderTree

typedef TrainSettings_t TrainSettings;
typedef TrainFolderInfo_t::FolderState eFolderState;

/////////////////////////////////////////////////////////////////////////////

class CMapiFolderTree : public TrainFolderInfo_t
{
public:
	CMapiFolderTree(){}
	CMapiFolderTree(FolderInfo_t &Folder)
	{
		*static_cast<FolderInfo_t *>(this) = Folder;
		m_FolderState = TrainFolderInfo_t::FolderUndefined;
	}
	bool CanBeChecked()
	{
		return !!bCanHaveMessages;
	}

public:
	std::vector<CMapiFolderTree> m_aChildren;
};

/////////////////////////////////////////////////////////////////////////////
// CFolderExpander

class CFolderExpander : public cThreadPoolBase, public cThreadTaskBase
{
public:
	CFolderExpander() : cThreadPoolBase("FolderExpander") {}
	tERROR Expand(void *pContext, CMapiFolderTree *pFolder, CItemBase *pItem);

protected:
	void   do_work();

	CMapiFolderTree * m_pFolder;
	void *            m_pContext;
	CItemBase *       m_pItem;
};

/////////////////////////////////////////////////////////////////////////////
// CFolderTree

class CFolderTree : public CTreeItemSink
{
public:
	CFolderTree(cAS_TrainSupport *pAS, CMapiFolderTree &Tree, CFolderExpander &FolderExpander, eFolderState CheckedState) : m_pAS(pAS), m_Tree(Tree), m_FolderExpander(FolderExpander), m_CheckedState(CheckedState) {}

protected:
	struct TVIDATA : public cTreeItem
	{
		TVIDATA(CMapiFolderTree * pFolder)
		{
			m_flags = UPDATE_FLAG_TEXT|UPDATE_FLAG_SUBCOUNT;
			m_strName = pFolder->szName;
			m_pFolder = pFolder;
			m_nChildren = m_pFolder->bHasChildren ? 1 : 0;
		}

		CMapiFolderTree * m_pFolder;
	};

protected:
	void      OnInited();
	bool      OnItemExpanding(cTreeItem * pItem, tUINT& nChildren);
	bool      OnItemChecking(cTreeItem * pItem);
	void      OnUpdateItemProps(cTreeItem * hItem, tDWORD flags);

protected:
	void              GetFolderInfo(CMapiFolderTree *pFolder);
	bool              ExpandItem(cTreeItem * hItem);
	CMapiFolderTree * GetFolder(cTreeItem * hItem);
	bool              CanBeChecked(CMapiFolderTree *pFolder);
public:
	void              GetFolderInfoAsync(CMapiFolderTree *pFolder, CItemBase *pItem);
	void              ApplyState(cTreeItem * hItem, eFolderState State, bool bApplyToChildren = false, bool bApplyToSiblings = false);

protected:
	cAS_TrainSupport * m_pAS;
	CMapiFolderTree &  m_Tree;
	eFolderState       m_CheckedState;
	CFolderExpander &  m_FolderExpander;
};

/////////////////////////////////////////////////////////////////////////////
// CWzTrainig

class CWzTrainig : public CDialogBindingT<>
{
public:
	typedef CDialogBindingT<> TBase;
	
	CWzTrainig();
	virtual ~CWzTrainig();

protected:
	SINK_MAP_BEGIN()
		SINK_DYNAMIC_EX("folders_ham",  GUI_ITEMT_TREE, CFolderTree(m_pAS, *m_pTree, m_FolderExpander, TrainFolderInfo_t::FolderHAM))
		SINK_DYNAMIC_EX("folders_spam", GUI_ITEMT_TREE, CFolderTree(m_pAS, *m_pTree, m_FolderExpander, TrainFolderInfo_t::FolderSPAM))
	SINK_MAP_END(CDialogBindingT<>)

	virtual void OnInited();
	virtual bool OnClicked(CItemBase* pItem);
	virtual bool OnSelect(CItemBase* pItem);
	virtual bool OnCanClose(bool bUser);
	virtual bool OnOk();
	virtual void OnEvent(tDWORD nEventId, cSerializable* pData);

protected:
	void         StartTraining();
	void         OnTrainComplete();
	void         OnTrainStatus(TrainStatus_t *pStatus);

protected:
	void         TwCancelResults();
	void         TwApplyResults();
	void         TwStop();
	void         TwSkip();

protected:
	cAS_TrainSupport *         m_pAS;
	CMapiFolderTree *          m_pTree;
	TrainStatus_t              m_Status;
	DWORD                      m_nStartTime;
	bool                       m_bTraining;
	CFolderExpander            m_FolderExpander;
};


/////////////////////////////////////////////////////////////////////////////
// CMakeRescueDiskWizard

class CMakeRescueDiskWizard :
	public CDialogBindingViewT<>,
	public cMakeRescueDisk,
	public CRootTaskBase
{
public:
	typedef CDialogBindingViewT<> TBase;

	CMakeRescueDiskWizard();
	~CMakeRescueDiskWizard();

	enum OpType
	{
		BartPE_Prepare, BartPE_MakeIso, BartPE_BurnCd,
		WinPE_Prepare, WinPE_MakeIso,
		Other_Prepare,
	};

protected:
	void OnInit();
	bool OnSelect(CItemBase* pItem);
	bool OnNext();
	bool OnPrev();
	void OnEvent(tDWORD nEventId, cSerializable * pData);
	void OnTimerRefresh(tDWORD nTimerSpin);
	bool OnCanClose(bool bUser);
	bool OnClose(tDWORD& nResult);
	void OnDestroy();

	void do_work();
	void on_exit();

	tERROR ApplyAVP6(OpType eOp, bool bApply = true);
	tERROR CopyFromPackage(cStringObj& strPackageFile, cStringObj& strScrDir, cStringObj& strDestPath);
	bool   ValidateData(OpType eOp);
	bool   CanInterruptProcess();

	OpType m_eOp;
	cGuiErrorInfo m_err;
	CStdinRedirectedSinkBase * m_pRecorders;
	CStdinRedirectedSinkBase * m_pMkIsoViewer;
	CStdinRedirectedSinkBase * m_pBurnCdViewer;
};

/////////////////////////////////////////////////////////////////////////////

extern CItemBase *g_pTW;


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

#endif
