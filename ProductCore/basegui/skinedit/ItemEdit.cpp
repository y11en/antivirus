#include "stdafx.h"

#include "resource.h"
#include "ItemEdit.h"
#include "MainFrm.h"

// ########################################################################
// CGuiItemEdit

static struct _NEWITEM
{
	tCHAR *		s_type;
	tCHAR *		s_idtpl;
	unsigned	s_container : 1;
	unsigned	s_top : 1;
	unsigned	s_extonly : 1;
	tCHAR *		s_props;
	tWCHAR *	s_text;
} g_GuiItemTypesInfo[] =
{
	// Simple items
	{GUI_ITEMT_BASE,			"Label",			0, 0, 0,		"sz(a,a)",									L"\"Label\""},
	{GUI_ITEMT_EDIT,			"Edit",				0, 0, 0,		"sz(50, a)",								L""},
	{GUI_ITEMT_TIMEEDIT,		"TimeEdit",			0, 0, 0,		"sz(50, a)",								L""},
	{GUI_ITEMT_RICHEDIT,		"RichEdit",			0, 0, 0,		"sz(50, 50)",								L""},
	{GUI_ITEMT_IPADDRESS,		"IpEdit",			0, 0, 0,		"sz(a, a)",									L""},
	{GUI_ITEMT_HTML,			"Html",				0, 0, 0,		"sz(50, 50)",								L""},
	{GUI_ITEMT_BUTTON,			"Button",			0, 0, 0,		"sz(75,23)",								L"\"Button\""},
	{GUI_ITEMT_BITMAPBUTTON,	"BmpButton",		0, 0, 0,		"sz(50, 50)",								L""},
	{GUI_ITEMT_CHECK,			"Check",			0, 0, 0,		"sz(a,a)",									L"\"Check\""},
	{GUI_ITEMT_RADIO,			"Radio",			0, 0, 0,		"sz(a,a)",									L"\"Radio\""},
	{GUI_ITEMT_HOTLINK,			"Hotlink",			0, 0, 0,		"sz(a,a)",									L"\"Hotlink\""},
	{GUI_ITEMT_PROGRESS,		"Progress",			0, 0, 0,		"sz(50, 20)",								L""},
	{GUI_ITEMT_COMBO,			"Combo",			0, 0, 0,		"sz(50, a)",								L""},
	{GUI_ITEMT_SLIDER,			"Slider",			0, 0, 0,		"sz(50, 50)",								L""},
	{GUI_ITEMT_LISTCTRL,		"List",				1, 0, 0,		"sz(p, p)",								L""},
	{GUI_ITEMT_TREE,			"Tree",				0, 0, 0,		"sz(50, 50)",								L""},

	// Complex items
	{0/*GUI_ITEMT_VALUE*/,			"Value",			0, 0, 0,		"sz(50, a) sep(30) v(0)",					L"Value:"},
	{0/*GUI_ITEMT_NAVIGATOR*/,		"Navigator",		0, 0, 0,		"sz(50, 50)",								L""},
	{0/*GUI_ITEMT_NAVIGATORGROUP*/,	"NavGroup",			0, 0, 0,		"sz(50, 50)",								L""},
	{GUI_ITEMT_SLIDESHOW,		"SlideShow",		0, 0, 0,		"sz(50, 50)",								L""},
	{GUI_ITEMT_CREDITS,			"Credits",			0, 0, 0,		"sz(50, 50)",								L""},
	
	// Container items
	{GUI_ITEMT_BASE,			"Frame",			1, 0, 0,		"sz(70, 70)",								L""},
	{GUI_ITEMT_GROUP,			"Group",			1, 0, 0,		"sz(70, 70) mg(7,7)",						L"\"Group\""},
	{GUI_ITEMT_BOX,				"GroupLine",		1, 0, 0,		"sz(70, 70)",								L"\"Group Line\""},
	{GUI_ITEMT_TAB,				"Tab",				1, 0, 0,		"sz(150, 150) mg(7,7)",						L""},
	
	// Top container items
	{GUI_ITEMT_PROPSHEETPAGE,	"PropSheetPage",	1, 0, 1,		"sz(p,p) mg(7,7)",							L"\"Page\""},
	{GUI_ITEMT_DIALOG,			"Dialog",			1, 1, 1,		"sz(350, 250) mg(7,7)",						L"\"Dialog\""},
	
	// Not windowed items
	{GUI_ITEMT_MENU,			"Menu",				0, 0, 0,		"",											L""},
	{GUI_ITEMT_TRAY,			"Tray",				0, 0, 0,		"",											L""},
};

// ########################################################################

CGuiItemEdit::CGuiItemEdit(CMainFrame * pFrame) :
	m_pFrame(pFrame), m_pRoot(NULL), m_pItem(NULL), m_pItemOwner(NULL), m_pItemCur(NULL), m_bDirty(0), m_bUpdateTracker(0)
{
}

CGuiItemEdit::~CGuiItemEdit()
{
}

HWND CGuiItemEdit::Create(HWND hWndParent, CRootItem * pRoot, LPCSTR strSection, bool bExt)
{
	m_pRoot = pRoot;
	
	if( !TBase::Create(hWndParent, rcDefault, NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_HSCROLL|WS_VSCROLL, WS_EX_CLIENTEDGE) )
		return NULL;

	if( !m_Designer.Create(m_hWnd, rcDefault) )
	{
		DestroyWindow();
		return NULL;
	}

	if( !strSection )
		return m_hWnd;
	
	if( bExt )
	{
		m_pItemOwner = m_pRoot->Edit_CreateExtContainer(m_hWnd);
		if( m_pItemOwner )
		{
			m_pItemOwner->Resize(500 - m_pItemOwner->m_szSize.cx, 300 - m_pItemOwner->m_szSize.cy);
			m_pItemOwner->LoadItem(NULL, m_pItem, strSection, NULL, LOAD_FLAG_EDIT);
		}

		if( m_pItem && !m_pItem->m_pEditData )
			m_pItem->Destroy(), m_pItem = NULL;
		
		if( m_pItem )
		{
			if( !(m_pItem->m_nFlags & AUTOSIZE_PARENT_X) )
				m_pItemOwner->m_nFlags |= AUTOSIZE_X;

			if( !(m_pItem->m_nFlags & AUTOSIZE_PARENT_Y) )
				m_pItemOwner->m_nFlags |= AUTOSIZE_Y;

			m_pItem->RecalcLayout();
		}
	}
	else
	{
		m_pRoot->LoadItem(NULL, m_pItem, strSection, NULL, LOAD_FLAG_EDIT);
		m_pItemOwner = m_pItem;
		if( m_pItemOwner )
			::SetParent(m_pItemOwner->GetWindow(), m_hWnd);

		memset(&m_pItem->m_rcShift, 0, sizeof(m_pItem->m_rcShift));
	}
	
	if( !m_pItem )
	{
		DestroyWindow();
		return NULL;
	}
	
	::SetWindowPos(m_pItemOwner->GetWindow(), NULL, m_Designer.m_Tracker.GetWidth(), m_Designer.m_Tracker.GetWidth(), 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOOWNERZORDER);
	::ShowWindow(m_pItemOwner->GetWindow(), SW_SHOWNOACTIVATE);

	m_Designer.BringWindowToTop();

	m_pItemCur = m_pItem;
	Update();
	return m_hWnd;
}

void CGuiItemEdit::Update()
{
	m_pRoot->Edit_UpdateItemProps(m_pItem, m_pItemCur, guiepmUpdateEditable);

	USHORT trStyle = CTracker::TSF_APPEALBORDER|CTracker::TSF_TRACKALL;
	if( m_pItemCur )
	{
		tQWORD nFlags = m_pItemCur->m_nFlags;
		if( nFlags & (AUTOSIZE|AUTOSIZE_PARENT) )
			trStyle &= ~(CTracker::TSF_TOPLEFT|CTracker::TSF_TOPRIGHT|CTracker::TSF_BOTTOMLEFT|CTracker::TSF_BOTTOMRIGHT);
		if( nFlags & (AUTOSIZE_X|AUTOSIZE_PARENT_X) )
			trStyle &= ~(CTracker::TSF_LEFT|CTracker::TSF_RIGHT);
		if( nFlags & (AUTOSIZE_Y|AUTOSIZE_PARENT_Y) )
			trStyle &= ~(CTracker::TSF_TOP|CTracker::TSF_BOTTOM);

		if( m_pItemCur == m_pItem )
		{
			// В случае топового элемента разрешаем изменение только размеров
			trStyle &= ~(CTracker::TSF_TOPLEFT|CTracker::TSF_TOPRIGHT|CTracker::TSF_BOTTOMLEFT|CTracker::TSF_LEFT|CTracker::TSF_TOP);

			// Для возможности резайза внешнего элемента, выровненного по родителю
			if( nFlags & AUTOSIZE_PARENT_X )
				trStyle |= CTracker::TSF_RIGHT;
			if( nFlags & AUTOSIZE_PARENT_Y )
				trStyle |= CTracker::TSF_BOTTOM;
			if( (nFlags & AUTOSIZE_PARENT) == AUTOSIZE_PARENT )
				trStyle |= CTracker::TSF_BOTTOMRIGHT;
		}
	}

	m_Designer.m_Tracker.SetStyle(trStyle);

	Invalidate();
	UpdateWindow();
	
	::RedrawWindow(m_pItemOwner->GetWindow(), NULL, NULL, RDW_ERASE|RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);

	m_Designer.Invalidate();
	m_Designer.UpdateWindow();
}

void	CGuiItemEdit::GetSelection(CItemBase::Items& pItems)
{
	pItems.push_back(m_pItemCur);
}

void	CGuiItemEdit::SetSelection(tPTR pItem)
{
	_SelectItem(m_pItem, pItem);
}

bool	CGuiItemEdit::DeleteItem(CItemBase * pItem)
{
	if( !m_pItem )
		return false;
	
	CItemBase * pSelItem = m_pItem->m_pEditData->DeleteItem(pItem);
	if( !pSelItem )
		return false;

	m_pItemCur = pSelItem;
	Update();
	
	m_pFrame->GuiItem_OnDataChanged(true);
	m_pFrame->GuiItem_OnSelChanged();
	m_bDirty = 1;
	return true;
}

void	CGuiItemEdit::ItemPropsChanged(CItemBase * pItem, bool bSetDirty, bool bPropStr)
{
	if( pItem == m_pItem && m_pItem != m_pItemOwner )
	{
		CItemPropVals& l_prpSize = pItem->m_pEditData->m_Props.Get(STR_PID_SIZE);
		
		if( l_prpSize.Get(0) == "p" )
			m_pItemOwner->m_nFlags &= ~(AUTOSIZE_X);
		else
			m_pItemOwner->m_nFlags |= AUTOSIZE_X;

		if( l_prpSize.Get(1) == "p" )
			m_pItemOwner->m_nFlags &= ~(AUTOSIZE_Y);
		else
			m_pItemOwner->m_nFlags |= AUTOSIZE_Y;
	}
	
	pItem->m_pRoot->Edit_UpdateItemProps(m_pItem, pItem, bPropStr ? guiepmUpdateFromString : guiepmUpdate);

	Update();
	if( bSetDirty )
		m_bDirty = 1;
	m_pFrame->GuiItem_OnDataChanged(false);
}

void	CGuiItemEdit::ResizeItem(CItemBase * pItem, const RECT& rcOld, const RECT& rcNew)
{
	POINT bOwnResize =
	{
		!(pItem->m_nFlags & (AUTOSIZE_PARENT_X|AUTOSIZE_X)),
		!(pItem->m_nFlags & (AUTOSIZE_PARENT_Y|AUTOSIZE_Y))
	};
	
	tCHAR szNum[20];

	if( pItem == m_pItem && m_pItem != m_pItemOwner && (pItem->m_nFlags & AUTOSIZE_PARENT) )
		m_pItemOwner->Resize(
			!bOwnResize.x ? (RECT_CX(rcNew) - RECT_CX(rcOld)) : 0,
			!bOwnResize.y ? (RECT_CY(rcNew) - RECT_CY(rcOld)) : 0);
	
	POINT ptOffsetDelta;
	if( pItem->m_nFlags & ALIGN_REVERSE_X )
		ptOffsetDelta.x = rcOld.right - rcNew.right;
	else
		ptOffsetDelta.x = rcNew.left - rcOld.left;
	if( pItem->m_nFlags & ALIGN_REVERSE_Y )
		ptOffsetDelta.y = rcOld.bottom - rcNew.bottom;
	else
		ptOffsetDelta.y = rcNew.top - rcOld.top;
	
	if( ptOffsetDelta.x || ptOffsetDelta.y )
	{
		CItemPropVals& prpOffset = pItem->m_pEditData->m_Props.Get(STR_PID_OFFSET, true);
		POINT ptNew =
		{
			(LONG)prpOffset.GetLong(0) + ptOffsetDelta.x,
			(LONG)prpOffset.GetLong(1) + ptOffsetDelta.y
		};
		
		prpOffset.Set(0, ptNew.x ? _ltoa(ptNew.x, szNum, 10) : "");
		prpOffset.Set(1, ptNew.y ? _ltoa(ptNew.y, szNum, 10) : "");
	}

	CItemPropVals& prpSize = pItem->m_pEditData->m_Props.Get(STR_PID_SIZE, true);
	if( !(pItem->m_nFlags & (AUTOSIZE_PARENT_X|AUTOSIZE_X)) )
		prpSize.Set(0, _ltoa(RECT_CX(rcNew), szNum, 10));
	if( !(pItem->m_nFlags & (AUTOSIZE_PARENT_Y|AUTOSIZE_Y)) )
		prpSize.Set(1, _ltoa(RECT_CY(rcNew), szNum, 10));

	ItemPropsChanged(pItem, bOwnResize.x || bOwnResize.y || ptOffsetDelta.x || ptOffsetDelta.y);
}

void	CGuiItemEdit::CreateItem(tDWORD nIdx)
{
	_USES_CONVERSION(CP_UTF8);

	if( nIdx >= countof(g_GuiItemTypesInfo) )
		return;
	
	if( !m_pItemCur )
		return;
	
	if( m_pItemCur->m_pEditData->IsLinked() )
		return;

	if( m_pItemCur->m_pEditData->IsOverridable() )
	{
		MessageBox(_T("Sorry, feature of override item by not external item not implemented yet"), _T("Warning"), MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	
	CItemBase * pList = NULL;
	for( CItemBase * p = m_pItemCur; p; p = p->m_pParent )
		if( p->m_strItemType == GUI_ITEMT_LISTCTRL )
		{
			pList = p;
			break;
		}
	
	if( m_pItemCur->m_strItemType != GUI_ITEMT_GROUP &&
		m_pItemCur->m_strItemType != GUI_ITEMT_BOX &&
		m_pItemCur->m_strItemType != GUI_ITEMT_TAB &&
		m_pItemCur->m_strItemType != GUI_ITEMT_DIALOG &&
		!pList &&
		!(m_pItemCur->m_strItemType == GUI_ITEMT_BASE && !m_pItemCur->m_strSection.empty()) )
	{
		MessageBox(_T("Can't insert child item to non container item"), _T("Error"), MB_OK|MB_ICONSTOP);
		return;
	}
	
	_NEWITEM& _newitem = g_GuiItemTypesInfo[nIdx];
	if( _newitem.s_extonly )
		return;
	
	CString strItemId; _GetNewId(m_pItemCur, A2T(_newitem.s_idtpl), false, strItemId);

	tString strProps;
	if( *_newitem.s_type )
	{
		strProps += "t(";
		strProps += _newitem.s_type;
		strProps += ") ";
	}
	strProps += _newitem.s_props;
	if( !pList && !_newitem.s_container && m_pItemCur->m_aItems.size() )
		strProps += "a(la) xy(,7)";
	
	if( _newitem.s_container )
	{
		if( strProps.size() )
			strProps += " ";

		strProps += "[_skinedit__new_item_]";
		/*		
		strProps += "[";
		strProps += m_pItemCur->m_strSection;
		strProps += "_";
		strProps += pItem->m_strItemId;
		strProps += "]";
		*/
	}

	CItemBase * pNewItem = m_pRoot->Edit_CreateItem(m_pItem, m_pItemCur, T2A(strItemId.GetString()), strProps.c_str(), W2A(_newitem.s_text));
	if( !pNewItem )
	{
		MessageBox(_T("Can't create item"), _T("Error"), MB_OK|MB_TOPMOST|MB_ICONSTOP|MB_APPLMODAL|MB_SETFOREGROUND);
		return;
	}
	
	if( pNewItem->m_pParent && pNewItem->m_pParent->m_pEditData && pNewItem->m_pParent->m_strSection.empty() )
		pNewItem->m_pParent->m_strSection = pNewItem->m_pParent->m_pEditData->m_strSection = "[_skinedit__new_item_]";

	_SelectItem(m_pItem, pNewItem);
	Update();
	m_bDirty = 1;
	
	m_pFrame->GuiItem_OnDataChanged(true);
	m_pFrame->GuiItem_OnSelChanged();
}

void	CGuiItemEdit::InsertExtItem(LPCSTR strSection)
{
	USES_CONVERSION;
	
	CItemBase * pList = NULL;
	for( CItemBase * p = m_pItemCur; p; p = p->m_pParent )
		if( p->m_strItemType == GUI_ITEMT_LISTCTRL )
		{
			pList = p;
			break;
		}

	if( m_pItemCur->m_strItemType != GUI_ITEMT_GROUP &&
		m_pItemCur->m_strItemType != GUI_ITEMT_BOX &&
		m_pItemCur->m_strItemType != GUI_ITEMT_TAB &&
		m_pItemCur->m_strItemType != GUI_ITEMT_DIALOG &&
		m_pItemCur->m_strItemType != GUI_ITEMT_BASE &&
		!pList )
	{
		MessageBox(_T("Can't insert child item into non container item"), _T("Error"), MB_OK|MB_ICONSTOP);
		return;
	}
	
	if( m_pItem->m_strSection == strSection )
	{
		MessageBox(_T("Can't insert item itself"), _T("Error"), MB_OK|MB_ICONSTOP);
		return;
	}

	CString strItemId; _GetNewId(m_pItemCur, _T("Frame"), false, strItemId);
	tString strProps;
	strProps += "[";
	strProps += strSection;
	strProps += "]";
	
	CItemBase * pNewItem = m_pRoot->Edit_CreateItem(m_pItem, m_pItemCur, T2A(strItemId.GetString()), strProps.c_str());
	if( !pNewItem )
	{
		MessageBox(_T("Can't insert external item"), _T("Error"), MB_OK|MB_TOPMOST|MB_ICONSTOP|MB_APPLMODAL|MB_SETFOREGROUND);
		return;
	}
	
	if( pNewItem->m_pParent && pNewItem->m_pParent->m_pEditData && pNewItem->m_pParent->m_strSection.empty() )
		pNewItem->m_pParent->m_strSection = pNewItem->m_pParent->m_pEditData->m_strSection = "[_skinedit__new_item_]";
	
	_SelectItem(m_pItem, pNewItem);
	Update();
	m_bDirty = 1;

	m_pFrame->GuiItem_OnDataChanged(true);
	m_pFrame->GuiItem_OnSelChanged();
}

void	CGuiItemEdit::CreateTopItemFrom(tDWORD nIdx)
{
	_USES_CONVERSION(CP_UTF8);

	if( nIdx >= countof(g_GuiItemTypesInfo) )
		return;
	
	_NEWITEM& _newitem = g_GuiItemTypesInfo[nIdx];

	CString strSection; _GetNewSection(A2T(_newitem.s_idtpl), strSection);

	tString strProps;
	if( *_newitem.s_type )
	{
		strProps += "t(";
		strProps += _newitem.s_type;
		strProps += ") ";
	}
	strProps += _newitem.s_props;
	
	if( !_newitem.s_top )
	{
		m_pItemOwner = m_pRoot->Edit_CreateExtContainer(m_hWnd);
		if( m_pItemOwner )
		{
			m_pItemOwner->Resize(500 - m_pItemOwner->m_szSize.cx, 300 - m_pItemOwner->m_szSize.cy);
			m_pItemOwner->LoadItem(NULL, m_pItem, strProps.c_str(), T2A(strSection.GetString()), LOAD_FLAG_INFO|LOAD_FLAG_EDIT);
		}

		if( m_pItem )
		{
			if( !(m_pItem->m_nFlags & AUTOSIZE_PARENT_X) )
				m_pItemOwner->m_nFlags |= AUTOSIZE_X;

			if( !(m_pItem->m_nFlags & AUTOSIZE_PARENT_Y) )
				m_pItemOwner->m_nFlags |= AUTOSIZE_Y;

			m_pItem->RecalcLayout();
			
			m_pItem->m_nFlags |= STYLE_EXT;
			m_pItem->m_pEditData->m_bExtProp = 1;
		}
	}
	else
	{
		m_pRoot->LoadItem(NULL, m_pItem, strProps.c_str(), T2A(strSection.GetString()), LOAD_FLAG_INFO|LOAD_FLAG_EDIT);
		m_pItemOwner = m_pItem;
		if( m_pItemOwner )
			::SetParent(m_pItemOwner->GetWindow(), m_hWnd);
	}

	if( m_pItem )
	{
		m_pItem->m_strSection = T2A(strSection.GetString());
		m_pItem->m_pEditData->m_strSection = T2A(strSection.GetString());
		m_pItem->m_pEditData->m_strProps = strProps;
		m_pItem->m_pEditData->m_strText += W2A(_newitem.s_text);
		
		m_pRoot->Edit_UpdateItemProps(m_pItem, m_pItem);
	}
	
	::SetWindowPos(m_pItemOwner->GetWindow(), NULL, m_Designer.m_Tracker.GetWidth(), m_Designer.m_Tracker.GetWidth(), 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOOWNERZORDER);
	::ShowWindow(m_pItemOwner->GetWindow(), SW_SHOWNOACTIVATE);

	m_Designer.BringWindowToTop();

	m_pItemCur = m_pItem;
	Update();
	m_bDirty = 1;
}

bool	CGuiItemEdit::Save()
{
	USES_CONVERSION;
	
	if( !m_bDirty )
		return true;

	if( m_pItem->m_pEditData->m_strGroup.empty() )
	{
		CString strText; strText.Format(_T("Please specify the group name for \"%s\" item."), A2T(m_pItem->m_strSection.c_str()));
		MessageBox(strText, _T("Error"), MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	
	tERROR err = m_pItem->m_pRoot->SaveToSkin(m_pItem);
	if( PR_FAIL(err) )
	{
		m_pFrame->ShowError(_T("Save error"), err);
		return false;
	}

	m_bDirty = 0;
	m_pFrame->GuiItem_OnSkinChanged();
	return true;
}

void	CGuiItemEdit::Copy(CItemEditData::Cd& _data)
{
	m_pItem->m_pEditData->CopyData(m_pItemCur, _data);
}

void	CGuiItemEdit::Paste(CItemEditData::Cd& _data)
{
	USES_CONVERSION;
	
	if( !m_pItemCur )
	{
		CString strSection; _GetNewSection(A2T(_data.m_strId.c_str()), strSection);

		bool bTop;
		{
			LPCSTR strIncl[] = {STR_PID_TYPE, NULL};
			CItemProps _Props(_data.m_strProps.c_str(), tString(), strIncl);
			bTop = _Props.Get(STR_PID_TYPE).Get() == GUI_ITEMT_DIALOG;
		}

		if( !bTop )
		{
			m_pItemOwner = m_pRoot->Edit_CreateExtContainer(m_hWnd);
			if( m_pItemOwner )
			{
				m_pItemOwner->Resize(500 - m_pItemOwner->m_szSize.cx, 300 - m_pItemOwner->m_szSize.cy);
				m_pItem = m_pRoot->Edit_CreateFromData(NULL, m_pItemOwner, _data, T2A(strSection.GetString()));
			}

			if( m_pItem )
			{
				if( !(m_pItem->m_nFlags & AUTOSIZE_PARENT_X) )
					m_pItemOwner->m_nFlags |= AUTOSIZE_X;

				if( !(m_pItem->m_nFlags & AUTOSIZE_PARENT_Y) )
					m_pItemOwner->m_nFlags |= AUTOSIZE_Y;

				m_pItem->RecalcLayout();
				
				m_pItem->m_nFlags |= STYLE_EXT;
				m_pItem->m_pEditData->m_bExtProp = 1;
			}
		}
		else
		{
			m_pItem = m_pRoot->Edit_CreateFromData(NULL, NULL, _data, T2A(strSection.GetString()));
			m_pItemOwner = m_pItem;
			if( m_pItemOwner )
				::SetParent(m_pItemOwner->GetWindow(), m_hWnd);
		}

		if( m_pItem )
			m_pItem->m_strSection = T2A(strSection.GetString());
		
		::SetWindowPos(m_pItemOwner->GetWindow(), NULL, m_Designer.m_Tracker.GetWidth(), m_Designer.m_Tracker.GetWidth(), 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOOWNERZORDER);
		::ShowWindow(m_pItemOwner->GetWindow(), SW_SHOWNOACTIVATE);

		m_Designer.BringWindowToTop();

		m_pItemCur = m_pItem;
		Update();
		m_bDirty = 1;
		return;
	}
	
	if( m_pItemCur->m_pEditData->IsOverridable() )
	{
		MessageBox(_T("Sorry, feature of override item by not external item not implemented yet"), _T("Warning"), MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	
	CItemBase * pList = NULL;
	for( CItemBase * p = m_pItemCur; p; p = p->m_pParent )
		if( p->m_strItemType == GUI_ITEMT_LISTCTRL )
		{
			pList = p;
			break;
		}

	if( m_pItemCur->m_strItemType != GUI_ITEMT_GROUP &&
		m_pItemCur->m_strItemType != GUI_ITEMT_BOX &&
		m_pItemCur->m_strItemType != GUI_ITEMT_TAB &&
		m_pItemCur->m_strItemType != GUI_ITEMT_DIALOG &&
		!pList &&
		!(m_pItemCur->m_strItemType == GUI_ITEMT_BASE && !m_pItemCur->m_strSection.empty()) )
	{
		MessageBox(_T("Can't insert child item into non container item"), _T("Error"), MB_OK|MB_ICONSTOP);
		return;
	}

	CString strItemId; _GetNewId(m_pItemCur, A2T(_data.m_strId.c_str()), true, strItemId);

	CItemBase * pNewItem = m_pRoot->Edit_CreateFromData(m_pItem, m_pItemCur, _data, T2A(strItemId.GetString()));
	if( !pNewItem )
	{
		MessageBox(_T("Can't paste item"), _T("Error"), MB_OK|MB_TOPMOST|MB_ICONSTOP|MB_APPLMODAL|MB_SETFOREGROUND);
		return;
	}
	
	if( pNewItem->m_pParent && pNewItem->m_pParent->m_pEditData && pNewItem->m_pParent->m_strSection.empty() )
		pNewItem->m_pParent->m_strSection = pNewItem->m_pParent->m_pEditData->m_strSection = "[_skinedit__new_item_]";
	
	_SelectItem(m_pItem, pNewItem);
	Update();
	m_bDirty = 1;

	m_pFrame->GuiItem_OnDataChanged(true);
	m_pFrame->GuiItem_OnSelChanged();
}

// ########################################################################

bool	CGuiItemEdit::_SelectItem(CItemBase * pItem, tPTR pSelItem)
{
	if( (tPTR)pItem == pSelItem )
	{
		m_pItemCur = pItem;
		Update();

		m_pFrame->GuiItem_OnSelChanged();
		return true;
	}

	for(tDWORD i = 0; i < pItem->m_aItems.size(); i++)
	{
		CItemBase * p = pItem->m_aItems[i];
		if( _SelectItem(p, pSelItem) )
		{
			if( pItem->m_nFlags & STYLE_SHEET_PAGES )
			{
				if( CItemBase * pSheet = pItem->GetByFlagsAndState(STYLE_SHEET_ITEM, ISTATE_ALL, NULL, CItemBase::gbfasUpToParent) )
					pSheet->SetCurPage(p);
			}
			else
			{
				// Скрываем элементы, точно совпадающие по размерам с выделенным
				CItemBase * pSel = p;
				pSel->Show(true, SHOWITEM_DIRECT);
				RECT rcSel; pSel->GetParentRect(rcSel, pItem);
				
				for(i = 0; i < pItem->m_aItems.size(); i++)
				{
					p = pItem->m_aItems[i];
					if( p == pSel )
						continue;
					
					RECT rcTest; p->GetParentRect(rcTest, pItem);
					if( rcTest.left == rcSel.left && rcTest.right == rcSel.right && rcTest.top == rcSel.top && rcTest.bottom == rcSel.bottom )
						p->Show(false, SHOWITEM_DIRECT);
				}
			}
			
			return true;
		}
	}

	return false;
}

bool	CGuiItemEdit::_IsSectExist(LPCTSTR strSection)
{
	USES_CONVERSION;
	
	if( m_pRoot->IsSectionExists(PROFILE_LAYOUT, T2A(strSection)) )
		return true;
	
	return false;
}

void	CGuiItemEdit::_GetNewId(CItemBase * pParent, LPCTSTR strPrefix, bool bTrySelf, CString& strItemId)
{
	for(int i = bTrySelf ? 0 : 1; i < 1000000; i++)
	{
		if( i )
			strItemId.Format(_T("%s%u"), strPrefix, i);
		else
			strItemId.Format(_T("%s"), strPrefix);
		
		tDWORD k;
		for(k = 0; k < pParent->m_aItems.size(); k++)
		{
			CItemBase * p = pParent->m_aItems[k];
			if( p->m_pEditData && !strItemId.Compare(CString(p->m_pEditData->m_strItemId.c_str())) )
				break;
		}

		if( k == pParent->m_aItems.size() )
			break;
	}
}

void	CGuiItemEdit::_GetNewSection(LPCTSTR strPrefix, CString& strSection)
{
	for(int i = 1; i < 1000000; i++)
	{
		strSection.Format(_T("%s%u"), strPrefix, i);
		if( !_IsSectExist(strSection.GetString()) )
			break;
	}
}

// ########################################################################

LRESULT CGuiItemEdit::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// do not call default event handler !!!
	USES_CONVERSION;
	
	if( !m_bDirty )
		return 0;

	CString strText; strText.Format(_T("Do you want to save chenges to the \"%s\"?"), A2T(m_pItem->m_strSection.c_str()));
	int nRes = MessageBox(strText, _T("Save"), MB_YESNOCANCEL|MB_TOPMOST|MB_ICONQUESTION|MB_APPLMODAL|MB_SETFOREGROUND);
	if( nRes == IDCANCEL )
		return 1;

	if( nRes == IDYES && !Save() )
		return 1;
	
	return 0;
}

LRESULT CGuiItemEdit::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	WORD cx = LOWORD(lParam);
	WORD cy = HIWORD(lParam);
	
	if( m_Designer.m_hWnd )
		m_Designer.SetWindowPos(HWND_TOP, 0, 0, cx, cy, SWP_SHOWWINDOW);
	return 0;
}

LRESULT CGuiItemEdit::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( m_pItem )
	{
		if( (m_pItem->m_nFlags & STYLE_TOP_ITEM) && m_pItem->m_strItemType == GUI_ITEMT_DIALOG )
			((CDialogItem *)m_pItem)->Close();
		else
			m_pItem->Destroy();
	}

	return 0;
}

LRESULT CGuiItemEdit::OnPaint(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
	m_bUpdateTracker = 1;
	bHandled = FALSE;
	return 0;
}

LRESULT CGuiItemEdit::OnChildActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_pFrame->GuiItem_OnSelChanged();
	return bHandled = FALSE, 0;
}

// ########################################################################
// CDesigner

#define ID_TIMER_DESIGNER 100

LRESULT CGuiItemEdit::CDesigner::OnPaint(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
	{
		CPaintDC dc(m_hWnd);
		m_Tracker.Draw(dc);
	}
    
	CGuiItemEdit * pEdit = _OWNEROFMEMBER_PTR(CGuiItemEdit, m_Designer);
	
	if( pEdit->m_bUpdateTracker )
		SetTimer(ID_TIMER_DESIGNER, 0);
	return 0;
}

LRESULT CGuiItemEdit::CDesigner::OnLButtonDown(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	CGuiItemEdit * pEdit = _OWNEROFMEMBER_PTR(CGuiItemEdit, m_Designer);

	CTracker::TrackerHit hitTest = m_Tracker.HitTest(pt);
	if( hitTest == CTracker::hitMiddle || hitTest == CTracker::hitNothing )
	{
		RECT rcEditor; ::GetWindowRect(m_hWnd, &rcEditor);
		POINT ptOwnerCli = {0, 0}; ::ClientToScreen(pEdit->m_pItemOwner->GetWindow(), &ptOwnerCli);
		POINT ptTest = {pt.x - (ptOwnerCli.x - rcEditor.left), pt.y - (ptOwnerCli.y - rcEditor.top)};

		CItemBase * pItem = pEdit->m_pItem->HitTest(ptTest);
		if( pItem )
		{
			// Предотвращение выделения нередактируемых элементов
			while( !pItem->m_pEditData )
				pItem = pItem->m_pParent;
		}
		
		if( !pItem )
			pItem = pEdit->m_pItem;
		
		if( pEdit->m_pItemCur != pItem )
		{
			pEdit->m_pItemCur = pItem;
			pEdit->Update();

			pEdit->m_pFrame->GuiItem_OnSelChanged();
		}
		else if( pEdit->m_pItemCur )
		{
			if( pEdit->m_pItemCur->m_strItemType == GUI_ITEMT_TAB )
			{
				RECT rcTab; pEdit->m_pItemCur->GetParentRect(rcTab, pEdit->m_pItem);
				::SendMessage(pEdit->m_pItemCur->GetWindow(), WM_LBUTTONDOWN, 0, (LPARAM)MAKELONG(ptTest.x - rcTab.left, ptTest.y - rcTab.top));
			}
		}
	}
	else if( hitTest != CTracker::hitBorder || pEdit->m_pItemCur != pEdit->m_pItem )
		m_Tracker.Track(m_hWnd, pt, false);
	
	bHandled = FALSE;
	BringWindowToTop();
	SetFocus();
	return 0;
}

LRESULT CGuiItemEdit::CDesigner::OnLButtonDblClk(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	CGuiItemEdit * pEdit = _OWNEROFMEMBER_PTR(CGuiItemEdit, m_Designer);

	if( pEdit->m_pItemCur )
	{
		CItemBase * pClickable = pEdit->m_pItemCur->GetByFlagsAndState(STYLE_CLICKABLE, ISTATE_ALL, NULL, CItemBase::gbfasUpToParent);
		if( pClickable )
		{
			pClickable->Select();

			BringWindowToTop();
			SetFocus();
			pEdit->Update();
		}
	}

	return bHandled = FALSE, 0;
}

LRESULT CGuiItemEdit::CDesigner::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CGuiItemEdit * pEdit = _OWNEROFMEMBER_PTR(CGuiItemEdit, m_Designer);

	if( wParam == VK_DELETE )
		pEdit->DeleteItem(pEdit->m_pItemCur);
	
	return bHandled = TRUE, 0;
}

// Обеспечиваем правильную смену курсора при его проходе над рамкой выделения
LRESULT CGuiItemEdit::CDesigner::OnSetCursor(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;   // обработчик по умолчанию не помешает

	CGuiItemEdit * pEdit = _OWNEROFMEMBER_PTR(CGuiItemEdit, m_Designer);

	// Проверяем корректность вызова
    if((HWND)wParam != m_hWnd ||  LOWORD(lParam) != HTCLIENT)
        return 0;

    // Курсор зависит от области окна
    CPoint ptCursor; ::GetCursorPos(&ptCursor); // выполняем преобразование координат
    ScreenToClient(&ptCursor);

    CTracker::TrackerHit hitTest = m_Tracker.HitTest(ptCursor);
	if( hitTest != CTracker::hitMiddle )
	{
		if( hitTest == CTracker::hitBorder && pEdit->m_pItemCur == pEdit->m_pItem )
			return 0;
		
		bHandled = m_Tracker.SetCursor(hitTest);
    }

    return 0;
}

// "Дорисовка" рамки, для ее более корректного отображения
LRESULT CGuiItemEdit::CDesigner::OnTimer(UINT, WPARAM wParam, LPARAM, BOOL& bHandled)
{
	if( wParam == ID_TIMER_DESIGNER )
	{
		CGuiItemEdit * pEdit = _OWNEROFMEMBER_PTR(CGuiItemEdit, m_Designer);

		KillTimer(wParam);
		pEdit->m_bUpdateTracker = 0;
		
		Invalidate(); UpdateWindow();
	}
    
	bHandled = TRUE;
    return 0;
}

LRESULT CGuiItemEdit::CDesigner::OnCtxMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CGuiItemEdit * pEdit = _OWNEROFMEMBER_PTR(CGuiItemEdit, m_Designer);

	CMenu popupMenu;
	CMenu menu; menu.LoadMenu(IDR_MENU_POPUPS);
	popupMenu.Attach(menu.GetSubMenu(0));

	popupMenu.EnableMenuItem(ID_CTXITEMEDIT_OPENEXTERNALITEM, MF_BYCOMMAND|((pEdit->m_pItemCur && pEdit->m_pItemCur != pEdit->m_pItem && (pEdit->m_pItemCur->m_nFlags & STYLE_EXT_ITEM)) ? MF_ENABLED : MF_GRAYED));
	
	popupMenu.TrackPopupMenu(0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), pEdit->m_pFrame->m_hWnd);
	return 0;
}

// Вызывается в процессе изменения размера track-контура до его отрисовки [из функции Track]
void CGuiItemEdit::CDesigner::Tracker_OnChangingRect(/*[in][out]*/ CRect& rc, CTracker::TrackerHit hitTest)
{
}

// Вызывается при отрисовке рамки
void CGuiItemEdit::CDesigner::Tracker_OnGetDrawingRect(/*[out]*/ CRect& rc)
{
	CGuiItemEdit * pEdit = _OWNEROFMEMBER_PTR(CGuiItemEdit, m_Designer);
	
	RECT rcItem; pEdit->m_pItemCur->GetParentRect(rcItem, pEdit->m_pItemOwner);
	rc = rcItem;
	
	RECT rcEditor; ::GetWindowRect(m_hWnd, &rcEditor);
	POINT ptOwnerCli = {0, 0}; ::ClientToScreen(pEdit->m_pItemOwner->GetWindow(), &ptOwnerCli);
	rc.OffsetRect(ptOwnerCli.x - rcEditor.left, ptOwnerCli.y - rcEditor.top);
}

// Вызывается после изменения размеров рамки
void CGuiItemEdit::CDesigner::Tracker_OnChangedRect(const CRect& rcOld, const CRect& rcNew)
{
	CGuiItemEdit * pEdit = _OWNEROFMEMBER_PTR(CGuiItemEdit, m_Designer);
	pEdit->ResizeItem(pEdit->m_pItemCur, rcOld, rcNew);
}

// Вызывается перед отображением/стиранием track-контура
void CGuiItemEdit::CDesigner::Tracker_OnDrawTrackerRect(CDC& dc, const CRect& rc)
{
}

// ########################################################################
// CGuiItemEdit::CTracker

HCURSOR		CGuiItemEdit::CTracker::_hCursors[6] = {NULL, };
const BYTE	CGuiItemEdit::CTracker::uHatchSpace = 3;
CBitmap		CGuiItemEdit::CTracker::_bmpBack;

// ########################################################################

#define _LC(x) x
#define _SHD (m_nStyle&TSF_HATCHEDBORDER ? 1 : 0)
#define ID_TIMER_TRACK 0xFFEFF

CGuiItemEdit::CTracker::CTracker(BYTE width) :
	m_nStyle(TSF_APPEALBORDER|TSF_TRACKALL),
	m_rect(0,0,0,0), uWidth(width)
{
	// При необходимости подгружаем требуемые курсоры
	if( !_hCursors[0] )
	{
		HINSTANCE hInst = _Module.GetResourceInstance();

		_hCursors[0] = ::LoadCursor(hInst, MAKEINTRESOURCE(IDC_TRACKNWSE)); // topleft, bottomright
		_hCursors[1] = ::LoadCursor(hInst, MAKEINTRESOURCE(IDC_TRACKNESW)); // topright, bottomleft
		_hCursors[2] = ::LoadCursor(hInst, MAKEINTRESOURCE(IDC_TRACKNS));   // top, bottom
		_hCursors[3] = ::LoadCursor(hInst, MAKEINTRESOURCE(IDC_TRACKWE));   // left, right
		_hCursors[4] = ::LoadCursor(hInst, MAKEINTRESOURCE(IDC_TRACKALL));  // middle
		_hCursors[5] = ::LoadCursor(hInst, MAKEINTRESOURCE(IDC_TRACKDRAG)); // drag
	}

	// При необходимости создаем bitmap фона
	if( _bmpBack.IsNull() )
	{
		CDC dc = ::GetDC(NULL);
		ATLASSERT(!dc.IsNull());

		createBackBitmap(dc, _bmpBack);

		ATLASSERT(!_bmpBack.IsNull());
		::ReleaseDC(NULL, dc.Detach());
	}
}

///////////////////////////////////////
// Рисуем рамку [как в визуальном С++]
void CGuiItemEdit::CTracker::Draw(CDC& dc)
{
	// Определяем рисуемый прямоугольник
	_OWNEROFMEMBER_PTR(CDesigner, m_Tracker)->Tracker_OnGetDrawingRect(m_rect);
//	if(m_rect.IsRectEmpty()) return;        // а есть ли вокруг чего рисовать рамку
	CPoint ptCenter(m_rect.CenterPoint());  // центр прямоугольника

	// Задаем кисть и рисуем фон рамки
	CBrush brBack;
	if(m_nStyle&TSF_HATCHEDBORDER)
		brBack.CreatePatternBrush(_LC(_bmpBack));
	else
		brBack.Attach(::GetSysColorBrush(COLOR_WINDOW));

	dc.FillRect(CRect(m_rect.left, m_rect.top - uWidth + _SHD, ptCenter.x - uWidth/2, m_rect.top), _LC(brBack));
	dc.FillRect(CRect(ptCenter.x + uWidth/2, m_rect.top - uWidth + _SHD, m_rect.right, m_rect.top), _LC(brBack));

	dc.FillRect(CRect(m_rect.left - uWidth + _SHD, m_rect.top, m_rect.left, ptCenter.y - uWidth/2), _LC(brBack));
	dc.FillRect(CRect(m_rect.left - uWidth + _SHD, ptCenter.y + uWidth/2, m_rect.left, m_rect.bottom), _LC(brBack));

	dc.FillRect(CRect(m_rect.left, m_rect.bottom, ptCenter.x - uWidth/2, m_rect.bottom + uWidth - _SHD), _LC(brBack));
	dc.FillRect(CRect(ptCenter.x + uWidth/2, m_rect.bottom, m_rect.right, m_rect.bottom + uWidth - _SHD), _LC(brBack));

	dc.FillRect(CRect(m_rect.right, m_rect.top, m_rect.right + uWidth - _SHD, ptCenter.y - uWidth/2), _LC(brBack));
	dc.FillRect(CRect(m_rect.right, ptCenter.y + uWidth/2, m_rect.right + uWidth - _SHD, m_rect.bottom), _LC(brBack));

	// Задаем кисть для отрисовки активных частей рамки
	CBrush brFront; brFront.Attach(::GetSysColorBrush(COLOR_ACTIVECAPTION));

	// Создаем обводку рамки и восстанавливаем кисть
	if(m_nStyle&TSF_HATCHEDBORDER)
	{
		dc.FrameRect(CRect(m_rect.left - uWidth, m_rect.top - uWidth, m_rect.right + uWidth, m_rect.bottom + uWidth), _LC(brFront));
		brBack.DeleteObject(); brBack.Attach(::GetSysColorBrush(COLOR_WINDOW));
	}

	// Рисуем линии, придающие рамке привлекательность
	if(m_nStyle&TSF_APPEALBORDER) drawAppealRect(dc, brFront);

	// Рисуем углы рамки
	CRect rc(m_rect.left - uWidth, m_rect.top - uWidth, m_rect.left, m_rect.top);
	dc.FillRect(rc, m_nStyle&TSF_TOPLEFT ? _LC(brFront) : _LC(brBack));
	if(~m_nStyle&TSF_TOPLEFT) dc.FrameRect(rc, _LC(brFront));

	rc.SetRect(m_rect.left - uWidth, m_rect.bottom, m_rect.left, m_rect.bottom + uWidth);
	dc.FillRect(rc, m_nStyle&TSF_BOTTOMLEFT ? _LC(brFront) : _LC(brBack));
	if(~m_nStyle&TSF_BOTTOMLEFT) dc.FrameRect(rc, _LC(brFront));

	rc.SetRect(m_rect.right, m_rect.bottom, m_rect.right + uWidth, m_rect.bottom + uWidth);
	dc.FillRect(rc, m_nStyle&TSF_BOTTOMRIGHT ? _LC(brFront) : _LC(brBack));
	if(~m_nStyle&TSF_BOTTOMRIGHT) dc.FrameRect(rc, _LC(brFront));

	rc.SetRect(m_rect.right, m_rect.top - uWidth, m_rect.right + uWidth, m_rect.top);
	dc.FillRect(rc, m_nStyle&TSF_TOPRIGHT ? _LC(brFront) : _LC(brBack));
	if(~m_nStyle&TSF_TOPRIGHT) dc.FrameRect(rc, _LC(brFront));

	// Рисуем центральные активные части
	rc.SetRect(m_rect.left - uWidth, ptCenter.y - uWidth/2, m_rect.left, ptCenter.y + uWidth/2);
	dc.FillRect(rc, m_nStyle&TSF_LEFT ? _LC(brFront) : _LC(brBack));
	if(~m_nStyle&TSF_LEFT) dc.FrameRect(rc, _LC(brFront));

	rc.SetRect(ptCenter.x - uWidth/2, m_rect.bottom, ptCenter.x + uWidth/2, m_rect.bottom + uWidth);
	dc.FillRect(rc, m_nStyle&TSF_BOTTOM ? _LC(brFront) : _LC(brBack));
	if(~m_nStyle&TSF_BOTTOM) dc.FrameRect(rc, _LC(brFront));
	
	rc.SetRect(m_rect.right, ptCenter.y - uWidth/2, m_rect.right + uWidth, ptCenter.y + uWidth/2);
	dc.FillRect(rc, m_nStyle&TSF_RIGHT ? _LC(brFront) : _LC(brBack));
	if(~m_nStyle&TSF_RIGHT) dc.FrameRect(rc, _LC(brFront));

	rc.SetRect(ptCenter.x - uWidth/2, m_rect.top - uWidth, ptCenter.x + uWidth/2, m_rect.top);
	dc.FillRect(rc, m_nStyle&TSF_TOP ? _LC(brFront) : _LC(brBack));
	if(~m_nStyle&TSF_TOP) dc.FrameRect(rc, _LC(brFront));

	// Запоминаем внешнюю границу рамки
	m_rect.InflateRect(uWidth, uWidth);
	brFront.Detach(); brBack.Detach();
}

//////////////////////////////////////////////////////////////////////
// Изменение курсора при его прохождении над рамкой. Предназначена для
// вызова из события OnSetCursor. hWnd - окно, содержащее рамку
bool CGuiItemEdit::CTracker::SetCursor(HWND hWnd, UINT hitTest) const
{
	if(HTCLIENT != hitTest) return false;       // трекер отображается только в клиентской области окна

	CPoint ptCursor; ::GetCursorPos(&ptCursor); // преобразование координат
	::ScreenToClient(hWnd, &ptCursor);

	return SetCursor(HitTest(ptCursor));
}

// Вспомогательная функция - создана для оптимизации вычислений
bool CGuiItemEdit::CTracker::SetCursor(CGuiItemEdit::CTracker::TrackerHit hitTest) const
{
	int iIndex = getCursorIndex(hitTest);
	if(iIndex != -1){
		::SetCursor(_hCursors[iIndex]);
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////
// Отрисовка резинового контура [используется захват мыши]
// Возвращает true, если размеры или расположение контроли-
// руемого item'а [site] были изменены. НЕ ИЗМЕНЯЕТ m_rect
bool CGuiItemEdit::CTracker::Track(HWND hWnd, const CPoint& pt, bool bRubberBand) const
{
	// При необходимости, определяем область, на которой находится переданная
	// точка, а также надобность изменения размеров / положения
	TrackerHit hitTest = hitBottomRight;
	if(!bRubberBand){
		hitTest = HitTest(pt);
		if(getCursorIndex(hitTest) == -1)
			return false;
	}

	// Проверяем готовность к захвату мыши
	if(::GetCapture() != NULL) return false;    // возвращает окно, осуществившее mouse capture

	// Аллокируем ресурсы
	::SetCapture(hWnd);                         // захватываем мышь
	CDC dc; dc.Attach(::GetDC(hWnd));           // получаем контекст устройства
	int iPrevMode = dc.SetROP2(R2_NOTXORPEN);   // устанавливаемый нужный стиль отрисовки

	// Начальные размеры резинового контура + установка таймера
	CRect rc(pt, pt), rcNew(pt, pt), rcPrev(pt, pt);    // rc - current position; rcPrev - last drawn rect
	bool bKillTimer = false;

	if(!bRubberBand){
		bKillTimer = ::SetTimer(hWnd, ID_TIMER_TRACK, 300, NULL) != 0;
		_OWNEROFMEMBER_PTR(CDesigner, m_Tracker)->Tracker_OnGetDrawingRect(rc);
		rcPrev = rcNew = rc;
	}

	//////////////////////////////////////////////////////
	// Обрабатываем нужные события до тех пор, пока захват
	// мыши не будет потерян - здорово  придумано  [by MS]
	bool bContinue = true, bErase = false;      // вспомогательные переменные
	MSG msg;

	// Цикл обработки сообщений
	while(bContinue){
		::GetMessage(&msg, NULL, 0, 0); // обрабатываем ВСЕ сообщения от всех окон
										// потока,а также сообщения самого потока
		if(::GetCapture() != hWnd)      // захват мыши потерян (аварийный выход)
			break;

		switch(msg.message){
		case WM_MOUSEMOVE:                              // здесь координаты клиентского окна [т.е. hWnd]
			switch(hitTest){                            // задаем новые координаты контура [calc rcNew]
			case hitBottomRight:
				rcNew.bottom = GET_Y_LPARAM(msg.lParam); rcNew.right = GET_X_LPARAM(msg.lParam);
				break;
			case hitMiddle:                             // move control [все остальное - изменение размера]
			case hitBorder:
				rcNew.SetRect(GET_X_LPARAM(msg.lParam) + rc.left - pt.x, GET_Y_LPARAM(msg.lParam) + rc.top - pt.y, \
							  GET_X_LPARAM(msg.lParam) + rc.right - pt.x, GET_Y_LPARAM(msg.lParam) + rc.bottom - pt.y);
				break;
			case hitTopRight:
				rcNew.top = GET_Y_LPARAM(msg.lParam); rcNew.right = GET_X_LPARAM(msg.lParam);
				break;
			case hitTopLeft:
				rcNew.top = GET_Y_LPARAM(msg.lParam); rcNew.left = GET_X_LPARAM(msg.lParam);
				break;
			case hitBottomLeft:
				rcNew.bottom = GET_Y_LPARAM(msg.lParam); rcNew.left = GET_X_LPARAM(msg.lParam);
				break;
			case hitRight:
				rcNew.right = GET_X_LPARAM(msg.lParam);
				break;
			case hitBottom:
				rcNew.bottom = GET_Y_LPARAM(msg.lParam);
				break;
			case hitLeft:
				rcNew.left = GET_X_LPARAM(msg.lParam);
				break;
			case hitTop:
				rcNew.top = GET_Y_LPARAM(msg.lParam);
				break;
			};
			_OWNEROFMEMBER_PTR(CDesigner, m_Tracker)->Tracker_OnChangingRect(rcNew, hitTest);// учитываем ограничения на изменение pos/size

			if(!rcNew.EqualRect(rcPrev)){           // уменьшаем количество flicker'ов
				if(bErase){
					drawTrackerRect(dc, rcPrev);    // стираем предыдущий контур
				}else{
					setDragCursor(hitTest);         // корректируем курсор
				}

				drawTrackerRect(dc, rcNew);         // отрисовываем новый контур
				rcPrev = rcNew; bErase = true;      // теперь есть что удалять
			}
			break;
		case WM_KEYDOWN:
			if(msg.wParam != VK_ESCAPE)
			   break;
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
			bContinue = false;          // признак нормального завершения
			break;
		case WM_TIMER:                  // таймер должен быть нашим <!>
			if(ID_TIMER_TRACK == msg.wParam && hWnd == msg.hwnd){
				if(!bErase){                        // контур отображен не был
					ATLASSERT(rc.EqualRect(rcNew) && rc.EqualRect(rcPrev));
					drawTrackerRect(dc, rcNew);     // отображаем контур по текущим [rc] размерам
					setDragCursor(hitTest);         // задаем соответствующий курсор [if needed]
					bErase = true;
				}

				::KillTimer(hWnd, ID_TIMER_TRACK);
				bKillTimer = false;
				break;
			}
		default:
			::DispatchMessage(&msg);
		};
	}

	// При необходимости снимаем таймер
	if(bKillTimer) ::KillTimer(hWnd, ID_TIMER_TRACK);

	// При необходимости стираем предыдущий контур
	if(bErase) drawTrackerRect(dc, rcPrev);

	// Освобождаем используемые ресурсы
	dc.SetROP2(iPrevMode);              // восстанавливаем режим отрисовки
	::ReleaseDC(hWnd, dc.Detach());     // освобождаем контекст устройства
	::ReleaseCapture();                 // освобождаем мышь

	// Задаем контролируемому объекту новый размер
	if(!bContinue && !rcNew.EqualRect(rc))
	{
		_OWNEROFMEMBER_PTR(CDesigner, m_Tracker)->Tracker_OnChangedRect(rc, rcNew);
		return true;
	}

	return false;
}

// ########################################################################

// Выдает затертый рамкой регион [like strong <!> reference], позволяет уменьшить эффект мигания
void CGuiItemEdit::CTracker::GetEraseRegion(/*[out]*/ HRGN& hRgn, unsigned short rt) const
{
	// Корректность параметров
	ATLASSERT(NULL == hRgn);
	if(m_rect.IsRectEmpty())
		return;

	HRGN hTmp = NULL;

	// Наиболее частый вариант использования
	if((rt&rtFull) == rtFull)
	{
		if(hRgn = ::CreateRectRgnIndirect(m_rect))
		{ // создаем и комбинируем составляющие части требуемого региона
			if((rt&rtOutRect) != rtOutRect)
			{        // при необходимости, исключаем область, занимаемую объектом
				if(hTmp = ::CreateRectRgn(m_rect.left + uWidth, m_rect.top + uWidth, m_rect.right - uWidth, m_rect.bottom - uWidth))
				{
					if(::CombineRgn(hRgn, hRgn, hTmp, RGN_DIFF) == ERROR)
						::DeleteObject(hRgn); hRgn = NULL;
					::DeleteObject(hTmp);
				}
			}
		}
		return;
	}

	// Остальные возможные варианты
	if(hRgn = ::CreateRectRgn(0, 0, 0, 0)) // создаем стартовый регион
	{
		if(rt&rtLeft) combineEraseRgn(hRgn, hTmp, m_rect.left, m_rect.top, m_rect.left + uWidth, m_rect.bottom);
		if(rt&rtBottom) combineEraseRgn(hRgn, hTmp, m_rect.left, m_rect.bottom - uWidth, m_rect.right, m_rect.bottom);
		if(rt&rtRight) combineEraseRgn(hRgn, hTmp, m_rect.right - uWidth, m_rect.top, m_rect.right, m_rect.bottom);
		if(rt&rtTop) combineEraseRgn(hRgn, hTmp, m_rect.left, m_rect.top, m_rect.right, m_rect.top + uWidth);
	}
}

// Helper function for GetEraseRegion
void CGuiItemEdit::CTracker::combineEraseRgn(HRGN& hRgn, HRGN& hTmp, int l, int t, int r, int b)
{
	if(hTmp = ::CreateRectRgn(l, t, r, b))
	{
		::CombineRgn(hRgn, hRgn, hTmp, RGN_OR);
		::DeleteObject(hTmp);
	}
}

//////////////////////////////////////////////////////////////////////////////
// Проверяет над какой областью последней ОТРИСОВАННОЙ рамки находится курсор
CGuiItemEdit::CTracker::TrackerHit CGuiItemEdit::CTracker::HitTest(const CPoint& pt) const
{
	// А надо ли вообще что-нибудь проверять
	if(!m_rect.PtInRect(pt))
		return hitNothing;	

	// Middle - строго внутри контура
	if(CRect(m_rect.left + uWidth, m_rect.top + uWidth, m_rect.right - uWidth, m_rect.bottom - uWidth).PtInRect(pt))
		return hitMiddle;

	/////////////////////////////////////////////////////////////////
	// Анализируем активные участки рамки - точка точно где-то здесь

	// TopLeft
	if(CRect(m_rect.left, m_rect.top, m_rect.left + uWidth, m_rect.top + uWidth).PtInRect(pt))
		return hitTopLeft;

	// BottomLeft
	if(CRect(m_rect.left, m_rect.bottom - uWidth, m_rect.left + uWidth, m_rect.bottom).PtInRect(pt))
		return hitBottomLeft;

	// BottomRight
	if(CRect(m_rect.right - uWidth, m_rect.bottom - uWidth, m_rect.right, m_rect.bottom).PtInRect(pt))
		return hitBottomRight;

	// TopRight
	if(CRect(m_rect.right - uWidth, m_rect.top, m_rect.right, m_rect.top + uWidth).PtInRect(pt))
		return hitTopRight;

	// Left
	CPoint ptCenter(m_rect.CenterPoint());
	if(CRect(m_rect.left, ptCenter.y - uWidth/2, m_rect.left + uWidth, ptCenter.y + uWidth/2).PtInRect(pt))
		return hitLeft;

	// Right
	if(CRect(m_rect.right - uWidth, ptCenter.y - uWidth/2, m_rect.right, ptCenter.y + uWidth/2).PtInRect(pt))
		return hitRight;

	// Top
	if(CRect(ptCenter.x - uWidth/2, m_rect.top, ptCenter.x + uWidth/2, m_rect.top + uWidth).PtInRect(pt))
		return hitTop;

	// Bottom
	if(CRect(ptCenter.x - uWidth/2, m_rect.bottom - uWidth, ptCenter.x + uWidth/2, m_rect.bottom).PtInRect(pt))
		return hitBottom;

	return hitBorder;           // это было проверено первыми двумя условиями
}

// Задание рамке нового стиля, возвращает предыдущий стиль.При передаче
// отрицательного значения просто возвращает текущий стиль. 
USHORT CGuiItemEdit::CTracker::SetStyle(USHORT newStyle)
{
	USHORT prevStyle = m_nStyle;
	m_nStyle = newStyle;
	return prevStyle;
}

// Задание ограничений на размер track-контура
void CGuiItemEdit::CTracker::EnsureSize(CRect& rc, const CSize& szMin, CGuiItemEdit::CTracker::TrackerHit hitTest)
{
	// Некорректные размеры по ширине
	if(rc.Width() < szMin.cx)
	{
		if(hitRight == (hitTest&hitRight))
			rc.right = rc.left + szMin.cx;
		else if(hitLeft == (hitTest&hitLeft))
			rc.left = rc.right - szMin.cx;
	}

	// Некорректные размеры по высоте
	if(rc.Height() < szMin.cy)
	{
		if(hitBottom == (hitTest&hitBottom))
			rc.bottom = rc.top + szMin.cy;
		else if(hitTop == (hitTest&hitTop))
			rc.top = rc.bottom - szMin.cy;
	}
}

// Отрисовка резинового track-контура [контекст должен быть подготовлен]
void CGuiItemEdit::CTracker::DrawTrackerRect(CDC& dc, const CRect& rc)
{
	LONG i = min(rc.left, rc.right), n = max(rc.left, rc.right);
	for(i; i < n; i+=2)
	{
		dc.SetPixel(i, rc.top, 0);
		dc.SetPixel(i, rc.bottom - 1, 0);           // -1 для того, чтобы не захватить базовую рамку
	}

	i = min(rc.top, rc.bottom) + 2; n = max(rc.top, rc.bottom) - 1;
	for(i; i < n; i+=2)
	{
		dc.SetPixel(rc.left, i, 0);
		dc.SetPixel(rc.right - 1, i, 0);            // -1 для того, чтобы не захватить базовую рамку
	}
}

// ########################################################################

int CGuiItemEdit::CTracker::getCursorIndex(CGuiItemEdit::CTracker::TrackerHit hitTest) const
{
	switch(hitTest){
	case hitTopLeft:
		if(m_nStyle&TSF_TOPLEFT) return 0;
		break;
	case hitBottomRight:
		if(m_nStyle&TSF_BOTTOMRIGHT) return 0;
		break;
	case hitTopRight:
		if(m_nStyle&TSF_TOPRIGHT) return 1;
		break;
	case hitBottomLeft:
		if(m_nStyle&TSF_BOTTOMLEFT) return 1;
		break;
	case hitTop:
		if(m_nStyle&TSF_TOP) return 2;
		break;
	case hitBottom:
		if(m_nStyle&TSF_BOTTOM) return 2;
		break;
	case hitLeft:
		if(m_nStyle&TSF_LEFT) return 3;
		break;
	case hitRight:
		if(m_nStyle&TSF_RIGHT) return 3;
		break;
	case hitMiddle:
	case hitBorder:
		if(m_nStyle&TSF_MIDDLE) return 4;
	};

	return -1;
}

// Helper inline function: fire event and call real DrawTrackerRect
void CGuiItemEdit::CTracker::drawTrackerRect(CDC& dc, const CRect& rc) const
{
	_OWNEROFMEMBER_PTR(CDesigner, m_Tracker)->Tracker_OnDrawTrackerRect(dc, rc);
	DrawTrackerRect(dc, rc);
}

// Придаем рамке привлекательность, использует переданную кисть
void CGuiItemEdit::CTracker::drawAppealRect(CDC& dc, const CBrush& br) const
{
	// Определяем цвет кисти
	LOGBRUSH lb;

	br.GetLogBrush(&lb);

	// Создаем горизонтальную и вертикальную сеточку
	int i, j;
	for(i = m_rect.left; i < m_rect.right; i += 4){
		for(j = 2; j < (int)uWidth; j+=2){
			dc.SetPixel(i + (j%4?1:3), m_rect.top - j, lb.lbColor);
			dc.SetPixel(i + (j%4?1:3), m_rect.bottom + j, lb.lbColor);
		}
	}

	for(i = m_rect.top; i < m_rect.bottom; i += 8){
		for(j = 2; j < (int)uWidth; j++){
			dc.SetPixel(m_rect.left - j, i + (j%2?0:4), lb.lbColor);
			dc.SetPixel(m_rect.right + j - 1, i + (j%2?0:4), lb.lbColor);
		}
	}
}

void CGuiItemEdit::CTracker::setDragCursor(CGuiItemEdit::CTracker::TrackerHit hitTest) const
{
	if(hitMiddle == hitTest || hitBorder == hitTest)
		::SetCursor(_hCursors[5]);
}

// Создаем bitmap кисти для отрисовки фона рамки
void CGuiItemEdit::CTracker::createBackBitmap(CDC& dc, CBitmap& bmp)
{
	// Корректность вызова
	ATLASSERT(bmp.IsNull());

	// Создаем bmp и позволяем ее редактировать
	bmp.CreateCompatibleBitmap(_LC(dc), uHatchSpace + 1, uHatchSpace + 1);
	CDC memdc; memdc.CreateCompatibleDC(_LC(dc));

	HBITMAP hBmp = memdc.SelectBitmap(bmp);

	// Создаем требуемое изображение
	CBrush br; br.Attach(::GetSysColorBrush(COLOR_WINDOW));
	memdc.FillRect(CRect(0, 0, uHatchSpace + 1, uHatchSpace + 1), _LC(br));

	// Отображаем видиую часть линий y = -x + b
	for(short i = 1; i <= 2*uHatchSpace; i += uHatchSpace + 1)	// цикл по b
	{                                       
		for(short j = i > uHatchSpace ? i - uHatchSpace : 0; -j + i >= 0 && j <= uHatchSpace; j++)	// цикл по x
		{
			memdc.SetPixel(j, -j + i, ::GetSysColor(COLOR_ACTIVECAPTION));
		}
	}

	memdc.SelectBitmap(hBmp);
	br.Detach();
}

// ########################################################################
// CComGuiItem

// CComGuiItemImpl::_PredefVals
HRESULT CComGuiItemImpl::_PredefVals::GetPredefinedStrings(DISPID dispID, CALPOLESTR * pCaStringsOut, CADWORD * pCaCookiesOut)
{
	CComGuiItemImpl& p = *_OWNEROFMEMBER_PTR(CComGuiItemImpl, m_PrefVals);
	DWORD idx = (ULONG)dispID - 1;
	if( idx >= p.m_Props.size() )
		return E_FAIL;
	
	PropDesc& _prop = p.m_Props[idx];
	PVOID pCtx = NULL;
	DWORD nCount = _prop.m_pVals ? _prop.m_pVals(gpvGetCount, p.m_pEditor, p.m_pItem, &pCtx, _REFCLASS_NULL(cStrObj), _REFCLASS_NULL(CComVariant)) : 0;
	if( !nCount )
	{
		pCaStringsOut->cElems = 0;
		pCaStringsOut->pElems = NULL;
		return S_OK;
	}

	m_Vals.resize(nCount);
	
	pCaStringsOut->cElems = nCount;
	pCaStringsOut->pElems = (LPOLESTR *)::CoTaskMemAlloc(nCount*sizeof(LPOLESTR));
	for(ULONG i = 0; i < nCount; i++)
	{
		cStrObj strName; _prop.m_pVals(gpvGetNextItem, p.m_pEditor, p.m_pItem, &pCtx, strName, m_Vals[i]);
		int n = (strName.size() + 1)*sizeof(WCHAR);
		pCaStringsOut->pElems[i] = (LPOLESTR)::CoTaskMemAlloc(n);
		memcpy(pCaStringsOut->pElems[i], strName.data(), n);
	}

	_prop.m_pVals(gpvFinish, p.m_pEditor, p.m_pItem, &pCtx, _REFCLASS_NULL(cStrObj), _REFCLASS_NULL(CComVariant));

	pCaCookiesOut->cElems = nCount;
	pCaCookiesOut->pElems = (DWORD *)::CoTaskMemAlloc(nCount*sizeof(DWORD));
	for(ULONG i = 0; i < nCount; i++)
		pCaCookiesOut->pElems[i] = i + 1;
	
	return S_OK;
}

HRESULT CComGuiItemImpl::_PredefVals::GetPredefinedValue(DISPID dispID, DWORD dwCookie, VARIANT * pVarOut)
{
	DWORD idx = dwCookie - 1;
	if( idx >= m_Vals.size() )
		return E_FAIL;

	CComVariant _v; _v.Attach(pVarOut);
	_v = m_Vals[idx];
	_v.Detach(pVarOut);
	return S_OK;
}

// ########################################################################

void	CComGuiItemImpl::Init(CItemBase * pItem)
{
	memset(&m_TypeAttr, 0, sizeof(m_TypeAttr));
	m_Props.clear();

	if( m_pItem )
		m_pItem->Release();
	m_pItem = pItem;
	if( !m_pItem )
		return;
	
	m_pItem->AddRef();
	
#define GUI_PROP(_name, _type, _rw, _pf)	\
	do{	\
		PropDesc& fd = *m_Props.insert(m_Props.end(), PropDesc());	\
		fd.Type() = _type;	\
		fd.m_Name = #_name;	\
		fd.m_pFunc = &_InvokeProp_##_name;	\
		fd.m_pVals = _pf;	\
		if( !_rw )	\
			fd.invkind = INVOKE_PROPERTYGET;	\
	} while(0)

	CItemBase * pOwner = m_pItem->GetByFlagsAndState(STYLE_EXT_ITEM, ISTATE_ALL, NULL, CItemBase::gbfasUpToParent);
	if( pOwner == m_pItem && m_pItem->m_pParent )
		pOwner = m_pItem->m_pParent->GetByFlagsAndState(STYLE_EXT_ITEM, ISTATE_ALL, NULL, CItemBase::gbfasUpToParent);
	bool bOwnerEditable = pOwner == m_pEditor->m_pItem;

	if( m_pItem == m_pEditor->m_pItem )
	{
		GUI_PROP(Group, VT_BSTR, true, &_GetPropPredefVal_Group);
		GUI_PROP(ExternalId, VT_BSTR, true, NULL);
	}
	else
	{
		if( m_pItem->m_nFlags & STYLE_EXT )
		{
			GUI_PROP(Group, VT_BSTR, false, NULL);
			GUI_PROP(ExternalId, VT_BSTR, false, NULL);
		}
		
		GUI_PROP(Id, VT_BSTR, bOwnerEditable, NULL);

		if( m_pItem->m_strSection.size() )
			GUI_PROP(External, VT_BOOL, true, NULL);

		GUI_PROP(Overridable, VT_BOOL, bOwnerEditable, NULL);
	}
	
	{
		GUI_PROP(Props, VT_BSTR, true, NULL);
		m_nPropStrId = m_Props.size();
	}
	
	GUI_PROP(Type,						VT_BSTR,	false,	NULL);
	GUI_PROP(Alias,						VT_BSTR,	true,	NULL);
	
	{
		GUI_PROP(Text, VT_BSTR,	true, NULL);
		m_nTextId = m_Props.size();
	}
	
	GUI_PROP(TextIsSimple,				VT_BOOL,	true,	NULL);
	GUI_PROP(Font,						VT_BSTR,	true,	&_GetPropPredefVal_Font);
	GUI_PROP(Icon,						VT_BSTR,	true,	&_GetPropPredefVal_Icon);
	GUI_PROP(Background,				VT_BSTR,	true,	&_GetPropPredefVal_Background);
	GUI_PROP(Border,					VT_BSTR,	true,	&_GetPropPredefVal_Border);
	GUI_PROP(SizeHorizontal,			VT_BSTR,	true,	&_GetPropPredefVal_Size);
	GUI_PROP(SizeVertical,				VT_BSTR,	true,	&_GetPropPredefVal_Size);
	GUI_PROP(MarginLeft,				VT_I4,		true,	NULL);
	GUI_PROP(MarginTop,					VT_I4,		true,	NULL);
	GUI_PROP(MarginRight,				VT_I4,		true,	NULL);
	GUI_PROP(MarginBottom,				VT_I4,		true,	NULL);

	if( m_pItem != m_pEditor->m_pItem )
		GUI_PROP(PreviousItem, VT_BSTR,	bOwnerEditable, &_GetPropPredefVal_PreviousItem);

	GUI_PROP(AlignHorizontal,			VT_BSTR,	true,	&_GetPropPredefVal_AlignHorizontal);
	GUI_PROP(AlignVertical,				VT_BSTR,	true,	&_GetPropPredefVal_AlignVertical);
	GUI_PROP(OffsetBeforeHorizontal,	VT_I4,		true,	NULL);
	GUI_PROP(OffsetBeforeVertical,		VT_I4,		true,	NULL);
	GUI_PROP(OffsetAfterHorizontal,		VT_I4,		true,	NULL);
	GUI_PROP(OffsetAfterVertical,		VT_I4,		true,	NULL);
	
#undef GUI_PROP
		
	m_TypeAttr.cFuncs = m_Props.size();
	for(DWORD i = 0; i < m_Props.size(); i++)
	{
		PropDesc& fd = m_Props[i];
		fd.Fixup();
		fd.Id() = i + 1;
	}
}

HRESULT CComGuiItemImpl::QueryInterface(REFIID riid, void ** ppvObject)
{
	if( riid == __uuidof(IDispatch) )
		return *(IUnknown **)ppvObject = (IDispatch *)this, (*(IUnknown **)ppvObject)->AddRef(), S_OK;
	if( riid == __uuidof(IPerPropertyBrowsing) )
		return *(IUnknown **)ppvObject = (IPerPropertyBrowsing *)&m_PrefVals, (*(IUnknown **)ppvObject)->AddRef(), S_OK;
	return E_NOINTERFACE;
}

HRESULT CComGuiItemImpl::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	return E_NOTIMPL;
}

HRESULT CComGuiItemImpl::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	DWORD idx = (ULONG)dispIdMember - 1;
	if( idx >= m_Props.size() )
		return E_FAIL;
	
	PropDesc& _prop = m_Props[idx];
	
	if( wFlags == DISPATCH_PROPERTYGET )
	{
		CComVariant _v; _v.Attach(pVarResult);
		HRESULT hr = m_pItem->m_pEditData ? _prop.m_pFunc(m_pEditor, m_pItem, true, _v) : S_FALSE;
		_v.Detach(pVarResult);
		return hr;
	}

	if( wFlags == DISPATCH_PROPERTYPUT )
	{
		CComVariant _v; _v.Attach(&pDispParams->rgvarg[0]);
		HRESULT hr;
		if( _v.vt != _prop.Type() )
			hr = E_INVALIDARG;
		else
			hr = m_pItem->m_pEditData ? _prop.m_pFunc(m_pEditor, m_pItem, false, _v) : S_FALSE;
		_v.Detach(&pDispParams->rgvarg[0]);
		
		if( hr == S_OK )
			m_pEditor->ItemPropsChanged(m_pItem, true, m_nPropStrId - 1 == idx);
		
		return hr;
	}
	
	return E_NOTIMPL;
}

HRESULT CComGuiItemImpl::GetFuncDesc(UINT index, FUNCDESC ** ppFuncDesc)
{
	if( index >= m_Props.size() )
		return E_FAIL;
	return *ppFuncDesc = &m_Props[index], S_OK;
}

HRESULT CComGuiItemImpl::GetDocumentation(MEMBERID memid, BSTR *pBstrName, BSTR *pBstrDocString, DWORD *pdwHelpContext, BSTR *pBstrHelpFile)
{
	DWORD idx = (ULONG)memid - 1;
	if( idx >= m_Props.size() )
		return E_FAIL;
	
	*pBstrDocString = NULL;
	*pdwHelpContext = NULL;
	*pBstrHelpFile = NULL;
	
	USES_CONVERSION;
	return *pBstrName = ::SysAllocString(A2W(m_Props[idx].m_Name.c_str())), S_OK;
}

// ########################################################################

HRESULT CComGuiItemImpl::_InvokeProp_Group(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	if( bSetGet )
	{
		if( pItem->m_strSection.empty() && pItem->m_pParent && pItem->m_pParent->m_pEditData )
			v = pItem->m_pParent->m_pEditData->m_strGroup.data();
		else
			v = pItem->m_pEditData->m_strGroup.data();
		
		return S_OK;
	}

	if( pItem->m_strSection.empty() || !v.bstrVal || !*v.bstrVal )
		return S_FALSE;

	if( !v.bstrVal || !*v.bstrVal )
		return S_FALSE;
	
	pItem->m_pEditData->m_strGroup = v.bstrVal;
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_ExternalId(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	USES_CONVERSION;
	
	if( bSetGet )
		return v = A2W(pItem->m_pEditData->m_strSection.c_str()), S_OK;

	if( pItem != pEditor->m_pItem )
		return S_FALSE;

	LPCTSTR strNewSection = W2T(v.bstrVal);
	if( pEditor->_IsSectExist(strNewSection) )
	{
		pEditor->MessageBox(_T("Section name already exists"), _T("Error"), MB_OK|MB_ICONEXCLAMATION);
		return S_FALSE;
	}
	
	if( !strNewSection || !*strNewSection )
		return S_FALSE;

	pItem->m_pEditData->m_strSection = T2A(strNewSection);
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_Id(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	USES_CONVERSION;
	
	if( bSetGet )
		return v = A2W(pItem->m_pEditData->m_strItemId.c_str()), S_OK;

	if( !pItem->m_pParent )
		return S_FALSE;
	
	LPCSTR strNewId = W2A(v.bstrVal);
	
	tDWORD k;
	for(k = 0; k < pItem->m_pParent->m_aItems.size(); k++)
	{
		CItemBase * p = pItem->m_pParent->m_aItems[k];
		if( p->m_pEditData && p->m_pEditData->m_strItemId == strNewId )
			break;
	}

	if( k < pItem->m_pParent->m_aItems.size() )
	{
		pEditor->MessageBox(_T("Identifier must be unique"), _T("Error"), MB_OK|MB_ICONEXCLAMATION);
		return S_FALSE;
	}
	
	if( !strNewId || !*strNewId )
		return S_FALSE;

	pItem->m_pEditData->m_strItemId = strNewId;
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_External(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	if( bSetGet )
		return v = pItem->m_pEditData->m_bExtProp, S_OK;

	if( pItem == pEditor->m_pItem )
		return S_FALSE;

	if( pEditor->MessageBox(_T("Are you sure to change \"External\" property of this item?"), NULL, MB_YESNO|MB_ICONEXCLAMATION) == IDNO )
		return S_FALSE;
	
	pItem->m_pEditData->m_bExtProp = !!v.boolVal;
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_Overridable(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	if( bSetGet )
		return v = !!(pItem->m_nFlags & STYLE_EXTPRM), S_OK;

	if( 0 /*MBI!!!!!!!!!!*/ )
	{
		pEditor->MessageBox(_T("Each overridable item must have unique Id inside owner item"), NULL, MB_OK|MB_ICONEXCLAMATION);
		return S_FALSE;
	}
	
	if( v.boolVal )
		pItem->m_nFlags |= STYLE_EXTPRM;
	else
		pItem->m_nFlags &= ~STYLE_EXTPRM;
	
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_Props(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	USES_CONVERSION;
	
	if( bSetGet )
		return v = A2W(pItem->m_pEditData->m_strProps.c_str()), S_OK;

	pItem->m_pEditData->m_strProps = W2A(v.bstrVal);
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_Type(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	USES_CONVERSION;
	
	if( !bSetGet )
		return S_FALSE;

	if( pItem->m_strItemType.empty() )
	{
		if( pItem->m_strSection.size() )
			v = L"frame";
		else
			v = L"label";
	}
	else
		v = A2W(pItem->m_strItemType.c_str());
	
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_Alias(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	USES_CONVERSION;
	
	if( bSetGet )
		return v = A2W(pItem->m_pEditData->m_Props.Get(STR_PID_ALIAS).GetStr()), S_OK;

	pItem->m_pEditData->m_Props.Get(STR_PID_ALIAS, true).Set(0, W2A(v.bstrVal));
	return S_OK;
}

static void _EnableApostrofs(tString& str, bool bEnable)
{
	if( bEnable )
	{
		str.insert(0, "\"");
		str.append("\"");
	}
	else
	{
		if( str.size() && str[0] == '\"' )
			str.erase(0, 1);
		if( str.size() && str[str.size() - 1] == '\"' )
			str.erase(str.size() - 1, 1);
	}
}

HRESULT CComGuiItemImpl::_InvokeProp_Text(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	_USES_CONVERSION(CP_UTF8);
	
	if( bSetGet )
	{
		tString strText = pItem->m_pEditData->m_strText;
		if( pItem->m_pEditData->m_bTextSimple )
			_EnableApostrofs(strText, false);
		return v = A2W(strText.c_str()), S_OK;
	}

	pItem->m_pEditData->m_strText = W2A(v.bstrVal);
	if( pItem->m_pEditData->m_strText.size() )
	{
		if( pItem->m_pEditData->m_bTextSimple )
			_EnableApostrofs(pItem->m_pEditData->m_strText, true);
		else
			pItem->m_pRoot->TrimString(pItem->m_pEditData->m_strText);
	}
	
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_TextIsSimple(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	if( bSetGet )
		return v = pItem->m_pEditData->m_bTextSimple, S_OK;

	pItem->m_pEditData->m_bTextSimple = v.boolVal;
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_QuotedParamsProp(LPCSTR strProp, CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	USES_CONVERSION;
	
	if( bSetGet )
	{
		tString str = pItem->m_pEditData->m_Props.Get(strProp).Get();
		_EnableApostrofs(str, false);
		
		CItemPropVals pAtrs; pAtrs.Parse(str.c_str(), 0, true);
		return v = A2W(pAtrs.GetStr()), S_OK;
	}

	tString& str = pItem->m_pEditData->m_Props.Get(strProp, true).Get(0, true);
	_EnableApostrofs(str, false);

	CItemPropVals pAtrs; pAtrs.Parse(str.c_str(), 0, true);
	pAtrs.Get(0, true) = W2A(v.bstrVal);

	str.erase(); pAtrs.DeParse(str, NULL, true);
	_EnableApostrofs(str, true);
	if( str.size() == 2 )
		str.erase();
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_Font(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	return _InvokeProp_QuotedParamsProp(STR_PID_FONT, pEditor, pItem, bSetGet, v);
}

HRESULT CComGuiItemImpl::_InvokeProp_Icon(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	return _InvokeProp_QuotedParamsProp(STR_PID_ICON, pEditor, pItem, bSetGet, v);
}

HRESULT CComGuiItemImpl::_InvokeProp_Background(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	return _InvokeProp_QuotedParamsProp(STR_PID_BACKGND, pEditor, pItem, bSetGet, v);
}

HRESULT CComGuiItemImpl::_InvokeProp_Border(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	USES_CONVERSION;
	
	if( bSetGet )
		return v = A2W(pItem->m_pEditData->m_Props.Get(STR_PID_BORDER).GetStr()), S_OK;

	pItem->m_pEditData->m_Props.Get(STR_PID_BORDER, true).Set(0, W2A(v.bstrVal));
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_SizeHorizontal(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	USES_CONVERSION;
	
	if( bSetGet )
		return v = A2W(pItem->m_pEditData->m_Props.Get(STR_PID_SIZE).GetStr(0)), S_OK;

	tString str = W2A(v.bstrVal);
	tCHAR szNum[20];
	pItem->m_pEditData->m_Props.Get(STR_PID_SIZE, true).Set(0, str.size() ? str.c_str() : _ltoa(pItem->m_szSize.cx, szNum, 10));
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_SizeVertical(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	USES_CONVERSION;
	
	if( bSetGet )
		return v = A2W(pItem->m_pEditData->m_Props.Get(STR_PID_SIZE).GetStr(1)), S_OK;

	tString str = W2A(v.bstrVal);
	tCHAR szNum[20];
	pItem->m_pEditData->m_Props.Get(STR_PID_SIZE, true).Set(1, str.size() ? str.c_str() : _ltoa(pItem->m_szSize.cy, szNum, 10));
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_MarginLeft(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	if( bSetGet )
		return v = (LONG)pItem->m_pEditData->m_Props.Get(STR_PID_MARGINS).GetLong(0), S_OK;

	tCHAR szNum[20];
	pItem->m_pEditData->m_Props.Get(STR_PID_MARGINS, true).Set(0, v.lVal ? _ltoa(v.lVal, szNum, 10) : "");
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_MarginTop(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	if( bSetGet )
		return v = (LONG)pItem->m_pEditData->m_Props.Get(STR_PID_MARGINS).GetLong(1), S_OK;
	
	tCHAR szNum[20];
	pItem->m_pEditData->m_Props.Get(STR_PID_MARGINS, true).Set(1, v.lVal ? _ltoa(v.lVal, szNum, 10) : "");
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_MarginRight(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	if( bSetGet )
	{
		CItemPropVals& prp = pItem->m_pEditData->m_Props.Get(STR_PID_MARGINS);
		return v = prp.Count() >= 3 ? (LONG)prp.GetLong(2) : (LONG)prp.GetLong(0), S_OK;
	}

	tCHAR szNum[20];
	pItem->m_pEditData->m_Props.Get(STR_PID_MARGINS, true).Set(2, v.lVal ? _ltoa(v.lVal, szNum, 10) : "");
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_MarginBottom(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	if( bSetGet )
	{
		CItemPropVals& prp = pItem->m_pEditData->m_Props.Get(STR_PID_MARGINS);
		return v = prp.Count() >= 4 ? (LONG)prp.GetLong(3) : (LONG)prp.GetLong(1), S_OK;
	}

	tCHAR szNum[20];
	pItem->m_pEditData->m_Props.Get(STR_PID_MARGINS, true).Set(3, v.lVal ? _ltoa(v.lVal, szNum, 10) : "");
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_OffsetBeforeHorizontal(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	if( bSetGet )
		return v = (LONG)pItem->m_pEditData->m_Props.Get(STR_PID_OFFSET).GetLong(0), S_OK;

	tCHAR szNum[20];
	pItem->m_pEditData->m_Props.Get(STR_PID_OFFSET, true).Set(0, v.lVal ? _ltoa(v.lVal, szNum, 10) : "");
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_OffsetBeforeVertical(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	if( bSetGet )
		return v = (LONG)pItem->m_pEditData->m_Props.Get(STR_PID_OFFSET).GetLong(1), S_OK;

	tCHAR szNum[20];
	pItem->m_pEditData->m_Props.Get(STR_PID_OFFSET, true).Set(1, v.lVal ? _ltoa(v.lVal, szNum, 10) : "");
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_OffsetAfterHorizontal(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	if( bSetGet )
		return v = (LONG)pItem->m_pEditData->m_Props.Get(STR_PID_OFFSET).GetLong(2), S_OK;

	tCHAR szNum[20];
	pItem->m_pEditData->m_Props.Get(STR_PID_OFFSET, true).Set(2, v.lVal ? _ltoa(v.lVal, szNum, 10) : "");
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_OffsetAfterVertical(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	if( bSetGet )
		return v = (LONG)pItem->m_pEditData->m_Props.Get(STR_PID_OFFSET).GetLong(3), S_OK;

	tCHAR szNum[20];
	pItem->m_pEditData->m_Props.Get(STR_PID_OFFSET, true).Set(3, v.lVal ? _ltoa(v.lVal, szNum, 10) : "");
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_PreviousItem(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	USES_CONVERSION;

	if( bSetGet )
	{
		if( pItem->m_pParent )
		{
			CItemBase * pPrev = pItem->GetPrev(false);
			v = (pPrev && pPrev->m_pEditData) ? pPrev->m_pEditData->m_strItemId.c_str() : "";
		}
		
		return S_OK;
	}

	if( !pItem->m_pParent )
		return S_FALSE;
	
	CItemBase * pPrev = NULL;
	{
		LPCSTR strId = W2A(v.bstrVal);
		for(tDWORD i = 0; i < pItem->m_pParent->m_aItems.size(); i++)
		{
			CItemBase * p = pItem->m_pParent->m_aItems[i];
			if( p->m_pEditData && p->m_pEditData->m_strItemId == strId )
			{
				pPrev = p;
				break;
			}
		}
	}
	
	if( pPrev == pItem )
		return S_FALSE;

	pItem->m_pParent->MoveItem(pPrev, pItem, true);
	for(tDWORD i = 0; i < pItem->m_pParent->m_aItems.size(); i++)
		pItem->m_pParent->m_aItems[i]->RecalcLayout();
	
	pEditor->m_pFrame->GuiItem_OnDataChanged(true);
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_AlignHorizontal(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	USES_CONVERSION;
	
	if( bSetGet )
	{
		tString str;
		{
			const tString& strPrp = pItem->m_pEditData->m_Props.Get(STR_PID_ALIGN).Get();
			if( strPrp.size() > 0 )
				str += strPrp[0];
		}
		
		v = str.c_str();
		return S_OK;
	}
	
	tString str = W2A(v.bstrVal);
	if( str.empty() )
		return S_FALSE;
	
	CItemPropVals& prp = pItem->m_pEditData->m_Props.Get(STR_PID_ALIGN, true);
	tString& strPrp = prp.Get();
	strPrp.resize(2);
	strPrp[0] = str[0];
	return S_OK;
}

HRESULT CComGuiItemImpl::_InvokeProp_AlignVertical(CGuiItemEdit * pEditor, CItemBase * pItem, bool bSetGet, CComVariant& v)
{
	USES_CONVERSION;
	
	if( bSetGet )
	{
		tString str;
		{
			const tString& strPrp = pItem->m_pEditData->m_Props.Get(STR_PID_ALIGN).Get();
			if( strPrp.size() > 0 )
				str += strPrp[1];
		}
		
		v = str.c_str();
		return S_OK;
	}
	
	tString str = W2A(v.bstrVal);
	if( str.empty() )
		return S_FALSE;
	
	CItemPropVals& prp = pItem->m_pEditData->m_Props.Get(STR_PID_ALIGN, true);
	tString& strPrp = prp.Get();
	strPrp.resize(2);
	strPrp[1] = str[0];
	return S_OK;
}

// ########################################################################

DWORD CComGuiItemImpl::_GetPropPredefVal_Group(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v)
{
	cVector<cStrObj>& _aGroups = pEditor->m_pFrame->m_GuiItems.m_aGroups;
	if( eOpType == gpvGetCount )
		return *pCtx = NULL, _aGroups.size();

	if( eOpType == gpvFinish )
		return 0;
	
	DWORD i = (DWORD)*pCtx;
	strName = _aGroups[i];
	v = strName.data();
	
	*pCtx = (PVOID)++i;
	return 0;
}

DWORD CComGuiItemImpl::_GetPropPredefVal_Font(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v)
{
	tKeys& _a = pEditor->m_pFrame->m_GuiItems.m_aFonts;
	if( eOpType == gpvGetCount )
		return *pCtx = NULL, _a.size() + 1;

	if( eOpType == gpvFinish )
		return 0;
	
	DWORD i = (DWORD)*pCtx;
	if( !i )
	{
		strName = "(Default)";
		v = L"";
	}
	else
	{
		strName = _a[i - 1].c_str();
		v = strName.data();
	}
	
	*pCtx = (PVOID)++i;
	return 0;
}

DWORD CComGuiItemImpl::_GetPropPredefVal_Icon(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v)
{
	tKeys& _a = pEditor->m_pFrame->m_GuiItems.m_aIcons;
	if( eOpType == gpvGetCount )
		return *pCtx = NULL, _a.size() + 1;

	if( eOpType == gpvFinish )
		return 0;
	
	DWORD i = (DWORD)*pCtx;
	if( !i )
	{
		strName = "(None)";
		v = L"";
	}
	else
	{
		strName = _a[i - 1].c_str();
		v = strName.data();
	}
	
	*pCtx = (PVOID)++i;
	return 0;
}

DWORD CComGuiItemImpl::_GetPropPredefVal_Background(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v)
{
	tKeys& _a = pEditor->m_pFrame->m_GuiItems.m_aBackgrounds;
	if( eOpType == gpvGetCount )
		return *pCtx = NULL, _a.size() + 1;

	if( eOpType == gpvFinish )
		return 0;
	
	DWORD i = (DWORD)*pCtx;
	if( !i )
	{
		strName = "(None)";
		v = L"";
	}
	else
	{
		strName = _a[i - 1].c_str();
		v = strName.data();
	}
	
	*pCtx = (PVOID)++i;
	return 0;
}

DWORD CComGuiItemImpl::_GetPropPredefVal_Border(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v)
{
	tKeys& _a = pEditor->m_pFrame->m_GuiItems.m_aBorders;
	if( eOpType == gpvGetCount )
		return *pCtx = NULL, _a.size() + 1;

	if( eOpType == gpvFinish )
		return 0;
	
	DWORD i = (DWORD)*pCtx;
	if( !i )
	{
		strName = "(None)";
		v = L"";
	}
	else
	{
		strName = _a[i - 1].c_str();
		v = strName.data();
	}
	
	*pCtx = (PVOID)++i;
	return 0;
}

DWORD CComGuiItemImpl::_GetPropPredefVal_Size(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v)
{
	if( eOpType == gpvGetCount )
		return *pCtx = NULL, 3;

	if( eOpType == gpvFinish )
		return 0;

	DWORD i = (DWORD)*pCtx;
	switch( i )
	{
	case 0:		strName = "Auto";			v = L"a"; break;
	case 1:		strName = "Parent";			v = L"p"; break;
	case 2:		strName = "(Current size)";	v = L""; break;
	}
	
	*pCtx = (PVOID)++i;
	return 0;
}

DWORD CComGuiItemImpl::_GetPropPredefVal_PreviousItem(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v)
{
	if( eOpType == gpvGetCount )
	{
		*pCtx = NULL;
		if( !pItem->m_pParent )
			return 0;
		
		tDWORD n = 0;
		for(tDWORD i = 0; i < pItem->m_pParent->m_aItems.size(); i++)
		{
			CItemBase * pChild = pItem->m_pParent->m_aItems[i];
			if( pChild->m_pEditData && pChild != pItem )
				n++;
		}
		
		return n + 1;
	}

	if( eOpType == gpvFinish )
		return 0;
	
	DWORD i = (DWORD)*pCtx;
	if( !i )
	{
		strName = "(First)";
		v = L"";
	}
	else
	{
		CItemBase * pChild = NULL;
		for( ; (i - 1) < pItem->m_pParent->m_aItems.size(); i++)
		{
			pChild = pItem->m_pParent->m_aItems[i - 1];
			if( pChild->m_pEditData && pChild != pItem )
				break;
		}

		strName = pChild->m_pEditData->m_strItemId.c_str();
		v = pChild->m_pEditData->m_strItemId.c_str();
	}
	
	*pCtx = (PVOID)++i;
	return 0;
}

DWORD CComGuiItemImpl::_GetPropPredefVal_AlignHorizontal(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v)
{
	if( eOpType == gpvGetCount )
		return *pCtx = NULL, 6;

	if( eOpType == gpvFinish )
		return 0;

	DWORD i = (DWORD)*pCtx;
	switch( i )
	{
	case 0:		strName = "Left";					v = L"l"; break;
	case 1:		strName = "Right";					v = L"r"; break;
	case 2:		strName = "Center";					v = L"c"; break;
	case 3:		strName = "After previous item";	v = L"a"; break;
	case 4:		strName = "Before previous item";	v = L"p"; break;
	case 5:		strName = "Same as previous item";	v = L"s"; break;
	}
	
	*pCtx = (PVOID)++i;
	return 0;
}

DWORD CComGuiItemImpl::_GetPropPredefVal_AlignVertical(eFnGetPropPredefVal eOpType, CGuiItemEdit * pEditor, CItemBase * pItem, PVOID * pCtx, cStrObj& strName, CComVariant& v)
{
	if( eOpType == gpvGetCount )
		return *pCtx = NULL, 6;

	if( eOpType == gpvFinish )
		return 0;

	DWORD i = (DWORD)*pCtx;
	switch( i )
	{
	case 0:		strName = "Top";					v = L"t"; break;
	case 1:		strName = "Bottom";					v = L"b"; break;
	case 2:		strName = "Center";					v = L"c"; break;
	case 3:		strName = "After previous item";	v = L"a"; break;
	case 4:		strName = "Before previous item";	v = L"p"; break;
	case 5:		strName = "Same as previous item";	v = L"s"; break;
	}
	
	*pCtx = (PVOID)++i;
	return 0;
}

// ########################################################################
// ########################################################################
