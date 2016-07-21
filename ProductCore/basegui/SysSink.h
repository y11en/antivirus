// SysSink.h: interface for the system depended sinks.
//

#if !defined(AFX_SYSSINK_H__BC57CD5C_0F2D_4921_AE3B_CCCBA11BB4A8__INCLUDED_)
#define AFX_SYSSINK_H__BC57CD5C_0F2D_4921_AE3B_CCCBA11BB4A8__INCLUDED_

//////////////////////////////////////////////////////////////////////
// CSkinDebugWindow

class CSkinDebugWindow : public CDialogBindingViewT<>
{
public:
	typedef CDialogBindingViewT<> TBase;

	CSkinDebugWindow() : TBase(&m_info)
	{
		m_strSection = "_SkinDebugWindow";
	}

	~CSkinDebugWindow()
	{
	}

	void OnCreate()
	{
		TBase::OnCreate();
		Root()->m_pDebugWnd = this;
	}

	void OnDestroy()
	{
		TBase::OnDestroy();
		Root()->m_pDebugWnd = NULL;
	}

	void OnInit();

	cGuiParams	m_info;

public:
	void	Show(CRootItem * pRoot);
	void	ProcessPoint(POINT& pt, CItemBase * pParent);
};

//////////////////////////////////////////////////////////////////////
// CNavigatorSink

class CNavigatorSink : public CItemSinkT<>
{
public:
	CNavigatorSink() : m_pButtons(NULL), m_pWorkArea(NULL) {}

protected:
	void OnInit();
	void OnInitProps(CItemProps& pProps);
	void OnChangedState(CItemBase * pItem, tDWORD nStateMask);
	bool OnSelect(CItemBase * pItem);

protected:
	CItemBase * m_pButtons;
	CItemBase * m_pWorkArea;
	unsigned	m_bLoad : 1;
};

//////////////////////////////////////////////////////////////////////
// CMsgBox

class CMsgBox : public CDialogBindingViewT<>
{
public:
	typedef CDialogBindingViewT<> TBase;
	
	CMsgBox(CItemBase * pItem, LPCSTR strTextId, LPCSTR strCaptionId = NULL,
		tDWORD nIcon = 0, tDWORD nActionsMask = 0, cSerializable * pStruct = NULL);

	void OnCreate();

	static int Show(CRootItem * pRoot, CItemBase * pItem, LPCSTR strTextId, LPCSTR strCaptionId, tUINT uType, cSerializable * pData, CFieldsBinder * pBinder);
	
	CItemBase *     m_pItem;
	LPCSTR          m_strTextId;
	LPCSTR          m_strCaptionId;
	cGuiMsgBox      m_sMsgBox;
};

//////////////////////////////////////////////////////////////////////
// CBrowseShellObject

class CBrowseShellObject : public CDialogBindingT<>
{
public:
	typedef CDialogBindingT<> TBase;
	
	CBrowseShellObject(CBrowseObjectInfo& pBrowseInfo);

protected:
	void OnCreate();
	void OnInit();
	void OnInited();
	bool OnSelect(CItemBase * pItem);
	void OnChangedData(CItemBase * pItem);
	bool OnOk();
	bool OnClicked(CItemBase * pItem);
	bool OnClose(tDWORD& nResult);
	bool OnMenu(CPopupMenuItem * pMenu, CItemBase * pItem);

protected:
	bool   IsValidObject();
	tERROR CreateNewSubFolder(CObjectInfo& ObjInfo);
	void   UpdateSelectionState();
	bool   UpdateBrowseObjectInfo();
	bool   CanAddObject();

protected:
	CBrowseObjectInfo& m_pObject;
	CTreeItemSink *    m_pTreeSink;
	CItemBase *        m_pEdit;
	CItemBase *        m_pAddBtn;
	unsigned           m_bEdit : 1;
	unsigned           m_bBlockSelect : 1;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_SYSSINK_H__BC57CD5C_0F2D_4921_AE3B_CCCBA11BB4A8__INCLUDED_)
