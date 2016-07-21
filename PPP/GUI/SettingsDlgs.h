// SettingsWindow.h: interface for the SettingsWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTINGSDLGS_H__AA90CADE_8429_4816_AA5B_1C67B4785355__INCLUDED_)
#define AFX_SETTINGSDLGS_H__AA90CADE_8429_4816_AA5B_1C67B4785355__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ListsView.h"
#include "FWRules.h"
#include "ScanObjects.h"

#include <iface/i_threadpool.h>

#include <structs/s_pdm2rt.h>
#include <structs/s_antispam.h>
#include <structs/s_urlflt.h>
#include <Hips/structs/s_hips.h>
#include <Updater/transport.h>
#include <Updater/updater.h>

#include <ProductCore/structs/s_ip.h>
#include <NetDetails/structs/s_netdetails.h>

#define EVENTID_SETTINGS_DETAILS       0x19ff34ce

//////////////////////////////////////////////////////////////////////////
// CRegistryBrowseCallBack

class CRegistryBrowseCallBack : public CBrowseCallBack
{
	bool NextObject(tDWORD& nCustomId) { return false; }
	bool OnCustomizeObject(bool bDir, CBrowseObjectInfo& pObject);
};

//////////////////////////////////////////////////////////////////////////
// CResourceList

class CResourceList : public CVectorViewT<cResource>, public CRegistryBrowseCallBack
{
public:
	CResourceList() : m_bResult(false), m_pGroup(NULL), m_grpList(NULL), m_sett(NULL) {}
	
	bool   OnSetValue(cResource * pGroup);

private:
	bool   OnClicked(CItemBase * pItem);
	void   OnChangedData(CItemBase * pItem);
	tERROR OnEditItem(CItemBase * pOpCtl);
	tERROR BrowseObject(cSer *pItem);

	cResource * m_pGroup;
	bool        m_bResult;
	
	friend class CResourceGroupList;
	CResourceGroupList * m_grpList;
	CHipsSettings *      m_sett;
};

//////////////////////////////////////////////////////////////////////////
// CResourceGroupList

class CResourceGroupList : public CSerTreeView
{
public:
	CResourceGroupList() : m_sett(NULL), m_pResList(NULL) {}

private:
	void OnInited();
	bool OnListEvent(eListEvents nEvent);
	bool OnPostOperateItem(cRowId &nItem, eOpCode nOp, tPTR pValue);
	
	CHipsSettings *m_sett;
	CResourceList *m_pResList;
};

//////////////////////////////////////////////////////////////////////////
// CAppGroupRuleList

class CAppGroupRuleList : public CSerTreeView
{
public:
	CAppGroupRuleList();

private:
	bool     OnSetType(tTYPE_ID nType);
	bool     OnSetValue(const cVariant &pValue);
	void     OnInit();
	bool     GetRowInfo(cSer *pNode, tIdxArr &pIdx);
	cSer *   GetRowData(tIdx nItem);
	cSer *   GetTreeData(tIdxArr &pIdx, bool bIsNode);
	bool     GetCellData(cCellData& cell);
	void     OnChangedData(CItemBase * pItem);
	bool     OnDragRow(cRowId& row, cRowId& rowto, bool bEnd);

	typedef std::vector<tDWORD> cIndexes;
	typedef std::map<tDWORD, cColumn *> cColumnMap;

	void     UpdateColumns(cResource &pGroup);
	void     UpdateColumns(cResource &pGroup, cColumn *pParent, cIndexes &aIdxs, cNode *pColName, tDWORD nDeep);
	void     RemoveColumn(cColumn * col, cIndexes &aIdxs);
	
	unsigned       m_updatingCols : 1;
	cColumnMap     m_cols;
	CHipsSettings *m_sett;
	cGuiAppGroup   m_item;
};

//////////////////////////////////////////////////////////////////////////
// CAppRuleList

class CResRuleList : public CSerTreeView
{
public:
	CResRuleList() : m_sett(NULL), m_appID(0), m_isAppGroup(false) {}

private:
	bool     OnSetType(tTYPE_ID nType);
	void     OnInit();
	cSer *   GetTreeData(tIdxArr &pIdx, bool bIsNode);
	void     OnChangedData(CItemBase * pItem);

	tDWORD         m_appID;
	bool           m_isAppGroup;
	cGuiResRule    m_item;
	CHipsSettings *m_sett;
};

//////////////////////////////////////////////////////////////////////////
// CNetworkList

class CNetworkList : public CVectorView
{
	bool     OnClicked(CItemBase* pItem);
};

//////////////////////////////////////////////////////////////////////////
// CAppDialog

class CAppDialog : public CDialogSink
{
public:
	CAppDialog(CSinkCreateCtx * pCtx);	

protected:
	void OnCreate();
	
 	BIND_MEMBER_MAP_BEGIN()
		BIND_VALUE(m_pid,        tid_DWORD, "Pid")
		BIND_VALUE(m_appID,      tid_DWORD, "AppId")
		BIND_VALUE(m_isAppGroup, tid_BOOL,  "IsAppGroup")
 	BIND_MEMBER_MAP_END(CDialogSink)

private:
	tQWORD      m_pid;
	tDWORD      m_appID;
	tBOOL       m_isAppGroup;
	cModuleInfo m_processInfo;
};

//////////////////////////////////////////////////////////////////////
// CVirtualKbd

#include "../vkbd/vkbd.h"

class CVirtualKbd : public CDialogSink, public VkbdApi
{
public:
	CVirtualKbd();
	virtual ~CVirtualKbd();

protected:
	void OnInit();
	void OnInited();
	bool OnClicked(CItemBase *pItem);
	void OnChangedState(CItemBase * pItem, tDWORD nStateMask = ISTATE_ALL);
	void OnTimerRefresh(tDWORD nTimerSpin);

protected:
	LPCSTR GetKeyName(tDWORD vk);
	void   InitKeyboard();
	void   UpdateLayout();

protected:
	tDWORD       m_nFlags;
	PrHMODULE    m_hVkbdModule;
	bool         m_bVkbdInited;
	const void * m_hLayout;
};

//////////////////////////////////////////////////////////////////////////
// CSettingsReseter

class CSettingsReseter :
	public cGuiResetArea
{
public:
	CSettingsReseter(CProfile * pProfile);

	void FindImportantSettings();
	void FillSettings();
	void Reset();
	bool IsEmpty() { return m_aResetItems.empty(); }
	bool IsProfileReset(LPCSTR strName, LPCSTR strVector = NULL);
	bool SetProfileReset(LPCSTR strName, bool bEnabled);

	static void CopyNotResetableProtSettings(cBLSettings * pDstCfg, const cBLSettings * pSrcCfg);

protected:
	bool OnAddDataSection(LPCSTR strSect);
	bool OnIsForceReset(LPCSTR strName, LPCSTR strVector);

	struct Profile
	{
		struct List
		{
			List() : m_pDataCopy(NULL) {}
			
			tString               m_name;
			const cSerDescriptorField * m_pField;
			tMemChunk *           m_pData;
			tMemChunk *           m_pDataCopy;
			
			tDWORD                m_nViewPos;
		};
		
		typedef cVector<List, cSimpleMover<List> > tLists;
		typedef cVector<Profile, cSimpleMover<Profile> > tSubs;
		
		Profile() : m_nViewPos(-1), m_bDirty(false) {}
		
		void Clear()
		{
			m_lists.clear();
			m_sub.clear();
		}

		Profile * Find(LPCSTR strName)
		{
			if( m_pProfile->m_sName == strName )
				return this;
			
			for(int i = 0, n = m_sub.size(); i < n; i++)
				if( Profile * pSub = m_sub[i].Find(strName) )
					return pSub;
			
			return NULL;
		}
		
		cSerObj<CProfile>    m_pProfile;
		cCfg                 m_def;
		tDWORD               m_nViewPos;
		tLists               m_lists;
		bool                 m_bDirty;

		tSubs                m_sub;
	};

	void _InitCfg(tDWORD lev, Profile& _item);
	bool _Init(tDWORD lev, Profile& _item, tDWORD nIdent);
	void _Reset(tDWORD lev, Profile& _item);
	bool _CheckCustomLevel(tDWORD lev, Profile& _item);
	
	Profile                  m_data;
	unsigned                 m_bDefBind : 1;
	unsigned                 m_bProtection : 1;
};

/*
//////////////////////////////////////////////////////////////////////////
// Commands

tERROR ParCtl_ProtLevelReset(cSerializable * pCtx, CItemBase * pItem);
tERROR ParCtl_SwitchProfile(cSerializable * pCtx, CItemBase * pItem, LPCSTR strParams);

//////////////////////////////////////////////////////////////////////////
// CResetItemVector

class CResetItemVector :
	public CSerVectorView<cResetItem>,
	public CIndentListAutocheckHelper
{
public:
	bool   OnListEvent(int nItem, int nColumn, int nEvent);
	tDWORD OnGetRecordIndent(tDWORD nSrcPos);

	CListItem * IndentListAutocheckHelper_OnGetList() { return Item(); }
	tDWORD      IndentListAutocheckHelper_OnGetFlags() { return INDENTLISTAUTOHLP_F_PARENTSELECTWITHOUTCHILDREN; }
};

//////////////////////////////////////////////////////////////////////
// CExceptsItemCfgDlg

class CExceptsItemCfgDlg : public CSettingsDlg<>
{
public:
	CExceptsItemCfgDlg(cSerializable* pStruct) : CSettingsDlg<>(pStruct) {}

	bool OnClicked(CItemBase* pItem);
};

//////////////////////////////////////////////////////////////////////
// CExceptsItemDlg

class CExceptsItemDlg : public CSettingsDlg<>
{
public:
	CExceptsItemDlg(cSerializable* pStruct, bool bNew = false, LPCSTR sTaskName = NULL);

protected:
	void OnCustomizeData(cSerializable * pData, bool bIn);

	cStringObj m_sTaskName;
};

//////////////////////////////////////////////////////////////////////
// CExceptsList

class CExceptsList : public CSerVectorView<cDetectExclude>
{
public:
	SINK_MAP_BEGIN()
		SINK_DYNAMIC("protection_excludeitem", CExceptsItemDlg(ExtractCtx(pCtx), m_bEditNew, m_sTaskName.c_str(cCP_ANSI)))
	SINK_MAP_END(CSerVectorView<cDetectExclude>)

	bool OnBindValue(CItemPropVals& pValue, tDWORD& nUnique);
	bool IsFiltered();
	bool OnAddRecord(CRecord &pRecord);

protected:
	cStringObj m_sTaskName;
};*/

//////////////////////////////////////////////////////////////////////
// CExceptsTasksList

class CExceptsTasksList : public CListItemSink
{
public:
	CExceptsTasksList() : m_nTasksMask(0) {}

	BIND_MEMBER_MAP_BEGIN()
		BIND_VALUE(m_nTasksMask, tid_DWORD, "TasksMask")
	BIND_MEMBER_MAP_END(CListItemSink)
	
	tTYPE_ID OnGetType()                          { return tid_STRING_OBJ; }
	tDWORD   OnBindingFlags()                     { return ITEMBIND_F_VALUE; }
	bool     OnIsVector()                         { return true; }
	bool     OnSetValue(const cVariant &pValue);
	bool     OnGetValue(cVariant &pValue);

protected:
	tDWORD   m_nTasksMask;
};

/*//////////////////////////////////////////////////////////////////////
// CPdmMakeRule

class CPdmMakeRule : public CDialogBindingT<>
{
public:
	CPdmMakeRule(cSerializable * pInfo) :
		CDialogBindingT<>(pInfo, DLG_ACTION_OKCANCEL) {}
	
	bool OnOk()
	{
		if( !CDialogBindingT<>::OnOk() )
			return false;
		
		if( m_pStruct->isBasedOn(cAskObjectAction::eIID) )
		{
			cAskObjectAction * pAsk = (cAskObjectAction *)m_pStruct;
			pAsk->m_nResultAction = pAsk->m_nExcludeAction;
			((CDialogItem *)Item()->m_pParent)->Close();
		}

		return true;
	}
};

//////////////////////////////////////////////////////////////////////
// CAddToExceptsDlg

class CAddToExceptsDlg : public CExceptsItemDlg
{
public:
	CAddToExceptsDlg(cDetectObjectInfo * pObject);
	bool OnOk();

	cSerObj<cDetectObjectInfo> m_pObject;
	cDetectExclude             m_ExclItem;

	static tINT FindExcludeObject(cVector<cDetectExclude>& aExcludes, cDetectExclude& ExclItem);
};

//////////////////////////////////////////////////////////////////////
// CFileMonAdvSettDlg

class CFileMonAdvSettDlg : public CCustomLevelDlg
{
public:
	CFileMonAdvSettDlg(CProfile * pProfilesList, CProfile * pProfile);

protected:
	void OnCustomizeData(cSerializable * pData, bool bIn);
};

//////////////////////////////////////////////////////////////////////
// CPdmAppLst, CPdmDllLst

class CPdmDllLst : public CSerVectorView<cPdmAppMonitoring_Data>
{
public:
	class CCalcHashesDlg : public CWaitingDlg
	{
	public:
		CCalcHashesDlg(CBrowseObjectInfo& ObjInfo, cVector<cPdmAppMonitoring_Data>& aHashes) :
			CWaitingDlg(&m_Progress, 0),
			m_ObjInfo(ObjInfo),
			m_aHashes(aHashes)
		{}

		tERROR OnDoWork();

	protected:
		CBrowseObjectInfo&               m_ObjInfo;
		cVector<cPdmAppMonitoring_Data>& m_aHashes;
		cProgressInfo                    m_Progress;
	};

public:
	virtual tERROR OnEditItem(CItemBase * pOpCtl);
};

//////////////////////////////////////////////////////////////////////
// CRegGuardAdvSettDlg

class CPdmRegGroupAppLst : public CSerVectorView<cPdmRegGroupApp_Item>
{
public:
	tERROR OnEditItem(CItemBase * pOpCtl);
};

class CPdmRegGroupKeyLst :
	public CSerVectorView<cPdmRegGroupKey_Item>,
	public CBrowseCallBack
{
public:
	tERROR OnEditItem(CItemBase * pOpCtl);

protected: // struct CBrowseCallBack
	bool NextObject(tDWORD& nCustomId) { return false; }
	bool OnCustomizeObject(bool bDir, CBrowseObjectInfo& pObject);
	bool OnCanAddObject(CBrowseObjectInfo& pObject);
};
	
class CRegGuardEditDlg : public CSettingsDlg<cPdmRegGroup_Item>
{
public:
	typedef CSettingsDlg<cPdmRegGroup_Item> Base;
	CRegGuardEditDlg(cPdmRegGroup_Item *pStruct, bool bNew = false) : Base(pStruct, bNew) {}

protected:
	bool OnOk();
};

//////////////////////////////////////////////////////////////////////
// CPdmRegGroupLst

class CPdmRegGroupLst : public CSerVectorView<cPdmRegGroup_Item>
{
public:
	SINK_MAP_BEGIN()
		SINK_DYNAMIC_TOPITEM(CRegGuardEditDlg((cPdmRegGroup_Item *)ExtractCtx(pCtx, cPdmRegGroup_Item::eIID), m_bEditNew))
	SINK_MAP_END(CListBindingSink)
	
	tERROR OnEditItem(CItemBase * pOpCtl);
};

//////////////////////////////////////////////////////////////////////
// CRegGuardAdvSettDlg

class CRegGuard2GroupAppLst : public CSerVectorView<CRegAppEntry>
{
public:
	tERROR OnEditItem(CItemBase * pOpCtl);
};

class CRegGuard2GroupKeyLst :
	public CSerVectorView<CRegKeyEntry>,
	public CBrowseCallBack
{
public:
	tERROR OnEditItem(CItemBase * pOpCtl);

protected: // struct CBrowseCallBack
	bool NextObject(tDWORD& nCustomId) { return false; }
	bool OnCustomizeObject(bool bDir, CBrowseObjectInfo& pObject);
	bool OnCanAddObject(CBrowseObjectInfo& pObject);
};

class CRegGuard2EditDlg : public CSettingsDlg<CRegGroupEntry>
{
public:
	typedef CSettingsDlg<CRegGroupEntry> Base;
	CRegGuard2EditDlg(CRegGroupEntry *pStruct, bool bNew = false) : Base(pStruct, bNew) {}

protected:
	bool OnOk();
};

class CRegGuard2GroupLst : public CSerVectorView<CRegGroupEntry>
{
public:
	SINK_MAP_BEGIN()
		SINK_DYNAMIC_TOPITEM(CRegGuard2EditDlg((CRegGroupEntry *)ExtractCtx(pCtx, CRegGroupEntry::eIID), m_bEditNew))
	SINK_MAP_END(CListBindingSink)

	tERROR OnEditItem(CItemBase * pOpCtl);
};

//////////////////////////////////////////////////////////////////////
// CAntiSpamAdvSettDlg

class CAsEmailsList : public CVectorView
{
protected:
	class CMAdress
	{
	public:
		CMAdress(size_t maxAddressLen) : m_maxAddressLen(maxAddressLen) {}
		std::vector<std::string> m_vAdress;
		void Parse(char* pBuffer, bool& bTruncated);
		void ParseEx(char* pBuffer, bool& bTruncated);
	private:
		void	Add(tString s, bool& bTruncated);
		int		Find(tString s);
		void	ToLower(tString& s);
		
		size_t m_maxAddressLen;
	};

protected:
	bool ImportWhiteList(LPCWSTR strFileName, bool& bTruncated);
	bool ParseImports(cAsBwListRules& pList, char* pBuffer, bool& bTruncated);

protected:
	typedef CVectorView TBase;

	bool OnClicked(CItemBase* pItem);
	bool IsEqual(cSerializable *pRec1, cSerializable *pRec2);
};

class CAntiSpamAdvSettDlg : public CCustomLevelDlg
{
public:
	typedef CCustomLevelDlg TBase;
	CAntiSpamAdvSettDlg(CProfile * pProfilesList, CProfile * pProfile) : CCustomLevelDlg(pProfilesList, pProfile) {}
	
protected:
	virtual void OnChangedData(CItemBase* pItem);
};*/

//#include "pid_gui_fix.h"

//////////////////////////////////////////////////////////////////////
// CUpdateSrcList

class CUpdateSrcList : public CVectorView, public CBrowseCallBack
{
protected:
	tERROR   OnEditItem(CItemBase * pOpCtl);
	bool     FilterObject(CObjectInfo& pObject);
	bool     OnCanAddObject(CBrowseObjectInfo& pObject);
	bool     OnCanOperateItem(cRowId &nItem, eOpCode nOp);
};

/*//////////////////////////////////////////////////////////////////////
// CIPv6AddrEditItem

class CIPv6AddrEditItem : public CItemSinkT<CEditItem>
{
	virtual tDWORD OnBindingFlags()			{ return ITEMBIND_F_VALUE; }
	virtual tTYPE_ID OnGetType()			{ return (tTYPE_ID)cIPv6::eIID; }
	virtual bool OnSetValue(tPTR pValue);
	virtual bool OnGetValue(tPTR pValue);
};

//////////////////////////////////////////////////////////////////////
// CIPv6MaskEditItem

class CIPv6MaskEditItem : public CItemSinkT<CEditItem>
{
	virtual tDWORD OnBindingFlags()			{ return ITEMBIND_F_VALUE; }
	virtual tTYPE_ID OnGetType()			{ return (tTYPE_ID)cIPv6::eIID; }
	virtual bool OnSetValue(tPTR pValue);
	virtual bool OnGetValue(tPTR pValue);
};

//////////////////////////////////////////////////////////////////////
// CAntihackerAdvSettDlg

class CAHFWAddrList : public CSerVectorView<cFWAddress>
{
protected:
	bool GetRecord(CRecord &pRecord);
};

//--------------------------------------------------------------------

class CAHFWPortsEditDlg :
	public CSettingsDlg<>
{
public:
	CAHFWPortsEditDlg(cVector<cFWPort> *pPorts);

	SINK_MAP_BEGIN()
		SINK_STATIC("Ports", m_Ports)
	SINK_MAP_END(CSettingsDlg<>)

protected:
	virtual void OnInited();
	virtual bool OnOk();
	
	CItemSinkT<>       m_Ports;
	cVector<cFWPort> * m_pPorts;
};

//--------------------------------------------------------------------

class CAHFWIcmpMsgEditDlg : public CSettingsDlg<>
{
public:
	CAHFWIcmpMsgEditDlg(cSerializable* pStruct);

protected:
	virtual void OnInit();
};

//--------------------------------------------------------------------

class CAHFWPortProtocolEditDlg : public CSettingsDlg<>
{
public:
	CAHFWPortProtocolEditDlg(cSerializable* pStruct);

protected:
	virtual void OnInit();
};*/

//--------------------------------------------------------------------

class CResolveIpDlg : public CDialogSink
{
public:
	CResolveIpDlg(cSerializable* pStruct);

protected:
	void OnEvent(tDWORD nEventId, cSerializable* pData);
	void OnTimerRefresh(tDWORD nTimerSpin);
	void OnInited();

protected:
	CItemBase *   m_pProgressBar;
	cFwAddress *  m_pAddr;
	tUINT         m_idResolveRequest;
	tUINT         m_nProgress;
	tUINT         m_nDontShowPeriod;
};

/*//--------------------------------------------------------------------

bool FwRuleProcessClicked(CItemBase* pItem, cFwBaseRule *pRule);

class CAntiHackerRulesImpExp : public CListImpExpHelper
{
public:
	CAntiHackerRulesImpExp(CVectorView * v) :
		CListImpExpHelper(v->Item()), m_pVect(v), m_nPos(0) {}
	
	bool Import() { return CListImpExpHelper::Import("ini"); }
	bool Export() { return CListImpExpHelper::Export("ini"); }

protected:
	bool CheckAppPresent(cStringObj &strFileName);

protected:
	tERROR OnExportPrepare(tObjPath& strFileName);
	tERROR OnExportItem(int nPos);
	tERROR OnImport(tObjPath& strFileName, bool bAppend);

	cAutoObj<cIO>    m_hIo;
	tDWORD           m_nPos;
	tString          m_strBuff;
	CVectorView *    m_pVect;
};

template <class T>
class CFwImpExpList : public CSerVectorView<T>
{
	bool OnClicked(CItemBase* pItem)
	{
		sswitch(pItem->m_strItemId.c_str())
		stcase(Export) CAntiHackerRulesImpExp(this).Export(); return true; sbreak;
		stcase(Import) CAntiHackerRulesImpExp(this).Import(); return true; sbreak;
		send;
		return CSerVectorView<T>::OnClicked(pItem);
	}
};

//--------------------------------------------------------------------

class CFwZoneEditDlg : public CDialogBindingT<>
{
public:
	CFwZoneEditDlg(cSerializable * pStruct, tDWORD nFlags);

protected:
	virtual void OnInited();
	virtual bool OnClicked(CItemBase* pItem);

	cAHFWNetwork * m_pNetwork;
};

//--------------------------------------------------------------------

class CFwRuleDlg : public CDialogBindingT<>
{
public:
	CFwRuleDlg(cSerializable* pStruct, tDWORD nFlags = 0);

public:
	bool DoModal(CItemBase* pParent, bool bNew = false);

protected:
	virtual void OnInited();
	virtual bool OnOk();
	virtual bool OnClicked(CItemBase* pItem);
	void         UpdatePropertiesList();
	
	cFwBaseRule *                       m_pRule;
	tDWORD                              m_nFlags;
};

//--------------------------------------------------------------------

template <class T>
class CFwSortList : public CFwImpExpList<T>
{
	typedef CFwImpExpList<T> TBase;
	using TBase::m_pBinding;
	using TBase::m_bEditNew;

protected:
	virtual int SortCompare(int nColumn, CRecord &pRecord1, CRecord &pRecord2)
	{
		const cSerDescriptorField *pField = m_pBinding->GetFieldDescriptor(nColumn);
		if(pField && (pField->m_flags & SDT_FLAG_VECTOR) && !(pField->m_flags & SDT_FLAG_POINTER))
		{
			if(pField->m_id == cFWAddress::eIID || pField->m_id == cFWPort::eIID)
			{
				// sort by the first element of the vector
				tPTR pe1 = TBase::GetRecordPtr(pRecord1.m_nPos);
				tPTR pe2 = TBase::GetRecordPtr(pRecord2.m_nPos);
				if(!pe1 || !pe2)
					return 0;

				tPTR pEl1 = ((tMemChunk *)((tBYTE*)pe1 + pField->m_offset))->m_ptr;
				tPTR pEl2 = ((tMemChunk *)((tBYTE*)pe2 + pField->m_offset))->m_ptr;

				if(pEl1 == NULL && pEl2 == NULL)
					return 0;
				if(pEl1 != NULL && pEl2 == NULL)
					return 1; 
				if(pEl1 == NULL && pEl2 != NULL)
					return -1; 

				if(pField->m_id == cFWAddress::eIID)
					return CFwRuleUtils::CompareAddrs(*(cFWAddress*)pEl1, *(cFWAddress*)pEl2);
				else
					return CFwRuleUtils::ComparePorts(*(cFWPort*)pEl1, *(cFWPort*)pEl2);
			}
		}
		return m_pBinding->CompareRecords(nColumn, pRecord1, pRecord2);
	}
	
	tERROR OnEditItem(CItemBase * pOpCtl)
	{
		if( pOpCtl && pOpCtl->m_pParent && pOpCtl->m_pParent->m_strItemType == GUI_ITEMT_RICHEDIT )
			return FwRuleProcessClicked(pOpCtl, &TBase::EditItem()) ? errOK : errNOT_OK;

		if( !CFwRuleDlg(&TBase::EditItem()).DoModal(*this, m_bEditNew) )
			return errNOT_OK;
		return errOK;
	}
};

//--------------------------------------------------------------------

class CFwAppRulesList : public CFwImpExpList<cFwAppRule>
{
	bool   OnListEvent(int nItem, int nColumn, int nEvent);
	tERROR OnEditItem(CItemBase * pOpCtl);
};*/

/*typedef CFwSortList<cFwBaseRule>     CFwRulesList;
typedef CFwSortList<cFwAppRuleEx>    CFwAppRulesListEx;
typedef CFwSortList<cFwPacketRule>   CFwPortRulesList;

//--------------------------------------------------------------------

class CFwAppRulesDlg : public CDialogBindingT<>
{
public:
	CFwAppRulesDlg(cSerializable *pStruct, CProfile *pPdmProfile, cAHFWAppEventReport *pAppEvent = NULL);

	SINK_MAP_BEGIN()
		SINK_STATIC_LIST("AppList", m_AppRulesLst)
	SINK_MAP_END(CDialogBindingT<>)

public:
	bool DoModal(CItemBase* pParent, bool bNew = false);
	
protected:
	virtual bool OnOk();
	virtual void OnInited();
	virtual bool OnMenu(CPopupMenuItem * pMenu, CItemBase * pItem);
	
	void         AddPdmRule();
	bool         IsPdmRuleExist();
	void         RemovePdmRules();

	CFwRulesList         m_AppRulesLst;
	CProfile *           m_pPdmProfile;
	cAHFWAppEventReport *m_pAppEvent;
};

//--------------------------------------------------------------------

class CAntihackerAdvSettDlg : public CProfileSettingsDlg
{
public:
	typedef CProfileSettingsDlg TBase;

	CAntihackerAdvSettDlg(CProfile * pProfilesList, CProfile * pProfile) : TBase(pProfilesList, pProfile), m_bNeedConvertRules(false) {}

	SINK_MAP_BEGIN()
		SINK_STATIC_LIST("AppGrpList", m_AppRulesLst)
		SINK_STATIC_LIST("AppList",    m_AppRulesLstEx)
		SINK_STATIC_LIST("PortList",   m_PacketRulesLst)
		SINK_STATIC_LIST("NetList",    m_NetworkLst)
	SINK_MAP_END(TBase)

protected:
	virtual void OnInit();
	virtual void OnInited();
	virtual bool OnOk();
	virtual void OnChangedData(CItemBase* pItem);
	virtual void OnCustomizeData(cSerializable * pData, bool bIn);

	void         Switch2GroupView(bool bGroupView);

	bool                   m_bNeedConvertRules;
	cVector<cFwAppRuleEx>  m_AppRulesEx;
	CFwAppRulesListEx      m_AppRulesLstEx;
	CFwAppRulesList        m_AppRulesLst;
	CFwPortRulesList       m_PacketRulesLst;
	CFwNetworkList         m_NetworkLst;
};

//////////////////////////////////////////////////////////////////////
// CNetHostEditDlg

class CNetHostsList : public CSerVectorView<cIP>
{
protected:
    bool GetRecord(CRecord &pRecord);
};

class CNetHostsEditDlg : public CDialogBindingT<>, public CNetHostsList::Cb
{
public:
    typedef CDialogBindingT<> TBase;
    CNetHostsEditDlg(cSerializable * pStruct, tDWORD nFlags);

    SINK_MAP_BEGIN()
        SINK_STATIC("List", m_List)
    SINK_MAP_END(TBase)

protected:
    virtual void OnCreate();
    virtual void OnInited();
    virtual bool OnEditItem(CItemBase * pList, CItemBase * pOpCtl, cIP& Item, bool bNew);
    virtual void OnChangedState(CItemBase * pItem, tDWORD nStateMask = ISTATE_ALL);

    cBLTrustedApp*  m_pApp;
    cIP             m_Host;
    CNetHostsList   m_List;
};

//////////////////////////////////////////////////////////////////////
// CNetPortsEditDlg

class CNetPortsEditDlg :
    public CSettingsDlg<>
{
public:
    typedef CSettingsDlg<> TBase;

    CNetPortsEditDlg(cSerializable * pStruct, tDWORD nFlags);

    SINK_MAP_BEGIN()
        SINK_STATIC("Ports", m_Ports)
    SINK_MAP_END(CSettingsDlg<>)

protected:
    virtual void OnInited();
    virtual bool OnOk();
    tString GetPortListStr(const cVector<tWORD> &Ports);
    bool    ParsePortListStr(LPCTSTR sPorts, cVector<tWORD> &aPorts);

    CItemSinkT<>    m_Ports;
    cBLTrustedApp*  m_pApp;
};

//////////////////////////////////////////////////////////////////////
// CBannerList

class CBannerList : public CVectorView
{
protected:
	bool OnClicked(CItemBase* pItem);
};

//////////////////////////////////////////////////////////////////////
// CUrlFltProfileAccountsList

class CUrlFltProfileAccountsList : public CSerVectorView<cGuiParams>
{
public:
	typedef CSerVectorView<cGuiParams> TBase;

	CUrlFltProfileAccountsList() : m_nProfileId(-1) {}

protected:
	tTYPE_ID OnGetType() { return (tTYPE_ID)tid_DWORD; }
	bool     OnIsVector() { return false; }
	bool     OnGetValue(cVariant &pValue) { return false; }

	bool     OnSetValue(const cVariant &pValue);
	void     OnInit();
	bool     OnDeleteRecord(CRecord &pRecord);
	tERROR   OnEditItem(CItemBase * pOpCtl);

	tDWORD						m_nProfileId;
	cVector<cGuiParams>			m_Accounts;
	cSerObj<cUrlFltSettings>	m_users;
};

//////////////////////////////////////////////////////////////////////
// CUrlFltProfilesCombo

class CUrlFltProfilesCombo : public CItemSinkT<CComboItem>
{
protected:
	typedef CItemSinkT<CComboItem>	TBase;

	void OnInit();
	void OnInitProps(CItemProps& pProps);
	void _ReinitItems();

	cSerObj<cUrlFltSettings> m_sett;
	tString m_strFmt;
};

//////////////////////////////////////////////////////////////////////
// CParCtlSwitchProfileDlg

class CParCtlSwitchProfileDlg : public CDialogSink
{
public:
	typedef CDialogSink TBase;

	CParCtlSwitchProfileDlg() : m_pProfile(NULL) {}
	
	void UpdateData(bool bIn) { CStructData _ds(NULL,STRUCTDATA_ALLSECTIONS); m_pBinding->UpdateData(bIn,_ds); }

	void OnCreate();
	void OnInit();
	bool OnOk();
	
	CProfile *                 m_pProfile;
	cSerObj<cUrlFltSettings>   m_sett;
	cSerObj<cUrlFltStatistics> m_stat;
	cGuiParams                 m_data;
};

//////////////////////////////////////////////////////////////////////////
// CCustomSupportLink

class CCustomSupportLink : public CItemSinkT<>
{
protected:
	typedef CItemSinkT<> TBase;
	
	tDWORD OnBindingFlags() { return ITEMBIND_F_OWN; }
	void OnEvent(tDWORD nEventId, cSerializable * pData);
	void OnInit();

	void UpdateLinks();
	
	std::vector<CItemBase *> m_aLinks;
};*/

/*
//////////////////////////////////////////////////////////////////////
// CPDM2FakeList

class CPDM2FakeList : public CSerVectorView<cPDM2rtSettings>
{
protected:
	void OnInit();
	void OnInited();
	bool OnDeleteRecord(CRecord &pRecord) { return false; }
	void OnChangedData(CItemBase * pItem);

	cVector<cPDM2rtSettings> m_Tasks;
	cSerObj<cPDM2rtSettings> m_sett;
};

//////////////////////////////////////////////////////////////////////
// CProductInfoWindow

class CProductInfoWindow : public CDialogBindingViewT<>
{
    typedef CDialogBindingViewT<> TBase;
public:
    CProductInfoWindow();

protected:
	void OnInit();
	bool OnClicked(CItemBase *pItem);

    void Show();
    bool m_bCredits;
};*/


/////////////////////////////////////////////////////////////////////////////
// CSendHelper

class CSendHelper : public cThreadPoolBase, public cThreadTaskBase
{
public:
	CSendHelper() : cThreadPoolBase("SendHelper"), m_pState(NULL), m_pDumps(NULL) {}
	bool Send(cDumpFiles* dumps, tDWORD* pState);
	cGuiErrorInfo* GetError() { return &m_err; }

protected:
	cDumpFiles*		m_pDumps;
	tDWORD*			m_pState;
	cGuiErrorInfo	m_err;

	void do_work();
	void on_exit();
};

//////////////////////////////////////////////////////////////////////////
// CSendTroublesDialog

class CSendTroublesDialog : 
	public CDialogBindingViewT<cDumpFiles>, 
	public cDumpFiles
{
public:
	typedef CDialogBindingViewT<cDumpFiles> TBase;

	CSendTroublesDialog(tBOOL bSendDumps);
	~CSendTroublesDialog();
	
	bool GetNewDumps();
	bool CheckInetConnection();
	
	void OnInit();	
	void OnEvent(tDWORD nEventId, cSerializable * pData);
	bool OnClicked(CItemBase *pItem);
	void OnTimerRefresh(tDWORD nTimerSpin);

	enum SendState { SS_DONE = 0, SS_PREPARING, SS_CREATE_SYSINFO, SS_MAKING_ZIP, SS_SENDING_FTP };

protected:
	BIND_MEMBER_MAP_BEGIN()
		BIND_VALUE(m_SendState, tid_DWORD, "SendState")
		BIND_VALUE(m_SendPercent, tid_DWORD, "SendPercent")
	BIND_MEMBER_MAP_END(TBase)
	
	tDWORD			m_SendState;
	tDWORD			m_SendPercent;
	cDateTime		m_dtLastChecked;
	CSendHelper		m_SendHelper;
};

//////////////////////////////////////////////////////////////////////
// CNetFrameDetail

class CNetFrameDetail : public CListItemDescr
{
public:
	CNetFrameDetail();

protected:
	BIND_MEMBER_MAP_BEGIN()
		BIND_VALUE(m_FrameDetails, cSnifferFrameDetailsItem::eIID, "FrameDetails")
		BIND_VECTOR(m_FrameBin,    tid_BYTE,                       "FrameBin")
	BIND_MEMBER_MAP_END(CListItemDescr)

	bool SetItemData(cSer *pData);

protected:
	cSnifferData             m_FrameBin;
	cSnifferFrameDetailsItem m_FrameDetails;

private:
	cReportDB*		m_pReport;
	cAutoObj<cTask> m_NetParser;
	bool			m_bNetParserCreated;

	bool CheckCreateNetParser();
};

//////////////////////////////////////////////////////////////////////////
// CNetDetailsListSink

class CNetDetailsListSink : public CSerTreeView
{
public:
	void SelectNodeByDataPos(int pos);
	bool OnListEvent(eListEvents nEvent);
private:
	void FindNodeIndxByDataPos(const cRowNode* pRNode, tDWORD dwPos, cRowId &nItem);
};

//////////////////////////////////////////////////////////////////////////
// CHexSnifferSink

class CHexSnifferSink : public CHexViewSink
{
public:
	CHexSnifferSink() : m_pNetDtlsSnk(NULL) {}

	// CItemSink
	void OnInited();
	// CHexViewSink
	void OnPosSelected(int pos);
	void OnNextPrev(bool bNext);

private:
	CNetDetailsListSink* m_pNetDtlsSnk;
};

//////////////////////////////////////////////////////////////////////////
// CCmnStatisticList

class CCmnStatisticList : public CVectorView
{
public:
	CCmnStatisticList();
	
	void OnTimerRefresh(tDWORD nTimerSpin);

protected:
	BIND_MEMBER_MAP_BEGIN()
		BIND_VECTOR(m_vCmnStat, cCmnStatisticItem::eIID, "CmnStat")
	BIND_MEMBER_MAP_END(CVectorView)

	cVector<cCmnStatisticItem> m_vCmnStat;

	void UpdateStatistics();

private:
	tDWORD	m_eDbType;
	tDWORD	m_nLastChecked;
	tBOOL	m_bFirstCheck;

	struct cStat
	{
		tDWORD m_Total, m_Treated;
		cStat() : m_Total(0), m_Treated(0) {}
	};
	
	typedef std::map<tDWORD, cStat> MAP_STAT;

	
	tINT FindIndex(tDWORD DetectType);
};


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_SETTINGSDLGS_H__AA90CADE_8429_4816_AA5B_1C67B4785355__INCLUDED_)
