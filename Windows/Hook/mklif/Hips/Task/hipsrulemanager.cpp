#include "hipsrulemanager.h"
#include <map>
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
CHipsRuleManager::CHipsRuleManager(void)
{
	m_HipsResourceList.clear();
//	m_HipsAppList.clear();
	m_HipsRulesList.clear();
	m_pStandardSettings = 0;
	m_pTask = 0;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
CHipsRuleManager::~CHipsRuleManager(void)
{
	ClearList<SHipsRuleItem>(&m_HipsRulesList);
	ClearList<SHipsAppItem>(&m_HipsAppList);
	ClearList<SHipsResourceItem>(&m_HipsResourceList);


	if (m_pStandardSettings)
		m_pStandardSettings->release();
	m_pStandardSettings = 0;

	//if (m_pHipsSettings)
	//	delete [] m_pHipsSettings;
	//m_pHipsSettings = 0;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::Init(void * l_pTask, CHipsManager * l_pHipsManager)
{
	m_pTask = l_pTask;
	m_pHipsManager = l_pHipsManager;


	//	get settings from xml-file
	tERROR err;
	hREGISTRY pReg = 0;
	err = g_root->sysCreateObject ( reinterpret_cast<hOBJECT*> ( &pReg ), IID_REGISTRY, PID_PROPERTIESMAP, SUBTYPE_ANY );
	if ( PR_SUCC ( err ) && pReg) {
		cStrObj strHipsBaseFile;

		strHipsBaseFile = "%Bases%";
		if (g_root->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strHipsBaseFile), 0, 0) != errOK_DECIDED )
		{
			strHipsBaseFile.erase();
		}

		//ExpandEnvironmentString("%Bases%", strHipsBaseFile);
		strHipsBaseFile.check_last_slash(true);
		strHipsBaseFile += "hipsDB.xml";
		err = pReg->propSetStr ( 0 , pgOBJECT_NAME, strHipsBaseFile.c_str(cCP_ANSI), 0, cCP_ANSI );
	}
	else
	{
		PR_TRACE((this, prtERROR, "CHipsRuleManager::Init g_root->sysCreateObject return %x", err));
	}

	if ( PR_SUCC ( err ) && pReg)
	{
		err = pReg->sysCreateObjectDone();
		if (PR_FAIL(err))
		{
			PR_TRACE((this, prtERROR, "CHipsRuleManager::Init g_root->sysCreateObjectDone return %x", err));
		}
	}

	if ( PR_SUCC ( err ) && pReg)
	{
		err = g_root->RegDeserialize((cSerializable**)&m_pStandardSettings, pReg, 0, CHipsSettings::eIID);
		if (PR_FAIL(err))
		{
			m_pStandardSettings = 0;
			PR_TRACE((this, prtERROR, "CHipsRuleManager::Init g_root->RegDeserialize return %x, pReg=%x", err, pReg));
		}
	}
	if (pReg)
		pReg->sysCloseObject();
	return (PR_SUCC(err) ? TRUE : FALSE);
}
////////////////////////////////////////////////////////////////////////////////
//	free internal lists
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::FreeInternalRules()
{
	ClearList<SHipsResourceItem>(&m_HipsResourceList);
	ClearList<SHipsAppItem>(&m_HipsAppList);
	ClearList<SHipsRuleItem>(&m_HipsRulesList);
	m_ResourceMap.clear();
	return HIPS_EC_OK;
}
////////////////////////////////////////////////////////////////////////////////
//	
////////////////////////////////////////////////////////////////////////////////
bool CHipsRuleManager::IsDirectRuleExist(DWORD l_AppId, DWORD l_ResIdEx)
{
	for (DWORD i = 0; i < m_Settings.m_vRuleList.count(); i++)
	{
		if ((m_Settings.m_vRuleList[i].m_RuleState == CHipsRuleItem::fEnabled) &&
			(m_Settings.m_vRuleList[i].m_AppIdEx == l_AppId) &&
			(m_Settings.m_vRuleList[i].m_ResIdEx == l_ResIdEx))
			return true;
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////
//	generate and fill internal list (for driver) from CHipsSettings
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::ResolveRuleList()
{
	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsRuleManager::ResolveRuleList"));
	//DebugBreak();
	DWORD StsrtTime = GetTickCount();
	FreeInternalRules();
	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsRuleManager::ResolveRuleList:: FreeInternalRules = %d ms",  GetTickCount() - StsrtTime));

	cFileInfo File;
	cRegKeyInfo Reg;
	cSeverity Sever;

	//CHipsResourceItem Res;
	CHipsAppItem App;
	CHipsRuleItem Rule;

	pSHipsResourceItem pRes = 0;
	pSHipsAppItem pApp = 0;
	pSHipsRuleItem pRule = 0;

	BOOL bRes = HIPS_EC_OK;

	StsrtTime = GetTickCount();

	// fill resource list
	cResource * pTempRes = 0;
	cResourceIterator it (&m_Settings.m_Resource);
	do
	{
		pTempRes = it.GetNext();
		if (pTempRes == 0) break;

		if (!pTempRes->m_pData)
		{
			//	skip group (only resource are needed)
			continue;
		}
		switch (pTempRes->m_pData->getIID())
		{
			case cFileInfo::eIID:
			{
				File.assign(*pTempRes->m_pData.ptr_ref(), true);
				AddResourceItem(	pTempRes->m_nID,
									//pTempRes->m_nParentID,
									cResource::ehrtFile,
									(wchar_t*)File.m_Path.data(),//c_str(cCP_UNICODE).ptr()
									0,
									//(wchar_t*)pTempRes->m_sName.data(),
									pTempRes->m_nFlags);// Res.m_Flags
				break;
			}
			case cRegKeyInfo::eIID:
			{
				Reg.assign(*pTempRes->m_pData.ptr_ref(), true);
				AddResourceItem(	pTempRes->m_nID,
									//pTempRes->m_nParentID,
									cResource::ehrtReg,
									(wchar_t*)Reg.m_KeyPath.data(),
									(wchar_t*)Reg.m_KeyValue.data(),
									//(wchar_t*)pTempRes->m_sName.data(),
									pTempRes->m_nFlags);
				break;
			}
			case cDeviceInfo::eIID:
			{
				break;
			}
			case cPrivilegeInfo::eIID:
			{
				break;
			}
			case cSeverity::eIID:
			{
				Sever.assign(*pTempRes->m_pData.ptr_ref(), true);
				AddResourceItem(	pTempRes->m_nID,
									cResource::ehrtSeverity,
									Sever.m_nType );
				break;
			}
		};
	} while (pTempRes);
	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsRuleManager::ResolveRuleList:: m_HipsResourceList size = %d", m_HipsResourceList.size()));
	

	//	fill application list
	for (DWORD i = 0; i < m_Settings.m_vAppList.count(); i++)
	{
		App = m_Settings.m_vAppList[i];
		AddAppItem(App.m_AppId, App.m_AppGrId, (wchar_t*)App.m_AppName.c_str(cCP_UNICODE).ptr(), App.m_AppHash, 0/*mApp.m_AppFlags*/);
	}

	//	fill rules
	for (DWORD i = 0; i < m_Settings.m_vRuleList.count(); i++)
	{
		pApp = 0;
		Rule = m_Settings.m_vRuleList[i];
		PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::ResolveRuleList:: source rule m_RuleId = %x, m_AppIdEx = %x, m_ResIdEx = %x, m_AccessFlag = %x",
			Rule.m_RuleId, Rule.m_AppIdEx, Rule.m_ResIdEx, Rule.m_AccessFlag));

		if (!Rule.m_RuleState)
		{
			//	skip disabled rules
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::ResolveRuleList:: Rule.m_RuleState OFF"));
			continue;
		}
		if ((Rule.m_AccessFlag == 0) &&
			(Rule.m_AppIdEx != 0) &&
			(Rule.m_RuleTypeId != CHipsRuleItem::ehrltAppSeverity))
		{
			//	skip allow rules for app, and no rules for severity
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::ResolveRuleList:: Rule.m_AccessFlag == 0"));
			continue;
		}

		//!!! need resolve system rules (without app)

		//	enum app for this rule
		bRes = GetFirstAppEx(Rule.m_AppIdEx, &pApp);
		if (Rule.m_AppIdEx == 0)
		{
			//	this is global rule (not for app)
			bRes = HIPS_EC_OK;
			pApp = 0;
		}
		if (bRes != HIPS_EC_OK)
		{
			//	there is no app for this rule
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::ResolveRuleList:: GetFirstAppEx return bRes = %x", bRes));
			continue;
		}
		do
		{

			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::ResolveRuleList:: generated rule m_AppId = %x, m_ResIdEx = %x",
				(pApp) ? pApp->f_AppID : 0,
				Rule.m_ResIdEx));

			if (pApp && IsDirectRuleExist(pApp->f_AppID, Rule.m_ResIdEx))
			{
				PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::ResolveRuleList:: IsDirectRuleExist return true"));
				if (Rule.m_AppIdEx != pApp->f_AppID)
				{
					//	there is another rule for this app (more detailed)
					break;
				}
				else
				{
					PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::ResolveRuleList:: but this is a direct rule itself"));
				}
			}
			//	generate rules for this app for all resources
			GenAndAddRulesForAppEx(pApp, &Rule, &m_HipsRulesList);

			bRes = GetNextAppEx(m_Settings.m_vRuleList[i].m_AppIdEx, &pApp);
		} while (bRes == HIPS_EC_OK);
	}
	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsRuleManager::ResolveRuleList:: m_HipsRulesList size = %d, other = %d ms",
		m_HipsRulesList.size(), GetTickCount() - StsrtTime));

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::AddRulesForAppToList(CHipsAppItem * l_pApp, HipsRulesList * l_pHipsRulesList)
{
	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddRulesForAppToList:: start"));
	CHipsRuleItem Rule;
	pSHipsAppItem pApp; 
	pSHipsAppItem pTempApp; 
	BOOL bRes;
	
	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddRulesForAppToList:: before AddAppItem"));
	AddAppItem(l_pApp->m_AppId, l_pApp->m_AppGrId, (wchar_t*)l_pApp->m_AppName.c_str(cCP_UNICODE).ptr(), l_pApp->m_AppHash, 0/*mApp.m_AppFlags*/);
	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddRulesForAppToList:: after AddAppItem"));
	if (!FindIntAppById(l_pApp->m_AppId, &pApp))
	{
		PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddRulesForAppToList:: FindIntAppById FAIL"));
		return FALSE;
	}
	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddRulesForAppToList:: FindIntAppById OK"));
	for (DWORD i = 0; i < m_Settings.m_vRuleList.count(); i++)
	{
		Rule = m_Settings.m_vRuleList[i];
		PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddRulesForAppToList:: source rule m_RuleId = 0x%x, m_AppIdEx = 0x%x, m_ResIdEx = 0x%x, m_AccessFlag = 0x%x",
			Rule.m_RuleId, Rule.m_AppIdEx, Rule.m_ResIdEx, Rule.m_AccessFlag));

		if (!Rule.m_RuleState)
		{
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddRulesForAppToList:: Rule.m_RuleState OFF"));
			continue;
		}
		if ((Rule.m_AccessFlag == 0) &&
			(Rule.m_RuleTypeId != CHipsRuleItem::ehrltAppSeverity))
		{
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddRulesForAppToList:: Rule.m_AccessFlag == 0"));
			continue;
		}

		PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddRulesForAppToList:: try to find app by app(0x%x) in rule",
			Rule.m_AppIdEx));
		
		bRes = GetFirstAppEx(Rule.m_AppIdEx, &pTempApp);
		if (Rule.m_AppIdEx == 0)
		{
			//	this is global rule (not for app)
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddRulesForAppToList:: app == 0, skip this rule"));
			continue;
		}
		if (bRes != HIPS_EC_OK)
		{
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddRulesForAppToList:: GetFirstAppEx return bRes = %x", bRes));
			continue;
		}
		do
		{
			if ((pTempApp) && 
				(pTempApp->f_AppID == pApp->f_AppID)
				)
			{
				if (IsDirectRuleExist(pApp->f_AppID, Rule.m_ResIdEx))
				{
					PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddRulesForAppToList:: IsDirectRuleExist(app 0x%x, res 0x%x) return true",
						pApp->f_AppID, Rule.m_ResIdEx));
					if (Rule.m_AppIdEx != pApp->f_AppID)
					{
						PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddRulesForAppToList:: threre is a direct rule for (app 0x%x, res 0x%x) return true",
							pApp->f_AppID, Rule.m_ResIdEx));
						continue;
					}
					else
					{
						PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddRulesForAppToList:: this is a direct rule itself(app 0x%x, res 0x%x) return true",
							pApp->f_AppID, Rule.m_ResIdEx));
					}
				}
				PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddRulesForAppToList:: generated rule m_AppId = 0x%x, m_ResIdEx = 0x%x",
					pApp->f_AppID, Rule.m_ResIdEx));
				GenAndAddRulesForAppEx(pApp, &Rule, l_pHipsRulesList, m_HipsRulesList.size());
			}
			bRes = GetNextAppEx(Rule.m_AppIdEx, &pTempApp);
		} while (bRes == HIPS_EC_OK);
	}
	PR_TRACE((g_root, prtIMPORTANT, TR "CHipsRuleManager::AddRulesForAppToList:: end"));
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::GenAndAddRulesForAppEx(pSHipsAppItem l_pApp, CHipsRuleItem * l_pRule, HipsRulesList * l_pHipsRulesList, DWORD l_StartOffset)
{
	pSHipsResourceItem pRes = 0;
	pSHipsRuleItem pRule = 0;
	BOOL bRes = HIPS_EC_OK;

	if (l_pApp != 0)
	{
		//	old rules (with app)
		if (HIPS_EC_OK != (bRes = GetFirstResEx((l_pApp) ? l_pApp->f_AppID : 0, l_pRule->m_ResIdEx, &pRes)))
		{
			//	nested resource was not found
			//	try to find direct resource (may be it`s a direct rule)
			cResource * pTempRes = m_Settings.m_Resource.Find(l_pRule->m_ResIdEx);
			if (pTempRes && pTempRes->m_pData)
			{
				std::map <DWORD, pSHipsResourceItem> :: const_iterator MapIterator;
				MapIterator = m_ResourceMap.find(pTempRes->m_nID);
				if (MapIterator != m_ResourceMap.end())
					pRes = MapIterator->second;
				else
					return bRes;
			}
			else
				return bRes;
		}
		do
		{
			//	app rule
			pRule = new SHipsRuleItem();
			pRule->f_RuleID = l_StartOffset + l_pHipsRulesList->size();
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::ResolveRuleList:: generated rule l_pApp->f_AppID = %x, l_pApp->f_AppGrID = %x, m_ResId = %x, m_AccessFlag = %d, f_RuleID = %d",
				(l_pApp) ? l_pApp->f_AppID : 0,
				(l_pApp) ? l_pApp->f_AppGrID : 0,
				pRes->f_ResID,
				l_pRule->m_AccessFlag,
				pRule->f_RuleID));

			pRule->f_AccessFlag = l_pRule->m_AccessFlag;
			pRule->f_RuleGrID = l_pRule->m_RuleTypeId;
			pRule->f_State = (EnumHipsRuleItemState)l_pRule->m_RuleState;

			pRule->f_ResID = pRes->f_ResID;
			pRule->f_ResType = pRes->f_ResType;
			pRule->f_pResValue = pRes->f_ResValue;
			pRule->f_pResName = pRes->f_ResName;
			pRule->f_pResNativeName = pRes->f_ResNativeName;
			pRule->f_Severity = pRes->f_Severity;
			//pRule->f_pResDesc = pRes->f_ResDesc;

			if (l_pApp)
			{
				pRule->f_pAppName = l_pApp->f_AppName;
				pRule->f_pAppNativeName = l_pApp->f_AppNativeName;
				pRule->f_pAppHash = l_pApp->f_AppHash;
			}

			l_pHipsRulesList->push_back(pRule);

			bRes = GetNextResEx((l_pApp) ? l_pApp->f_AppID : 0, l_pRule->m_ResIdEx, &pRes);
		} while (bRes == HIPS_EC_OK);
	}
	else
	{
		//	new rules
		DWORD ResId = l_pRule->m_ResIdEx;//HIPS_GET_ID_FROM_EX(l_pRule->m_ResIdEx);
		cResource * pRes = m_Settings.m_Resource.Find(ResId);//FindResByID(0, ResId);
		if (pRes)
		{
			MakeDevRuleRule(pRes, l_pRule->m_AccessFlag, l_StartOffset, l_pHipsRulesList);

		}
		//l_pRule->m_ResIdEx
	}
	return bRes;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void CHipsRuleManager::MakeDevRuleRule(cResource * l_pResource, DWORD l_AccessFlag, DWORD l_StartOffset, HipsRulesList * l_pHipsRulesList)
{
	if (l_pResource == 0)
		return;
	cDeviceInfo Dev;
	pSHipsRuleItem pRule = 0;

	wchar_t * TempStr = 0;

	if (l_pResource->m_pData)
	{
		Dev.assign(*(l_pResource->m_pData.ptr_ref()), true);

		PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::ResolveRuleList:: generated system rule, m_ResId = %x, m_AccessFlag = %d, GUID = %S", //, f_RuleID = %d
			l_pResource->m_nID,
			l_AccessFlag,
			(wchar_t*)Dev.m_sClassGUID.c_str(cCP_UNICODE).ptr()
			/*pRule->f_RuleID*/));

		pRule = new SHipsRuleItem();
		pRule->f_RuleID = l_StartOffset + l_pHipsRulesList->size();
		pRule->f_AccessFlag = l_AccessFlag;
		pRule->f_State = hrisEnable;

		pRule->f_ResID = l_pResource->m_nID;
		pRule->f_ResType = HIPS_GET_RES_TYPE(l_pResource->m_nFlags);

		TempStr = (wchar_t*)Dev.m_sClassGUID.c_str(cCP_UNICODE).ptr();
		StrAssign(TempStr, &pRule->f_DevGuid);

		TempStr = (wchar_t*)Dev.m_sTypeName.c_str(cCP_UNICODE).ptr();
		StrAssign(TempStr, &pRule->f_DevType);

		TempStr = (wchar_t*)l_pResource->m_sName.c_str(cCP_UNICODE).ptr();
		StrAssign(TempStr, &pRule->f_DevDesc);

		l_pHipsRulesList->push_back(pRule);

	}
	for (DWORD i = 0; i < l_pResource->m_vChilds.size(); i++)
	{
		MakeDevRuleRule(&l_pResource->m_vChilds[i], l_AccessFlag, l_StartOffset, l_pHipsRulesList);
	}
	return;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::GetFirstResEx(DWORD l_AppIdEx, DWORD l_ResIdEx, pSHipsResourceItem * l_ppResItem)
{
	//!!!
	//m_HipsResourceListIt = m_HipsResourceList.begin();
	cResource * pRes = m_Settings.m_Resource.Find(l_ResIdEx);
	if (pRes == 0)
		return HIPS_EC_NO_MORE_ELEMENTS;
	m_ResourceIterator.ReInit(pRes);
	return GetResEx(l_AppIdEx, l_ResIdEx, l_ppResItem);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::GetNextResEx(DWORD l_AppIdEx, DWORD l_ResIdEx, pSHipsResourceItem * l_ppResItem)
{
	//if (m_HipsResourceListIt == m_HipsResourceList.end())
	//{
	//	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::GetNextResEx m_HipsResourceListIt == m_HipsResourceList.end()"));
	//	return HIPS_EC_NO_MORE_ELEMENTS;
	//}
	//m_HipsResourceListIt++;
	return GetResEx(l_AppIdEx, l_ResIdEx, l_ppResItem);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::GetResEx(DWORD l_AppIdEx, DWORD l_ResIdEx, pSHipsResourceItem * l_ppResItem)
{
	//!!!
	if (l_ppResItem == 0)
	{
		return HIPS_EC_INVALID_ARG;
	}

	cResource * pTempRes = 0;
	std::map <DWORD, pSHipsResourceItem> :: const_iterator MapIterator;
	//pSHipsResourceItem pItem = 0;
	bool IsNeedToSkip = false;
	do
	{
		pTempRes = m_ResourceIterator.GetNext(IsNeedToSkip);
		if (pTempRes == 0) break;

		IsNeedToSkip = IsDirectRuleExist(l_AppIdEx, pTempRes->m_nID);

		//??? need add flags check (disabled resources)
		if (!IsNeedToSkip && pTempRes->m_pData)
		{
			MapIterator = m_ResourceMap.find(pTempRes->m_nID);
			if (MapIterator != m_ResourceMap.end())
			{
				*l_ppResItem = MapIterator->second;
				return HIPS_EC_OK;
			}
		}
		if (IsNeedToSkip && !pTempRes->m_vChilds.size())
			IsNeedToSkip = false;
			
	} while (pTempRes);
	return HIPS_EC_NO_MORE_ELEMENTS;

	//DWORD * pGrIdList = 0;
	//DWORD GrIdListCount = 0;
	//DWORD IdEx;
	//DWORD Id;

	//bool IsGroup = l_ResIdEx >> 31;
	//DWORD ResId = (IsGroup) ? (l_ResIdEx - (1<<31)) : l_ResIdEx;


	//if (m_HipsResourceListIt == m_HipsResourceList.end())
	//{
	//	return HIPS_EC_NO_MORE_ELEMENTS;
	//}

	//	get list of all child groups
	//if (IsGroup)
	//	GetChildGrIdList(&m_Settings.m_ResGroups, ResId, &pGrIdList, &GrIdListCount);

	//do {
	//	if (IsGroup)
	//	{
	//		if (((*m_HipsResourceListIt)->f_ResGrID == ResId) &&
	//			((*m_HipsResourceListIt)->f_Flags != CHipsResourceItem::fDisabled))
	//		{
	//			*l_ppResItem = *m_HipsResourceListIt;
	//			if (pGrIdList)
	//				delete [] pGrIdList;
	//			pGrIdList = 0;
	//			return HIPS_EC_OK;
	//		}
	//		if (GrIdListCount > 0 && pGrIdList)
	//		{
	//			for (DWORD i = 0; i < GrIdListCount; i++)
	//			{
	//				Id = pGrIdList[i];
	//				IdEx = HIPS_MAKE_ID_EX(Id, 1);
	//				if (IsDirectRuleExist(l_AppIdEx, IdEx))
	//				{
	//					//PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::ResolveRuleList:: IsDirectRuleExist return true"));
	//					continue;
	//				}
	//				if (((pGrIdList[i] == (*m_HipsResourceListIt)->f_ResGrID) &&
	//					(*m_HipsResourceListIt)->f_Flags != CHipsResourceItem::fDisabled))
	//				{
	//					*l_ppResItem = *m_HipsResourceListIt;
	//					if (pGrIdList)
	//						delete [] pGrIdList;
	//					pGrIdList = 0;
	//					return HIPS_EC_OK;
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{
	//		if (((*m_HipsResourceListIt)->f_ResID == ResId) &&
	//			(*m_HipsResourceListIt)->f_Flags != CHipsResourceItem::fDisabled)
	//		{
	//			*l_ppResItem = *m_HipsResourceListIt;
	//			return HIPS_EC_OK;
	//		}
	//	}
	//	m_HipsResourceListIt++;
	//} while (m_HipsResourceListIt != m_HipsResourceList.end());

	//if (pGrIdList)
	//	delete [] pGrIdList;
	//pGrIdList = 0;
	//return HIPS_EC_NO_MORE_ELEMENTS;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void AddGrIdToList(DWORD l_GrId, DWORD ** l_ppGrIdList, DWORD * l_pGrCount)
{
	if (l_pGrCount == 0 || l_ppGrIdList == 0)
		return;
	DWORD * pTemp = new DWORD [(*l_pGrCount) + 1];
	if (*l_pGrCount > 0)
		memcpy(pTemp, *l_ppGrIdList, (*l_pGrCount)*sizeof(DWORD));
	memcpy(pTemp+(*l_pGrCount), &l_GrId, sizeof(DWORD));
	if (*l_pGrCount > 0)
		delete [] (*l_ppGrIdList);
	*l_ppGrIdList = pTemp;
	(*l_pGrCount)++;
	return;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
cSerializable * CHipsRuleManager::GetChildItem(cSerializable * pGroup, tDWORD ItemNum)
{
	//!!!
/*	if (pGroup->isBasedOn(CHipsResourceGroupsItem::eIID))
	{
		return (cSerializable*)&(((CHipsResourceGroupsItem*)pGroup)->m_vChildResGroupList[ItemNum]);
	}
	else */
	if (pGroup->isBasedOn(cAppGroup::eIID))
	{
		return (cSerializable*)&(((cAppGroup*)pGroup)->m_aChilds[ItemNum]);
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::GetChildGrIdList(cSerializable * pGroup, DWORD l_ParentGrId, DWORD ** l_ppGrIdList, DWORD * l_pGrCount, bool IsNeedToAdd)
{
	if (pGroup == 0 || l_ppGrIdList == 0 || l_pGrCount == 0)
		return FALSE;

	tDWORD GrId = 0;

	//!!!
	//if (pGroup->isBasedOn(CHipsResourceGroupsItem::eIID))
	//{
	//	GrId = ((CHipsResourceGroupsItem*)pGroup)->m_ResGroupID;
	//}
	//else 
	if (pGroup->isBasedOn(cAppGroup::eIID))
	{
		GrId = ((cAppGroup*)pGroup)->m_nAppGrpID;
	}
	if (GrId == 0)
		return FALSE;

	tDWORD Size = 0;
	//!!!
	//if (pGroup->isBasedOn(CHipsResourceGroupsItem::eIID))
	//{
	//	Size = ((CHipsResourceGroupsItem*)pGroup)->m_vChildResGroupList.size();
	//}
	//else 
	if (pGroup->isBasedOn(cAppGroup::eIID))
	{
		Size = ((cAppGroup*)pGroup)->m_aChilds.size();
	}
	if (GrId == l_ParentGrId || IsNeedToAdd)
	{
		//	this is parent
		//	need add all childs group
		if (IsNeedToAdd)
		{
			AddGrIdToList(GrId, l_ppGrIdList, l_pGrCount);
			for (tDWORD i = 0; i < Size; i++)
			{
				GetChildGrIdList(GetChildItem(pGroup, i), l_ParentGrId, l_ppGrIdList, l_pGrCount, true);
			}
		}
		else
		{
			//	add root
			AddGrIdToList(GrId, l_ppGrIdList, l_pGrCount);
			for (tDWORD i = 0; i < Size; i++)
			{
				GetChildGrIdList(GetChildItem(pGroup, i), l_ParentGrId, l_ppGrIdList, l_pGrCount, true);
			}
		}
	}
	else
	{
		//
		for (tDWORD i = 0; i < Size; i++)
		{
			GetChildGrIdList(GetChildItem(pGroup, i), l_ParentGrId, l_ppGrIdList, l_pGrCount);
		}
	}
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::GetFirstAppEx(DWORD l_AppIdEx, pSHipsAppItem * l_ppAppItem)
{
	m_HipsAppListIt = m_HipsAppList.begin();
	return GetAppEx(l_AppIdEx, l_ppAppItem);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::GetNextAppEx(DWORD l_AppIdEx, pSHipsAppItem * l_ppAppItem)
{
	if (m_HipsAppListIt == m_HipsAppList.end())
	{
		PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::GetNextAppEx m_HipsAppListIt == m_HipsAppList.end()"));
		return HIPS_EC_NO_MORE_ELEMENTS;
	}
	m_HipsAppListIt++;
	return GetAppEx(l_AppIdEx, l_ppAppItem);
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::GetAppEx(DWORD l_AppIdEx, pSHipsAppItem * l_ppAppItem)
{
	if (l_ppAppItem == 0)
	{
		return HIPS_EC_INVALID_ARG;
	}
	bool IsGroup = HIPS_IS_GR_ID_EX(l_AppIdEx);// l_AppIdEx >> 31;
	DWORD AppId = HIPS_GET_ID_FROM_EX(l_AppIdEx);//(IsGroup) ? (l_AppIdEx - (1<<31)) : l_AppIdEx;
	if (m_HipsAppListIt == m_HipsAppList.end())
	{
		PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::GetAppEx m_HipsAppListIt == m_HipsAppList.end()"));
		return HIPS_EC_NO_MORE_ELEMENTS;
	}

	DWORD * pGrIdList = 0;
	DWORD GrIdListCount = 0;
	if (IsGroup)
		GetChildGrIdList(&m_Settings.m_AppGroups, AppId, &pGrIdList, &GrIdListCount);

	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::GetAppEx IsAppGroup=%d, AppId=%x",
		IsGroup, AppId));
	do {
		if (IsGroup)
		{
			if ((*m_HipsAppListIt)->f_AppGrID == AppId)
			{
				*l_ppAppItem = *m_HipsAppListIt;
				return HIPS_EC_OK;
			}
			if (GrIdListCount > 0 && pGrIdList)
			{
				for (DWORD i = 0; i < GrIdListCount; i++)
				{
					if (pGrIdList[i] == (*m_HipsAppListIt)->f_AppGrID)
					{
						*l_ppAppItem = *m_HipsAppListIt;
						if (pGrIdList)
							delete [] pGrIdList;
						pGrIdList = 0;
						return HIPS_EC_OK;
					}
				}
			}
		}
		else
		{
			if ((*m_HipsAppListIt)->f_AppID == AppId)
			{
				*l_ppAppItem = *m_HipsAppListIt;
				return HIPS_EC_OK;
			}
		}
		m_HipsAppListIt++;
	} while (m_HipsAppListIt != m_HipsAppList.end());

	return HIPS_EC_NO_MORE_ELEMENTS;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::FindIntAppById(DWORD l_AppId, pSHipsAppItem * l_ppAppItem)
{
	HipsAppListIt It = m_HipsAppList.begin();
	while (It != m_HipsAppList.end())
	{
		if ((*It)->f_AppID == l_AppId)
		{
			*l_ppAppItem = *It;
			return TRUE;
		}
		It++;
	};
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
template <class S>
void CHipsRuleManager::ClearList(std::list<S*> * list)
{
	S * pItem = 0;
	std::list<S*>::iterator ItemIt;
	while ((ItemIt = list->begin()) != list->end()) {
		pItem = *ItemIt;
		if (pItem) {
			delete pItem;
			pItem = 0;
		}
		list->erase(ItemIt);
	}
	list->clear();
	return;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::AddResourceItem(DWORD l_ResID, /*DWORD l_ResGtID, */DWORD l_ResType, wchar_t * l_ResName, wchar_t * l_ResValue, /*wchar_t * l_ResDesc, */DWORD l_Flags)
{
	if (m_pTask == 0)
	{
		PR_TRACE((g_root, prtERROR, TR "CHipsRuleManager::AddResourceItem:: m_pTask == 0, return FALSE"));
		return FALSE;
	}
	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddResourceItem:: (l_ResName = %S)", l_ResName));
	BOOL bRes = TRUE;
	pSHipsResourceItem pResItem = 0;
	wchar_t * pNativeResName = 0;

	switch (l_ResType)
	{
		case cResource::ehrtFile:
			{
				if (!((CHipsTask*)m_pTask)->ConvertFilePathToNative(l_ResName, &pNativeResName))
				{
					PR_TRACE((g_root, prtERROR, TR "CHipsRuleManager::AddResourceItem:: ConvertFilePathToNative fail, skip this resource !!!"));
					bRes = FALSE;
				}
				else
				{
					PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddResourceItem:: ConvertFilePathToNative (pNativeResName = %S)", pNativeResName));
				}
				break;
			}
		case cResource::ehrtReg:
			{
				if (!((CHipsTask*)m_pTask)->UserRegPath2Kernel(l_ResName, &pNativeResName))
				{
					bRes = FALSE;
				}
				else
				{
					PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::AddResourceItem:: ConvertFilePathToNative (pNativeResName = %S)", pNativeResName));
				}
				break;
			}
	}
	if (bRes == TRUE)
	{
		pResItem = new SHipsResourceItem(l_ResID, /*l_ResGtID, */l_ResType, l_ResName, pNativeResName, l_ResValue, /*l_ResDesc, */l_Flags);
		m_HipsResourceList.push_back(pResItem);

		m_ResourceMap.insert(tResourcePair(l_ResID, m_HipsResourceList.back()));

		if (bRes != HIPS_EC_OK)
		{
			PR_TRACE((g_root, prtIMPORTANT, TR "HIPS:: AddResourceItem (%S) FAIL (%d)",
				l_ResName,
				bRes));
		}
	}
	if (pNativeResName)
		delete [] pNativeResName;
	pNativeResName = 0;
	return bRes;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::AddResourceItem(DWORD l_ResID, DWORD l_ResType, DWORD l_Severity)
{
	pSHipsResourceItem pResItem = 0;
	pResItem = new SHipsResourceItem(l_ResID, l_ResType, l_Severity);
	m_HipsResourceList.push_back(pResItem);
	m_ResourceMap.insert(tResourcePair(l_ResID, m_HipsResourceList.back()));
	return HIPS_EC_OK;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::AddAppItem(DWORD l_AppID, DWORD l_AppGrID, wchar_t * l_AppName, BYTE * l_pAppHash, DWORD l_AppFlags)
{
	if (m_pTask == 0)
	{
		PR_TRACE((g_root, prtERROR, TR "CHipsRuleManager::AddAppItem:: m_pTask == 0, return false"));
		return FALSE;
	}
	//PR_TRACE((g_root, prtIMPORTANT, TR "CHipsRuleManager::AddAppItem:: start (l_AppID = %x, l_AppGrID = %x, l_AppName = %S)",
	//	l_AppID, l_AppGrID, l_AppName));
	BOOL bRes = TRUE;
	pSHipsAppItem pAppItem = 0;
	wchar_t * pNativeAppName = 0;
	if (((CHipsTask*)m_pTask)->ConvertFilePathToNative(l_AppName, &pNativeAppName))
	{
		//PR_TRACE((g_root, prtIMPORTANT, TR "CHipsRuleManager::AddAppItem:: ConvertFilePathToNative OK (pNativeAppName = %S)", pNativeAppName));
		pAppItem = new SHipsAppItem(l_AppID, l_AppGrID, l_AppName, pNativeAppName, l_pAppHash, l_AppFlags);
		if (pAppItem)
			m_HipsAppList.push_back(pAppItem);
		else
		{
			bRes = HIPS_EC_OUT_OF_MEMORY;
			PR_TRACE((g_root, prtERROR, TR "CHipsRuleManager::AddAppItem:: new SHipsAppItem FAIL"));
		}
	}
	else
	{
		PR_TRACE((g_root, prtERROR, TR "CHipsRuleManager::AddAppItem:: ConvertFilePathToNative fail, skip this app !!!"));
		bRes = FALSE;
	}
	if (pNativeAppName)
		delete [] pNativeAppName;
	pNativeAppName = 0;
	return bRes;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
DWORD CHipsRuleManager::GetResIDByDrvRuleID(DWORD l_DrvRuleID)
{
	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::GetResIDByDrvRuleID(l_DrvRuleID = %d, m_HipsRulesList.size() = %d)",
		l_DrvRuleID, m_HipsRulesList.size()));
	if (l_DrvRuleID == 0)
		return 0;
	for (HipsRulesListIt It = m_HipsRulesList.begin();
							It != m_HipsRulesList.end();
							It++)
	{
		//PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::GetResTypeByRuleID ((*It)->f_DrvRuleID = %d)", (*It) ? (*It)->f_DrvRuleID : 0));
		if ((*It) && ((*It)->f_DrvRuleID == l_DrvRuleID)) {
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::GetResTypeByRuleID rule finded ((*It)->f_ResID = %d)", (*It)->f_ResID));
			return (*It)->f_ResID;
		}
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
DWORD CHipsRuleManager::GetResTypeByRuleID(DWORD l_RuleID, DWORD * l_pOutResId)
{
	PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::GetResTypeByRuleID(l_RuleID = %d, l_pOutResId = %x, m_HipsRulesList.size() = %d)",
		l_RuleID, l_pOutResId, m_HipsRulesList.size()));
	if (l_RuleID == 0)
		return cResource::ehrtUnknown;
	for (HipsRulesListIt It = m_HipsRulesList.begin();
							It != m_HipsRulesList.end();
							It++)
	{
		//PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::GetResTypeByRuleID ((*It)->f_DrvRuleID = %d)", (*It) ? (*It)->f_DrvRuleID : 0));
		if (*It && ((*It)->f_DrvRuleID == l_RuleID)) {
			PR_TRACE((g_root, prtNOT_IMPORTANT, TR "CHipsRuleManager::GetResTypeByRuleID rule finded ((*It)->f_ResID = %d)", (*It)->f_ResID));
			if (l_pOutResId) *l_pOutResId = (*It)->f_ResID;
			return (*It)->f_ResType;
		}
	}
	return cResource::ehrtUnknown;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//bool CHipsRuleManager::GetResDescByID(DWORD l_ResID, wchar_t ** l_ppResDesc, DWORD * l_pOutResGrId)
//{
//	if (l_ResID == 0)
//		return false;
//
//	for (HipsResourceListIt It = m_HipsResourceList.begin();
//							It != m_HipsResourceList.end();
//							It++) {
//		if (*It && ((*It)->f_ResID == l_ResID)) {
//			if (l_ppResDesc && (*It)->f_ResDesc )
//			{
//				DWORD size = wcslen((*It)->f_ResDesc) + 1;
//				*l_ppResDesc = new wchar_t[size];
//				wcsncpy(*l_ppResDesc, (*It)->f_ResDesc, size);
//			}
//			if (l_pOutResGrId)
//			{
//				*l_pOutResGrId = (*It)->f_ResGrID;
//			}
//			return true;
//		}
//	}
//	return true;
//}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::GetFirstRule(pSHipsRuleItem * l_ppRule, HipsRulesList * l_pHipsRulesList)
{
	if (l_ppRule == 0) {
		PR_TRACE((g_root, prtERROR, TR "HIPS:: GetFirstRule FAIL (l_ppRule == 0)"));
		return HIPS_EC_INVALID_ARG;
	}
	m_HipsRulesListIt = l_pHipsRulesList->begin();
	if (m_HipsRulesListIt == l_pHipsRulesList->end()) {
		PR_TRACE((g_root, prtERROR, TR "HIPS:: GetFirstRule FAIL (m_HipsRulesList empty)"));
		return HIPS_EC_NO_MORE_ELEMENTS;
	}
	*l_ppRule = *m_HipsRulesListIt;
	//PR_TRACE((g_root, prtERROR, TR "HIPS:: GetFirstRule *l_ppRule = %x",
	//	*l_ppRule));
	if (*l_ppRule) 
	{
		PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS:: GetFirstRule \r\n f_pAppName = %S, f_pResName = %S",
			(*l_ppRule)->f_pAppName,
			(*l_ppRule)->f_pResName));
	}
	return HIPS_EC_OK;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::GetNextRule(pSHipsRuleItem * l_ppRule, HipsRulesList * l_pHipsRulesList)
{
	if (l_ppRule == 0) {
		PR_TRACE((g_root, prtERROR, TR "HIPS:: GetNextRule FAIL (l_ppRule == 0)"));
		return HIPS_EC_INVALID_ARG;
	}
	m_HipsRulesListIt++;
	if (m_HipsRulesListIt == l_pHipsRulesList->end()) {
		PR_TRACE((g_root, prtERROR, TR "HIPS:: GetNextRule return FALSE (no more elements)"));
		return HIPS_EC_NO_MORE_ELEMENTS;
	}
	*l_ppRule = *m_HipsRulesListIt;
	//PR_TRACE((g_root, prtERROR, TR "HIPS:: GetNextRule *l_ppRule = %x",
	//	*l_ppRule));
	if (*l_ppRule) 
	{
		PR_TRACE((g_root, prtNOT_IMPORTANT, TR "HIPS:: GetNextRule \r\n f_pAppName = %S, f_pResName = %S",
			(*l_ppRule)->f_pAppName,
			(*l_ppRule)->f_pResName));
	}
	return HIPS_EC_OK;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void FillAppGrList(cVector<cAppGroup> * pVec, cAppGroup Group)
{
	if (pVec == 0)
		return;
	pVec->push_back(Group);
	for (DWORD i = 0; i < Group.m_aChilds.size(); i++)
	{
		FillAppGrList(pVec, Group.m_aChilds[i]);
	}
	return;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
//!!!
//void FillResGrList(cVector<CHipsResourceGroupsItem> * pVec, CHipsResourceGroupsItem Group)
//{
//	if (pVec == 0)
//		return;
//	pVec->push_back(Group);
//	for (DWORD i = 0; i < Group.m_vChildResGroupList.size(); i++)
//	{
//		FillResGrList(pVec, Group.m_vChildResGroupList[i]);
//	}
//	return;
//}
////////////////////////////////////////////////////////////////////////////////
//	find app in m_Settings.m_vAppList
////////////////////////////////////////////////////////////////////////////////
tERROR CHipsRuleManager::FindAppByHash(BYTE * l_pHash, DWORD l_HashSize, CHipsAppItem ** ppApp)
{
	if (ppApp == 0)
		return errNOT_OK;
	for (DWORD i = 0; i < m_Settings.m_vAppList.size(); i++)
	{
		if (0 == memcmp(&m_Settings.m_vAppList[i].m_AppHash[0], l_pHash, l_HashSize))
		{
			*ppApp = &m_Settings.m_vAppList[i];
			return errOK;
		}
	}
	return errNOT_FOUND;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
void CHipsRuleManager::CollectAppGrIDByServInfo(cAppGroup & Item, cAppGroup & ParentItem, void * pContext)
{
	HipsAppGrFindMask * Mask = (HipsAppGrFindMask*)pContext;
	if ((Mask->pFindedGr == 0) &&
		(Item.m_nServStrType == Mask->ServStrType) &&
		(0 == wcscmp(Item.m_sServStr.c_str(cCP_UNICODE), Mask->ServStr)))
	{
		Mask->pFindedGr = &Item; 
		Mask->pFindedParentGr = &ParentItem;
	}
	return;
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
tERROR CHipsRuleManager::FindAppGrByServInfo(wchar_t * l_ServStr, DWORD ServType, DWORD BaseType, cAppGroup ** ppAppGr, cAppGroup ** ppParentAppGr)
{
	if (ppAppGr == 0)
	{
		PR_TRACE((g_root, prtERROR, TR"CHipsRuleManager::FindAppGrByServInfo:: ppAppGr == 0, return errNOT_OK"));
		return errNOT_OK;
	}
	if (m_pStandardSettings == 0)
	{
		PR_TRACE((g_root, prtERROR, TR"CHipsRuleManager::FindAppGrByServInfo:: m_pStandardSettings == 0, return errNOT_OK"));
		return errNOT_OK;
	}
	HipsAppGrFindMask Mask;
	Mask.ServStr = l_ServStr;
	Mask.ServStrType = ServType;
	//	find group in standard base (current settings already have no empty known goups)
	switch (BaseType)
	{
		case HIPS_BASE_TYPE_STANDARD: 
		{
			m_Settings.EnumAppGroups(
				m_pStandardSettings->m_AppGroups,
				m_pStandardSettings->m_AppGroups,
				&CHipsRuleManager::CollectAppGrIDByServInfo, (void*)&Mask);
			break;
		}
		case HIPS_BASE_TYPE_CURRENT: 
		{
			m_Settings.EnumAppGroups(
				m_Settings.m_AppGroups,
				m_Settings.m_AppGroups,
				&CHipsRuleManager::CollectAppGrIDByServInfo, (void*)&Mask);
			break;
		}
	};

	if (Mask.pFindedGr)
	{
		*ppAppGr = Mask.pFindedGr;
		if (ppParentAppGr && Mask.pFindedParentGr)
			*ppParentAppGr = Mask.pFindedParentGr;
		return errOK;
	}
	return errNOT_FOUND;	
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
CHipsAppItem * CHipsRuleManager::AddApp(wchar_t * l_ProcName, BYTE * l_pHash, DWORD l_HashSize, cAppGroup & Gr, DWORD l_ParentGrId)
{
	if (l_ParentGrId != 0)
	{
		//	need add gr
		cAppGroup * ppParentGr = 0;
		if (PR_SUCC(m_Settings.GetAppGrById(
						m_Settings.m_AppGroups,
						l_ParentGrId,
						&ppParentGr, 0)) && ppParentGr)
		{
			ppParentGr->m_aChilds.push_back(Gr);
		}
	}
	CHipsAppItem App;
	memcpy(App.m_AppHash, l_pHash, l_HashSize);
	App.m_AppId = m_Settings.GetUniqueAppID();
	App.m_AppGrId = Gr.m_nAppGrpID;// l_GrId;
	App.m_AppName = l_ProcName;
	m_Settings.m_vAppList.push_back(App);
	return &m_Settings.m_vAppList[m_Settings.m_vAppList.size()-1];
}
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
BOOL CHipsRuleManager::TestFunc()
{

	//DWORD Start = GetTickCount();
	//m_Settings.MakeConsistent();
	//PR_TRACE((g_root, prtIMPORTANT, TR"CHipsRuleManager::TestFunc:: MakeConsistent time = %d", GetTickCount() - Start));

//	m_Settings.SetState(5, 1, 3, 1, HIPS_RULE_STATE_DENY);

//	tERROR err = ((CHipsTask*)m_pTask)->sysSendMsg(pmc_TASK_SETTINGS_CHANGED, 0, NULL, NULL, NULL);

	//cVector<cAppGroup> AppGrVec;
	//FillAppGrList(&AppGrVec, m_Settings.m_AppGroups);

	//cVector<CHipsResourceGroupsItem> ResGrVec;
	//FillResGrList(&ResGrVec, m_Settings.m_ResGroups);

	//CHipsRuleItem Rule;
	//for (DWORD i = 0; i < AppGrVec.size(); i++)
	//{
	//	for (DWORD j = 0; j < ResGrVec.size(); j++)
	//	{
	//		//m_Settings.GetRule(AppGrVec[i].m_nAppGrpID, true, ResGrVec[j].m_ResGroupID, true, Rule);
	//	}
	//}
	//for (DWORD i = 0; i < m_Settings.m_vAppList.size(); i++)
	//{
	////m_Settings.m_AppGroups.m_nAppGrpID
	//	//m_aChilds
	//}
	return TRUE;
}
