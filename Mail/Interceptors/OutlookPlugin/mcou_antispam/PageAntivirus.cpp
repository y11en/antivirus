// PageAntivirus.cpp : Implementation of CPageAntivirus

#include "stdafx.h"
#pragma warning(disable: 4146)
//#import "dll\mso9.dll" rename_namespace("Office"), named_guids, implementation_only
#include "OutlookAddin.h"
#include "PageAntivirus.h"

#ifdef SubclassWindow
#undef SubclassWindow
#endif

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
	CRootItem *pRoot = CGuiLoader::Instance().GetRoot();
	if(pRoot)
	{
		m_hWndParent = hWndParent;
		
		CItemBase *pItem = NULL;
		pRoot->LoadItem(this, pItem, "OutlookPlugun.Antivirus");
		if(pItem)
		{
			pItem->m_nFlags |= STYLE_TOP_ITEM;
			m_hWnd = pItem->GetWindow();
		}
	}
	return m_hWnd;
}

void CPageAntivirus::OnInit()
{
	if(!IsInited())
		GetStatus(m_Status);

	TBaseDlg::OnInit();
}

void CPageAntivirus::OnClicked(CItemBase* pItem)
{
	sswitch(pItem->m_strItemId.c_str())
//		scase("Link")      if(m_pAS) m_pAS->ShowProductASSettings(); sbreak
	send

	TBaseDlg::OnClicked(pItem);
}

HWND CPageAntivirus::OnGetParent()
{
	return m_hWndParent;
}

void CPageAntivirus::OnTimerRefresh(tDWORD nTimerSpin)
{
	cAntispamPluginStatus Status; GetStatus(Status);
	if(m_Status.m_bKavStarted != Status.m_bKavStarted || m_Status.m_bAsEnabled != Status.m_bAsEnabled)
	{
		m_Status = Status;
		UpdateData(&m_Status, true);
	}
}

void CPageAntivirus::GetStatus(cAntispamPluginStatus &Status)
{
	Status.m_bKavStarted = CAntispamOUSupportClass::IsAVPTaskManagerEnabled();
	Status.m_bAsEnabled  = CAntispamOUSupportClass::IsAntispamEngineEnabled();
}

void CPageAntivirus::SetDirty(bool bDirty)
{
	m_fDirty = true;
	m_pPropPageSite->OnStatusChange();
}
