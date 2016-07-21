#ifndef _EXPLORERHANDLER_H_
#define _EXPLORERHANDLER_H_

#include "handler.h"

//
//! Explorer handler used to sink Outlook::Explorer events...
//
class CExplorerHandler: public CHandler 
{
public:
    CExplorerHandler(CAddin *pAddin, Outlook::_Explorer *spExplorer);
    virtual ~CExplorerHandler(void);

    // *** IDispatch methods ***
    STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);

private:
	bool bConnected;
    CComPtr<Outlook::_Explorer> m_pExplorer;
	CComPtr<Office::_CommandBarButton> m_spSpamButton; 
	CComPtr<Office::_CommandBarButton> m_spHamButton; 
	CComPtr<Office::_CommandBarButton> m_spSpamMenu; 
	CComPtr<Office::_CommandBarButton> m_spHamMenu; 
	CComPtr<Office::CommandBar> m_spAntispamBar; 

	void OnNewExplorer();
	void OnFolderChange();
	void OnSelectionChange();
	void ShowAntispamBar(VARIANT_BOOL bShow);
	
	DWORD m_dwCookie_HamButton;
	DWORD m_dwCookie_SpamButton;
	DWORD m_dwCookie_HamMenu;
	DWORD m_dwCookie_SpamMenu;

	void AdviseButtons();
};

#endif//_EXPLORERHANDLER_H_