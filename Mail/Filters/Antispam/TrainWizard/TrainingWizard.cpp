// TrainingWizard.cpp : implementation of the TrainingWizard
//
/////////////////////////////////////////////////////////////////////////////

#include "TrainingWizard.h"

CItemBase * g_pTW = NULL;

#define TREE_GRAY_COLOR                (RGB(128, 128, 128))

void CopyToPlainVector(TrainFolderInfoList_t &aFolders, CMapiFolderTree &FolderTree)
{
	if(FolderTree.m_FolderState != TrainFolderInfo_t::FolderUndefined)
		aFolders.push_back(*(TrainFolderInfo_t *)&FolderTree);

	for(int i = 0; i < FolderTree.m_aChildren.size(); i++)
		CopyToPlainVector(aFolders, FolderTree.m_aChildren[i]);
}

/////////////////////////////////////////////////////////////////////////////
// CFolderTree

void CFolderTree::OnInited()
{
	ExpandItem(NULL);
}

bool CFolderTree::OnItemExpanding(cTreeItem * pItem)
{
	return ExpandItem(pItem);
}

bool CFolderTree::OnItemChecking(cTreeItem * pItem)
{
	CMapiFolderTree * pFolder = CFolderTree::GetFolder(pItem);
	if(pFolder && CanBeChecked(pFolder))
	{
		pFolder->m_FolderState = pFolder->m_FolderState == TrainFolderInfo_t::FolderUndefined ? m_CheckedState : TrainFolderInfo_t::FolderUndefined;
		return true;
	}
	return false;
}

void CFolderTree::OnItemGetTextColor(cTreeItem * pItem, DWORD &nColor)
{
	CMapiFolderTree * pFolder = CFolderTree::GetFolder(pItem);
	if(pFolder && !CanBeChecked(pFolder))
		nColor = TREE_GRAY_COLOR;
}

bool CFolderTree::ExpandItem(cTreeItem * hItem)
{
	CMapiFolderTree * pFolder = &m_Tree;
	if( hItem )
	{
		if( Item()->GetChildItem(hItem) )
			return true;
		pFolder = GetFolder(hItem);
	}

	std::vector<CMapiFolderTree>& aChildren = pFolder->m_aChildren;

	if( pFolder->bHasChildren && !aChildren.size() )
	{
		FolderInfoList_t m_aFolders;
		if( m_pAS->GetFolderInfoList(pFolder == &m_Tree ? NULL : pFolder, &m_aFolders) == S_OK )
			for(int i = 0; i < m_aFolders.size(); i++)
				aChildren.push_back(m_aFolders[i]);
	}
	
	for(int i = 0; i < aChildren.size(); i++)
	{
		pFolder = &aChildren[i];
		Item()->AppendItem(hItem, new TVIDATA(pFolder));
	}
	
	return true;
}

void CFolderTree::ApplyState(cTreeItem * hItem, eFolderState State, bool bApplyToChildren, bool bApplyToSiblings)
{
	if( !hItem )
		return;
	
	CMapiFolderTree *pFolder = GetFolder(hItem);
	if( CanBeChecked(pFolder) && pFolder->m_FolderState != State )
		Item()->SetItemCheckState(hItem, (pFolder->m_FolderState = State) != TrainFolderInfo_t::FolderUndefined);
	
	if( bApplyToChildren )
		ApplyState(Item()->GetChildItem(hItem), State, bApplyToChildren, true);
	
	if( bApplyToSiblings )
		ApplyState(Item()->GetNextItem(hItem), State, bApplyToChildren, bApplyToSiblings);
}

CMapiFolderTree *CFolderTree::GetFolder(cTreeItem * hItem)
{
	return hItem ? ((TVIDATA *)hItem)->m_pFolder : NULL;
}

bool CFolderTree::CanBeChecked(CMapiFolderTree *pFolder)
{
	return pFolder->bCanHaveMessages && (pFolder->m_FolderState == TrainFolderInfo_t::FolderUndefined || pFolder->m_FolderState == m_CheckedState);
}


/////////////////////////////////////////////////////////////////////////////
// CWzTrainig

CWzTrainig::CWzTrainig(hAS_TRAINSUPPORT pAS) : TBase(&m_Status), m_pAS(pAS), m_bStopped(false), m_bCanceled(false), m_hFinishEvent(NULL)
{
	m_nActionsMask = DLG_ACTION_WIZARD;
	m_hFinishEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
};

CWzTrainig::~CWzTrainig()
{
	m_bCanceled = true;
	SetEvent(m_hFinishEvent);
	CSimpleThread::WaitForStop();
	if ( m_hFinishEvent )
	{
		CloseHandle(m_hFinishEvent);
		m_hFinishEvent = NULL;
	}
	g_pTW = NULL;
}

bool CWzTrainig::OnClicked(CItemBase* pItem)
{
	if( pItem->m_strItemId == "Clear" )
	{
		CFolderTree *pTree = (CFolderTree *)pItem->m_pParent->GetSink("FolderTree", GUI_ITEMT_TREE);
		if( pTree )
			pTree->ApplyState(pTree->Item()->GetChildItem(NULL), TrainFolderInfo_t::FolderUndefined, true, true);
	}
	
	return TBase::OnClicked(pItem);
}

bool CWzTrainig::OnNext()
{
	if(Item()->GetCurPage()->m_strItemId == "Page3")
	{
		StartTraining();
	}
	return true;
}

inline int MsgBox(CItemBase * pItem, LPCTSTR strTextId, LPCTSTR strCaptId = NULL, UINT uType = MB_OK|MB_ICONINFORMATION, cSerializable * pBind = NULL)
{
	if( !pItem )
		return 0;

	return pItem->m_pRoot->ShowMsgBox(pItem, strTextId,
		strCaptId ? strCaptId : "ProductName", uType, pBind);
}

bool CWzTrainig::OnCanClose(bool bUser)
{
	if ( !m_BtnFinishCaption.empty() )
	{
		if ( IDYES == MsgBox(*this, "ConfirmStop", NULL, MB_YESNO|MB_ICONQUESTION, NULL) )
			m_bStopped = true;
		return false;
	}
	m_bCanceled = true;
	
	if(m_hFinishEvent)
		SetEvent(m_hFinishEvent);
	return true;
}

bool CWzTrainig::OnOk()
{
	if(m_hFinishEvent)
		SetEvent(m_hFinishEvent);
	return true;
}

void CWzTrainig::StartTraining()
{
	if(CSimpleThread::IsWorking())
		return;

	m_aFolders.clear();
	CopyToPlainVector(m_aFolders, m_Tree);

	m_nStartTime = GetTickCount();
	m_bCanceled = false;

	CItemBase *pItem = GetItem("Cancel");
	if ( pItem )
	{
		m_BtnFinishCaption = pItem->GetText();
		pItem->SetText(Item()->LoadString("BtnStop").c_str());
	}

	//TODO: «адать пониженный приоритет этому потоку
	if ( 0 == CSimpleThread::Run() )
	{
		CSimpleThread::SetPriority(THREAD_PRIORITY_LOWEST);
		Item()->GetOwner()->Enable("Back", false);
		Item()->GetOwner()->Enable("Next", false);
	}
}

DWORD CWzTrainig::OnDoWork()
{
	return m_pAS->Train(NULL/*!!!!!!!!!!!!!!!!!!!!!!&m_aFolders*/);
}

void CWzTrainig::OnTrainComplete()
{
	Item()->GetOwner()->Enable("Back", false);
	Item()->GetOwner()->Enable("Next", true);
	CItemBase *pItem = GetItem("Cancel");
	if ( pItem )
	{
		pItem->SetText(m_BtnFinishCaption.c_str());
		m_BtnFinishCaption = "";
	}

	TrainStatus_t &Status = *(TrainStatus_t *)m_ser;
	Status.ulPercentCompleted = 100;
	Status.ulTimeToFinish = 0;
	Status.bTrainComplete = cTRUE;

	UpdateData(true);
}

void CWzTrainig::OnTrainStatus(TrainStatus_t *pStatus)
{
	DWORD nCurTime = GetTickCount();

	pStatus->ulPercentCompleted = pStatus->ulMessagesCount ? pStatus->ulMessagesProcessed * 100 / pStatus->ulMessagesCount : pStatus->ulPercentCompleted;
	pStatus->ulTimeToFinish = pStatus->ulMessagesProcessed ? (nCurTime - m_nStartTime) * (pStatus->ulMessagesCount - pStatus->ulMessagesProcessed) / pStatus->ulMessagesProcessed / 1000 : pStatus->ulTimeToFinish;
	pStatus->bTrainComplete = cFALSE;

	Item()->GetOwner()->Enable("Back", false);
	Item()->GetOwner()->Enable("Next", false);

	m_ser->assign(*pStatus, false);
	UpdateData(true);
}

void CWzTrainig::OnEvent(tDWORD nEventId, cSerializable* pData)
{
	TBase::OnEvent(nEventId, pData);

	if(nEventId == TrainStatus_t::eIID)
	{
		if(pData)
			OnTrainStatus((TrainStatus_t *)pData);
		else
			OnTrainComplete();
	}
}


tERROR CWzTrainig::OnTrainStatusEx(TrainStatus_t * pStatus)
{
	if( (m_bCanceled || m_bStopped) && pStatus )
		return errOK_DECIDED;

	if( Item() )
		Item()->m_pRoot->SendEvent(TrainStatus_t::eIID, pStatus, 0, Item());

	if( !pStatus )
	{
		// waiting for Ok or Cancel
		if( m_hFinishEvent )
			WaitForSingleObject(m_hFinishEvent, INFINITE);
		return m_bCanceled ? errOK_DECIDED : errOK;
	}

	return errOK;
}
