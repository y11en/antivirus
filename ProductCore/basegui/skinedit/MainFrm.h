// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <DockTabFrame.h>
#include "AboutDlg.h"
#include "ClientViewListener.h"
#include "OutputView.h"
#include "ItemEdit.h"
#include "PropsView.h"

#define WM_GSE_INITPANELS	WM_USER + 100

namespace NewEditViewDialog
{
	enum Choice
	{
		  choiceUnknown        = -1
		, choiceMainPane       = 0 //IDC_MAINPANE_RADIO
		, choiceLeftDockPane   = 1 //IDC_LEFTDOCK_RADIO
		, choiceTopDockPane    = 2 //IDC_TOPDOCK_RADIO
		, choiceRightDockPane  = 3 //IDC_RIGHTDOCK_RADIO
		, choiceBottomDockPane = 4 //IDC_BOTTOMDOCK_RADIO
		, choiceFloatFrame     = 5 //IDC_FLOATFRAME_RADIO
	};
};

// ########################################################################
// CGuiItemsList

class CGuiItemsList : public CWindowImpl<CGuiItemsList, CTreeViewCtrl>
{
public:
	typedef CWindowImpl<CGuiItemsList, CTreeViewCtrl>	TBase;

	bool	Create(HWND hParentWnd);
	void	Reinit(CRootItem * pRoot);

public:
	BEGIN_MSG_MAP(CGuiItemBrowser)
		MESSAGE_HANDLER(WM_KEYDOWN,		OnKeyDown)
	END_MSG_MAP()

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	CImageList			m_Imgs;
	cVector<cStrObj>	m_aGroups;
	tKeys				m_aFonts;
	tKeys				m_aIcons;
	tKeys				m_aBackgrounds;
	tKeys				m_aBorders;
};

// ########################################################################
// CGuiItemBrowser

class CGuiItemBrowser : public CWindowImpl<CGuiItemBrowser, CTreeViewCtrl>
{
public:
	typedef CWindowImpl<CGuiItemBrowser, CTreeViewCtrl>	TBase;

	CGuiItemBrowser() : m_pItem(NULL), m_bSel(0) {}
	
	bool	Create(HWND hParentWnd);
	void	Update(CItemBase * pItem, tPTR pSelItem = NULL);
	tPTR	GetSel();
	bool	IsInterSel() { return m_bSel; }

public:
	BEGIN_MSG_MAP(CGuiItemBrowser)
		MESSAGE_HANDLER(WM_KEYDOWN,		OnKeyDown)
	END_MSG_MAP()

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	HTREEITEM	_Update(HTREEITEM hParent, HTREEITEM hChild, CItemBase * pParent, bool bClean = true);
	bool		_Select(HTREEITEM hParent, tPTR pSelItem);
	
	CItemBase *	m_pItem;
	CImageList	m_Imgs;
	unsigned	m_bSel : 1;
};

// ########################################################################
// CMainFrame

class CMainFrame :
	public CFrameWindowImpl<CMainFrame>,
	public CUpdateUI<CMainFrame>,
	public CMessageFilter,
	public CIdleHandler,
	public FrameListener,
	public ClientViewListener,
	public CRootSink
{
public:
	CAtlMap<HWND, ClientView *>		m_aViews;
	CAtlMap<HWND, CGuiItemEdit *>	m_aEdits;
	Frame							m_Frame;
	CCommandBarCtrl					m_CmdBar;
	RichEdit						m_Output;
	CGuiItemsList					m_GuiItems;
	CPropBrowser					m_PropsView;
	CGuiItemBrowser					m_ItemBrowser;
	CSingleTextPropEditor			m_PropsEditor;
	CSingleTextPropEditor			m_TextEditor;

	friend class CGuiItemsList;
	friend class CGuiItemBrowser;
	friend class CGuiItemEdit;
	
protected:
	// WM_USER events
	static const int eventCloseClientView = WM_USER + 1;
	
	ClientView* createClientWindow( const TCHAR* caption, HWND hWnd, const TCHAR* toolTip, int imageIndex = -1)
	{
		ClientView* result = new ClientView( caption, hWnd, toolTip, imageIndex);
		this->m_aViews[ hWnd] = result;
		return result;
	}
	
	ClientView * lookUpClientWindow(HWND clientWnd)
	{
		CAtlMap<HWND, ClientView *>::CPair * pair = this->m_aViews.Lookup(clientWnd);
		return pair ? pair->m_value : NULL;
	}
	
/*
	ClientView * lookUpClientWindow(SplitPane * pPane)
	{
		POSITION position = m_aViews.GetStartPosition();
		if( position )
		{
			do
			{
				ClientView * cv = m_aViews.GetNextValue(position);
				if( cv->owner == pPane )
					return cv;
			}
			while(position);
		}
	
		return NULL;
	}
*/

	CGuiItemEdit * lookUpEdit(HWND hEditWnd)
	{
		CAtlMap<HWND, CGuiItemEdit *>::CPair * pair = this->m_aEdits.Lookup(hEditWnd);
		return pair ? pair->m_value : NULL;
	}

	CGuiItemEdit * getActiveEdit()
	{
		POSITION position = m_aEdits.GetStartPosition();
		if( NULL != position)
		{
			do
			{
				CGuiItemEdit * p = m_aEdits.GetNextValue(position);
				if( p->IsWindowVisible() )
					return p;
			}
			while( NULL != position );
		}
	
		return NULL;
	}

	CGuiItemEdit * getEdit(LPCSTR strSection)
	{
		POSITION position = m_aEdits.GetStartPosition();
		if( NULL != position)
		{
			do
			{
				CGuiItemEdit * p = m_aEdits.GetNextValue(position);
				if( p->m_pItem->m_strSection == strSection )
					return p;
			}
			while( NULL != position );
		}
	
		return NULL;
	}

public:
	CMainFrame();
	~CMainFrame();
	
	// ClientViewListener interface
	const ClientView* get( HWND clientViewWnd) const
	{
		return this->m_Frame.getClientView( clientViewWnd);
	}
	
	void clientViewHide( HWND clientViewWnd)
	{
		ATLASSERT( ::IsWindow( clientViewWnd));
		
		if ( 0 == ::SendMessage( clientViewWnd, WM_CLOSE, 0, 0)) {
			
			if ( 0 )
			{
				
				// just hide client list view window
				ClientView* clientView = this->m_Frame.getClientView( clientViewWnd);
				
				this->m_Frame.detachClientView( clientViewWnd);
			} else
				// to destroy the client view we get out of client view call stack
				this->PostMessage( eventCloseClientView, 0, reinterpret_cast<LPARAM>( clientViewWnd));
		}
		return;
	}
	
	void clientViewDockable( HWND clientViewWnd) {
		
		ATLASSERT( ::IsWindow( clientViewWnd));
		
		this->m_Frame.toggleClientViewState( clientViewWnd, ClientView::stateDockable);
		return;
	}
	
	void clientViewFloating(HWND clientViewWnd)
	{
		ATLASSERT(::IsWindow( clientViewWnd));
		
		this->m_Frame.toggleClientViewState( clientViewWnd, ClientView::stateFloating);
		return;
	}
	
	// FrameListener interface
	void clientDetached(ClientView * clientView)
	{
		CGuiItemEdit * pEdit = this->lookUpEdit(clientView->wnd);
		if( !pEdit )
			return;
		
		// remove rich edit view
		this->m_aViews.RemoveKey(pEdit->m_hWnd);
		delete clientView;
		
		this->m_aEdits.RemoveKey(pEdit->m_hWnd);
		pEdit->DestroyWindow();
		delete pEdit;
		
		GuiItem_OnSelChanged();
	}
	
	void CreatePane(NewEditViewDialog::Choice type, HWND hWnd, const TCHAR * caption, const TCHAR * ttip = NULL);
	bool CanClose();
	void CloseOpenViews();

public:
	// Message handlers
	DECLARE_FRAME_WND_CLASS(_T("KL.SkinEditor"), IDR_MAINFRAME)
	
	virtual BOOL PreTranslateMessage(MSG * pMsg)
	{
		return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
	}
	
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR,					UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR,				UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_OUTPUTVIEW,				UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_PROPSVIEW,				UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_ITEMSVIEW,				UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_BROWSER,					UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_WORDWRAP,				UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_FILE_SAVE,					UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_FILE_REFRESH,					UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_GUIITEMS_CREATE_EXTERNAL,		UPDUI_TOOLBAR)
		
		UPDATE_ELEMENT(ID_EDIT_CUT,						UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_EDIT_COPY,					UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_EDIT_PASTE,					UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_EDIT_CLEAR,					UPDUI_TOOLBAR)

		// Temp disable new !!!!!!!!!!!!!!!!!!!!!!!!!!
		UPDATE_ELEMENT(ID_FILE_NEW,				UPDUI_TOOLBAR)
	END_UPDATE_UI_MAP()
	
	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE,					OnCreate)
		MESSAGE_HANDLER(WM_CLOSE,					OnClose)
		MESSAGE_HANDLER(eventCloseClientView,		OnCloseClientView)
		MESSAGE_HANDLER(WM_GSE_INITPANELS,			OnInitPanels)
		MESSAGE_HANDLER(WM_QUERYENDSESSION,			OnQueryEndSession)
		
		COMMAND_ID_HANDLER(ID_APP_EXIT,				OnFileExit)
		COMMAND_ID_HANDLER(ID_APP_ABOUT,			OnAppAbout)
		COMMAND_ID_HANDLER(ID_FILE_NEW,				OnFileNew)
		COMMAND_ID_HANDLER(ID_FILE_OPEN,			OnFileOpen)
		COMMAND_ID_HANDLER(ID_FILE_SAVE,			OnFileSave)
		COMMAND_ID_HANDLER(ID_FILE_CLEAROUTPUT,		OnFileClearOutput)
		COMMAND_ID_HANDLER(ID_FILE_REFRESH,			OnFileRefresh)
		
		COMMAND_ID_HANDLER(ID_EDIT_CUT,				OnEditCut)
		COMMAND_ID_HANDLER(ID_EDIT_COPY,			OnEditCopy)
		COMMAND_ID_HANDLER(ID_EDIT_PASTE,			OnEditPaste)
		COMMAND_ID_HANDLER(ID_EDIT_CLEAR,			OnEditClear)

		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR,			OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR,		OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_VIEW_OUTPUTVIEW,		OnViewOutput)
		COMMAND_ID_HANDLER(ID_VIEW_PROPSVIEW,		OnViewProps)
		COMMAND_ID_HANDLER(ID_VIEW_ITEMSVIEW,		OnViewObjects)
		COMMAND_ID_HANDLER(ID_VIEW_BROWSER,			OnViewBrowser)
		COMMAND_ID_HANDLER(ID_VIEW_WORDWRAP,		OnViewWordWrap)
		
		COMMAND_RANGE_HANDLER(ID_GUIITEMS_CREATE_01, ID_GUIITEMS_CREATE_01 + 100, OnCreateGuiItem)
		COMMAND_ID_HANDLER(ID_GUIITEMS_CREATE_FROM_EXTERNAL,	OnInsertGuiItemFromExt)
		COMMAND_ID_HANDLER(ID_GUIITEMS_CREATE_EXTERNAL,	OnCreateGuiItemExt)
		
		COMMAND_ID_HANDLER(ID_CTXITEMEDIT_OPENEXTERNALITEM,		OnCtxItemEdit_OpenExternalItem)

		NOTIFY_HANDLER_EX(m_GuiItems.m_hWnd, NM_DBLCLK, OnEditGuiItem)
		NOTIFY_HANDLER_EX(m_GuiItems.m_hWnd, NM_RETURN, OnEditGuiItem)
		NOTIFY_HANDLER_EX(m_ItemBrowser.m_hWnd, TVN_SELCHANGED, OnSelectGuiItem)

		CHAIN_MSG_MAP(CUpdateUI< CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl< CMainFrame>)
	END_MSG_MAP()

	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnQueryEndSession(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnCreateGuiItem(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnInsertGuiItemFromExt(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnCreateGuiItemExt(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnEditGuiItem(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnSelectGuiItem(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnInitPanels(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnCloseClientView(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileClearOutput(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEditClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewOutput( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewProps( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewObjects( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewBrowser( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewWordWrap( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCtxItemEdit_OpenExternalItem(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

public:	// CRootSink
	void   ReportError(tTRACE_LEVEL level, LPCSTR strInfo);
	void   GetLocalizationId(tString& strLocId) { strLocId = m_strLocId; }
	void   OnGetSkinPath(tObjPath& strSkinPath) { strSkinPath = m_strSkinPath; }
	tERROR OnFileSaveError(const cStrObj& strPath, tERROR err);

	cStrObj m_strSkinPath;
	tString m_strLocId;

protected:
	void	OpenSkin(LPCWSTR strPath, LPCSTR strLocId = NULL);
	bool	OpenGuiItem(LPCSTR strSection, bool bExt);
	bool	DeleteGuiItem(LPCSTR strSection);
	void	ShowError(LPTSTR strAction, tERROR err);

public:
	void	GuiItem_OnSelChanged();
	void	GuiItem_OnDataChanged(bool bItems);
	void	GuiItem_OnSkinChanged();

protected:
	CGuiLoader			m_Gui;
	unsigned			m_bCreateNewItem : 1;
	unsigned			m_bReinitItems : 1;
	CItemEditData::Cd	m_ClipData;
};
