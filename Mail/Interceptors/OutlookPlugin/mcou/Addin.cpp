
////////////////////////////////////////////////////////////////////////////
//	Copyright : Amit Dey 2002
//
//	Email :amitdey@joymail.com
//
//	This code may be used in compiled form in any way you desire. This
//	file may be redistributed unmodified by any means PROVIDING it is 
//	not sold for profit without the authors written consent, and 
//	providing that this notice and the authors name is included.
//
//	This file is provided 'as is' with no expressed or implied warranty.
//	The author accepts no liability if it causes any damage to your computer.
//
//	Do expect bugs.
//	Please let me know of any bugs/mods/improvements.
//	and I will try to fix/incorporate them into this file.
//	Enjoy!
//
//	Description : Outlook2K Addin 
////////////////////////////////////////////////////////////////////////////


// Addin.cpp : Implementation of CAddin
#include "stdafx.h"
#include "OutlookAddin.h"
#include "Addin.h"

#include <Prague/plugin/p_win32_reg.h>
#include <ProductCore/GuiLoader.h>

CAddin* g_addin = NULL;
HHOOK g_hook = NULL;

class CGuiRoot : public CRootSink
{
public:
	void GetHelpStorageId(cStrObj& strHelpStorageId)
	{
		strHelpStorageId = "%HelpFile%";
		if( ::g_root->sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(strHelpStorageId), 0, 0) != errOK_DECIDED )
			strHelpStorageId.erase();
	}
};

//! Класс, обеспещивающий клавиатурные shortcut'ы


//#define ACTION_ON_MESSAGE( _type, _obj )	_ACTION_ON_MESSAGE##_type(_obj, action)
//#define DEF_ACTION_ON_MESSAGE( _type )	\
//HRESULT _ACTION_ON_MESSAGE##_type( IDispatch* _obj, Action_t action  )	\
//{	\
//	CComPtr<Outlook::_type> spMailItem = NULL;	\
//	HRESULT hr = _obj->QueryInterface(__uuidof(Outlook::_type), (void**)&spMailItem);	\
//	if ( SUCCEEDED(hr) && spMailItem )	\
//	{	\
//		LPMESSAGE pMessage = NULL;	\
//		hr = spMailItem->get_MAPIOBJECT((IUnknown**)&pMessage);	\
//		if ( SUCCEEDED(hr) && pMessage )	\
//		{	\
//			CAntispamOUSupportClass* pAS =	\
//				CAntispamOUSupportClass::GetInstance(_Module.GetModuleInstance());	\
//			if ( pAS )	\
//				hr = pAS->ActionOnMessage(pMessage, action);	\
//			pMessage->Release();	\
//			pMessage = NULL;	\
//		}	\
//	}	\
//	return hr;	\
//}
//DEF_ACTION_ON_MESSAGE( _MailItem );
//DEF_ACTION_ON_MESSAGE( _ReportItem );
//DEF_ACTION_ON_MESSAGE( _AppointmentItem );
//DEF_ACTION_ON_MESSAGE( _MeetingItem );
//DEF_ACTION_ON_MESSAGE( _DocumentItem );
//DEF_ACTION_ON_MESSAGE( _JournalItem );
//DEF_ACTION_ON_MESSAGE( _PostItem );
//DEF_ACTION_ON_MESSAGE( _RemoteItem );
//DEF_ACTION_ON_MESSAGE( _TaskItem );
//DEF_ACTION_ON_MESSAGE( _TaskRequestItem );
//DEF_ACTION_ON_MESSAGE( _TaskRequestAcceptItem );
//DEF_ACTION_ON_MESSAGE( _TaskRequestDeclineItem );
//DEF_ACTION_ON_MESSAGE( _TaskRequestUpdateItem );



/////////////////////////////////////////////////////////////////////////////
// CAddin
_ATL_FUNC_INFO OnClickButtonInfo ={CC_STDCALL,VT_EMPTY,2,{VT_DISPATCH,VT_BYREF | VT_BOOL}};
_ATL_FUNC_INFO OnOptionsAddPagesInfo={CC_STDCALL,VT_EMPTY,1,{VT_DISPATCH}};
_ATL_FUNC_INFO OnSimpleEventInfo ={CC_STDCALL,VT_EMPTY,0};
_ATL_FUNC_INFO OnNewMailInfo ={CC_STDCALL,VT_EMPTY,0};

STDMETHODIMP CAddin::InterfaceSupportsErrorInfo(REFIID riid)
{
	if (IsEqualGUID(IID_IAddin,riid))
		return S_OK;
	return S_FALSE;
}

//HRESULT CAddin::ActionOnMessage(Action_t action)
//{
//	HRESULT hr = S_OK;
//	CComPtr<Outlook::_Explorer> spExplorer; 	
//	hr = m_spApp->ActiveExplorer(&spExplorer);
//	if ( SUCCEEDED(hr) && spExplorer )
//	{
//		CComPtr<Outlook::Selection> spSelection; 	
//		hr = spExplorer->get_Selection(&spSelection);
//		if ( SUCCEEDED(hr) && spSelection )
//		{
//			long lItemCount = 0;
//			hr = spSelection->get_Count(&lItemCount);
//			if ( SUCCEEDED(hr) )
//			{
//				for ( long iCounter = 1; iCounter <= lItemCount; iCounter++  )
//				{
//					CComPtr<IDispatch> obj = NULL;
//					VARIANT v;
//					v.vt = VT_I4;
//					v.intVal = iCounter;
//					hr = spSelection->Item(v, &obj);
//					if ( SUCCEEDED(hr) && obj )
//					{// Check messages
//						if ( FAILED(ACTION_ON_MESSAGE(_MailItem, obj)) )
//						if ( FAILED(ACTION_ON_MESSAGE(_ReportItem, obj)) )
//						if ( FAILED(ACTION_ON_MESSAGE(_AppointmentItem, obj)) )
//						if ( FAILED(ACTION_ON_MESSAGE(_MeetingItem, obj)) )
//						if ( FAILED(ACTION_ON_MESSAGE(_DocumentItem, obj)) )
//						if ( FAILED(ACTION_ON_MESSAGE(_JournalItem, obj)) )
//						if ( FAILED(ACTION_ON_MESSAGE(_PostItem, obj)) )
//						if ( FAILED(ACTION_ON_MESSAGE(_RemoteItem, obj)) )
//						if ( FAILED(ACTION_ON_MESSAGE(_TaskItem, obj)) )
//						if ( FAILED(ACTION_ON_MESSAGE(_TaskRequestItem, obj)) )
//						if ( FAILED(ACTION_ON_MESSAGE(_TaskRequestAcceptItem, obj)) )
//						if ( FAILED(ACTION_ON_MESSAGE(_TaskRequestDeclineItem, obj)) )
//						if ( FAILED(ACTION_ON_MESSAGE(_TaskRequestUpdateItem, obj)) )
//							hr = hr; // else - do nothing
//					}
//				}
//			}
//		}
//	}
//	
//	return hr;
//}
//
//void __stdcall CAddin::OnClickSpamButton(IDispatch* /*Office::_CommandBarButton* */ Ctrl,VARIANT_BOOL * CancelDefault)
//{
//	ActionOnMessage(TrainAsSpam);
//}
//
//void __stdcall CAddin::OnClickHamButton(IDispatch* /*Office::_CommandBarButton* */ Ctrl,VARIANT_BOOL * CancelDefault)
//{
//	ActionOnMessage(RemoveFromSpam);
//}

void __stdcall CAddin::OnOptionsAddPages(IDispatch *Ctrl)
{
	CComQIPtr<Outlook::PropertyPages> spPages(Ctrl);
	ATLASSERT(spPages);

	if(g_root)
	{
		CGuiLoader &GUILdr = CGuiLoader::Instance();
		CRootItem *pRoot = GUILdr.GetRoot();
		if(!pRoot)
		{
			CGuiRoot *pGUIRoot = new CGuiRoot;
			if((pRoot = GUILdr.CreateAndInitRoot(g_root, 0, pGUIRoot)) == NULL)
				delete pGUIRoot;
		}
		if(pRoot)
		{
			pRoot->m_nFlags |= STYLE_DESTROYSINK;
			
			CComVariant varProgId(OLESTR("OutlookAsPlg.PageAntivirus"));
#if _BASEGUI_VER >= 0x0200
			std::wstring wsTitle =  pRoot->GetString(TOR_cStrObj, PROFILE_LOCALIZE, "OutlookPlugun.Antivirus", "@");
			remove_quotes(wsTitle);
			CComBSTR bstrTitle = wsTitle.c_str();
#else
			CComBSTR bstrTitle = pRoot->GetStringBufW(PROFILE_LOCALIZE, "OutlookPlugun.Antivirus", "dlg_caption");
#endif
			HRESULT hr = spPages->Add((_variant_t)varProgId, (_bstr_t)bstrTitle);
			if(FAILED(hr))
				ATLTRACE("\nFailed to add property page");
		}
	}
}

//void RefreshCommandBar(Office::CommandBar* cb)
//{
//	cb->PutEnabled( !cb->GetEnabled() );
//	cb->PutEnabled( !cb->GetEnabled() );
//}
//
//#define PUT_ENABLED(_ctrl, _enbl, _must_refresh)	\
//{	\
//	_must_refresh |= (_enbl != _ctrl->GetEnabled());	\
//	_ctrl->PutEnabled(_enbl);	\
//}
//		
//
//void CAddin::PutVisibleControls(VARIANT_BOOL pvarfVisible, bool bMustRefresh)
//{
//	CComPtr<Outlook::_Explorer> spExplorer; 	
//	HRESULT hr = m_spApp->ActiveExplorer(&spExplorer);
//	if( FAILED(hr) || !spExplorer )
//		return;
//	m_spSpamButton->PutVisible(pvarfVisible);
//	m_spHamButton->PutVisible(pvarfVisible);
//	m_spSpamMenu->PutVisible(pvarfVisible);
//	m_spHamMenu->PutVisible(pvarfVisible);
//	m_spAntispamBar->PutVisible(pvarfVisible);
//	if ( bMustRefresh )
//		RefreshCommandBar(m_spAntispamBar);
//}
//
//
//void __stdcall CAddin::OnSelectionChange()
//{
//	HRESULT hr = S_OK;
//	CComPtr<Outlook::_Explorer> spExplorer = m_explorer; 	
//	//hr = m_spApp->ActiveExplorer(&spExplorer);
//	if( SUCCEEDED(hr) ) 
//	{
//		CComPtr<Outlook::MAPIFolder> spFolder;
//		spExplorer->get_CurrentFolder(&spFolder);
//		if ( SUCCEEDED(hr) && spFolder )
//		{
//			CComBSTR bszFolderName;
//			hr = spFolder->get_Name(&bszFolderName);
//			if ( SUCCEEDED(hr) ) 
//			{
//				bool bMustRefresh = false;
//				CAntispamOUSupportClass* pAS = CAntispamOUSupportClass::GetInstance(_Module.GetModuleInstance());
//				if ( pAS )
//				{
//					AntispamPluginSettings_t AntispamPluginSettings;
//					if ( SUCCEEDED(hr= pAS->GetSettings(&AntispamPluginSettings)) )
//					{
//						{// Дизаблим кнопки, если не выделено ни одного сообщения
//							CComPtr<Outlook::Selection> spSelection; 	
//							hr = spExplorer->get_Selection(&spSelection);
//							if ( SUCCEEDED(hr) && spSelection )
//							{
//								long lItemCount = 0;
//								hr = spSelection->get_Count(&lItemCount);
//								if ( SUCCEEDED(hr) && !lItemCount )
//								{
//									PUT_ENABLED(m_spSpamButton, VARIANT_FALSE, bMustRefresh);
//									PUT_ENABLED(m_spSpamMenu,   VARIANT_FALSE, bMustRefresh);
//									PUT_ENABLED(m_spHamButton,  VARIANT_FALSE, bMustRefresh);
//									PUT_ENABLED(m_spHamMenu,    VARIANT_FALSE, bMustRefresh);
//									if ( pAS->IsAntispamEngineEnabled() )
//										PutVisibleControls(VARIANT_TRUE, bMustRefresh);
//									else
//										PutVisibleControls(VARIANT_FALSE);
//									return;
//								}
//							}
//						}
//						if ( bszFolderName == (tCHAR*)(AntispamPluginSettings.CertainSpamRule.FolderForAction.szName.c_str(cCP_ANSI)) )
//						{
//							PUT_ENABLED(m_spSpamButton, VARIANT_FALSE, bMustRefresh);
//							PUT_ENABLED(m_spSpamMenu,   VARIANT_FALSE, bMustRefresh);
//							PUT_ENABLED(m_spHamButton,  VARIANT_TRUE,  bMustRefresh);
//							PUT_ENABLED(m_spHamMenu,    VARIANT_TRUE,  bMustRefresh);
//						}
//						else
//						if ( bszFolderName == (tCHAR*)(AntispamPluginSettings.PossibleSpamRule.FolderForAction.szName.c_str(cCP_ANSI)) )
//						{
//							PUT_ENABLED(m_spSpamButton, VARIANT_TRUE,  bMustRefresh);
//							PUT_ENABLED(m_spSpamMenu,   VARIANT_TRUE,  bMustRefresh);
//							PUT_ENABLED(m_spHamButton,  VARIANT_TRUE,  bMustRefresh);
//							PUT_ENABLED(m_spHamMenu,    VARIANT_TRUE,  bMustRefresh);
//						}
//						else
//						{
//							PUT_ENABLED(m_spSpamButton, VARIANT_TRUE,  bMustRefresh);
//							PUT_ENABLED(m_spSpamMenu,   VARIANT_TRUE,  bMustRefresh);
//							PUT_ENABLED(m_spHamButton,  VARIANT_FALSE, bMustRefresh);
//							PUT_ENABLED(m_spHamMenu,    VARIANT_FALSE, bMustRefresh);
//						}
//						if ( pAS->IsAntispamEngineEnabled() )
//							PutVisibleControls(VARIANT_TRUE, bMustRefresh);
//						else
//							PutVisibleControls(VARIANT_FALSE);
//					}
//					else
//						PutVisibleControls(VARIANT_FALSE);
//				}
//				else
//					PutVisibleControls(VARIANT_FALSE);
//			}
//		}
//	}
//	if ( FAILED(hr) ) 
//	{
//		m_spSpamButton->PutEnabled(VARIANT_FALSE);
//		m_spSpamMenu->PutEnabled(VARIANT_FALSE);
//		m_spHamButton->PutEnabled(VARIANT_FALSE);
//		m_spHamMenu->PutEnabled(VARIANT_FALSE);
//		RefreshCommandBar(m_spAntispamBar);
//		return;
//	}
//}

HRESULT CAddin::OnConnection(IDispatch * Application, ext_ConnectMode ConnectMode, IDispatch * AddInInst, SAFEARRAY * * custom)
{
	HRESULT hr = S_OK;
//
//	CComPtr < Office::_CommandBars> spCmdBars; 
//	CComPtr < Office::CommandBar> spCmdBar;
//
	// QI() for _Application
	CComQIPtr <Outlook::_Application> spApp(Application); 
	ATLASSERT(spApp);
	// get the CommandBars interface that represents Outlook's 	
	//toolbars & menu 
	//items	
	m_spApp = spApp;
//
//	CComPtr<Outlook::_Explorer> spExplorer; 	
//	hr = spApp->ActiveExplorer(&spExplorer);
//	if( FAILED(hr) || !spExplorer )
//		return hr;
//	
//	ATLASSERT(spExplorer);
//	m_explorer = spExplorer;
//	hr = spExplorer->get_CommandBars(&spCmdBars);
//	if( FAILED(hr) || !spCmdBars )
//		return hr;
//
//	ATLASSERT(spCmdBars);
//	// now we add a new toolband to Outlook
//	// to which we'll add 2 buttons
//	CComVariant vName("Kaspersky Antispam");
//	CComPtr <Office::CommandBar> spNewCmdBar;
//	
//	// position it below all toolbands
//	//MsoBarPosition::msoBarTop = 1
//	CComVariant vPos(1); 
//
//	CComVariant vTemp(VARIANT_TRUE); // menu is temporary		
//	CComVariant vEmpty(DISP_E_PARAMNOTFOUND, VT_ERROR);			
//	//Add a new toolband through Add method
//	// vMenuTemp holds an unspecified parameter
//	//spNewCmdBar points to the newly created toolband
//	spNewCmdBar = spCmdBars->Add(vName, vPos, vEmpty, vTemp);
//	spNewCmdBar->Protection = Office::msoBarNoCustomize;
// 
//	//now get the toolband's CommandBarControls
//	CComPtr < Office::CommandBarControls> spBarControls;
//	spBarControls = spNewCmdBar->GetControls();
//	ATLASSERT(spBarControls);
//	
//	//MsoControlType::msoControlButton = 1
//	CComVariant vToolBarType(1);
//	//show the toolbar?
//	CComVariant vShow(VARIANT_TRUE);
//	
//	CComPtr < Office::CommandBarControl> spNewBar; 
//	CComPtr < Office::CommandBarControl> spNewBar2; 
//			
//	// add first button
//	spNewBar = spBarControls->Add(vToolBarType, vEmpty, vEmpty, vEmpty, vShow); 
//	ATLASSERT(spNewBar);
//	// add 2nd button
//	spNewBar2 = spBarControls->Add(vToolBarType, vEmpty, vEmpty, vEmpty, vShow);
//	ATLASSERT(spNewBar2);
//			
//	_bstr_t bstrSpamCaption(OLESTR("Spam"));
//	_bstr_t bstrSpamTipText(OLESTR("Check selected messages as Spam"));
//	_bstr_t bstrHamCaption(OLESTR("Not Spam"));
//	_bstr_t bstrHamTipText(OLESTR("Check selected messages as Not Spam"));
//
//	// get CommandBarButton interface for each toolbar button
//	// so we can specify button styles and stuff
//	// each button displays a bitmap and caption next to it
//	CComQIPtr < Office::_CommandBarButton> spSpamButton(spNewBar);
//	CComQIPtr < Office::_CommandBarButton> spHamButton(spNewBar2);
//			
//	ATLASSERT(spSpamButton);
//	ATLASSERT(spHamButton);
//			
//	// to set a bitmap to a button, load a 32x32 bitmap
//	// and copy it to clipboard. Call CommandBarButton's PasteFace()
//	// to copy the bitmap to the button face. to use
//	// Outlook's set of predefined bitmap, set button's FaceId to 	//the
//	// button whose bitmap you want to use
//	HBITMAP hBmp =(HBITMAP)::LoadImage(_Module.GetResourceInstance(),
//	MAKEINTRESOURCE(IDB_BITMAP1),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS);
//
//	// put bitmap into Clipboard
//	::OpenClipboard(NULL);
//	::EmptyClipboard();
//	::SetClipboardData(CF_BITMAP, (HANDLE)hBmp);
//	::CloseClipboard();
//	::DeleteObject(hBmp);		
//	// set style before setting bitmap
//	spSpamButton->PutStyle(Office::msoButtonIconAndCaption);
//	spSpamButton->PutOLEUsage(Office::msoControlOLEUsageNeither);
//			
//	hr = spSpamButton->PasteFace();
//	if (FAILED(hr))
//		return hr;
//
//	spSpamButton->PutCaption(bstrSpamCaption); 
//	spSpamButton->PutEnabled(VARIANT_TRUE);
//	spSpamButton->PutTooltipText(bstrSpamTipText); 
//	spSpamButton->PutTag(bstrSpamCaption); 
//	spSpamButton->PutVisible(VARIANT_TRUE); 
//	
//	//show the toolband
//	//spNewCmdBar->PutVisible(VARIANT_TRUE); 
//			
//	spHamButton->PutStyle(Office::msoButtonIconAndCaption);
//	spHamButton->PutOLEUsage(Office::msoControlOLEUsageNeither);
//	
//	//specify predefined bitmap
//	spHamButton->PutFaceId(1760);  	
//	spHamButton->PutCaption(bstrHamCaption); 
//	spHamButton->PutEnabled(VARIANT_TRUE);
//	spHamButton->PutTooltipText(bstrHamTipText); 
//	spHamButton->PutTag(bstrHamCaption);
//	spHamButton->PutVisible(VARIANT_TRUE);
//
//
//	m_spSpamButton = spSpamButton;
//	m_spHamButton = spHamButton;
//
//	CComPtr < Office::CommandBarControls> spCmdCtrls;
//	CComPtr < Office::CommandBarControls> spCmdBarCtrls; 
//	CComPtr < Office::CommandBarPopup> spCmdPopup;
//	CComPtr < Office::CommandBarControl> spCmdCtrl;
//
//	// get CommandBar that is Outlook's main menu
//	hr = spCmdBars->get_ActiveMenuBar(&spCmdBar); 
//	if (FAILED(hr))
//		return hr;
//	// get menu as CommandBarControls 
//	spCmdCtrls = spCmdBar->GetControls(); 
//	ATLASSERT(spCmdCtrls);
//	
//	spCmdBar->Protection = Office::msoBarNoCustomize;
//
//	// we want to add a menu entry to Outlook's 6th(Tools) menu 	//item
//	CComVariant vItem(6);
//	spCmdCtrl= spCmdCtrls->GetItem(vItem);
//	ATLASSERT(spCmdCtrl);
//		
//	CComPtr < IDispatch > spDisp;
//	spDisp = spCmdCtrl->GetControl(); 
//		
//	// a CommandBarPopup interface is the actual menu item
//	CComQIPtr < Office::CommandBarPopup> ppCmdPopup(spDisp);  
//	ATLASSERT(ppCmdPopup);
//			
//	spCmdBarCtrls = ppCmdPopup->GetControls();
//	ATLASSERT(spCmdBarCtrls);
//		
//	CComVariant vMenuType(1); // type of control - menu
//	CComVariant vMenuPos(6);  
//	CComVariant vMenuEmpty(DISP_E_PARAMNOTFOUND, VT_ERROR);
//	CComVariant vMenuShow(VARIANT_TRUE); // menu should be visible
//	CComVariant vMenuTemp(VARIANT_TRUE); // menu is temporary		
//	
//		
//	CComPtr < Office::CommandBarControl> spSpamMenu;
//	CComPtr < Office::CommandBarControl> spHamMenu;
//	// now create the actual menu item and add it
//	spSpamMenu = spCmdBarCtrls->Add(vMenuType, vMenuEmpty, vMenuEmpty, vMenuEmpty, vMenuTemp); 
//	spHamMenu = spCmdBarCtrls->Add(vMenuType, vMenuEmpty, vMenuEmpty, vMenuEmpty, vMenuTemp); 
//	ATLASSERT(spSpamMenu);
//	ATLASSERT(spHamMenu);
//			
//	spSpamMenu->PutEnabled(VARIANT_TRUE);
//	
//	spSpamMenu->PutOLEUsage(Office::msoControlOLEUsageNeither);
//	spHamMenu->PutOLEUsage(Office::msoControlOLEUsageNeither);
//	
//	
//	//we'd like our new menu item to look cool and display
//	// an icon. Get menu item  as a CommandBarButton
//	CComQIPtr < Office::_CommandBarButton> spSpamMenuButton(spSpamMenu);
//	CComQIPtr < Office::_CommandBarButton> spHamMenuButton(spHamMenu);
//	ATLASSERT(spSpamMenuButton);
//	ATLASSERT(spHamMenuButton);
//	spSpamMenuButton->PutStyle(Office::msoButtonIconAndCaption);
//	spHamMenuButton->PutStyle(Office::msoButtonIconAndCaption);
//	spSpamMenuButton->PutCaption(bstrSpamCaption);
//	spHamMenuButton->PutCaption(bstrHamCaption);
//	spSpamMenuButton->PutOLEUsage(Office::msoControlOLEUsageNeither);
//	spHamMenuButton->PutOLEUsage(Office::msoControlOLEUsageNeither);
//	spSpamMenuButton->PutShortcutText("Alt+Del");
//	spHamMenuButton->PutShortcutText("Alt+Ins");
//
//	// we want to use the same toolbar bitmap for menuitem too.
//	// we grab the CommandBarButton interface so we can add
//	// a bitmap to it through PasteFace().
//	spSpamMenuButton->PasteFace(); 
//	spHamMenuButton->PasteFace(); 
//	// show the menu		
//	m_spSpamMenu = spSpamMenuButton;
//	m_spHamMenu = spHamMenuButton;
//	m_spAntispamBar = spNewCmdBar;
//
//	m_dwCookie_SpamButton = AdviseSelectionChange(m_spSpamButton, (IUnknown*)(SpamButtonEvents*)this, &__uuidof(Office::_CommandBarButtonEvents));
//	m_dwCookie_HamButton = AdviseSelectionChange(m_spHamButton,   (IUnknown*)(HamButtonEvents*)this, &__uuidof(Office::_CommandBarButtonEvents));
//	m_dwCookie_SpamMenu = AdviseSelectionChange(m_spSpamMenu,     (IUnknown*)(SpamMenuEvents*)this, &__uuidof(Office::_CommandBarButtonEvents));
//	m_dwCookie_HamMenu = AdviseSelectionChange(m_spHamMenu,       (IUnknown*)(HamMenuEvents*)this, &__uuidof(Office::_CommandBarButtonEvents));
	m_dwCookie_OptionsPage = AdviseSelectionChange(m_spApp,       (IUnknown*)(AppEvents*)this, &__uuidof(Outlook::ApplicationEvents));
//	m_dwCookie_SelectionChange = AdviseSelectionChange(m_explorer,(IUnknown*)(ExplEvents*)this, &__uuidof(Outlook::ExplorerEvents));
//	m_dwCookie_NewMail = AdviseSelectionChange(m_spApp,           (IUnknown*)(NewMailEvents*)this, &__uuidof(Outlook::ApplicationEvents));
//
//	OnSelectionChange();
//	
//	m_hCheckThread = new CCheckThread(this);
//	if ( m_hCheckThread )
//		m_hCheckThread->Start();
//
//	g_pKeyboardHook = new CKeyboardHook();
//	g_addin = this;

	bConnected = true;
	return S_OK;

}
HRESULT CAddin::OnDisconnection(ext_DisconnectMode RemoveMode, SAFEARRAY * * custom)
{
	if(bConnected)
	{
//		if ( g_pKeyboardHook )
//		{
//			delete g_pKeyboardHook;
//			g_pKeyboardHook = NULL;
//		}
//		if ( m_hCheckThread )
//		{
//			m_hCheckThread->TerminateAndWait();
//			delete m_hCheckThread;
//			m_hCheckThread = NULL;
//		}
//		UnadviseSelectionChange(m_spApp,        __uuidof(Outlook::ApplicationEvents),      m_dwCookie_NewMail);
//		UnadviseSelectionChange(m_explorer,     __uuidof(Outlook::ExplorerEvents),         m_dwCookie_SelectionChange);
		UnadviseSelectionChange(m_spApp,        __uuidof(Outlook::ApplicationEvents),      m_dwCookie_OptionsPage);
//		UnadviseSelectionChange(m_spSpamMenu,   __uuidof(Office::_CommandBarButtonEvents), m_dwCookie_SpamMenu);
//		UnadviseSelectionChange(m_spHamMenu,    __uuidof(Office::_CommandBarButtonEvents), m_dwCookie_HamMenu);
//		UnadviseSelectionChange(m_spHamButton,  __uuidof(Office::_CommandBarButtonEvents), m_dwCookie_HamButton);
//		UnadviseSelectionChange(m_spSpamButton, __uuidof(Office::_CommandBarButtonEvents), m_dwCookie_SpamButton);
//		g_addin = NULL;
		bConnected = false;
	}
//	CAntispamOUSupportClass::ReleaseInstance();
//

	CGuiLoader::Instance().Unload();

	return S_OK;
}

//void __stdcall CAddin::OnNewMail(/*IDispatch* Item, VARIANT_BOOL* Cancel*/)
//{
//	CAntispamOUSupportClass* pAS = CAntispamOUSupportClass::GetInstance(_Module.GetModuleInstance());
//	pAS->ScanNewMessages();
//}
//
