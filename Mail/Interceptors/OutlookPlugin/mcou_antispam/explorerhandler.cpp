#include "stdafx.h"
#include "OutlookAddin.h"
#include "Addin.h"
#include "AntispamOUSupportClass.h"
#include <ProductCore/GuiLoader.h>
#include "explorerhandler.h"
#include "macros.h"

_bstr_t vAntispamBarName("Kaspersky Antispam");
_bstr_t vSpamBtnName("Report_Body.ReportDetails.antispam.Events.menu.TrainAsSpam");
_bstr_t vHamBtnName( "Report_Body.ReportDetails.antispam.Events.menu.TrainAsHam");

#define ADVISEBUTTON(_type) m_dwCookie_##_type##Button = m_pAddin->AdviseSelectionChange(m_sp##_type##Button, (IUnknown*)(##_type##ButtonEvents*)m_pAddin, &__uuidof(Office::_CommandBarButtonEvents), CAddin::at##_type##ButtonClick); if (m_dwCookie_##_type##Button) PR_TRACE((0, prtNOT_IMPORTANT, " => Button advised")); else PR_TRACE((0, prtNOT_IMPORTANT, " => Button NOT advised"));
#define ADVISEMENU(_type)   m_dwCookie_##_type##Menu   = m_pAddin->AdviseSelectionChange(m_sp##_type##Menu,   (IUnknown*)(##_type##MenuEvents*)m_pAddin,   &__uuidof(Office::_CommandBarButtonEvents), CAddin::at##_type##ButtonClick); if (m_dwCookie_##_type##Menu)   PR_TRACE((0, prtNOT_IMPORTANT, " => Menu advised"));   else PR_TRACE((0, prtNOT_IMPORTANT, " => Menu NOT advised"));

static bool _IsWin9x()
{
	OSVERSIONINFO vi = {sizeof(vi)};
	GetVersionEx(&vi); // Jeffrey Richter's method of detecting Win9x
	if (vi.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS)
		return false; // it's not Win9x
	
	return true;
}

bool IsWin9x()
{
	static bool bIsWin9x = _IsWin9x();
	return bIsWin9x;
}


HRESULT LocalizeButton(Office::_CommandBarButton* pBtn, DWORD dwIDB, _bstr_t szCaption, _bstr_t szTag, _bstr_t szTip, _bstr_t szShortcut )
{
	FUNC_TRACE(("CExplorerHandler::LocalizeButton"));
	HRESULT hr = S_OK;
	_TRY_BEGIN

	// to set a bitmap to a button, load a 32x32 bitmap
	// and copy it to clipboard. Call CommandBarButton's PasteFace()
	// to copy the bitmap to the button face. to use
	// Outlook's set of predefined bitmap, set button's FaceId to 	//the
	// button whose bitmap you want to use
	if ( IsWin9x() )
		pBtn->PutStyle(Office::msoButtonCaption);
	else
	{
		HBITMAP hBmp_old = NULL;
		HBITMAP hBmp =(HBITMAP)::LoadImage(
			_Module.GetResourceInstance(),
			MAKEINTRESOURCE(dwIDB),
			IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
		// put bitmap into Clipboard
		::OpenClipboard(NULL);
		//::EmptyClipboard();
		hBmp_old = (HBITMAP)::GetClipboardData(CF_BITMAP);
		::SetClipboardData(CF_BITMAP, (HANDLE)hBmp);
		::CloseClipboard();
		::DeleteObject(hBmp);		
		// set style before setting bitmap
		pBtn->PutStyle(Office::msoButtonIconAndCaption);
		pBtn->PutOLEUsage(Office::msoControlOLEUsageNeither);
		hr = pBtn->PasteFace();
		::OpenClipboard(NULL);
		::SetClipboardData(CF_BITMAP, (HANDLE)hBmp_old);
		::CloseClipboard();
	}
	pBtn->PutCaption(szCaption); 
	//  pBtn->PutVisible(VARIANT_FALSE); 
	//	pBtn->PutEnabled(VARIANT_TRUE);
	pBtn->PutTooltipText(szTip); 
	pBtn->PutTag(szTag); 
	pBtn->PutShortcutText(szShortcut);

	_CATCH_ALL_TRACE
	_CATCH_END
	return hr;
}

Office::CommandBarControlPtr FindControl(CComPtr<Office::CommandBarControls> spBarControls, _bstr_t bstrTag, int* pnPosFound = NULL, int nFromPos = 1)
{
	FUNC_TRACE(("CExplorerHandler::FindControl"));
	FOREACH_CONTROL_BEGIN(spBarControls, nFromPos)
	{
//		FUNC_TRACE(("CExplorerHandler::FindControl",  i));
		_bstr_t szTag = spCurrentControl->GetTag();
		if ( spCurrentControl->GetTag() == bstrTag )
		{
			if (pnPosFound) *pnPosFound = i;
			return (CommandBarControl *)spCurrentControl; 
		}
	}
	FOREACH_CONTROL_END
	if (pnPosFound) *pnPosFound = 0;
	return NULL;
}

Office::_CommandBarButtonPtr CreateControl(CComPtr<Office::CommandBarControls> spBarControls, DWORD dwIDB, _bstr_t szCaption, _bstr_t szTag, _bstr_t szTip, _bstr_t szShortcut)
{
	FUNC_TRACE(("CExplorerHandler::CreateControl(spBarControls)"));
//	if ( IsWin9x() ) return 0;

	CComVariant vTemp (VARIANT_FALSE);
	CComVariant vEmpty (DISP_E_PARAMNOTFOUND, VT_ERROR);
	bool bFound = false;
	CComPtr<Office::CommandBarControl> spBarControl; 
	_TRY_BEGIN
		spBarControl = FindControl(spBarControls, szTag); 
		bFound = spBarControl != NULL;
		if(!bFound)
		{
			PR_TRACE((0, prtIMPORTANT, "Control not found, creating new"));
			spBarControl = spBarControls->Add(CComVariant(1), vEmpty, vEmpty, vEmpty, vTemp); 
		}
		else
		{
			PR_TRACE((0, prtIMPORTANT, "Control found"));
		}
	_CATCH_ALL_TRACE
	_CATCH_END
	CComQIPtr<Office::_CommandBarButton> spBarButton(spBarControl);
	_TRY_BEGIN
		if ( spBarButton ) // локализуем кнопки всегда
			LocalizeButton(spBarButton, dwIDB, szCaption, szTag, szTip, szShortcut);
	_CATCH_ALL_TRACE
	_CATCH_END
	return (_CommandBarButton *)spBarButton;
}

Office::_CommandBarButtonPtr CreateControl(CComPtr<Office::CommandBar> spBar, DWORD dwIDB, _bstr_t szCaption, _bstr_t szTag, _bstr_t szTip, _bstr_t szShortcut)
{
	FUNC_TRACE(("CExplorerHandler::CreateControl(spBar)"));
	CComPtr<Office::CommandBarControls> spCurrentBarControls = NULL;
	_TRY_BEGIN spCurrentBarControls = spBar->GetControls();	_CATCH_ALL_TRACE _CATCH_END
	return CreateControl(spCurrentBarControls, dwIDB, szCaption, szTag, szTip, szShortcut);
}


/******************************************************************
 * CExplorerHandler
 *
 ******************************************************************/
CExplorerHandler::CExplorerHandler(CAddin *pAddin, Outlook::_Explorer *spExplorer)
		: CHandler(__uuidof(Outlook::ExplorerEvents), pAddin)
 , bConnected(false)
 , m_pExplorer(spExplorer)
 , m_dwCookie_HamButton(0)
 , m_dwCookie_SpamButton(0)
 , m_dwCookie_HamMenu(0)
 , m_dwCookie_SpamMenu(0)
{
	FUNC_TRACE(("CExplorerHandler::CExplorerHandler"));
	OnNewExplorer();
}

#define CTRL_DELETE(_ctrl_) if (_ctrl_) _ctrl_->Delete(); _ctrl_=0;

CExplorerHandler::~CExplorerHandler() 
{
	FUNC_TRACE(("CExplorerHandler::~CExplorerHandler"));
	_TRY_BEGIN
		m_pAddin->UnadviseSelectionChange(m_spSpamMenu,   __uuidof(Office::_CommandBarButtonEvents), m_dwCookie_SpamMenu);
		m_pAddin->UnadviseSelectionChange(m_spHamMenu,    __uuidof(Office::_CommandBarButtonEvents), m_dwCookie_HamMenu);
		m_pAddin->UnadviseSelectionChange(m_spHamButton,  __uuidof(Office::_CommandBarButtonEvents), m_dwCookie_HamButton);
		m_pAddin->UnadviseSelectionChange(m_spSpamButton, __uuidof(Office::_CommandBarButtonEvents), m_dwCookie_SpamButton);
		
		PUT_ENABLED_CTRL(m_spSpamButton , VARIANT_FALSE);
		PUT_ENABLED_CTRL(m_spHamButton  , VARIANT_FALSE);
		PUT_ENABLED_CTRL(m_spSpamMenu   , VARIANT_FALSE);
		PUT_ENABLED_CTRL(m_spHamMenu    , VARIANT_FALSE);
		m_spAntispamBar->PutEnabled(VARIANT_FALSE);
	_CATCH_ALL_TRACE
	_CATCH_END
}

/******************************************************************
 * CExplorerHandler::Invoke()
 *
 ******************************************************************/
STDMETHODIMP CExplorerHandler::Invoke(DISPID dispIdMember, REFIID riid, LCID /*lcid*/, WORD /*wFlags*/, DISPPARAMS* pDispParams, VARIANT* /*pVarResult*/, EXCEPINFO* /*pExcepInfo*/, UINT* /*puArgErr*/)
{
    if ((riid != IID_NULL))
        return E_INVALIDARG;

	FUNC_TRACE(("CExplorerHandler::Invoke", dispIdMember));
    HRESULT hr = S_OK;  // Initialize

    switch(dispIdMember)
	{
		// Outlook::ExplorerEvents::Activate()
        case 0xf001: 
			PR_TRACE((0, prtIMPORTANT, "Processing Outlook::ExplorerEvents::Activate event")); 
            if (pDispParams->cArgs != 0)
                return E_INVALIDARG;
            else
			{
                OnNewExplorer();
				CAntispamOUSupportClass* pAS = CAntispamOUSupportClass::GetInstance(_Module.GetModuleInstance());
				if(pAS->IsAntispamEngineEnabled())
					ShowAntispamBar(VARIANT_TRUE);
			}
        break;
		// Outlook::ExplorerEvents::FolderSwitch()
        case 0xf002: 
			PR_TRACE((0, prtIMPORTANT, "Processing Outlook::ExplorerEvents::FolderSwitch event")); 
            if (pDispParams->cArgs != 0)
                return E_INVALIDARG;
            else
			{
				OnNewExplorer();
				OnFolderChange();
			}
        break;
		// Outlook::ExplorerEvents::SelectionChange()
		case 0xf007: 
			PR_TRACE((0, prtIMPORTANT, "Processing Outlook::ExplorerEvents::SelectionChange event")); 
			if (pDispParams->cArgs != 0)
				return E_INVALIDARG;
			else
				OnSelectionChange();
			break;
		// Outlook::ExplorerEvents::Close()
        case 0xf008: 
			PR_TRACE((0, prtIMPORTANT, "Processing Outlook::ExplorerEvents::Close event")); 
            if (pDispParams->cArgs != 0)
                return E_INVALIDARG;
            else 
			{
                FreeEvents();
                Release();
            }
	        break;
        default:
            hr = DISP_E_MEMBERNOTFOUND;
			break;
    }
  
    return hr;
}

void CExplorerHandler::OnFolderChange()
{
	FUNC_TRACE(("CExplorerHandler::OnFolderChange"));
	OnSelectionChange();
}

void CExplorerHandler::OnSelectionChange()
{
	FUNC_TRACE(("CExplorerHandler::OnSelectionChange"));
	HRESULT hr = S_OK;
	CComPtr<Outlook::_Explorer> spExplorer = m_pExplorer; 	

	long lItemCount = 0;
	if(spExplorer) 
	{
		CComPtr<Outlook::MAPIFolder> spFolder;
		spExplorer->get_CurrentFolder(&spFolder);
		if(SUCCEEDED(hr) && spFolder)
		{
			CComBSTR bszFolderName;
			hr = spFolder->get_Name(&bszFolderName);
			if(SUCCEEDED(hr))
			{
				// Дизаблим кнопки, если не выделено ни одного сообщения
				// Подсчет количества писем в папке без учета выделения позволяет не генерировать
				// события OnRead! Поэтому сделаем именно так
				CComPtr<Outlook::_Items> spItems;
				hr = spFolder->get_Items(&spItems);
				
				if(SUCCEEDED(hr) && spItems)
				{
					hr = spItems->get_Count(&lItemCount);
					if(SUCCEEDED(hr))
					{
						PR_TRACE((0, prtIMPORTANT, "Folder '%S' contains %d items", (BSTR)bszFolderName, lItemCount));
					}
					else
					{
						PR_TRACE((0, prtERROR, "Failed to get folder item count (0x%08x)", hr));
					}
				}
				else
				{
					PR_TRACE((0, prtERROR, "Failed to get folder items (0x%08x)", hr));
				}
			}
			else
			{
				PR_TRACE((0, prtERROR, "Failed to get folder name (0x%08x)", hr));
			}
		}
		else
		{
			PR_TRACE((0, prtERROR, "Failed to get current folder (0x%08x)", hr));
		}
	}
	
	VARIANT_BOOL vEnable = SUCCEEDED(hr) && lItemCount > 0;

	PR_TRACE((0, prtIMPORTANT, "%s controls", vEnable ? "Enabling" : "Disabling"));
	PUT_ENABLED(m_spSpamButton, vEnable);
	PUT_ENABLED(m_spSpamMenu,   vEnable);
	PUT_ENABLED(m_spHamButton,  vEnable);
	PUT_ENABLED(m_spHamMenu,    vEnable);
}

#if _BASEGUI_VER >= 0x0200
static _bstr_t LoadString(CRootItem *pRoot, LPCTSTR szSection, LPCTSTR szKey)
{
	std::wstring ws;
	ws = pRoot->GetString(TOR_cStrObj, PROFILE_LOCALIZE, szSection, szKey);
	remove_quotes(ws);
	return ws.c_str();
}
#endif

void CExplorerHandler::OnNewExplorer()
{
	if ( bConnected )
		return;

	FUNC_TRACE(("CExplorerHandler::OnNewExplorer"));

	_bstr_t bstrSpamCaption(OLESTR("Spam"));
	_bstr_t bstrSpamTipText(OLESTR("Mark selected messages as Spam"));
	_bstr_t bstrHamCaption(OLESTR("Not Spam"));
	_bstr_t bstrHamTipText(OLESTR("Mark selected messages as Not Spam"));

	CRootItem* pGuiRoot = CGuiLoader::Instance().GetRoot();
	if ( pGuiRoot )
	{
#if _BASEGUI_VER >= 0x0200
		bstrSpamCaption = LoadString(pGuiRoot, "OutlookPlugun.Antispam", "SpamCaption");
		bstrSpamTipText = LoadString(pGuiRoot, "OutlookPlugun.Antispam", "Spam");
		bstrHamCaption =  LoadString(pGuiRoot, "OutlookPlugun.Antispam", "HamCaption");
		bstrHamTipText =  LoadString(pGuiRoot, "OutlookPlugun.Antispam", "Ham");
#else
		bstrSpamCaption = pGuiRoot->GetStringBufW(PROFILE_LOCALIZE, "Report_Body.ReportDetails.antispam.Events.menu", "TrainAsSpamCaption");
		bstrSpamTipText = pGuiRoot->GetStringBufW(PROFILE_LOCALIZE, "Report_Body.ReportDetails.antispam.Events.menu", "TrainAsSpam");
		bstrHamCaption =  pGuiRoot->GetStringBufW(PROFILE_LOCALIZE, "Report_Body.ReportDetails.antispam.Events.menu", "TrainAsHamCaption");
		bstrHamTipText =  pGuiRoot->GetStringBufW(PROFILE_LOCALIZE, "Report_Body.ReportDetails.antispam.Events.menu", "TrainAsHam");
#endif
	}
	
	CComPtr<Office::CommandBar> spAntispamBar;
	CComPtr<Office::_CommandBars> spCmdBars;
	CComVariant vPos(Office::msoBarTop); 
	CComVariant vTemp(VARIANT_FALSE); // menu is temporary?
	CComVariant vEmpty(DISP_E_PARAMNOTFOUND, VT_ERROR);			
	CComPtr<Outlook::_Explorer> spExplorer = m_pExplorer; 
	CComPtr<Office::CommandBarControls> spCmdCtrls;
	CComPtr<Office::CommandBarControls> spCmdBarCtrls; 
	CComPtr<Office::CommandBarPopup> spCmdPopup;
	CComPtr<Office::CommandBarControl> spCmdCtrl;
	_TRY_BEGIN
		HRESULT hr = spExplorer->get_CommandBars(&spCmdBars);
		if( FAILED(hr) || !spCmdBars )
			return;

		//if ( !IsWin9x() )
		{
		_TRY_BEGIN 
			FOREACH_BAR_BEGIN(spExplorer)
				if ( spCurrentBar->GetName() == vAntispamBarName )
				{
					spAntispamBar = spCurrentBar;
					break;
				}
			FOREACH_BAR_END
			if ( !spAntispamBar )
			{
				spAntispamBar = spCmdBars->Add(vAntispamBarName, vPos, vEmpty, vTemp); 
			}
			spAntispamBar->PutProtection((MsoBarProtection)(msoBarNoCustomize|msoBarNoChangeVisible));
			spAntispamBar->PutEnabled(VARIANT_TRUE);
			spAntispamBar->PutVisible(VARIANT_FALSE);
//			CAntispamOUSupportClass* pAS = CAntispamOUSupportClass::GetInstance(_Module.GetModuleInstance());
//			if ( pAS && pAS->IsAntispamEngineEnabled() ) spAntispamBar->PutVisible(VARIANT_TRUE);
			m_spAntispamBar = spAntispamBar;
			m_spSpamButton = CreateControl(spAntispamBar, IDB_BITMAP_RED, bstrSpamCaption, vSpamBtnName, bstrSpamTipText, "Alt+Shift+Del");
			m_spHamButton  = CreateControl(spAntispamBar, IDB_BITMAP_GREEN, bstrHamCaption, vHamBtnName, bstrHamTipText, "Alt+Shift+Ins");
			m_spAntispamBar->PutEnabled(VARIANT_FALSE);
		_CATCH_ALL_TRACE
		_CATCH_END
		}

		if ( !IsWin9x() )
		{
			CComPtr<Office::CommandBar> spActiveMenuBar;
			hr = spCmdBars->get_ActiveMenuBar(&spActiveMenuBar); 
			if ( SUCCEEDED(hr) && spActiveMenuBar )
			{
				MsoBarProtection prop_old = msoBarNoProtection;
				_TRY_BEGIN prop_old = spActiveMenuBar->GetProtection(); _CATCH_ALL_TRACE _CATCH_END
				_TRY_BEGIN spActiveMenuBar->PutProtection(msoBarNoProtection); _CATCH_ALL_TRACE _CATCH_END
				_TRY_BEGIN spCmdBarCtrls = spActiveMenuBar->GetControls();  _CATCH_ALL_TRACE _CATCH_END
				FOREACH_CONTROL_BEGIN(spCmdBarCtrls, 1)
				{
					if ( spCurrentControl->GetId() == 30131 /* Actions */ )
					{
						CComPtr<IDispatch> spDisp;
						spDisp = spCurrentControl->GetControl(); 
						CComQIPtr<Office::CommandBarPopup> ppCmdPopup(spDisp);  
						spCmdCtrls = ppCmdPopup->GetControls();
						m_spSpamMenu = CreateControl(spCmdCtrls, IDB_BITMAP_RED, bstrSpamTipText, vSpamBtnName, bstrSpamTipText, "Alt+Shift+Del");
						m_spHamMenu  = CreateControl(spCmdCtrls, IDB_BITMAP_GREEN, bstrHamTipText, vHamBtnName, bstrHamTipText, "Alt+Shift+Ins");
					}
				}
				FOREACH_CONTROL_END
					_TRY_BEGIN spActiveMenuBar->PutProtection(prop_old); _CATCH_ALL_TRACE _CATCH_END
			}
		}

		AdviseButtons();
		OnSelectionChange();
		bConnected = m_spSpamButton && m_spHamButton && (IsWin9x() || (m_spSpamMenu && m_spHamMenu));
		if ( bConnected )
		{
			CAntispamOUSupportClass* pAS = CAntispamOUSupportClass::GetInstance(_Module.GetModuleInstance());
			if ( pAS && pAS->IsAntispamEngineEnabled() ) m_pAddin->PutVisibleControls(VARIANT_TRUE, m_pExplorer);
		}
	_CATCH_ALL_TRACE
	_CATCH_END
}

void CExplorerHandler::ShowAntispamBar(VARIANT_BOOL bShow)
{
	FUNC_TRACE(("CExplorerHandler::ShowAntispamBar"));
	_TRY_BEGIN
		if ( m_spAntispamBar ) 
		{
			m_spAntispamBar->PutEnabled(bShow);
			if ( bShow )
			{
				m_spAntispamBar->PutVisible(bShow);
				AdviseButtons();
			}
		}
	_CATCH_ALL_TRACE
	_CATCH_END
//	FOREACH_BAR_BEGIN(m_pExplorer)
//		if ( spCurrentBar->GetName() == vAntispamBarName )
//			spCurrentBar->PutVisible(bShow);
//	FOREACH_BAR_END
}

void CExplorerHandler::AdviseButtons()
{
	{
		// Если нет доступа к меню - считаем, что не нужно привязывать кнопки
		// Этот алгоритм определен опытным путем: в противном случае при синхронизации 
		// со смартфоном кнопки реально не привязываются
		if ( !m_spSpamMenu ) return;
		if ( !m_spHamMenu ) return;
	}

	FUNC_TRACE(("CExplorerHandler::AdviseButtons"));
	ADVISEBUTTON(Spam);
	ADVISEBUTTON(Ham);
	ADVISEMENU(Spam);
	ADVISEMENU(Ham);
} 