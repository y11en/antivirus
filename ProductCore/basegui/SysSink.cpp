//////////////////////////////////////////////////////////////////////
// SysSink.cpp : implementation file

#include "StdAfx.h"
#include "SysSink.h"

#define IMPEX_IMPORT_LIB
    #include <Prague/plugin/p_win32_nfio.h>

//////////////////////////////////////////////////////////////////////
// CSkinDebugWindow

void CSkinDebugWindow::OnInit()
{
	CItemBase * pItem = NULL;
	m_pItem->LoadItem(NULL, pItem, "t(richedit) sz(p,p) at(readonly) text(strVal1)", "Info", LOAD_FLAG_DESTROY_SINK|LOAD_FLAG_INFO|LOAD_FLAG_PRELOAD);
	
	TBase::OnInit();
}

void CSkinDebugWindow::Show(CRootItem * pRoot)
{
	{
		CItemBase * pItem = NULL;
		while( pItem = pRoot->GetNextItem(pItem, m_strSection.c_str(), NULL, false) )
		{
			((CDialogItem *)pItem)->Activate();
			delete this;
			return;
		}
	}

	CDialogItem * pItem = NULL;
	pRoot->LoadItem(this, (CItemBase *&)pItem, "t(dialog) mg(7,7) at(resizable,nohelp,ontop) sz(200,150) text(\"Skin Debug\")", m_strSection.c_str(), LOAD_FLAG_DESTROY_SINK|LOAD_FLAG_INFO);

	if( !pItem )
	{
		delete this;
		return;
	}

	pItem->Activate();
}

void CSkinDebugWindow::ProcessPoint(POINT& pt, CItemBase * pTop)
{
	if( pTop == *this )
		return;
	
	CItemBase * pItem = pTop->HitTest(pt, pTop);
	if( !pItem )
	{
		m_info.m_strVal1.erase();
		UpdateData(true);
		return;
	}

	RECT rcFrame; pItem->GetParentRect(rcFrame, pTop);
	CItemBase * pParent = pItem->m_pParent;

	cStrObj& _dst = m_info.m_strVal1;
	cStrObj _str;
	
	_dst.erase();
	
	// Section
	{
		if( pItem->m_strSection.empty() && pParent )
		{
			_dst += L"<b>Parent Section:</b>\t";
			_dst += pParent->m_strSection.c_str();
		}
		else
		{
			_dst += L"<b>Section:</b>\t\t";
			_dst += pItem->m_strSection.c_str();
		}
		
		_dst += L"\n";
	}

	// Id
	{
		_dst += L"<b>Id:</b>\t\t";
		_dst += pItem->m_strItemId.c_str();
		_dst += L"\n";
	}

	// Full ID path
	{
		_dst += L"<b>Full path:</b>\t";
		_dst += pItem->GetOwnerRelativePath(TOR_tString, pItem->m_pRoot, true);
		_dst += L"\n";
	}

	// Top owner
	{
		_dst += L"<b>Top owner:</b>\t";
		_dst += pTop->m_strItemId.c_str();
		_dst += L"\n";
	}

	// Type
	if( !pItem->m_strItemType.empty() )
	{
		_dst += L"<b>Type:</b>\t\t";
		_dst += pItem->m_strItemType.c_str();
		_dst += L"\n";
	}

	// Sink
	if( pItem->m_pSink )
	{
		_dst += L"<b>Sink:</b>\t\t";
		_dst += L"yes";
		
		if( !pItem->m_strItemAlias.empty() )
		{
			_dst += L", \"";
			_dst += pItem->m_strItemAlias.c_str();
			_dst += L"\"";
		}
		
		_dst += L"\n";
	}

	// Sink
	if( pItem->m_pIcon )
	{
		_dst += L"<b>Icon:</b>\t\t";

		LPCSTR strIcon = pItem->m_pRoot->GetIconKey((CIcon *)(CImageBase *)pItem->m_pIcon);
		if( strIcon )
			_dst += strIcon;
		else
			_dst += L"<not skin>";

		_dst += L"\n";
	}

	// Rect
	{
		_dst += L"<b>Rectangle:</b>\t";
		_str.format(cCP_UNICODE, L"(%d, %d)-(%d,%d), %dx%d", rcFrame.left, rcFrame.top, rcFrame.right, rcFrame.bottom, RECT_CX(rcFrame), RECT_CY(rcFrame)); _dst += _str;
		_dst += L"\n";
	}
	
	// RichText Links
	if( pItem->m_aItems.size() )
	{
		_dst += L"\n<b>Children:</b>\n";
		for(tDWORD i = 0; i < pItem->m_aItems.size(); i++)
		{
			_dst += pItem->m_aItems[i]->m_strItemId.c_str();
			_dst += L"\n";
		}
	}
	
	// Text
	if( !pItem->m_strText.empty() )
	{
		_dst += L"\n<b>Text:</b>\n";
		Root()->LocalizeStr(_str, pItem->m_strText.c_str(), pItem->m_strText.size()); _dst += _str;
		_dst += L"\n";
	}

	UpdateData(true);
}

//////////////////////////////////////////////////////////////////////////
// CNavigatorSink

void CNavigatorSink::OnInitProps(CItemProps& pProps)
{
	CItemSinkT<>::OnInitProps(pProps);

	m_pItem->m_nFlags |= STYLE_SHEET_ITEM;

	m_bLoad = 0;
	
	CItemPropVals& l_prpAttr = pProps.Get(STR_PID_ATTRS);
	for(int p = 0, n = l_prpAttr.Count(); p < n; p++)
	{
		if( l_prpAttr.Get(p) == "loadpages" )
		{
			m_bLoad = 1;
			break;
		}
	}

	m_pButtons = Item()->GetItem(pProps.Get(STR_PID_BUTTONS).GetStr(), NULL, false);
	if( !m_pButtons )
		m_pButtons = Item();
	
	m_pWorkArea = Item()->GetItem(pProps.Get(STR_PID_WORKAREA).GetStr(), NULL, false);
	if( !m_pWorkArea )
		m_pWorkArea = Item()->GetItem("_client_area", NULL, false);
}

void CNavigatorSink::OnInit()
{

	if( m_pWorkArea )
	{
		m_pWorkArea->m_nFlags |= STYLE_SHEET_PAGES;
		for(tDWORD i = 0; i < m_pWorkArea->m_aItems.size(); i++)
			m_pWorkArea->m_aItems[i]->Show(false);
	}
}

void CNavigatorSink::OnChangedState(CItemBase * pItem, tDWORD nStateMask)
{
	if( !m_pButtons || !m_pWorkArea )
		return;
	if( !(nStateMask & ISTATE_SELECTED) )
		return;
	if( !pItem || pItem->m_pParent != m_pButtons )
		return;
	if( !(pItem->m_nState & ISTATE_SELECTED) )
		return;

	m_pWorkArea->LockUpdate(true);

	{
		CItemBase * pNewPanel = NULL;
		for(tDWORD i = 0; i < m_pWorkArea->m_aItems.size(); i++)
		{
			CItemBase * p = m_pWorkArea->m_aItems[i];
			if( p->m_strItemId == pItem->m_strItemId )
			{
				p->Show(true, SHOWITEM_DIRECT);
				pNewPanel = p;
			}
			else
				p->Show(false, SHOWITEM_DIRECT);
		}

		if( !pNewPanel && m_bLoad )
		{
			CItemBase * pOwner = m_pWorkArea->GetByFlagsAndState(STYLE_EXT_ITEM, ISTATE_ALL, NULL, CItemBase::gbfasUpToParent);
			if( pOwner )
			{
				tString strSection = pOwner->m_strSection;
				strSection += GUI_STR_AUTOSECT_SEP_STR;
				strSection += pItem->m_strItemId;
				m_pWorkArea->LoadItem(NULL, pNewPanel, strSection.c_str(), pItem->m_strItemId.c_str());
			}
		}
	}

	m_pWorkArea->LockUpdate(false);
}

bool CNavigatorSink::OnSelect(CItemBase * pItem)
{
	if( m_pButtons && pItem->m_pParent == m_pWorkArea )
		if( CItemBase * pBtn = m_pButtons->GetItem(pItem->m_strItemId.c_str(), NULL, false) )
			pBtn->Select();
	
	return CItemSinkT<>::OnSelect(pItem);
}

//////////////////////////////////////////////////////////////////////
// CMsgBox

CMsgBox::CMsgBox(CItemBase * pItem, LPCSTR strTextId, LPCSTR strCaptionId,
	tDWORD nIcon, tDWORD nActionsMask, cSerializable * pStruct) :
	TBase(pStruct, nActionsMask), m_pItem(pItem),
	m_strTextId(strTextId), m_strCaptionId(strCaptionId)
{
    m_sMsgBox.m_dwIcon = nIcon;
}

void CMsgBox::OnCreate()
{
	TBase::OnCreate();
	Item()->AddDataSource(&m_sMsgBox, "msg");

	CItemBase * pResourceItem = m_pItem ? m_pItem : Root();
	
	tString strCaption; Root()->GetString(strCaption, PROFILE_LOCALIZE, NULL, m_strCaptionId);
	if( strCaption.empty() )
	{
		if( CItemBase * pTopOwner = pResourceItem->GetOwner(true) )
			Root()->LocalizeStr(m_sMsgBox.m_strCaption, pTopOwner->GetDisplayText());
	}
	else
		Root()->GetFormatedText(m_sMsgBox.m_strCaption, strCaption.c_str(), Item()->GetBinder());
	
	Root()->GetFormatedText(m_sMsgBox.m_strText, Root()->GetString(TOR_tString, PROFILE_LOCALIZE, NULL, m_strTextId), Item()->GetBinder());
	if( m_sMsgBox.m_strText.empty() )
		Root()->LocalizeStr(m_sMsgBox.m_strText, m_strTextId);
}

int CMsgBox::Show(CRootItem * pRoot, CItemBase * pItem, LPCSTR strTextId, LPCSTR strCaptionId, tUINT uType, cSerializable * pData, CFieldsBinder * pBinder)
{
	tDWORD _nActionsMask = 0;
	switch( uType & MB_TYPEMASK )
	{
	case MB_OK:
		_nActionsMask = DLG_ACTION_OK;
		if( !(uType & MB_ICONMASK) ) uType |= MB_ICONINFORMATION; break;
	case MB_OKCANCEL:
		_nActionsMask = DLG_ACTION_OKCANCEL;
		if( !(uType & MB_ICONMASK) ) uType |= MB_ICONQUESTION; break;
	case MB_YESNO:
		_nActionsMask = DLG_ACTION_YESNO;
		if( !(uType & MB_ICONMASK) ) uType |= MB_ICONQUESTION; break;
	case MB_YESNOCANCEL:
		_nActionsMask = DLG_ACTION_YESNO|DLG_ACTION_CANCEL;
		if( !(uType & MB_ICONMASK) ) uType |= MB_ICONQUESTION; break;
	}

	return CMsgBox(pItem, strTextId, strCaptionId, uType & MB_ICONMASK, _nActionsMask|DLG_MODE_CENTERBUTTONS|DLG_MODE_NOSAVEPOS|DLG_MODE_ALWAYSMODAL, pData).DoModal(pItem, "MessageDialog", 0);
}

//////////////////////////////////////////////////////////////////////
// CBrowseShellObject

CBrowseShellObject::CBrowseShellObject(CBrowseObjectInfo& pBrowseInfo) :
	CDialogBindingT<>(pBrowseInfo.m_pData),
	m_pObject(pBrowseInfo),
	m_pTreeSink(NULL),
	m_bEdit(0), m_bBlockSelect(0)
{
}

void CBrowseShellObject::OnCreate()
{
	TBase::OnCreate();
	
	for(size_t i = 0; i < m_pObject.m_aDataEx.size(); i++)
		Item()->AddDataSource(m_pObject.m_aDataEx[i]);
}

void CBrowseShellObject::OnInit()
{
	m_pObject.m_pDlg = *this;
	if( m_pObject.m_pEnum )
		m_pObject.m_pEnum->OnCustomizeObject(false, m_pObject);

	TBase::OnInit();

	m_pEdit = GetItem("PathEdit", GUI_ITEMT_EDIT);
	m_pAddBtn = GetItem("Add");
	
	CItemBase * pTree = (CTreeItem *)GetItem("ShellTree", GUI_ITEMT_TREE);
	if( pTree && pTree->m_pSink )
		m_pTreeSink = (CTreeItemSink *)pTree->m_pSink;

	if( m_pTreeSink )
		m_pTreeSink->InitBrowseInfo(m_pObject);
}

void CBrowseShellObject::OnInited()
{
	TBase::OnInited();

	UpdateSelectionState();
	
	if( m_pEdit && !m_pObject.m_strDisplayName.empty() )
		m_pEdit->SetValue(cVariant(m_pObject.m_strDisplayName.data()));
	else if( m_pTreeSink )
		m_pTreeSink->Item()->SelectItem(m_pTreeSink->Item()->GetChildItem(NULL));
}

bool CBrowseShellObject::OnSelect(CItemBase * pItem)
{
	if( m_bBlockSelect )
		return true;
	
	bool bEnableOk = false;
	
	if( m_pTreeSink )
	{
		tObjPath strTmp = m_pObject.m_strDisplayName;
		m_pTreeSink->GetSelObject(m_pObject);
		if( m_bEdit )
			m_pObject.m_strName = m_pObject.m_strDisplayName;
		else if( m_pEdit )
			m_pEdit->SetValue(cVariant(m_pObject.m_strDisplayName.data()));

		bEnableOk = IsValidObject();
	}
	
	Item()->EnableAction(DLG_ACTION_OK, bEnableOk);
	if( m_pAddBtn )
		m_pAddBtn->Enable(bEnableOk);

	if( m_pObject.m_pEnum && m_pObject.m_pEnum->OnCustomizeObject(true, m_pObject) )
		UpdateData(m_pObject.m_pData, true);
	return true;
}

void CBrowseShellObject::OnChangedData(CItemBase * pItem)
{
	if( pItem != m_pEdit )
		return;
	
	m_bEdit = 1;
	cVariant res;
	m_pEdit->GetValue(res);
	res.ToStringObj(m_pObject.m_strName);
	m_pObject.m_strDisplayName.erase();
	m_pObject.m_nType = SHELL_OBJTYPE_UNKNOWN;
	m_pObject.m_nCustomId = 0;

	UpdateSelectionState();
	m_bEdit = 0;
}

bool CBrowseShellObject::OnOk()
{
	if( !CanAddObject() )
		return false;
	
	return TBase::OnOk();
}

bool CBrowseShellObject::OnClicked(CItemBase * pItem)
{
	if( m_pObject.m_pEnum && m_pAddBtn == pItem )
	{
		if( !CanAddObject() )
			return false;
		m_pObject.m_pEnum->OnAddObject(m_pObject);
		return true;
	}

	if( pItem->m_strItemId == "NewFolder" && IsValidObject() )
	{
		CObjectInfo ObjInfo;
		if( PR_SUCC(CreateNewSubFolder(ObjInfo)) && m_pTreeSink )
		{
			CTreeItem* pItem = m_pTreeSink->Item();
			cTreeItem* pSelection = pItem->GetSelectionItem();
			pItem->DeleteChildItems(pSelection);
			m_pTreeSink->OnUpdateItemProps(pSelection, UPDATE_FLAG_DATA);
			m_pTreeSink->SetSelObject(ObjInfo);
		}
		true;
	}

	return TBase::OnClicked(pItem);
}

bool CBrowseShellObject::OnClose(tDWORD& nResult)
{
	if( !TBase::OnClose(nResult) )
		return false;
	m_pObject.m_pDlg = NULL;
	return true;
}

bool CBrowseShellObject::OnMenu(CPopupMenuItem * pMenu, CItemBase * pItem)
{
	return IsValidObject();
}

//////////////////////////////////////////////////////////////////////

bool CBrowseShellObject::IsValidObject()
{
	bool bCustom = (m_pObject.m_nCustomId || ((m_pObject.m_nTypeMask & SHELL_OBJTYPE_MYCOMPUTER)
		&& m_pObject.m_nType == SHELL_OBJTYPE_MYCOMPUTER));

	if( bCustom )
		return true;
	
	if( m_pObject.m_pEnum && !m_pObject.m_pEnum->OnIsValidObject(m_pObject) )
		return false;
	return !m_pObject.m_strName.empty();
}

tERROR CBrowseShellObject::CreateNewSubFolder(CObjectInfo& ObjInfo)
{
	if( !PrCreateDir )
	    return errOBJECT_NOT_INITIALIZED;

	if( !m_pTreeSink->GetSelObject(ObjInfo) ||
		!m_pItem->m_pRoot->GetObjectInfo(&ObjInfo) ||
		ObjInfo.m_nType != SHELL_OBJTYPE_FOLDER )
		return errOBJECT_INCOMPATIBLE;

	tObjPath sNewFolder;
	tString strVal; m_pItem->m_pRoot->GetString(strVal, PROFILE_LOCALIZE, "BrowseForFolder", "NewFolderName");
	m_pItem->m_pRoot->LocalizeStr(sNewFolder, strVal.c_str(), strVal.size());

	tERROR err;

	tObjPath sFolderName;
	sFolderName = ObjInfo.m_strName;
	sFolderName.check_last_slash();

	cStrObj sSubFolderName;
	for(tDWORD i = 0; i < 100; i++)
	{
		if( i )
			sSubFolderName.format(cCP_UNICODE, L"%S (%u)", sNewFolder.data(), i + 1);
		else
			sSubFolderName = sNewFolder;

		ObjInfo.m_strName = sFolderName;
		ObjInfo.m_strName += sSubFolderName;

		err = PrCreateDir(cAutoString(ObjInfo.m_strName));
		if( err == errOK )
			return errOK;
		
		if( err != warnFALSE )
			return err;
	}

	return errEND_OF_THE_LIST;
}

void CBrowseShellObject::UpdateSelectionState()
{
	m_pObject.m_strDisplayName = m_pObject.m_strName;

	tDWORD nTypeMask = m_pObject.m_nTypeMask;

	bool bEnableOk = true;

	if( m_pObject.m_strName.empty() && !m_pObject.m_nCustomId && (m_pObject.m_nType != SHELL_OBJTYPE_MYCOMPUTER) )
		bEnableOk = false;
	else if( !m_pObject.m_strName.compare(0, 2, "\\\\") )
		bEnableOk = !!(nTypeMask & (SHELL_OBJTYPE_FOLDER|SHELL_OBJTYPE_FILE));
	else if( m_pObject.m_strName.find(L"://") != cStrObj::npos )
		bEnableOk = !!(nTypeMask & SHELL_OBJTYPE_URL);
	else
	{
		tObjPath strName = m_pObject.m_strName;
		CObjectInfo _SuccessSelObj; CTreeItemSink::CSuccessSel _SuccessSel(_SuccessSelObj);
		bEnableOk = m_pTreeSink ? m_pTreeSink->SetSelObject(m_pObject, &_SuccessSel) : false;
		if( !bEnableOk && (Root()->m_nGuiFlags & GUIFLAG_ADMIN) &&
			strName.size() >= 2 && strName[1] == L':' && strName[2] == L'\\' )
			bEnableOk = true;

		strName.erase(0, _SuccessSel.m_nSrcSuccSize);
		m_pObject.m_strName = _SuccessSel.m_Object.m_strName;
		m_pObject.m_strName += strName;
		m_pObject.m_strDisplayName = _SuccessSel.m_Object.m_strDisplayName;
		m_pObject.m_strDisplayName += strName;
		if( bEnableOk )
		{
			m_pObject.m_nType = _SuccessSel.m_Object.m_nType;
			m_pObject.m_nCustomId = _SuccessSel.m_Object.m_nCustomId;
		}

		if( !bEnableOk && m_pTreeSink )
		{
			m_bBlockSelect = 1;
			m_pTreeSink->SetSelObject(_SuccessSel.m_Object);
			m_bBlockSelect = 0;
		}
		
		if( (nTypeMask & SHELL_OBJTYPE_MASK) && !bEnableOk )
			bEnableOk = true;
	}

	if( bEnableOk )
		bEnableOk = IsValidObject();
	
	if( m_pItem )
	{
		Item()->EnableAction(DLG_ACTION_OK, bEnableOk);

		if( m_pAddBtn )
			m_pAddBtn->Enable(bEnableOk);
	}
}

bool CBrowseShellObject::UpdateBrowseObjectInfo()
{
	UpdateStruct();
	
	if( m_pObject.m_nType == SHELL_OBJTYPE_UNKNOWN )
	{
		m_pObject.m_nQueryMask = 0;
		if( !m_pItem->m_pRoot->GetObjectInfo(&m_pObject) )
			if( m_pObject.m_nTypeMask & SHELL_OBJTYPE_MASK )
				m_pObject.m_nType = SHELL_OBJTYPE_MASK;
			else
				m_pObject.m_nType = SHELL_OBJTYPE_UNKNOWN;
	}
	
	if( m_pObject.m_nType == SHELL_OBJTYPE_UNKNOWN && !m_pObject.m_strName.compare(0, 2, "\\\\") && Root()->ShowMsgBox(*this, "BrowseObjectNetPathUnavailableWarning", NULL, MB_YESNO|MB_ICONEXCLAMATION) == DLG_ACTION_YES )
		return true;
	
	if( Root()->m_nGuiFlags & GUIFLAG_ADMIN )
		return true;
	return m_pObject.m_nType != SHELL_OBJTYPE_UNKNOWN;
}

bool CBrowseShellObject::CanAddObject()
{
	if( !UpdateBrowseObjectInfo() )
		return false;
	
	return !m_pObject.m_pEnum || m_pObject.m_pEnum->OnCanAddObject(m_pObject);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
