// TrainingWizard.h
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#pragma warning ( disable: 4786 )

#include <plugin/p_as_trainsupport.h>
#include <basegui.h>
#include <binding.h>
#include <wingui/SimpleThread.h>

extern CItemBase * g_pTW;

typedef TrainFolderInfoList_t TrainSettings;

typedef TrainFolderInfo_t::FolderState eFolderState;


/////////////////////////////////////////////////////////////////////////////
// CFolderTree

class CMapiFolderTree : public TrainFolderInfo_t
{
public:
	CMapiFolderTree(){}
	CMapiFolderTree(FolderInfo_t &Folder)
	{
		*static_cast<FolderInfo_t *>(this) = Folder;
		m_FolderState = TrainFolderInfo_t::FolderUndefined;
	}

public:
	std::vector<CMapiFolderTree> m_aChildren;
};

/////////////////////////////////////////////////////////////////////////////
// CFolderTree

class CFolderTree : public CTreeItemSink
{
public:
	CFolderTree(hAS_TRAINSUPPORT pAS, CMapiFolderTree &Tree, eFolderState CheckedState) : m_pAS(pAS), m_Tree(Tree), m_CheckedState(CheckedState) {}

protected:
	struct TVIDATA : public cTreeItem
	{
		TVIDATA(CMapiFolderTree * pFolder)
		{
			m_flags = UPDATE_FLAG_TEXT|UPDATE_FLAG_SUBCOUNT;
			m_strName = pFolder->szName;
			m_pFolder = pFolder;
			m_nChildren = m_pFolder->bHasChildren ? 1 : 0;
		}

		CMapiFolderTree * m_pFolder;
	};

protected:
	virtual void      OnInited();
	virtual bool      OnItemExpanding(cTreeItem * pItem);
	virtual bool      OnItemChecking(cTreeItem * pItem);
	virtual void      OnItemGetTextColor(cTreeItem * pItem, DWORD &nColor);

	bool              ExpandItem(cTreeItem * hItem);
	CMapiFolderTree * GetFolder(cTreeItem * hItem);
	bool              CanBeChecked(CMapiFolderTree *pFolder);
public:
	void              ApplyState(cTreeItem * hItem, eFolderState State, bool bApplyToChildren = false, bool bApplyToSiblings = false);

protected:
	hAS_TRAINSUPPORT m_pAS;
	CMapiFolderTree& m_Tree;
	eFolderState     m_CheckedState;
};

/////////////////////////////////////////////////////////////////////////////
// CWzTrainig

class CWzTrainig : public CDialogBindingT<>, public CSimpleThread
{
public:
	typedef CDialogBindingT<> TBase;
	
	CWzTrainig(hAS_TRAINSUPPORT pAS);
	virtual ~CWzTrainig();

protected:
	SINK_MAP_BEGIN()
		SINK_DYNAMIC("FolderTree", CFolderTree(m_pAS, m_Tree, pItem->m_pParent->m_strItemId == "Page2" ? TrainFolderInfo_t::FolderHAM : TrainFolderInfo_t::FolderSPAM))
	SINK_MAP_END(TBase)

	virtual bool OnClicked(CItemBase* pItem);
	virtual bool OnNext();
	virtual bool OnCanClose(bool bUser);
	virtual bool OnOk();
	virtual void OnEvent(tDWORD nEventId, cSerializable* pData);

protected:
	void         StartTraining();
	void         OnTrainComplete();
	void         OnTrainStatus(TrainStatus_t *pStatus);
	virtual DWORD OnDoWork();

public:
	tERROR       OnTrainStatusEx(TrainStatus_t *pStatus);

protected:
	hAS_TRAINSUPPORT m_pAS;
	CMapiFolderTree  m_Tree;
	TrainSettings    m_aFolders;
	TrainStatus_t    m_Status;
	DWORD            m_nStartTime;
	volatile bool    m_bStopped;
	volatile bool    m_bCanceled;
	volatile HANDLE  m_hFinishEvent;
	tString          m_BtnFinishCaption;
};