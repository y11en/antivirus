// AdminGUI.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "CSIfacesImpl.h"
#include "ComAdminGui.h"
#include <plugin/p_cryptohelper.h>

HINSTANCE           g_hInst = NULL;
DWORD               g_nInstCnt = 0;
DWORD               g_nTlsIndex = TLS_OUT_OF_INDEXES;
eProductType        g_nProductType = eProductTypeUnknown;
HANDLE              g_hPluginRunningMutex = NULL;

extern "C" __declspec(dllexport) void InitPlugin()
{
	InterlockedIncrement((PLONG)&g_nInstCnt);
}

extern "C" __declspec(dllexport) void DeinitPlugin()
{
	if( !InterlockedDecrement((PLONG)&g_nInstCnt) )
		g_Prague.UnLoadPrague();
}

extern "C" __declspec(dllexport) IKLAccessor *GetAccessor()
{
	static CAccessor accessor;
	return &accessor;
}

////////////////////////////////////////////////////////////
// ValidateProductSuite function
//
// Terminal Services detection code for systems running
// Windows NT 4.0 and earlier.
//
////////////////////////////////////////////////////////////

static BOOL ValidateProductSuite (LPSTR lpszSuiteToValidate) 
{
	BOOL fValidated = FALSE;
	LONG lResult;
	HKEY hKey = NULL;
	DWORD dwType = 0;
	DWORD dwSize = 0;
	LPSTR lpszProductSuites = NULL;
	LPSTR lpszSuite;

	// Open the ProductOptions key.

	lResult = RegOpenKeyA(
		HKEY_LOCAL_MACHINE,
		"System\\CurrentControlSet\\Control\\ProductOptions",
		&hKey
		);
	if (lResult != ERROR_SUCCESS)
		goto exit;

	// Determine required size of ProductSuite buffer.

	lResult = RegQueryValueExA( hKey, "ProductSuite", NULL, &dwType, 
		NULL, &dwSize );
	if (lResult != ERROR_SUCCESS || !dwSize)
		goto exit;

	// Allocate buffer.

	lpszProductSuites = (LPSTR) LocalAlloc( LPTR, dwSize );
	if (!lpszProductSuites)
		goto exit;

	// Retrieve array of product suite strings.

	lResult = RegQueryValueExA( hKey, "ProductSuite", NULL, &dwType,
		(LPBYTE) lpszProductSuites, &dwSize );
	if (lResult != ERROR_SUCCESS || dwType != REG_MULTI_SZ)
		goto exit;

	// Search for suite name in array of strings.

	lpszSuite = lpszProductSuites;
	while (*lpszSuite) 
	{
		if (lstrcmpA( lpszSuite, lpszSuiteToValidate ) == 0) 
		{
			fValidated = TRUE;
			break;
		}
		lpszSuite += (lstrlenA( lpszSuite ) + 1);
	}

exit:
	if (lpszProductSuites)
		LocalFree( lpszProductSuites );

	if (hKey)
		RegCloseKey( hKey );

	return fValidated;
}

static BOOL HasGlobalNamespace()
{
	return BYTE(GetVersion()) > 4 || ValidateProductSuite("Terminal Server");
};

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		g_hInst = (HINSTANCE)hModule;
		g_nTlsIndex = TlsAlloc();
		if( !GUIControllerInit(g_hInst) )
			return FALSE;

		WCHAR strModule[2 * MAX_PATH];
		if( GetModuleFileNameW(g_hInst, strModule, countof(strModule)) )
		{
			LoadLibraryW(strModule); // increment reference to myself
			
			WCHAR *strFile = wcsrchr(strModule, L'\\');
			if( strFile++ )
			{
				if( !wcscmp(strFile, KAVFS_MODULE_NAME) )
					g_nProductType = eProductTypeFs;
				
				if( !wcscmp(strFile, KAVWKS_MODULE_NAME) )
					g_nProductType = eProductTypeWks;

				if( !wcscmp(strFile, KAVSOS_MODULE_NAME) )
					g_nProductType = eProductTypeSos;
			}
		}
		if( g_nProductType == eProductTypeUnknown )
			return FALSE;

		g_hPluginRunningMutex = ::CreateMutexW(NULL, FALSE, HasGlobalNamespace() ? (L"Global\\" KAV_PLUGIN_MUTEX) : KAV_PLUGIN_MUTEX);

		break;
	
	case DLL_PROCESS_DETACH:
		GUIControllerDone();
		TlsFree(g_nTlsIndex);
		CloseHandle(g_hPluginRunningMutex);
		g_hPluginRunningMutex = NULL;
		break;
	
	case DLL_THREAD_ATTACH:
		break;
	
	case DLL_THREAD_DETACH:
		break;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//

eProductType GetProductType()
{
	return g_nProductType;
}

wchar_t *GetProductName()
{
	switch(GetProductType())
	{
	case eProductTypeFs:  return KAVFS_PRODUCT_NAME;
	case eProductTypeWks: return KAVWKS_PRODUCT_NAME;
	case eProductTypeSos: return KAVSOS_PRODUCT_NAME;
	}
	return NULL;
}

char *GetProductRegistryPath()
{
	switch(GetProductType())
	{
	case eProductTypeFs:  return REGPATH_SETTINGS_FS;
	case eProductTypeWks: return REGPATH_SETTINGS_WKS;
	case eProductTypeSos: return REGPATH_SETTINGS_SOS;
	}
	return NULL;
}



//////////////////////////////////////////////////////////////////////////
// CRootSinkAdm

bool CRootSinkAdm::OnGetHelpId(tWORD &nHelpId)
{
	tWORD nNewHelpId = nHelpId;
	if( m_mapCtxHelpIds.empty() )
	{
		tKeys Keys;
		((CRootItem *)Item())->GetKeys(PROFILE_LOCALIZE, "AkHelpIdsMap", Keys, false, *this);

		m_mapCtxHelpIds.reserve(Keys.size());
		for(size_t i = 0; i < Keys.size(); i++)
		{
			tIniKey &Key = Keys[i];
			m_mapCtxHelpIds.push_back(tHelpPair(atoi(Key.c_str()), atoi(Key.val.c_str())));
		}
	}

	for(size_t i = 0; i < m_mapCtxHelpIds.size(); i++)
	{
		tHelpPair &HelpId = m_mapCtxHelpIds[i];
		if( HelpId.first == nHelpId )
		{
			nNewHelpId = HelpId.second;
			break;
		}
	}

	PR_TRACE((g_root, prtIMPORTANT, "ADM\tCRootSinkAdm::GetHelpId. help id map (%d - %d)", nHelpId, nNewHelpId));
	
	nHelpId = nNewHelpId;
	
	return true;
}

bool CRootSinkAdm::IsSerializableHolder(const cSerDescriptor* pDesc, const cSerDescriptorField * pFld, LPCSTR strType, CustomHolderData ** ppHolderData)
{
	bool bLockLevel = !strcmp(strType, HOLDERID_LEVEL_FIELDS);
	if( bLockLevel )
		strType = HOLDERID_FIELDS;
	
	sswitch(strType)
	scase(HOLDERID_FIELDS)
		*ppHolderData = new CLockButtonHolder(bLockLevel);
		return true;
		sbreak;
	send;
	return TBase::IsSerializableHolder(pDesc, pFld, strType, ppHolderData);
}

bool CRootSinkAdm::ResolveSerializable(bool bToStruct, const cSerDescriptorField * pFld, LPCSTR strType, CustomHolderData * pHolderData, cSerializable * pStruct, tString& strResult, CBindProps * pProps)
{
	if( !strcmp(strType, HOLDERID_LEVEL_FIELDS) )
		strType = HOLDERID_FIELDS;

	sswitch(strType)
	scase(HOLDERID_FIELDS)
		if( pHolderData )
			((CLockButtonHolder *)pHolderData)->Resolve(bToStruct, pStruct, strResult);
		return true;
		sbreak;
	send;
	
	return TBase::ResolveSerializable(bToStruct, pFld, strType, pHolderData, pStruct, strResult, pProps);
}

cCryptoHelper* CRootSinkAdm::GetCryptoHelper(bool& bSimple)
{
	bSimple = true;
	if( !m_pCrHelp && g_hTM )
        g_hTM->GetService(0, NULL, CRYPTOHELPER_SERVICE_NAME, (hOBJECT*)&m_pCrHelp);
    return m_pCrHelp;
}

//////////////////////////////////////////////////////////////////////////
// CLockButtonHolder

void CLockButtonHolder::Init(CItemPropVals& pParams, CRootItem * pRoot)
{
	m_pRoot = pRoot;
	m_pBinding = pRoot->CreateTextBinding();
	m_pBinding->SetCb(this);

	for(int i = 0; i < pParams.Count(); i++)
		m_Fields.push_back(pParams.GetStr(i));
}

void CLockButtonHolder::Resolve(bool bToStruct, cSerializable * pStruct, tString& strResult)
{
	if( !pStruct )
		return;

	if( pStruct->isBasedOn(CProfile::eIID) )
	{
		m_pProfile = (CProfile *)pStruct;
		m_pProfilesList = m_pProfile->GetRootProfile();
	}
	else
		AddSource(pStruct);
	
	Bind();
	Apply(bToStruct, strResult);
	
	m_pBinding->ClearSources();
	m_pProfile = NULL;
	m_pProfilesList = NULL;
}

void CLockButtonHolder::Bind()
{
	if( !m_Fields.size() )
		return;

	for(size_t i = 0; i < m_Fields.size(); i++)
	{
		LPCSTR strFldPath = m_Fields[i].c_str();
		
		cFldData _FldData;
		if( !m_pBinding->FindField(m_pRoot, strFldPath, _FldData) )
			continue;
		
		MaskItem * pMask = NULL;
		for(int m = 0, n = m_Masks.size(); m < n; m++)
		{
			MaskItem& _MaskItem = m_Masks[m];
			if( _MaskItem.m_pStr == (tPTR)_FldData.m_pFldStruct )
			{
				pMask = &_MaskItem;
				break;
			}
		}
	
		if( !pMask )
		{
			m_Masks.push_back(MaskItem());
			pMask = &m_Masks[m_Masks.size() - 1];
			pMask->m_pStr = (tPTR)_FldData.m_pFldStruct;
			pMask->m_strStructFldPath = strFldPath;
		}

		pMask->m_nMask.SetBit(_FldData.m_nPolicyBit, true);
	}

	if( m_bLockLevel )
	{
		cFldData _FldData;
		if( m_pBinding->FindField(m_pRoot, FIELDID_LEVEL, _FldData) )
		{
			m_LevelMaskItem.m_pStr = (tPTR)_FldData.m_pFldStruct;
			m_LevelMaskItem.m_strStructFldPath = FIELDID_LEVEL;
			m_LevelMaskItem.m_nMask.SetBit(_FldData.m_nPolicyBit, true);
		}
	}
	
	m_Fields.clear();
}

void CLockButtonHolder::Apply(bool bToStruct, tString& strResult)
{
	bool bMandatored = false;
	if( bToStruct )
		bMandatored = strResult == "1";
	
	int nMandatoriedCheckedCount = 0;
	int nLockedCheckedCount = 0;

	int m, n;
	if( m_bLockLevel && !bToStruct && !m_LevelMaskItem.m_nMask.IsZero() )
	{
		cFldData _FldData;
		if( m_pBinding->FindField(m_pRoot, m_LevelMaskItem.m_strStructFldPath.c_str(), _FldData) )
		{
			cCfg * pCfg = (cCfg *)_FldData.m_pFldStruct;
			if( pCfg || pCfg->isBasedOn(cCfg::eIID) )
			{
				bool bAnyMandatoried = false;
				for(m = 0, n = m_Masks.size(); !bAnyMandatoried && m < n; m++)
				{
					MaskItem& _MaskItem = m_Masks[m];

					cFldData _FldData;
					if( !m_pBinding->FindField(m_pRoot, _MaskItem.m_strStructFldPath.c_str(), _FldData) )
						continue;

					cPolicySettings * pSett = (cPolicySettings *)_FldData.m_pFldStruct;
					if( !pSett || !pSett->isBasedOn(cPolicySettings::eIID) )
						continue;

					if( pSett == pCfg )
						continue;

					bAnyMandatoried = !!(pSett->m_dwMandatoriedFields & _MaskItem.m_nMask);
				}
				
				if( bAnyMandatoried )
				{
					pCfg->m_dwMandatoriedFields |= m_LevelMaskItem.m_nMask;
					pCfg->m_dwLevel = SETTINGS_LEVEL_CUSTOM;
				}
				else
					pCfg->m_dwMandatoriedFields &= ~m_LevelMaskItem.m_nMask;
			}
		}
	}
	
	for(m = 0, n = m_Masks.size(); m < n; m++)
	{
		MaskItem& _MaskItem = m_Masks[m];

		cFldData _FldData;
		if( !m_pBinding->FindField(m_pRoot, _MaskItem.m_strStructFldPath.c_str(), _FldData) )
			continue;

		cPolicySettings * pSett = (cPolicySettings *)_FldData.m_pFldStruct;
		if( !pSett || !pSett->isBasedOn(cPolicySettings::eIID) )
			continue;

		if( bToStruct )
		{
			if( bMandatored )
				pSett->m_dwMandatoriedFields |= _MaskItem.m_nMask;
			else
				pSett->m_dwMandatoriedFields &= ~_MaskItem.m_nMask;
		}
		else
		{
			nMandatoriedCheckedCount += (pSett->m_dwMandatoriedFields & _MaskItem.m_nMask) == _MaskItem.m_nMask;
			nLockedCheckedCount      += (pSett->m_dwLockedFields      & _MaskItem.m_nMask) == _MaskItem.m_nMask;
		}
	}

	if( !bToStruct )
		strResult = __itoa10(((nMandatoriedCheckedCount == n) << 0) + ((nLockedCheckedCount == n) << 1));
}

bool CLockButtonHolder::OnAddDataSection(LPCSTR strSect)
{
	if( !m_pProfilesList )
		return false;
	
	CProfile * pProfile = (strSect && *strSect) ?
		m_pProfilesList->GetProfile(strSect) : m_pProfile;

	if( !pProfile )
		return false;

	AddSource(&pProfile->m_cfg, strSect);
	return true;
}

void CLockButtonHolder::AddSource(cSerializable * pStruct, LPCSTR strSect)
{
	if( !pStruct->isBasedOn(cPolicySettings::eIID) )
		return;

	if( pStruct->isBasedOn(cCfg::eIID) )
	{
		cTaskSettings * pSett = ((cCfg *)pStruct)->settings();
		m_pBinding->AddDataSource(pSett, strSect);
		if( pSett && pSett->isBasedOn(cTaskSettings::eIID) )
			m_pBinding->AddDataSource(((cTaskSettings *)pSett)->m_pTaskBLSettings, strSect);
	}

	m_pBinding->AddDataSource(pStruct, strSect);
}
