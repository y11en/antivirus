// SettingsWindowAK.cpp: implementation of the SettingsWindowAK class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CSIfacesImpl.h"
#include "SettingsPages.h"
#include "ComAdminGui.h"

#include <iface\i_inifile.h>
#include <plugin\p_inifile.h>

#define SECTID_SETTINGSWINDOW           "SettingsWindow"
#define STRID_SETTINGSERR               "SettingsOperationError"

//////////////////////////////////////////////////////////////////////////
// CPageBase

CPageBase::~CPageBase()
{
	if( Item() )
	{
		Item()->Destroy();
		g_hGui->DestroyRoot();
	}
}

bool CPageBase::CreatePage(CONFIGURATOR_TYPES eCfgType, PAGE_TYPES ePageType, LPCSTR sTaskID, IEnumInterfaces* pIfaces, HWND hParentWnd)
{
	PR_TRACE((g_root, prtIMPORTANT, "ADM\tCPageBase::CreatePage. creating page: task type %s, cfg type %d, page type %d", sTaskID, eCfgType, ePageType));
	
	m_hParentWnd = hParentWnd;
	m_eCfgType   = eCfgType;
	m_ePageType  = ePageType;
	m_sTaskID    = sTaskID;

	if( !OnLoadSettings(sTaskID, pIfaces) )
		return false;

	if( CRootItem * pRoot = g_hGui->GetRoot() )
	{
		LCID userLCID = PRIMARYLANGID(GetSystemDefaultLangID());
		if( userLCID == LANG_CHINESE || userLCID == LANG_JAPANESE || userLCID == LANG_KOREAN || userLCID == LANG_ARABIC )
		{
			RECT rc = {0, 0, 0, 970};
			MapDialogRect(hParentWnd, &rc);
			pRoot->m_dwScaleY = MulDiv(pRoot->m_dwScaleY, rc.bottom, 1625);
		}

		pRoot->m_nGuiFlags |= GUIFLAG_ADMIN|GUIFLAG_STYLESIMPLE;
		if( eCfgType == IKLConfigurator::GROUP_POLICY_CONFIGURATOR )
			pRoot->m_nGuiFlags |= GUIFLAG_LOCKBUTTONS;
		
		tString strItemId;
		strItemId += m_strSection;
		strItemId += GUI_STR_AUTOSECT_SEP;
		strItemId += sTaskID;
		return pRoot->LoadItem(this, m_pItem, m_strSection.c_str(), strItemId.c_str(), 0);
	}
	return false;
}

void CPageBase::LoadPage(LPCSTR strItemId, CItemBase *&pPage, LPCSTR strProfile)
{
	if( pPage )
		pPage->Show(false);

	if( !strProfile )
		strProfile = strItemId;

	pPage = GetItem(strProfile, NULL, false);
	if( !pPage )
	{
		tString strSect;
		strSect += SECTID_SETTINGSWINDOW;
		strSect += GUI_STR_AUTOSECT_SEP;
		strSect += strItemId;

		if( m_ePageType == IKLConfigurator::WIZARD97_PAGE )
		{
			tString strSectWiz(strSect);

			if( m_eCfgType == IKLConfigurator::LOCAL_POLICY_CONFIGURATOR ||
				m_eCfgType == IKLConfigurator::GROUP_POLICY_CONFIGURATOR )
			{
				strSectWiz += GUI_STR_AUTOSECT_SEP;
				strSectWiz += "Policy";
			}

			strSectWiz += GUI_STR_AUTOSECT_SEP;
			strSectWiz += "Wizard";

			m_pItem->LoadItem(NULL, pPage, strSectWiz.c_str(), strProfile);
		}

		if( !pPage )
			m_pItem->LoadItem(NULL, pPage, strSect.c_str(), strProfile);
	}

	if( pPage )
	{
		Item()->m_nHelpId = pPage->m_nHelpId;
		pPage->Show(true);
	}
}

void CPageBase::OnEvent(tDWORD nEventId, cSerializable* pData)
{
	TBase::OnEvent(nEventId, pData);
	
	if( nEventId == cGuiEvent::eIID && pData && pData->isBasedOn(cGuiEvent::eIID) )
		GUIControllerEvent((hOBJECT)g_root, Root(), (cGuiEvent *)pData);
}

bool CPageBase::OnLoadSettings(LPCSTR sTaskID, IEnumInterfaces* pIfaces)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
// CTaskSettingsBase

CTaskSettingsBase::CTaskSettingsBase() :
	m_pProfile(&m_Profile),
	m_pProfilesList(m_pProfile),
	m_bAlreadyValidated(false)
{
}	

CTaskSettingsBase::~CTaskSettingsBase()
{
}

bool CTaskSettingsBase::OnLoadSettings(LPCSTR sTaskID, IEnumInterfaces* pIfaces)
{
	CProfileAdm *pProfile = (CProfileAdm *)g_pProduct->GetProfileByType(sTaskID);
	if( !pProfile )
	{
		PR_TRACE((g_root, prtERROR, "ADM\tCTaskSettingsBase::OnLoadSettings. wrong task type (%s)", sTaskID));
		return false;
	}
	*m_pProfile = *pProfile;
	return m_pProfile->SetInterfaces(m_eCfgType, pIfaces) && m_pProfile->GetSettings();
}

bool CTaskSettingsBase::OnOk()
{
	bool bOk = CheckSettings();
	CItemBase *pThisItem = Item();
	for(CItemBase *pItem = pThisItem->GetChildFirst(false); bOk && pItem; pItem = pItem->GetNext(false))
	{
		if( pItem->m_pSink && !pItem->m_strSection.compare(0, sizeof(SECTID_SETTINGSWINDOW) - 1, SECTID_SETTINGSWINDOW, sizeof(SECTID_SETTINGSWINDOW) - 1) )
		{
			bOk = ((CDialogSink *)pItem->m_pSink)->OnOk();
		}
	}
	if( bOk )
		bOk = ((CDialogValidator *)pThisItem)->ProcessValidator(pThisItem, DLG_ACTION_OK, true) == 0;
	return bOk;
}

bool CTaskSettingsBase::OnApply()
{
	cGuiErrorInfo settErrInfo;
	if( PR_SUCC(settErrInfo.m_nErr = m_pProfile->SetSettings()) )
		return true;

	MsgBoxErr(*this, &settErrInfo, STRID_SETTINGSERR);
	return false;
}

BOOL CTaskSettingsBase::IsPageDirty()
{
	return m_pProfile->isSettingsDirty();
}

static bool any_selected(const updatesources_t &srcs)
{
	for(tDWORD i = 0; i < srcs.size(); i++)
		if( srcs[i].m_bEnable )
			return true;
	return false;
}

bool CTaskSettingsBase::CheckSettings()
{
	if( m_ePageType == IKLConfigurator::WIZARD97_PAGE && m_eCfgType == IKLConfigurator::TASK_CONFIGURATOR )
	{
		if( cUpdaterSettings *pSett = (cUpdaterSettings *)m_pProfile->settings(NULL, cUpdaterSettings::eIID) )
		{
			if( !pSett->m_blAKServerUse && !pSett->m_bUseInternalSources && !any_selected(pSett->m_aSources) )
			{
				MsgBox(*this, "Update_EmptySources");
				return false;
			}
		}
	}

	if( m_bAlreadyValidated )
		return true;
	
	if( Root()->m_nGuiFlags & GUIFLAG_LOCKBUTTONS )
	{
		if( CProfileAdm *pFwProfile = (CProfileAdm *)m_pProfile->GetProfile(AVP_PROFILE_FIREWALL) )
		{
			if( cAHFWSettings *pFwSett = (cAHFWSettings *)pFwProfile->settings(NULL, cAHFWSettings::eIID) )
			{
				if( pFwProfile->enabled() && pFwSett->m_KnownNetworks.empty() )
				{
					bool bMandatoried = pFwSett->IsMandatoriedByPtr(&pFwSett->m_KnownNetworks);
					bool bLocked      = pFwSett->IsLockedByPtr(&pFwSett->m_KnownNetworks);

					if( bMandatoried && !bLocked )
						return m_bAlreadyValidated = (MsgBox(*this, "EmptyNetlistQuestion", NULL, MB_YESNO|MB_ICONQUESTION) == IDYES);
				}
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// CTaskSettingsPage

CTaskSettingsPage::CTaskSettingsPage()
{
	m_strSection = SECTID_TASKSETTINGSAK;
}

void CTaskSettingsPage::OnInited()
{
	CItemBase *pPage = NULL;
	LoadPage(m_pProfilesList->GetName().c_str(cCP_ANSI), pPage);
}

//////////////////////////////////////////////////////////////////////////
// CPolicySettingsPage

CPolicySettingsPage::CPolicySettingsPage(LPCSTR srtSection) :
	m_pActivePanel(NULL),
	m_pPageCombo(NULL)
{
	m_strSection = srtSection;
}

CPolicySettingsPage::CPolicySettingsPage(CProfileAdm * pProfile) :
	m_pActivePanel(NULL),
	m_pPageCombo(NULL)
{
	m_pProfile = (CProfileAdm*)pProfile->GetRootProfile();
	m_strCurrent = pProfile->m_sName.c_str(cCP_ANSI);
}

void CPolicySettingsPage::OnInit()
{
	TBase::OnInit();

	if( m_pPageCombo = (CComboItem *)GetItem("PageCombo", GUI_ITEMT_COMBO) )
	{
		tDWORD nDefIdx = 0;

        // check if all profiles are avaible
		m_pPageCombo->SetType(tid_STRING_OBJ);

		tKeys aKeys;
		Root()->GetKeys(PROFILE_LOCALIZE, "AkPagesCombo", aKeys, false, Item());
		for(size_t i = 0; i < aKeys.size(); i++)
		{
			tIniKey &key = aKeys[i];
			if( m_pProfile->GetProfile(key.c_str()) ||
				key == "Service" ||
				key == "DataFiles"||
				( key == "NetworkSettings" && m_pProfile->GetProfile(AVP_PROFILE_TRAFFICMONITOR) )
				)
			{
				tDWORD nIdx = m_pPageCombo->AddItem(Root()->LocalizeStr(key.val.c_str()), UNDEF_VALUE, key.c_str());
				if( key == m_strCurrent )
					nDefIdx = nIdx;
			}
		}
		m_pPageCombo->SetDef(nDefIdx);
	}
}

void CPolicySettingsPage::OnInited()
{
	OnChangedData(m_pPageCombo);
}

void CPolicySettingsPage::OnChangedData(CItemBase* pItem)
{
	if( m_pPageCombo && pItem == m_pPageCombo )
	{
		cVariant strPage;
		m_pPageCombo->GetValue(strPage);
		LoadPage(variant_strref(strPage).c_str(), m_pActivePanel);
		return;
	}
	
	TBase::OnChangedData(pItem);
}

bool CPolicySettingsPage::OnLoadSettings(LPCSTR sTaskID, IEnumInterfaces* pIfaces)
{
	*m_pProfile = *(CProfileAdm *)g_pProduct;
	if( m_eCfgType == IKLConfigurator::LOCAL_POLICY_CONFIGURATOR )
	{
		for(tDWORD i = 0; i < m_pProfile->m_aChildren.size(); i++)
		{
			cProfileEx *pProfile = m_pProfile->m_aChildren.at(i);
			if( !pProfile->isService() )
			{
				m_pProfile->m_aChildren.remove(pProfile);
				i--;
			}
		}
	}
	return m_pProfile->SetInterfaces(m_eCfgType, pIfaces) && m_pProfile->GetSettings();
}

//////////////////////////////////////////////////////////////////////////
// CWizardPage

CWizardPage::CWizardPage(CProfileAdm* pProfile, int nPage, int nTotal) :
	m_nPage(nPage), m_nTotal(nTotal)
{
	m_strSection = SECTID_TASKSETTINGSAK;
	m_pProfile = m_pProfilesList = pProfile;
}

bool CWizardPage::OnLoadSettings(LPCSTR sTaskID, IEnumInterfaces* pIfaces)
{
	if( m_nPage )
		return true;

	if( m_eCfgType == IKLConfigurator::GROUP_POLICY_CONFIGURATOR || m_eCfgType == IKLConfigurator::LOCAL_POLICY_CONFIGURATOR )
		sTaskID = AVP_PROFILE_PRODUCT;
	
	return CTaskSettingsBase::OnLoadSettings(sTaskID, pIfaces);
}

bool CWizardPage::OnOk()
{
	if( m_nPage )
		return true;
	
	return CTaskSettingsBase::OnOk();
}

bool CWizardPage::OnApply()
{
	if( m_nPage != m_nTotal - 1 )
		return true;

	return CTaskSettingsBase::OnApply();
}

void CWizardPage::OnInited()
{
	if( m_eCfgType == IKLConfigurator::GROUP_POLICY_CONFIGURATOR || m_eCfgType == IKLConfigurator::LOCAL_POLICY_CONFIGURATOR )
	{
		CItemBase *pPage = NULL;

		LPCSTR strProfile = NULL;
		CProfileAdm *pProfile = (CProfileAdm *)g_pProduct->GetProfileByType(m_sTaskID.c_str());
		if( pProfile )
			strProfile = pProfile->m_sName.c_str(cCP_ANSI);

		LoadPage(m_sTaskID.c_str(), pPage, strProfile);
	}
	else
	{
		CItemBase *pPage = NULL;
		if( m_pProfile->m_sType == m_sTaskID.c_str() )
			LoadPage(m_pProfilesList->GetName().c_str(cCP_ANSI), pPage);
		else
			LoadPage(m_sTaskID.c_str(), pPage);
	}
}

//////////////////////////////////////////////////////////////////////////
// CSettingsPage

/*std::wstring __itow(tDWORD value, wchar_t *format = L"%08x")
{
	std::wstring result;
	result.resize(50);
	result.resize(swprintf((wchar_t *)result.c_str(), format, value));
	return result;
}

tDWORD __wtoi(wchar_t *str, wchar_t *format = L"%08x")
{
	tQWORD value = 0;
	swscanf(str, format, &value);
	return value;
}*/

std::string itoa2(tDWORD value, char *format = "%08x")
{
	std::string result;
	result.resize(50);
	result.resize(sprintf((char *)result.c_str(), format, value));
	return result;
}

tDWORD atoi2(char *str, char *format = "%08x")
{
	tQWORD value = 0;
	sscanf(str, format, &value);
	return (tDWORD)value;
}


#define SECTID_SETTINGS  "Settings"
#define FILEID_SETTINGS L"Installer.ini"
#define FILEID_CFG      L"Install.cfg"

CInstallPrepareDlg::CInstallPrepareDlg() : m_pPackage(NULL)
{
	m_strSection = SECTID_TASKSETTINGSAK;
	/*
	enum tComponentFlags {
		iciScan           = 0x0001, //- устанавливать компонент Scan
		iciFileMonitor    = 0x0002, //- устанавливать компонент File Monitor
		iciMailMonitor    = 0x0004, //- устанавливать компонент Mail Monitor
		iciWebMonitor     = 0x0008, //- устанавливать компонент Web Monitor
		iciPdm            = 0x0010, //- устанавливать компонент Pdm
		iciAntiSpy        = 0x0020, //- устанавливать компонент Anti Spy
		iciAntiHacker     = 0x0040, //- устанавливать компонент Anti Hacker
		iciAntiSpam       = 0x0080, //- устанавливать компонент Anti Spam
		iciScanMyComputer = 0x0100, //- устанавливать задачу Scan My Computer
		iciScanStartup    = 0x0200, //- устанавливать задачу Scan Startup
		iciScanCritical   = 0x0400, //- устанавливать задачу Scan Critical Areas
		iciUpdater        = 0x0800, //- устанавливать задачу Updater
		iciEnvironment    = 0x1000, //- добавить путь к avp.com в переменную среды окружения %Path%
		iciMsExclusions   = 0x2000, //- какая-то хрень для Fs от MS
		iciSelfProtection = 0x4000,	//- включать защиту до установки
		iciRetranslation  = 0x8000,

		iciAllProfiles    = iciScan | iciFileMonitor | iciMailMonitor | iciWebMonitor | iciPdm | iciAntiSpy | iciAntiHacker | iciAntiSpam | iciScanMyComputer | iciScanStartup | iciScanCritical | iciUpdater | iciEnvironment | iciMsExclusions | iciSelfProtection | iciRetranslation,
	};
	*/
	switch (GetProductType())
	{
	case eProductTypeFs:  m_Params.m_nVal1 = (tPTR)0x03; break;
	case eProductTypeWks: m_Params.m_nVal1 = (tPTR)0xff; break;
	case eProductTypeSos: m_Params.m_nVal1 = (tPTR)0x01; break;
	}
}

void CInstallPrepareDlg::OnInited()
{
	CItemBase *pPage = NULL;
	LoadPage(m_sTaskID.c_str(), pPage);
}

bool CInstallPrepareDlg::OnApply()
{
	return GetSetSettings(false);
}

bool CInstallPrepareDlg::OnLoadSettings(LPCSTR sTaskID, IEnumInterfaces* pIfaces)
{
	if( !pIfaces->QueryInterface(KLPlugin::KAV_INTERFACE_PACKAGE_SETTINGS, (void **)&m_pPackage) )
		return false;

	return GetSetSettings(true);
}

std::string wcs2mbcs(const wchar_t *str)
{
	std::string strResult;
	if( int nSize = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL) )
	{
		strResult.resize(nSize);
		nSize = WideCharToMultiByte(CP_UTF8, 0, str, -1, (LPSTR)strResult.c_str(), nSize, NULL, NULL);
		strResult.resize(nSize - 1);
	}
	return strResult;
}

std::wstring mbcs2wcs(const char *str)
{
	std::wstring strResult;
	if( int nSize = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0) )
	{
		strResult.resize(nSize);
		nSize = MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)strResult.c_str(), nSize);
		strResult.resize(nSize - 1);
	}
	return strResult;
}

bool CInstallPrepareDlg::GetSetSettings(bool bGet)
{
	if( !m_pPackage )
		return false;

	cAutoObj<cIO> pTmpIo;
	cAutoObj<cIniFile> pInif;
	
	tERROR err = errOK;
	if( PR_SUCC(err) ) err = g_root->sysCreateObjectQuick(pTmpIo, IID_IO, PID_TMPFILE);
	if( PR_SUCC(err) && bGet )
	{
		KLERR_TRY
			KLSTD::MemoryChunkPtr pMem;
			m_pPackage->ReadPkgCfgFile(FILEID_SETTINGS, &pMem);
			err = pTmpIo->SeekWrite(NULL, 0, pMem->GetDataPtr(), pMem->GetDataSize());
		KLERR_CATCH( pError )
			return true;
		KLERR_ENDTRY
	}
	
	if( PR_SUCC(err) ) err = g_root->sysCreateObject(pInif, IID_INIFILE, PID_INIFILE);
	if( PR_SUCC(err) ) err = pInif->set_pgm_hIO(pTmpIo);
	if( PR_SUCC(err) ) err = pInif->sysCreateObjectDone();

	if( PR_SUCC(err) )
	{
		if( bGet )
		{
			tSTRING strValue;
			
			if( PR_SUCC(pInif->GetValue(SECTID_SETTINGS, "Components", &strValue)) )
				m_Params.m_nVal1 = (tPTR)atoi2(strValue);
			
			if( PR_SUCC(pInif->GetValue(SECTID_SETTINGS, "Location", &strValue)) )
				m_Params.m_strVal1 = mbcs2wcs(strValue).c_str();
			
			if( PR_SUCC(pInif->GetValue(SECTID_SETTINGS, "CfgFile", &strValue)) )
				m_Params.m_strVal2 = mbcs2wcs(strValue).c_str();
		}
		else
		{
			// store settings
			m_Params.m_strVal1.check_last_slash(false);
			pInif->SetValue(SECTID_SETTINGS, "Components", (char *)itoa2((tDWORD)m_Params.m_nVal1).c_str());
			pInif->SetValue(SECTID_SETTINGS, "Location",   (char *)wcs2mbcs(m_Params.m_strVal1.data()).c_str());
			pInif->SetValue(SECTID_SETTINGS, "CfgFile",    (char *)wcs2mbcs(m_Params.m_strVal2.data()).c_str());
			pInif->Commit();

			KLERR_TRY
				tQWORD nSize;
				KLSTD::MemoryChunkPtr pMem;
				pTmpIo->GetSize(&nSize, IO_SIZE_TYPE_EXPLICIT);
				KLSTD_AllocMemoryChunk((size_t)nSize, &pMem);
				pTmpIo->SeekRead(NULL, 0, pMem->GetDataPtr(), pMem->GetDataSize());
				m_pPackage->WritePkgCfgFile(FILEID_SETTINGS, pMem);
			KLERR_CATCH( pError )
				err = errNOT_OK;
			KLERR_ENDTRY
			
			// copy cfg to package
			if( !m_Params.m_strVal2.empty() )
			{
				cIOObj pCfgIo((cObject *)g_root, cAutoString(m_Params.m_strVal2), fACCESS_READ);
				if( PR_SUCC(err = pCfgIo.last_error()) )
				{
					tQWORD nSize;
					KLSTD::MemoryChunkPtr pMem;
					pCfgIo->GetSize(&nSize, IO_SIZE_TYPE_EXPLICIT);
					KLSTD_AllocMemoryChunk((size_t)nSize, &pMem);
					pCfgIo->SeekRead(NULL, 0, pMem->GetDataPtr(), pMem->GetDataSize());
					m_pPackage->WritePkgCfgFile(FILEID_CFG, pMem);
				}
				else
					m_pPackage->WritePkgCfgFile(FILEID_CFG, NULL);
			}
			else
			{
				m_pPackage->WritePkgCfgFile(FILEID_CFG, NULL);
			}
		}
	}
	return PR_SUCC(err);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
