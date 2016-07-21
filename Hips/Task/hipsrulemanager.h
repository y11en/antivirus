#ifndef _HIPS_RULE_MANAGER_H_
#define _HIPS_RULE_MANAGER_H_

#include "local_inc.h"

#include "task.h"

#include <map>
typedef std::pair<DWORD, pSHipsResourceItem> tResourcePair;
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////
class CHipsRuleManager
{
private:
	HipsResourceList	m_HipsResourceList;
	HipsResourceGrList	m_HipsResourceGrList;
	HipsAppList			m_HipsAppList;			//	internal app list

	HipsRulesListIt		m_HipsRulesListIt;	//	for GetFirst/GetNext rule
	HipsResGrListIt		m_ResGrListIt;	//	for GetFirst/GetNext rule

	cResourceIterator m_ResourceIterator;

	void * m_pTask;
	CHipsManager * m_pHipsManager;


public:
	CHipsSettings m_Settings;

	CHipsSettings * m_pStandardSettings;
	HipsRulesList	m_HipsRulesList;
private:

	BOOL AddResourceItem(DWORD l_ResID, /*DWORD l_ResGrID, */DWORD l_ResType, wchar_t * l_ResName, wchar_t * l_ResValue, /*wchar_t * l_ResDesc, */DWORD l_Flags);
	BOOL AddResourceItem(DWORD l_ResID, DWORD l_ResType, DWORD l_Severity);
	//BOOL AddResourceGrItem(DWORD l_ID, DWORD l_ResID, DWORD l_ResGrID);
	BOOL AddAppItem(DWORD l_AppID, DWORD l_AppGrID, wchar_t * l_AppName, BYTE * l_pAppHash, DWORD l_AppFlags);
	//BOOL AddRuleItem(DWORD l_RuleID, EnumHipsRuleItemState l_State, DWORD l_RuleGrID, DWORD l_AppID, DWORD l_ResID, DWORD l_AccessFlag, DWORD l_AppGrId, DWORD l_ResGrId);

	BOOL GenAndAddRulesForAppEx(pSHipsAppItem l_pApp, CHipsRuleItem * l_pRule, HipsRulesList * l_pHipsRulesList, DWORD l_StartOffset = 0);

	wchar_t * GetResName(DWORD l_ResID);
	wchar_t * GetResValue(DWORD l_ResID);
	wchar_t * GetAppName(DWORD l_AppID);


	//BOOL GetFirstResIdFromResGrTab(DWORD l_ResGrID, DWORD * l_pOutValue);
	//BOOL GetNextResIdFromResGrTab(DWORD l_ResGrID, DWORD * l_pOutValue);

	HipsResourceListIt m_HipsResourceListIt;
	BOOL GetFirstResEx(DWORD l_AppIdEx, DWORD l_ResIdEx, pSHipsResourceItem * l_ppResItem);
	BOOL GetNextResEx(DWORD l_AppIdEx, DWORD l_ResIdEx, pSHipsResourceItem * l_ppResItem);
	BOOL GetResEx(DWORD l_AppIdEx, DWORD l_ResIdEx, pSHipsResourceItem * l_ppResItem);

	HipsAppListIt m_HipsAppListIt;
	BOOL GetFirstAppEx(DWORD l_AppIdEx, pSHipsAppItem * l_ppAppItem);
	BOOL GetNextAppEx(DWORD l_AppIdEx, pSHipsAppItem * l_ppAppItem);
	BOOL GetAppEx(DWORD l_AppIdEx, pSHipsAppItem * l_ppAppItem);

	BOOL FindIntAppById(DWORD l_AppIs, pSHipsAppItem * l_ppAppItem);

	cSerializable * GetChildItem(cSerializable * pGroup, tDWORD ItemNum);
	BOOL GetChildGrIdList(cSerializable * pGroup, DWORD l_ParentGrId, DWORD ** l_ppGrIdList, DWORD * l_pGrCount, bool IsNeedToAdd = false);

	bool ConvertFilePathToNative(wchar_t * l_pSrcPath, wchar_t ** l_ppDstPath);

	//BOOL DesirializeFromXML(wchar_t * l_pBaseFileName, CHipsSettings * l_pSettings);
	void MakeDevRuleRule(cResource * l_pResource, DWORD l_AccessFlag, DWORD l_StartOffset, HipsRulesList * l_pHipsRulesList);


public:
	CHipsRuleManager(void);
	~CHipsRuleManager(void);

	BOOL Init(void * l_pTask, CHipsManager * l_pHipsManager);

	template <class S>
	void ClearList(std::list<S*> * list);

	//	resolving rule list
	BOOL ResolveRuleList();
	//	free internal lists
	BOOL FreeInternalRules();

	BOOL GetFirstRule(pSHipsRuleItem * l_ppRule, HipsRulesList * l_pHipsRulesList);
	BOOL GetNextRule(pSHipsRuleItem * l_ppRule, HipsRulesList * l_pHipsRulesList);

	bool IsDirectRuleExist(DWORD l_AppId, DWORD l_ResIdEx);

	DWORD GetResIDByDrvRuleID(DWORD l_DrvRuleID);

	DWORD GetResTypeByRuleID(DWORD l_RuleID, DWORD * l_pOutResId);
	//bool GetResDescByID(DWORD l_ResID, wchar_t ** l_ppResDesc, DWORD * l_pOutResGrId);

	//	find app in m_Settings.m_vAppList
	tERROR FindAppByHash(BYTE * l_pHash, DWORD l_HashSize, CHipsAppItem ** ppApp);

	static void CollectAppGrIDByServInfo(cAppGroup & Item, cAppGroup & ParentItem, void * pContext);

	tERROR FindAppGrByServInfo(wchar_t * l_ServStr, DWORD ServType, DWORD BaseType, cAppGroup ** ppAppGr, cAppGroup ** ppParentAppGr);

	CHipsAppItem * AddApp(wchar_t * l_ProcName, BYTE * l_pHash, DWORD l_HashSize, cAppGroup & Gr, DWORD l_ParentGrId);
	
	BOOL AddRulesForAppToList(CHipsAppItem * l_pApp, HipsRulesList * l_pHipsRulesList);

	std::map<DWORD, pSHipsResourceItem> m_ResourceMap;

	BOOL TestFunc();

};
#endif //_HIPS_RULE_MANAGER_H_