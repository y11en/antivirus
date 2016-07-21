// SettingsDlgs.cpp: implementation of the SettingsDlgs class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TaskView.h"
#include "ListsView.h"
#include "SettingsDlgs.h"

#include <structs/s_antiphishing.h>

#include <algorithm>

#define IMPEX_IMPORT_LIB
#include <plugin/p_win32_nfio.h>
#include <plugin/p_miniarc.h>
#include <plugin/p_minizip.h>
#include <PPP/common/SpecialName.h>
#include <ProductCore/GetSystemInfoMain.h>

#ifdef _WIN32
 #include <Wininet.h>
#endif


//////////////////////////////////////////////////////////////////////

#define ITEMID_RULEDESC                 "Rule"
#define ITEMID_PROTOCOL                 "Protocol"
#define ITEMID_ICMPCODE                 "ICMPCode"
#define ITEMID_COMPUTER                 "Computer"
#define ITEMID_ADDRSINGLE               "AddressSingle"
#define ITEMID_ADDRRANGE                "AddressRange"
#define ITEMID_ADDRSUBNET               "AddressSubnet"
#define ITEMID_RANGE                    "Range"
#define ITEMID_SUBNET                   "Subnet"
#define ITEMID_PROGRESS                 "Progress"
#define ITEMID_CSPAMRATE                "CertainSpamRate"
#define ITEMID_PSPAMRATE                "PossibleSpamRate"
#define ITEMID_CSPAMSLIDER              "CertainSpamSlider"
#define ITEMID_PSPAMSLIDER              "PossibleSpamSlider"
#define ITEMID_PDMRULE                  "PDMRule"

// AHFW items
#define ITEMID_REMOTEADDRSLISTEDIT      "RemoteAddrsListEdit"
#define ITEMID_LOCALADDRSLISTEDIT       "LocalAddrsListEdit"
#define ITEMID_REMOTEPORTSLISTEDIT      "RemotePortsListEdit"
#define ITEMID_LOCALPORTSLISTEDIT       "LocalPortsListEdit"

#define STRID_APPRULEDEFNAME            "ahfwAppRuleDefName"
#define STRID_PACKETRULEDEFNAME         "ahfwPacketRuleDefName"
#define STRID_SPECIFYADDRESS            "ahfwSpecifyAddress"
#define STRID_SPECIFYPORT               "ahfwSpecifyPort"
#define STRID_ERRNEEDAPP                "ahfwErrNeedApp"
#define STRID_ERRNEEDPROP               "ahfwErrNeedProp"
#define STRID_ERRINVALIDADDRESS         "ahfwErrInvalidAddress"
#define STRID_ERRNEEDRULENAME           "ahfwErrNeedRuleName"
#define STRID_ERRRULESNOTMATCH          "ahfwErrRulesNotMatch"
#define STRID_ERRPDMNOTLOADED           "errPDMNotLoaded"
#define STRID_ERRCALCHASH               "errCalcHash"
#define STRID_ERRINVALIDPORT            "ahfwErrInvalidPort"

#define PROFILE_SID_ANTIHACKER_LOC      "Anti_Hacker_Localize"
#define PROFILE_SID_AH_PROTOCOLS        "FwProtocols"
#define PROFILE_SID_AH_PORT_ICMPCODES   "FwIcmp"
#define PROFILE_SID_AH_RESOLVEIP        "SettingsWindow.Anti_Hacker.ResolveIp"

#define ITEMID_FWBLOCKEDIT              "FwBlockEdit"
#define ITEMID_FWDIRECTIONEDIT          "FwDirectionEdit"
#define ITEMID_FWTIMERANGEEDIT          "FwTimeRangeEdit"
#define ITEMID_FWPROTOCOLEDIT           "FwProtocolEdit"
#define ITEMID_FWICMPMSGEDIT            "FwIcmpMsgEdit"

#define EVENTID_TESTCONNECTIONFINISHED   0xbc6c265 //is a crc32 for "EVENTID_TESTCONNECTIONFINISHED"
#define EVENTID_SEND_TROUBLES_COMPLETE	 0xa74ba426 //is a crc32 for "SEND_TROUBLES_COMPLETE_STAGE"
#define EVENTID_SYSINFO_CALLBACK		 0x9098e20d //is a crc32 for "EVENTID_SYSINFO_CALLBACK"

#define STRID_ERR_RG_NO_ANY_KEYS		"ErrNoAny_RG_Keys"
#define STRID_ERR_RG_NO_ANY_APPS		"ErrNoAny_RG_Apps"
#define STRID_ERR_RG_UNKNOWN_APP		"ErrUnknown_RG_AppPath"
#define STRID_MAIL_ADDRESS_TRUNCTATED	"WarnAddressTruncated"
#define STRID_APP_DLL_ALREADY_EXIST		"AppDllAlreadyExistMsg"

#define MAX_MAIL_ADDRESS_LEN			100


//////////////////////////////////////////////////////////////////////////
// CRegistryBrowseCallBack

bool CRegistryBrowseCallBack::OnCustomizeObject(bool bDir, CBrowseObjectInfo& pObject)
{
	if( !pObject.m_pData )
		return false;

	cRegKeyInfo * pRkeyInfo = (cRegKeyInfo *)pObject.m_pData;
	if( bDir )
	{
		if( pObject.m_nType == SHELL_OBJTYPE_REGVALUE )
		{
			int nPos = pObject.m_strName.find_last_of(pObject.m_strShortName.data());
			if( nPos != tObjPath::npos )
				pRkeyInfo->m_KeyPath = pObject.m_strName.substr(0, nPos - pObject.m_strShortName.length());
			pRkeyInfo->m_KeyValue	= pObject.m_strShortName;
		}
		else
			pRkeyInfo->m_KeyPath = pObject.m_strName;
	}
	else
	{
		pObject.m_strName = pRkeyInfo->m_KeyPath;
		pObject.m_strShortName = pRkeyInfo->m_KeyValue;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// CResourceList

bool CResourceList::OnSetValue(cResource * pGroup)
{
	m_pGroup = pGroup;
	cVectorData vd; vd.m_type = (tTYPE_ID)cResource::eIID; vd.m_v = (tMemChunk *)&m_pGroup->m_vChilds;
	cVariant var; var.Init(vd);
	return CVectorViewT<cResource>::OnSetValue(var);
}

tERROR CResourceList::OnEditItem(CItemBase * pOpCtl)
{
	if( !IsEditNewItem() )
		return BrowseObject(((cResource *)m_pEditItem)->m_pData);

	m_bResult = cFALSE;
	if( pOpCtl->m_pBinding )
	{
		cVariant result;
		pOpCtl->m_pBinding->ProcessReflection(CItemBinding::hOnClick, result);
	}
	return m_bResult ? errOK : errNOT_OK;
}

bool CResourceList::OnClicked(CItemBase * pItem)
{
	if( m_pEditItem && m_pEditItem->isBasedOn(cResource::eIID) )
	{
		tDWORD nSerID = cSer::eIID;
		
		sswitch(pItem->m_strItemId.c_str())
		scase("File")       nSerID = cFileInfo::eIID;   sbreak;
		scase("Registry")   nSerID = cRegKeyInfo::eIID; sbreak;
		scase("WebService") nSerID = cWebService::eIID; sbreak;
		scase("IpRange")    nSerID = cIpRange::eIID;    sbreak;
		send;

		if( nSerID != cSer::eIID && m_pGroup )
		{
			cResource *pRes = (cResource *)m_pEditItem;
			pRes->m_pData.init(nSerID);
			pRes->m_nParentID = m_pGroup->m_nID;
			pRes->m_nID = m_sett->GetNextResID();

			tERROR err = BrowseObject(pRes->m_pData);
			if( err != errNOT_IMPLEMENTED )
				m_bResult = PR_SUCC(err);
			else
				if( pItem->m_pBinding )
				{
					cVariant result;
					pItem->m_pBinding->ProcessReflection(CItemBinding::hOnClick, result);
					m_bResult = result;
				}
			return true;
		}
	}
	return CVectorView::OnClicked(pItem);
}

void CResourceList::OnChangedData(CItemBase * pItem)
{
	CVectorViewT<cResource>::OnChangedData(pItem);
	if( m_grpList )
		m_grpList->m_pItem->SetChangedData();
}

tERROR CResourceList::BrowseObject(cSer *pObject)
{
	if( !pObject )
		return errPARAMETER_INVALID;
	if( pObject->isBasedOn(cFileInfo::eIID) )
	{
		cFileInfo *fi = (cFileInfo *)pObject;

		CBrowseObjectInfo boi("BrowseForFile", SHELL_OBJTYPE_FILE|SHELL_OBJTYPE_FOLDER|SHELL_OBJTYPE_MASK, *this, fi, NULL);
		boi.m_strName = fi->m_Path;
		if( Root()->BrowseObject(boi) )
		{
			fi->m_Path = boi.m_strName;
			return errOK;
		}
		return errNOT_OK;
	}
	
	if( pObject->isBasedOn(cRegKeyInfo::eIID) )
	{
		CBrowseObjectInfo boi("BrowseForRegistry", SHELL_OBJTYPE_REGKEY|SHELL_OBJTYPE_REGVALUE|SHELL_OBJTYPE_MASK, *this, pObject, this);
		return Root()->BrowseObject(boi) ? errOK : errNOT_OK;
	}
	
	return errNOT_IMPLEMENTED;
}

//////////////////////////////////////////////////////////////////////////
// CResourceGroupList

void CResourceGroupList::OnInited()
{
	if( CFieldsBinder *pBinder = m_pItem->GetBinder() )
		pBinder->GetDataSource(TOR(CStructId, (CHipsSettings::eIID)), (cSer **)&m_sett);
	
	CListItem *pList = (CListItem *)Item()->GetItemByPath("parent.ResList");
	if( pList && pList->m_strItemType == GUI_ITEMT_LISTCTRL )
		m_pResList = (CResourceList *)pList->m_pSink;

	if( m_pResList )
	{
		m_pResList->m_sett = m_sett;
		m_pResList->m_grpList = this;
	}

	CSerTreeView::OnInited();
}

bool CResourceGroupList::OnListEvent(eListEvents nEvent)
{
	if( m_pResList && nEvent == leFocusChanged )
	{
		cResource * pRg = (cResource *)GetEditRowData();
		if( pRg && pRg->isBasedOn(cResource::eIID) )
			m_pResList->OnSetValue(pRg);
	}
	return CSerTreeView::OnListEvent(nEvent);
}

bool CResourceGroupList::OnPostOperateItem(cRowId &nItem, eOpCode nOp, tPTR pValue)
{
	if( nOp == opEdit && IsEditNewItem() )
	{
		cResource * pRes = (cResource *)pValue;
		pRes->m_nID = m_sett->GetNextResID();
	}
	return CListViewSink::OnPostOperateItem(nItem, nOp, pValue);
}

//////////////////////////////////////////////////////////////////////////
// CAppGroupRuleList

CAppGroupRuleList::CAppGroupRuleList() :
	m_sett(NULL),
	m_updatingCols(false)
{}

bool CAppGroupRuleList::OnSetType(tTYPE_ID nType)
{
	if( nType != cAppGroup::eIID )
		return false;

	bool res = CSerTreeView::OnSetType((tTYPE_ID)cGuiAppGroup::eIID);
	m_nSerID = (tTYPE_ID)cAppGroup::eIID;
	return res;
}

bool CAppGroupRuleList::OnSetValue(const cVariant &pValue)
{
	if( m_sett )
		UpdateColumns(m_sett->m_Resource);
	return CSerTreeView::OnSetValue(pValue);
}

void CAppGroupRuleList::OnInit()
{
	if( CFieldsBinder *pBinder = m_pItem->GetBinder() )
	{
		CStructId si;
		si.m_serId = CHipsSettings::eIID;

		pBinder->GetDataSource(si, (cSer **)&m_sett);
	}
	
	CSerTreeView::OnInit();
}

bool CAppGroupRuleList::OnDragRow(cRowId& row, cRowId& rowto, bool bEnd)
{
	if( !bEnd )
		return rowto.IsNode();
	
	cAppGroup * pApp = (cAppGroup *)Item()->GetRowData(row);
	cAppGroup * pDst = (cAppGroup *)Item()->GetRowData(rowto);
	return false;
}

bool CAppGroupRuleList::GetRowInfo(cSer *pNode, tIdxArr &pIdx)
{
	if( m_sett )
	{
		const cApplications &app = m_sett->m_vAppList;
		const cAppGroup &ag = *(cAppGroup*)pNode;
		for(size_t i = 0, n = app.size(); i < n; i++)
			if( ag.m_nAppGrpID == app[i].m_AppGrId )
				pIdx.push_back(i);
	}
	return true;
}

cSer * CAppGroupRuleList::GetRowData(tIdx nItem)
{
	if( m_sett )
	{
		m_item.m_pApp = &m_sett->m_vAppList[nItem];
		return &m_item;
	}
	return NULL;
}

cSer * CAppGroupRuleList::GetTreeData(tIdxArr &pIdx, bool bIsNode)
{
	cSer *ser = CSerTreeView::GetTreeData(pIdx, bIsNode);
	if( !bIsNode )
		return ser;
	if( !ser )
		return NULL;

	m_item.assign(*ser, false);
	m_item.m_pApp = NULL;
	return &m_item;
}

bool CAppGroupRuleList::GetCellData(cCellData& cell)
{
	tDWORD appID = m_item.m_pApp ? m_item.m_pApp->m_AppId : m_item.m_nAppGrpID;
	if( cell.m_col->m_nID )
	{
		m_item.m_nResId = cell.m_col->m_nID;
		m_item.m_nState = m_sett ? m_sett->GetState(appID, !m_item.m_pApp, m_item.m_nResId, true) : HIPS_RULE_STATE_UNK;
	}	
	return true;
}

void CAppGroupRuleList::OnChangedData(CItemBase * pItem)
{
	if( m_sett && pItem->m_strItemId == "HipsRuleColumn.Menu" )
	{
		tDWORD appID = m_item.m_pApp ? m_item.m_pApp->m_AppId : m_item.m_nAppGrpID;
		m_sett->SetState(appID, !m_item.m_pApp, m_item.m_nResId, true, m_item.m_nState);
		Item()->SetChangedData();
	}
	CSerTreeView::OnChangedData(pItem);
}

void CAppGroupRuleList::UpdateColumns(cResource &pGroup)
{
	if( m_updatingCols )
		return;
	
	m_updatingCols = true;

	cIndexes aIdxs;
	aIdxs.reserve(m_cols.size());

	for(cColumnMap::iterator i = m_cols.begin(), n = m_cols.end(); i != n; i++)
		aIdxs.push_back(i->first);

	CGuiPtr<CFieldsBinder> pBinder = Root()->CreateFieldsBinder(cSerializableObj::getDescriptor(cResource::eIID));
	CGuiPtr<cNode> pColName = Root()->CreateNode("s_ResGroupName(Name)", pBinder);

	cColumn *pColumn;
	Item()->GetColData(&pColumn);
	UpdateColumns(pGroup, pColumn, aIdxs, pColName, 0);
	
	while(aIdxs.size())
	{
		cColumn * col = m_cols.find(aIdxs[0])->second;
		RemoveColumn(col, aIdxs);
		col->Destroy();
	}

	m_updatingCols = false;
}

void CAppGroupRuleList::UpdateColumns(cResource &pGroup, cColumn *pParent, cIndexes &aIdxs, cNode *pColName, tDWORD nDeep)
{
	if( nDeep > 2 )
		return;

	if( !(pGroup.m_nFlags & cResource::fRulesView) )
		return;

	if( nDeep )
	{
		tDWORD nID = pGroup.m_nID;
		cColumnMap::iterator it = m_cols.find(nID);
		if( it == m_cols.end() )
		{
			cVariant colName;
			pColName->Exec(colName, cNodeExecCtx(Root(), NULL, &CStructData(&pGroup)));
			pParent = pParent->AddColumn("HipsRuleColumn", colName.c_str(), nID);
			m_cols.insert(cColumnMap::value_type(nID, pParent));
		}
		else
		{
			aIdxs.erase(std::find(aIdxs.begin(), aIdxs.end(), nID));
			pParent = it->second;
		}
	}

	for(size_t i = 0, n = pGroup.m_vChilds.size(); i < n; i++)
		UpdateColumns(pGroup.m_vChilds[i], pParent, aIdxs, pColName, nDeep + 1);
}

void CAppGroupRuleList::RemoveColumn(cColumn * col, cIndexes &aIdxs)
{
	m_cols.erase(m_cols.find(col->m_nID));
	aIdxs.erase(std::find(aIdxs.begin(), aIdxs.end(), col->m_nID));

	for(size_t i = 0, n = col->m_vChilds.size(); i < n; i++)
		RemoveColumn(col->m_vChilds[i], aIdxs);
}

//////////////////////////////////////////////////////////////////////////
// CResRuleList

bool CResRuleList::OnSetType(tTYPE_ID nType)
{
	if( nType != cResource::eIID )
		return false;

	bool res = CSerTreeView::OnSetType((tTYPE_ID)cGuiResRule::eIID);
	m_nSerID = (tTYPE_ID)cResource::eIID;
	return res;
}

void CResRuleList::OnInit()
{
// 	if( cVariant * var = Root()->GetGlobalVar("g_appID") )
// 		m_appID = var->ToDWORD();

	if( CFieldsBinder *pBinder = m_pItem->GetBinder() )
	{
		pBinder->GetDataSource(TOR(CStructId, (CHipsSettings::eIID)), (cSer **)&m_sett);
		
		if( CItemBase *pList = Item()->GetOwner(true)->m_pParent->GetItemByPath("Hips.WorkArea.Rules.Rules") )
			if( pList->m_strItemType == GUI_ITEMT_LISTCTRL )
				if( CListItemSink *pListSink = (CListItemSink *)pList->m_pSink )
					if( cGuiAppGroup *pGrp = (cGuiAppGroup *)pListSink->GetEditRowData() )
						if( pGrp->isBasedOn(cGuiAppGroup::eIID) )
						{
							m_appID = pGrp->m_pApp ? pGrp->m_pApp->m_AppId : pGrp->m_nAppGrpID;
							m_isAppGroup = !pGrp->m_pApp;
						}
	}

	CSerTreeView::OnInit();
}

cSer * CResRuleList::GetTreeData(tIdxArr &pIdx, bool bIsNode)
{
	cSer *ser = CSerTreeView::GetTreeData(pIdx, bIsNode);
	if( !bIsNode || !ser )
		return ser;

	m_item.assign(*ser, false);
	m_sett->GetRule(m_appID, m_isAppGroup, m_item.m_nID, false, NULL, m_item.m_Rule);
	return &m_item;
}

void CResRuleList::OnChangedData(CItemBase * pItem)
{
	if( CItemBase *pOwner = pItem->GetOwner() )
		if( pOwner->m_strItemType == GUI_ITEMT_MENU )
			m_sett->SetRule(m_item.m_Rule);
	CSerTreeView::OnChangedData(pItem);
}

//////////////////////////////////////////////////////////////////////
// CNetworksList

bool CNetworkList::OnClicked(CItemBase* pItem)
{
	if( pItem == m_pBtnEdit && pItem->m_pBinding )
	{
		cRowId nItem;
		if( GetSingleSelectedItem(nItem) && nItem.IsNode() )
		{
			if( cNode * pHolder = pItem->m_pBinding->GetHandler(CItemBinding::hOnClick) )
			{
				cVariant res;
				cNodeExecCtx ctx(Root(), pItem, NULL);
				pHolder->Exec(res, ctx);
			}
			return true;
		}
	}
	return CVectorView::OnClicked(pItem);
}

//////////////////////////////////////////////////////////////////////////
// CAppDialog

cNode *GetNamedArg(const char *name, cNode **m_args, size_t m_argc)
{
	for(size_t i = 0; i < m_argc; i++)
	{
		cNode *node = m_args[i];
		if( node->Type() == ntNamedArg && ((cNodeNamedArg *)node)->m_name == name )
			return node;
	}
	return NULL;
}

CAppDialog::CAppDialog(CSinkCreateCtx * pCtx) :
	m_pid(0),
	m_appID(0),
	m_isAppGroup(cFALSE)
{
	if( cNode *node = GetNamedArg("pid", pCtx->m_args, pCtx->m_argc) )
		m_pid = node->ExecGet(*pCtx->m_ctx).ToQWORD();
	
	if( cNode *node = GetNamedArg("image", pCtx->m_args, pCtx->m_argc) )
		node->ExecGet(*pCtx->m_ctx).ToStringObj(m_processInfo.m_sImagePath);
	
	if( cNode *node = GetNamedArg("appID", pCtx->m_args, pCtx->m_argc) )
		m_appID = node->ExecGet(*pCtx->m_ctx).ToDWORD();
	
	if( cNode *node = GetNamedArg("isAppGroup", pCtx->m_args, pCtx->m_argc) )
		m_isAppGroup = node->ExecGet(*pCtx->m_ctx);

	if( !m_isAppGroup )
	{
//#ifndef _DEBUG
		cProcessMonitor * pPM;
		if( PR_SUCC(g_hTM->GetService(0, (hOBJECT)g_hTM, AVP_SERVICE_PROCESS_MONITOR, (cObject **)&pPM)) )
		{
			if( m_pid )
				pPM->GetProcessInfoEx(m_pid, &m_processInfo);
			else
				pPM->GetProcessInfo(&m_processInfo);
			
			g_hTM->ReleaseService(0, (hOBJECT)pPM);
		}
//#endif
	}
}

void CAppDialog::OnCreate()
{
	Item()->AddDataSource(&m_processInfo);
	CDialogSink::OnCreate();
}

//////////////////////////////////////////////////////////////////////
// CVirtualKbd

CVirtualKbd::CVirtualKbd() : m_nFlags(0), m_bVkbdInited(false), m_hVkbdModule(NULL), m_hLayout(NULL)
{
	if( PrLoadLibrary && PrFreeLibrary && PrGetProcAddress )
	{
		if( m_hVkbdModule = PrLoadLibrary("vkbd.dll", cCP_ANSI) )
		{
			m_pInit             = (tInit)            PrGetProcAddress(m_hVkbdModule, "Init");
			m_pDeinit           = (tDeinit)          PrGetProcAddress(m_hVkbdModule, "Deinit");
			m_pProcessVkbdEvent = (tProcessVkbdEvent)PrGetProcAddress(m_hVkbdModule, "ProcessVkbdEvent");
			m_pGetKeyName       = (tGetKeyName)      PrGetProcAddress(m_hVkbdModule, "GetKeyName");
			m_pGetKeboardLayout = (tGetKeboardLayout)PrGetProcAddress(m_hVkbdModule, "GetKeboardLayout");
			if( m_pInit && m_pDeinit && m_pProcessVkbdEvent && m_pGetKeyName && m_pGetKeboardLayout )
			{
				m_bVkbdInited = !!m_pInit();
				m_hLayout = m_pGetKeboardLayout();
			}
		}
	}
}

CVirtualKbd::~CVirtualKbd()
{
	if( m_hVkbdModule )
	{
		if( m_bVkbdInited )
			m_pDeinit();
		PrFreeLibrary(m_hVkbdModule);
	}
}

void CVirtualKbd::OnInit()
{
	InitKeyboard();
	UpdateLayout();
}

void CVirtualKbd::OnInited()
{
}

bool CVirtualKbd::OnClicked(CItemBase *pItem)
{
	return CDialogSink::OnClicked(pItem);
}

void CVirtualKbd::OnChangedState(CItemBase * pItem, tDWORD nStateMask)
{
	if( nStateMask & ISTATE_SELECTED )
	{
		tDWORD vk = (tDWORD)pItem->m_pUserData;
		tDWORD keyStateFlag = 0;
#ifndef __unix__
		switch(vk)
		{
		case VK_SHIFT:
		case VK_LSHIFT:
		case VK_RSHIFT:
			keyStateFlag = MOD_SHIFT;
			break;
		case VK_CONTROL:
		case VK_LCONTROL:
		case VK_RCONTROL:
			keyStateFlag = MOD_CONTROL;
			break;
		case VK_MENU:
		case VK_LMENU:
		case VK_RMENU:
			keyStateFlag = MOD_ALT;
			break;
		case VK_CAPITAL:
			keyStateFlag = MOD_CAPITAL;
			break;
		}
		if( keyStateFlag )
		{
			if( pItem->m_nState & ISTATE_SELECTED )
				m_nFlags |= keyStateFlag;
			else
				m_nFlags &= ~keyStateFlag;
			if( keyStateFlag == MOD_SHIFT || keyStateFlag == MOD_CAPITAL )
				UpdateLayout();
		}
		m_pProcessVkbdEvent((vk << 16) | m_nFlags, !!(pItem->m_nState & ISTATE_SELECTED));
#endif
	}
}

void CVirtualKbd::OnTimerRefresh(tDWORD nTimerSpin)
{
	if( !(nTimerSpin % 5) )
	{
		if( m_pGetKeboardLayout )
		{
			const void *hLayout = m_pGetKeboardLayout();
			if( m_hLayout != hLayout )
			{
				m_hLayout = hLayout;
				UpdateLayout();
			}
		}
	}
}

static const char *next_field(const char *str, char delim = ',')
{
	if( str )
		str = strchr(str, delim);
	return str ? str + 1 : str;
}

LPCSTR CVirtualKbd::GetKeyName(tDWORD vk)
{
	if( !m_bVkbdInited )
		return "";

// platform dependent!
#ifndef __unix__
	switch(vk)
	{
	case VK_SHIFT:
	case VK_LSHIFT:
	case VK_RSHIFT:     return "Shift";
	case VK_CONTROL:
	case VK_LCONTROL:
	case VK_RCONTROL:   return "Ctrl";
	case VK_MENU:
 	case VK_LMENU:
 	case VK_RMENU:      return "Alt";
	case VK_CAPITAL:    return "Caps";
	case VK_TAB:        return "Tab";
	case VK_RETURN:     return "Enter";
	case VK_INSERT:     return "Ins";
	case VK_DELETE:     return "Del";
	case VK_HOME:       return "Hm";
	case VK_END:        return "End";
	case VK_PRIOR:      return "PUp";
	case VK_NEXT:       return "PDn";
	}
#endif
	return m_pGetKeyName((vk << 16) | m_nFlags, m_hLayout);
}

void CVirtualKbd::InitKeyboard()
{
	for(CItemBase *pKey = Item()->GetChildFirst(); pKey; pKey = pKey->GetNext() )
	{
		tDWORD vk;
		sscanf(pKey->m_strItemId.c_str(), "0x%x", &vk);
		pKey->m_pUserData = (void *)vk;
	}
}

void CVirtualKbd::UpdateLayout()
{
	for(CItemBase *pKey = Item()->GetChildFirst(); pKey; pKey = pKey->GetNext() )
		pKey->SetText(GetKeyName((tDWORD)pKey->m_pUserData));
}

/*
//////////////////////////////////////////////////////////////////////////
// CResetItemVector

tDWORD CResetItemVector::OnGetRecordIndent(tDWORD nSrcPos)
{
	return Vect()->at(nSrcPos).m_nIdent;
}

bool CResetItemVector::OnListEvent(int nItem, int nColumn, int nEvent)
{
	if( nEvent == VLISTCTRL_EVENT_CHECKED )
		ProcessCheck(nItem, nColumn);
	
	return CSerVectorView<cResetItem>::OnListEvent(nItem, nColumn, nEvent);
}
*/
//////////////////////////////////////////////////////////////////////////
// CSettingsReseter

void copy_vector(tMemChunk * &pDst, tMemChunk * pSrc, const cSerDescriptorField * pFieldDescr, bool cClearSrc)
{
	if( !pDst )
	{
		pDst = new tMemChunk;
		memset(pDst, 0, sizeof(tMemChunk));
	}
	
	cVectorHnd vdst, vsrc;
	vdst.attach(pDst, (tTYPE_ID)pFieldDescr->m_id, pFieldDescr->m_flags);
	vsrc.attach(pSrc, (tTYPE_ID)pFieldDescr->m_id, pFieldDescr->m_flags);
	
	vdst.assign(vsrc, VECTORAPI_CLEARDST|(cClearSrc ? VECTORAPI_CLEARSRC : 0));
	
	if( cClearSrc )
		delete pSrc;
}

CSettingsReseter::CSettingsReseter(CProfile * pProfile) :
	m_bDefBind(0), m_bProtection(0)
{
	m_data.m_pProfile.assign(pProfile, true);
}

void CSettingsReseter::FindImportantSettings()
{
	m_aResetItems.clear();
	m_data.Clear();
	
	m_bProtection = m_data.m_pProfile->isProtection();

	tDWORD lev = m_bProtection ? SETTINGS_LEVEL_RESET : SETTINGS_LEVEL_DEFAULT;
	_InitCfg(lev, m_data);
	_Init(lev, m_data, 0);
}

void CSettingsReseter::FillSettings()
{
	m_aResetItems.clear();
	m_data.Clear();
	_InitCfg(SETTINGS_LEVEL_UNK, m_data);
	_Init(SETTINGS_LEVEL_UNK, m_data, 0);
}

void CSettingsReseter::Reset()
{
	cBLSettings _OldBlCfg;
	cBLSettings * pBlCfg = NULL;
	
	if( m_bProtection )
	{
		pBlCfg = (cBLSettings *)m_data.m_pProfile->settings(NULL, cBLSettings::eIID);
		if( pBlCfg )
			_OldBlCfg = *pBlCfg;
	}
	
	_Reset(m_bProtection ? SETTINGS_LEVEL_RESET : SETTINGS_LEVEL_DEFAULT, m_data);
	
	if( m_bProtection && pBlCfg )
		CopyNotResetableProtSettings(pBlCfg, &_OldBlCfg);
}

bool CSettingsReseter::IsProfileReset(LPCSTR strName, LPCSTR strVector)
{
	Profile * pSub = m_data.Find(strName);
	if( !pSub )
		return true;

	if( pSub->m_nViewPos == -1 )
		return true;
	
	if( !m_aResetItems[pSub->m_nViewPos].m_bEnabled )
		return false;
	
	if( !strVector )
		return true;

	for(int i = 0, n = pSub->m_lists.size(); i < n; i++)
	{
		Profile::List& _list = pSub->m_lists[i];
		if( _list.m_name == strVector )
			return !!m_aResetItems[_list.m_nViewPos].m_bEnabled;
	}

	return true;
}

bool CSettingsReseter::SetProfileReset(LPCSTR strName, bool bEnabled)
{
	Profile * pSub = m_data.Find(strName);
	if( !pSub || pSub->m_nViewPos == -1 )
		return false;

	m_aResetItems[pSub->m_nViewPos].m_bEnabled = bEnabled;
	return true;
}

bool CSettingsReseter::OnAddDataSection(LPCSTR strSect)
{
	Profile * pSub = &m_data;
	if( strSect && *strSect )
		pSub = pSub->Find(strSect);
	
	if( !pSub )
		return false;

	if( m_bDefBind )
	{
//		Item()->AddDataSource(pSub->m_def.m_settings, strSect);
//		Item()->AddDataSource(CProfile::GetUserSettings(pSub->m_def.m_settings), strSect);
	}
	else
	{
//		Item()->AddDataSource(pSub->m_pProfile->settings(), strSect);
//		Item()->AddDataSource(pSub->m_pProfile->settings_user(), strSect);
	}
	
	return true;
}

void CSettingsReseter::_InitCfg(tDWORD lev, Profile& _item)
{
	CProfile * pProfile = _item.m_pProfile;
	
	if( lev != SETTINGS_LEVEL_UNK )
	{
		pProfile->ResetSettingsLevelTo(lev, _item.m_def);

		if( &m_data == &_item )
			_item.m_bDirty = true;
		else
			_item.m_bDirty = !CProfile::IsCfgEqual(_item.m_def, pProfile->m_cfg);
	}
	else
	{
		_item.m_bDirty = false;
	}
	
	_item.m_sub.resize(pProfile->GetChildrenCount());
	for(int i = 0, n = _item.m_sub.size(); i < n; i++)
	{
		Profile& _sub = _item.m_sub[i];
		_sub.m_pProfile.assign(pProfile->GetChild(i), true);
		
		_InitCfg(lev, _sub);
		if( _sub.m_bDirty )
			_item.m_bDirty = true;
	}
}

bool CSettingsReseter::OnIsForceReset(LPCSTR strName, LPCSTR strVector)
{
	sswitch(strName)
	scase(AVP_PROFILE_BEHAVIORMONITOR) return true; sbreak;
	scase(AVP_PROFILE_BEHAVIORMONITOR2)return true; sbreak;
	scase(AVP_PROFILE_REGGUARD2)       return true; sbreak;
	scase(AVP_PROFILE_ANTIHACKER)      return true; sbreak;
	send;

	return false;

//	sswitch(strVector)
//	scase("pdm.AppsMonitoring_List") return true; sbreak;
//	scase("pdm.TrustedImageList")    return true; sbreak;
//	scase("fw.AppRules")             return true; sbreak;
//	scase("fw.PortRules")            return true; sbreak;
//	send;
}

bool CSettingsReseter::_Init(tDWORD lev, Profile& _item, tDWORD nIdent)
{
	tString strSect ( _item.m_pProfile->m_sName.c_str(cCP_ANSI) );
	strSect += ".reset_items";
	
	tKeys keys; g_pRoot->GetKeys(PROFILE_LOCALIZE, strSect.c_str(), keys);
	for(size_t i = 0; i < keys.size(); i++)
	{
		LPCSTR pId = keys[i].c_str();

		CBindProps _props;
		const tString& strText = "";//g_pRoot->GetFormatedText(NULL, g_pRoot->GetString(tString(), PROFILE_LOCALIZE, strSect.c_str(), pId), &_props);

		if( _props.m_strValue != "list" )
		{
			Profile * _sub = _item.Find(pId);
			if( !_sub )
				continue;

			if( !_sub->m_bDirty && !OnIsForceReset(pId, NULL) )
				continue;

			cResetItem& _ri = m_aResetItems.push_back();
			//g_pRoot->LocalizeStr(strText.c_str(), _ri.m_sName);
			_ri.m_bEnabled = cTRUE;
			_ri.m_nIdent = nIdent;

			_sub->m_nViewPos = m_aResetItems.size() - 1;

			_Init(lev, *_sub, nIdent + 1);
			continue;
		}
		
/*		cFldData _FldDataDef;
		cFldData _FldData;
		
		{
			bool bOk = false;
			
			m_bDefBind = 1;
			if( m_pBinding->FindField(g_pRoot, pId, _FldDataDef) )
				if( _FldDataDef.m_pField->m_flags & SDT_FLAG_VECTOR )
					bOk = true;

			m_pBinding->ClearSources();
			if( !bOk )
				continue;
		}
		
		{
			bool bOk = false;
			
			m_bDefBind = 0;
			if( m_pBinding->FindField(g_pRoot, pId, _FldData) )
				if( _FldData.m_pField->m_flags & SDT_FLAG_VECTOR )
					bOk = true;

			m_pBinding->ClearSources();
			if( !bOk )
				continue;
		}

		{
			cVectorHnd _vDef, _v;
			_vDef.attach((tMemChunk *)_FldDataDef.m_pData, (tTYPE_ID)_FldDataDef.m_pField->m_id, _FldDataDef.m_pField->m_flags);
			_v.attach((tMemChunk *)_FldData.m_pData, (tTYPE_ID)_FldData.m_pField->m_id, _FldData.m_pField->m_flags);
			
			if( _v.isequal(_vDef) && !OnIsForceReset(_item.m_pProfile->m_sName.c_str(cCP_ANSI), pId) )
				continue;
		}

		cResetItem& _ri = m_aResetItems.push_back();
		//g_pRoot->LocalizeStr(strText.c_str(), _ri.m_sName);
		_ri.m_bEnabled = cFALSE;
		_ri.m_nIdent = nIdent;

		Profile::List& _list = _item.m_lists.push_back();
		_list.m_name = pId;
		_list.m_nViewPos = m_aResetItems.size() - 1;
		_list.m_pField = _FldData.m_pField;
		_list.m_pData = (tMemChunk *)_FldData.m_pData;*/
	}

	return true;
}

void CSettingsReseter::_Reset(tDWORD lev, Profile& _item)
{
	CProfile * pProfile = _item.m_pProfile;

	if( _item.m_nViewPos != -1 )
	{
		cResetItem& _ri = m_aResetItems[_item.m_nViewPos];
		if( !_ri.m_bEnabled )
			return;
	}

	if( !_item.m_bDirty )
		return;
	
	int i, n = _item.m_lists.size(), s;

	for(i = 0; i < n; i++)
	{
		Profile::List& _list = _item.m_lists[i];
		cResetItem& _ri = m_aResetItems[_list.m_nViewPos];
		if( !_ri.m_bEnabled )
			copy_vector(_list.m_pDataCopy, _list.m_pData, _list.m_pField, false);
	}

	if (pProfile->m_eOrigin == cProfile::poSystem)
		pProfile->Assign(_item.m_def);

	for(i = 0, s = _item.m_sub.size(); i < s; i++)
		_Reset(lev, _item.m_sub[i]);

	bool bRestored = false;
	for(i = 0; i < n; i++)
	{
		Profile::List& _list = _item.m_lists[i];
		if( _list.m_pDataCopy )
		{
			copy_vector(_list.m_pData, _list.m_pDataCopy, _list.m_pField, true);
			bRestored = true;
		}
	}

	if( bRestored && (!m_bProtection || (&_item != &m_data)) )
		_CheckCustomLevel(lev, _item);
}

bool CSettingsReseter::_CheckCustomLevel(tDWORD lev, Profile& _item)
{
	CProfile * pProfile = _item.m_pProfile;

	int i, s;

	bool bCustom = false;
	if( lev == SETTINGS_LEVEL_DEFAULT )
		bCustom = true;
	else
	{
		cCfg _cfgDef;
		pProfile->ResetSettingsLevelTo(SETTINGS_LEVEL_DEFAULT, _cfgDef);
		bCustom = !CProfile::IsCfgEqual(_cfgDef, pProfile->m_cfg);
	}

	if( !bCustom )
	{
		for(i = 0, s = _item.m_sub.size(); i < s; i++)
		{
			if( _CheckCustomLevel(lev, _item.m_sub[i]) )
			{
				bCustom = true;
				break;
			}
		}
	}

	if( bCustom )
		pProfile->m_cfg.m_dwLevel = SETTINGS_LEVEL_CUSTOM;

	return bCustom;
}

void CSettingsReseter::CopyNotResetableProtSettings(cBLSettings * pDstCfg, const cBLSettings * pSrcCfg)
{
	pDstCfg->m_bEnablePswrdProtect         = pSrcCfg->m_bEnablePswrdProtect;
	pDstCfg->m_bAskPswrdAlwaysExeptsAlerts = pSrcCfg->m_bAskPswrdAlwaysExeptsAlerts;
	pDstCfg->m_bAskPswrdOnSaveSetting      = pSrcCfg->m_bAskPswrdOnSaveSetting;
	pDstCfg->m_bAskPswrdOnExit             = pSrcCfg->m_bAskPswrdOnExit;
	pDstCfg->m_bAskPswrdOnStop             = pSrcCfg->m_bAskPswrdOnStop;
	pDstCfg->m_PrtctPassword               = pSrcCfg->m_PrtctPassword;
	pDstCfg->m_LicActivationLink           = pSrcCfg->m_LicActivationLink;
	pDstCfg->m_LicBuyLink                  = pSrcCfg->m_LicBuyLink;
	pDstCfg->m_LicRenewLink                = pSrcCfg->m_LicRenewLink;
	pDstCfg->m_bUseUninstallPwd            = pSrcCfg->m_bUseUninstallPwd;
	pDstCfg->m_UninstallPwd                = pSrcCfg->m_UninstallPwd;
	pDstCfg->m_strCustomerId               = pSrcCfg->m_strCustomerId;
	pDstCfg->m_strCustomerPwd              = pSrcCfg->m_strCustomerPwd;

	// bf 31974
	pDstCfg->m_Ins_InitMode                = pSrcCfg->m_Ins_InitMode;
	pDstCfg->m_Ins_DisplayName             = pSrcCfg->m_Ins_DisplayName;
	pDstCfg->m_Ins_ProductPath             = pSrcCfg->m_Ins_ProductPath;

	// Самозащита не дает работать автотестам, поэтому при наличии ключа в реестре 
	// не включаем ее при сбросе настроек
	if( !GUI_COM_RO(g_pRoot) )
		pDstCfg->m_bEnableSelfProtection   = pSrcCfg->m_bEnableSelfProtection;
}
/*
//////////////////////////////////////////////////////////////////////
// CProfileSettingsDlg

bool CCustomLevelDlg::OnOk()
{
	if( !CProfileSettingsDlg::OnOk() )
		return false;

	if( !m_bModified || !m_pProfile )
		return true;
	
	for(DWORD i = 0; i < m_data.GetChildrenCount(); i++)
	{
		CProfile * pProfile = m_data.GetChild(i)->m_pOriginal;
		
		pProfile->settings();
		cCfg _CfgTest(pProfile->m_cfg);
		
		if( !pProfile->GetSettingsByLevelTo(pProfile->m_cfg.m_dwLevel, &_CfgTest) )
			return false;

		if( _CfgTest.isEqual(&pProfile->m_cfg) )
			continue;
		
		m_pProfile->ResetSettingsLevel(SETTINGS_LEVEL_CUSTOM);
		break;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
// CExceptsItemCfgDlg

bool CExceptsItemCfgDlg::OnClicked(CItemBase* pItem)
{
	if( pItem->m_strItemId == "Browse" && m_ser && m_ser->isBasedOn(cDetectExclude::eIID) )
	{
		cStringObj& strMask = ((cDetectExclude*)m_ser)->m_Object.m_strMask;

		CBrowseObjectInfo pObject("BrowseForExcludeMask", SHELL_OBJTYPE_MASK, *this);
		pObject.m_strName = strMask;
		if( Root()->BrowseObject(pObject) )
		{
			strMask = pObject.m_strName;
			UpdateData(true);
			pItem->SetChangedData();
		}
	}

	return CSettingsDlg<>::OnClicked(pItem);
}

//////////////////////////////////////////////////////////////////////
// CExceptsItemDlg

CExceptsItemDlg::CExceptsItemDlg(cSerializable* pStruct, bool bNew, LPCSTR sTaskName) :
	CSettingsDlg<>(pStruct, bNew)
{
	if( sTaskName && *sTaskName )
		m_sTaskName = sTaskName;
	else
    {
        cStrObj productType = g_hGui->ExpandEnvironmentString("%ProductType%");
        m_sTaskName = (productType == "sos") ? AVP_TASK_ONDEMANDSCANNER : AVP_TASK_ONACCESSSCANNER;
    }
}

void CExceptsItemDlg::OnCustomizeData(cSerializable * pData, bool bIn)
{
	if( !bIn )
		return;

	if( !pData || !pData->isBasedOn(cDetectExclude::eIID) )
		return;

    cDetectExclude& _Item = *(cDetectExclude *)pData;

    if( !_Item.m_aTaskList.empty() )     // hack: try to differentiate fresh data structure from filled one
        return;

	if( m_nActionsMask & DLG_MODE_NEWDATA )
		_Item.m_Object.m_bRecurse = cFALSE;
	
	if( !(m_nActionsMask & DLG_MODE_NEWDATA) || m_sTaskName.empty() )
		return;

	_Item.m_nTriggers |= cDetectExclude::fTaskList;
	_Item.m_aTaskList.push_back(m_sTaskName);

	if( CProfile * pProfile = g_pProduct->GetProfileByType(m_sTaskName.c_str(cCP_ANSI)) )
	{
		CProfile * pParent = pProfile->m_pParent;
		if( pParent && pParent != g_pProduct )
		{
			for(DWORD i = 0; i < pParent->GetChildrenCount(); i++)
			{
				CProfile * pChild = pParent->GetChild(i);
				if( pChild != pProfile )
					_Item.m_aTaskList.push_back(pChild->m_sType);
			}

			_Item.m_aTaskList.push_back(pParent->m_sType);

			// bug 21512 fix: Т.к. в скинах привязка к списку задач осуществляется только
			// по одному из вариантов проактивки, то здесь мы добавляем "парную"
			if( pParent->m_sType == AVP_TASK_BEHAVIORBLOCKER )
				_Item.m_aTaskList.push_back(AVP_TASK_BEHAVIORBLOCKER2);
			else if( pParent->m_sType == AVP_TASK_BEHAVIORBLOCKER2 )
				_Item.m_aTaskList.push_back(AVP_TASK_BEHAVIORBLOCKER);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CExceptsList

bool CExceptsList::OnBindValue(CItemPropVals& pValue, tDWORD& nUnique)
{
	if( pValue.Count() )
		m_sTaskName = pValue.Pop(0).c_str();
	return false;
}

bool CExceptsList::IsFiltered()
{
	return !m_sTaskName.empty();
}

bool CExceptsList::OnAddRecord(CRecord &pRecord)
{
	cDetectExclude *p = GetRecordData(pRecord.m_nPos);
	return p ? p->IsForTask(m_sTaskName) : false;
}*/

//////////////////////////////////////////////////////////////////////
// CExceptsTasksList

struct { const char *type; tDWORD mask; } g_aTaskMap[] = 
{
	{ AVP_TASK_ONACCESSSCANNER, 0x01 },
	{ AVP_TASK_MAILCHECKER,     0x02 },
	{ AVP_TASK_HIPS,            0x04 },
	{ AVP_TASK_ONDEMANDSCANNER, 0x08 },
};

bool CExceptsTasksList::OnSetValue(const cVariant &pValue)
{
	cStringObjVector& arrTasks = *(cStringObjVector *)pValue.c_vector()->m_v;
	if( !arrTasks.size() )
		return true;

	m_nTasksMask = 0;
	for(tDWORD i = 0; i < arrTasks.size(); i++)
	{
		cStringObj &sTaskType = arrTasks[i];

		for(tDWORD j = 0; j < countof(g_aTaskMap); j++)
			if( sTaskType == g_aTaskMap[j].type )
				m_nTasksMask |= g_aTaskMap[j].mask;
	}
	
	Item()->UpdateView(0);
	return true;
}

bool CExceptsTasksList::OnGetValue(cVariant &pValue)
{
	cStringObjVector& arrTasks = *(cStringObjVector *)pValue.c_vector()->m_v;

	arrTasks.clear();
	for(tDWORD i = 0; i < countof(g_aTaskMap); i++)
		if( m_nTasksMask & g_aTaskMap[i].mask )
			arrTasks.push_back(g_aTaskMap[i].type);

	return true;
}

/*//////////////////////////////////////////////////////////////////////
// CAddToExceptsDlg

CAddToExceptsDlg::CAddToExceptsDlg(cDetectObjectInfo * pObject) :
	CExceptsItemDlg(&m_ExclItem, true, (LPCSTR)pObject->m_strTaskType.c_str(cCP_ANSI))
{
	m_pObject.assign(pObject, true);

	m_nActionsMask |= DLG_MODE_NEWDATA;

	m_ExclItem.m_nTriggers |= cDetectExclude::fObjectMask|cDetectExclude::fVerdictMask;
	m_ExclItem.m_Object.m_strMask = m_pObject->m_strObjectName;
	m_ExclItem.m_strVerdictMask = m_pObject->m_strDetectName;
	
	if( m_pObject->isBasedOn(cAskObjectAction_PDM_BASE::eIID) )
	{
		cAskObjectAction_PDM_BASE * pAsk = (cAskObjectAction_PDM_BASE *)m_pObject.ptr_ref();
		if( pAsk->m_EventType != REPORT_EVENT_REGMODIFY && !pAsk->m_strUserDescription.empty() )
		{
			m_ExclItem.m_nTriggers |= cDetectExclude::fVerdictPath;
			m_ExclItem.m_VerdictPath.m_strMask = pAsk->m_strUserDescription;
		}
	}
	if(m_pObject->isBasedOn(cPrivacyControlAskObjectAction::eIID))
	{
		cPrivacyControlAskObjectAction *pAsk = (cPrivacyControlAskObjectAction *)m_pObject.ptr_ref();
		if(!pAsk->m_strData2.empty())
		{
			m_ExclItem.m_nTriggers |= cDetectExclude::fVerdictPath;
			m_ExclItem.m_VerdictPath.m_strMask = pAsk->m_strData2;
		}
	}
}

tINT CAddToExceptsDlg::FindExcludeObject(cVector<cDetectExclude>& aExcludes, cDetectExclude& ExclItem)
{
	for( tINT i = 0; i < (tINT)aExcludes.size(); ++i )
	{
		if( aExcludes[i].m_Object.m_strMask.compare(ExclItem.m_Object.m_strMask, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ 
			&& aExcludes[i].m_strVerdictMask.compare(ExclItem.m_strVerdictMask, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ 
			&& aExcludes[i].m_VerdictPath.m_strMask.compare(ExclItem.m_VerdictPath.m_strMask, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ )
		{
			return i;
		}
	}
	return -1;
}

bool CAddToExceptsDlg::OnOk()
{
	if( !CExceptsItemDlg::OnOk() )
		return false;

	CProfile * pAvs = g_pProduct->GetProfile(AVP_SERVICE_AVS);
	if( !pAvs )
		return false;
	
	cAVSSettings * pAvsSett = (cAVSSettings *)pAvs->settings(NULL, cAVSSettings::eIID);
	if( !pAvsSett )
		return false;

	bool bAddExclItem = true;
	int i = FindExcludeObject(pAvsSett->m_aExcludes, m_ExclItem);
	if( i >= 0 )
	{
		if( pAvsSett->m_aExcludes[i].m_bEnable )
		{
			for( tDWORD j = 0; j < m_ExclItem.m_aTaskList.size(); ++j)
			{
				if( pAvsSett->m_aExcludes[i].m_aTaskList.find( m_ExclItem.m_aTaskList[j] ) == pAvsSett->m_aExcludes[i].m_aTaskList.npos )
					pAvsSett->m_aExcludes[i].m_aTaskList.push_back( m_ExclItem.m_aTaskList[j] );
			}
			bAddExclItem = false;
		}
		else
		{
			if( pAvsSett->m_aExcludes[i].m_aTaskList == m_ExclItem.m_aTaskList )
			{
				pAvsSett->m_aExcludes[i].m_bEnable = cTRUE;
				bAddExclItem = false;
			}
		}
	}

	if( bAddExclItem )
		pAvsSett->m_aExcludes.push_back(m_ExclItem);

	if( PR_FAIL(pAvs->SaveSettings()) )
		return false;

	if( m_pObject->isBasedOn(cAskObjectAction::eIID) )
	{
		cAskObjectAction * pAsk = (cAskObjectAction *)m_pObject.ptr_ref();
		pAsk->m_nResultAction = pAsk->m_nExcludeAction;
		((CDialogItem *)Item()->m_pParent)->Close();
	}

	cInfectedObjectInfo _Info; _Info.assign(*m_pObject, false);
	_Info.m_nObjectStatus = OBJSTATUS_ADDEDTOEXCLUDE;
	g_pProduct->SendMsg(pmc_UPDATE_THREATS_LIST, 0, &_Info);
	return true;
}

//////////////////////////////////////////////////////////////////////
// CFileMonAdvSettDlg

CFileMonAdvSettDlg::CFileMonAdvSettDlg(CProfile * pProfilesList, CProfile * pProfile) :
	CCustomLevelDlg(pProfilesList, pProfile)
{
	if( pProfile->settings() && pProfile->settings()->isBasedOn(cProtectionSettings::eIID) )
	{
		cProtectionSettings * pSett = (cProtectionSettings *)pProfile->settings();
		if( !pSett->m_pForUnchangedObjects )
			pSett->m_pForUnchangedObjects.init(cScanSettings::eIID);
	}
}

void CFileMonAdvSettDlg::OnCustomizeData(cSerializable * pData, bool bIn)
{
	if( !bIn )
	{
		if( !pData )
			return;
		
		if( pData->isBasedOn(cODSSettings::eIID) )
		{
			cODSSettings * pODSSett = (cODSSettings *)pData;
			pODSSett->m_bScanPlainMail = pODSSett->m_bScanMailBases;
		}
	}
}

/////////////////////////////////////////////////////////////////////
// CAntiSpamAdvSettDlg

bool CAsEmailsList::OnClicked(CItemBase* pItem)
{
	if( pItem->m_strItemId == "Import" )
	{
		cStrObj strFileName;
		if( Root()->BrowseFile(*this, NULL, "csv,txt", strFileName, false) )
		{
			bool bTruncated;
			if( ImportWhiteList(strFileName.data(), bTruncated) )
			{
				if (bTruncated)
					MsgBox(pItem, STRID_MAIL_ADDRESS_TRUNCTATED, NULL, MB_OK|MB_ICONWARNING);
				Item()->UpdateView(UPDATE_FLAG_SOURCE);				
			}
		}
	}
	return TBase::OnClicked(pItem);
}

bool CAsEmailsList::IsEqual(cSerializable *pRec1, cSerializable *pRec2)
{
	return ((cAsBwListRule *)pRec1)->m_sRule == ((cAsBwListRule *)pRec2)->m_sRule;
}

bool CAsEmailsList::ImportWhiteList(LPCWSTR strFileName, bool& bTruncated)
{
	cIOObj pFile(g_hGui->Obj(), cAutoString(cStringObj(strFileName)), fACCESS_READ);
	if( PR_FAIL(pFile.last_error()) )
		return false;
	
	tQWORD nFileSize;
	if( PR_FAIL(pFile->GetSize(&nFileSize, IO_SIZE_TYPE_EXPLICIT)) )
		return false;

	tDWORD size=(tDWORD)nFileSize;
	char *buffer;
	if( PR_FAIL(pFile->heapAlloc((void **)&buffer, size + 1)) )
		return false;


	bool bOk = false;
	try
	{
		if( PR_SUCC(pFile->SeekRead(&size, 0, buffer, size)) )
		{
			buffer[size] = 0;

			bOk = ParseImports(*(cAsBwListRules *)m_v, buffer, bTruncated);
		}
	} catch(...) {}
	
	pFile->heapFree(buffer);
	return bOk;
}


bool CAsEmailsList::ParseImports(cAsBwListRules& pList, char* pBuffer, bool& bTruncated)
{
	CMAdress ma(MAX_MAIL_ADDRESS_LEN); ma.Parse(pBuffer, bTruncated);

	for(size_t i = 0; i < ma.m_vAdress.size(); i++)
	{
		cAsBwListRule& rule = pList.push_back();
		rule.m_sRule = ma.m_vAdress[i].c_str();
	}
	return ma.m_vAdress.size() ? true : false;
}

void CAsEmailsList::CMAdress::Parse(char* pBuffer, bool& bTruncated)
{
	tString s(pBuffer);
	tString sep("\"\t,; \n\r");

	bTruncated = false;
 
	bool trunc;
	int pos = 0;
	while(true)
	{
		pos = s.find('@', pos);
		if( pos == tString::npos )
			break;

		int pos_e = s.find_first_of(sep, pos);
		int pos_s = s.find_last_of(sep, pos);

		if ( pos_e == -1 )
			pos_e = s.length();
		if( pos_s < pos_e )
			Add(s.substr(pos_s + 1, pos_e - pos_s - 1), trunc);

		if (trunc)
			bTruncated = true;
 
		pos = pos_e;
	}
} 

void CAsEmailsList::CMAdress::ParseEx(char* pBuffer, bool& bTruncated)
{
	tString s(pBuffer);

	tString sep("\"\t, \n\r");

	bTruncated = false;

	bool trunc;
	int pos_old = 0;
	int pos_new = (int)s.find_first_of(sep, pos_old);
	if( pos_new == -1 )
	{
		Add(s, trunc);
		if (trunc)
			bTruncated = true;
	}

	while( pos_new >= pos_old ){
		tString one_val = s.substr(pos_old, (pos_new - pos_old));
		if( false == one_val.empty() )
		{
			Add(one_val, trunc);
			if (trunc)
				bTruncated = true;
		}
		pos_old = pos_new + 1;
		pos_new = (int)s.find_first_of(sep, pos_old);
		if( pos_new < 0 ) 
			pos_new = (int)s.length();
	}
} 

void CAsEmailsList::CMAdress::Add(tString s, bool& bTruncated)
{
	bTruncated = false;

	if( -1 == Find(s) )
	{
		if (s.length() > m_maxAddressLen)
		{
			m_vAdress.push_back(s.substr(0, m_maxAddressLen));
			bTruncated = true;
		}
		else
			m_vAdress.push_back(s);
	}
}

int CAsEmailsList::CMAdress::Find(tString s)
{
	if( s.empty() )
		return -1;

	ToLower(s);

	for(size_t i = 0; i < m_vAdress.size(); i++)
	{
		tString s1 = m_vAdress[i];
		ToLower(s1);
		int pos = ( s1.length() > s.length() ) ? s1.find(s) : s.find(s1);
		if( pos != -1 ) return i;
	}
	
	return -1;
}	
	
void CAsEmailsList::CMAdress::ToLower(tString& s)
{
	for(size_t i = 0; i < s.length(); i++)
		s[i] = tolower(s[i]);
}

void CAntiSpamAdvSettDlg::OnChangedData(CItemBase * pItem)
{
	TBase::OnChangedData(pItem);

	if( pItem->m_strItemId == ITEMID_CSPAMRATE || pItem->m_strItemId == ITEMID_CSPAMSLIDER )
	{
		if( CItemBase * pPSpam = GetItem(ITEMID_PSPAMRATE) )
		{
			tDWORD nVal = (tDWORD)pItem->GetInt();
			if( nVal < pPSpam->GetInt() )
			{
				pPSpam->SetInt(nVal);
				m_pBinding->UpdateField(pPSpam);
			}
		}
	}
	
	if( pItem->m_strItemId == ITEMID_PSPAMRATE || pItem->m_strItemId == ITEMID_PSPAMSLIDER )
	{
		if( CItemBase * pCSpam = GetItem(ITEMID_CSPAMRATE) )
		{
			tDWORD nVal = (tDWORD)pItem->GetInt();
			if( nVal > pCSpam->GetInt() )
			{
				pCSpam->SetInt(nVal);
				m_pBinding->UpdateField(pCSpam);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
// CPdmDllLst

tERROR CPdmDllLst::OnEditItem(CItemBase * pOpCtl)
{
	cPdmAppMonitoring_Data& _Item = EditItem();
	if( m_bEditNew )
	{
		cVector<cPdmAppMonitoring_Data> aHashes;

		if( Root()->m_nGuiFlags & GUIFLAG_ADMIN )
		{
			tObjPath strFile;
			if( !Root()->BrowseFile(pOpCtl->m_pOwner, NULL, "exe", strFile) )
				return errNOT_OK;
			
			cPdmAppMonitoring_Data& pHash = aHashes.push_back();
			pHash.m_ImagePath = strFile;
		}
		else
		{
			CBrowseObjectInfo Obj("BrowseForSharedDll", SHELL_OBJTYPE_FOLDER|SHELL_OBJTYPE_FILE);
			Obj.m_pParent = pOpCtl->m_pOwner;
			
			if( !Root()->BrowseObject(Obj) )
				return errNOT_OK;

			if( PR_FAIL(CCalcHashesDlg(Obj, aHashes).DoModal(Obj.m_pParent, "PdmCalcHash")) )
				return errNOT_OK;
		}

		for(tDWORD i = 0; i < aHashes.size(); i++)
		{
			cPdmAppMonitoring_Data& _add = aHashes[i];

			int k, n;
			for(k = 0, n = Vect()->size(); k < n; k++)
			{
				cPdmAppMonitoring_Data& _exist = Vect()->at(k);
				if( _exist.m_ImagePath == _add.m_ImagePath )
					break;
			}

			if( k == n )
				Vect()->push_back(_add);
			else
			{
				if (DLG_ACTION_YES == MsgBox(pOpCtl, STRID_APP_DLL_ALREADY_EXIST, NULL, MB_YESNO|MB_ICONEXCLAMATION))
					Vect()->at(k) = _add;
			}
		}
		
		Item()->UpdateView(UPDATE_FLAG_SOURCE);
		
		return errNOT_OK;
	}	
	
	return CSerVectorView<cPdmAppMonitoring_Data>::OnEditItem(pOpCtl);
}

tERROR CPdmDllLst::CCalcHashesDlg::OnDoWork()
{
	m_aHashes.clear();
	
	cVector<cStringObj> aModules;

	if( m_ObjInfo.m_nType & SHELL_OBJTYPE_FILE )
	{
		aModules.push_back(m_ObjInfo.m_strName);
	}
	else if( m_ObjInfo.m_nType & SHELL_OBJTYPE_FOLDER )
	{
		cAutoObj<cObjPtr> pIoPtr;
		tERROR err = g_root->sysCreateObject((hOBJECT *)&pIoPtr, IID_OBJPTR, PID_NATIVE_FIO);
		if( PR_SUCC(err) ) err = m_ObjInfo.m_strName.copy(pIoPtr, pgOBJECT_PATH);
		if( PR_SUCC(err) ) err = pIoPtr->propSetStr(NULL, pgMASK, (tPTR)"*.dll");
		if( PR_SUCC(err) ) err = pIoPtr->sysCreateObjectDone();
		if( PR_SUCC(err) ) err = pIoPtr->Reset(false);
		if( PR_SUCC(err) )
		{
			while( PR_SUCC(err = pIoPtr->Next()) )
			{
				if( m_bAbort )
					break;

				if( !pIoPtr->get_pgIS_FOLDER() )
				{
					cStringObj &strModule = aModules.push_back();
					strModule.assign(pIoPtr, pgOBJECT_FULL_NAME);
				}
			}
		}
	}
	
	m_Progress.m_nProgressPercent = 5;
	UpdateWaitStatus();

	if( aModules.size() && !m_bAbort )
	{
		cProcessMonitor * pPM = NULL;
		tERROR err = g_hTM->GetService(0, (hOBJECT)g_hTM, AVP_SERVICE_PROCESS_MONITOR, (cObject **)&pPM);
		if( PR_FAIL(err) )
		{
			PR_TRACE(( this, prtERROR, "gui\tpdm_calc_hashes(): can't get process monitor service. %terr", err ));
			return err;
		}
		
		for(tDWORD i = 0; i < aModules.size(); i++)
		{
			if( m_bAbort )
				break;

			cModuleInfoHash mih; mih.m_sImagePath = aModules[i];
			if( PR_SUCC(pPM->GetProcessInfo(&mih)) )
			{
				cPdmAppMonitoring_Data &AppInfo = m_aHashes.push_back();
				AppInfo.m_bInUse      = cTRUE;
				AppInfo.m_ImagePath   = mih.m_sImagePath;
				AppInfo.m_Version     = mih.m_sVersion;
				AppInfo.m_Version     = mih.m_sVersion;
				AppInfo.m_Vendor      = mih.m_sVendor;
				AppInfo.m_Description = mih.m_sDescription;
				AppInfo.m_tCreation   = mih.m_tCreation;
				AppInfo.m_tModify     = mih.m_tModify;
				AppInfo.m_ObjectSize  = mih.m_nObjectSize;
				AppInfo.m_Hash        = mih.m_nHash;
			}

			m_Progress.m_nProgressPercent = 95*(i + 1)/aModules.size() + 5;
			UpdateWaitStatus();
		}

		g_hTM->ReleaseService(0, (cObject *)pPM);
	}

	return m_bAbort ? warnFALSE : errOK;
}

//////////////////////////////////////////////////////////////////////
// CPdmRegGroupLst

tERROR CPdmRegGroupLst::OnEditItem(CItemBase * pOpCtl)
{
	if( m_bEditNew )
	{
		tString str; str.resize(256);
		_snprintf(&str[0], str.size(), "%s %d", 
			Root()->GetString(tString(), PROFILE_LOCALIZE, "REGSettings_Group", "NewRegGroup"), GetRecordsCount() + 1);

		EditItem().m_strName = str.c_str();
	}
	
	return CSerVectorView<cPdmRegGroup_Item>::OnEditItem(pOpCtl);
}

//////////////////////////////////////////////////////////////////////
// CPdmRegGroupAppLst

tERROR CPdmRegGroupAppLst::OnEditItem(CItemBase* pOpCtl)
{
	cPdmRegGroupApp_Item& _Item = EditItem();

	if( m_bEditNew )
	{
		_Item.m_Data.m_ImagePath = "*";
		return errOK;
	}

	return CSerVectorView<cPdmRegGroupApp_Item>::OnEditItem(pOpCtl);
}

tERROR CPdmRegGroupKeyLst::OnEditItem(CItemBase * pOpCtl)
{
	cPdmRegGroupKey_Item& _Item = EditItem();

	cRegKeyInfo RkeyInfo;
	RkeyInfo.m_KeyPath		= _Item.m_strRegPath;
	RkeyInfo.m_KeyValue		= _Item.m_strValueName;
	RkeyInfo.m_bRecursive	= _Item.m_bRecursive;

	CBrowseObjectInfo pObject("BrowseForRegistry", SHELL_OBJTYPE_REGKEY|SHELL_OBJTYPE_REGVALUE|SHELL_OBJTYPE_MASK, *this, &RkeyInfo, this);
	
	if( !Root()->BrowseObject(pObject) )
		return warnFALSE;
	
	_Item.m_strRegPath	    = RkeyInfo.m_KeyPath;
	_Item.m_strValueName    = RkeyInfo.m_KeyValue;
	_Item.m_bRecursive	    = RkeyInfo.m_bRecursive;
	return errOK;
}

bool CPdmRegGroupKeyLst::OnCustomizeObject(bool bDir, CBrowseObjectInfo& pObject)
{ 
	if( !pObject.m_pData )
		return false;
	
	cRegKeyInfo * pRkeyInfo = (cRegKeyInfo *)pObject.m_pData;
	if( bDir )
	{
		if( pObject.m_nType == SHELL_OBJTYPE_REGVALUE )
		{
			int nPos = pObject.m_strName.find_last_of(pObject.m_strShortName.data());
			if( nPos != tObjPath::npos )
				pRkeyInfo->m_KeyPath = pObject.m_strName.substr(0, nPos - pObject.m_strShortName.length());
			pRkeyInfo->m_KeyValue	= pObject.m_strShortName;
		}
		else
			pRkeyInfo->m_KeyPath = pObject.m_strName;
	}
	else
	{
		pObject.m_strName = pRkeyInfo->m_KeyPath;
		pObject.m_strShortName = pRkeyInfo->m_KeyValue;
	}
	
	return true; 
}

bool CPdmRegGroupKeyLst::OnCanAddObject(CBrowseObjectInfo& pObject)
{
	if( !pObject.m_pData )
		return false;
	
	cRegKeyInfo * pRkeyInfo = (cRegKeyInfo *)pObject.m_pData;
	if( !pRkeyInfo->m_bRecursive )
		return true;
	
	cStrObj strLastKey;
	tDWORD nSlash = pRkeyInfo->m_KeyPath.find_last_of(L"\\/");
	if( cStrObj::npos != nSlash )
		strLastKey.assign(pRkeyInfo->m_KeyPath, nSlash + 1);
	else
		strLastKey = pRkeyInfo->m_KeyPath;
	
	if( cStrObj::npos == strLastKey.find_first_of(L"*?") )
		return true;
	return MsgBox(pObject.m_pDlg, "RegKeyMaskWithSubfoldersWarning", NULL, MB_YESNO|MB_ICONEXCLAMATION) == DLG_ACTION_YES;
}

//////////////////////////////////////////////////////////////////////
// CRegGuardEditDlg

bool CRegGuardEditDlg::OnOk()
{
	if( !m_ser->m_vRegKeys.size() )
	{
		MsgBox(*this, STRID_ERR_RG_NO_ANY_KEYS, NULL, MB_OK|MB_ICONWARNING);
		return false;
	}
	
	if( !m_ser->m_vRules.size() )
	{
		MsgBox(*this, STRID_ERR_RG_NO_ANY_APPS, NULL, MB_OK|MB_ICONWARNING);
		return false;
	}

	// проверка на приложения с не заданным именем
	for(tDWORD i = 0; i < m_ser->m_vRules.size(); i++ )
	{
		cPdmRegGroupApp_Item& _Elm = m_ser->m_vRules[i];
		if( _Elm.m_Data.m_ImagePath.empty() )
		{
			MsgBox(*this, STRID_ERR_RG_UNKNOWN_APP, NULL, MB_OK|MB_ICONWARNING);
			return false;
		}
	}
	
	return Base::OnOk();
}

//////////////////////////////////////////////////////////////////////
// CPdmRegGroupLst

tERROR CRegGuard2GroupLst::OnEditItem(CItemBase * pOpCtl)
{
	if( m_bEditNew )
	{
		tString str; str.resize(256);
		_snprintf(&str[0], str.size(), "%s %d", 
			Root()->GetString(tString(), PROFILE_LOCALIZE, "REGSettings_Group", "NewRegGroup"), GetRecordsCount() + 1);

		EditItem().m_GroupName = str.c_str();
	}

	return CSerVectorView<CRegGroupEntry>::OnEditItem(pOpCtl);
}

//////////////////////////////////////////////////////////////////////
// CRegGuard2GroupAppLst

tERROR CRegGuard2GroupAppLst::OnEditItem(CItemBase* pOpCtl)
{
	CRegAppEntry& _Item = EditItem();

	if( m_bEditNew )
	{
		_Item.m_ApplicationExeWildcard = "*";
		return errOK;
	}

	return CSerVectorView<CRegAppEntry>::OnEditItem(pOpCtl);
}

tERROR CRegGuard2GroupKeyLst::OnEditItem(CItemBase * pOpCtl)
{
	CRegKeyEntry& _Item = EditItem();

	cRegKeyInfo RkeyInfo;
	RkeyInfo.m_KeyPath		= _Item.m_KeyWildcard;
	RkeyInfo.m_KeyValue		= _Item.m_ValueWildcard;
	RkeyInfo.m_bRecursive	= _Item.m_bRecursive;

	CBrowseObjectInfo pObject("BrowseForRegistry", SHELL_OBJTYPE_REGKEY|SHELL_OBJTYPE_REGVALUE|SHELL_OBJTYPE_MASK, *this, &RkeyInfo, this);

	if( !Root()->BrowseObject(pObject) )
		return warnFALSE;

	_Item.m_KeyWildcard	    = RkeyInfo.m_KeyPath;
	_Item.m_ValueWildcard   = RkeyInfo.m_KeyValue;
	_Item.m_bRecursive      = RkeyInfo.m_bRecursive;
	return errOK;
}

bool CRegGuard2GroupKeyLst::OnCustomizeObject(bool bDir, CBrowseObjectInfo& pObject)
{ 
	if( !pObject.m_pData )
		return false;

	cRegKeyInfo * pRkeyInfo = (cRegKeyInfo *)pObject.m_pData;
	if( bDir )
	{
		if( pObject.m_nType == SHELL_OBJTYPE_REGVALUE )
		{
			int nPos = pObject.m_strName.find_last_of(pObject.m_strShortName.data());
			if( nPos != tObjPath::npos )
				pRkeyInfo->m_KeyPath = pObject.m_strName.substr(0, nPos - pObject.m_strShortName.length());
			pRkeyInfo->m_KeyValue	= pObject.m_strShortName;
		}
		else
		{
			pRkeyInfo->m_KeyPath = pObject.m_strName;
			pRkeyInfo->m_KeyValue = L"*";
		}
	}
	else
	{
		pObject.m_strName = pRkeyInfo->m_KeyPath;
		pObject.m_strShortName = pRkeyInfo->m_KeyValue;
	}

	return true; 
}

bool CRegGuard2GroupKeyLst::OnCanAddObject(CBrowseObjectInfo& pObject)
{
	if( !pObject.m_pData )
		return false;

	cRegKeyInfo * pRkeyInfo = (cRegKeyInfo *)pObject.m_pData;
	if( !pRkeyInfo->m_bRecursive )
		return true;

	cStrObj strLastKey;
	tDWORD nSlash = pRkeyInfo->m_KeyPath.find_last_of(L"\\/");
	if( cStrObj::npos != nSlash )
		strLastKey.assign(pRkeyInfo->m_KeyPath, nSlash + 1);
	else
		strLastKey = pRkeyInfo->m_KeyPath;

	if( cStrObj::npos == strLastKey.find_first_of(L"*?") )
		return true;
	return MsgBox(pObject.m_pDlg, "RegKeyMaskWithSubfoldersWarning", NULL, MB_YESNO|MB_ICONEXCLAMATION) == DLG_ACTION_YES;
}

//////////////////////////////////////////////////////////////////////
// CRegGuardEditDlg

bool CRegGuard2EditDlg::OnOk()
{
	if( !m_ser->m_vKeyList.size() )
	{
		MsgBox(*this, STRID_ERR_RG_NO_ANY_KEYS, NULL, MB_OK|MB_ICONWARNING);
		return false;
	}

	if( !m_ser->m_vAppList.size() )
	{
		MsgBox(*this, STRID_ERR_RG_NO_ANY_APPS, NULL, MB_OK|MB_ICONWARNING);
		return false;
	}

	// проверка на приложения с не заданным именем
	for(tDWORD i = 0; i < m_ser->m_vAppList.size(); i++ )
	{
		CRegAppEntry& _Elm = m_ser->m_vAppList[i];
		if( _Elm.m_ApplicationExeWildcard.empty() )
		{
			MsgBox(*this, STRID_ERR_RG_UNKNOWN_APP, NULL, MB_OK|MB_ICONWARNING);
			return false;
		}
	}

	return Base::OnOk();
}*/

//////////////////////////////////////////////////////////////////////
// CUpdateSrcList

tERROR CUpdateSrcList::OnEditItem(CItemBase * pOpCtl)
{
	cUpdaterSource &Item = *(cUpdaterSource *)GetEditRowData();
	
	CBrowseObjectInfo pObject("BrowseForUpdateSource", SHELL_OBJTYPE_DRIVE|SHELL_OBJTYPE_FOLDER|SHELL_OBJTYPE_URL|SHELL_OBJTYPE_FILE|SHELL_OBJTYPE_FILTERED, *this, &Item);

	if( m_pItem && m_pItem->m_pParent )
	{
		//cSerializableObj pSer; pSer.ptr_ref() = m_pItem->m_pParent->GetCtx();
		//pObject.m_aDataEx.push_back() = ExtractCtx(pSer, cProfile::eIID);
	}
	
	pObject.m_strName = Item.m_strPath;
	pObject.m_pEnum = this;
	
	if( !Root()->BrowseObject(pObject) )
		return errNOT_OK;

	Item.m_bEnable = cTRUE;
	Item.m_strPath = pObject.m_strName;
	return errOK;
}

bool CUpdateSrcList::OnCanOperateItem(cRowId &nItem, eOpCode nOp)
{
	cUpdaterSource *pSrc = (cUpdaterSource *)GetItemData(nItem);
	return (pSrc && pSrc->m_dwType >= UPDSRC_DEF_NUMB || nOp != opDelete && nOp != opEdit) &&
		CVectorView::OnCanOperateItem(nItem, nOp);
}

bool CUpdateSrcList::FilterObject(CObjectInfo& pObject)
{
	if( pObject.m_nType != SHELL_OBJTYPE_FILE )
		return true;

	tDWORD pos = pObject.m_strName.find_last_of(".");
	if( pos == cStrObj::npos )
		return false;
	return !pObject.m_strName.compare(pos+1, cStrObj::whole, "zip", fSTRING_COMPARE_CASE_INSENSITIVE);
}

bool CUpdateSrcList::OnCanAddObject(CBrowseObjectInfo& pObject)
{
	cUpdaterSource newItem;
	newItem.m_strPath = pObject.m_strName;
	return Validate(*this, &newItem);
}

/*//////////////////////////////////////////////////////////////////////
// CIPv6AddrEditItem

bool CIPv6AddrEditItem::OnSetValue(tPTR pValue)
{
	cIPv6* pIP = (cIPv6 *)pValue;
	if( pIP->IsValid() )
	{
		tSIZE_T size = pIP->ToStr(NULL, 0);
		PR_ASSERT( size );
		if( !size )
			return false;
		std::vector<tCHAR> buf(size);
		size = pIP->ToStr(&buf[0], buf.size());
		PR_ASSERT( size );
		Item()->SetText(&buf[0]);
	}
	else
		Item()->SetText("");
	return true;
}

bool CIPv6AddrEditItem::OnGetValue(tPTR pValue)
{
	cIPv6* pIP = (cIPv6 *)pValue;
	return pIP->FromStr(Item()->GetText());
}

//////////////////////////////////////////////////////////////////////
// CIPv6MaskEditItem

bool CIPv6MaskEditItem::OnSetValue(tPTR pValue)
{
	cIPv6* pIP = (cIPv6 *)pValue;
	tCHAR buf[4];
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "%u", pIP->GetMaskPrefixLength());
	Item()->SetText(&buf[0]);
	return true;
}

bool CIPv6MaskEditItem::OnGetValue(tPTR pValue)
{
	cIPv6* pIP = (cIPv6 *)pValue;
	tDWORD dwMaskPrefixLength(0);
	if( sscanf_s(Item()->GetText(), "%u", &dwMaskPrefixLength) != 1 )
		return false;
	pIP->SetMask(dwMaskPrefixLength, 0);
	return true;
}

//////////////////////////////////////////////////////////////////////
// CAntihackerAdvSettDlg

tERROR CAntiHackerRulesImpExp::OnExportPrepare(tObjPath& strFileName)
{
	cIOObj hIo((cObject *)g_root, cAutoString(strFileName), fACCESS_WRITE, fOMODE_CREATE_ALWAYS|fOMODE_SHARE_DENY_WRITE);
	if( PR_FAIL(hIo.last_error()) )
		return hIo.last_error();
	
	m_hIo = hIo.relinquish();
	m_strBuff.reserve(0x1000);
	return errOK;
}
	
tERROR CAntiHackerRulesImpExp::OnExportItem(int nPos)
{
	CFwPresets::ExportRule(m_strBuff, (cSerializable *)m_pVect->GetSerPtr(nPos));

	tDWORD nWriteRes;
	tERROR err = m_hIo->SeekWrite(&nWriteRes, m_nPos, (void *)m_strBuff.c_str(), m_strBuff.size());
	if( PR_FAIL( err ) )
		return err;

	m_nPos += nWriteRes;
	m_strBuff.resize(0);
	return errOK;
}

tERROR CAntiHackerRulesImpExp::OnImport(tObjPath& strFileName, bool bAppend)
{
	cFwAppRulePresets aPresets;
	if( !CFwPresets::LoadPresets(strFileName.c_str(cCP_ANSI), aPresets) )
		return errNOT_OK;
		
	if( !bAppend )
	{
		if( m_pVect->_GetListType() == cFwAppRule::eIID )
			(*(cVector<cFwAppRule> *)m_pVect->m_v).clear();
		if( m_pVect->_GetListType() == cFwPacketRule::eIID )
			(*(cVector<cFwPacketRule> *)m_pVect->m_v).clear();
	}
	
	for(tDWORD i = 0; i < aPresets.size(); i++)
	{
		cFwAppRulePreset &Preset = aPresets[i];
		
		if( m_pVect->_GetListType() == cFwAppRule::eIID )
		{
			if( Preset.m_sName == FWPRESETNAME_PACKETRULE || !Preset.m_aApps.size() )
				continue;

			cFwAppRules &RulList = *(cFwAppRules *)m_pVect->m_v;
			for(tDWORD j = 0; j < Preset.m_aApps.size(); j++)
			{
				cStringObj &sApp = Preset.m_aApps[j];
				if( CheckAppPresent(sApp) )
				{
					cFwAppRule &Rule = *CFwRuleUtils::FindRule(RulList, sApp, NULL, Preset.m_sCmdLine, Preset.m_bUseCmdLine, cTRUE);
					CFwRuleUtils::ApplyPreset(Rule, &Preset, cFALSE);
				}
			}
		}
		else if( m_pVect->_GetListType() == cFwPacketRule::eIID )
		{
			if( !(Preset.IsPacketPreset() && Preset.m_aRules.size() == 1) )
				continue;
			
			cVector<cFwPacketRule> &RulList = *(cVector<cFwPacketRule> *)m_pVect->m_v;
			cFwPacketRule &Rule = RulList.push_back();
			*(cFwBaseRule *)&Rule = Preset.m_aRules[0];		
			if( Rule.m_nProto == _fwPROTO_ICMP )
			{
				Rule.m_nIcmpCode = Preset.m_nIcmpCode;
				Rule.InitProps();
			}
		}
	}
	
	return errOK;
}

bool CAntiHackerRulesImpExp::CheckAppPresent(cStringObj &strFileName)
{
	if( m_pList->m_pRoot->m_nGuiFlags & GUIFLAG_ADMIN )
		return true;
	
	if( strFileName.compare(L"system", fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ )
		return true;

	g_hGui->ExpandEnvironmentString(strFileName);
#if defined (_WIN32)	
	tDWORD nAttr = GetFileAttributesA(strFileName.c_str(cCP_ANSI));
	if( nAttr == INVALID_FILE_ATTRIBUTES || nAttr & FILE_ATTRIBUTE_DIRECTORY )
#elif defined (__unix__)
	struct stat l_stat;		
	if ( stat ( strFileName.c_str(cCP_ANSI), &l_stat ) ||  S_ISDIR(l_stat.st_mode) )
#endif
	{
		cMsgParams param;
		param.m_strVal1 = strFileName;
		if( MsgBox(m_pList, "fwImpBadApp", "fwImpBadAppCaption", MB_YESNO|MB_ICONQUESTION, &param) != DLG_ACTION_YES )
			return false;

		tString sTmp ( strFileName.c_str(cCP_ANSI) );
		LPCSTR sFile = strrchr(sTmp.c_str(), '\\');
		if ( !sFile )
			sFile = strrchr(sTmp.c_str(), '/');
		sFile = sFile ? sFile + 1 : sTmp.c_str();
		if( !m_pList->m_pRoot->BrowseFile(m_pList, NULL, sFile, strFileName) )
			return false;
	}
	return true;
}

void CAntihackerAdvSettDlg::OnInit()
{
	m_bNeedConvertRules = false;
	TBase::OnInit();
}

void CAntihackerAdvSettDlg::OnInited()
{
	if( m_AppRulesLstEx )
	{
		m_AppRulesLstEx.Bind(&m_AppRulesEx);

		cAHFWSettings *pSett = (cAHFWSettings *)Ser();
		if( pSett && pSett->isBasedOn(cAHFWSettings::eIID)  )
		{
			if( IsBlockedByPolicy(pSett, &pSett->m_AppRules) )
				m_AppRulesLstEx->Freeze(true);
		}
	}

}

bool CAntihackerAdvSettDlg::OnOk()
{
	if( !Item()->IsChecked("GroupCheck") && Ser() && Ser()->isBasedOn(cAHFWSettings::eIID) )
		CFwRuleUtils::AppEx2App(m_AppRulesEx, ((cAHFWSettings *)Ser())->m_AppRules);
	return TBase::OnOk();
}

void CAntihackerAdvSettDlg::OnChangedData(CItemBase* pItem)
{
	TBase::OnChangedData(pItem);
	if( pItem->m_strItemId == "GroupCheck" )
		Switch2GroupView(!!pItem->GetInt());
}

void CAntihackerAdvSettDlg::OnCustomizeData(cSerializable * pData, bool bIn)
{
	cAHFWSettings *pSett = (cAHFWSettings *)Ser(NULL, cAHFWSettings::eIID);
	if( !(pSett && m_NetworkLst && m_NetworkLst.m_aDefItems.size() == 1) )
		return;

	if( bIn )
		m_NetworkLst.m_aDefItems[0].m_bStealthed = pSett->m_InetStealthed;
	else
		pSett->m_InetStealthed = m_NetworkLst.m_aDefItems[0].m_bStealthed;
}

void CAntihackerAdvSettDlg::Switch2GroupView(bool bGroupView)
{
	cAHFWSettings * pSett = (cAHFWSettings *)Ser(NULL, cAHFWSettings::eIID);
	if( !pSett )
		return;

	if( bGroupView )
	{
		Show("Branch", false);
		if( m_bNeedConvertRules )
		{
			CFwRuleUtils::AppEx2App(m_AppRulesEx, pSett->m_AppRules);
			if( m_AppRulesLst )   m_AppRulesLst.UpdateView(UPDATE_FLAG_REINIT);
			if( m_AppRulesLstEx ) m_AppRulesLstEx.UpdateView(UPDATE_FLAG_REINIT);
		}
		Show("Group", true);
	}
	else
	{
		Show("Group", false);
		CFwRuleUtils::App2AppEx(pSett->m_AppRules, m_AppRulesEx);
		if( m_AppRulesLst )   m_AppRulesLst.UpdateView(UPDATE_FLAG_REINIT);
		if( m_AppRulesLstEx ) m_AppRulesLstEx.UpdateView(UPDATE_FLAG_REINIT);
		Show("Branch", true);
	}
	
	m_bNeedConvertRules = true;
}

//////////////////////////////////////////////////////////////////////
// CFwZoneEditDlg

CFwZoneEditDlg::CFwZoneEditDlg(cSerializable* pStruct, tDWORD nFlags) :
	CDialogBindingT<>(pStruct, nFlags), m_pNetwork(NULL)
{
}

void CFwZoneEditDlg::OnInited()
{
	if( m_ser && m_ser->isBasedOn(cAHFWNetwork::eIID) )
	{
		m_pNetwork = (cAHFWNetwork*)m_ser;
		m_pNetwork->m_Mask.m_Version = m_pNetwork->m_IP.m_Version;
	}
}

bool CFwZoneEditDlg::OnClicked(CItemBase* pItem)
{
	if( m_pNetwork )
	{
		sswitch(pItem->m_strItemId.c_str())
		stcase(IPv4) m_pNetwork->m_IP.m_Version = m_pNetwork->m_Mask.m_Version = 4; UpdateData(true); sbreak;
		stcase(IPv6) m_pNetwork->m_IP.m_Version = m_pNetwork->m_Mask.m_Version = 6; UpdateData(true); sbreak;
		send;
	}
	return CDialogBindingT<>::OnClicked(pItem);
}

//////////////////////////////////////////////////////////////////////
// CNetHostEditDlg

bool CNetHostsList::GetRecord(CRecord &pRecord)
{
    cIP *p = GetRecordData(pRecord.m_nPos);
    if(!p || !p->IsValid())
        return false;
    pRecord.SetTextData(0, 0, 1, CFwRuleUtils::Ip2Str(*p).c_str());
    return true;
}

CNetHostsEditDlg::CNetHostsEditDlg(cSerializable* pStruct, tDWORD nFlags) :
    TBase(pStruct, nFlags), m_pApp(NULL)
{
    m_Host.m_Version = 4;
}

void CNetHostsEditDlg::OnCreate()
{
    TBase::OnCreate();
    m_pBinding->AddDataSource(&m_Host, "Host");
}

void CNetHostsEditDlg::OnInited()
{
    if( m_ser && m_ser->isBasedOn(cBLTrustedApp::eIID) )
        m_pApp = (cBLTrustedApp*)m_ser;

    if( m_pApp && m_List )
    {
        m_List.InitListController(this);
        m_List->SetItemFocus(0);
    }
}

bool CNetHostsEditDlg::OnEditItem(CItemBase * pList, CItemBase * pOpCtl, cIP& Item, bool bNew)
{
    Item = m_Host;
    return true;
}

void CNetHostsEditDlg::OnChangedState(CItemBase * pItem, tDWORD nStateMask)
{
    if( pItem == m_List )
    {
        int nItem = m_List.GetSingleSelectedItem();
        if( nItem != -1 && m_pApp )
        {
            int nPos = m_List->GetSourcePos(nItem);
            if( nPos != -1 )
            {
                m_Host = m_pApp->m_Hosts.at(nPos);
                CStructData data (&m_Host, "Host");
                m_pBinding->UpdateData(true, data);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////
// CNetPortsEditDlg

CNetPortsEditDlg::CNetPortsEditDlg(cSerializable* pStruct, tDWORD nFlags) : 
    TBase(pStruct, nFlags)
{
    if( pStruct && pStruct->isBasedOn(cBLTrustedApp::eIID) )
        m_pApp = (cBLTrustedApp*)pStruct;
}

void CNetPortsEditDlg::OnInited()
{
    if(m_pApp && m_Ports)
        m_Ports.Item()->SetText(GetPortListStr(m_pApp->m_Ports).c_str());
}

bool CNetPortsEditDlg::OnOk()
{
    if (ParsePortListStr(m_Ports.Item()->GetText(), m_pApp->m_Ports))
        return true;
    else
    {
        MsgBox(*this, STRID_ERRINVALIDPORT, NULL, MB_OK|MB_ICONWARNING);
        return false;
    }
}

tString CNetPortsEditDlg::GetPortListStr(const cVector<tWORD> &Ports)
{
    tString str;
    char strPort[40];
    for(size_t i = 0; i < Ports.size(); i++)
    {
        str += i ? ", " : "";
        sprintf(strPort, "%d", Ports[i]);
        str += strPort;
    }
    return str;
}

bool CNetPortsEditDlg::ParsePortListStr(LPCTSTR sPorts, cVector<tWORD> &aPorts)
{
    aPorts.clear();

    for(; *sPorts; sPorts++)
    {
        if(isdigit(*sPorts))
        {
            tWORD nPort = atoi(sPorts);
            if (nPort > 0 && nPort <= 65535)
            {
                aPorts.push_back(nPort);
                for(; isdigit(*sPorts); sPorts++); sPorts--;
                continue;
            }
            else
                return false;
        }
        else if(*sPorts == ' ' || *sPorts == ',' || *sPorts == ';')
        {
            continue;
        }
        else
            return false;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////
// CFwRuleDlg

CFwRuleDlg::CFwRuleDlg(cSerializable* pStruct, tDWORD nFlags) :
	CDialogBindingT<>(pStruct),
	m_pRule(NULL),
	m_nFlags(nFlags)
{
	m_strSection = "FwRuleElementEdit";
}

bool CFwRuleDlg::DoModal(CItemBase* pParent, bool bNew)
{
	return CDialogBindingT<>::DoModal(pParent, m_strSection.c_str(), DLG_ACTION_OKCANCEL, bNew) == DLG_ACTION_OK;
}

void CFwRuleDlg::OnInited()
{
	if(m_ser && m_ser->isBasedOn(cFwBaseRule::eIID))
	{
		m_pRule = (cFwBaseRule *)m_ser;
		m_pRule->m_nTriggers = cFwBaseRule::prpFwEditedNow | m_pRule->GetProps();
		if( m_pRule->m_sName.empty() )
		{
			Root()->LocalizeStr(
				Root()->GetString(tString(), PROFILE_LOCALIZE, PROFILE_SID_ANTIHACKER_LOC, m_pRule->isBasedOn(cFwPacketRule::eIID) ? STRID_PACKETRULEDEFNAME : STRID_APPRULEDEFNAME),
				m_pRule->m_sName);
			UpdateData(true);
		}
	}
	
	UpdatePropertiesList();
}

bool CFwRuleDlg::OnOk()
{
	if(m_pRule)
	{
		if( m_pRule->isBasedOn(cFwAppRuleEx::eIID) && ((cFwAppRuleEx *)m_pRule)->m_sAppName.empty() )
		{
			MsgBox(*this, STRID_ERRNEEDAPP, NULL, MB_OK | MB_ICONERROR);
			return false;
		}
		if( m_pRule->m_sName.empty() )
		{
			MsgBox(*this, STRID_ERRNEEDRULENAME, NULL, MB_OK | MB_ICONERROR);
			return false;
		}
		if( m_pRule->m_nTriggers & ~(m_pRule->GetProps() | cFwBaseRule::prpFwEditedNow) )
		{
			MsgBox(*this, STRID_ERRNEEDPROP, NULL, MB_OK | MB_ICONERROR);
			return false;
		}
		m_pRule->CleanupProps();
		m_pRule->m_nTriggers &= ~cFwBaseRule::prpFwEditedNow;
	}
	
	return CDialogBindingT<>::OnOk();
}

bool CFwRuleDlg::OnClicked(CItemBase* pItem)
{
	if(FwRuleProcessClicked(pItem, m_pRule))
	{
		if(pItem->m_strItemId == ITEMID_FWPROTOCOLEDIT)
			UpdatePropertiesList();
	}

	return CDialogBindingT<>::OnClicked(pItem);
}

void CFwRuleDlg::UpdatePropertiesList()
{
	CItemBase *pPropListItem = GetItem("PropList");
	if( !pPropListItem || !pPropListItem->m_pSink )
		return;

	tQWORD nProps = cFwBaseRule::prpFwRemAddr | cFwBaseRule::prpFwTimeRng;
	if( m_pRule->m_nProto == _fwPROTO_TCP || m_pRule->m_nProto == _fwPROTO_UDP )
		nProps |= cFwBaseRule::prpFwRemPort | cFwBaseRule::prpFwLocPort;
	
	if( m_pRule->isBasedOn(cFwPacketRule::eIID) )
	{
		nProps |= cFwBaseRule::prpFwLocAddr;
		if( m_pRule->m_nProto == _fwPROTO_ICMP )
			nProps |= cFwBaseRule::prpFwIcmpCode;
	}

	m_pRule->m_nTriggers &= nProps | cFwBaseRule::prpFwEditedNow;
	((CFlagsListView *)pPropListItem->m_pSink)->SetFilterMask(~nProps);
}

//////////////////////////////////////////////////////////////////////
// CFwAppRulesDlg

CFwAppRulesDlg::CFwAppRulesDlg(cSerializable* pStruct, CProfile *pPdmProfile, cAHFWAppEventReport *pAppEvent) :
	CDialogBindingT<>(pStruct),
	m_pPdmProfile(pPdmProfile),
	m_pAppEvent(pAppEvent)
{
	m_strSection = "FwAppRuleEdit";
}

bool CFwAppRulesDlg::DoModal(CItemBase* pParent, bool bNew)
{
	return CDialogBindingT<>::DoModal(pParent, m_strSection.c_str(), DLG_ACTION_OKCANCEL, bNew) == DLG_ACTION_OK;
}

void CFwAppRulesDlg::OnInited()
{
	CItemBase *pItem = Item()->GetItem(ITEMID_PDMRULE);
	if(pItem)
	{
		pItem->Enable(!!m_pPdmProfile);
		pItem->SetInt(IsPdmRuleExist());
	}
}

bool CFwAppRulesDlg::OnMenu(CPopupMenuItem * pMenu, CItemBase * pItem)
{
	if( pItem->m_strItemId != "Preset" )
		return true;
	
	if( !(m_AppRulesLst && m_ser && m_ser->isBasedOn(cFwAppRule::eIID)) )
		return false;

	cFwAppRule& Rule = *(cFwAppRule *)m_ser;
	
	cVector<cFwAppRulePreset *> Presets;
	g_FwPresets.Get();
	g_FwPresets.GetPresetsByApp(Rule.m_sAppName, Presets);
	int nSep1Pos = Presets.size();
	int nSep2Pos = nSep1Pos + 2;
	g_FwPresets.GetGeneralPresets(Presets);
	if( !Presets.size() )
		return false;

	CItemBase * pMenuItem = pMenu->GetChildFirst(false);
	for(tDWORD i = 0; i < Presets.size(); i++)
	{
		if(i)
			pMenuItem = pMenu->CloneItem(pMenuItem, "");
		if((nSep1Pos && nSep1Pos == i) || (nSep2Pos && nSep2Pos == i))
		{
			pMenuItem->m_strText = pMenuItem->m_strItemId = "Separator";
			pMenuItem = pMenu->CloneItem(pMenuItem, "");
		}

		pMenuItem->m_strItemId = Presets[i]->m_sName.c_str(cCP_ANSI);
		pMenuItem->m_strText = CFwPresets::Localize(Root(), pMenuItem->m_strItemId.c_str());
	}
	
	if( pMenuItem = pMenu->Track() )
	{
		for(tDWORD i = 0; i < Presets.size(); i++)
		{
			if(Presets[i]->m_sName == (tSTRING)pMenuItem->m_strItemId.c_str())
			{
				CFwRuleUtils::ApplyPreset(Rule, Presets[i], cFALSE);
				m_AppRulesLst->UpdateView(UPDATE_FLAG_SOURCE);
				break;
			}
		}
	}
	
	return false;
}

bool CFwAppRulesDlg::OnOk()
{
	cFwAppRule *pRule = (cFwAppRule *)m_ser;
	if( pRule && pRule->isBasedOn(cFwAppRule::eIID) )
	{
		pRule->m_bEnabled = pRule->IsAnyEnabled();
		if( m_pAppEvent )
		{
			if( !CFwRuleUtils::CheckRule(*m_pAppEvent, *(cFwAppRule *)m_ser) )
			{
				MsgBox(*this, STRID_ERRRULESNOTMATCH, NULL, MB_OK | MB_ICONERROR);
				return false;
			}
		}
	}

//	if(Item()->IsChecked(ITEMID_PDMRULE))
//		AddPdmRule();
//	else
//		RemovePdmRules();
	
	return CDialogBindingT<>::OnOk();
}

void CFwAppRulesDlg::AddPdmRule()
{
	if(m_ser && m_ser->isBasedOn(cFwAppRuleEx::eIID) && m_pPdmProfile && m_pPdmProfile->settings() && m_pPdmProfile->settings()->isBasedOn(cPDMSettings::eIID))
	{
		if(!IsPdmRuleExist())
		{
			cFwAppRuleEx &RuleEx = *(cFwAppRuleEx*)m_ser;
			cPDMSettings &PdmSett = *(cPDMSettings*)m_pPdmProfile->settings();

			cPdmAppMonitoring_Item Rule;
			Rule.m_Data.m_ImagePath = RuleEx.m_sAppName;
			Rule.m_AppStart = rga_ask;
			Rule.m_bLog = cTRUE;
			Rule.m_ContentChange = rga_ask;
			Rule.m_bContentChangeLog = cTRUE;
			Rule.m_AppStartAsChild = rga_ask;
			Rule.m_bLogAsChild = cTRUE;
			
			PdmSett.m_AppsMonitoring_List.push_back(Rule);
			m_pPdmProfile->SetSettingsDirty();
		}
	}
}

bool CFwAppRulesDlg::IsPdmRuleExist()
{
	if(m_ser && m_ser->isBasedOn(cFwAppRuleEx::eIID) && m_pPdmProfile && m_pPdmProfile->settings() && m_pPdmProfile->settings()->isBasedOn(cPDMSettings::eIID))
	{
		cFwAppRuleEx &Rule = *(cFwAppRuleEx *)m_ser;
		cPDMSettings &PdmSett = *(cPDMSettings*)m_pPdmProfile->settings();
		cVector<cPdmAppMonitoring_Item> & PdmAppRules = PdmSett.m_AppsMonitoring_List;
		
		for(tDWORD i = 0; i < PdmAppRules.size(); i++)
		{
			cPdmAppMonitoring_Item & PdmAppRule = PdmAppRules[i];
			if(PdmAppRule.m_Data.m_ImagePath == Rule.m_sAppName)
			{
				return true;
			}
		}
	}
	return false;
}

void CFwAppRulesDlg::RemovePdmRules()
{
	if( m_ser && m_ser->isBasedOn(cFwAppRuleEx::eIID) &&
		m_pPdmProfile && m_pPdmProfile->settings() &&
		m_pPdmProfile->settings()->isBasedOn(cPDMSettings::eIID) )
	{
		cFwAppRuleEx &Rule = *(cFwAppRuleEx *)m_ser;
		cPDMSettings &PdmSett = *(cPDMSettings*)m_pPdmProfile->settings();
		cVector<cPdmAppMonitoring_Item> & PdmAppRules = PdmSett.m_AppsMonitoring_List;

		bool bDirty = false;
		for(tDWORD i = 0; i < PdmAppRules.size(); i++)
		{
			cPdmAppMonitoring_Item & PdmAppRule = PdmAppRules[i];
			if(PdmAppRule.m_Data.m_ImagePath == Rule.m_sAppName)
			{
				PdmAppRules.remove(i--);
				bDirty = true;
			}
		}
		
		if( bDirty )
			m_pPdmProfile->SetSettingsDirty();
	}
}

////////////////////////////////////////////////////////////////////
// CAHFWPortsEditDlg

CAHFWPortsEditDlg::CAHFWPortsEditDlg(cVector<cFWPort> *pPorts) : 
	CSettingsDlg<>(NULL), m_pPorts(pPorts) {}

void CAHFWPortsEditDlg::OnInited()
{
	if(m_Ports)
		m_Ports.Item()->SetText(CFwRuleUtils::GetPortListStr(*m_pPorts).c_str());
}

bool CAHFWPortsEditDlg::OnOk()
{
	if (CFwRuleUtils::ParsePortListStr(m_Ports.Item()->GetText(), *m_pPorts))
	    return true;
    else
    {
		MsgBox(*this, STRID_ERRINVALIDPORT, NULL, MB_OK|MB_ICONWARNING);
		return false;
    }
}

////////////////////////////////////////////////////////////////////
// CAHFWIcmpMmgEditDlg

CAHFWIcmpMsgEditDlg::CAHFWIcmpMsgEditDlg(cSerializable* pStruct) :
	CSettingsDlg<>(pStruct) {}

void CAHFWIcmpMsgEditDlg::OnInit()
{
	if( CComboItem *pCombo = (CComboItem *)GetItem(ITEMID_ICMPCODE, GUI_ITEMT_COMBO) )
		pCombo->Fill(PROFILE_SID_AH_PORT_ICMPCODES);
	
	CSettingsDlg<>::OnInit();
}

////////////////////////////////////////////////////////////////////
// CAHFWIcmpMmgEditDlg

CAHFWPortProtocolEditDlg::CAHFWPortProtocolEditDlg(cSerializable* pStruct) :
	CSettingsDlg<>(pStruct) {}

void CAHFWPortProtocolEditDlg::OnInit()
{
	if( CComboItem *pCombo = (CComboItem *)GetItem(ITEMID_PROTOCOL, GUI_ITEMT_COMBO) )
		pCombo->Fill(PROFILE_SID_AH_PROTOCOLS);
	
	CSettingsDlg<>::OnInit();
}*/

////////////////////////////////////////////////////////////////////
// CResolveIpDlg

CResolveIpDlg::CResolveIpDlg(cSerializable* pStruct) :
	m_idResolveRequest(0),
	m_nProgress(0),
	m_nDontShowPeriod(3),
	m_pProgressBar(NULL)
{
	m_pAddr = pStruct->isBasedOn(cFwAddress::eIID) ? (cFwAddress *)pStruct : NULL;
}

void CResolveIpDlg::OnInited()
{
	tERROR error = errUNEXPECTED;
	if( m_pAddr && !m_pAddr->m_Address.empty() )
		if( cIPRESOLVER * pIpRes = g_hGui->GetIpResolver() )
			error = pIpRes->ResolveHost(&m_idResolveRequest, cAutoString(m_pAddr->m_Address), cJUST_RESOLVE);
	
	if( PR_FAIL(error) )
		Close(DLG_ACTION_CANCEL);

	m_pProgressBar = GetItem(ITEMID_PROGRESS, GUI_ITEMT_PROGRESS);
}

void CResolveIpDlg::OnEvent(tDWORD nEventId, cSerializable* pData)
{
	if( nEventId == pm_RESOLVE_INFO && m_pAddr && pData && pData->isBasedOn(cIPResolveResult::eIID) )
	{
		cIPResolveResult *pIpInfo = (cIPResolveResult *)pData;
		if( pIpInfo->m_HostName == m_pAddr->m_Address )
		{
			if( !pIpInfo->m_IPs.empty() )
			{
				m_pAddr->m_AddressIP = pIpInfo->m_IPs;
				Close(DLG_ACTION_OK);
			}
			else
			{
				Close(DLG_ACTION_CANCEL);
			}
		}
	}
}

void CResolveIpDlg::OnTimerRefresh(tDWORD nTimerSpin)
{
	if( m_nDontShowPeriod )
		if( !(--m_nDontShowPeriod) )
			Item()->Show(true);

	if( m_nProgress < 100 )
	{
		m_nProgress += 3;
		if( m_pProgressBar )
			m_pProgressBar->SetValue(cVariant(m_nProgress));
	}
	else
		Close(DLG_ACTION_CANCEL);
}


/*////////////////////////////////////////////////////////////////////
// CAHFWRuleDescription


////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////

bool FwRuleProcessClicked(CItemBase* pItem, cFwBaseRule *pRule)
{
	if(!pRule)
		return false;
	
	if(!(pItem && pItem->m_pParent && pItem->m_pParent->m_strItemType == GUI_ITEMT_RICHEDIT))
		return false;

	CItemBase *pOwner = pItem->GetOwner(true);
	if(!pOwner)
		return false;

	tString strDlgSect = "SettingsWindow.Anti_Hacker.Rules";
	strDlgSect += GUI_STR_AUTOSECT_SEP;
	strDlgSect += pItem->m_strItemId;

	bool ret = false;
	sswitch(pItem->m_strItemId.c_str())
		scase(ITEMID_REMOTEADDRSLISTEDIT)
			ret = CAHFWAddrsEditDlg(&pRule->m_aRemoteAddresses).DoModal(pOwner, "SettingsWindow.Anti_Hacker.Rules.AddrsListEdit");
			sbreak;

		scase(ITEMID_REMOTEPORTSLISTEDIT)
			ret = CAHFWPortsEditDlg(&pRule->m_aRemotePorts).DoModal(pOwner, "SettingsWindow.Anti_Hacker.Rules.PortsListEdit");
			sbreak;

		scase(ITEMID_LOCALADDRSLISTEDIT)
			ret = CAHFWAddrsEditDlg(&pRule->m_aLocalAddresses).DoModal(pOwner, "SettingsWindow.Anti_Hacker.Rules.AddrsListEdit");
			sbreak;

		scase(ITEMID_LOCALPORTSLISTEDIT)
			ret = CAHFWPortsEditDlg(&pRule->m_aLocalPorts).DoModal(pOwner, "SettingsWindow.Anti_Hacker.Rules.PortsListEdit");
			sbreak;

		scase(ITEMID_FWPROTOCOLEDIT)
			if(pRule->isBasedOn(cFwPacketRule::eIID))
			{
				ret = CAHFWPortProtocolEditDlg(pRule).DoModal(pOwner, strDlgSect.c_str());
				if(ret && pRule->m_nProto != _fwPROTO_ICMP)
					((cFwPacketRule *)pRule)->m_nIcmpCode = FwInvalidIcmpCode;
			}
			else
			{
				pRule->m_nProto = pRule->m_nProto == _fwPROTO_TCP ? _fwPROTO_UDP : _fwPROTO_TCP;
				ret = true;
			}
			if(ret && pRule->m_nProto == _fwPROTO_TCP)
			{
				if(pRule->m_eDirection == _fwInbound)  pRule->m_eDirection = _fwInboundStream;
				if(pRule->m_eDirection == _fwOutbound) pRule->m_eDirection = _fwOutboundStream;
			}
			sbreak;
		
		scase(ITEMID_FWBLOCKEDIT)
			pRule->m_bBlocking = !pRule->m_bBlocking;
			ret = true;
			sbreak;
		
		scase(ITEMID_FWDIRECTIONEDIT)
			ret = CSettingsDlg<>(pRule).DoModal(pOwner, strDlgSect.c_str());
			sbreak;

		scase(ITEMID_FWTIMERANGEEDIT)
			ret = CSettingsDlg<>(pRule).DoModal(pOwner, strDlgSect.c_str());
			sbreak;
		
		scase(ITEMID_FWICMPMSGEDIT)
			ret = CAHFWIcmpMsgEditDlg(pRule).DoModal(pOwner, strDlgSect.c_str());
			sbreak;
	send;

	if( !(pRule->m_nTriggers & cFwBaseRule::prpFwEditedNow) )
		pRule->m_nTriggers = pRule->GetProps();

	return ret;
}

//////////////////////////////////////////////////////////////////////
// CAHFWAddrList

bool CAHFWAddrList::GetRecord(CRecord &pRecord)
{
	cFWAddress *p = GetRecordData(pRecord.m_nPos);
	if(!p)
		return false;
	tString str = CFwRuleUtils::Address2Str(*p);
	pRecord.SetTextData(0, 0, 1, str.c_str());
	return true;
}

//////////////////////////////////////////////////////////////////////
// CFwAppRulesList

bool CFwAppRulesList::OnListEvent(int nItem, int nColumn, int nEvent)
{
	if(nEvent == VLISTCTRL_EVENT_CHECKED)
	{
		int nPos = Item()->GetSourcePos(nItem);
		if(nPos != -1)
		{
			ItemType *pRule = GetRecordData(nPos);
			if(pRule)
				pRule->SetEnabled(pRule->m_bEnabled);
		}
	}
	return CFwImpExpList<cFwAppRule>::OnListEvent(nItem, nColumn, nEvent);
}

tERROR CFwAppRulesList::OnEditItem(CItemBase * pOpCtl)
{
	ItemType *pRule = NULL;
	if( m_bEditNew )
	{
		tERROR err = Root()->DoCommand(*this, "browse:app(AppName,Hash)");
		if( PR_FAIL(err) || err == warnFALSE )
			return errNOT_OK;

		pRule = CFwRuleUtils::FindRule(*Vect(), EditItem().m_sAppName, NULL, EditItem().m_sCmdLine, EditItem().m_bUseCmdLine);
	}
	if( !pRule )
		pRule = &EditItem();

	if( !CFwAppRulesDlg(pRule, NULL).DoModal(*this, m_bEditNew) )
		return errNOT_OK;

	if( pRule != &EditItem() )
	{
		Item()->UpdateView(UPDATE_FLAG_SOURCE);
		return errNOT_OK;
	}
	return errOK;
}*/

/*//////////////////////////////////////////////////////////////////////
// CFwNetworkList

CFwNetworkList::CFwNetworkList() : TBase()
{
	cAHFWNetwork &GlobalNet = m_aDefItems.push_back();
	GlobalNet.m_bStealthed = cTRUE;
	GlobalNet.m_eZone = _fwnzUntrusted;
	GlobalNet.m_IP = GlobalNet.m_Mask = cIP();
}

tERROR CFwNetworkList::AddAllNetworks()
{
	if( !(Item()->m_nState & ISTATE_FROZEN) )
	{
        cFwNets FwNets; FwNets.m_aNetworks = *Vect();
		if( g_hTM->sysSendMsg(pmc_PRODUCT_OPERATION, pm_FW_UPDATE_NETWORKS, NULL, &FwNets, SER_SENDMSG_PSIZE) == errOK_DECIDED )
		{
			*Vect() = FwNets.m_aNetworks;
			Item()->UpdateView(0);
			return errOK;
		}
	}
	return errNOT_OK;
}

bool CFwNetworkList::OnClicked(CItemBase* pItem)
{
	if( pItem->m_strItemId == "Refind" )
	{
		AddAllNetworks();
		return true;
	}

	return TBase::OnClicked(pItem);
}*/

// bool CFwNetworkList::OnCanOperateItem(int nItem, int nOp)
// {
//     switch(nOp)
//     {
//     case VLISTCTRL_OP_EDIT:
//         {
//             int nPos = Item()->GetSourcePos(nItem);
//             if (nPos >= 0 && nPos < (int)Vect()->size())
//             {
//                 if (!(*Vect())[nPos].IsShown() ||   // hidden
//                      (*Vect())[nPos].IsDependent()) // dependent
//                     return false;
//             }
//         }
//         break;
//     }
//     return TBase::OnCanOperateItem(nItem, nOp);
// }

// bool CFwNetworkList::OnOperateItem(int nItem, int nOp)
// {
//     if (nOp == VLISTCTRL_OP_DELETE)
//     {
//         int nPos = Item()->GetSourcePos(nItem);
//         if (nPos >= 0 && nPos < (int)Vect()->size())
//         {
//             cFwChangeNet ChangeNet; 
//             ChangeNet.m_aNetworks.m_aNetworks = *Vect();
//             ChangeNet.m_aIndex = nPos;
// 
// 	        if( g_hTM->sysSendMsg(pmc_PRODUCT_OPERATION, pm_FW_DELETE_NETWORK, NULL, &ChangeNet, SER_SENDMSG_PSIZE) == errOK_DECIDED )
// 	        {
// 		        *Vect() = ChangeNet.m_aNetworks.m_aNetworks;
// 		        Item()->UpdateView(UPDATE_FLAG_SOURCE);
// 	        }
//             return true;
//         }
//     }
// 	return false;
// }

// bool CFwNetworkList::OnPostOperateItem(int nItem, int nOp, tPTR pValue)
// {
// 	cAHFWNetwork &newNet = *(cAHFWNetwork *)pValue;
// 
//     if (nOp == VLISTCTRL_OP_EDIT)
//     {
//         if (nItem == -1) // add
//         {
//             cFwChangeNet ChangeNet;
//             ChangeNet.m_aNetworks.m_aNetworks = *Vect();
//             ChangeNet.m_aNewNet = *(cAHFWNetwork *)pValue;
//             ChangeNet.m_aIndex = 0;
// 
// 	        if( g_hTM->sysSendMsg(pmc_PRODUCT_OPERATION, pm_FW_ADD_NETWORK, NULL, &ChangeNet, SER_SENDMSG_PSIZE) == errOK_DECIDED )
// 	        {
// 		        *Vect() = ChangeNet.m_aNetworks.m_aNetworks;
// 		        Item()->UpdateView(UPDATE_FLAG_SOURCE);
//                 int nItem = Item()->FindItemByPos(0);
//                 if (nItem != -1)
//                     Item()->SetItemFocus(nItem, true);
//                 m_pDescr->UpdateData(0);
// 	        }
//             return true;
//         }
//         else // edit
//         {
//             int nPos = nItem;
//             if (nPos >= 0 && nPos < Vect()->size())
//             {
//                 cFwChangeNet ChangeNet;
//                 ChangeNet.m_aNetworks.m_aNetworks = *Vect();
//                 ChangeNet.m_aIndex = nPos;
//                 ChangeNet.m_aNewNet = *(cAHFWNetwork *)pValue;
// 
// 	            if( g_hTM->sysSendMsg(pmc_PRODUCT_OPERATION, pm_FW_CHANGE_NETWORK, NULL, &ChangeNet, SER_SENDMSG_PSIZE) == errOK_DECIDED )
// 	            {
// 	                *Vect() = ChangeNet.m_aNetworks.m_aNetworks;
// 	                Item()->UpdateView(UPDATE_FLAG_SOURCE);
//                     m_pDescr->UpdateData(nPos);
//                 }
//             }
//             return true;
//         }
//     }
//     return false;
// }

// void CFwNetworkList::OnChangedState(CItemBase * pItem, tDWORD nStateMask)
// {
// 	TBase::OnChangedState(pItem, nStateMask);
// //	int nPos = Item()->GetSourcePos(GetSingleSelectedItem());
// //    if (nPos >= 0 && nPos < (int)Vect()->size() &&
// //        !(*Vect())[nPos].IsShown())
// //    {
// //        m_pDescr->Enable(false);
// //    }
// //    else
// //        m_pDescr->Enable(true);
// }

// void CFwNetworkList::OnChangedData(CItemBase * pItem)
// {
//     int nPos = Item()->GetSourcePos(GetSingleSelectedItem());
//     if (nPos >= 0 && nPos < Vect()->size())
//     {
//         cFwChangeNet ChangeNet;
//         ChangeNet.m_aNetworks.m_aNetworks = *Vect();
//         ChangeNet.m_aIndex = nPos;
//         ChangeNet.m_aNewNet = (*Vect())[nPos];
// 
//         if( g_hTM->sysSendMsg(pmc_PRODUCT_OPERATION, pm_FW_CHANGE_NETWORK, NULL, &ChangeNet, SER_SENDMSG_PSIZE) == errOK_DECIDED )
//         {
//             *Vect() = ChangeNet.m_aNetworks.m_aNetworks;
// //          Item()->UpdateView(UPDATE_FLAG_SOURCE);
//         }
//     }
// 
//     return TBase::OnChangedData(pItem);
// }

// bool CFwNetworkList::UpdateRecord(CRecord &pRecord)
// { 
//     bool res = TBase::UpdateRecord(pRecord); 
// 
//     int nPos = pRecord.m_nPos;
//     if (nPos >= 0 && nPos < Vect()->size())
//     {
//         cFwChangeNet ChangeNet;
//         ChangeNet.m_aNetworks.m_aNetworks = *Vect();
//         ChangeNet.m_aIndex = nPos;
//         ChangeNet.m_aNewNet = (*Vect())[nPos];
// 
//         if( g_hTM->sysSendMsg(pmc_PRODUCT_OPERATION, pm_FW_CHANGE_NETWORK, NULL, &ChangeNet, SER_SENDMSG_PSIZE) == errOK_DECIDED )
//         {
//             *Vect() = ChangeNet.m_aNetworks.m_aNetworks;
// //          Item()->UpdateView(UPDATE_FLAG_SOURCE);
//         }
//     }
// 
//     return res;
// }

// bool CFwNetworkList::GetRecord(CRecord &pRecord)
// {
// 	cAHFWNetwork* pNetwork = (cAHFWNetwork*)GetSerPtr(pRecord.m_nPos);
// 	if( pNetwork )
// 	{
//         pRecord.EnableCheck(2, pNetwork->IsShown() && 
//                                !pNetwork->IsDependent() && 
//                                pNetwork->m_eZone == _fwnzUntrusted);
// 		if( pNetwork->m_eZone != _fwnzUntrusted )
// 			pNetwork->m_bStealthed = cFALSE;
// 	}
// 
// 	CSerVectorView<cAHFWNetwork>::GetRecord(pRecord);
// 	return true;
// }

// bool CFwNetworkList::OnAddRecord(CRecord &pRecord)
// {
//     cAHFWNetwork* pNetwork = (cAHFWNetwork*)GetSerPtr(pRecord.m_nPos);
//     return pNetwork->IsShown() && !pNetwork->IsDependent();
// }

// bool CFwNetworkList::IsEqual(cSerializable *pRec1, cSerializable *pRec2)
// {
// 	cAHFWNetwork &Rec1 = *(cAHFWNetwork *)pRec1; // in the list
// 	cAHFWNetwork &Rec2 = *(cAHFWNetwork *)pRec2; // added/changed
//     if (!Rec1.m_IP.IsValid() && !Rec1.m_Mask.IsValid())
//         return Rec1.IsIPEqual(Rec2);
//     else
//         return Rec1.IsIPGreaterEqual(Rec2);
// }

/*//////////////////////////////////////////////////////////////////////
// CBannerList

class CAntiBannerImpExp : public CStrListImpExpHelper
{
public:
	CAntiBannerImpExp(CListItem * pList, cVector<cEnabledStrItem>& pData) : CStrListImpExpHelper(pList), m_Data(pData) {}
	
	cStringObj * OnGetString(int idx)         { return &m_Data[idx].m_sName; }
	void         OnPutString(const cStringObj &str) { m_Data.push_back().m_sName = str; }
	void         OnClearVector()              { m_Data.clear(); }

protected:
	cVector<cEnabledStrItem> &m_Data;
};

//////////////////////////////////////////////////////////////////////

bool CBannerList::OnClicked(CItemBase* pItem)
{
	bool (CAntiBannerImpExp::*pFunc)() = NULL;

	sswitch(pItem->m_strItemId.c_str())
	stcase(Export) pFunc = &CAntiBannerImpExp::Export; sbreak;
	stcase(Import) pFunc = &CAntiBannerImpExp::Import; sbreak;
	send;
	
	if( pFunc && Item() && _GetListType() == cEnabledStrItem::eIID )
		(CAntiBannerImpExp(Item(), *(cVector<cEnabledStrItem> *)m_v).*pFunc)();
	
	return CVectorView::OnClicked(pItem);
}

//////////////////////////////////////////////////////////////////////
// CUrlFltProfileAccountsList

#define PARCTL_PROFILEID_PARENT	1
#define PARCTL_PROFILEID_TEEN	2
#define PARCTL_PROFILEID_CHILD	3

void CUrlFltProfileAccountsList::OnInit()
{
	TBase::OnInit();
	TBase::Bind(&m_Accounts);
}

bool CUrlFltProfileAccountsList::OnSetValue(const cVariant &pValue)
{
	if( m_nProfileId != -1 )
		return true;

	m_nProfileId = pValue.ToDWORD();

	if( !m_users )
	{
		if( CItemBase * pParent = Item()->GetOwner(true) )
		{
			cSerializableObj pSer; pSer.ptr_ref() = pParent->GetCtx();
			m_users.assign(ExtractCtx(pSer, cUrlFltSettings::eIID), true);
			if( m_users && m_users->settings_byuser_getid() != cUrlFltUserSettings::eIID )
				m_users.clear();
		}
	}

	if( !m_users )
		return true;
	
	for(tDWORD i = 0; i < m_users->m_users.size(); i++)
	{
		cTaskSettingsPerUserItem& _u = m_users->m_users[i];
		if( _u.isdefault() )
			continue;

		cUrlFltUserSettings * pUfUsr = (cUrlFltUserSettings *)_u.m_settings.ptr_ref();
		if( !pUfUsr )
			continue;

		if( pUfUsr->m_nProfileId != m_nProfileId )
			continue;

		cGuiParams& _a = m_Accounts.push_back();
		_a.m_strVal1 = _u.m_sUserId;
	}

	Item()->UpdateView(UPDATE_FLAG_SOURCE);
	return true;
}

tERROR CUrlFltProfileAccountsList::OnEditItem(CItemBase * pOpCtl)
{
	cGuiParams& _cur = EditItem();

	// Browse for user
	{
		CObjectInfo _UsrInfo;
		_UsrInfo.m_strName = _cur.m_strVal1;

		if( !Root()->BrowseUserAccount(*this, _UsrInfo) )
			return errNOT_OK;

		_cur.m_strVal1 = _UsrInfo.m_strName;
	}
	
	if( m_users )
	{
		bool bCreated = false;
		cUrlFltUserSettings * pUfUsr = (cUrlFltUserSettings *)m_users->settings_byuser(_cur.m_strVal1, true, NULL, &bCreated);
		
		if( pUfUsr->m_nProfileId != m_nProfileId )
		{
			if( !bCreated && pUfUsr->m_nProfileId != PARCTL_PROFILEID_CHILD )
			{
				cUrlFltProfile * pProfile = m_users->get_profile(pUfUsr->m_nProfileId);
				if( pProfile && MsgBox(Item(), "ParCtlAssignUserToProfileWarning", NULL, MB_YESNO|MB_ICONQUESTION, pProfile) == DLG_ACTION_NO )
					return errNOT_OK;
			}

			pUfUsr->m_nProfileId = m_nProfileId;
		}
		else
			MsgBox(Item(), "ParCtlAssignUserToProfileInfo", NULL, MB_OK|MB_ICONINFORMATION);
	}

	return errOK;
}

bool CUrlFltProfileAccountsList::OnDeleteRecord(CRecord &pRecord)
{
	if( m_users )
	{
		cGuiParams& _cur = m_Accounts.at(pRecord.m_nPos);
		m_users->delete_user(_cur.m_strVal1);
	}

	return TBase::OnDeleteRecord(pRecord);
}

//////////////////////////////////////////////////////////////////////
// CUrlFltProfilesCombo

void CUrlFltProfilesCombo::OnInit()
{
	TBase::OnInit();

	cSerializableObj pSer; pSer.ptr_ref() = Item()->GetCtx();
	m_sett.assign(ExtractCtx(pSer, cUrlFltSettings::eIID), true);

	_ReinitItems();
}

void CUrlFltProfilesCombo::OnInitProps(CItemProps& pProps)
{
	TBase::OnInitProps(pProps);

	CItemPropVals& prpVal = pProps.Get(STR_PID_VALUE);
	m_strFmt = prpVal.Get(1);
}

void CUrlFltProfilesCombo::_ReinitItems()
{
	if( !m_sett )
		return;

	Item()->DeleteAllItems();

	CGuiPtr<CFieldsBinder> pFb =  Root()->CreateFieldsBinder(cSerializableObj::getDescriptor(cUrlFltProfile::eIID));
	CGuiPtr<CTextBinding> pTb =  Root()->CreateTextBinding2();
	pTb->m_pBinder = pFb;
	pTb->Init(m_strFmt.c_str());

	cVector<cUrlFltProfile>& _pl = m_sett->m_aProfiles;
	for(tDWORD i = 0; i < _pl.size(); i++)
	{
		cUrlFltProfile& _p = _pl[i];
		if( !_p.m_bEnabled )
			continue;
		
		CStructData _sd(&_p); CBindProps _bp(escUTF8);
		cStrObj strItemText; Root()->LocalizeStr(pTb->GetText(_sd, &_bp).c_str(), strItemText);
		Item()->AddItem(strItemText.data(), _p.m_nId);
	}
}

//////////////////////////////////////////////////////////////////////
// ParCtl_SwitchProfile

tERROR ParCtl_SwitchProfile(cSerializable * pCtx, CItemBase * pItem, LPCSTR strParams)
{
	CProfile * _pProfile = g_pProduct->GetProfile(AVP_PROFILE_PARENTALCONTROL);
	if( !_pProfile )
		return errNOT_OK;
	
	cSerObj<cUrlFltSettings>   _sett; _sett.assign(_pProfile->settings(NULL, cUrlFltSettings::eIID), true);
	cSerObj<cUrlFltStatistics> _stat; _stat.assign(_pProfile->statistics(NULL, cUrlFltStatistics::eIID), true);
	if( !_sett || !_stat )
		return errNOT_OK;

	if( _stat->m_aProfiles.size() == 2 && _stat->m_nCurUserProfileId == PARCTL_PROFILEID_PARENT )
	{
		cUrlFltProfile * pChild = NULL;
		for(tDWORD i = 0; i < _sett->m_aProfiles.size(); i++)
		{
			cUrlFltProfile& _p = _sett->m_aProfiles[i];
			if( _p.m_nId == PARCTL_PROFILEID_CHILD )
			{
				pChild = &_p;
				break;
			}
		}
		
		if( MsgBox(pItem, "ParCtlSwitchToChildWarning", NULL, MB_YESNO, pChild) == DLG_ACTION_NO )
			return warnFALSE;
		
		cUrlFltAskAction _Ask(cUrlFltAskAction::eSwitchProfile);
		_Ask.m_strUserId = g_hGui->GetUserId();
		_Ask.m_nProfileId = PARCTL_PROFILEID_CHILD;
		g_hTM->AskTaskAction(_pProfile->m_nRuntimeId, pmcASK_ACTION, &_Ask);
		return errOK;
	}

	CParCtlSwitchProfileDlg().DoModal(pItem, strParams, DLG_ACTION_OKCANCEL, false, true);
	return errOK;
}

//////////////////////////////////////////////////////////////////////
// CParCtlSwitchProfileDlg

void CParCtlSwitchProfileDlg::OnCreate()
{
	m_pProfile = g_pProduct->GetProfile(AVP_PROFILE_PARENTALCONTROL);
	if( m_pProfile )
	{
		m_sett.assign(m_pProfile->settings(NULL, cUrlFltSettings::eIID), true);
		m_stat.assign(m_pProfile->statistics(NULL, cUrlFltStatistics::eIID), true);
	}
	
	TBase::OnCreate();
	m_pBinding->AddDataSource(m_sett);
	m_pBinding->AddDataSource(&m_data);
}

void CParCtlSwitchProfileDlg::OnInit()
{
	m_data.m_nRes = (tPTR)-1;
	
	if( m_stat )
	{
		switch( m_stat->m_nCurUserProfileId )
		{
		case PARCTL_PROFILEID_PARENT: m_data.m_nRes = (tPTR)PARCTL_PROFILEID_CHILD; break;
		case PARCTL_PROFILEID_TEEN:   m_data.m_nRes = (tPTR)PARCTL_PROFILEID_PARENT; break;
		case PARCTL_PROFILEID_CHILD:  m_data.m_nRes = (tPTR)PARCTL_PROFILEID_PARENT; break;
		default: m_data.m_nRes = (tPTR)m_stat->m_nCurUserProfileId; break;
		}
	}
	
	if( m_data.m_nRes == (tPTR)-1 && m_sett )
	{
		cUrlFltProfile * pUfProfileCur = NULL;
		{
			cUrlFltUserSettings * pUfUserCfg = (cUrlFltUserSettings *)m_pProfile->GetUserSettings(m_sett, cUrlFltUserSettings::eIID);
			if( pUfUserCfg )
				m_data.m_nRes = (tPTR)pUfUserCfg->m_nProfileId;
		}

		if( (m_data.m_nRes == (tPTR)-1) && m_sett->m_aProfiles.size() )
			m_data.m_nRes = (tPTR)m_sett->m_aProfiles[0].m_nId;
	}

	TBase::OnInit();
	m_pBinding->Bind(*this);
	UpdateData(true);
}

bool CParCtlSwitchProfileDlg::OnOk()
{
	if( !m_sett )
		return false;
	
	UpdateData(false);
	
	cUrlFltProfile * pUfProfile = m_sett->get_profile((tDWORD)m_data.m_nRes);
	if( !pUfProfile )
		return false;

	if( !pUfProfile->m_strPassword.empty() )
	{
		if( !(g_pRoot->m_nGuiFlags & GUIFLAG_NOCRYPT) && !((CRootSink *)(Root()->m_pSink))->CheckPasswords(pUfProfile->m_strPassword, m_data.m_strVal1) )
		{
			MsgBox(*this, "ErrPswrdWrong", NULL, MB_OK|MB_ICONERROR);
			m_data.m_strVal1.erase();
			UpdateData(true);
			return false;
		}
	}

	cUrlFltAskAction _Ask(cUrlFltAskAction::eSwitchProfile);
	_Ask.m_strUserId = g_hGui->GetUserId();
	_Ask.m_nProfileId = pUfProfile->m_nId;
	g_hTM->AskTaskAction(m_pProfile->m_nRuntimeId, pmcASK_ACTION, &_Ask);
	return TBase::OnOk();
}

//////////////////////////////////////////////////////////////////////////
// CCustomSupportLink

void CCustomSupportLink::OnInit()
{
	TBase::OnInit();
	UpdateLinks();
}

void CCustomSupportLink::OnEvent(tDWORD nEventId, cSerializable * pData)
{
	TBase::OnEvent(nEventId, pData);
	
	if( nEventId == pmPROFILE_SETTINGS_MODIFIED &&
		pData && pData->isBasedOn(cProfileBase::eIID) &&
		((cProfileBase *)pData)->m_sName == g_pProduct->m_sName )
		UpdateLinks();
}

void CCustomSupportLink::UpdateLinks()
{
	{
		for(int i = 0, n = m_aLinks.size(); i < n; i++)
			m_aLinks[i]->Destroy();
		m_aLinks.clear();
	}
	
	CItemBase * pLinkTpl = Item();
	
	cBLSupportCustomization * pSuppCust = NULL;
	{
		if( cBLSettings * pBLCfg = (cBLSettings *)g_pProduct->settings(NULL, cBLSettings::eIID) )
			pSuppCust = &pBLCfg->m_SupportCustom;
	}
	
	if( !(pSuppCust && pLinkTpl && pLinkTpl->m_pParent && pLinkTpl->m_strItemId == "LinkTpl") )
		return;

	CGuiPtr<CSectionBinding> pDataSrc = Root()->CreateTextBinding();

	for(int i = 0, n = pSuppCust->m_aLinks.size(); i < n; i++)
	{
		pDataSrc->AddDataSource(&pSuppCust->m_aLinks[i]);
		
		tCHAR szId[100]; _snprintf(szId, countof(szId), "Link%u", i + 1);
		CItemBase * pLink = pLinkTpl->m_pParent->CloneItem(pLinkTpl, szId, true);

		{
			CGuiPtr<CItemBinding> pBinding = Root()->CreateItemBinding(pLink, pDataSrc);
			pBinding->Init();
			CStructData data (NULL);
			pBinding->UpdateData(true, data);
		}

		pDataSrc->ClearSources();

		m_aLinks.push_back(pLink);
		pLink->Show(true);
	}
}*/

/*
//////////////////////////////////////////////////////////////////////
// CPDM2FakeList

void CPDM2FakeList::OnInit()
{
	m_Tasks.clear();
	m_Tasks.push_back();
	
	CSerVectorView<cPDM2rtSettings>::OnInit();
	CSerVectorView<cPDM2rtSettings>::Bind(&m_Tasks);
}

void CPDM2FakeList::OnInited()
{
	CSerVectorView<cPDM2rtSettings>::OnInited();

	if( Item()->m_pParent )
	{
		cSerializableObj pSer; pSer.ptr_ref() = Item()->m_pParent->GetCtx();
		m_sett.assign(ExtractCtx(pSer, cPDM2rtSettings::eIID), true);
	}

	if( m_sett && !m_Tasks.empty() )
	{
		m_Tasks[0].m_Action = m_sett->m_Action;
		m_Tasks[0].m_bLog = m_sett->m_bLog;
	}
	UpdateView(0);
	Item()->Freeze(m_sett->IsMandatoriedByPtr(&m_sett->m_Action));

	CSerVectorView<cPDM2rtSettings>::UpdateView(0);
}

void CPDM2FakeList::OnChangedData(CItemBase * pItem)
{
	CSerVectorView<cPDM2rtSettings>::OnChangedData(pItem);

	if( Item() == pItem && m_sett && !m_Tasks.empty() )
	{
		m_sett->m_Action = m_Tasks[0].m_Action;
		m_sett->m_bLog = m_Tasks[0].m_bLog;
	}
}

//////////////////////////////////////////////////////////////////////

CProductInfoWindow::CProductInfoWindow()
{
    m_bCredits = false;
}

void CProductInfoWindow::OnInit()
{
    TBase::OnInit();
    Show();
}

void CProductInfoWindow::Show()
{
    Item()->LockUpdate(true);
    TBase::Show("About",   !m_bCredits);
    TBase::Show("Credits",  m_bCredits);
    Item()->LockUpdate(false);
}

bool CProductInfoWindow::OnClicked(CItemBase *pItem)
{
    if (pItem->m_strItemId == "logo") 
    {
        m_bCredits = !m_bCredits;
        Show();
    }
        
    return TBase::OnClicked(pItem);
}*/


//////////////////////////////////////////////////////////////////////////
// function GetSysInfo 
long __stdcall CallBackSysInfo(int TotalProcent, int CurrentProcent, char* name)
{
	cSerObj<cGuiParams> _Data; _Data.init(cGuiParams::eIID);
	_Data->m_nVal1 = (tPTR)TotalProcent;
	_Data->m_nVal2 = (tPTR)CurrentProcent;
	
	g_pRoot->SendEvent(EVENTID_SYSINFO_CALLBACK, _Data);
	return 0;
}


bool GetSysInfo(const cStrObj& strFile, fCallBackFunc pfCallBackFunc)
{
#if defined (_WIN32)
	HMODULE hLib = LoadLibrary("GetSI.dll");
	if(!hLib)
	{
		PR_TRACE((g_root, prtERROR, "gui\tCan't load GetSI.dll\n"));
		return false;
	}

	fSaveReport pSaveReport;
	pSaveReport = (fSaveReport)GetProcAddress(hLib, "SaveReport");
	if(!pSaveReport)
	{
		PR_TRACE((g_root, prtERROR, "gui\tCan't find SaveReport function\n"));
		FreeLibrary(hLib);
		return false;
	}

	bool res = !pSaveReport(strFile.c_str(cCP_ANSI), pfCallBackFunc);
	if (!res)
		PR_TRACE((g_root, prtERROR, "gui\tSaveReport fails\n"));

	FreeLibrary(hLib);

	return res;
#else
	return false;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CIConnectionHelper

#if defined (_WIN32)
class CIConnectionHelper
{
public:
	CIConnectionHelper() : m_hInternet(NULL), m_hConnection(NULL){}
	
	~CIConnectionHelper()
	{
		if(m_hConnection)
			InternetCloseHandle(m_hConnection), m_hConnection = NULL;
		if(m_hInternet)
			InternetCloseHandle(m_hInternet), m_hInternet = NULL;
	}

	HINTERNET OpenKLDumpFTP()
	{
		m_hInternet = InternetOpen(NULL, PRE_CONFIG_INTERNET_ACCESS, NULL, NULL, 0);
		if( NULL == m_hInternet )
			return NULL;

		HKEY hDmp;
		cStrObj url(L"kavdumps.kaspersky.com"), user(L"kavdumps"), pwd(L"UxzAbKFLufVBSg8Y");
		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, VER_PRODUCT_REGISTRY_PATH "\\settings\\dump", 0, KEY_READ, &hDmp))
		{
			BYTE buf[1024];
			DWORD type, size = sizeof(buf);
			if (ERROR_SUCCESS == RegQueryValueEx(hDmp, "FTP", NULL, &type, buf, &size))
				url = (const char*) buf;
			size = sizeof(buf);
			if (ERROR_SUCCESS == RegQueryValueEx(hDmp, "User", NULL, &type, buf, &size))
				user = (const char*) buf;
			size = sizeof(buf);
			if (ERROR_SUCCESS == RegQueryValueEx(hDmp, "Password", NULL, &type, buf, &size))
				pwd = (const char*) buf;
			RegCloseKey(hDmp);
		}

		m_hConnection = InternetConnect(
			m_hInternet,
			url.c_str(cCP_ANSI),
			INTERNET_INVALID_PORT_NUMBER,
			user.c_str(cCP_ANSI), 
			pwd.c_str(cCP_ANSI), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
		
		return m_hConnection;
	}

private:
	HINTERNET m_hInternet;
	HINTERNET m_hConnection;
};
#endif


/////////////////////////////////////////////////////////////////////////////
// CSendHelper

bool CSendHelper::Send(cDumpFiles* dumps, tDWORD* pState)
{ 
	m_pDumps = dumps;
	m_pState = pState;
	return PR_SUCC(cThreadTaskBase::start(*this)); 
}

void CSendHelper::do_work()
{
#if defined (_WIN32)
	m_err.m_nErr = errOK;
	m_err.m_nOperationCode = 0;
	m_err.m_nVal1 = NULL;
	m_err.m_strVal1.clear();

	if( !m_pDumps )
		return;

	cVector<cDumpFile>& files = m_pDumps->m_Dumps;

	// Create sysinfo 
	m_err.m_nOperationCode = CSendTroublesDialog::SS_CREATE_SYSINFO;
	if( m_pState )
		*m_pState = m_err.m_nOperationCode;

	TCHAR SIFile[MAX_PATH];
	MakeSpecialName(SIFile, MAX_PATH, "sysinfo", "txt");
	cStrObj strSysInfo(SIFile);

	for (tDWORD i = 0; i < files.size(); ++i)
	{
		if (files[i].m_bSysInfo)
		{
			if (GetSysInfo(strSysInfo, &CallBackSysInfo)) // NULL
			{
				WIN32_FILE_ATTRIBUTE_DATA fa;
				files[i].m_dwSize = GetFileAttributesEx(strSysInfo.c_str(cCP_ANSI), GetFileExInfoStandard, &fa) ? fa.nFileSizeLow : 0;
				files[i].m_strPath = strSysInfo;
			}
			else
				files.remove(i);
			break;
		}
	}

	// Sorting...  min-size first (for corrupted zips case)
	for (tDWORD i = 0; i < files.size() - 1; ++i)
	{
		tDWORD min = i;
		for (tDWORD j = i + 1; j < files.size(); ++j)
		{
			if (files[min].m_dwSize > files[j].m_dwSize)
				min = j;
		}
		if (min != i)
		{
			cDumpFile t = files[i];
			files[i] = files[min];
			files[min] = t;
		}
	}

	// Packing...
	m_err.m_nOperationCode = CSendTroublesDialog::SS_MAKING_ZIP;
	if( m_pState )
		*m_pState = m_err.m_nOperationCode;

	TCHAR ZipFile[MAX_PATH];
	MakeSpecialName(ZipFile, MAX_PATH, m_pDumps->m_bSendDumps ? "dump" : "log", "zip");
	cStrObj strTempFileName(ZipFile);
	cIOObj pZip(*g_hGui, cAutoString(strTempFileName), fACCESS_RW, fOMODE_CREATE_ALWAYS);
	m_err.m_nErr = pZip.last_error();
	if (PR_FAIL(m_err.m_nErr))
		return;

	cAutoObj<cOS> pPackageOS;
	m_err.m_nErr = pPackageOS.create(pZip, IID_OS, PID_UNIVERSAL_ARCHIVER);
	if (PR_FAIL(m_err.m_nErr))
		return;
	PR_VERIFY_SUCC(pPackageOS->propSetDWord(ppMINI_ARCHIVER_PID, PID_MINIZIP));
	m_err.m_nErr = pPackageOS.done();
	if (PR_FAIL(m_err.m_nErr))
		return;

	for (tDWORD i = 0; i < files.size(); ++i)
	{
		tDWORD pos = files[i].m_strPath.find_last_of(L"\\");
		if (pos == cStrObj::npos)
			continue;
		cStrObj filename = files[i].m_strPath.substr(pos + 1);
		cIO * pFile = NULL;
		m_err.m_nErr = pPackageOS->IOCreate(&pFile, cAutoString(filename), fACCESS_RW, fOMODE_CREATE_ALWAYS);
		if (PR_FAIL(m_err.m_nErr))
			return;
		m_err.m_nErr = PrCopy(cAutoString(files[i].m_strPath), (hOBJECT)pFile, fPR_FILE_REPLACE_EXISTING);
		if (PR_FAIL(m_err.m_nErr))
			return;
	}

	if (PR_FAIL(pPackageOS.clean()) || PR_FAIL(pZip.clean()))
	{
		m_err.m_nErr = errOPERATION_CANCELED;
		return;
	}

	if (!strSysInfo.empty())
		PrDeleteFile(cAutoString(strSysInfo), 0);

	// Prepare ftp filename with file size
	m_err.m_nOperationCode = CSendTroublesDialog::SS_SENDING_FTP;
	if( m_pState )
		*m_pState = m_err.m_nOperationCode;

	WIN32_FILE_ATTRIBUTE_DATA fa = {};
	if (!GetFileAttributesEx(strTempFileName.c_str(cCP_ANSI), GetFileExInfoStandard, &fa) || fa.nFileSizeLow == 0)
	{
		m_err.m_nErr = errOBJECT_NOT_CREATED;
		PrDeleteFile(cAutoString(strTempFileName), 0);
		return;
	}

	CIConnectionHelper InetHelper; 
	HINTERNET hConnection = InetHelper.OpenKLDumpFTP();
	if( hConnection )
	{
		cStrObj id;
		HKEY hDmp;
		if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, VER_PRODUCT_REGISTRY_PATH "\\settings", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hDmp, NULL))
		{
			BYTE buf[1024];
			DWORD type, size = sizeof(buf);
			if (ERROR_SUCCESS == RegQueryValueEx(hDmp, "DumpUID", NULL, &type, buf, &size))
				id = (const char*) buf;
			if (id.empty())
			{
				GUID guid;
				if (CoCreateGuid (&guid) == S_OK)
				{
					id.format(cCP_UNICODE, L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
						guid.Data1, guid.Data2, guid.Data3, 
						guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
						guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
					RegSetValueEx(hDmp, "DumpUID", 0, REG_SZ, (LPCBYTE)(LPCSTR)id.c_str(cCP_ANSI), id.memsize(cCP_ANSI));
				}
			}
			RegCloseKey(hDmp);
		}

		cStrObj suffix;
		suffix.format(cCP_UNICODE, L"%i.%S", fa.nFileSizeLow, id.data());
		TCHAR name[MAX_PATH];
		MakeSpecialName(name, MAX_PATH, suffix.c_str(cCP_ANSI), m_pDumps->m_bSendDumps ? "dump.zip" : "log.zip");
		cStrObj ftpname = name;

		tDWORD pos = ftpname.find_last_of(L"\\");
		if (pos != cStrObj::npos)
		{
			cStrObj path = g_hGui->ExpandEnvironmentString("//%ProductVersion%");
			cBLStatistics stat;
			if (g_pProduct->GetProfile(AVP_PROFILE_PROTECTION)->GetInfo(&stat))
			{
				tWCHAR hf[2] = { 0, 0 };
				for (tDWORD i = 0; i < stat.m_strProductHotfix.size(); ++i)
				{
					if (stat.m_strProductHotfix[i] > hf[0])
						hf[0] = stat.m_strProductHotfix[i];
				}
				if (hf[0])
					path += hf;
			}
			path += "/";
			path += ftpname.substr(pos + 1);
			if (!FtpPutFile(hConnection, strTempFileName.c_str(cCP_ANSI), path.c_str(cCP_ANSI), FTP_TRANSFER_TYPE_BINARY, 0))
			{
				char buf[1024] = "";
				FormatMessage(FORMAT_MESSAGE_FROM_HMODULE, GetModuleHandle("wininet.dll"), GetLastError(), 0, (LPSTR)buf,1024,NULL);
				m_err.m_nErr = errNOT_OK;
				m_err.m_strVal1 = buf;
			}
		}
	}

	PrDeleteFile(cAutoString(strTempFileName), 0);
#endif
}

void CSendHelper::on_exit()
{
	if( m_pState )
		m_pDumps = NULL;
	if( m_pDumps )
		delete m_pDumps, m_pDumps = NULL;

	g_pRoot->SendEvent(EVENTID_SEND_TROUBLES_COMPLETE);
}

//////////////////////////////////////////////////////////////////////////
// CSendTroublesDialog

CSendTroublesDialog::CSendTroublesDialog(tBOOL bSendDumps) : 
	TBase(this), m_SendHelper(),
	m_dtLastChecked( cDateTime::current_utc ), m_SendState(SS_DONE), m_SendPercent(0)
{
	m_strSection = "SendTroublesDialog";
	m_bSendDumps = bSendDumps;

	m_SendHelper.init(g_hGui);
}

CSendTroublesDialog::~CSendTroublesDialog()
{
	m_SendHelper.wait_for_stop();
	m_SendHelper.de_init();
}

bool CSendTroublesDialog::GetNewDumps()
{
#ifdef _WIN32
	//
	cBLSettings* pSet = (cBLSettings*)g_pProduct->settings(AVP_PROFILE_PROTECTION, cBLSettings::eIID);
	if( !pSet )
		return false;

	cDateTime dtFilesAfter = m_bSendDumps ? &pSet->m_LastShownDumpsTime : &pSet->m_LastShownTracesTime;

	cStrObj res;
	char dumpsDir[MAX_PATH];
	if (!GetSpecialDir(dumpsDir, MAX_PATH))
		return false;

	cAutoObj<cObjPtr> pFiles;
	if( PR_FAIL(g_hGui->sysCreateObject(pFiles, IID_OBJPTR, PID_NATIVE_FIO))
		|| PR_FAIL(pFiles->set_pgOBJECT_PATH(dumpsDir, strlen(dumpsDir), cCP_ANSI))
		|| PR_FAIL(pFiles.done()))
		return false;
	
	// первым элементом добавляем системный файл
	cDumpFile& dump = m_Dumps.push_back();
	dump.m_bSysInfo = cTRUE;
	dump.m_dwSize = 1024*1024;
	dump.m_dwPackSize = dump.m_dwSize / 8;

	pFiles->propSetStr(NULL, pgMASK, m_bSendDumps ? (tPTR)"*.dmp" : (tPTR)"*.log");
	pFiles->Reset(cFALSE);
	while (PR_SUCC(pFiles->Next()))
	{
		if (pFiles->get_pgIS_FOLDER())
			continue;

		cDateTime lwt;
		pFiles->get_pgOBJECT_LAST_WRITE_TIME(lwt);

		if (lwt > dtFilesAfter)
		{
			tDWORD dwSize = pFiles->get_pgOBJECT_SIZE();
			if (dwSize > 1024) // явно битые не шлём
			{
				cDumpFile& dump = m_Dumps.push_back();
				dump.m_bSysInfo = cFALSE;
				dump.m_strPath.assign(pFiles, pgOBJECT_FULL_NAME);
				dump.m_dwSize = dwSize;
				dump.m_dwPackSize = m_bSendDumps ? (dwSize / 4) : (dwSize / 8);
			}
		}
	}		
	
	m_dtLastChecked = cDateTime::now_utc();

	return m_Dumps.size() > 1;
#else
	return false;
#endif
}

bool CSendTroublesDialog::CheckInetConnection()
{
#ifdef _WIN32
	CIConnectionHelper InetHelper; 
	return NULL == InetHelper.OpenKLDumpFTP() ? false : true;
#else
	return false;
#endif
}

void CSendTroublesDialog::OnInit()
{
	if( m_bSendDumps == cFALSE )
		GetNewDumps();
}

bool CSendTroublesDialog::OnClicked(CItemBase *pItem)
{
	if(pItem->m_strItemId == "Send" && m_SendState == SS_DONE )
	{
		m_SendState = SS_PREPARING;

		CStructData data (NULL, STRUCTDATA_ALLSECTIONS);
		Item()->UpdateData(false, &data);

#ifdef _WIN32
		cDumpFiles* dumps = new cDumpFiles(*this);
		for (int i = dumps->m_Dumps.size() - 1; i >= 0; --i)
		{
			if (!dumps->m_Dumps[i].m_bSend)
				dumps->m_Dumps.remove(i);
		}
		
		if (dumps->m_Dumps.empty() || (dumps->m_Dumps.size() == 1 && dumps->m_Dumps[0].m_bSysInfo) )
		{	
			// нечего посылать
			delete dumps;
			Close();
		}
		else 
		{
			m_SendPercent = 0;
			m_SendHelper.Send(dumps, &m_SendState);
		}
#endif
	}

	return TBase::OnClicked(pItem);
}

void CSendTroublesDialog::OnEvent(tDWORD nEventId, cSerializable * pData)
{
	TBase::OnEvent(nEventId, pData);

	if(	nEventId == EVENTID_SEND_TROUBLES_COMPLETE )
	{
		m_SendState = SS_DONE;

		cGuiErrorInfo* pErr = m_SendHelper.GetError();
		if( PR_SUCC(pErr->m_nErr) )	
		{
			if( m_bSendDumps == cFALSE )
			{
				cBLSettings* pSet = (cBLSettings*)g_pProduct->settings(AVP_PROFILE_PROTECTION, cBLSettings::eIID);
				if( pSet )
				{
					m_dtLastChecked.CopyTo(pSet->m_LastShownTracesTime);
					g_pProduct->SaveSettings();
				}
			}
		}
//		else
//			MsgBoxErr(g_pRoot, pErr, "SendTroublesErrorDescr");

		Close();
	}
	else if( nEventId == EVENTID_SYSINFO_CALLBACK )
	{
		if( pData && pData->isBasedOn(cGuiParams::eIID))
		{
			cGuiParams* pParams = (cGuiParams*)pData;
			m_SendPercent = (tDWORD)pParams->m_nVal1;
			if( m_SendPercent == 100)
				m_SendPercent = 0;
		}
	}
}

void CSendTroublesDialog::OnTimerRefresh(tDWORD nTimerSpin)
{
	if( GUI_TIMERSPIN_INTERVAL(1000) )
		UpdateData(true);
}

//////////////////////////////////////////////////////////////////////
// CNetFrameDetail

CNetFrameDetail::CNetFrameDetail() : m_bNetParserCreated(false)
{
	m_pReport = g_hGui->m_pReport;
	
	// объект NetDetails
	m_bNetParserCreated = CheckCreateNetParser();
}

bool CNetFrameDetail::SetItemData(cSer *pData)
{ 
	m_FrameDetails.clearAll();
	m_FrameBin.clear();

	CStructData data(NULL);
	data.m_staticData = true;

	// объект NetDetails
	m_bNetParserCreated = CheckCreateNetParser();

	if(!m_pReport || !m_bNetParserCreated || !pData || !pData->isBasedOn(cSnifferFrameReport::eIID))
	{
		if( !pData || !pData->isBasedOn(cSnifferFrameReport::eIID) )
		{
			PR_TRACE((g_root, prtERROR, "gui\tCNetFrameDetail::SetItemData()\tcSnifferFrameReport data inavaliable \n"));	
		}

		m_FrameDetails.m_fmt_item.m_id = 0xffff;
		Item()->UpdateData(true, &data);
		return true;
	}

	tERROR error = errOK;

	tObjectId objID = ((cSnifferFrameReport*)pData)->m_ObjectID;
	
	tDWORD size = 0;
	error = m_pReport->GetObjectData( objID, 0, 0, &size, 0 );
	if( PR_FAIL(error) )
	{
		PR_TRACE((g_root, prtERROR, "gui\tCNetFrameDetail::SetItemData()\tFailed to get report data\n"));	
		m_FrameDetails.m_fmt_item.m_id = 0xffff;
		Item()->UpdateData(true, &data);
		return true;
	}

	m_FrameBin.resize(size);
	size = 0;
	error = m_pReport->GetObjectData( objID, m_FrameBin.data(), m_FrameBin.size(), &size, 0 );
	if( PR_FAIL(error) )
	{
		PR_TRACE((g_root, prtERROR, "gui\tCNetFrameDetail::SetItemData()\tFailed to get report data\n"));	
		m_FrameDetails.m_fmt_item.m_id = 0xffff;
		Item()->UpdateData(true, &data);
		return true;
	}

	cSnifferAskAction AskAction;
	AskAction.m_pDetails = (tPTR)&m_FrameDetails;
	AskAction.m_pData	 = (tPTR)&m_FrameBin;
	error = m_NetParser->AskAction(0, &AskAction);
	if( PR_FAIL(error))	
	{
		PR_TRACE((g_root, prtERROR, "gui\tCNetFrameDetail::SetItemData()\tFailed to parse sniffer data\n"));	
		m_FrameDetails.m_fmt_item.m_id = 0xffff;
	}
	
	Item()->UpdateData(true, &data);

	CItemBase* pItem = Item()->GetItem("Tree");
	if( pItem && pItem->m_pSink && pItem->m_strItemType == GUI_ITEMT_LISTCTRL)	{
		((CNetDetailsListSink*)pItem->m_pSink)->OnListEvent(CListItemSink::leFocusChanged);
	}
	
	return true; 
}

bool CNetFrameDetail::CheckCreateNetParser()
{
	if(m_bNetParserCreated)
		return true;

	// объект NetDetails
	if( PR_SUCC( g_hGui->sysCreateObject( (hOBJECT*)&m_NetParser, IID_TASK, PID_NETDETAILS ) ))
		return true;

	PR_TRACE((g_root, prtERROR, "gui\tCan't create NetParser object\n"));
	return false;
}


//////////////////////////////////////////////////////////////////////////
// CNetDetailsListSink

void CNetDetailsListSink::SelectNodeByDataPos(int pos)
{
	if( pos == -1 )
		return;

	cRowId nItem;
	FindNodeIndxByDataPos(Item(), (tDWORD)pos, nItem);
	Item()->SetItemFocus(nItem, true);
}

bool CNetDetailsListSink::OnListEvent(eListEvents nEvent)
{
	if( nEvent == leFocusChanged)
	{
		CItemBase* pItem = Item()->m_pParent->GetItem("HexView");
		if( pItem && pItem->m_pSink && pItem->m_strItemType == GUI_ITEMT_HEXVIEWER)
		{
			cSer* pSer = GetSelItemData(TOR(cRowId, ()));

			if( pSer && pSer->isBasedOn(cSnifferFrameDetailsItem::eIID) )
			{
				const FmtItem& fmt = ((cSnifferFrameDetailsItem*)pSer)->m_fmt_item;
				((CHexViewSink*)pItem->m_pSink)->SetCurSel(fmt.m_offData, fmt.m_lenData, true, true);
			}
		}
	}
	return CSerTreeView::OnListEvent(nEvent);
}

void CNetDetailsListSink::FindNodeIndxByDataPos(const cRowNode* pRNode, tDWORD dwPos, cRowId &nItem)
{
	for(size_t item = 0; item < pRNode->Nodes().size(); item++)
	{
		cSer* pSer = GetItemData(TOR(cRowId, (pRNode->Nodes()[item], 0)) );
		if( pSer && pSer->isBasedOn(cSnifferFrameDetailsItem::eIID))
		{
			const FmtItem& fmt = ((cSnifferFrameDetailsItem*)pSer)->m_fmt_item;
			if( fmt.m_lenData > 0 && (dwPos >= fmt.m_offData && dwPos <= (fmt.m_offData + fmt.m_lenData - 1)) )
			{
				nItem = TOR(cRowId, (pRNode->Nodes()[item], 0));
			}
		}
		
		FindNodeIndxByDataPos(pRNode->Nodes()[item], dwPos, nItem);
	}
}

//////////////////////////////////////////////////////////////////////////
// CHexSnifferSink

void CHexSnifferSink::OnInited()
{
	CItemBase* pItem = m_pItem->m_pParent->GetItem("Tree");
	if( pItem && pItem->m_pSink && pItem->m_strItemType == GUI_ITEMT_LISTCTRL)	{
		m_pNetDtlsSnk = (CNetDetailsListSink*)pItem->m_pSink;
	}
}

void CHexSnifferSink::OnNextPrev(bool bNext)
{
	if(m_pNetDtlsSnk)
	{
		int pos = 0;
		if( bNext )		{
			int max_pos = ((CHexViewItem*)m_pItem)->DataSize() - 1;
			pos = CHexViewSink::m_selEnd + 1;
			if( pos > max_pos )
				pos = max_pos;
		}
		else		{
			pos = CHexViewSink::m_selStart - 1;
			if( pos < 0 )
				pos = 0;
		}

		m_pNetDtlsSnk->SelectNodeByDataPos(pos);
	}
}

void CHexSnifferSink::OnPosSelected(int pos)
{
	if(m_pNetDtlsSnk)
		m_pNetDtlsSnk->SelectNodeByDataPos(pos);
}


//////////////////////////////////////////////////////////////////////////
// CCmnStatisticList

CCmnStatisticList::CCmnStatisticList() : 
	m_eDbType(dbRTP), m_nLastChecked(0)
{ 
	m_vCmnStat.push_back(cCmnStatisticItem((tDWORD)eVirware, 0, 0));
	m_vCmnStat.push_back(cCmnStatisticItem((tDWORD)eTrojware, 0, 0));
	m_vCmnStat.push_back(cCmnStatisticItem((tDWORD)eMalware, 0, 0));
	m_vCmnStat.push_back(cCmnStatisticItem((tDWORD)eAdware, 0, 0));
	m_vCmnStat.push_back(cCmnStatisticItem((tDWORD)ePornware, 0, 0));
	m_vCmnStat.push_back(cCmnStatisticItem((tDWORD)eRiskware, 0, 0));
	m_vCmnStat.push_back(cCmnStatisticItem((tDWORD)eXFiles, 0, 0));

	m_bFirstCheck = cTRUE;
	UpdateStatistics(); 
}
	
void CCmnStatisticList::OnTimerRefresh(tDWORD nTimerSpin)
{
	if( GUI_TIMERSPIN_INTERVAL(5000) && cFALSE == m_bFirstCheck )
		UpdateStatistics();
}

void CCmnStatisticList::UpdateStatistics()
{
	cReportDB* pReport = g_hGui->m_pReport;
	if( !pReport )
		return;
	
	tDWORD nCount = 0; tQWORD ts = 0;
	pReport->GetDBInfo(m_eDbType, &nCount, NULL, &ts);

	tReportEvent Event;

	MAP_STAT map;

	tDWORD i = m_nLastChecked;
	for(; i < nCount; i++)
	{
		pReport->GetEvents(dbRTP, i, &Event, sizeof(tReportEvent), 1, NULL);
		if( Event.m_DetectType != 0 )
		{
			map[(tDWORD)Event.m_DetectType].m_Total++;
			if( Event.m_Verdict != eNOT_DISINFECTED && Event.m_Verdict != eNOT_DELETED)
				map[(tDWORD)Event.m_DetectType].m_Treated++;
		}
	}
	
	// обновление статистики
	for(MAP_STAT::iterator it = map.begin(); it != map.end(); it++)
	{
		tINT index = FindIndex((*it).first);
		if( index != -1 )
		{
			m_vCmnStat[index].m_dwTreated = (*it).second.m_Treated;
			m_vCmnStat[index].m_dwTotal = (*it).second.m_Total;
		}
		else
		{
			m_vCmnStat.push_back(cCmnStatisticItem((*it).first, (*it).second.m_Total, (*it).second.m_Treated));
		}
	}

	m_nLastChecked = i;
	m_bFirstCheck = cFALSE;
}

tINT CCmnStatisticList::FindIndex(tDWORD DetectType)
{
	for(tDWORD i = 0; i < m_vCmnStat.size(); i++)
		if(m_vCmnStat[i].m_Type == DetectType ) return (tINT)i;
	return -1;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
