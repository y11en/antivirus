// PageAntivirus.cpp : Implementation of CPageAntivirus

#define PR_IMPEX_DEF

#include "stdafx.h"
#pragma warning(disable: 4146)

//#import "dll\mso9.dll" rename_namespace("Office"), named_guids, implementation_only
#include "OutlookAddin.h"
#include "PageAntivirus.h"
#include "util.h"
#include "InitTerm.h"

#include <Prague/plugin/p_win32_reg.h>
#include <Prague/plugin/p_string.h>

#include <PPP/structs/s_gui.h>

#include <ProductCore/structs/s_profiles.h>


#define REGPATH_MCOUSETTINGS "HKEY_CURRENT_USER\\" VER_PRODUCT_REGISTRY_PATH "\\mcou"

/////////////////////////////////////////////////////////////////////////////
// CPageAntivirus

STDMETHODIMP  CPageAntivirus::GetControlInfo(LPCONTROLINFO pCI )
{
	return S_OK;
};

// IOleObject
STDMETHODIMP CPageAntivirus::SetClientSite(IOleClientSite *pClientSite)
{
	// call default ATL implementation
	HRESULT result = IOleObjectImpl<CPageAntivirus>::SetClientSite(pClientSite);
	if (result != S_OK)
		return result;
	
	// pClientSite may be NULL when container has being destructed
	if (pClientSite != NULL)
	{
		CComQIPtr<Outlook::PropertyPageSite> pPropertyPageSite(pClientSite);
		result = pPropertyPageSite.CopyTo(&m_pPropPageSite);
	}
	m_fDirty = false;
	return result;
}

STDMETHODIMP CPageAntivirus::get_Dirty(VARIANT_BOOL *Dirty)
{
	*Dirty = m_fDirty.boolVal;
	return S_OK;
}

STDMETHODIMP CPageAntivirus::Apply()
{
	SetSettings(m_Settings);
	return S_OK;
}

STDMETHODIMP CPageAntivirus::GetPageInfo(BSTR *HelpFile,LONG *HelpContext)
{
	if (HelpFile == NULL)
		return E_POINTER;
	if (HelpContext == NULL)
		return E_POINTER;
	return E_NOTIMPL;	
}

STDMETHODIMP CPageAntivirus::OnAmbientPropertyChange(DISPID dispid)
{
	if (dispid == DISPID_AMBIENT_BACKCOLOR)
	{
		//SetBackgroundColorFromAmbient();
		FireViewChange();
	}
	return IOleControlImpl<CPageAntivirus>::OnAmbientPropertyChange(dispid);
}

// ---------------------- CPageAntivirus kav6 se ----------------------

HRESULT CPageAntivirus::ControlQueryInterface(const IID& iid, void** ppv)
{
	return _InternalQueryInterface(iid, ppv);
}

HWND CPageAntivirus::CreateControlWindow(HWND hWndParent, RECT& rcPos)
{
	CRootItem * pRoot = CGuiLoader::Instance().GetRoot();
	if( !pRoot )
		return NULL;

	m_hWndParent = hWndParent;
	
	CItemBase * pItem = NULL;
	pRoot->LoadItem(this, pItem, "OutlookPlugun.Antivirus");
	if( !pItem )
		return NULL;
	
	return m_hWnd = pItem->GetWindow();
}

void CPageAntivirus::OnCreate()
{
	TBaseDlg::OnCreate();
#if _BASEGUI_VER >= 0x0200
	Item()->AddDataSource(&m_Status);
#else
	m_pBinding->AddDataSource(&m_Status);
#endif
}

void CPageAntivirus::OnInit()
{
	if(!IsInited())
	{
		GetSettings(m_Settings);
		GetStatus(m_Status);
	}

	UpdateEnablingByPolicy();

	TBaseDlg::OnInit();
}

bool CPageAntivirus::OnClicked(CItemBase* pItem)
{
	if(pItem->m_strItemType == GUI_ITEMT_HOTLINK)
	{
		hTASKMANAGER pTM = (hTASKMANAGER)GetBL();
		if(pTM)
		{
#if _BASEGUI_VER >= 0x0200
			pTM->AskAction(TASKID_TM_ITSELF, cAskGuiAction::SHOW_MAIN_WND, &cProfileAction(AVP_PROFILE_MAILMONITOR));
#else
			pTM->AskAction(TASKID_TM_ITSELF, cAskGuiAction::SHOW_SETTINGS_WND, &cProfileAction(AVP_PROFILE_MAILMONITOR));
#endif
		}
	}

	return TBaseDlg::OnClicked(pItem);
}

HWND CPageAntivirus::OnGetParent()
{
	return m_hWndParent;
}

void CPageAntivirus::UpdateEnablingByPolicy()
{
	bool bIsPolicyApplied = false;
	if(m_Status.m_bKavStarted)
	{
		hTASKMANAGER pTM = (hTASKMANAGER)GetBL();
		cMCSettings mcSettings;
		if(pTM && PR_SUCC(pTM->GetProfileInfo(AVP_PROFILE_MAILMONITOR, &mcSettings)) &&
			mcSettings.IsMandatoriedByPtr(&mcSettings.m_bCheckIncomingMessagesOnly))
		{
			bIsPolicyApplied = true;

//			Enable("CheckDelivery", false);
			Enable("CheckRead", false);
			Enable("CheckSend", false);
			if(!m_Settings.m_bCheckOnDelivery || !m_Settings.m_bCheckOnRead ||
				m_Settings.m_bCheckOnSend == mcSettings.m_bCheckIncomingMessagesOnly)
			{
//				m_Settings.m_bCheckOnDelivery = true;
				m_Settings.m_bCheckOnRead = true;
				m_Settings.m_bCheckOnSend = !mcSettings.m_bCheckIncomingMessagesOnly;
				UpdateData(true);
				SetDirty();
			}
		}
	}

	if(!bIsPolicyApplied)
	{
//		Enable("CheckDelivery", true);
		Enable("CheckRead", true);
		Enable("CheckSend", true);
	}
}

void CPageAntivirus::OnTimerRefresh(tDWORD nTimerSpin)
{
	if(nTimerSpin % 10)
		return;

	cAntispamPluginStatus Status; GetStatus(Status);
	if(m_Status.m_bKavStarted != Status.m_bKavStarted || m_Status.m_bMcEnabled != Status.m_bMcEnabled)
	{
		m_Status = Status;
		UpdateData(&m_Status, true);
	}

	UpdateEnablingByPolicy();
}

void CPageAntivirus::OnChangedData(CItemBase* pItem)
{
	TBaseDlg::OnChangedData(pItem);
	SetDirty();
}

void CPageAntivirus::GetStatus(cAntispamPluginStatus &Status)
{
	MCOU::MCState mcState = MCOU::CheckMCState();

	Status.m_bKavStarted = GetBL() != NULL;
	Status.m_bMcEnabled  = mcState == MCOU::mcsOPEnabledDuplex || mcState == MCOU::mcsOPEnabledIncoming;
}

void CPageAntivirus::SetDirty(bool bDirty)
{
	m_fDirty = true;
	m_pPropPageSite->OnStatusChange();
}

void CPageAntivirus::GetSettings(cMCPluginSettings &Settings)
{
	cAutoObj<cRegistry> hReg;
	if(PR_FAIL(g_root->sysCreateObject((hOBJECT *)&hReg, IID_REGISTRY, PID_WIN32_REG)))
		return;

	if(PR_FAIL(hReg->propSetStr(0, pgROOT_POINT, REGPATH_MCOUSETTINGS)))
		return;

	if(PR_FAIL(hReg->propSetBool(pgOBJECT_RO, cTRUE)))
		return;

	if(PR_FAIL(hReg->sysCreateObjectDone()))
		return;

	cSerializable *pSer = &Settings;
	g_root->RegDeserialize(&pSer, hReg, NULL, cMCPluginSettings::eIID);
}

void CPageAntivirus::SetSettings(cMCPluginSettings &Settings)
{
	cAutoObj<cRegistry> hReg;
	if(PR_FAIL(g_root->sysCreateObject((hOBJECT *)&hReg, IID_REGISTRY, PID_WIN32_REG)))
		return;

	if(PR_FAIL(hReg->propSetStr(0, pgROOT_POINT, REGPATH_MCOUSETTINGS)))
		return;

	if(PR_FAIL(hReg->propSetBool(pgOBJECT_RO, cFALSE)))
		return;

	if(PR_FAIL(hReg->sysCreateObjectDone()))
		return;

	g_root->RegSerialize(&Settings, SERID_UNKNOWN, hReg, NULL);
}


