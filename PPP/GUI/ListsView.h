// ListsView.h: interface for the Lists View classes.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKREPORT_H__6A4B16E2_4BFB_4AF5_94C4_978E8F567B2B__INCLUDED_)
#define AFX_TASKREPORT_H__6A4B16E2_4BFB_4AF5_94C4_978E8F567B2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "globals.h"

struct cReport;
struct cSerializable;
class CListItemDescr;

/////////////////////////////////////////////////////////////////////////////
// CWaitingDlg

class CWaitingDlg : public CDialogBindingViewT<>, public cSerializableObj, public cThreadPoolBase, public cThreadTaskBase
{
public:
	typedef CDialogBindingViewT<> TBase;

	// delays in TimerRefresh periods
	CWaitingDlg(cSerializable *pData, tUINT nShowDelay = 30, tUINT nWaitTimeout = 10000);
	~CWaitingDlg();

	tERROR       DoModal(CItemBase* pParent, LPCSTR strSection = NULL, DWORD nActionsMask = DLG_ACTION_CANCEL);
	void         StartWork();
	void         WaitEndWork();
	void         WorkSyncEvt(tDWORD nEventId, cSerializable * pData = NULL);
	bool         IsTerm();
	
protected:
	virtual void   OnStartWork() { StartWork(); }
	virtual tERROR OnDoWork()    { return errNOT_IMPLEMENTED; }
	virtual void   OnEndWork()   { Close(); }

protected:
	void OnCreate();
	void OnDestroy();
	void OnInit();
	void OnInited();
	bool OnCanClose(bool bUser);
	void OnTimerRefresh(tDWORD nTimerSpin);
	void OnEvent(tDWORD nEventId, cSerializable *pData);
	void do_work();
	
	void UpdateWaitStatus();
	void EndWork();
	
	tERROR          m_nErr;
	tUINT           m_nShowDelay;
	tUINT           m_nWaitTimeout;
	cSerializable * m_pData;
	volatile bool   m_bAbort;

private:
	CItemBase *     m_pLoopItem;
	CItemBase *     m_pThisRef;
	CRootItem *     m_pRoot;
};

//////////////////////////////////////////////////////////////////////
// CResultDialogT

template<class TBase, class T = cSerializable>
class CResultDialogT : public TBase
{
public:
	CResultDialogT(T * pStruct = NULL, tDWORD nActionsMask = DLG_ACTION_OKCANCEL) : TBase(pStruct, nActionsMask) {}

	bool OnClose(tDWORD& nResult)
	{
		if( nResult == DLG_ACTION_CANCEL || nResult == DLG_ACTION_NO )
			TBase::Item()->SetErrResult(errOPERATION_CANCELED);
		else
			TBase::Item()->SetErrResult(errOK_DECIDED);
		return true;
	}
};

//////////////////////////////////////////////////////////////////////////
// CListSinkMaster

class CListSinkMaster : public CListItemSink
{
public:
	CListSinkMaster() : m_pDescr(NULL) {}

protected:
	CListItem *      Item()                                                     { return (CListItem *)m_pItem; }
	
	virtual tDWORD   OnBindingFlags()                                           { return ITEMBIND_F_VALUE; }

	virtual void     SetDescriptor(CDescriptor *pDesc)                          { m_pDescr = pDesc; }
	virtual bool     OnListEvent(eListEvents nEvent);
	virtual void     OnChangedData(CItemBase * pItem);

protected:
	cSer *           GetItemData(cRowId &nItem);
	cSer *           GetSelItemData(cRowId &nItem);
	bool             GetSingleSelectedItem(cRowId &nItem);

protected:
	CDescriptor *    m_pDescr;
};

//////////////////////////////////////////////////////////////////////////
// CListViewSink

class CListViewSink : public CListSinkMaster
{
public:
	enum eOpCode { opAdd, opEdit, opDelete, opMove, };

	CListViewSink();

protected:
	virtual tTYPE_ID OnGetType()                                                { return m_nSerID; }
	virtual tTYPE_ID OnGetFixedType()                                           { return tTYPE_ID(cSer::eIID); }
	virtual bool     OnSetType(tTYPE_ID nType);
	
	virtual cSer *   GetEditRowData()                                           { return m_pEditItem ? m_pEditItem : GetSelItemData(TOR(cRowId, ())); }
	virtual bool     IsEditNewItem()                                            { return m_pEditItem && !m_nEditItem.m_node; }

	virtual bool     OnCanOperate(eOpCode nOp)                                  { return true; }
	virtual bool     OnCanOperateItem(cRowId &nItem, eOpCode nOp)               { return true; }
	virtual bool     OnOperateItem(cRowId &nItem, eOpCode nOp)                  { return false; }
	virtual bool     OnPostOperateItem(cRowId &nItem, eOpCode nOp, tPTR pValue) { return false; }
	virtual tERROR   OnEditItem(CItemBase * pOpCtl)                             { return errNOT_IMPLEMENTED; }

protected:
	virtual void     OnInit();
	virtual void     OnDestroy();
	virtual void     OnEnable(bool bOn);
	virtual bool     OnClicked(CItemBase * pItem);
	virtual bool     OnDblClicked(CItemBase * pItem);
	virtual void     OnEvent(tDWORD nEventId, cSerializable *pData);
	virtual bool     OnListEvent(eListEvents nEvent);

protected:
	virtual size_t   GetCount()                             { return 0; }
	virtual bool     InsertItem(cRowId &nItem, cSer *pItem) { return false; }
	virtual void     DeleteItem(cRowId &nItem)              {}

protected:
	void             AddItem();
	void             EditSelectedItem(CItemBase * pOpCtl);
	tERROR           EditItem(CItemBase * pOpCtl, cSer *pItem, const cRowId &nItem = TOR(cRowId, ()));
	tERROR           CheckItemExist(cSer *pItem, bool bUpdateExisting, bool bFocusExisting);
	
	void             UpdateCtrls();
	bool             SwapItems(cRowId &nFirst, cRowId &nSecond);
	void             MoveSelectedItem(bool bUp);
	void             DeleteSelectedItems();

protected:
	class BtnSink : public CItemSink
	{
	public:
		BtnSink(CListViewSink * pList) : m_pList(pList) {}

		bool  OnClicked(CItemBase * pItem)                           { return m_pList->OnClicked(pItem); }
		void  OnInitBinding(CItemBase* pItem, CSinkCreateCtx * pCtx) { m_pList->OnInitBinding(pItem, pCtx); }
		void  OnChangedData(CItemBase * pItem)                       { m_pList->OnChangedData(m_pList->Item()); }
	
	protected:
		CListViewSink * m_pList;
	}; friend class BtnSink;

protected:
	tTYPE_ID         m_nSerID;

	tQWORD           m_DataTimestamp;
	cSer *           m_pEditItem;
	cRowId           m_nEditItem;
	
	CGuiPtr<cNode>         m_pUniqueChecker;
	CGuiPtr<cNode>         m_pUniqueCopier;
	CGuiPtr<CFieldsBinder> m_pUniqueBinder;
	
	CItemBase *      m_pBtnAdd;
	CItemBase *      m_pBtnEdit;
	CItemBase *      m_pBtnDelete;
	CItemBase *      m_pBtnMoveUp;
	CItemBase *      m_pBtnMoveDown;
	CItemBase *      m_pBtnExport;
	CItemBase *      m_pBtnImport;
	CItemBase::Items m_aBtns;
};

//////////////////////////////////////////////////////////////////////
// CSerTreeView

class CSerTreeView : public CListViewSink
{
public:
	CSerTreeView();

protected:
	virtual bool     OnSetType(tTYPE_ID nType);
	virtual bool     OnSetValue(const cVariant &pValue);
	virtual bool     GetTreeInfo(tIdxArr &pIdx, tDWORD *pNodes, tIdxArr &pRows);
	virtual cSer *   GetTreeData(tIdxArr &pIdx, bool bIsNode)                { return GetTreeDataInt(pIdx, bIsNode); }
	virtual bool     GetRowInfo(cSer *pNode, tIdxArr &pIdx)                  { return true; }

protected:
	virtual bool     InsertItem(cRowId &nItem, cSer *pItem);
	virtual void     DeleteItem(cRowId &nItem);

protected:
	bool             GetChildren(cSer *pSer, cVectorHnd &aChildren)          { return cSerTreeFind(Root(), m_pChildVecAcc, NULL).GetChildren(pSer, aChildren); }
	cSer *           GetTreeDataInt(tIdxArr &pIdx, bool bIsNode);
	cSer *           GetTreeDataInt(cSer *pSer, const tIdxArr &aIdxs, size_t nPos = 0);

protected:
	cSer *           m_pRoot;
	CGuiPtr<cNode>   m_pChildVecAcc;
	CGuiPtr<cNode>   m_pRootFinder;
};

//////////////////////////////////////////////////////////////////////
// CVectorView

class CVectorView : public CListViewSink, protected cVectorHnd, protected CDataValidator
{
public:
	CVectorView();

public:
	virtual cSer *   GetRowData(tIdx nItem)          { return m_v ? ser_ptr(at_safe(nItem)) : 0; }
	virtual bool     GetInfo(cDataInfo& info)        { info.m_nCount = GetCount(); info.m_nTimeStamp = m_DataTimestamp; return true; }
	virtual size_t   GetCount()                      { return m_v ? size() : 0; }
	virtual bool     InsertItem(cRowId &nItem, cSer *pItem);
	virtual void     DeleteItem(cRowId &nItem);

protected:
	virtual bool     OnSetType(tTYPE_ID nType);
	virtual bool     OnSetValue(const cVariant &pValue);
	virtual bool     OnIsVector()                    { return true; }

protected:
	virtual void     OnDestroy();
	virtual bool     Validate(CItemBase *pItem, cSerializable *pData);

protected:
	friend class CListItemDescr;
};

//////////////////////////////////////////////////////////////////////
// CVectorViewT

template <class _Struct>
class CVectorViewT : public CVectorView
{
	virtual tTYPE_ID OnGetFixedType() { return tTYPE_ID(_Struct::eIID); }
	
	virtual void OnInitProps(CItemProps& pProps)
	{
		CVectorView::OnInitProps(pProps);
		OnSetType(OnGetFixedType());
	}
};

//////////////////////////////////////////////////////////////////////
// CListItemDescr

class CListItemDescr : public CDescriptor
{
public:
	CListItemDescr() : m_pDescOwner(NULL) {}
	~CListItemDescr() { }

	virtual bool SetItemData(cSer *pData);
	virtual bool GetItemData(cSer *pData);

protected:
	void OnInitProps(CItemProps& pProps);
	void OnChangedData(CItemBase * pItem);

	CDescriptorDataOwner * m_pDescOwner;
	cSerializableObj       m_pData;
};

//////////////////////////////////////////////////////////////////////
// CListImportHelper

class CListImpExpHelper
{
public:
	CListImpExpHelper(CListItem * pList) : m_pList(pList) {}
	
	bool Export(LPCSTR strFilesTypes);
	bool Import(LPCSTR strFilesTypes);
	
	virtual tERROR OnExportPrepare(tObjPath& strFileName) { return true; }
	virtual tERROR OnExportItem(int nPos) { return true; }
	virtual tERROR OnImport(tObjPath& strFileName, bool bAppend) { return true; }

protected:
	CListItem * m_pList;
};

class CStrListImpExpHelper : public CListImpExpHelper
{
public:
	CStrListImpExpHelper(CListItem * pList) : CListImpExpHelper(pList), m_nPos(0) {}

	bool   Export() { return CListImpExpHelper::Export("txt"); }
	bool   Import() { return CListImpExpHelper::Import("txt"); }

	bool   Export(cStringObj &sFile);
	bool   Import(cStringObj &sFile);

protected:
	tERROR OnExportPrepare(tObjPath& strFileName);
	tERROR OnExportItem(int nPos);
	tERROR OnImport(tObjPath& strFileName, bool bAppend);

	virtual int          OnGetSize()                  { return 0; }
	virtual cStringObj * OnGetString(int idx)         { return NULL; }
	virtual void         OnClearVector()              {}
	virtual void         OnPutString(const cStringObj &str) {}

protected:
	cAutoObj<cIO> m_hIo;
	int           m_nPos;
};

class CStrListImpExp : public CStrListImpExpHelper
{
public:
	CStrListImpExp(CListItem * pList, cStringObjVector &Strings) : CStrListImpExpHelper(pList), m_Strings(Strings) {}
	
	int          OnGetSize()                  { return m_Strings.size(); }
	cStringObj * OnGetString(int idx)         { return &m_Strings[idx]; }
	void         OnPutString(const cStringObj &str) { m_Strings.push_back(str); }
	void         OnClearVector()              { m_Strings.clear(); }

protected:
	cStringObjVector &m_Strings;
};

//////////////////////////////////////////////////////////////////////////
// CReportView

class CReportView : public CVectorView, tMemChunk
{
public:
	CReportView(tTYPE_ID type, LPCSTR profileName);

private:
	virtual void OnInitProps(CItemProps& pProps);
	virtual void OnEvent(tDWORD nEventId, cSerializable * pData);
	virtual void OnTimerRefresh(tDWORD nTimerSpin);

	static tERROR MsgReceive(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD *blen);

	cStringObj             m_profileName;
	tSIZE_T                m_lastEventsCount;
	cAutoObj<cMsgReceiver> m_msgReceiver;
};

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_TASKREPORT_H__6A4B16E2_4BFB_4AF5_94C4_978E8F567B2B__INCLUDED_)
