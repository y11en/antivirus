/////////////////////////////////////////////////////////////////////////////
// ShellTreeCtrl.cpp : implementation file
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShellCtrls.h"

/////////////////////////////////////////////////////////////////////////////

IMalloc*      g_pMalloc = NULL;
IShellFolder* g_pFolderForCompare;

IMalloc* shell_GetMalloc()
{
	if( !g_pMalloc )
		::SHGetMalloc(&g_pMalloc);
	return g_pMalloc;
}

void     shell_FreeMalloc()
{
	if( g_pMalloc )
		g_pMalloc->Release(), g_pMalloc = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CShellTreeSink

CShellTreeSink::CShellTreeSink() : m_hImageList(NULL)
{
	m_nMask = SHELL_OBJTYPE_CUSTOM|SHELL_OBJTYPE_FOLDER|SHELL_OBJTYPE_FILE;
	m_hSysImageList = NULL;
}

CShellTreeSink::~CShellTreeSink()
{
	if( m_hImageList )
		ImageList_Destroy(m_hImageList);
}

void CShellTreeSink::OnInited()
{
	CShellPidl pidl((UINT)CSIDL_DESKTOP, Item()->GetWindow());

	{
		SHFILEINFO sfi;
		ZeroMemory(&sfi, sizeof(SHFILEINFO));
		m_hSysImageList = (HIMAGELIST) SHGetFileInfo((LPCTSTR)(LPCITEMIDLIST)pidl,
			0, &sfi, sizeof(SHFILEINFO), SHGFI_PIDL|SHGFI_SYSICONINDEX|SHGFI_SMALLICON);
	}

	HIMAGELIST hSysImageList = m_hSysImageList;
	if( hSysImageList )
	{
		m_hImageList = ImageList_Duplicate(hSysImageList);
		ImageList_SetImageCount(m_hImageList, 0);
		Item()->SetImageList(m_hImageList);
	}

	DWORD nFlags = STCF_INCLUDEHIDDEN;
	if( m_nMask & SHELL_OBJTYPE_FILE || m_nMask & SHELL_OBJTYPE_MASK )
		nFlags |= STCF_INCLUDEFILES;

	SHGetDesktopFolder(&m_pDesktopFolder);
	m_pRoot = Item()->AppendItem(NULL, new TVITEMDATA(NULL, m_pDesktopFolder, pidl.Detach(), nFlags));
	Item()->ExpandItem(m_pRoot);
}

bool CShellTreeSink::SetSelObject(CObjectInfo& pObject, CSuccessSel * pSuccessSel)
{
	USES_CONVERSION;
	if( pObject.m_nCustomId || pObject.m_strName.empty() )
	{
		cTreeItem * pSelItem = NULL;
		if( pObject.m_nType == SHELL_OBJTYPE_MYCOMPUTER )
			pSelItem = GotoPIDL(m_pRoot, CShellPidl((UINT)CSIDL_DRIVES, Item()->GetWindow()));
		else if( pObject.m_nCustomId )
			pSelItem = GotoCustom(m_pRoot, pObject.m_nCustomId);

		if( pSelItem && pSuccessSel )
		{
			GetObjectInfo(pSuccessSel->m_Object, (TVITEMDATA *)pSelItem);
			pSuccessSel->m_nSrcSuccSize = pObject.m_strName.size();
		}
		
		if( pSelItem )
			return true;
	}
	else
	{
		if( pObject.m_strName.compare(0, 2, "\\\\") == cSTRING_COMP_EQ )
		{
			if( pSuccessSel )
			{
				pSuccessSel->m_Object.m_strName = pObject.m_strName;
				pSuccessSel->m_Object.m_strDisplayName = pObject.m_strName;
				pSuccessSel->m_nSrcSuccSize = pObject.m_strName.size();
			}

			return true;
		}

		HWND hParent = NULL;
		{
			CItemBase * pOwner = Item()->GetOwner(true);
			if( !pOwner )
				pOwner = Item();
			hParent = pOwner->GetWindow();
		}

		CShellPidl pidlToGo;

		BOOL bParentEnabled = ::IsWindowEnabled(hParent);
		if( bParentEnabled )
			::EnableWindow(hParent, FALSE);

		if( (m_nMask & SHELL_OBJTYPE_PERSONAL) &&
			!wcsncmp(pObject.m_strName.data(), L"%personal%", 10) )
		{
			SHGetSpecialFolderLocation(hParent, CSIDL_PERSONAL, &pidlToGo);

			if( bParentEnabled )
				::EnableWindow(hParent, TRUE);

			cTreeItem * pItem = GotoPIDL(m_pRoot, pidlToGo, false);
			if( !pItem )
				return false;

			_success_path_int _LastSuccess; _LastSuccess.m_pLastSuccess = pItem; _LastSuccess.m_nSrcSuccSize = 10;

			bool ok = true;
			if( pObject.m_strName.size() > 10 )
			{
				Item()->ExpandItem(pItem);
				ok = GotoDisplayName(pItem, pObject.m_strName, _LastSuccess.m_nSrcSuccSize + 1, &_LastSuccess);
			}
			else
				Item()->SelectItem(pItem);

			if( pSuccessSel && _LastSuccess.m_pLastSuccess )
			{
				GetObjectInfo(pSuccessSel->m_Object, (TVITEMDATA *)_LastSuccess.m_pLastSuccess);
				pSuccessSel->m_nSrcSuccSize = _LastSuccess.m_nSrcSuccSize;
			}

			return ok;
		}
		else
		{
			tObjPath strPathTest = pObject.m_strName;
			tDWORD nSlashPos = pObject.m_strName.size();

			for(;;)
			{
				m_pDesktopFolder->ParseDisplayName(hParent, NULL,
					(LPWSTR)strPathTest.data(), NULL, &pidlToGo, NULL);
				if( pidlToGo.IsValid() )
					break;
				
				nSlashPos = strPathTest.find_last_of(L"\\/");
				if( nSlashPos == cStrObj::npos )
					break;
				strPathTest.erase(nSlashPos);
			}

			if( bParentEnabled )
				::EnableWindow(hParent, TRUE);

			bool bAllPath = nSlashPos == pObject.m_strName.size();
			cTreeItem * pSelItem;
			if( pidlToGo.IsValid() && (pSelItem = GotoPIDL(m_pRoot, pidlToGo, bAllPath)) )
			{
				if( pSuccessSel )
				{
					GetObjectInfo(pSuccessSel->m_Object, (TVITEMDATA *)pSelItem);
					pSuccessSel->m_nSrcSuccSize = bAllPath ? pObject.m_strName.size() : nSlashPos;
				}

				return bAllPath;
			}
		}
	}

	// Try to locate item by display name
	_success_path_int _LastSuccess; _LastSuccess.m_pLastSuccess = NULL; _LastSuccess.m_nSrcSuccSize = 0;
	bool ok = GotoDisplayName(m_pRoot, pObject.m_strName, 0, &_LastSuccess);
	
	if( pSuccessSel && _LastSuccess.m_pLastSuccess )
	{
		GetObjectInfo(pSuccessSel->m_Object, (TVITEMDATA *)_LastSuccess.m_pLastSuccess);
		pSuccessSel->m_nSrcSuccSize = _LastSuccess.m_nSrcSuccSize;
	}
	
	return ok;
}

bool CShellTreeSink::GetSelObject(CObjectInfo& pObject)
{
	cTreeItem * pSel = Item()->GetSelectionItem();
	if( !pSel )
		return false;
	return GetObjectInfo(pObject, (TVITEMDATA *)pSel);
}

bool CShellTreeSink::GetObjectInfo(CObjectInfo& pObject, TVITEMDATA * pItem)
{
	pObject.m_nType = SHELL_OBJTYPE_UNKNOWN;
	pObject.m_strDisplayName = pItem->m_strName;
	pObject.m_strName.erase();
	
	TVITEMDATA * pRootPersonalItem = NULL;
	
	if( pItem->nCustomId )
	{
		pObject.m_nCustomId = pItem->nCustomId;
		pObject.m_nType = SHELL_OBJTYPE_CUSTOM;
		pObject.m_strShortDisplayName = pObject.m_strDisplayName;
	}
	else
	{
		pObject.m_nCustomId = 0;
		if( IsMyComputer(pItem) )
		{
			if( !(m_nMask & SHELL_OBJTYPE_MYCOMPUTER) )
				return false;

			pObject.m_nType = SHELL_OBJTYPE_MYCOMPUTER;
			pObject.m_strShortDisplayName = pObject.m_strDisplayName;
			pObject.m_strName.erase();
		}
		else if( IsPersonalFolder(pItem, &pRootPersonalItem) )
		{
			if( pItem->dwAttributes & SFGAO_FOLDER )
				pObject.m_nType = SHELL_OBJTYPE_FOLDER;
			else
				pObject.m_nType = SHELL_OBJTYPE_FILE;
			
			pObject.m_strShortDisplayName = pObject.m_strDisplayName;
			pObject.m_strName = L"%personal%";
			
			if( pRootPersonalItem )
			{
				cVectorSimple(TVITEMDATA *) _aPersPath;
				for(TVITEMDATA * pPersItem = pItem; pPersItem != pRootPersonalItem; pPersItem = pPersItem->pParent)
					_aPersPath.insert(0) = pPersItem;

				pObject.m_strDisplayName = pRootPersonalItem->m_strName;
				for(tDWORD i = 0; i < _aPersPath.size(); i++)
				{
					pObject.m_strDisplayName.add_path_sect(_aPersPath[i]->m_strName);
					pObject.m_strName.add_path_sect(_aPersPath[i]->m_strName);
				}
			}
		}
		else
		{
			tWCHAR strPath[2*MAX_PATH] = L"";
			if( !SHGetPathFromIDListW(GetFullPIDL(pItem), (WCHAR*)strPath) )
				return false;
			
			strPath[countof(strPath) - 1] = 0;
			
			if( (Item()->m_pRoot->m_nGuiFlags & GUIFLAG_ADMIN) && wcsncmp(strPath, L"\\\\", 2) )
				strPath[0] = 0;

			if( wcsstr(strPath, L"::{") )
				return false;

			pObject.m_strShortDisplayName = pObject.m_strDisplayName;
			pObject.m_strDisplayName = strPath;
			pObject.m_strName = strPath;
		}
		
		if( const tWCHAR * strShortName = wcsrchr(pObject.m_strName.data(), '\\') )
			pObject.m_strShortName = ++strShortName;
	}
	
	return true;
}

cTreeItem * CShellTreeSink::GotoPIDL(cTreeItem * pParent, LPITEMIDLIST pidl, bool bSelect)
{
	if( !pidl )
		return NULL;

	for(cTreeItem * pItem = Item()->GetChildItem(pParent); pItem; pItem = Item()->GetNextItem(pItem))
	{
		TVITEMDATA * pData = (TVITEMDATA *)pItem;
		LPITEMIDLIST ipidl = pData->pidl;
		if( !ipidl || ipidl->mkid.cb != pidl->mkid.cb )
			continue;
		if( memcmp(ipidl, pidl, pidl->mkid.cb) )
			continue;

		LPITEMIDLIST pidlnext = (LPITEMIDLIST)((LPBYTE)pidl + pidl->mkid.cb);

		if( pidlnext->mkid.cb )
		{
			Item()->ExpandItem(pItem);
			return GotoPIDL(pItem, pidlnext, bSelect);
		}

		Item()->EnsureVisibleItem(pItem);
		if( bSelect )
			Item()->SelectItem(pItem);
		return pItem;
	}
	
	return NULL;
}

bool CShellTreeSink::GotoDisplayName(cTreeItem * pParent, tObjPath& strName, tDWORD nNamePos, _success_path_int * pLastSuccess)
{
	cStrObj strN;
	tDWORD nSepPos = strName.find_first_of(L"\\/", nNamePos);
	if( nSepPos == cStrObj::npos )
		strN.assign(strName, nNamePos);
	else
		strN.assign(strName, nNamePos, nSepPos - nNamePos);

	for(cTreeItem * pItem = Item()->GetChildItem(pParent); pItem; pItem = Item()->GetNextItem(pItem))
	{
		TVITEMDATA * pData = (TVITEMDATA *)pItem;
		if( pData->m_strName.empty() )
			OnUpdateItemProps(pData, UPDATE_FLAG_TEXT);
		
		if( pData->m_strName.compare(strN, fSTRING_COMPARE_CASE_INSENSITIVE) != cSTRING_COMP_EQ )
			continue;

		if( nSepPos != cStrObj::npos )
		{
			Item()->ExpandItem(pItem);
			if( GotoDisplayName(pItem, strName, nSepPos + 1, pLastSuccess) )
				return true;
			
			if( pLastSuccess )
			{
				pLastSuccess->m_pLastSuccess = pItem;
				pLastSuccess->m_nSrcSuccSize = nSepPos;
			}
			return false;
		}

		Item()->EnsureVisibleItem(pItem);
		Item()->SelectItem(pItem);
		if( pLastSuccess )
		{
			pLastSuccess->m_pLastSuccess = pItem;
			pLastSuccess->m_nSrcSuccSize = strName.size();
		}
		return true;
	}

	return false;
}

cTreeItem * CShellTreeSink::GotoCustom(cTreeItem * pParent, tDWORD nCustomId)
{
	for(cTreeItem * pItem = Item()->GetChildItem(pParent); pItem; pItem = Item()->GetNextItem(pItem))
	{
		TVITEMDATA * pData = (TVITEMDATA *)pItem;
		if( pData->nCustomId != nCustomId )
			continue;
		
		Item()->EnsureVisibleItem(pItem);
		Item()->SelectItem(pItem);
		return pItem;
	}

	return NULL;
}

bool CShellTreeSink::IsMyComputer(TVITEMDATA * pTreeItem)
{
	static CShellPidl pidlMyComputer((UINT)CSIDL_DRIVES, Item()->GetWindow());
    return !HRESULT_CODE(pTreeItem->pFolder->CompareIDs(0, pTreeItem->pidl, pidlMyComputer));
}

bool CShellTreeSink::IsPersonalFolder(TVITEMDATA * pItem, TVITEMDATA ** pRootPersonalItem)
{
	if( !(m_nMask & SHELL_OBJTYPE_PERSONAL) )
		return false;

	static CShellPidl pidlPersonal((UINT)CSIDL_PERSONAL, Item()->GetWindow());

	if( pItem->pPersonal == 3 )
	{
		return IsPersonalFolder((TVITEMDATA *)pItem->pParent, pRootPersonalItem);
	}

	if( pItem->pPersonal == 1 )
		return false;

	if( pItem->pPersonal == 2 || !HRESULT_CODE(pItem->pFolder->CompareIDs(0, pItem->pidl, pidlPersonal)) )
	{
		if( pRootPersonalItem )
			*pRootPersonalItem = pItem;
		pItem->pPersonal = 2;
		return true;
	}

	if( !pItem->pParent || !IsPersonalFolder((TVITEMDATA *)pItem->pParent, pRootPersonalItem) )
	{
		pItem->pPersonal = 1;
		return false;
	}

	pItem->pPersonal = 3;
	return true;
}

CShellPidl CShellTreeSink::GetFullPIDL(TVITEMDATA* pData)
{
	DWORD nSize = 0;
	TVITEMDATA* d;
	for(d = pData; d; d = (TVITEMDATA*)d->pParent)
		nSize += d->pidl->mkid.cb;

	LPITEMIDLIST pidl = (LPITEMIDLIST)shell_GetMalloc()->Alloc(nSize + sizeof(USHORT));
	LPITEMIDLIST pidlEnd = (LPITEMIDLIST)((LPBYTE)pidl + nSize);
	pidlEnd->mkid.cb = 0;

	for(d = pData; d; d = (TVITEMDATA*)d->pParent)
	{
		pidlEnd = (LPITEMIDLIST)((LPBYTE)pidlEnd - d->pidl->mkid.cb);
		CopyMemory(pidlEnd, (LPITEMIDLIST)d->pidl, d->pidl->mkid.cb);
	}
	return pidl;
}

tUINT CShellTreeSink::GetImageIndex(int nId, HICON hIcon)
{
	if( !m_hImageList )
		return 0;

	for(size_t i = 0; i < m_vecImages.size(); i++)
		if( nId == m_vecImages[i] )
			return (tUINT)i;

	int nIndex = -1;
	if( !hIcon && nId >= 0 )
	{
		hIcon = ImageList_GetIcon(m_hSysImageList, nId, ILD_NORMAL);
		if( !hIcon )
			return -1;
		nIndex = ImageList_AddIcon(m_hImageList, hIcon);
		::DestroyIcon(hIcon);
	}
	else
		nIndex = ImageList_AddIcon(m_hImageList, hIcon);

	if(nIndex!=-1)
		m_vecImages.push_back(nId);

	return nIndex;
}

void CShellTreeSink::InitItemImage(TVITEMDATA * pData, bool bSelected)
{
	if( bSelected && !(pData->dwAttributes & SFGAO_FOLDER) )
	{
		pData->m_nSelectedImage = pData->m_nImage;
		return;
	}

	SHFILEINFO sfi;
	ZeroMemory(&sfi, sizeof(SHFILEINFO));
	
	UINT uFlags = SHGFI_PIDL|SHGFI_SMALLICON|SHGFI_SYSICONINDEX;
	if( bSelected )
		uFlags |= SHGFI_OPENICON;
	
	if( pData->dwAttributes & SFGAO_FILESYSTEM )
		SHGetFileInfo((LPCTSTR)pData->pidl, 0, &sfi, sizeof(SHFILEINFO), uFlags);
	
	if( !sfi.iIcon )
	{
		CShellPidl& pidl = GetFullPIDL(pData);
		SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(SHFILEINFO), uFlags);
	}
	
	UINT& nImage = bSelected ? pData->m_nSelectedImage : pData->m_nImage;
	nImage = GetImageIndex(sfi.iIcon, NULL);
}

void CShellTreeSink::OnUpdateItemProps(cTreeItem * pItem, tDWORD flags)
{
	TVITEMDATA * pData = (TVITEMDATA *)pItem;
	
	if( pData->nCustomId )
	{
		pData->m_nSelectedImage = pData->m_nImage;
		pData->m_nChildren = 0;
		return;
	}

	if( flags & UPDATE_FLAG_DATA )
	{
		pData->pFolder->ParseDisplayName(NULL, NULL, (LPWSTR)pItem->m_strName.data(), NULL, &pData->pidl, NULL);
	}

	if( flags & (UPDATE_FLAG_ICON|UPDATE_FLAG_ICONSELECTED|UPDATE_FLAG_SUBCOUNT) )
		if( pData->dwAttributes == -1 )
		{
			// check if item being processed is both file and container
			// in this case we don't check if it has subfolders
			DWORD dwAttr= SFGAO_FOLDER|SFGAO_ISSLOW|SFGAO_STREAM;
			bool bCheckSubfolders = SUCCEEDED(pData->pFolder->GetAttributesOf(1, pData->pidl, &dwAttr)) &&
				dwAttr & SFGAO_FOLDER && !(dwAttr & SFGAO_ISSLOW || dwAttr & SFGAO_STREAM);

			pData->dwAttributes = SFGAO_FOLDER|SFGAO_LINK|SFGAO_SHARE|SFGAO_GHOSTED|0x4;
			if(bCheckSubfolders)
				pData->dwAttributes |= SFGAO_HASSUBFOLDER;
			pData->pFolder->GetAttributesOf(1, pData->pidl, &pData->dwAttributes);
		}

	if( flags & UPDATE_FLAG_STATE )
	{
		// set correct icon
		if( pData->dwAttributes & SFGAO_GHOSTED )
		{
			pData->m_stateMask |= LVIS_CUT;
			pData->m_state |= LVIS_CUT;
		}
		if( pData->dwAttributes & SFGAO_SHARE )
		{
			pData->m_state &= ~LVIS_OVERLAYMASK;
			pData->m_state |= INDEXTOOVERLAYMASK(1);
			pData->m_stateMask |= LVIS_OVERLAYMASK;
		}
		else if( pData->dwAttributes & SFGAO_LINK )
		{
			pData->m_state &= ~LVIS_OVERLAYMASK;
			pData->m_state |= INDEXTOOVERLAYMASK(2);
			pData->m_stateMask |= LVIS_OVERLAYMASK;
		}
	}

	if( flags & UPDATE_FLAG_ICON )
		InitItemImage(pData, false);
	if( flags & UPDATE_FLAG_ICONSELECTED )
		InitItemImage(pData, true);

	if( flags & UPDATE_FLAG_TEXT )
	{
		// get display name
		STRRET strResult;
		HRESULT hr = pData->pFolder->GetDisplayNameOf(pData->pidl, SHGDN_INFOLDER, &strResult);
		if( SUCCEEDED(hr) )
		{
			if( strResult.uType == STRRET_WSTR )
			{
				pData->m_strName = strResult.pOleStr;
				shell_GetMalloc()->Free(strResult.pOleStr);
			}
			else if( strResult.uType == STRRET_CSTR )
				pData->m_strName = strResult.cStr;
			else if( strResult.uType == STRRET_OFFSET )
				pData->m_strName = (LPSTR)(LPITEMIDLIST)pData->pidl + strResult.uOffset;
		}
	}

	if( flags & UPDATE_FLAG_SUBCOUNT )
	{
		pData->m_nChildren = pData->dwAttributes & SFGAO_FOLDER && !(pData->dwAttributes & SFGAO_STREAM) ? 1 : 0;
	}
}

bool CShellTreeSink::OnItemExpanding(cTreeItem * pItem, tUINT& nChildren)
{
	TVITEMDATA * pData = (TVITEMDATA *)pItem;

	// get parent pidl
	bool bIsRoot = !pData->pidl->mkid.cb;

	SShellFolderPtr pFolder;
	if( !bIsRoot )
	{
		pData->pFolder->BindToObject(pData->pidl,
			NULL, IID_IShellFolder, (LPVOID*)&pFolder);
	}
	else
	{
		pFolder = m_pDesktopFolder;

		CRootSink* pSink = (CRootSink*)Root()->m_pSink;
		if( m_pInfo && m_pInfo->m_pEnum && pSink && (m_nMask & SHELL_OBJTYPE_CUSTOM) )
		{
			CObjectInfo pObject;
			pObject.m_nQueryMask = OBJINFO_DISPLAYNAME|OBJINFO_ICON;
			pObject.m_nType = SHELL_OBJTYPE_CUSTOM;
			pObject.m_nCustomId = 0;

			while( m_pInfo->m_pEnum->NextObject(pObject.m_nCustomId) )
			{
				if( !pSink->GetCustomObjectInfo(pObject) )
					continue;

				TVITEMDATA * pChild = new TVITEMDATA(pData, pObject.m_nCustomId);
				pChild->m_strName = pObject.m_strDisplayName;
				pChild->m_nImage = pObject.m_pIcon ? GetImageIndex(-(int)pObject.m_nCustomId, pObject.m_pIcon->IconHnd()) : -1;
				Item()->AppendItem(pItem, pChild);
			}
		}
	}

	if( !pFolder )
		return nChildren = 0, false;

	// enum child pidls
	DWORD nFlags = SHCONTF_FOLDERS;
	if( pData->nFlags & STCF_INCLUDEFILES )
		nFlags |= SHCONTF_NONFOLDERS;
	if( pData->nFlags & STCF_INCLUDEHIDDEN )
		nFlags |= SHCONTF_INCLUDEHIDDEN;

	SEnumIDListPtr pIDListPtr;
	pFolder->EnumObjects(Item()->GetWindow(), nFlags, &pIDListPtr);

	if( !pIDListPtr )
		return nChildren = 0, false;

	DWORD nCount = 0;
	DWORD nSize = 0x1000, nReadSize;
	LPITEMIDLIST* pIDList = (LPITEMIDLIST*)malloc(nSize*sizeof(LPITEMIDLIST));
	while( pIDListPtr->Next(nSize/2, pIDList+nCount, &nReadSize) == NOERROR )
	{
		nCount += nReadSize;
		if( nCount + nSize/2 > nSize )
			pIDList = (LPITEMIDLIST*)realloc(pIDList,
				(nCount+nSize)*sizeof(LPITEMIDLIST));
	}

	g_pFolderForCompare = pFolder;
	qsort(pIDList, nCount, sizeof(LPITEMIDLIST), SortCompare);

	for(DWORD i = 0; i < nCount; i++ )
	{
		LPITEMIDLIST _pidl = pIDList[i];
		bool bDisableChildren = false;
		
		if( Root()->m_nGuiFlags & GUIFLAG_ADMIN )
		{
			static CShellPidl pidlNetwork((UINT)CSIDL_NETWORK, Item()->GetWindow());
			
			if( !pData->pParent /*|| !HRESULT_CODE(pData->pParent->pFolder->CompareIDs(0, pData->pidl, pidlNetwork))*/ )
			{
				static CShellPidl pidlPersonal((UINT)CSIDL_PERSONAL, Item()->GetWindow());
				static CShellPidl pidlNetHood((UINT)CSIDL_NETHOOD, Item()->GetWindow());
				static CShellPidl pidlMyComputer((UINT)CSIDL_DRIVES, Item()->GetWindow());

				bool bHide = true;
				
				if( bHide && !HRESULT_CODE(pFolder->CompareIDs(0, _pidl, pidlPersonal)) )
				{
					bHide = false;
					bDisableChildren = true;
				}
				
				if( bHide && !HRESULT_CODE(pFolder->CompareIDs(0, _pidl, pidlMyComputer)) )
				{
					bHide = false;
					bDisableChildren = true;
				}
				
				if( bHide && !HRESULT_CODE(pFolder->CompareIDs(0, _pidl, pidlNetwork)) )
					bHide = false;
				
				if( bHide && !HRESULT_CODE(pFolder->CompareIDs(0, _pidl, pidlNetHood)) )
					bHide = false;
				
				if( bHide )
					continue;
			}
		}
		
		TVITEMDATA * pItemData = new TVITEMDATA(pData, pFolder, _pidl, pData->nFlags);
	
		tDWORD itemFlags = UPDATE_FLAG_TEXT|UPDATE_FLAG_ICON|UPDATE_FLAG_ICONSELECTED|UPDATE_FLAG_SUBCOUNT;

		if( (m_nMask & SHELL_OBJTYPE_FILTERED) && m_pInfo->m_pEnum )
		{
			OnUpdateItemProps(pItemData, itemFlags);

			CObjectInfo _Obj;
			GetObjectInfo(_Obj, pItemData);
			
			_Obj.m_nQueryMask = 0;
			Root()->GetObjectInfo(&_Obj);
			
			if( !m_pInfo->m_pEnum->FilterObject(_Obj) )
			{
				delete pItemData;
				continue;
			}
		}
		
		Item()->AppendItem(pItem, pItemData);
		if( bIsRoot )
			OnUpdateItemProps(pItemData, itemFlags);

		if( bDisableChildren )
		{
			pItemData->m_flags |= UPDATE_FLAG_SUBCOUNT;
			pItemData->m_nChildren = 0;
		}

		#ifdef _DEBUG
//		PR_TRACE((g_root, prtIMPORTANT, "GUI::ShellTree: %s", item.pszText));
		#endif
	}
	
	free(pIDList);

	return nChildren = nCount, false;
}

int CShellTreeSink::SortCompare(const void *elem1, const void *elem2 )
{
	LPITEMIDLIST pidl1 = *(LPITEMIDLIST*)elem1;
	LPITEMIDLIST pidl2 = *(LPITEMIDLIST*)elem2;

	HRESULT hr = g_pFolderForCompare->CompareIDs(0, pidl1, pidl2);
	if( !FAILED(hr) )
	{
		short ret = (short)HRESULT_CODE(hr);
		if (ret < 0) return -1;
		if (ret > 0) return 1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

void CRegTreeSink::OnInited()
{
	if( CIcon * pImages = Root()->GetIcon("regicons") )
	{
		HIMAGELIST hImageList = (HIMAGELIST)pImages->ImageList();
		if( hImageList )
			Item()->SetImageList(hImageList);
	}

	TVITEMDATA* pRoot = new TVITEMDATA(NULL, SHELL_OBJTYPE_UNKNOWN);
	tString strVal; Root()->GetString(strVal, PROFILE_LOCALIZE, NULL, "RegRoot");
	Root()->LocalizeStr(pRoot->m_strName, strVal.c_str(), strVal.size());

	m_pRoot = Item()->AppendItem(NULL, pRoot);
	Item()->ExpandItem(m_pRoot);

	if( m_pInfo )
		SetSelObject(*m_pInfo);
}

bool CRegTreeSink::SetSelObject(CObjectInfo& pObject, CSuccessSel * pSuccessSel)
{
	return GotoKey(m_pRoot, Root()->LocalizeStr(TOR(tString,()), pObject.m_strName.data(), pObject.m_strName.size()));
}

bool CRegTreeSink::GetSelObject(CObjectInfo& pObject)
{
	cTreeItem * pSel = Item()->GetSelectionItem();
	if( !pSel )
		return false;

	TVITEMDATA * pSelItem = (TVITEMDATA *)pSel;

	pObject.m_nType = pSelItem->nType;
	pObject.m_strName.erase();

	GetFullName(pSelItem, true, pObject.m_strName);
	pObject.m_strShortName = pSelItem->m_strName;
	return true;
}

void CRegTreeSink::ShortRegRoot2LongRegRoot(tString &strKey)
{
	sswitch(strKey.c_str())
	scase("HKCR") strKey = "HKEY_CLASSES_ROOT";   sbreak;
	scase("HKCU") strKey = "HKEY_CURRENT_USER";   sbreak;
	scase("HKLM") strKey = "HKEY_LOCAL_MACHINE";  sbreak;
	scase("HKU")  strKey = "HKEY_USERS";          sbreak;
	scase("HKCC") strKey = "HKEY_CURRENT_CONFIG"; sbreak;
	send
}

bool CRegTreeSink::GotoKey(cTreeItem * pParent, LPCSTR strKeyPath)
{
	LPCSTR strFind = strchr(strKeyPath, '\\');

	tString strKey;
	if( strFind )
		strKey.assign(strKeyPath, strFind - strKeyPath);
	else
		strKey = strKeyPath;

	if( pParent == m_pRoot )
		ShortRegRoot2LongRegRoot(strKey);

	for(cTreeItem * pItem = Item()->GetChildItem(pParent); pItem; pItem = Item()->GetNextItem(pItem) )
	{
		TVITEMDATA * pData = (TVITEMDATA *)pItem;

		if( pData->m_strName.compare(0, cStrObj::whole, strKey.c_str(), fSTRING_COMPARE_CASE_INSENSITIVE) != cSTRING_COMP_EQ )
			continue;

		Item()->ExpandItem(pItem);

		if( strFind && *++strFind )
			return GotoKey(pItem, strFind);

		Item()->EnsureVisibleItem(pItem);
		Item()->SelectItem(pItem);
		return true;
	}

	return false;
}

bool CRegTreeSink::GetFullName(TVITEMDATA* pData, bool bWithRoot, tObjPath& strName)
{
	std::wstring strPath;
	for(TVITEMDATA* d = pData; d->pParent; d = (TVITEMDATA*)d->pParent)
	{
		if( !bWithRoot && !d->pParent->pParent )
			break;

		if( d != pData )
			strPath.insert(0, L"\\");
		strPath.insert(0, d->m_strName.data());
	}
	strName = strPath.c_str();
	return true;
}

void CRegTreeSink::AddKey(TVITEMDATA * pParent, HKEY pKey, LPCSTR strKey)
{
	TVITEMDATA * pItem = new TVITEMDATA(pParent, SHELL_OBJTYPE_REGKEY);
	pItem->pRootKey = pParent == m_pRoot ? pKey : pParent->pRootKey;
	pItem->m_strName = strKey;
	pItem->m_flags = UPDATE_FLAG_TEXT|UPDATE_FLAG_ICON|UPDATE_FLAG_ICONSELECTED;
	pItem->m_nImage = 1;
	pItem->m_nSelectedImage = 2;

	DWORD nSubKeysLen = 0, nValuesNameLen = 0, nValuesLen;

	if( pKey && !(Root()->m_nGuiFlags & GUIFLAG_ADMIN) )
		RegQueryInfoKey(pKey, NULL, NULL, 0, &pItem->nSubKeys, &nSubKeysLen, NULL,
			&pItem->nValues, &nValuesNameLen, &nValuesLen, NULL, NULL);

	pItem->nMaxLen = max(nSubKeysLen, nValuesNameLen) + 1;
	pItem->m_nChildren = pItem->nSubKeys + pItem->nValues;

	Item()->AppendItem(pParent, pItem);
}

void CRegTreeSink::AddValue(TVITEMDATA * pParent, LPCSTR strName, DWORD nValType)
{
	TVITEMDATA * pItem = new TVITEMDATA(pParent, SHELL_OBJTYPE_REGVALUE);
	pItem->m_strName = *strName ? strName : "(Default)";
	pItem->m_flags = UPDATE_FLAG_TEXT|UPDATE_FLAG_ICON|UPDATE_FLAG_ICONSELECTED;
	pItem->m_nChildren = 0;

	if( nValType == REG_SZ || nValType == REG_MULTI_SZ || nValType == REG_EXPAND_SZ )
		pItem->m_nImage = 3;
	else
		pItem->m_nImage = 4;
	pItem->m_nSelectedImage = pItem->m_nImage;

	Item()->AppendItem(pParent, pItem);
}

int CRegTreeSink::OnSortCompare(cTreeItem * pItem1, cTreeItem * pItem2)
{
	TVITEMDATA * pData1= (TVITEMDATA *)pItem1;
	TVITEMDATA * pData2= (TVITEMDATA *)pItem2;

	if( pData1->nType != pData2->nType )
		return pData1->nType == SHELL_OBJTYPE_REGKEY ? 1 : -1;

	return pData1->m_strName.compare(0, cStrObj::whole, pData2->m_strName);
}

bool CRegTreeSink::OnItemExpanding(cTreeItem * pItem, tUINT& nChildren)
{
	TVITEMDATA * pData= (TVITEMDATA *)pItem;

	if( pItem == m_pRoot )
	{
		AddKey(pData, HKEY_CLASSES_ROOT,   "HKEY_CLASSES_ROOT");
		AddKey(pData, HKEY_CURRENT_USER,   "HKEY_CURRENT_USER");
		AddKey(pData, HKEY_LOCAL_MACHINE,  "HKEY_LOCAL_MACHINE");
		AddKey(pData, HKEY_USERS,          "HKEY_USERS");
		AddKey(pData, HKEY_CURRENT_CONFIG, "HKEY_CURRENT_CONFIG");
		return nChildren = 5, false;
	}

	if( !pData->m_nChildren )
		return nChildren = 0, false;

	tObjPath strKeyName;
	GetFullName(pData, false, strKeyName);

	HKEY hKey; LONG res;
	if( CWindowL::m_bAnsi )
		res = RegOpenKeyExA(pData->pRootKey, Root()->LocalizeStr(TOR_tString, strKeyName.data(), strKeyName.size()), 0, KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS, &hKey);
	else
		res = RegOpenKeyExW(pData->pRootKey, strKeyName.data(), 0, KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS, &hKey);

	if( res != ERROR_SUCCESS )
		return nChildren = 0, false;

// this value from msdn:  ms-help://MS.MSDNQTR.2004APR.1033/sysinfo/base/enumerating_registry_subkeys.htm
#define MAX_VALUE_NAME_LENGTH 16383
	
	DWORD i, nSize, nValType, nCount = 0;
	CHAR pBuff[MAX_VALUE_NAME_LENGTH];

	for(i = 0; i < pData->nValues; i++)
	{
		nSize = MAX_VALUE_NAME_LENGTH;
		if( RegEnumValue(hKey, i, pBuff, &nSize, 0, &nValType, NULL, NULL) != ERROR_SUCCESS )
			continue;

		AddValue(pData, pBuff, nValType);
		nCount++;
	}

	for(i = 0; i < pData->nSubKeys; i++)
	{
		nSize = MAX_VALUE_NAME_LENGTH;
		if( RegEnumKeyEx(hKey, i, pBuff, &nSize, 0, 0, 0, 0) != ERROR_SUCCESS )
			continue;

		HKEY hSubKey = NULL;
		RegOpenKeyEx(hKey, pBuff, 0, KEY_QUERY_VALUE|KEY_ENUMERATE_SUB_KEYS, &hSubKey);
		AddKey(pData, hSubKey, pBuff);
		if( hSubKey )
			RegCloseKey(hSubKey);
		nCount++;
	}

	RegCloseKey(hKey);

	Item()->SortChildren(pItem);
	return nChildren = nCount, false;
}

/////////////////////////////////////////////////////////////////////////////
