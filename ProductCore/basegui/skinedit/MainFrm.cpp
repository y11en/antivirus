#include "stdafx.h"

#include "resource.h"
#include "aboutdlg.h"
#include "MainFrm.h"
#include "ItemEdit.h"

// ########################################################################
// CGuiItemsList

#define GUIITEMSLIST_GROUP	((DWORD_PTR)1)
#define GUIITEMSLIST_DLG	((DWORD_PTR)2)
#define GUIITEMSLIST_EXT	((DWORD_PTR)3)

bool	CGuiItemsList::Create(HWND hParentWnd)
{
	if( !TBase::Create(hParentWnd, 0, 0, WS_CHILD|WS_VISIBLE|TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|TVS_DISABLEDRAGDROP|TVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE) )
		return false;

	m_Imgs.CreateFromImage(IDB_GUIEXTITEMS, 16, -1, RGB(255, 0, 0), IMAGE_BITMAP, LR_CREATEDIBSECTION);
	
	SetImageList(m_Imgs, TVSIL_NORMAL);
	return true;
}

struct CGuiItemsList_Sect
{
	tString				n;
	bool				ext;
};

struct CGuiItemsList_Group
{
	cStrObj				n;
	std::vector<CGuiItemsList_Sect>	s;
};

void	CGuiItemsList::Reinit(CRootItem * pRoot)
{
	USES_CONVERSION;

	if( !pRoot )
	{
		DeleteAllItems();
		return;
	}
	
	std::vector<CGuiItemsList_Group>	_grp;
	{
		tSections aSections; pRoot->GetSections(PROFILE_LAYOUT, aSections);

		// Составление списка редактируемых элементов
		for(DWORD i = 0; i < aSections.size(); i++)
		{
			const tString& strSection = aSections[i];
			tString strProps; pRoot->GetString(strProps, PROFILE_LAYOUT, strSection.c_str(), STR_SELF);
			
			bool bExt = false;
			{
				LPCSTR strIncl[] = {STR_PID_TYPE, STR_PID_EXT, NULL};
				CItemProps _Props(strProps.c_str(), tString(), strIncl);
				if( _Props.Get(STR_PID_TYPE).Get() != GUI_ITEMT_DIALOG )
				{
					if( !_Props.Get(STR_PID_EXT).GetBool() )
						continue;
				
					bExt = true;
				}
			}

			LPCWSTR strFileW = pRoot->GetSectionGroup(PROFILE_LAYOUT, strSection.c_str());
			if( !strFileW )
				continue;

			cStrObj strGroup(strFileW);
			
			CGuiItemsList_Group * _g;
			DWORD k;
			for(k = 0; k < _grp.size(); k++)
			{
				_g = &_grp[k];
				if( _g->n == strGroup )
					break;
			}

			if( k == _grp.size() )
			{
				_g = &*_grp.insert(_grp.end(), CGuiItemsList_Group());
				_g->n = strGroup;
			}

			CGuiItemsList_Sect& _s = *_g->s.insert(_g->s.end(), CGuiItemsList_Sect());
			_s.n = strSection;
			_s.ext = bExt;
		}
	}
	
	// Синхронизация групп
	{
		std::vector<bool> _aOldDel;
		std::vector<bool> _aNewSame; _aNewSame.resize(_grp.size());

		CString strGroup;

		for(HTREEITEM h = GetChildItem(TVI_ROOT); h; h = GetNextSiblingItem(h))
		{
			GetItemText(h, strGroup);

			DWORD k;
			for(k = 0; k < _grp.size(); k++)
				if( _grp[k].n == strGroup.GetString() )
					break;

			if( k < _grp.size() )
			{
				_aNewSame[k] = true;
				_aOldDel.push_back(false);
			}
			else
				_aOldDel.push_back(true);
		}

		// Удаление отсутствующих элементов
		tDWORD i = 0;
		for(HTREEITEM h = GetChildItem(TVI_ROOT); h; i++)
		{
			HTREEITEM hNext = GetNextSiblingItem(h);
			if( _aOldDel[i] )
				DeleteItem(h);
			
			h = hNext;
		}

		// Добавление новых элементов
		for(DWORD k = 0; k < _grp.size(); k++)
		{
			if( !_aNewSame[k] )
			{
				HTREEITEM h = InsertItem(W2T((LPWSTR)_grp[k].n.data()), 0, 0, TVI_ROOT, TVI_LAST);
				SetItemData(h, GUIITEMSLIST_GROUP);
			}
		}
		
		SortChildren(TVI_ROOT);
	}

	// Синхронизация элементов групп
	{
		CString strGroup;
		for(HTREEITEM hGrp = GetChildItem(TVI_ROOT); hGrp; hGrp = GetNextSiblingItem(hGrp))
		{
			CGuiItemsList_Group * _g = NULL;
			{
				GetItemText(hGrp, strGroup);
				DWORD k;
				for(k = 0; k < _grp.size(); k++)
					if( _grp[k].n == strGroup.GetString() )
					{
						_g = &_grp[k];
						break;
					}
			}

			if( !_g )
				continue;

			std::vector<bool> _aOldDel;
			std::vector<bool> _aNewSame; _aNewSame.resize(_g->s.size());
			CString strSection;

			// Выявление одинаковых элементов
			for(HTREEITEM h = GetChildItem(hGrp); h; h = GetNextSiblingItem(h))
			{
				GetItemText(h, strSection);

				DWORD k;
				for(k = 0; k < _g->s.size(); k++)
					if( strSection == _g->s[k].n.c_str() )
						break;

				if( k < _g->s.size() )
				{
					_aNewSame[k] = true;
					_aOldDel.push_back(false);
				}
				else
					_aOldDel.push_back(true);
			}

			// Удаление отсутствующих элементов
			tDWORD i = 0;
			for(HTREEITEM h = GetChildItem(hGrp); h; i++)
			{
				HTREEITEM hNext = GetNextSiblingItem(h);
				if( _aOldDel[i] )
					DeleteItem(h);
				
				h = hNext;
			}

			// Добавление новых элементов
			for(DWORD k = 0; k < _g->s.size(); k++)
			{
				CGuiItemsList_Sect& _s = _g->s[k];
				
				if( !_aNewSame[k] )
				{
					int nImage = _s.ext ? 2 : 1;
					HTREEITEM h = InsertItem(CString(_s.n.c_str()), nImage, nImage, hGrp, TVI_LAST);
					SetItemData(h, _s.ext ? GUIITEMSLIST_EXT : GUIITEMSLIST_DLG);
				}
			}
			
			SortChildren(hGrp);
		}
	}

	HTREEITEM hSelItem = GetSelectedItem();
	if( !hSelItem )
		SelectItem(GetChildItem(TVI_ROOT));

	m_aGroups.clear();		for(DWORD k = 0; k < _grp.size(); k++) m_aGroups.push_back() = _grp[k].n;
	m_aFonts.clear();		pRoot->GetKeys(PROFILE_RESOURCE, PROFILE_SID_FONTS,		m_aFonts);
	m_aIcons.clear();		pRoot->GetKeys(PROFILE_RESOURCE, PROFILE_SID_ICONS,		m_aIcons);
	m_aBackgrounds.clear();	pRoot->GetKeys(PROFILE_RESOURCE, PROFILE_SID_BACKGNDS,	m_aBackgrounds);
	m_aBorders.clear();		pRoot->GetKeys(PROFILE_RESOURCE, PROFILE_SID_BORDERS,	m_aBorders);
}

// ########################################################################

LRESULT CGuiItemsList::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	USES_CONVERSION;
	
	CMainFrame * pMain = _OWNEROFMEMBER_PTR(CMainFrame, m_GuiItems);
	
	if( wParam == VK_DELETE )
	{
		HTREEITEM hSelItem = GetSelectedItem();
		DWORD_PTR nData = GetItemData(hSelItem);
		if( nData == GUIITEMSLIST_DLG || nData == GUIITEMSLIST_EXT )
		{
			CString strSection; GetItemText(hSelItem, strSection);
			pMain->DeleteGuiItem(T2A(strSection));
		}
	}
	
	return bHandled = FALSE, 0;
}

// ########################################################################
// CGuiItemBrowser

bool	CGuiItemBrowser::Create(HWND hParentWnd)
{
	if( !TBase::Create(hParentWnd, 0, 0, WS_CHILD|WS_VISIBLE|TVS_HASBUTTONS|TVS_HASLINES|TVS_DISABLEDRAGDROP|TVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE) )
		return false;

	m_Imgs.CreateFromImage(IDB_GUIEXTITEMS, 16, -1, RGB(255, 0, 0), IMAGE_BITMAP, LR_CREATEDIBSECTION);
	
	SetImageList(m_Imgs, TVSIL_NORMAL);
	return true;
}

void	CGuiItemBrowser::Update(CItemBase * pItem, tPTR pSelItem)
{
	USES_CONVERSION;
	
	m_bSel = 1;
	
	if( m_pItem != pItem )
	{
		DeleteAllItems();
		m_pItem = pItem;
	}

	if( m_pItem )
	{
		HTREEITEM hRoot = GetChildItem(TVI_ROOT);
		if( !hRoot )
		{
			int nImage = (m_pItem->m_nFlags & STYLE_EXT) ? 2 : 1;
			hRoot = InsertItem(A2T(m_pItem->m_pEditData->m_strSection.c_str()), nImage, nImage, TVI_ROOT, TVI_LAST);
		}
		else
		{
			CString strName; GetItemText(hRoot, strName);
			if( strName != m_pItem->m_strSection.c_str() )
				SetItemText(hRoot, A2T(m_pItem->m_pEditData->m_strSection.c_str()));
		}

		SetItemData(hRoot, (DWORD_PTR)m_pItem);

		_Update(hRoot, GetChildItem(hRoot), m_pItem);
		
		Expand(hRoot);

		_Select(TVI_ROOT, pSelItem);
	}

	m_bSel = 0;
}

tPTR	CGuiItemBrowser::GetSel()
{
	HTREEITEM hSelItem = GetSelectedItem();
	if( !hSelItem )
		return NULL;
	return (tPTR)GetItemData(hSelItem);
}

// ########################################################################

LRESULT CGuiItemBrowser::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CMainFrame * pMain = _OWNEROFMEMBER_PTR(CMainFrame, m_ItemBrowser);
	
	if( wParam == VK_DELETE )
	{
		CGuiItemEdit * pEditor = pMain->getActiveEdit();
		if( pEditor )
			pEditor->DeleteItem((CItemBase *)GetSel());
	}
	
	return bHandled = FALSE, 0;
}

// ########################################################################

HTREEITEM	CGuiItemBrowser::_Update(HTREEITEM hParent, HTREEITEM h, CItemBase * pParent, bool bClean)
{
	USES_CONVERSION;
	
	CString strName;

	// Установка одинаковых элементов
	for(DWORD k = 0; k < pParent->m_aItems.size(); k++)
	{
		CItemBase * pItem = pParent->m_aItems[k];
		if( pItem->m_pEditData )
		{
			if( h )
			{
				GetItemText(h, strName);
				if( strName != pItem->m_pEditData->m_strItemId.c_str() )
					SetItemText(h, CString(pItem->m_pEditData->m_strItemId.c_str()));
			}
			else
				h = InsertItem(CString(pItem->m_pEditData->m_strItemId.c_str()), 3, 3, hParent, TVI_LAST);
		
			SetItemData(h, (DWORD_PTR)pItem);

			h = _Update(h, GetChildItem(h), pItem);
		}
		else
			h = _Update(hParent, h, pItem, false);
	}

	// Удаление отсутствующих элементов
	if( !bClean )
		return h;

	while( h )
	{
		HTREEITEM hNext = GetNextSiblingItem(h);
		DeleteItem(h);
		h = hNext;
	}

	return GetNextSiblingItem(hParent);
}

bool	CGuiItemBrowser::_Select(HTREEITEM hParent, tPTR pSelItem)
{
	for(HTREEITEM h = GetChildItem(hParent); h; h = GetNextSiblingItem(h))
	{
		if( GetItemData(h) == (DWORD_PTR)pSelItem )
		{
			SelectItem(h);
			EnsureVisible(h);
			return true;
		}

		if( _Select(h, pSelItem) )
			return true;
	}

	return false;
}

// ########################################################################
// CMainFrame

CMainFrame::CMainFrame() :
	m_Output(this), m_GuiItems(), m_Frame(this), m_bCreateNewItem(0), m_bReinitItems(0)
{}

CMainFrame::~CMainFrame()
{
	POSITION position = m_aEdits.GetStartPosition();
	if( position )
	{
		do
			delete m_aEdits.GetNextValue(position);
		while( position );
	}
	m_aEdits.RemoveAll();
	
	position = m_aViews.GetStartPosition();
	if( position )
	{
		do
			delete m_aViews.GetNextValue(position);
		while( position );
	}
	m_aViews.RemoveAll();
}

void	CMainFrame::OpenSkin(LPCWSTR strPath, LPCSTR strLocId)
{
	m_Gui.Unload();

	m_strSkinPath.erase();
	m_strLocId.erase();
	
	if( strPath )
	{
		m_strSkinPath = strPath;
		m_strSkinPath.check_last_slash(false);
		if( strLocId )
			m_strLocId = strLocId;
		m_Gui.CreateAndInitRoot(g_root, GUIFLAG_EDITOR, this);
	}

	m_bReinitItems = 1;
	m_GuiItems.Reinit(m_Gui.GetRoot());
	m_bReinitItems = 0;

	CString strName; strName.LoadString(IDR_MAINFRAME);
	
	USES_CONVERSION;
	
	CString strTitle;
	strTitle += W2T((LPWSTR)m_strSkinPath.data());
	strTitle += " - ";
	strTitle += strName;
	SetWindowText(strTitle);
}

bool	CMainFrame::OpenGuiItem(LPCSTR strSection, bool bExt)
{
	USES_CONVERSION;
	
	CGuiItemEdit * pEditor = getEdit(strSection);
	if( pEditor )
	{
		m_Frame.setFocusTo(pEditor->m_hWnd);
		pEditor->SetSelection(pEditor->m_pItem);
		GuiItem_OnSelChanged();
		return true;
	}
	
	pEditor = new CGuiItemEdit(this);
	pEditor->Create(m_hWnd, m_Gui.GetRoot(), strSection, bExt);
	if( !pEditor->m_pItem )
	{
		delete pEditor;
		return false;
	}
	
	m_aEdits[pEditor->m_hWnd] = pEditor;

	CString strTip(strSection); strTip += _T(" item");
	CreatePane(NewEditViewDialog::choiceMainPane, pEditor->m_hWnd, A2T(strSection), strTip);
	return true;
}

bool	CMainFrame::DeleteGuiItem(LPCSTR strSection)
{
	USES_CONVERSION;
	
	CString strText; strText.Format(_T("Are you sure want to permanently delete \"%s\" item?"), A2T(strSection));
	if( MessageBox(strText, _T("Warning"), MB_YESNO|MB_ICONEXCLAMATION) != IDYES )
		return false;
	
	CGuiItemEdit * pEditor = getEdit(strSection);
	if( pEditor )
	{
		CString strText; strText.Format(_T("You can't delete \"%s\" item while it opened for edit. Please close the item editor before delete."), A2T(strSection));
		MessageBox(strText, _T("Warning"), MB_OK|MB_ICONEXCLAMATION);

		m_Frame.setFocusTo(pEditor->m_hWnd);
		GuiItem_OnSelChanged();
		return false;
	}

	tERROR err = m_Gui.GetRoot()->DeleteFromSkin(strSection);
	if( PR_SUCC(err) )
		return GuiItem_OnSkinChanged(), true;

	ShowError(_T("Delete error"), err);
	return false;
}

void	CMainFrame::ShowError(LPTSTR strAction, tERROR err)
{
	if( err == errOBJECT_READ_ONLY )
		return;
	
	CString strErr; strErr.Format(_T("%s: 0x%08X"), strAction, err);
	MessageBox(strErr, _T("Error"), MB_OK|MB_TOPMOST|MB_ICONSTOP|MB_APPLMODAL|MB_SETFOREGROUND);
}

// ########################################################################

void CMainFrame::CreatePane(NewEditViewDialog::Choice type, HWND hWnd, const TCHAR * caption, const TCHAR * ttip)
{
	ClientView * clientView = this->createClientWindow(caption, hWnd, ttip);
	switch(type)
	{
	case NewEditViewDialog::choiceMainPane:
		this->m_Frame.addClientView( clientView);
		break;
		
	case NewEditViewDialog::choiceLeftDockPane:
		this->m_Frame.dockClientView( DockSplitTab::dockLEFT, clientView);
		break;
		
	case NewEditViewDialog::choiceTopDockPane:
		this->m_Frame.dockClientView( DockSplitTab::dockTOP, clientView);
		break;
		
	case NewEditViewDialog::choiceRightDockPane:
		this->m_Frame.dockClientView( DockSplitTab::dockRIGHT, clientView);
		break;
		
	case NewEditViewDialog::choiceBottomDockPane:
		this->m_Frame.dockClientView( DockSplitTab::dockBOTTOM, clientView);
		break;
		
	case NewEditViewDialog::choiceFloatFrame:
		{
			CRect rect;
			this->m_Frame.GetWindowRect( rect);
			this->m_Frame.floatClientView( rect, clientView);
		}
		break;
		
	}
}

bool CMainFrame::CanClose()
{
	for(POSITION position = m_aEdits.GetStartPosition(); position; )
	{
		CGuiItemEdit * p = m_aEdits.GetNextValue(position);
		if( p->SendMessage(WM_CLOSE) )
			return false;
	}

	return true;
}

void CMainFrame::CloseOpenViews()
{
	for(;;)
	{
		POSITION position = m_aEdits.GetStartPosition();
		if( !position )
			break;

		CGuiItemEdit * p = m_aEdits.GetNextValue(position);

		ClientView * clientView = lookUpClientWindow(p->m_hWnd);
		if( clientView )
		{
			m_Frame.detachClientView(clientView->wnd);
			clientDetached(clientView);
		}
	}
}

// ########################################################################

void CMainFrame::ReportError(tTRACE_LEVEL level, LPCSTR strInfo)
{
	USES_CONVERSION;
	
	if( !m_Output.m_hWnd )
		return;

	if( m_bReinitItems )
		return;
	
	CString strErr; strErr.Format(_T("<%c>: %s\n"), (level <= prtERROR) ? 'E' : ((level <= prtIMPORTANT) ? 'W' : 'I'), A2T(strInfo));
	m_Output.AppendText(strErr);
}

tERROR CMainFrame::OnFileSaveError(const cStrObj& strPath, tERROR err)
{
	USES_CONVERSION;

	if( err == errOBJECT_READ_ONLY )
	{
		CString strText; strText.Format(_T("The file \"%s\" cannot be saved because of it is read-only.\n\nIf You choose Ignore the Skin Editor can attempt to remove the read-only attribute and overwrite the file in it's current location."), W2T((LPWSTR)strPath.data()));
		UINT nRes = MessageBox(strText, _T("Write file"), MB_ABORTRETRYIGNORE|MB_ICONEXCLAMATION);
		if( nRes == IDABORT )
			return errNOT_OK;

		return nRes == IDIGNORE ? warnFALSE : errOK;
	}

	CString strText; strText.Format(_T("The file \"%s\" cannot be saved: error 0x%08X.\n\nIf You choose Retry the Skin Editor can attempt to save file again."), W2T((LPWSTR)strPath.data()), err);
	return MessageBox(strText, _T("Write file"), MB_RETRYCANCEL|MB_ICONSTOP) == IDRETRY ? errOK : errNOT_OK;
}

// ########################################################################

BOOL CMainFrame::OnIdle()
{
	CGuiItemEdit * pEditor = getActiveEdit();
	
	this->UIEnable(ID_VIEW_OUTPUTVIEW,    this->m_Frame.dockPaneExists(dockBOTTOM));
	this->UIEnable(ID_VIEW_PROPSVIEW, this->m_Frame.dockPaneExists(dockRIGHT));
	this->UIEnable(ID_VIEW_ITEMSVIEW,   this->m_Frame.dockPaneExists(dockLEFT));
	
	this->UIEnable(ID_FILE_SAVE,  pEditor ? pEditor->m_bDirty : false);
	this->UIEnable(ID_FILE_REFRESH, !!m_Gui.GetRoot());
	
	this->UIEnable(ID_EDIT_CUT, true);
	this->UIEnable(ID_EDIT_COPY, true);
	this->UIEnable(ID_EDIT_PASTE, m_ClipData.IsValid());
	this->UIEnable(ID_EDIT_CLEAR, true);

	// Temp disable new !!!!!!!!!!!!!!!!!!!!!!!!!!
	this->UIEnable(ID_FILE_NEW, false);
	
	this->UISetCheck(ID_VIEW_OUTPUTVIEW,    this->m_Frame.dockPaneVisible(dockBOTTOM));
	this->UISetCheck(ID_VIEW_PROPSVIEW, this->m_Frame.dockPaneVisible(dockRIGHT));
	this->UISetCheck(ID_VIEW_ITEMSVIEW,   this->m_Frame.dockPaneVisible(dockLEFT));
	
	{
		CSingleTextPropEditor * pTxtEdit = NULL;
		if( m_PropsEditor.m_hWnd == ::GetFocus() )
			pTxtEdit = &m_PropsEditor;
		else if( m_TextEditor.m_hWnd == ::GetFocus() )
			pTxtEdit = &m_TextEditor;

		if( pTxtEdit )
		{
			UIEnable(ID_VIEW_WORDWRAP, true);
			UISetCheck(ID_VIEW_WORDWRAP, pTxtEdit->GetWordWrap());
		}
		else
		{
			UIEnable(ID_VIEW_WORDWRAP, false);
			UISetCheck(ID_VIEW_WORDWRAP, false);
		}
	}

	this->UISetCheck(ID_GUIITEMS_CREATE_EXTERNAL,    !!this->m_bCreateNewItem);

	this->UIUpdateToolBar(TRUE);
	return FALSE;
}

// ########################################################################

#define CBRS_BORDER_LEFT    0x0100L
#define CBRS_BORDER_TOP     0x0200L
#define CBRS_BORDER_RIGHT   0x0400L
#define CBRS_BORDER_BOTTOM  0x0800L
#define CBRS_BORDER_ANY     0x0F00L

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	this->CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	
	// Create command bar window
	HWND hWndCmdBar = this->m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	this->m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	this->m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	this->SetMenu(NULL);
	this->AddSimpleReBarBand( hWndCmdBar);
	
	// Create toolbar and add it to rebar
	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	this->AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	// Create toolbar and add it to rebar
	HWND hWndToolBarGuiItems = CreateSimpleToolBarCtrl(m_hWnd, IDR_TB_GUIITEMS, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	this->AddSimpleReBarBand(hWndToolBarGuiItems, NULL, FALSE);
	
	// Status bar
	CreateSimpleStatusBar();
	
	this->UIAddToolBar(hWndToolBar);
	this->UIAddToolBar(hWndToolBarGuiItems);
	this->UISetCheck(ID_VIEW_TOOLBAR, 1);
	this->UISetCheck(ID_VIEW_STATUS_BAR, 1);
	
	// Create client window
	this->m_hWndClient = this->m_Frame.create( m_hWnd, rcDefault);
    
	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);
	
	// Create client window list view and add it into DockSplitFrame
	m_GuiItems.Create(m_hWnd);
	m_ItemBrowser.Create(m_hWnd);
	m_PropsView.Create(m_hWnd);
	m_Output.create(m_hWnd);
	m_Output.SetReadOnly(TRUE);
	m_PropsEditor.Create(m_hWnd);
	m_TextEditor.Create(m_hWnd);
	
	// Load placement
	{
		WINDOWPLACEMENT _WndPlacement; tDWORD size = sizeof(_WndPlacement);
		if( PR_SUCC(cAutoRegKey(cAutoRegistry((hOBJECT)g_root, "HKCU\\Software\\KasperskyLab\\SkinEditor", PID_WIN32_REG), cRegRoot).get_val("WindowPlacement", tid_BINARY, &_WndPlacement, &size)) )
			SetWindowPlacement(&_WndPlacement);
	}
	
	PostMessage(WM_GSE_INITPANELS);
	return 0;
}

LRESULT CMainFrame::OnInitPanels(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	CreatePane(NewEditViewDialog::choiceRightDockPane, m_PropsView.m_hWnd, _T("Properties"));
	CreatePane(NewEditViewDialog::choiceLeftDockPane, m_GuiItems.m_hWnd, _T("Objects"));
	CreatePane(NewEditViewDialog::choiceLeftDockPane, m_ItemBrowser.m_hWnd, _T("Browser"));
	CreatePane(NewEditViewDialog::choiceBottomDockPane, m_Output.m_hWnd, _T("Output"));
	CreatePane(NewEditViewDialog::choiceBottomDockPane, m_TextEditor.m_hWnd, _T("Item Text String"));
	CreatePane(NewEditViewDialog::choiceBottomDockPane, m_PropsEditor.m_hWnd, _T("Item Properties String"));

	lookUpClientWindow(m_ItemBrowser.m_hWnd)->owner->splitClientView(m_ItemBrowser.m_hWnd, m_GuiItems.m_hWnd, SplitPane::targetBottom);

	m_Frame.setFocusTo(m_GuiItems.m_hWnd);
	return 0;
}

LRESULT CMainFrame::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	if( !CanClose() )
		return 1;
	
	// Save placement
	{
		WINDOWPLACEMENT _WndPlacement; _WndPlacement.length = sizeof(_WndPlacement);
		if( GetWindowPlacement(&_WndPlacement) )
			cAutoRegKey(cAutoRegistry((hOBJECT)g_root, "HKCU\\Software\\KasperskyLab\\SkinEditor", PID_WIN32_REG, cFALSE), cRegRoot).set_val("WindowPlacement", tid_BINARY, &_WndPlacement, _WndPlacement.length);
	}

	return bHandled = FALSE, 0;
}

LRESULT CMainFrame::OnQueryEndSession(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	return CanClose();
}

LRESULT CMainFrame::OnCreateGuiItem(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
{
	USES_CONVERSION;

	tDWORD nCtlIdx = wID - ID_GUIITEMS_CREATE_01;
	
	if( !m_Gui.GetRoot() )
		return 0;
	
	if( m_bCreateNewItem || wID == ID_GUIITEMS_CREATE_26 || wID == ID_GUIITEMS_CREATE_27 )
	{
		CGuiItemEdit * pEditor = new CGuiItemEdit(this);
		if( pEditor->Create(m_hWnd, m_Gui.GetRoot()) )
			pEditor->CreateTopItemFrom(nCtlIdx);
		if( !pEditor->m_pItem )
		{
			pEditor->DestroyWindow();
			delete pEditor, pEditor = NULL;
		}

		if( pEditor )
		{
			m_aEdits[pEditor->m_hWnd] = pEditor;
			
			CString strTip(pEditor->m_pItem->m_strSection.c_str()); strTip += _T(" item");
			CreatePane(NewEditViewDialog::choiceMainPane, pEditor->m_hWnd, A2T(pEditor->m_pItem->m_strSection.c_str()), strTip);
		}
		else
			MessageBox(_T("Error create new item"), NULL, MB_OK|MB_ICONSTOP);

		m_bCreateNewItem = 0;
	}
	else
	{
		CGuiItemEdit * pEditor = getActiveEdit();
		if( pEditor )
			pEditor->CreateItem(nCtlIdx);
	}
	
	return 0;
}

LRESULT CMainFrame::OnInsertGuiItemFromExt(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
{
	USES_CONVERSION;
	
	CGuiItemEdit * pEditor = getActiveEdit();
	if( !pEditor )
		return 0;

	CString strSection;
	{
		HTREEITEM hSel = m_GuiItems.GetSelectedItem();
		if( hSel )
		{
			if( m_GuiItems.GetItemData(hSel) != GUIITEMSLIST_EXT )
			{
				MessageBox(_T("Only external items can be inserted"), _T("Warning"), MB_OK|MB_ICONEXCLAMATION);
				return 0;
			}
			
			m_GuiItems.GetItemText(hSel, strSection);
		}
	}
	
	if( strSection.IsEmpty() )
	{
		MessageBox(_T("Please select external item from \"Objects\" browser"), _T("Warning"), MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}

	pEditor->InsertExtItem(T2A(strSection));
	return 0;
}

LRESULT CMainFrame::OnCreateGuiItemExt(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
{
	m_bCreateNewItem = !m_bCreateNewItem;
	return 0;
}

LRESULT CMainFrame::OnEditGuiItem(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
	USES_CONVERSION;
	
	HTREEITEM hSelItem = m_GuiItems.GetSelectedItem();
	
	DWORD_PTR nData = m_GuiItems.GetItemData(hSelItem);
	if( nData == GUIITEMSLIST_DLG || nData == GUIITEMSLIST_EXT )
	{
		CString strSection; m_GuiItems.GetItemText(hSelItem, strSection);
		OpenGuiItem(T2A(strSection), nData == GUIITEMSLIST_EXT);
	}
	
	return bHandled = TRUE, 0;
}

LRESULT CMainFrame::OnSelectGuiItem(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
	if( m_ItemBrowser.IsInterSel() )
		return 0;

	CGuiItemEdit * pEditor = getActiveEdit();
	if( !pEditor )
		return 0;

	pEditor->SetSelection(m_ItemBrowser.GetSel());
	return bHandled = TRUE, 0;
}

LRESULT CMainFrame::OnCloseClientView(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	HWND clientViewWnd = reinterpret_cast<HWND>(lParam);
	ClientView * clientView = this->m_Frame.getClientView(clientViewWnd);
	if( !clientView )
		return 0;
	
	this->m_Frame.detachClientView( clientViewWnd);
	// DockSplitTab::detachClientView does not call FrameListener::clientDetached event handler
	// so need to do it explicitly
	this->clientDetached( clientView);
	return 0;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

class COpenSkinDlg : public CDialogImpl<COpenSkinDlg>
{
public:
	COpenSkinDlg(bool bNew = false) : CDialogImpl<COpenSkinDlg>(), m_bNew(bNew) {}

public:
	enum {IDD = IDD_OPENSKIN};
	
	BEGIN_MSG_MAP(COpenSkinDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		
		COMMAND_ID_HANDLER(IDOK,     OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER(IDBROWSE, OnBrowse)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		USES_CONVERSION;

		m_PathEdit = GetDlgItem(IDC_PATH_EDIT);
		m_LocEdit = GetDlgItem(IDC_LOC_EDIT);

		if( m_bNew )
			SetWindowText(_T("New"));
		else
		{
			cAutoRegistry _reg((hOBJECT)g_root, "HKCU\\Software\\KasperskyLab\\SkinEditor\\LastOpenList", PID_WIN32_REG);
			cAutoRegKey _regKeyValues(_reg, cRegRoot);

			for(tDWORD i = 0, n = _regKeyValues.get_val_count(); i < n; i++)
			{
				cStrObj strPath; _regKeyValues.get_val_by_index(i, tid_STRING_OBJ, &strPath);
				if( strPath.size() )
					m_aPaths.push_back() = strPath;
			}

			if( m_aPaths.size() )
			{
				for(tDWORD i = 0; i < m_aPaths.size(); i++)
					m_PathEdit.AddString(W2T((LPWSTR)m_aPaths[i].data()));

				m_PathEdit.SetWindowText(W2T((LPWSTR)m_aPaths[0].data()));
			}
			else
				m_PathEdit.AddString(_T(""));
		}

		m_LocEdit.SetWindowText(_T("ru"));

		CenterWindow(GetParent());
		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if( wID == IDOK )
		{
			USES_CONVERSION;
			CString str;
			
			m_PathEdit.GetWindowText(str);
			m_strSkinPath = T2W((LPTSTR)str.GetString());
			
			m_LocEdit.GetWindowText(str);
			m_strLocId = T2A((LPTSTR)str.GetString());

			for(tDWORD i = 0; i < m_aPaths.size(); i++)
				if( m_aPaths[i] == m_strSkinPath )
				{
					m_aPaths.remove(i);
					break;
				}

			m_aPaths.insert(0) = m_strSkinPath;

			if( m_aPaths.size() > 15 )
				m_aPaths.resize(15);

			cAutoRegistry _reg((hOBJECT)g_root, "HKCU\\Software\\KasperskyLab\\SkinEditor", PID_WIN32_REG, cFALSE);
			cAutoRegKey _regKey(_reg, cRegRoot);
			_regKey.del_key("LastOpenList");

			cAutoRegKey _regKeyValues(_reg, _regKey, "LastOpenList", cTRUE);
			for(tDWORD i = 0; i < m_aPaths.size(); i++)
				_regKeyValues.set_val_by_index(i, tid_STRING_OBJ, &m_aPaths[i]);
		}
		
		EndDialog(wID);
		return 0;
	}

	LRESULT OnBrowse(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CFolderDialog _dlg(m_hWnd, _T("Browse for skin source"), BIF_RETURNONLYFSDIRS|BIF_USENEWUI|(m_bNew ? 0 : BIF_NONEWFOLDERBUTTON)|BIF_SHAREABLE/*|BIF_BROWSEINCLUDEFILES*/);
		if( _dlg.DoModal(m_hWnd) != IDOK )
			return 0;
		
		m_PathEdit.SetWindowText(_dlg.GetFolderPath());
		return 0;
	}

protected:
	unsigned			m_bNew : 1;
	CComboBox			m_PathEdit;
	CEdit				m_LocEdit;
	cVector<cStrObj>	m_aPaths;

public:
	cStrObj m_strSkinPath;
	tString m_strLocId;
};

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	COpenSkinDlg _dlg(true);
	if( _dlg.DoModal(m_hWnd) != IDOK )
		return 0;
	
	if( !CanClose() )
		return 0;

	CloseOpenViews();

	// Create new skin structure at _dlg.m_strSkinPath
	{
	}

	OpenSkin(_dlg.m_strSkinPath.data(), _dlg.m_strLocId.c_str());
	return 0;
}

LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	COpenSkinDlg _dlg;
	if( _dlg.DoModal(m_hWnd) != IDOK )
		return 0;
	
	if( !CanClose() )
		return 0;

	CloseOpenViews();

	OpenSkin(_dlg.m_strSkinPath.data(), _dlg.m_strLocId.c_str());
	return 0;
}

LRESULT CMainFrame::OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CGuiItemEdit * pEditor = getActiveEdit();
	if( pEditor )
		pEditor->Save();
	return 0;
}

LRESULT CMainFrame::OnFileClearOutput(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_Output.Clear();
	return 0;
}

LRESULT CMainFrame::OnFileRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if( m_Gui.GetRoot() )
		m_Gui.GetRoot()->Refresh(REFRESH_SKIN_DATA);
	GuiItem_OnSkinChanged();
	return 0;
}

LRESULT CMainFrame::OnEditCut(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	CGuiItemEdit * pEditor = getActiveEdit();
	HWND hWndFocus = ::GetFocus();
	if( !(pEditor && (pEditor->m_hWnd == hWndFocus || ::IsChild(pEditor->m_hWnd, hWndFocus) || m_ItemBrowser.m_hWnd == hWndFocus)) )
		return bHandled = FALSE, 0;
	
	pEditor->Copy(m_ClipData);
	pEditor->DeleteItem(pEditor->m_pItemCur);
	return 0;
}

LRESULT CMainFrame::OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	CGuiItemEdit * pEditor = getActiveEdit();
	HWND hWndFocus = ::GetFocus();
	if( !(pEditor && (pEditor->m_hWnd == hWndFocus || ::IsChild(pEditor->m_hWnd, hWndFocus) || m_ItemBrowser.m_hWnd == hWndFocus)) )
		return bHandled = FALSE, 0;
	
	pEditor->Copy(m_ClipData);
	return 0;
}

LRESULT CMainFrame::OnEditPaste(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	USES_CONVERSION;
	
	if( m_bCreateNewItem )
	{
		CGuiItemEdit * pEditor = new CGuiItemEdit(this);
		if( pEditor->Create(m_hWnd, m_Gui.GetRoot()) )
			pEditor->Paste(m_ClipData);
		
		if( !pEditor->m_pItem )
		{
			pEditor->DestroyWindow();
			delete pEditor, pEditor = NULL;
		}

		if( pEditor )
		{
			m_aEdits[pEditor->m_hWnd] = pEditor;
			
			CString strTip(pEditor->m_pItem->m_strSection.c_str()); strTip += _T(" item");
			CreatePane(NewEditViewDialog::choiceMainPane, pEditor->m_hWnd, A2T(pEditor->m_pItem->m_strSection.c_str()), strTip);
		}
		else
			MessageBox(_T("Error create new item"), NULL, MB_OK|MB_ICONSTOP);

		m_bCreateNewItem = 0;
	}
	else
	{
		CGuiItemEdit * pEditor = getActiveEdit();
		HWND hWndFocus = ::GetFocus();
		if( !(pEditor && (pEditor->m_hWnd == hWndFocus || ::IsChild(pEditor->m_hWnd, hWndFocus) || m_ItemBrowser.m_hWnd == hWndFocus)) )
			return bHandled = FALSE, 0;

		pEditor->Paste(m_ClipData);
	}

	return 0;
}

LRESULT CMainFrame::OnEditClear(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	CGuiItemEdit * pEditor = getActiveEdit();
	HWND hWndFocus = ::GetFocus();
	if( !(pEditor && (pEditor->m_hWnd == hWndFocus || ::IsChild(pEditor->m_hWnd, hWndFocus) || m_ItemBrowser.m_hWnd == hWndFocus)) )
		return bHandled = FALSE, 0;
	
	pEditor->DeleteItem(pEditor->m_pItemCur);
	return 0;
}

LRESULT CMainFrame::OnViewOutput( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_Frame.showDockPane(dockBOTTOM, !m_Frame.dockPaneVisible(dockBOTTOM));
	UISetCheck(ID_VIEW_OUTPUTVIEW, m_Frame.dockPaneVisible(dockBOTTOM));
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewProps( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_Frame.showDockPane(dockRIGHT, !m_Frame.dockPaneVisible(dockRIGHT));
	UISetCheck(ID_VIEW_PROPSVIEW, m_Frame.dockPaneVisible(dockRIGHT));
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewObjects( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_Frame.showDockPane(dockLEFT, !m_Frame.dockPaneVisible(dockLEFT));
	UISetCheck(ID_VIEW_ITEMSVIEW, m_Frame.dockPaneVisible(dockLEFT));
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewBrowser( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_Frame.showDockPane( dockRIGHT, !m_Frame.dockPaneVisible( dockRIGHT));
	UISetCheck(ID_VIEW_BROWSER, m_Frame.dockPaneVisible( dockRIGHT));
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewWordWrap( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CSingleTextPropEditor * pTxtEdit = NULL;
	if( m_PropsEditor.m_hWnd == ::GetFocus() )
		pTxtEdit = &m_PropsEditor;
	else if( m_TextEditor.m_hWnd == ::GetFocus() )
		pTxtEdit = &m_TextEditor;

	if( pTxtEdit )
		pTxtEdit->SetWordWrap(!pTxtEdit->GetWordWrap());
	
	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg().DoModal();
	return 0;
}

LRESULT CMainFrame::OnCtxItemEdit_OpenExternalItem(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CGuiItemEdit * pEditor = getActiveEdit();
	if( pEditor && pEditor->m_pItemCur && (pEditor->m_pItemCur->m_nFlags & STYLE_EXT_ITEM) )
		OpenGuiItem(pEditor->m_pItemCur->m_strSection.c_str(), !!(pEditor->m_pItemCur->m_nFlags & STYLE_EXT));
	
	return 0;
}

// ########################################################################

void	CMainFrame::GuiItem_OnSelChanged()
{
	std::vector<CComPtr<IDispatch> > aComItems;
	CItemBase * pItem = NULL;
	CItemBase * pSelItem = NULL;

	CGuiItemEdit * pEditor = getActiveEdit();
	if( pEditor )
	{
		pItem = pEditor->m_pItem;
		pSelItem = pEditor->m_pItemCur;
		
		CItemBase::Items aItems; pEditor->GetSelection(aItems);
		for(DWORD k = 0; k < aItems.size(); k++)
		{
			CComGuiItem * pComItem = new CComGuiItem(); pComItem->m_pEditor = pEditor; pComItem->Init(aItems[k]);
			*aComItems.insert(aComItems.end(), CComPtr<IDispatch>()) = pComItem->p();
			pComItem->Release();
		}
	}

	m_PropsView.SetObjects(aComItems.size(), aComItems.size() ? &aComItems[0].p : NULL);
	
	{
		CComGuiItem * pComItem = aComItems.size() ? (CComGuiItem *)aComItems[0].p : NULL;
		m_PropsEditor.SetObject(pComItem, pComItem ? pComItem->m_nPropStrId : -1);
		m_TextEditor.SetObject(pComItem, pComItem ? pComItem->m_nTextId : -1);
	}

	m_ItemBrowser.Update(pItem, pSelItem);
}

void	CMainFrame::GuiItem_OnDataChanged(bool bItems)
{
	m_PropsView.Update();
	m_PropsEditor.Update();
	m_TextEditor.Update();

	CGuiItemEdit * pEditor = getActiveEdit();
	if( pEditor )
		m_ItemBrowser.Update(pEditor->m_pItem, pEditor->m_pItemCur);
	else
		m_ItemBrowser.Update(NULL);
}

void	CMainFrame::GuiItem_OnSkinChanged()
{
	m_bReinitItems = 1;
	m_GuiItems.Reinit(m_Gui.GetRoot());
	m_bReinitItems = 0;
	
	m_PropsView.Update();
	m_PropsEditor.Update();
	m_TextEditor.Update();

	CGuiItemEdit * pEditor = getActiveEdit();
	if( pEditor )
		m_ItemBrowser.Update(pEditor->m_pItem, pEditor->m_pItemCur);
	else
		m_ItemBrowser.Update(NULL);
}

// ########################################################################
// ########################################################################
