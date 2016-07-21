
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

// Addin.h : Declaration of the CAddin

#ifndef __ADDIN_H_
#define __ADDIN_H_
#include "stdafx.h"
#include "resource.h"       // main symbols
#import "dll\MSADDNDR.DLL" raw_interfaces_only, raw_native_types, no_namespace, named_guids 

/////////////////////////////////////////////////////////////////////////////
// CAddin
extern _ATL_FUNC_INFO OnClickButtonInfo;
extern _ATL_FUNC_INFO OnOptionsAddPagesInfo;
extern _ATL_FUNC_INFO OnFolderChangeInfo;
extern _ATL_FUNC_INFO OnSelectionChangeInfo;
extern _ATL_FUNC_INFO OnExplCloseInfo;
extern _ATL_FUNC_INFO OnExplNewInfo;
extern _ATL_FUNC_INFO OnNewMailInfo;

class CAddin;
typedef IDispEventSimpleImpl</*nID =*/ 1,CAddin, &__uuidof(Office::_CommandBarButtonEvents)> SpamButtonEvents;
typedef IDispEventSimpleImpl</*nID =*/ 2,CAddin, &__uuidof(Office::_CommandBarButtonEvents)> HamButtonEvents;
typedef IDispEventSimpleImpl</*nID =*/ 3,CAddin, &__uuidof(Office::_CommandBarButtonEvents)> SpamMenuEvents;
typedef IDispEventSimpleImpl</*nID =*/ 4,CAddin, &__uuidof(Office::_CommandBarButtonEvents)> HamMenuEvents;
typedef IDispEventSimpleImpl</*nID =*/ 5,CAddin, &__uuidof(Outlook::ApplicationEvents)> AppEvents;
typedef IDispEventSimpleImpl</*nID =*/ 6,CAddin, &__uuidof(Outlook::ExplorerEvents)> ExplEvents;
typedef IDispEventSimpleImpl</*nID =*/ 7,CAddin, &__uuidof(Outlook::ApplicationEvents)> NewMailEvents;
typedef IDispEventSimpleImpl</*nID =*/ 8,CAddin, &__uuidof(Outlook::ExplorerEvents)> SelectionEvents;
typedef IDispEventSimpleImpl</*nID =*/ 9,CAddin, &__uuidof(Outlook::ExplorerEvents)> ExplClose;
typedef IDispEventSimpleImpl</*nID =*/10,CAddin, &__uuidof(Outlook::ExplorersEvents)> ExplNew;

class ATL_NO_VTABLE CAddin : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAddin, &CLSID_Addin>,
	public ISupportErrorInfo,
	public IDispatchImpl<IAddin, &IID_IAddin, &LIBID_OUTLOOKADDINLib>,
	public IDispatchImpl<_IDTExtensibility2, &IID__IDTExtensibility2, &LIBID_AddInDesignerObjects>,
	public SpamButtonEvents,
	public HamButtonEvents,
	public SpamMenuEvents,
	public HamMenuEvents,
	public AppEvents,
	public ExplEvents,
	public NewMailEvents,
	public SelectionEvents,
	public ExplClose,
	public ExplNew
{
public:

	CAddin():
	bConnected(false),
//	m_dwCookie_FolderChange(0),
//	m_dwCookie_SelectionChange(0),
//	m_dwCookie_SpamButton(0),
//	m_dwCookie_HamButton(0),
//	m_dwCookie_SpamMenu(0),
//	m_dwCookie_HamMenu(0),
	m_dwCookie_OptionsPage(0),
//	m_dwCookie_ExplClose(0),
	m_dwCookie_ExplNew(0),
	m_dwCookie_NewMail(0)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ADDIN)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CAddin)
	COM_INTERFACE_ENTRY(IAddin)
//DEL 	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY2(IDispatch, IAddin)
	COM_INTERFACE_ENTRY(_IDTExtensibility2)
END_COM_MAP()

BEGIN_SINK_MAP(CAddin)
SINK_ENTRY_INFO(1, __uuidof(Office::_CommandBarButtonEvents),/*dispid*/ 1, OnClickSpamButton, &OnClickButtonInfo)
SINK_ENTRY_INFO(2, __uuidof(Office::_CommandBarButtonEvents),/*dispid*/ 1, OnClickHamButton,  &OnClickButtonInfo)
SINK_ENTRY_INFO(3, __uuidof(Office::_CommandBarButtonEvents),/*dispid*/ 1, OnClickSpamButton, &OnClickButtonInfo)
SINK_ENTRY_INFO(4, __uuidof(Office::_CommandBarButtonEvents),/*dispid*/ 1, OnClickHamButton,  &OnClickButtonInfo)
SINK_ENTRY_INFO(5,__uuidof(Outlook::ApplicationEvents),/*dispinterface*/ 0xf005,OnOptionsAddPages,&OnOptionsAddPagesInfo)
//SINK_ENTRY_INFO(6,__uuidof(Outlook::ExplorerEvents),/*dispinterface*/ 0xf004,OnFolderChange,&OnFolderChangeInfo)
SINK_ENTRY_INFO(7,__uuidof(Outlook::ApplicationEvents),/*dispinterface*/ 0xf003,OnNewMail,&OnNewMailInfo)
//SINK_ENTRY_INFO(8,__uuidof(Outlook::ExplorerEvents),/*dispinterface*/ 0xf007,OnSelectionChange,&OnSelectionChangeInfo)
//SINK_ENTRY_INFO(9,__uuidof(Outlook::ExplorerEvents),/*dispinterface*/ 0xf008,OnExplClose,&OnExplCloseInfo)
SINK_ENTRY_INFO(10,__uuidof(Outlook::ExplorersEvents),/*dispinterface*/ 0xf001,OnNewExplorer,&OnExplNewInfo)
END_SINK_MAP()



// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);
	void __stdcall OnClickSpamButton(IDispatch * /*Office::_CommandBarButton**/ Ctrl,VARIANT_BOOL * CancelDefault);
	void __stdcall OnClickHamButton(IDispatch * /*Office::_CommandBarButton**/ Ctrl,VARIANT_BOOL * CancelDefault);
	void __stdcall OnOptionsAddPages(IDispatch* /*PropertyPages**/ Ctrl);
	void __stdcall OnNewExplorer(IDispatch* expl);
	void __stdcall OnNewMail(/*IDispatch* Item, VARIANT_BOOL* Cancel*/);

	void PutVisibleControls(VARIANT_BOOL pvarfVisible);
	void PutVisibleControls(VARIANT_BOOL pvarfVisible, CComPtr<Outlook::_Explorer> spExplorer);
	void PutVisibleControls(VARIANT_BOOL pvarfVisible, CComPtr<Office::CommandBar> spBar);
	void PutVisibleControls(VARIANT_BOOL pvarfVisible, CComPtr<Office::CommandBarControls> spControls);


// SelectionChange
private:
	DWORD m_dwCookie_NewMail;
	DWORD m_dwCookie_ExplNew;
	DWORD m_dwCookie_OptionsPage;

	//! Поток, отслеживающий доступность задачи антиспама в avp.exe
	class CCheckThread
	{
	public:
		CCheckThread(CAddin* _Addin) : m_hAddin(_Addin), m_timer(NULL),
										m_hThread(NULL), m_hStopEvent(NULL)
		{ 
			if(m_hAddin)
				m_hAddin->AddRef(); 
		};
		virtual ~CCheckThread() 
		{
			if(m_timer)
			{
				KillTimer(0, m_timer);
				m_timer = NULL;
			}
			if(m_hThread && m_hStopEvent)
			{
				SetEvent(m_hStopEvent);
				WaitForSingleObject(m_hThread, INFINITE);
			}
			if(m_hStopEvent)
				CloseHandle(m_hStopEvent);
			if(m_hThread)
				CloseHandle(m_hThread);
			if(m_hAddin)
				m_hAddin->Release();
		};
		virtual tERROR Start()
		{
			if(!m_timer)
				m_timer = SetTimer(NULL, NULL, 1000, (TIMERPROC)TimerProcRun);
			if(!m_hStopEvent)
				m_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			if(!m_hThread)
			{
				DWORD dwThreadId;
				m_hThread = CreateThread(NULL, 0, ThreadProcRun, this, 0, &dwThreadId);
			}
			return m_timer && m_hStopEvent && m_hThread ? errOK : errNOT_OK;
		}
		ULONG TerminateAndWait()
		{
			if(m_timer)
			{
				KillTimer(0, m_timer);
				m_timer = NULL;
			}
			return 0;
		}
		static VOID CALLBACK TimerProcRun(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
		static DWORD WINAPI ThreadProcRun(LPVOID lpThreadParameter);
	private:
		CAddin *m_hAddin;
		UINT_PTR m_timer;
		HANDLE m_hThread;
		HANDLE m_hStopEvent;
		static volatile long m_AntispamEnabled;
	};
	CCheckThread* m_hCheckThread;

public:
	enum AdviseType_t
	{
		atSpamButtonClick,
		atHamButtonClick,
		atOther,
	};
	DWORD AdviseSelectionChange(IUnknown *target, IUnknown *sink, const IID* piid, AdviseType_t atType = atOther)
	{
		if ( (atType == atSpamButtonClick) && SpamButtonAdvased ) return 0;
		if ( (atType == atHamButtonClick)  && HamButtonAdvased  ) return 0;
		//
		// Sink for Events
		//
		HRESULT hr = S_OK;
		LPCONNECTIONPOINTCONTAINER pConnPtCont;
		DWORD dwCookie_SelectionChange = 0;
		if ((target!= NULL) &&
			SUCCEEDED(hr = target->QueryInterface(IID_IConnectionPointContainer,
				(LPVOID*)&pConnPtCont)))
		{
			ATLASSERT(pConnPtCont != NULL);
			LPCONNECTIONPOINT pConnPt = NULL;

			if (SUCCEEDED(hr = pConnPtCont->FindConnectionPoint(*piid, &pConnPt)))
			{
				ATLASSERT(pConnPt != NULL);
				hr = pConnPt->Advise(sink, &dwCookie_SelectionChange);
				if (atType == atSpamButtonClick)  SpamButtonAdvased = SUCCEEDED(hr);
				if (atType == atHamButtonClick)   HamButtonAdvased  = SUCCEEDED(hr);
				pConnPt->Release();
			}
			pConnPtCont->Release();
		}
		return dwCookie_SelectionChange;
	};
	void UnadviseSelectionChange(IUnknown *target,REFIID iid,DWORD dwCookie)
	{
		//
		// Sink for Events
		//
		LPCONNECTIONPOINTCONTAINER pConnPtCont;
		
		if ((target != NULL) &&
			SUCCEEDED(target->QueryInterface(IID_IConnectionPointContainer,
			(LPVOID*)&pConnPtCont)))
		{
			ATLASSERT(pConnPtCont != NULL);
			LPCONNECTIONPOINT pConnPt = NULL;
			
			if (SUCCEEDED(pConnPtCont->FindConnectionPoint(iid, &pConnPt)))
			{
				ATLASSERT(pConnPt != NULL);
				pConnPt->Unadvise(dwCookie);
				pConnPt->Release();
			}
			pConnPtCont->Release();
		}
		return;
	};
private:
	bool SpamButtonAdvased;
	bool HamButtonAdvased;

// IAddin
public:
// _IDTExtensibility2
	STDMETHOD(OnConnection)(IDispatch * Application, ext_ConnectMode ConnectMode, IDispatch * AddInInst, SAFEARRAY * * custom);
	STDMETHOD(OnDisconnection)(ext_DisconnectMode RemoveMode, SAFEARRAY * * custom);
	STDMETHOD(OnAddInsUpdate)(SAFEARRAY * * custom)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(OnStartupComplete)(SAFEARRAY * * custom)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(OnBeginShutdown)(SAFEARRAY * * custom)
	{
		return E_NOTIMPL;
	}

	private:
		CComPtr<Outlook::_Application> m_spApp;
		bool bConnected;
		HRESULT ActionOnMessage(Action_t action);
};

#endif //__ADDIN_H_
