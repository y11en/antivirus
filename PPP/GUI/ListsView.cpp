// ListsView.cpp: implementation of the Lists View classes.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ListsView.h"
//#include "SettingsDlgs.h"

#include <iface/e_ktrace.h>
#include <plugin/p_report.h>

// RM use of <malloc.h> is wrong. malloc function is defined in stdlib.h
//#include <malloc.h>
#include <stdlib.h>

#define ITEMALIAS_LISTBUTTON "listbutton"

/////////////////////////////////////////////////////////////////////////////
// CWaitingDlg

#define EVENTID_STARTWORK        0x7263bd71
#define EVENTID_WAITENDWORK      0x05307fb3
#define EVENTID_UPDATEWAITSTATUS 0xc22bc27c

CWaitingDlg::CWaitingDlg(cSerializable * pData, tUINT nShowDelay, tUINT nWaitTimeout) :
	TBase(NULL),
	cThreadPoolBase("WaitingDlg"),
	m_nShowDelay(nShowDelay),
	m_nWaitTimeout(nWaitTimeout),
	m_bAbort(false),
	m_pLoopItem(NULL),
	m_pRoot(NULL)
{
	m_pData = pData;
}	

CWaitingDlg::~CWaitingDlg()
{
	if( m_pThisRef )
		m_pThisRef->Release(), m_pThisRef = NULL;
	if( m_pRoot )
		m_pRoot->Release(), m_pRoot = NULL;
}

tERROR CWaitingDlg::DoModal(CItemBase* pParent, LPCSTR strSection, DWORD nActionsMask)
{
	m_nErr = errNOT_INITIALIZED;
	TBase::DoModal(pParent, strSection, nActionsMask);
	return m_nErr;
}

void CWaitingDlg::StartWork()
{
	if( !Item() )
		return;
	
	m_pLoopItem->m_pRoot = m_pRoot;
	cThreadTaskBase::start(*this);
}

void CWaitingDlg::WaitEndWork()
{
	if( !Item() )
		return;

	CItemBase * pItem = m_pLoopItem;
	
	pItem->AddRef();
	Root()->MessageLoop(pItem);
	pItem->Release();
	
	cThreadTaskBase::wait_for_stop();
}

void CWaitingDlg::OnCreate()
{
	cSerializableObj::assign(m_pData);
	m_pStruct = cSerializableObj::m_ser;
	
	TBase::OnCreate();
	cThreadPoolBase::init(g_hGui->Obj());
}

void CWaitingDlg::OnDestroy()
{
	m_pLoopItem->Destroy(), m_pLoopItem = NULL;
	
	cThreadPoolBase::de_init();
	m_pTP = NULL;
	TBase::OnDestroy();

	m_bAbort = true;
}

void CWaitingDlg::OnInit()
{
	TBase::OnInit();
	m_pLoopItem = Root()->CreateItem(NULL, NULL);

	m_pRoot = Root();
	m_pRoot->AddRef();

	m_pThisRef = *this;
	m_pThisRef->AddRef();
}

void CWaitingDlg::OnInited()
{
	TBase::OnInited();
	Root()->SendEvent(EVENTID_STARTWORK, NULL, 0, *this);
}

bool CWaitingDlg::OnCanClose(bool bUser)
{
	Item()->EnableAction(DLG_ACTION_CANCEL, false);
	
	m_bAbort = true;
	return false;
}

void CWaitingDlg::OnTimerRefresh(tDWORD nTimerSpin)
{
	if( m_nShowDelay )
	{
		if( !(--m_nShowDelay) )
			Item()->Show(true);
	}
	else
		UpdateData(true);
}

void CWaitingDlg::OnEvent(tDWORD nEventId, cSerializable * pData)
{
	switch(nEventId)
	{
	case EVENTID_STARTWORK:        OnStartWork(); break;
	case EVENTID_WAITENDWORK:      OnEndWork();	break;
	case EVENTID_UPDATEWAITSTATUS: cSerializableObj::assign(m_pData); break;
	}
}

void CWaitingDlg::do_work()
{
	m_nErr = OnDoWork();
	
	m_pLoopItem->m_pRoot = NULL;
	EndWork();
}

void CWaitingDlg::UpdateWaitStatus()
{
	if( m_pRoot )
		m_pRoot->SendEvent(EVENTID_UPDATEWAITSTATUS, NULL, 0, m_pThisRef);
}

void CWaitingDlg::EndWork()
{
	if( m_pRoot )
		m_pRoot->SendEvent(EVENTID_WAITENDWORK, NULL, 0, m_pThisRef);
}

void CWaitingDlg::WorkSyncEvt(tDWORD nEventId, cSerializable * pData)
{
	if( m_pRoot )
		m_pRoot->SendEvent(nEventId, pData, EVENT_SEND_SYNC, m_pThisRef, 30000);
}

//////////////////////////////////////////////////////////////////////////
// CListSinkMaster

bool CListSinkMaster::OnListEvent(eListEvents nEvent)
{
	if( nEvent == leFocusChanged )
	{
		if( m_pDescr )
			m_pDescr->SetItemData(GetSelItemData(TOR(cRowId, ())));
	}
	return true;
}

void CListSinkMaster::OnChangedData(CItemBase * pItem)
{
	CListItemSink::OnChangedData(pItem);
	
	if( m_pDescr && m_pDescr->m_pItem == pItem )
	{
		cRowId nItem;
		if( cSer * ser = GetSelItemData(nItem) ) 
		{
			if( m_pDescr->GetItemData(ser) )
			{
				Item()->UpdateItem(nItem);
				Item()->SetChangedData();
			}
		}
	}
}

cSer * CListSinkMaster::GetItemData(cRowId &nItem)
{
/*	if( Item()->IsTree() )
	{
		tIdxArr idxs; nItem.IdxArr(idxs);
		return GetTreeData(idxs, nItem.IsNode());
	}

	if( nItem.IsNode() )
		return NULL;
*/
	return Item()->GetRowData(nItem);
}

cSer * CListSinkMaster::GetSelItemData(cRowId &nItem)
{
	if( GetSingleSelectedItem(nItem) )
		return GetItemData(nItem);
	return NULL;
}

bool CListSinkMaster::GetSingleSelectedItem(cRowId &nItem)
{
	if( Item()->GetSelectedCount() == 1 )
		return Item()->GetSelectedItem(nItem) != CListItem::invalidIndex;
	return false;
}

//////////////////////////////////////////////////////////////////////////
// CListViewSink

CListViewSink::CListViewSink() :
	m_nSerID(tTYPE_ID(cSer::eIID)),
	m_DataTimestamp(0),
	m_pEditItem(NULL),
	m_pBtnAdd(NULL),
	m_pBtnEdit(NULL),
	m_pBtnDelete(NULL),
	m_pBtnMoveUp(NULL),
	m_pBtnMoveDown(NULL),
	m_pBtnExport(NULL),
	m_pBtnImport(NULL)
{}

bool CListViewSink::OnSetType(tTYPE_ID nType)
{
	tTYPE_ID nFixedType = OnGetFixedType();
	if( nFixedType != cSer::eIID && nFixedType != nType )
		return false;

	if( m_nSerID != cSer::eIID && m_nSerID == nType )
		return true;
	
	m_nSerID = nType;

	if( !Item()->Bind(m_nSerID) )
		return false;

	CItemPropVals &vals = Item()->m_pProps->Get("unique");
	if( vals.IsValid() )
	{
		cSerializableObj tmp; tmp.init(m_nSerID);

		m_pUniqueBinder = Root()->CreateFieldsBinder(cSerializableObj::getDescriptor(m_nSerID));
		m_pUniqueBinder->AddDataSource(tmp, "newItem");
		m_pUniqueBinder->m_pParent = Item()->GetBinder();

		if( m_pUniqueChecker = Root()->CreateNode(vals.GetStr(0), m_pUniqueBinder) )
		{
			m_pUniqueCopier = Root()->CreateNode(vals.GetStr(1), m_pUniqueBinder);
			m_pUniqueBinder->ClearSources(false);
		}
		else
			m_pUniqueBinder->Destroy(), m_pUniqueBinder = NULL;
	}

	return true;
}

void CListViewSink::OnInit()
{
	CListItemSink::OnInit();

	CItemBase * pParent = Item()->m_pParent;
	if( !pParent )
		return;

	#define BINDBTN(_Btn)	\
	if( m_pBtn##_Btn = GetItem(#_Btn) )	\
		m_pBtn##_Btn->AttachSink(new BtnSink(this), true);	\

	BINDBTN(Add);
	BINDBTN(Delete);
	BINDBTN(Edit);
	BINDBTN(MoveUp);
	BINDBTN(MoveDown);
	BINDBTN(Export);
	BINDBTN(Import);

	m_aBtns.clear();
	for(CItemBase * pBtn = NULL; pBtn = pParent->GetNextItem(pBtn, NULL, NULL, false); )
	{
		if( pBtn->m_strItemAlias == ITEMALIAS_LISTBUTTON )
		{
			pBtn->AttachSink(new BtnSink(this), true);
			m_aBtns.push_back(pBtn);
		}
	}
}

void CListViewSink::OnDestroy()
{
	CListItemSink::OnDestroy();
	
	m_pDescr = NULL;
	m_pEditItem = NULL;
	m_nEditItem = 0;
}

void CListViewSink::OnEnable(bool bOn)
{
	if( m_pBtnAdd )      m_pBtnAdd->EnableDep(bOn);
	if( m_pBtnEdit )     m_pBtnEdit->EnableDep(bOn);
	if( m_pBtnDelete )   m_pBtnDelete->EnableDep(bOn);
	if( m_pBtnMoveUp )   m_pBtnMoveUp->EnableDep(bOn);
	if( m_pBtnMoveDown ) m_pBtnMoveDown->EnableDep(bOn);
	if( m_pBtnExport )   m_pBtnExport->EnableDep(bOn);
	if( m_pBtnImport )   m_pBtnImport->EnableDep(bOn);
	if( m_pDescr )       m_pDescr->Item()->EnableDep(bOn);

	for(size_t i = 0; i < m_aBtns.size(); i++)
		m_aBtns[i]->EnableDep(bOn);
}

bool CListViewSink::OnClicked(CItemBase* pItem)
{
	if( pItem == m_pBtnAdd )       return AddItem(), true;
	if( pItem == m_pBtnEdit )      return EditSelectedItem(m_pBtnEdit), true;
	if( pItem == m_pBtnDelete )    return DeleteSelectedItems(), true;
	if( pItem == m_pBtnMoveUp )    return MoveSelectedItem(true), true;
	if( pItem == m_pBtnMoveDown )  return MoveSelectedItem(false), true;

	return false;
}

bool CListViewSink::OnDblClicked(CItemBase * pItem)
{
	return (m_pBtnEdit && !(m_pBtnEdit->m_nState & ISTATE_DISABLED)) ? OnClicked(m_pBtnEdit) : false;
}

void CListViewSink::OnEvent(tDWORD nEventId, cSerializable *pData)
{
	if( nEventId == EVENT_ID_ICONEXTRUCTED )
		Item()->UpdateLayout(true);
}

bool CListViewSink::OnListEvent(eListEvents nEvent)
{
	if( nEvent == leFocusChanged )
		UpdateCtrls();
	
	return CListSinkMaster::OnListEvent(nEvent);
}

void CListViewSink::UpdateCtrls()
{
	if( Item()->m_nState & ISTATE_DISABLED )
		return;

	cRowId nSel; Item()->GetSelectedItem(nSel);
	size_t nSelCnt  = Item()->GetSelectedCount();
	bool bCanMove   = nSelCnt == 1;

// 	if( m_pBtnEdit )
// 	{
// 		bool bCanEdit = nSelCnt == 1;
// 		if( bCanEdit )
// 			bCanEdit = OnCanOperateItem(nSel, opEdit);
// 		m_pBtnEdit->Enable(bCanEdit);
// 	}
// 
// 	if( m_pBtnDelete )
// 	{
// 		bool bCanDel = nSelCnt > 0;
// 		if( bCanDel )
// 			for(cRowId nSel; bCanDel && Item()->GetSelectedItem(nSel) != CListItem::invalidIndex; )
// 				bCanDel = OnCanOperateItem(nSel, opDelete);
// 		m_pBtnDelete->Enable(bCanDel);
// 	}

	if( m_pBtnMoveUp )
	{
		bool bCanMoveUp = bCanMove && nSel.m_pos > 1;
		if( bCanMoveUp )
		{
			cRowId nPrev(nSel.m_node, nSel.m_pos - 1);
			bCanMoveUp = OnCanOperateItem(nSel, opMove) && OnCanOperateItem(nPrev, opMove);
		}
		m_pBtnMoveUp->Enable(bCanMoveUp);
	}

	if( m_pBtnMoveDown )
	{
		bool bCanMoveDown = bCanMove && nSel.m_pos && nSel.m_pos < nSel.RowCount();
		if( bCanMoveDown )
		{
			cRowId nNext(nSel.m_node, nSel.m_pos + 1);
			bCanMoveDown = OnCanOperateItem(nSel, opMove) && OnCanOperateItem(nNext, opMove);
		}
		m_pBtnMoveDown->Enable(bCanMoveDown);
	}

// 	if( m_pBtnExport )
// 	{
// 		m_pBtnExport->Enable(!!nCnt);
// 	}

	if( m_pDescr )
	{
		m_pDescr->SetItemData(nSelCnt == 1 ? GetItemData(nSel) : NULL);
	}
}

void CListViewSink::AddItem()
{
	cSerializableObj tmpItem;
	if( PR_SUCC(tmpItem.init(OnGetType())) )
	{
		tERROR err = EditItem(m_pBtnAdd ? m_pBtnAdd : m_pBtnEdit, tmpItem);
		if( err == errOK )
		{
			cRowId nItem;
			if( !GetSingleSelectedItem(nItem) )
				nItem.m_node = Item();
			if( InsertItem(nItem, tmpItem) )
				err = warnUPDATED;
		}
		if( err == warnUPDATED )
			Item()->SetChangedData();
	}
}

void CListViewSink::EditSelectedItem(CItemBase * pOpCtl)
{
	cRowId nSel;
	if( !GetSingleSelectedItem(nSel) )
		return;

	if( !OnCanOperateItem(nSel, opEdit) )
		return;

	if( OnOperateItem(nSel, opEdit) )
		return;

	cSer *pItem = GetItemData(nSel);
	if( !pItem )
		return;

	cSerializableObj tmpItem;
	tmpItem.assign(pItem);
	
	tERROR err = EditItem(pOpCtl, tmpItem, nSel);
	if( err == errOK )
	{
		if( pItem = GetItemData(nSel) )
		{
			pItem->assign(*tmpItem, true);
			err = warnUPDATED;
		}

	}
	if( err == warnUPDATED )
	{
		Item()->UpdateItem(nSel);
		Item()->SetChangedData();
	}
}

tERROR CListViewSink::EditItem(CItemBase * pOpCtl, cSer *pItem, const cRowId &nItem)
{
	cRowId voidRowId;

	m_pEditItem = pItem;
	m_nEditItem = nItem;

	CFieldsBinder *binder = Item()->GetBinder();
	binder->AddDataSource(pItem);

	tERROR err = OnEditItem(pOpCtl);
	if( err == errNOT_IMPLEMENTED )
	{
		tQWORD currDataTimestamp = m_DataTimestamp;

		if( !(pOpCtl->m_pBinding && pOpCtl->m_pBinding->GetHandler(CItemBinding::hOnClick)) )
			pOpCtl = pOpCtl == m_pBtnAdd ? m_pBtnEdit : m_pBtnAdd;
		
		cVariant result;
		if( pOpCtl && pOpCtl->m_pBinding )
			pOpCtl->m_pBinding->ProcessReflection(CItemBinding::hOnClick, result, &CStructData(pItem));
		err = result ? errOK : errNOT_OK;

		// пока работает диалог редактирования существующего элемента листа возможно обновление всех данных листа.
		// если это произошло, nItem более не указывает на валидный элемент листа.
		// в случае добавления нового элемента сие обновление ни на что не влияет.
		if( err == errOK && currDataTimestamp != m_DataTimestamp && !(m_nEditItem == voidRowId) )
			err = errPARAMETER_INVALID;
	}
	
	binder->ClearSources();
	
	if( err == errOK )
		err = OnPostOperateItem(m_nEditItem, opEdit, m_pEditItem) ? errNOT_OK : errOK;
	if( err == errOK )
		err = CheckItemExist(m_pEditItem, true, true);

	m_pEditItem = NULL;
	m_nEditItem = voidRowId;
	
	return err;
}

tERROR CListViewSink::CheckItemExist(cSer *pItem, bool bUpdateExisting, bool bFocusExisting)
{
	if( !m_pUniqueChecker )
		return errOK;

	size_t nCount = GetCount();
	if( !nCount )
		return errOK;

	cVariant res;

	m_pUniqueBinder->AddDataSource(pItem, "newItem");

	cNodeExecCtx ctx(Root(), NULL, &CStructData(NULL));
	for(size_t i = 0; i < nCount; i++)
	{
		cSer *ser = GetRowData(i);
		ctx.m_data->m_ser = ser;
		m_pUniqueChecker->Exec(res, ctx);

		if( res )
		{
			if( bUpdateExisting )
			{
				if( m_pUniqueCopier )
					m_pUniqueCopier->ExecGet(ctx);
				else
					ser->assign(*pItem, true);
			}
				
			if( bFocusExisting )
			{
				cRowId nItem;
				if( Item()->FindRow(i, nItem) )
					Item()->SetItemFocus(nItem, true);
			}
			break;
		}
	}

	m_pUniqueBinder->ClearSources(false);

	return res ? (bUpdateExisting ? warnUPDATED : errALREADY) : errOK;
}

bool CListViewSink::SwapItems(cRowId &nFirst, cRowId &nSecond)
{
	if( nFirst.IsNode() || nSecond.IsNode() )
		return false;

	cSer *first = GetItemData(nFirst);
	cSer *second = GetItemData(nSecond);
	if( !first || !second )
		return false;

	if( !OnCanOperateItem(nFirst, opMove) ||
		!OnCanOperateItem(nSecond, opMove) )
		return false;

	cSerializableObj tmp; tmp.init(OnGetType());
	tmp.assign(second);
	second->assign(*first, true);
	first->assign(*tmp, true);

	Item()->UpdateItem(nFirst);
	Item()->UpdateItem(nSecond);
	Item()->SetItemFocus(nSecond, true);
	
	return true;
}

void CListViewSink::MoveSelectedItem(bool bUp)
{
	cRowId nFirst;
	if( GetSingleSelectedItem(nFirst) )
	{
		cRowId nSecond(nFirst.m_node, nFirst.m_pos + (bUp ? -1 : +1));
		if( nSecond.m_pos >= 1 && nSecond.m_pos <= nSecond.RowCount() )
			if( SwapItems(nFirst, nSecond) )
				Item()->SetChangedData();
	}
}

void CListViewSink::DeleteSelectedItems()
{
	size_t n = Item()->GetSelectedCount();
	if( !n )
		return;

	if( !OnCanOperate(opDelete) )
		return;

	if( m_pBtnDelete && m_pBtnDelete->m_pBinding )
	{
		cVariant result;
		if( m_pBtnDelete->m_pBinding->ProcessReflection(CItemBinding::hOnClick, result) && !result )
			return;
	}

	typedef std::vector<cRowId> tIdxs;
	tIdxs idxs; idxs.reserve(n);

	for(cRowId rowID; Item()->GetSelectedItem(rowID) != CListItem::invalidIndex; )
		if( OnCanOperateItem(rowID, opDelete) )
			idxs.push_back(rowID);

	if( idxs.size() )
	{
		for(size_t i = 0; i < idxs.size(); i++)
		{
			cRowId &rowID = idxs[i];
			DeleteItem(rowID);
			rowID.DeleteItem();
		}
		Item()->UpdateLayout(true);
		Item()->SetChangedData();
	}
}

//////////////////////////////////////////////////////////////////////////
// CSerTreeView

CSerTreeView::CSerTreeView() :
	m_pRoot(NULL),
	m_pChildVecAcc(NULL)
{}

bool CSerTreeView::OnSetType(tTYPE_ID nType)
{
	if( !CListViewSink::OnSetType(nType) )
		return false;

	if( CFieldsBinder *pBinder = Item()->GetBinder() )
	{
		CItemProps &pProps = *Item()->m_pProps;
		m_pChildVecAcc = Root()->CreateNode(pProps.Get("childs").GetStr(), pBinder);
		m_pRootFinder = Root()->CreateNode(pProps.Get("root").GetStr(), pBinder);
	}	
	
	return !!m_pChildVecAcc;
}

bool CSerTreeView::OnSetValue(const cVariant &pValue)
{
	m_pRoot = const_cast<cSer *>(pValue.c_ser(m_nSerID));
	if( m_pRootFinder )
		m_pRoot = cSerTreeFind(Root(), m_pChildVecAcc, m_pRootFinder).Find(m_pRoot);
	
	if( !m_pRoot )
		return false;
	
	m_DataTimestamp++;
	Item()->UpdateView(0);
	return true;
}

bool CSerTreeView::GetTreeInfo(tIdxArr &pIdx, tDWORD *pNodes, tIdxArr &pRows)
{
	if( pIdx.empty() )
	{
		*pNodes = 1;
		return GetRowInfo(m_pRoot, pRows);
	}

	if( cSer *pSer = GetTreeDataInt(m_pRoot, pIdx, 1) )
	{
		cVectorHnd vect;
		if( GetChildren(pSer, vect) )
			*pNodes = vect.size();

		return GetRowInfo(pSer, pRows);
	}
	return true;
}

cSer * CSerTreeView::GetTreeDataInt(tIdxArr &pIdx, bool bIsNode)
{
	if( !bIsNode )
		return GetRowData(pIdx.back());

	if( pIdx.size() == 1 )
		return m_pRoot;

	return GetTreeDataInt(m_pRoot, pIdx, 1);
}

cSer * CSerTreeView::GetTreeDataInt(cSer *pSer, const tIdxArr &aIdxs, size_t nPos)
{
	if( nPos >= aIdxs.size() )
		return pSer;

	cVectorHnd vect;
	if( GetChildren(pSer, vect) )
	{
		size_t idx = aIdxs[nPos++];
		if( idx < vect.size() )
			return GetTreeDataInt((cSer *)vect.at(idx), aIdxs, nPos);
	}

	return NULL;
}

bool CSerTreeView::InsertItem(cRowId &nItem, cSer *pItem)
{
	if( !nItem.IsNode() )
		return false;

	cSer *ser = GetItemData(nItem);
	if( !ser )
		return false;

	cVectorHnd vect;
	if( !GetChildren(ser, vect) )
		return false;
	
	tIdx idx = vect.size();
	cSer *pNew = (cSer *)vect.insert(idx);
	vect.elem_assign(pNew, pItem);

	cRowId nItemNew(nItem.m_node->AddNode(idx));
	Item()->SetItemFocus(nItemNew, true);
	
	return true;
}

void CSerTreeView::DeleteItem(cRowId &nItem)
{
	if( !nItem.IsNode() )
		return;

	tIdxArr idxs;
	if( !nItem.IdxArr(idxs) || idxs.empty() )
		return;

	size_t idx = idxs.back(); 
	idxs.pop_back();

	if( cSer *ser = GetTreeData(idxs, true) )
	{
		cVectorHnd vect;
		if( GetChildren(ser, vect) )
			vect.remove(idx, 1);
	}
}

//////////////////////////////////////////////////////////////////////
// CVectorView

CVectorView::CVectorView() {}

bool CVectorView::OnSetType(tTYPE_ID nType)
{
	if( CListViewSink::OnSetType(nType) )
	{
		m_type = nType;
		return true;
	}
	return false;
}

bool CVectorView::OnSetValue(const cVariant &pValue)
{
	if( const cVectorData *vd = pValue.c_vector() )
	{
		if( !m_v )
		{
			attach(vd->m_v, vd->m_type, vd->m_flags);
			return true;
		}

		if( m_flags == vd->m_flags || m_type == vd->m_type )
		{
			m_v = vd->m_v;
			m_DataTimestamp++;
			Item()->UpdateView(0);
			return true;
		}
	}
	return false;
}

void CVectorView::OnDestroy()
{
	CListViewSink::OnDestroy();
	detach();
}

bool CVectorView::Validate(CItemBase *pItem, cSerializable *pData)
{
	return true;
}

bool CVectorView::InsertItem(cRowId &nItem, cSer *pItem)
{
	if( !m_v )
		return false;

	tIdx idx = size();
	tPTR pNew = insert(idx);
	elem_assign(pNew, pItem);

	cRowNode* node = nItem.m_node ? nItem.m_node : Item();
	
	cRowId nItemNew;
	node->AddRows(1, &idx, &nItemNew);
	Item()->SetItemFocus(nItemNew, true);
	
	return true;
}

void CVectorView::DeleteItem(cRowId &nItem)
{
	remove(nItem.Idx().m_idx, 1);
}

//////////////////////////////////////////////////////////////////////
// CListItemDescr

void CListItemDescr::OnInitProps(CItemProps& pProps)
{
	CItemBase *pDescOwner = Item()->GetItemByPath(pProps.Get("dataowner").Get().c_str());
	if( !pDescOwner || !pDescOwner->m_pSink )
		return;
	
	tDWORD nSerId = pDescOwner->GetType();
	if( !IS_SERIALIZABLE(nSerId) )
		return;

	m_pData.init(nSerId);
	Item()->InitBinding(NULL, cSerializableObj::getDescriptor(nSerId));
	
	m_pDescOwner = (CDescriptorDataOwner *)pDescOwner->m_pSink;
	m_pDescOwner->SetDescriptor(this);
	
	Item()->m_nFlags |= STYLE_SKIP_UPDATEDATA;
}

bool CListItemDescr::SetItemData(cSer *pData)
{
	if( pData )
	{
		Item()->m_nFlags &= ~STYLE_SKIP_UPDATEDATA;
		m_pData.assign(pData, false);
		Item()->AddDataSource(m_pData);
		Item()->UpdateData(true);
	}
	else
	{
		Item()->m_nFlags |= STYLE_SKIP_UPDATEDATA;
		CFieldsBinder * pBinder = Item()->GetBinder();
		if( pBinder )
			pBinder->ClearSources(false);
		
		Item()->SetText("");
	}

	return true;
}

bool CListItemDescr::GetItemData(cSer *pData)
{
	if( !pData )
		return false;
	
	pData->assign(*m_pData.ptr_ref(), false);
	return true;
}

void CListItemDescr::OnChangedData(CItemBase * pItem)
{
	CDescriptor::OnChangedData(pItem);
	if( m_pDescOwner )
		m_pDescOwner->OnChangedData(Item());
}

//////////////////////////////////////////////////////////////////////
// CListImpExpHelper

/*
bool CListImpExpHelper::Export(LPCSTR strFilesTypes)
{
	bool bBySel = true;
	if( !m_pList->GetSelectedCount() )
		bBySel = false;
	
	if( bBySel )
		switch( MsgBox(m_pList, "ListExportBySelectionQuestion", NULL, MB_YESNOCANCEL|MB_ICONQUESTION) )
		{
		case DLG_ACTION_NO:     bBySel = false; break;
		case DLG_ACTION_CANCEL: return false;
		}

	tObjPath strFileName;
	if( !m_pList->m_pRoot->BrowseFile(m_pList, NULL, strFilesTypes, strFileName, true) )
		return false;

	tERROR err = OnExportPrepare(strFileName);
	if( PR_FAIL( err ) )
	{
		MsgBoxErr(m_pList, &cGuiErrorInfo(err));
		return false;
	}

	int nCount = m_pList->GetItemsCount();
	int nSel = -1;
	int nIdx = -1;
	
	for(;;)
	{
		if( bBySel )
		{
			nIdx = m_pList->GetSourcePos(nSel = m_pList->GetSelectedItem(nSel));
			if( nIdx == -1 )
				break;
		}
		else
		{
			nIdx++;
			if( nIdx >= nCount )
				break;
		}

		err = OnExportItem( nIdx );
		if( PR_FAIL( err ) )
		{
			MsgBoxErr(m_pList, &cGuiErrorInfo(err));
			return false;
		}
	}

	return true;
}

bool CListImpExpHelper::Import(LPCSTR strFileOrTypes)
{
	bool bAppend = true;
	if( m_pList && m_pList->GetItemsCount() )
	{
		switch( MsgBox(m_pList, "ListImportAppendQuestion", NULL, MB_YESNOCANCEL|MB_ICONQUESTION) )
		{
		case DLG_ACTION_NO:     bAppend = false; break;
		case DLG_ACTION_CANCEL: return false;
		}
	}
	
	tObjPath strFileName;
	if( m_pList )
	{
		if( !m_pList->m_pRoot->BrowseFile(m_pList, NULL, strFileOrTypes, strFileName) )
			return false;
	}
	else
	{
		strFileName = strFileOrTypes;
	}
	
	tERROR err = OnImport(strFileName, bAppend);
	if( PR_FAIL( err ) )
	{
		if( m_pList )
			MsgBoxErr(m_pList, &cGuiErrorInfo(err));
		return false;
	}
	
	if( m_pList )
		m_pList->UpdateView(UPDATE_FLAG_REINIT);
	
	return true;
}
*/

//////////////////////////////////////////////////////////////////////////
// CStringListImportHelper

bool CStrListImpExpHelper::Export(cStringObj &sFile)
{
	if( PR_FAIL(OnExportPrepare(sFile)) )
		return false;
	for(int i = 0; i < OnGetSize(); i++)
		if( PR_FAIL(OnExportItem(i)) )
			return false;
	return true;
}

bool CStrListImpExpHelper::Import(cStringObj &sFile)
{
	return PR_SUCC(OnImport(sFile, true));
}

tERROR CStrListImpExpHelper::OnExportPrepare(tObjPath& strFileName)
{
	cIOObj hIo((cObject *)g_root, cAutoString(strFileName), fACCESS_WRITE, fOMODE_CREATE_ALWAYS|fOMODE_SHARE_DENY_WRITE);
	if( PR_FAIL(hIo.last_error()) )
		return hIo.last_error();
	
	m_hIo = hIo.relinquish();
	return errOK;
}

tERROR CStrListImpExpHelper::OnExportItem(int nPos)
{
	cStringObj *pStr = OnGetString(nPos);
	if( !pStr )
		return errOK;
	
	tString strItem ( pStr->c_str(cCP_ANSI) );
	strItem += "\r\n";
	
	tDWORD nWriteRes = 0;
	tERROR err = m_hIo->SeekWrite(&nWriteRes, m_nPos, (tPTR)strItem.c_str(), strItem.size());
	if( PR_FAIL( err ) )
		return err;

	m_nPos += nWriteRes;
	return errOK;
}

tERROR CStrListImpExpHelper::OnImport(tObjPath& strFileName, bool bAppend)
{
	tString strData;
	{
		tQWORD nFileSize;
		cIOObj hIo((cObject *)g_root, cAutoString(strFileName), fACCESS_READ);
		tERROR err = hIo.last_error();
		
		if( PR_SUCC(err) )
			err = hIo->GetSize(&nFileSize, IO_SIZE_TYPE_EXPLICIT);
		
		if( PR_SUCC(err) )
		{
			strData.resize((unsigned int)nFileSize);
			
			tDWORD size = (tDWORD)nFileSize;
			err = hIo->SeekRead(&size, 0, &strData[0], size);
		}
		
		if( PR_FAIL(err) )
			return err;
	}

	if( !bAppend )
		OnClearVector();
	
	for(LPCSTR strItem = strData.c_str(); ;)
	{
		LPSTR strSep = (LPSTR)strstr(strItem, "\r\n");
		if( strSep )
			*strSep = 0;

		tString strTmp(strItem);
		g_pRoot->TrimString(strTmp);
		
		if( !strTmp.empty() )
			OnPutString(cStringObj(strTmp.c_str()));

		if( !strSep )
			break;

		strItem = strSep + 2;
	}

	return errOK;
}

//////////////////////////////////////////////////////////////////////////
// CReportView

CReportView::CReportView(tTYPE_ID type, LPCSTR profileName) :
	m_lastEventsCount(0), m_profileName(profileName)
{
	m_ptr = NULL;
	m_used = 0;
	m_allocated = 0;
	attach(this, type);
}

void CReportView::OnInitProps(CItemProps& pProps)
{
	CVectorView::OnInitProps(pProps);
	
	if( OnSetType(m_type) )
	{
		tERROR err = m_msgReceiver.create((cObj *)g_hGui, IID_MSG_RECEIVER);
		if( PR_SUCC(err) ) err = m_msgReceiver->propSetPtr(pgRECEIVE_PROCEDURE, MsgReceive);
		if( PR_SUCC(err) ) err = m_msgReceiver->propSetPtr(pgRECEIVE_CLIENT_ID, this);
		if( PR_SUCC(err) ) err = m_msgReceiver->sysCreateObjectDone();
		if( PR_SUCC(err) )
		{
			err = m_msgReceiver->sysRegisterMsgHandler(pmc_EVENTS_CRITICAL,  rmhLISTENER, g_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
			err = m_msgReceiver->sysRegisterMsgHandler(pmc_EVENTS_IMPORTANT, rmhLISTENER, g_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
			err = m_msgReceiver->sysRegisterMsgHandler(pmc_EVENTS_NOTIFY,    rmhLISTENER, g_hTM, IID_ANY, PID_ANY, IID_ANY, PID_ANY);
		}
		else
			if( !!m_msgReceiver )
				m_msgReceiver.clean();
	}
}

void CReportView::OnTimerRefresh(tDWORD nTimerSpin)
{
	CVectorView::OnTimerRefresh(nTimerSpin);
	if( size() != m_lastEventsCount )
	{
		m_lastEventsCount = size();
		Item()->UpdateView(0);
	}
}

void CReportView::OnEvent(tDWORD nEventId, cSerializable * pData)
{
	CVectorView::OnEvent(nEventId, pData);
	if( !pData )
		return;

	switch(nEventId)
	{
	case pmc_EVENTS_CRITICAL:
	case pmc_EVENTS_IMPORTANT:
	case pmc_EVENTS_NOTIFY:
		if( cSer *ser = (cSer *)insert(-1, 1) )
			ser->assign(*pData, true);
		break;

	case pmPROFILE_STATE_CHANDED:
		if( pData->isBasedOn(cProfileBase::eIID) )
		{
			if( ((cProfileBase*)pData)->m_sName == m_profileName )
			{
				clear();
				Item()->UpdateView(0);
			}
		}
		break;
	}
}

tERROR CReportView::MsgReceive(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT receive_point, tPTR pbuff, tDWORD *blen)
{
	if( blen == SER_SENDMSG_PSIZE && pbuff && (msg_cls == pmc_EVENTS_CRITICAL || msg_cls == pmc_EVENTS_IMPORTANT || msg_cls == pmc_EVENTS_NOTIFY) )
	{
		CReportView *pThis = (CReportView *)_this->propGetPtr(pgRECEIVE_CLIENT_ID);
		cTaskHeader *pTh = (cTaskHeader *)pbuff;
		if( pTh->isBasedOn(pThis->m_type) )
		{
			if( !(pThis->Item()->m_nState & ISTATE_HIDE) && pTh->m_strProfile == pThis->m_profileName )
				pThis->Root()->SendEvent(msg_cls, pTh, 0, pThis->Item());
		}
	}
	return errOK;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
