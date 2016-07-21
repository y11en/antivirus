// SettingsDlg.h: interface for the CSettingsDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SETTINGSDLG_H__D92A7D2C_B8E3_41A9_A21C_EDDD3A09D9C6__INCLUDED_)
#define AFX_SETTINGSDLG_H__D92A7D2C_B8E3_41A9_A21C_EDDD3A09D9C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "OESettings.h"
#include "OETricks.h"

#include <ProductCore/binding.h>
#include <Mail/structs/s_mcou_antispam.h>

/////////////////////////////////////////////////////////////////////////////
// CPageAntispamSink

class CPageAntispamSink : public CDialogBindingT<AntispamPluginSettings_t>
{
public:
	typedef CDialogBindingT<AntispamPluginSettings_t> TBaseDlg;
	CPageAntispamSink(HWND hWndParent, HIMAGELIST hTreeImageList, const COETricks::FOLDERS_LIST& arrFolders) :
		TBaseDlg(&m_Settings),
		m_hWndParent(hWndParent),
		m_hTreeImageList(hTreeImageList), 
		m_arrLocalFolders(arrFolders),
		m_bShouldSaveAnyway(false)
		{}
	
	void SetSettings(IN const COESettings& settings);
	void GetSettings(OUT COESettings& settings) const;

	bool ShouldSaveAnyway() const
	{
		return m_bShouldSaveAnyway;
	}

protected:
	virtual void OnInit();
	virtual bool OnOk();
	virtual bool OnCanClose(bool bUser);
	virtual bool OnClicked(CItemBase* pItem);
	virtual void OnTimerRefresh(tDWORD nTimerSpin);
	
	virtual HWND OnGetParent();
	bool         ValidateRule(AntispamPluginRule_t &Rule);
	void         UpdateStatus();
	void         _FolderId_O2N(__int64 nId, FolderInfo_t& pFolder) const;
	__int64      _FolderId_N2O(const FolderInfo_t& pFolder) const;
	
	AntispamPluginSettings_t  m_Settings;
	cAntispamPluginStatus     m_Status;

private:
	HWND                      m_hWndParent;
	HIMAGELIST                m_hTreeImageList;
	COETricks::FOLDERS_LIST   m_arrLocalFolders;
	bool m_bShouldSaveAnyway;
};

/////////////////////////////////////////////////////////////////////////////
// CFolderTree

class CFolderTree : public CTreeItemSink
{
public:
	CFolderTree(const COETricks::FOLDERS_LIST& aFolders, HIMAGELIST hTreeImageList) : m_aLocalFolders(aFolders), m_hTreeImageList(hTreeImageList) {}

protected:
	struct TVIDATA : public cTreeItem
	{
		TVIDATA(cStrObj& strName, tUINT nFolder, tUINT nChildren, tUINT nImage = 0)
		{
			m_flags = UPDATE_FLAG_TEXT|UPDATE_FLAG_SUBCOUNT;
			m_strName = strName;
			m_nFolder = nFolder;
			m_nChildren = nChildren ? 1 : 0;
			m_nImage = m_nSelectedImage = nImage;
		}
		tUINT m_nFolder;
	};

public:
	bool GetFolder(COETricks::TFolder& FolderInfo);

protected:
	bool _FolderExists(__int64 dwFolderID);
	bool _FolderHasChildren(__int64 dwFolderID);

	void GetFolderInfoList(IN COETricks::TFolder* pFolderInfo, OUT COETricks::FOLDERS_LIST* pFolderInfoList);

	void OnInited();
	bool OnItemExpanding(cTreeItem * pItem, tUINT& nChildren);

	bool ExpandItem(cTreeItem * pItem);

	const COETricks::FOLDERS_LIST& m_aLocalFolders;
	HIMAGELIST m_hTreeImageList;
	COETricks::FOLDERS_LIST m_aFolders;
};

/////////////////////////////////////////////////////////////////////////////
// CBrowseForFolder

class CBrowseForFolder : public CDialogBindingT<>
{
public:
	typedef CDialogSink TBase;
	CBrowseForFolder(const COETricks::FOLDERS_LIST& arrFolders, FolderInfo_t *pFldInfo, HIMAGELIST hTreeImageList)
		: m_Tree(arrFolders, hTreeImageList), m_pFldInfo(pFldInfo) {}

	SINK_MAP_BEGIN()
		SINK_STATIC("FolderTree", m_Tree)
	SINK_MAP_END(CDialogSink)

protected:
	virtual bool OnOk();
	
	FolderInfo_t *            m_pFldInfo;
	CFolderTree               m_Tree;
};

#endif // !defined(AFX_SETTINGSDLG_H__D92A7D2C_B8E3_41A9_A21C_EDDD3A09D9C6__INCLUDED_)
