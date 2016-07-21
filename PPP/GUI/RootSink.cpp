// RootSink.h: implementation for the CKav6RootSink classes.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "RootSink.h"
#include "ListsView.h"
#include "FWRules.h"
#include "ScanObjects.h"
#include "SettingsDlgs.h"
#include "ServiceSettings.h"

#include "../bl/Holders.h"

#include <structs/s_oas.h>
#include <plugin/p_dtreg.h>
#include <plugin/p_cryptohelper.h>
#include <plugin/p_memmod.h>
#include <plugin/p_memmodscan.h>
#include <iface/i_report.h>

#define STRID_OBJECTINFO                    "ScanObjectInfo"

//////////////////////////////////////////////////////////////////////
// Commands

/*
static tERROR ResetSettings(cSerializable * pCtx, CItemBase * pItem)
{
	if( !pCtx || !pCtx->isBasedOn(CProfileCtx::eIID) )
		return errPARAMETER_INVALID;
	
	CProfile * pProfile = ((CProfileCtx *)pCtx)->m_pProfile;
	if( !pProfile )
		return errPARAMETER_INVALID;

	CSettingsReseter reseter(pProfile);
	reseter.FindImportantSettings();
	
	if( reseter.IsEmpty() )
	{
//		if( MsgBox(pItem, "ResetSettings", NULL, MB_YESNO) != DLG_ACTION_YES )
//			return warnFALSE;
	}
	else
	{
		if( CDialogBindingT<>(&reseter).DoModal(pItem, "SettingsWindow.Reset", DLG_ACTION_OKCANCEL) != DLG_ACTION_OK )
			return warnFALSE;
	}
	
	reseter.Reset();

	if( CItemBase * pOwner = pItem->GetOwner(true) )
		if( CItemBase * pView = pOwner->GetItem((LPCSTR)pProfile->m_sName.c_str(cCP_ANSI)) )
			pView->SetChangedData();
		else 
			pItem->SetChangedData(); // возможно, SetChangedData нужно вызывать сразу у pItem, он сам поднимится до вьюхи
	
	return errOK;
}

static tERROR SaveLoadSettings(cSerializable * pCtx, CItemBase * pItem, LPCSTR strCommand)
{
	if( !pCtx || !pCtx->isBasedOn(CProfileCtx::eIID) )
		return errPARAMETER_INVALID;

	CProfile * pProfilesList = ((CProfileCtx *)pCtx)->m_pProfilesList;
	if( !pProfilesList )
		return errPARAMETER_INVALID;

	bool bSaveConfig = false;
	COMMAND_ITEM(save) bSaveConfig = true;

	cMsgParams Params;
	if( !g_pRoot->BrowseFile(pItem, pItem->LoadLocString(tString(), "BrowseForConfig"), "cfg", Params.m_strVal1, bSaveConfig) )
		return warnFALSE;

	cProfileEx oTopProile;
	Params.m_nVal1 = &oTopProile;
	
	if( bSaveConfig )
	{
		pProfilesList->PopSettings();
		pProfilesList->CopyTo(oTopProile);
	}

	tERROR error = g_hTM->sysSendMsg(pmc_SETTINGS, bSaveConfig ? pm_SAVE_SETTINGS : pm_LOAD_SETTINGS, NULL, &Params, SER_SENDMSG_PSIZE);
	if( PR_FAIL(error) )
	{
		cGuiErrorInfo loadErrInfo;
		loadErrInfo.m_nErr = error;
		MsgBoxErr(pItem, &loadErrInfo, bSaveConfig ? "SettingsOperationError" : "SettingsLoadError");
		return error;
	}

	if( !bSaveConfig )
	{
		g_hTM->sysSendMsg(pmc_SETTINGS, pm_SETTINGS_UPGRADE, NULL, &oTopProile, SER_SENDMSG_PSIZE);

		// Dont allow to overwrite password settings
		{
			cBLSettings * pSrcCfg = (cBLSettings *)(cTaskSettings *)oTopProile.settings();
			cBLSettings * pDstCfg = (cBLSettings *)pProfilesList->settings(NULL, cBLSettings::eIID);
			
			if( pDstCfg && pSrcCfg && pSrcCfg->isBasedOn(cBLSettings::eIID) )
				CSettingsReseter::CopyNotResetableProtSettings(pSrcCfg, pDstCfg);
		}

		CProfile *pDst = pProfilesList;
		if( pProfilesList->m_sName != oTopProile.m_sName )
			pDst = pProfilesList->GetProfile(oTopProile.m_sName.c_str(cCP_ANSI));

		if( !pDst )
			return errUNEXPECTED;

		pDst->CopyFrom(oTopProile, true);
		pDst->ApplyPolicy();
		
		if( CItemBase * pOwner = pItem->GetOwner(true) )
		{
			pOwner->SendEvent(EVENTID_SETTINGS_UPDATE, NULL, true);
			pOwner->SendEvent(EVENTID_SETTINGS_LOADED, NULL, true);
			pOwner->SetChangedData();
		}
		
		pDst->SetSettingsDirty(true, true);
	}
	
	return error;
}
*/
bool FillAppListMenu(CPopupMenuItem* pMenu)
{
	CItemBase * pAppList = pMenu->GetItem("AppList");
	if( !pAppList )
		return false;

	hOS hOs = 0;
	tERROR err;

	std::vector<cStrObj> setProcesses;
	tPROCESS TmProcess;

	if (g_hGui->GetObjectProcess && g_hGui->CreateObjectQuick)
	{
		err = g_hGui->GetObjectProcess((hOBJECT)g_hTM, &TmProcess);
		if (PR_SUCC(err))
			err = g_hGui->CreateObjectQuick(TmProcess, (hOBJECT)g_root, (hOBJECT*) &(hOs), IID_OS, PID_MEMSCAN, 0);
	}
	else
		err = errNOT_FOUND;

	if( PR_FAIL(err) )
		err = CALL_SYS_ObjectCreateQuick(g_root, (hOBJECT*) &(hOs), IID_OS, PID_MEMSCAN, 0);

	if( PR_SUCC(err) )
	{
		// creating enumerator
		hObjPtr hPtr = 0;
		err = CALL_OS_PtrCreate(hOs, &hPtr, 0);
		if (PR_SUCC(err))
		{
			err = CALL_SYS_ObjectCreateDone(hPtr);
			if (PR_SUCC(err))
			{
				cStrObj strPath;
				while (PR_SUCC(err = CALL_ObjPtr_Next(hPtr)))
				{
					strPath.assign(hPtr, pgOBJECT_FULL_NAME);
					if( strPath.empty() )
						continue;

					size_t i;
					for(i = 0; i < setProcesses.size(); i++)
						if( setProcesses[i].compare(strPath, fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ )
							break;

					if( i != setProcesses.size() )
						continue;

					CItemBase* pApp = pMenu->m_pRoot->CreateItem(NULL, NULL);
					pMenu->m_pRoot->LocalizeStr(pApp->m_strText, strPath.data(), strPath.size());

					CObjectInfo info;
					info.m_nQueryMask = OBJINFO_ICON;
					info.m_strName = strPath;
					
					if( pMenu->m_pRoot->GetObjectInfo(&info) )
					{
						pApp->m_pIcon = info.m_pIcon;
						pApp->m_pFont = pMenu->m_pRoot->GetFont(FONTID_DEFAULT);
						pApp->m_nFlags |= STYLE_DESTROY_ICON;
						pApp->m_nIconAlign = ALIGN_LEFT|ALIGN_VCENTER;
					}

					pAppList->AddItem(pApp);
					setProcesses.push_back(strPath);
				}
			}
			CALL_SYS_ObjectClose(hPtr);
		}
		CALL_SYS_ObjectClose(hOs);
	}

	return true;
}

static bool BrowseApp(CItemBase* pItem, tObjPath &strAppPath)
{
	CItemBase* pOwner = pItem ? pItem : g_pRoot->m_pClicked;
	if( pOwner )
		pOwner = pOwner->GetOwner(true);

	bool bBrowse = true;
	if( g_pRoot->m_pClicked )
	{
		CPopupMenuItem* pMenu = NULL;
		pOwner->LoadItem(NULL, (CItemBase*&)pMenu, "BrowseForAppMenu");
		if( pMenu )
		{
			CItemBase* pSelected = NULL;
			if( FillAppListMenu(pMenu) )
			{
				POINT ptItem; LPPOINT pptItem = NULL;
				if( g_pRoot->m_pClicked->m_strItemType.size() )
				{
					RECT rc; g_pRoot->m_pClicked->GetParentRect(rc, pItem->m_pRoot);
					ptItem.x = rc.left; ptItem.y = rc.bottom;
					pptItem = &ptItem;
				}

				pSelected = pMenu->Track(pptItem);
				bBrowse = pSelected && pSelected->m_strItemId == "Browse";

				if( pSelected && !bBrowse )
					g_pRoot->LocalizeStr(strAppPath, pSelected->m_strText.c_str(), pSelected->m_strText.size());
			}

			pMenu->Destroy();

			if( pSelected && !bBrowse )
				return true;
		}
	}

	if( !bBrowse )
		return false;

	return g_pRoot->BrowseFile(pOwner, g_pRoot->LoadLocString(tString(), "BrowseForExecutable"), "exe", strAppPath);
}

//////////////////////////////////////////////////////////////////////
// CKav6RootSink

SINK_MAP_BEGINEX(CKav6RootSink)
	SINK_DYNAMIC_EX("vector",                        GUI_ITEMT_LISTCTRL, CVectorView())
	SINK_DYNAMIC_EX("vector.ScanObject",             GUI_ITEMT_LISTCTRL, CScanObjectsList(false))
	SINK_DYNAMIC_EX("vector.ScanObject.ex",          GUI_ITEMT_LISTCTRL, CScanObjectsList(true))
	SINK_DYNAMIC_EX("vector.UpdaterSource",          GUI_ITEMT_LISTCTRL, CUpdateSrcList())
	SINK_DYNAMIC_EX("vector.ExceptsTaskList",        GUI_ITEMT_LISTCTRL, CExceptsTasksList())
	SINK_DYNAMIC_EX("vector.HipsResources",          GUI_ITEMT_LISTCTRL, CResourceList())
	SINK_DYNAMIC_EX("vector.Networks",               GUI_ITEMT_LISTCTRL, CNetworkList())
	SINK_DYNAMIC_EX("vector.report.Avz",             GUI_ITEMT_LISTCTRL, CReportView((tTYPE_ID)cDetectObjectInfo::eIID, AVP_PROFILE_AVZ_SCRIPT))
	SINK_DYNAMIC_EX("vector.CmnStatistics",          GUI_ITEMT_LISTCTRL, CCmnStatisticList())
	

	SINK_DYNAMIC_EX("tree",                          GUI_ITEMT_LISTCTRL, CSerTreeView())
	SINK_DYNAMIC_EX("tree.ResGroup",                 GUI_ITEMT_LISTCTRL, CResourceGroupList())
	SINK_DYNAMIC_EX("tree.AppGroups",                GUI_ITEMT_LISTCTRL, CAppGroupRuleList())
	SINK_DYNAMIC_EX("tree.ResRule",                  GUI_ITEMT_LISTCTRL, CResRuleList())
	SINK_DYNAMIC_EX("tree.Notifications",            GUI_ITEMT_LISTCTRL, CNotifyEventsList(pItem))
	SINK_DYNAMIC_EX("tree.NetDetailsList",           GUI_ITEMT_LISTCTRL, CNetDetailsListSink())

	SINK_DYNAMIC_EX("taskthreats",                   GUI_ITEMT_LISTCTRL, CThreatsList())

	SINK_DYNAMIC("descriptor", CListItemDescr())
	SINK_DYNAMIC("settings", CSettingsDlg(pCtx))
	SINK_DYNAMIC("task",     CTaskProfileView(0, pCtx))
	SINK_DYNAMIC("taskview", CTaskProfileView(CTaskProfileView::fStat, pCtx))
	SINK_DYNAMIC("taskedit", CTaskProfileView(CTaskProfileView::fEdit, pCtx))
	SINK_DYNAMIC("taskviewedit", CTaskProfileView(CTaskProfileView::fStat|CTaskProfileView::fEdit, pCtx))

	SINK_DYNAMIC("appprop", CAppDialog(pCtx))

	SINK_DYNAMIC_EX("hexview",           GUI_ITEMT_HEXVIEWER, CHexViewSink())
	SINK_DYNAMIC_EX("hexview.Sniffer",   GUI_ITEMT_HEXVIEWER, CHexSnifferSink())
	SINK_DYNAMIC_EX("statistics",  GUI_ITEMT_CHART, CStatisticsSink())
SINK_MAP_END(TBase)

//////////////////////////////////////////////////////////////////////

tDWORD CKav6RootSink::ExpandEnvironmentInt(LPCSTR strName)
{
	cStringObj strValue = strName;
	ExpandEnvironmentString(strValue);
	return atol(strValue.c_str(cCP_ANSI));
}

cStringObj CKav6RootSink::ExpandEnvironmentString(LPCSTR strName)
{
	cStringObj strValue = strName;
	if( !ExpandEnvironmentString(strValue) )
		strValue.erase();
	return strValue;
}

bool CKav6RootSink::ExpandEnvironmentString(cStringObj& strName)
{
	return g_hGui->Obj()->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strName), 0, 0) == errOK_DECIDED;
}

//////////////////////////////////////////////////////////////////////

class CProfileExistHolder : public CustomHolderData
{
public:
	CProfileExistHolder() : m_exists(0) {}
	
	void Init(CItemPropVals& pParams, CRootItem * pRoot)
	{
		LPCSTR strProfile = pParams.GetStr();
		if( !!g_pProduct && g_pProduct->GetProfile(strProfile) )
			m_exists = 1;
	}

	unsigned m_exists : 1;
};

class CExcludeTasksHolder : public CustomHolderData
{
public:
	void Init(CItemPropVals& pParams, CRootItem * pRoot)
	{
		pRoot->GetKeys(PROFILE_LOCALIZE, pParams.GetStr(0), m_aTypes);
	}

	void Get(CRootItem * pRoot, cStringObjVector& pVect, tString& strResult)
	{
		for(size_t i = 0; i < pVect.size(); i++)
		{
			const cStrObj& strTaskName = pVect[i];

			size_t t;
			for(t = 0; t < m_aTypes.size(); t++)
			{
				if( strTaskName == m_aTypes[t].c_str() )
					break;
			}
			
			if( t == m_aTypes.size() )
				continue;
			
			if( !strResult.empty() )
				strResult += ", ";
			strResult += pRoot->GetString(TOR(tString,()), PROFILE_LOCALIZE, STRID_TASKTYPES, strTaskName.c_str(cCP_ANSI));
		}
	}

	tKeys m_aTypes;
};

bool CKav6RootSink::GetCustomObjectInfo(CObjectInfo& pObject)
{
	cScanObject pScanObject;
	pScanObject.m_nObjType = (enScanObjType)pObject.m_nCustomId;

	tString strIconName;
	Root()->GetFormatedText(strIconName, "switch(ObjectType,\"ScanObjectIcon\")", &pScanObject);
	pObject.m_pIcon = Root()->GetIcon(strIconName.c_str());
	
	Root()->GetFormatedText(pObject.m_strDisplayName, "switch(ObjectType,\"ScanObjectName\")", &pScanObject);
	return true;
}

void CKav6RootSink::GetLocalizationId(tString& strLocId)
{
	strLocId = ExpandEnvironmentString("%Localization%").c_str(cCP_ANSI);
}

void CKav6RootSink::GetHelpStorageId(cStrObj& strHelpStorageId)
{
	strHelpStorageId = ExpandEnvironmentString("%HelpFile%");
}

bool CKav6RootSink::IsTooltipTest()
{
	return !!ExpandEnvironmentInt("%TipTest%");
}

bool CKav6RootSink::IsGuiControllerFull()
{
	return !!ExpandEnvironmentInt("%GuiController%");
}

//////////////////////////////////////////////////////////////////////

bool CKav6RootSink::CryptDecrypt(cStringObj& str, bool bCrypt)
{
	if( str.length() == 0 )
		return true;

	bool bSimple = false;
	cCryptoHelper *hCryptoHelper = GetCryptoHelper(bSimple);
	if( hCryptoHelper == NULL )
    {
        PR_TRACE((g_root, prtERROR, "gui\tGUI::CryptoHelper not created."));
		return false;
    }
	
	tDWORD flags = 0;
	if( !bCrypt )
		flags |= PSWD_DECRYPT;
	if( bSimple )
		flags |= PSWD_SIMPLE;
	return PR_SUCC(hCryptoHelper->CryptPassword(&cSerString(str), flags));
}

bool CKav6RootSink::CheckPasswords(cStringObj& strOldCrypted, cStringObj& strNew)
{
	bool bSimple = false;
	cCryptoHelper* hCryptoHelper = GetCryptoHelper(bSimple);
    if( hCryptoHelper == NULL )
		return false;

	bool bGoodPswrd = false;

	cStringObj strOld = strOldCrypted;
	if( PR_SUCC(hCryptoHelper->CryptPassword(&cSerString(strOld), PSWD_DECRYPT|(bSimple ? PSWD_SIMPLE : 0))) )
		bGoodPswrd = strNew == strOld;
			
	return bGoodPswrd;
}

bool CKav6RootSink::CheckCondition(CItemBase * pParent, LPCSTR strCondition)
{
	cNode *condition = Root()->CreateNode(strCondition, pParent ? pParent->GetBinder() : NULL, pParent);
	if( !condition )
		return false;

	cVariant res;
	condition->Exec(res, cNodeExecCtx(Root()));
	condition->Destroy();

	return res;
}

bool CKav6RootSink::GetAppInfoByPid(tDWORD nPID, CObjectInfo * pObjInfo)
{
	hOS hOs = NULL;
	tERROR err;
	tPROCESS TmProcess;

	if (g_hGui->GetObjectProcess && g_hGui->CreateObject)
	{
		err = g_hGui->GetObjectProcess((hOBJECT)g_hTM, &TmProcess);
		if (PR_SUCC(err))
			err = g_hGui->CreateObject(TmProcess, (hOBJECT)g_root, (hOBJECT *)&hOs, IID_OS, PID_MEMMODSCAN, 0);
	}
	else
	{
		err = errNOT_FOUND;
	}

	tBOOL bIs9x = cFALSE;
#ifdef _WIN32
	bIs9x = (GetVersion() & 0x80000000) != 0;
#endif

	if (PR_FAIL(err))
		err = CALL_SYS_ObjectCreate(g_root, (hOBJECT *)&hOs, IID_OS, PID_MEMMODSCAN, 0);
	if (PR_SUCC(err))
	{
		err = hOs->propSetDWord(plPID, nPID);
		if(PR_SUCC(err))
			err = hOs->sysCreateObjectDone();
		if (PR_SUCC(err))
		{
			// creating enumerator
			hObjPtr hPtr = 0;
			err = hOs->PtrCreate(&hPtr, 0);
			if (PR_SUCC(err))
			{
				cStringObj strPath;
				while(PR_SUCC(hPtr->Next()))
				{
					if(PR_FAIL(err = strPath.assign(hPtr, pgOBJECT_PATH)))
						break;
					if(!bIs9x)
						break;
					// search for .exe under 9x
					tDWORD len = strPath.length();
					if(len > 4 && strPath.compare(len - 4, 4, ".exe", fSTRING_COMPARE_CASE_INSENSITIVE) == cSTRING_COMP_EQ)
						break;
					err = errNOT_FOUND;
				}
				if(PR_SUCC(err))
					pObjInfo->m_strShortName = strPath;
				hPtr->sysCloseObject();
			}
		}
		hOs->sysCloseObject();
	}

	return PR_SUCC(err);
}

void CKav6RootSink::OnCreate()
{
	Item()->AddDataSource(this);
	CTaskProfileView::AddProfileSource(true, Item()->GetBinder(), g_pProduct);
}

bool CKav6RootSink::OnAddDataSection(CFieldsBinder *pBinding, LPCSTR strSect)
{
	if( strSect && *strSect )
		return CTaskProfileView::AddProfileSource(true, pBinding, g_pProduct->GetProfile(strSect), strSect);
	return false;
}

FUNC_NODE(cNodeIsInstalled, 1)
{
	if( !m_args[0] )
		return;

	m_args[0]->Exec(value, ctx);
	value = g_pProduct && g_pProduct->GetProfile(value.c_str());
}

struct cNodeRepDbObject : public cNodeArgs<1>
{							
	cNodeRepDbObject(cNode ** args = NULL, bool bSer = false) : cNodeArgs(args), m_fSer(bSer) {}
	bool IsConst() const { return false; }
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		if( !get || !g_hGui->m_pReport || !m_args[0] ) return;

		m_info.clear();
		cBuff<tBYTE, 0x4000> buf;
		
		if( tQWORD nObjId = FUNC_NODE_PRM(0).ToQWORD() )
		{
			buf.reserve(buf.size(), true);
			tDWORD nObjType = 0;

			tDWORD nSize = 0;
			tERROR err = g_hGui->m_pReport->GetObjectName(nObjId, (tPTR)buf.ptr(), buf.used(), &nSize, 0, &nObjType);
			if( err == errBUFFER_TOO_SMALL )
			{
				buf.reserve(nSize, true);
				if( PR_FAIL(g_hGui->m_pReport->GetObjectName(nObjId, (tPTR)buf.ptr(), buf.used(), &nSize, 0, &nObjType)) )
					buf.m_used = 0;
				else
					buf.m_used = nSize;
			}
			else if( PR_FAIL(err) )
				buf.m_used = 0;
			else
				buf.m_used = nSize;
		}

		if( buf.used() )
		{
			if( m_fSer )
			{
				m_info.deserialize((tBYTE *)buf.ptr(), buf.used());
				value.Init(m_info.ptr_ref());
			}
			else
				value.Init((wchar_t *)buf.ptr(), buf.used()/sizeof(wchar_t));
		}
	}

	cSerializableObj m_info;
	unsigned         m_fSer : 1;
};

FUNC_NODE_CONST(cNodeClearReportDB, 1)
{
	if( !g_hGui->m_pReport )
		return;
	if( !m_args[0] )
		return;
	
	m_args[0]->Exec(value, ctx);

	g_hGui->m_pReport->ClearDB(value.ToDWORD());
}

FUNC_NODE_CONST(cNodePrepareFullScanObjects, 1)
{
	tERROR err = errNOT_OK;
	
	if( CProfile * pFullScan = g_pProduct->GetProfile(AVP_PROFILE_FULLSCAN) )
	{
		cScanObjects * pSett = (cScanObjects *)pFullScan->settings(NULL, cScanObjects::eIID);
		if( pSett )
		{
			// !!! Необходимый список объектов надо синхронизировать с BlImpl::check_my_computer_was_scanned
			
			scanobjarr_t& aObjects = pSett->m_aScanObjects;
			for(size_t i = 0; i < aObjects.size(); i++)
			{
				cScanObject &obj = aObjects[i];
				switch( obj.m_nObjType )
				{
				case OBJECT_TYPE_MY_COMPUTER:
				case OBJECT_TYPE_MEMORY:
				case OBJECT_TYPE_STARTUP:
				case OBJECT_TYPE_MAIL:
				case OBJECT_TYPE_ALL_FIXED_DRIVES:
				case OBJECT_TYPE_ALL_REMOVABLE_DRIVES:
				case OBJECT_TYPE_SYSTEM_RESTORE:
					if( !obj.m_bEnabled )
					{
						obj.m_bEnabled = cTRUE;
						pFullScan->SetSettingsDirty();
					}
					break;
				}
			}
			
			err = pFullScan->StoreSettings();
		}
	}

	value = (tDWORD)err;
}

class cNodeParsePorts : public cNodeArgs<2>
{
public:
	cNodeParsePorts(cNode **args) : cNodeArgs<2>(args) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		m_args[0]->Exec(value, ctx);
		const cVectorData *vd = value.c_vector();
		if( vd && vd->m_type == cFwPort::eIID )
		{
			cVector<cFwPort> &ports = *(cVector<cFwPort> *)vd->m_v;
			
			m_args[1]->Exec(value, ctx);
			CFwRuleUtils::ParsePortListStr(value.c_str(), ports);
		}
	}
};

class cNodeIp : public cNodeArgs<1>
{
public:
	cNodeIp(cNode **args) : cNodeArgs<1>(args) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		tString &str = value.MakeString(); str.clear(); str.resize(50);
		if( const cIP * ip = (cIP *)m_args[0]->ExecGet(ctx).c_ser(cIP::eIID) )
		{
			char *buff = &str.at(0);
			size_t len = str.capacity();
			len = ip->isBasedOn(cIpMask::eIID) ? ((cIpMask *)ip)->ToStr(buff, len) : ip->ToStr(buff, len);
			if( len )
				str.resize(len - 1);
		}
	}
};

class cNodeParseIp : public cNodeArgs<2>
{
public:
	cNodeParseIp(cNode **args) : cNodeArgs<2>(args) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		m_args[0]->Exec(value, ctx);
		if( cIpMask *ip = (cIpMask *)value.c_ser(cIpMask::eIID) )
		{
			m_args[1]->Exec(value, ctx);
			value = ip->FromStr(value.c_str());
		}
	}
};

class cNodeParseAddr : public cNodeArgs<2>
{
public:
	cNodeParseAddr(cNode **args) : cNodeArgs<2>(args) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		m_args[0]->Exec(value, ctx);
		if( cFwAddress *addr = (cFwAddress *)value.c_ser(cFwAddress::eIID) )
		{
			// clear structure
			cSerializableObj newStruct;
			newStruct.init(addr->getIID());
			addr->assign(*newStruct, true);

			m_args[1]->Exec(value, ctx);
			if( addr->m_IP.FromStr(value.c_str()) )
				return;

			// resolve domain name
			value.ToStringObj(addr->m_Address);

			CItemBase *top = ctx.m_item->GetOwner(true);
			value = CResolveIpDlg(addr).DoModal(top, "ResolveIp", DLG_ACTION_CANCEL) == DLG_ACTION_OK;
		}
	}
};

class cNodeBrowseObject : public cNodeArgs<3>
{
public:
	enum eBrowseObjectType {eBrowseObjectAny, eBrowseObjectApp, eBrowseObjectAnyApp, eBrowseObjectFileOpen, eBrowseObjectFileSave};

	cNodeBrowseObject(cNode **args, eBrowseObjectType type = eBrowseObjectAny) : cNodeArgs<3>(args), m_type(type) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);

	eBrowseObjectType m_type;
};

void cNodeBrowseObject::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !get )
		return;

	if( !ctx.m_item )
		return;

	value = cFALSE;

	cNode * pStrRes = m_args[0];
	
	cStrObj str;
	{
		cVariant v; pStrRes->Exec(v, ctx);
		v.ToStringObj(str);
	}
	
	if( m_type == eBrowseObjectApp || m_type == eBrowseObjectAnyApp )
	{
		cNode * pHash = m_args[1];

		if( m_type == eBrowseObjectApp )
		{
			if( ctx.m_item->m_pRoot->m_nGuiFlags & GUIFLAG_ADMIN )
			{
				if( !ctx.m_item->m_pRoot->BrowseFile(ctx.m_item, NULL, "exe", str) )
					return;
			}
			else
			{
				if( !BrowseApp(ctx.m_item, str) )
					return;
			}
		}
		else
		{
			if( str != L"*" )
				str = L"*";
			else
				str.erase();
		}

		pStrRes->Exec(cVariant(str.data()), ctx, false);
				
		if( pHash )
		{
			tQWORD nHash = 0;
			if( !(ctx.m_item->m_pRoot->m_nGuiFlags & GUIFLAG_ADMIN) )
			{
				cProcessMonitor * pPM = NULL;
				if( PR_SUCC(g_hTM->GetService(0, (hOBJECT)g_hTM, AVP_SERVICE_PROCESS_MONITOR, (cObject **)&pPM)) )
				{
					pPM->CalcObjectHash(cAutoString(str), &nHash);
					g_hTM->ReleaseService(0, (hOBJECT)pPM);
				}
			}
			
			pHash->Exec(cVariant(nHash), ctx, false);
		}
		
		// TODO: Надо убрать этот вызов и переделать по другому
		ctx.m_item->SetChangedData();
		if( CItemBase * pOwner = ctx.m_item->GetOwner(true) )
			pOwner->UpdateData(true);
		
		value = cTRUE;
		return;
	}
	
	if( m_type == eBrowseObjectFileOpen || m_type == eBrowseObjectFileSave )
	{
		tString strCaption; FUNC_NODE_PRM(1).ToString(strCaption);
		tString strFilter; FUNC_NODE_PRM(2).ToString(strFilter);
		
		if( !ctx.m_item->m_pRoot->BrowseFile(ctx.m_item, strCaption.c_str(), strFilter.c_str(), str, m_type == eBrowseObjectFileSave) )
			return;
		
		pStrRes->Exec(cVariant(str.data()), ctx, false);
		
		// TODO: Надо убрать этот вызов и переделать по другому
		ctx.m_item->SetChangedData();
		if( CItemBase * pOwner = ctx.m_item->GetOwner(true) )
			pOwner->UpdateData(true);
		
		value = cTRUE;
		return;
	}

	if( !m_args[1] )
		return;
	
	tDWORD nTypeMask = SHELL_OBJTYPE_FOLDER;
	if( m_args[2] )
		nTypeMask = m_args[2]->ExecGet(ctx).ToDWORD();

	cSerializable* pData = NULL; ctx.m_item->GetBinder()->GetDataSource(TOR(CStructId, ()), &pData);
	
	cVar vSect; m_args[1]->Exec(vSect, ctx);
	CBrowseObjectInfo _Obj(vSect.c_str(), nTypeMask, ctx.m_item, pData);
	_Obj.m_strName = str;
	if( !g_pRoot->BrowseObject(_Obj) )
		return;

	pStrRes->Exec(cVariant(_Obj.m_strName.data()), ctx, false);
	
	// TODO: Надо убрать этот вызов и переделать по другому
	ctx.m_item->SetChangedData();
	if( CItemBase * pOwner = ctx.m_item->GetOwner(true) )
		pOwner->UpdateData(true);

	value = cTRUE;
}

//////////////////////////////////////////////////////////////////////

cNode *CKav6RootSink::CreateOperator(const char *name, cNode **args, cAlloc *al)
{
	sswitch(name)
	stcase(isInstalled)		return new cNodeIsInstalled(args); sbreak;
	stcase(object)			return new cNodeRepDbObject(args); sbreak;
	stcase(repser)			return new cNodeRepDbObject(args, true); sbreak;
	stcase(clear_reportDB)	return new cNodeClearReportDB(args); sbreak;
	stcase(parsePorts)		return new cNodeParsePorts(args); sbreak;
	stcase(ip)				return new cNodeIp(args); sbreak;
	stcase(parseAddr)		return new cNodeParseAddr(args); sbreak;
	stcase(parseIp)			return new cNodeParseIp(args); sbreak;
	stcase(browseobject)	return new cNodeBrowseObject(args); sbreak;
	stcase(browseapp)		return new cNodeBrowseObject(args, cNodeBrowseObject::eBrowseObjectApp); sbreak;
	stcase(browseanyapp)	return new cNodeBrowseObject(args, cNodeBrowseObject::eBrowseObjectAnyApp); sbreak;
	stcase(browsefileopen)	return new cNodeBrowseObject(args, cNodeBrowseObject::eBrowseObjectFileOpen); sbreak;
	stcase(browsefilesave)	return new cNodeBrowseObject(args, cNodeBrowseObject::eBrowseObjectFileSave); sbreak;
	stcase(prepareFullScanObjects)	return new cNodePrepareFullScanObjects(args); sbreak;
	send

	return TBase::CreateOperator(name, args, al);
}

//////////////////////////////////////////////////////////////////////
//CStatisticsSink

void CStatisticsSink::OnInitProps(CItemProps& pProps)
{
	tString szReportName = pProps.Get(STR_PID_CHARTREPORT).Get();
	
	if (!szReportName.empty() && g_hTM)
		g_hTM->GetTaskReport(TASKID_TM_ITSELF, szReportName.c_str(), *g_hTM, fACCESS_READ, &m_pReport);
}

void CStatisticsSink::OnDestroy()
{ 
	if (m_pReport)
		m_pReport->sysCloseObject();

	m_pReport = NULL;
}

tERROR CStatisticsSink::GetRecordCount( tDWORD* pdwRecords )
{
	if (!m_pReport)
		return errNOT_OK;

	return m_pReport->GetRecordCount(pdwRecords);
}

tERROR CStatisticsSink::GetRecord( tDWORD* result, tDWORD dwIndex, tVOID* pBuffer, tDWORD dwSize, tDATETIME* pdtTimestamp )
{
	if (!m_pReport)
		return errNOT_OK;

	return m_pReport->GetRecord( result, dwIndex, pBuffer, dwSize, pdtTimestamp );
}




//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
