
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
#include "AntispamOUSupportClass.h"
#include <ProductCore/GuiLoader.h>
#include "explorerhandler.h"
#include "macros.h"
#include <version/ver_product.h>
#include "trace.h"

CAddin* g_addin = NULL;
HHOOK g_hook = NULL;
extern _bstr_t vAntispamBarName;
extern _bstr_t vSpamBtnName;
extern _bstr_t vHamBtnName;

volatile long CAddin::CCheckThread::m_AntispamEnabled = 0;

DWORD WINAPI CAddin::CCheckThread::ThreadProcRun(LPVOID lpThreadParameter)
{
	CAddin::CCheckThread *pThis = (CAddin::CCheckThread *)lpThreadParameter;

	CAntispamOUSupportClass* pAS = 
		CAntispamOUSupportClass::GetInstance(_Module.GetModuleInstance());
	if(!pAS)
		return -1;

	while(WaitForSingleObject(pThis->m_hStopEvent, 1000) == WAIT_TIMEOUT)
	{
		InterlockedExchange(&m_AntispamEnabled, pAS->IsAntispamEngineEnabled());
	}

	return 0;
}

VOID CALLBACK CAddin::CCheckThread::TimerProcRun(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	static bool bAntispamEnabled_Prev = false;
	static bool bAntispamEnabled_Current = false;
	static bool bSettingsShowFailed = false;
	CAntispamOUSupportClass* pAS = 
		CAntispamOUSupportClass::GetInstance(_Module.GetModuleInstance());
	if ( pAS )
	{
		bAntispamEnabled_Current = m_AntispamEnabled != 0;
		if ( bAntispamEnabled_Prev != bAntispamEnabled_Current )
		{
			bAntispamEnabled_Prev = bAntispamEnabled_Current;
			if ( bAntispamEnabled_Current )
			{
				g_addin->PutVisibleControls(VARIANT_TRUE);
				bSettingsShowFailed = false;
				bSettingsShowFailed = !pAS->CheckPluginSettings();
				pAS->ScanNewMessages();
			}
			else
				g_addin->PutVisibleControls(VARIANT_FALSE);
		}
		if(bSettingsShowFailed)
		{
			bSettingsShowFailed = false;
			bSettingsShowFailed = !pAS->CheckPluginSettings();
		}
	}
}


//! Класс, обеспещивающий клавиатурные shortcut'ы
class CKeyboardHook
{
public:
	CKeyboardHook()
	{
		if ( !g_hook )
		{
			g_hook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, _Module.GetModuleInstance(), ::GetCurrentThreadId());
			if ( !g_hook )
			{
				DWORD dwErr = GetLastError();
			}
		}
	}
	~CKeyboardHook() 
	{ 
		if ( g_hook ) 
			UnhookWindowsHookEx(g_hook); 
	};
	static LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
	{
		if ( 
			code == HC_NOREMOVE &&
			lParam < 0 && 
			GetKeyState(/*VK_CONTROL*/VK_MENU) < 0 &&
			GetKeyState(VK_SHIFT) < 0
			)
		{
			switch ( wParam ) 
			{
			case VK_DELETE:
				g_addin->OnClickSpamButton(0,0);
				break;
			case VK_INSERT:
				g_addin->OnClickHamButton(0,0);
				break;
			default:
				return CallNextHookEx( g_hook, code, wParam, lParam );
			}
		}
		else
			return CallNextHookEx( g_hook, code, wParam, lParam );
		return 1;
	}
};
CKeyboardHook* g_pKeyboardHook = NULL;

class CWaitCur
{
public:
	CWaitCur()  { m_hcurPrev = ::SetCursor(::LoadCursor(NULL, IDC_WAIT)); }
	~CWaitCur() { ::SetCursor(m_hcurPrev); }
protected:
	HCURSOR m_hcurPrev;
};

template<typename _type>
HRESULT DoActionOnMessage(IDispatch* _obj, Action_t action )
{
	{
		MSG msg;
		while (PeekMessage (&msg, NULL, WM_PAINT, WM_PAINT, PM_REMOVE))
		{
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	}
	CComPtr<_type> spMailItem = NULL;
	HRESULT hr = _obj->QueryInterface(__uuidof(_type), (void**)&spMailItem);
	if ( SUCCEEDED(hr) && spMailItem )
	{
		LPMESSAGE pMessage = NULL;
		_TRY_BEGIN hr = spMailItem->get_MAPIOBJECT((IUnknown**)&pMessage); _CATCH_ALL_TRACE _CATCH_END
			if ( SUCCEEDED(hr) && pMessage )
			{
				CAntispamOUSupportClass* pAS =
					CAntispamOUSupportClass::GetInstance(_Module.GetModuleInstance());
				if ( pAS )
					hr = pAS->ActionOnMessage(pMessage, action);
				pMessage->Release();
				pMessage = NULL;
			}
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CAddin
_ATL_FUNC_INFO OnClickButtonInfo ={CC_STDCALL,VT_EMPTY,2,{VT_DISPATCH,VT_BYREF | VT_BOOL}};
_ATL_FUNC_INFO OnOptionsAddPagesInfo={CC_STDCALL,VT_EMPTY,1,{VT_DISPATCH}};
_ATL_FUNC_INFO OnFolderChangeInfo ={CC_STDCALL,VT_EMPTY,0};
_ATL_FUNC_INFO OnSelectionChangeInfo ={CC_STDCALL,VT_EMPTY,0};
_ATL_FUNC_INFO OnExplCloseInfo ={CC_STDCALL,VT_EMPTY,0};
_ATL_FUNC_INFO OnExplNewInfo ={CC_STDCALL,VT_EMPTY,1,{VT_DISPATCH}};
_ATL_FUNC_INFO OnNewMailInfo ={CC_STDCALL,VT_EMPTY,0};

STDMETHODIMP CAddin::InterfaceSupportsErrorInfo(REFIID riid)
{
	if (IsEqualGUID(IID_IAddin,riid))
		return S_OK;
	return S_FALSE;
}

BOOL GetCheckOnRead()
{
	HKEY key; 
	LONG res = RegOpenKeyEx(
		HKEY_CURRENT_USER,
		VER_PRODUCT_REGISTRY_PATH "\\mcou",
		0,
		KEY_READ,
		&key
		);
	if ( res == ERROR_SUCCESS )
	{
		DWORD type;
		DWORD size = sizeof(DWORD);
		BOOL bValue = TRUE;
		res = RegQueryValueEx(
			key,
			"CheckOnRead",
			NULL,
			&type,
			(BYTE*)&bValue,
			&size
			);
		if ( res == ERROR_SUCCESS )
		{
			RegCloseKey(key);
			return bValue;
		}
		RegCloseKey(key);
	}
	return TRUE;
}

void SetCheckOnRead(BOOL bCheckOnRead)
{
	HKEY key; 
	LONG res = RegCreateKeyEx(
		HKEY_CURRENT_USER,
		VER_PRODUCT_REGISTRY_PATH "\\mcou",
		0, 0, 0,
		KEY_WRITE,
		0,
		&key,
		0
		);
	if ( res == ERROR_SUCCESS )
	{
		DWORD size = sizeof(DWORD);
		DWORD bValue = bCheckOnRead;
		res = RegSetValueEx(
			key,
			"CheckOnRead",
			NULL,
			REG_DWORD,
			(BYTE*)&bValue,
			sizeof(DWORD)
			);
		RegCloseKey(key);
	}
}

const char *GetActionStr(AntispamOUSupport::Action_t action)
{
	switch(action)
	{
	case AntispamOUSupport::TrainAsSpam:
		return "TrainAsSpam";
	case AntispamOUSupport::TrainAsHam:
		return "TrainAsHam";
	case AntispamOUSupport::RemoveFromSpam:
		return "RemoveFromSpam";
	case AntispamOUSupport::RemoveFromHam:
		return "RemoveFromHam";
	case AntispamOUSupport::Process:
		return "Process";
	}
	return "Unknown";
}

HRESULT CAddin::ActionOnMessage(Action_t action)
{
	FUNC_TRACE(("CAddin::ActionOnMessage", action));
	HRESULT hr = S_OK;
	CComPtr<Outlook::_Explorer> spExplorer; 	
	_TRY_BEGIN
		hr = m_spApp->ActiveExplorer(&spExplorer);
	_CATCH_ALL_TRACE
		_CATCH_END
	if(SUCCEEDED(hr) && spExplorer)
	{
		CWaitCur wc;
		BOOL bCheckOnRead = GetCheckOnRead();
		SetCheckOnRead(FALSE);
		CComPtr<Outlook::Selection> spSelection; 	
		_TRY_BEGIN
			hr = spExplorer->get_Selection(&spSelection);
		_CATCH_ALL_TRACE
		_CATCH_END
		if(SUCCEEDED(hr) && spSelection)
		{
			long lItemCount = 0;
			_TRY_BEGIN
				hr = spSelection->get_Count(&lItemCount);
			_CATCH_ALL_TRACE
			_CATCH_END
			if(SUCCEEDED(hr))
			{
				PR_TRACE((0, prtIMPORTANT, "Performing action '%s' on %d messages", GetActionStr(action), lItemCount));
				bool bIsMassTraining =
					(action == AntispamOUSupport::TrainAsHam || action == AntispamOUSupport::TrainAsSpam) &&
					lItemCount > 1;
				CAntispamOUSupportClass* pAS = CAntispamOUSupportClass::GetInstance(_Module.GetModuleInstance());
				if(pAS)
				{
					cERROR err = errOK;
					if(bIsMassTraining)
						err = pAS->MassTrainingStart();
					if(PR_SUCC(err))
					{
						for(long iCounter = 1; iCounter <= lItemCount; ++iCounter)
						{
							PR_TRACE((0, prtIMPORTANT, "Processing message %d of %d", iCounter, lItemCount));
							CComPtr<IDispatch> obj = NULL;
							VARIANT v;
							v.vt = VT_I4;
							v.intVal = iCounter;
							_TRY_BEGIN
								hr = spSelection->Item(v, &obj);
							_CATCH_ALL_TRACE
							_CATCH_END
							if(SUCCEEDED(hr) && obj)
							{
								// Check messages
								if(FAILED(DoActionOnMessage<_MailItem>(obj, action)) &&
									FAILED(DoActionOnMessage<_ReportItem>(obj, action)) &&
									FAILED(DoActionOnMessage<_AppointmentItem>(obj, action)) &&
									FAILED(DoActionOnMessage<_MeetingItem>(obj, action)) &&
									FAILED(DoActionOnMessage<_DocumentItem>(obj, action)) &&
									FAILED(DoActionOnMessage<_JournalItem>(obj, action)) &&
									FAILED(DoActionOnMessage<_PostItem>(obj, action)) &&
									FAILED(DoActionOnMessage<_RemoteItem>(obj, action)) &&
									FAILED(DoActionOnMessage<_TaskItem>(obj, action)) &&
									FAILED(DoActionOnMessage<_TaskRequestItem>(obj, action)) &&
									FAILED(DoActionOnMessage<_TaskRequestAcceptItem>(obj, action)) &&
									FAILED(DoActionOnMessage<_TaskRequestDeclineItem>(obj, action)) &&
									FAILED(DoActionOnMessage<_TaskRequestUpdateItem>(obj, action)))
								{
									hr = hr; // else - do nothing
								}
							}
						}
						if ( bIsMassTraining )
							pAS->MassTrainingFinish();
						PR_TRACE((0, prtIMPORTANT, "Processing done"));
					}
				}
			}
			else
			{
				_TRACEHR(hr, "Getting count");
			}
		}
		else
		{
			_TRACEHR(hr, "Getting selection");
		}

		SetCheckOnRead(bCheckOnRead);
	}
	else
	{
		_TRACEHR(hr, "Getting active explorer");
	}
	
	return hr;
}

void __stdcall CAddin::OnClickSpamButton(IDispatch* /*Office::_CommandBarButton* */ Ctrl,VARIANT_BOOL * CancelDefault)
{
	FUNC_TRACE(("CAddin::OnClickSpamButton"));
	_TRY_BEGIN
		ActionOnMessage(TrainAsSpam);
	_CATCH_ALL_TRACE
	_CATCH_END
}

void __stdcall CAddin::OnClickHamButton(IDispatch* /*Office::_CommandBarButton* */ Ctrl,VARIANT_BOOL * CancelDefault)
{
	FUNC_TRACE(("CAddin::OnClickHamButton"));
	_TRY_BEGIN
		ActionOnMessage(TrainAsHam);
	_CATCH_ALL_TRACE
	_CATCH_END
}

void __stdcall CAddin::OnOptionsAddPages(IDispatch *Ctrl)
{
	FUNC_TRACE(("CAddin::OnOptionsAddPages"));
	_TRY_BEGIN
		CComQIPtr<Outlook::PropertyPages> spPages(Ctrl);
		ATLASSERT(spPages);

		CRootItem *pRoot = CGuiLoader::Instance().GetRoot();
		if(pRoot)
		{
			CComVariant varProgId(OLESTR("OutlookAddin.PageAntispam"));
#if _BASEGUI_VER >= 0x0200
			std::wstring wsTitle =  pRoot->GetString(TOR_cStrObj, PROFILE_LOCALIZE, "OutlookPlugun.Antispam", "@");
			remove_quotes(wsTitle);
			CComBSTR bstrTitle = wsTitle.c_str();
#else
			CComBSTR bstrTitle = pRoot->GetStringBufW(PROFILE_LOCALIZE, "OutlookPlugun.Antispam", "dlg_caption");
#endif
			HRESULT hr = spPages->Add((_variant_t)varProgId,(_bstr_t)bstrTitle);
			if(FAILED(hr))
				_TRACEHR(hr, "Property page add");
		}
	_CATCH_ALL_TRACE
	_CATCH_END
}

void CAddin::PutVisibleControls(VARIANT_BOOL pvarfVisible)
{
	FUNC_TRACE(("CAddin::PutVisibleControls"));
	FOREACH_EXPLORER_BEGIN(m_spApp)
		PutVisibleControls(pvarfVisible, spCurrentExplorer);
	FOREACH_EXPLORER_END
}

void CAddin::PutVisibleControls(VARIANT_BOOL pvarfVisible, CComPtr<Outlook::_Explorer> spExplorer)
{
	FOREACH_BAR_BEGIN(spExplorer)
		PutVisibleControls(pvarfVisible, spCurrentBarControls);
		if ( spCurrentBar->GetName() == vAntispamBarName )
		{
			spCurrentBar->PutEnabled(pvarfVisible);
			if ( pvarfVisible )
				spCurrentBar->PutVisible(pvarfVisible);
			//PUT_ENABLED_CTRL(spCurrentBar, pvarfVisible);
		}
	FOREACH_BAR_END
	
	//////////////////////////////////////////////////////////////////////////
	//
	// Actions menu buttons
	//
	HRESULT hr = S_OK;
	CComPtr<Office::_CommandBars> spCmdBars;
	CComPtr<Office::CommandBar> spCmdBar;
	if(SUCCEEDED(hr = spExplorer->get_CommandBars(&spCmdBars)) && spCmdBars
		&& SUCCEEDED(hr = spCmdBars->get_ActiveMenuBar(&spCmdBar)) && spCmdBar)
	{
		CComPtr<Office::CommandBarControls> spMenuCtrls;
		CComPtr<Office::CommandBarControls> spActionsCtrls;
		_TRY_BEGIN
			spMenuCtrls = spCmdBar->GetControls();
		_CATCH_ALL_TRACE
		_CATCH_END
		FOREACH_CONTROL_BEGIN(spMenuCtrls, 1)
		{
			if ( spCurrentControl->GetId() == 30131 /* Actions */ )
			{
				CComPtr<IDispatch> spDisp;
				_TRY_BEGIN spDisp = spCurrentControl->GetControl(); _CATCH_ALL_TRACE _CATCH_END
				CComQIPtr<Office::CommandBarPopup> ppCmdPopup(spDisp);  
				_TRY_BEGIN spActionsCtrls = ppCmdPopup->GetControls(); _CATCH_ALL_TRACE _CATCH_END
				PutVisibleControls(pvarfVisible, spActionsCtrls);
			}
		}
		FOREACH_CONTROL_END
	}
	//
	// Actions menu buttons
	//
	//////////////////////////////////////////////////////////////////////////
}

void CAddin::PutVisibleControls(VARIANT_BOOL pvarfVisible, CComPtr<Office::CommandBar> spBar)
{
	CComVariant vTRUE    (VARIANT_TRUE);
	CComVariant vEmpty   (DISP_E_PARAMNOTFOUND, VT_ERROR);
	_variant_t  vSpamTag (vSpamBtnName); 
	_variant_t  vHamTag  (vHamBtnName); 
    HRESULT hr = S_OK;
	{
		CComPtr<Office::CommandBarControl> spBarControl; 
		_TRY_BEGIN
		if (SUCCEEDED(hr = spBar->raw_FindControl(vEmpty, vEmpty, vSpamTag, vtMissing, vTRUE, &spBarControl)) && spBarControl)
			spBarControl->PutVisible(pvarfVisible);
		_CATCH_ALL_TRACE   
		_CATCH_END
	}
	{
		_TRY_BEGIN
		CComPtr<Office::CommandBarControl> spBarControl; 
		if (SUCCEEDED(hr = spBar->raw_FindControl(vEmpty, vEmpty, vHamTag, vtMissing, vTRUE, &spBarControl)) && spBarControl)
			spBarControl->PutVisible(pvarfVisible);
		_CATCH_ALL_TRACE   
		_CATCH_END
	}
}

void CAddin::PutVisibleControls(VARIANT_BOOL pvarfVisible, CComPtr<Office::CommandBarControls> spControls)
{
	FOREACH_CONTROL_BEGIN(spControls, 1)
		if(spCurrentControl->GetTag() == vSpamBtnName || spCurrentControl->GetTag() == vHamBtnName)
			spCurrentControl->PutVisible(pvarfVisible);
	FOREACH_CONTROL_END
}


void __stdcall CAddin::OnNewExplorer(IDispatch* expl)
{
	FUNC_TRACE(("CAddin::OnNewExplorer"));
	if ( expl )
	{
		CComPtr<Outlook::_Explorer> spExplorer; 
		_TRY_BEGIN
			HRESULT hr = expl->QueryInterface(&spExplorer);
		_CATCH_ALL_TRACE
		_CATCH_END
		if ( spExplorer )
		{
			PR_TRACE((0, prtIMPORTANT, "Creating new CExplorerHandler"));
			CExplorerHandler* pExplorerHandler = new CExplorerHandler(this, spExplorer);
			if(pExplorerHandler)
			{
				pExplorerHandler->AddRef();
				pExplorerHandler->SinkEvents(spExplorer);
			}
		}
	}
}


HRESULT CAddin::OnConnection(IDispatch * Application, ext_ConnectMode ConnectMode, IDispatch * AddInInst, SAFEARRAY * * custom)
{
	FUNC_TRACE(("CAddin::OnConnection"));

	_TRY_BEGIN

		HRESULT hr = S_OK;
		SpamButtonAdvased = false;
		HamButtonAdvased = false;

		CAntispamOUSupportClass* pAS = CAntispamOUSupportClass::GetInstance(_Module.GetModuleInstance());
		if ( !pAS )
		{
			PR_TRACE((0, prtIMPORTANT, "CAddin::OnConnection => Cannot get CAntispamOUSupportClass"));
			return E_FAIL;
		}
		
		CRootItem* pGuiRoot = CGuiLoader::Instance().GetRoot();

		CComPtr<Office::_CommandBars> spCmdBars; 
		CComPtr<Office::CommandBar> spCmdBar;

		// QI() for _Application
		CComQIPtr<Outlook::_Application> spApp(Application); 
		m_spApp = spApp;

		CComPtr<Outlook::_Explorers> spExplorers;
		_TRY_BEGIN hr = spApp->get_Explorers(&spExplorers); _CATCH_ALL_TRACE _CATCH_END

		m_dwCookie_ExplNew = AdviseSelectionChange(spExplorers,       (IUnknown*)(ExplNew*)this, &__uuidof(Outlook::ExplorersEvents));
		m_dwCookie_OptionsPage = AdviseSelectionChange(m_spApp,       (IUnknown*)(AppEvents*)this, &__uuidof(Outlook::ApplicationEvents));
		m_dwCookie_NewMail = AdviseSelectionChange(m_spApp,           (IUnknown*)(NewMailEvents*)this, &__uuidof(Outlook::ApplicationEvents));

		g_pKeyboardHook = new CKeyboardHook();
		g_addin = this; 
		g_addin->AddRef();

		//	CComPtr<Outlook::_Explorer> spExplorer;
		//	_TRY_BEGIN hr = m_spApp->ActiveExplorer(&spExplorer); _CATCH_ALL_TRACE _CATCH_END
		//	OnNewExplorer(spExplorer);
		FOREACH_EXPLORER_BEGIN(m_spApp)
			OnNewExplorer(spCurrentExplorer);
		FOREACH_EXPLORER_END


		m_hCheckThread = new CCheckThread(this);
		if ( m_hCheckThread )
			m_hCheckThread->Start();

		bConnected = true;
		
		return S_OK;

	_CATCH_ALL_TRACE
	_CATCH_END
	return E_FAIL;
}

HRESULT CAddin::OnDisconnection(ext_DisconnectMode RemoveMode, SAFEARRAY * * custom)
{
	FUNC_TRACE(("CAddin::OnDisconnection"));
	
	_TRY_BEGIN

		if(bConnected)
		{
			if ( g_pKeyboardHook )
			{
				delete g_pKeyboardHook;
				g_pKeyboardHook = NULL;
			}
			if ( m_hCheckThread )
			{
				m_hCheckThread->TerminateAndWait();
				delete m_hCheckThread;
				m_hCheckThread = NULL;
			}
			CComPtr<Outlook::_Explorers> spExplorers;
			_TRY_BEGIN HRESULT hr = m_spApp->get_Explorers(&spExplorers);  _CATCH_ALL_TRACE _CATCH_END
			UnadviseSelectionChange(spExplorers,    __uuidof(Outlook::ExplorersEvents),        m_dwCookie_ExplNew);
			UnadviseSelectionChange(m_spApp,        __uuidof(Outlook::ApplicationEvents),      m_dwCookie_NewMail);
			UnadviseSelectionChange(m_spApp,        __uuidof(Outlook::ApplicationEvents),      m_dwCookie_OptionsPage);
			if ( g_addin )
			{
				g_addin->Release();
				g_addin = NULL;
			}
			bConnected = false;
		}
		CGuiLoader::Instance().Unload();
		
		CAntispamOUSupportClass::ReleaseInstance();
		
		return S_OK;
	
	_CATCH_ALL_TRACE
	_CATCH_END
	
	return E_FAIL;
}

void __stdcall CAddin::OnNewMail(/*IDispatch* Item, VARIANT_BOOL* Cancel*/)
{
	FUNC_TRACE(("CAddin::OnNewMail"));
	_TRY_BEGIN
		CAntispamOUSupportClass* pAS = CAntispamOUSupportClass::GetInstance(_Module.GetModuleInstance());
		pAS->ScanNewMessages();
	_CATCH_ALL_TRACE
	_CATCH_END
}

