
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
#import "..\mcou_antispam\dll\MSADDNDR.DLL" raw_interfaces_only, raw_native_types, no_namespace, named_guids 

/////////////////////////////////////////////////////////////////////////////
// CAddin
//extern _ATL_FUNC_INFO OnClickButtonInfo;
extern _ATL_FUNC_INFO OnOptionsAddPagesInfo;
//extern _ATL_FUNC_INFO OnSimpleEventInfo;
//extern _ATL_FUNC_INFO OnNewMailInfo;

class CAddin;
//typedef IDispEventSimpleImpl</*nID =*/ 1,CAddin, &__uuidof(Office::_CommandBarButtonEvents)> SpamButtonEvents;
//typedef IDispEventSimpleImpl</*nID =*/ 2,CAddin, &__uuidof(Office::_CommandBarButtonEvents)> HamButtonEvents;
//typedef IDispEventSimpleImpl</*nID =*/ 3,CAddin, &__uuidof(Office::_CommandBarButtonEvents)> SpamMenuEvents;
//typedef IDispEventSimpleImpl</*nID =*/ 4,CAddin, &__uuidof(Office::_CommandBarButtonEvents)> HamMenuEvents;
typedef IDispEventSimpleImpl</*nID =*/ 5,CAddin, &__uuidof(Outlook::ApplicationEvents)> AppEvents;
//typedef IDispEventSimpleImpl</*nID =*/ 6,CAddin, &__uuidof(Outlook::ExplorerEvents)> ExplEvents;
//typedef IDispEventSimpleImpl</*nID =*/ 7,CAddin, &__uuidof(Outlook::ApplicationEvents)> NewMailEvents;

class ATL_NO_VTABLE CAddin : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAddin, &CLSID_Addin>,
	public ISupportErrorInfo,
	public IDispatchImpl<IAddin, &IID_IAddin, &LIBID_OUTLOOKADDINLib>,
	public IDispatchImpl<_IDTExtensibility2, &IID__IDTExtensibility2, &LIBID_AddInDesignerObjects>,
//	public SpamButtonEvents,
//	public HamButtonEvents,
//	public SpamMenuEvents,
//	public HamMenuEvents,
	public AppEvents
//	public ExplEvents,
//	public NewMailEvents
{
public:

	CAddin():
	bConnected(false),
//	m_dwCookie_SelectionChange(0),
//	m_dwCookie_SpamButton(0),
//	m_dwCookie_HamButton(0),
//	m_dwCookie_SpamMenu(0),
//	m_dwCookie_HamMenu(0),
	m_dwCookie_OptionsPage(0)
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
//SINK_ENTRY_INFO(1, __uuidof(Office::_CommandBarButtonEvents),/*dispid*/ 1, OnClickSpamButton, &OnClickButtonInfo)
//SINK_ENTRY_INFO(2, __uuidof(Office::_CommandBarButtonEvents),/*dispid*/ 1, OnClickHamButton, &OnClickButtonInfo)
//SINK_ENTRY_INFO(3, __uuidof(Office::_CommandBarButtonEvents),/*dispid*/ 1, OnClickSpamButton, &OnClickButtonInfo)
//SINK_ENTRY_INFO(4, __uuidof(Office::_CommandBarButtonEvents),/*dispid*/ 1, OnClickHamButton, &OnClickButtonInfo)
SINK_ENTRY_INFO(5,__uuidof(Outlook::ApplicationEvents),/*dispinterface*/ 0xf005,OnOptionsAddPages,&OnOptionsAddPagesInfo)
//SINK_ENTRY_INFO(5,__uuidof(Outlook::ExplorerEvents),/*dispinterface*/ 0xf003,OnSelectionChange,&OnOptionsAddPagesInfo)
//SINK_ENTRY_INFO(6,__uuidof(Outlook::ExplorerEvents),/*dispinterface*/ 0xf002,OnSelectionChange,&OnSimpleEventInfo)
//SINK_ENTRY_INFO(7,__uuidof(Outlook::ApplicationEvents),/*dispinterface*/ 0xf003,OnNewMail,&OnNewMailInfo)
END_SINK_MAP()



// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);
//	void __stdcall OnClickSpamButton(IDispatch * /*Office::_CommandBarButton**/ Ctrl,VARIANT_BOOL * CancelDefault);
//	void __stdcall OnClickHamButton(IDispatch * /*Office::_CommandBarButton**/ Ctrl,VARIANT_BOOL * CancelDefault);
	void __stdcall OnOptionsAddPages(IDispatch* /*PropertyPages**/ Ctrl);
//	void __stdcall OnSelectionChange();
//	void __stdcall OnNewMail(/*IDispatch* Item, VARIANT_BOOL* Cancel*/);

	void PutVisibleControls(VARIANT_BOOL pvarfVisible, bool bMustRefresh = false);


// SelectionChange
private:
	DWORD m_dwCookie_NewMail;
//	DWORD m_dwCookie_SelectionChange;
//	DWORD m_dwCookie_HamButton;
//	DWORD m_dwCookie_SpamButton;
//	DWORD m_dwCookie_HamMenu;
//	DWORD m_dwCookie_SpamMenu;
	DWORD m_dwCookie_OptionsPage;

	DWORD AdviseSelectionChange(IUnknown *target, IUnknown *sink, const IID* piid)
	{
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
public:

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
//		CComPtr<Outlook::_Explorer> m_explorer;
		bool bConnected;
//		HRESULT ActionOnMessage(Action_t action);
};

#endif //__ADDIN_H_
