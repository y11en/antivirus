//////////////////////////////////////////////////////////////////////
// SysSink.cpp

#include "StdAfx.h"
#include "SysSink.h"

/////////////////////////////////////////////////////////////////////////////
// CShellTreeSink

CShellTreeSink::CShellTreeSink()
{
	m_nMask = SHELL_OBJTYPE_CUSTOM|SHELL_OBJTYPE_FOLDER|SHELL_OBJTYPE_FILE;
	
	m_dirs.setReadOnly(true);
	m_dirs.setSorting(QDir::Name|QDir::DirsFirst);
}

CShellTreeSink::~CShellTreeSink()
{
}

void CShellTreeSink::OnInited()
{
	tDWORD _filter = QDir::NoFilter;
	if( !(m_nMask & (SHELL_OBJTYPE_FILE|SHELL_OBJTYPE_MASK)) )
		_filter &= ~QDir::Files;
	
	m_dirs.setFilter((QDir::Filter)_filter);

	Item()->SetImageList((hImageList)&m_images);

	m_pRoot = Item()->AppendItem(NULL, new TVITEMDATA(NULL));
	Item()->ExpandItem(m_pRoot);

	if( m_pInfo )
		SetSelObject(*m_pInfo);

	if( !Item()->GetSelectionItem() )
		Item()->SelectItem(m_pRoot);
}

bool CShellTreeSink::SetSelObject(CObjectInfo& pObject, CSuccessSel * pSuccessPath)
{
	if( pObject.m_nCustomId || pObject.m_strName.empty() )
		return false;

	if( pObject.m_strName.compare(0, 2, "\\\\") == cSTRING_COMP_EQ )
		return true;

	return true;
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
	pObject.m_nCustomId = 0;
	
	if( pItem->nCustomId )
	{
		pObject.m_nCustomId = pItem->nCustomId;
		pObject.m_nType = SHELL_OBJTYPE_CUSTOM;
	}
	else
	{
		QFileInfo _fileInfo(m_dirs.data(pItem->m_idx, QDirModel::FilePathRole).toString());
		
		if( _fileInfo.isRoot() )
			pObject.m_nType = SHELL_OBJTYPE_DRIVE;
		else if( _fileInfo.isFile() )
			pObject.m_nType = SHELL_OBJTYPE_FILE;
		else if( _fileInfo.isDir() )
			pObject.m_nType = SHELL_OBJTYPE_FOLDER;
		
		((CUniRoot *)Root())->_2PS(_fileInfo.filePath(), pObject.m_strName);
		((CUniRoot *)Root())->_2PS(_fileInfo.fileName(), pObject.m_strShortName);
	}
	
	return true;
}

void CShellTreeSink::OnUpdateItemProps(cTreeItem * pItem, tDWORD flags)
{
	TVITEMDATA * pData = (TVITEMDATA *)pItem;
	
	if( pData->nCustomId )
		return;

	if( flags & (UPDATE_FLAG_ICON|UPDATE_FLAG_ICONSELECTED) )
	{
		const QVariant& var = m_dirs.data(pData->m_idx, QDirModel::FileIconRole);
		QIcon * qIcon = (QIcon *)var.data();
		
		if( var.type() == QVariant::Icon && qIcon )
		{
			pData->m_nImage = _AddImage(qIcon);
			pData->m_nSelectedImage = _AddImage(qIcon, true);
		}
	}

	if( flags & UPDATE_FLAG_TEXT )
	{
		const QString& qText = m_dirs.data(pData->m_idx, QDirModel::FileNameRole).toString();
		((CUniRoot *)Root())->_2PS(qText, pItem->m_strName);
	}

	if( flags & UPDATE_FLAG_SUBCOUNT )
	{
		pData->m_nChildren = m_dirs.hasChildren(pData->m_idx) ? 1 : 0;
	}
}

bool CShellTreeSink::OnItemExpanding(cTreeItem * pItem, tUINT& nChildren)
{
	TVITEMDATA * pData = (TVITEMDATA *)pItem;

	bool bIsRoot = !pData->pParent;

	if( bIsRoot )
	{
		CRootSink * pSink = (CRootSink *)Root()->m_pSink;
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
				pChild->m_nImage = pObject.m_pIcon ? _AddImage((QIcon *)pObject.m_pIcon->IconHnd()) : -1;
				pChild->m_nSelectedImage = pChild->m_nImage;
				pChild->m_nChildren = 0;
				Item()->AppendItem(pItem, pChild);
			}
		}
	}

	int i, n;
	for(i = 0, n = m_dirs.rowCount(pData->m_idx); i < n; i++)
	{
		TVITEMDATA * pChild = new TVITEMDATA(pData, m_dirs.index(i, 0, pData->m_idx));

		if( (m_nMask & SHELL_OBJTYPE_FILTERED) && m_pInfo->m_pEnum )
		{
			OnUpdateItemProps(pChild, UPDATE_FLAG_TEXT|UPDATE_FLAG_ICON|UPDATE_FLAG_ICONSELECTED|UPDATE_FLAG_SUBCOUNT);

			CObjectInfo _Obj;
			GetObjectInfo(_Obj, pChild);
			
			_Obj.m_nQueryMask = 0;
			Root()->GetObjectInfo(&_Obj);
			
			if( !m_pInfo->m_pEnum->FilterObject(_Obj) )
			{
				delete pChild;
				continue;
			}
		}

		Item()->AppendItem(pItem, pChild);
	}
	
	return nChildren = n, false;
}

tUINT CShellTreeSink::_AddImage(QIcon * qIcon, bool bSelected)
{
	if( !qIcon )
		return -1;

	QSize szActual = qIcon->actualSize(QSize(1000, 1000));
	return m_images.AddIcon(qIcon, szActual, QIcon::Normal, bSelected ? QIcon::On : QIcon::Off);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
