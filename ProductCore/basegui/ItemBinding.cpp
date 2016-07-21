// SerializableView.cpp: implementation of the CSerializableView class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <ProductCore/binding.h>

//////////////////////////////////////////////////////////////////////

static void GetString(cNode *node, cNodeExecCtx &ctx, tString &res)
{
	cVariant text; node->Exec(text, ctx);
	text.MakeString();
	swap_objmem(res, text.strRef());
}

//////////////////////////////////////////////////////////////////////
// cDependChecker

class cDependChecker : public cNode::cNodeEnum
{
public:
	cDependChecker(const CStructData * data, cNodeExecCtx &ctx, cNode* node) :
		m_data(data), m_ctx(ctx), m_serField(NULL), m_staticData(NULL), m_chkProp(false), m_node(node)
	{
		if( m_data->m_item )
		{
			if( m_data->m_propMask & CBindProps::prpValue )
			{
				if( m_data->m_item->m_pBinding )
				{
					if( cNode *node = m_data->m_item->m_pBinding->GetHandler(CItemBinding::hValue) )
					{
						if( cSerFieldAcc *serNode = (cSerFieldAcc *)node->Find(ntSerField) )
							m_serField = serNode->m_field;
						else if( cMemberAcc *memberNode = (cMemberAcc *)node->Find(ntMember) )
							m_staticData = memberNode->m_data;
					}
				}
			}
			
			m_chkProp = !!(m_data->m_propMask);
		}
	}

protected:
	virtual cNode::EnumRes Enum(cNode *node)
	{
		switch(node->Type())
		{
/*		case ntSwitch:
			{
				if( cNode::EnumRes res = ((cNodeSwitch *)node)->EnumChildren(this, ntAny, m_ctx) )
					return res;
				return cNode::erSkipChilds;
			}
*/		
		case ntGlobalVar:
			return cNode::erStop;

		case ntSerField:
			{
				cSerFieldAcc *serNode = (cSerFieldAcc *)node;

				// ѕроверка соответстви€ биндеров
				if( m_data->m_binder && m_data->m_binder != serNode->m_binder )
					return cNode::erContinue;

				// ѕроверка соответстви€ секций
				LPCSTR sectData = m_data->m_sect ? m_data->m_sect : "";
				if( sectData != STRUCTDATA_ALLSECTIONS )
				{
					LPCSTR sect = "";
					if( cSerFieldNamespace * pNs = (cSerFieldNamespace *)serNode->Find(ntNamespace) )
						sect = pNs->m_namespace;
					
					if( strcmp(sect, sectData) )
						return cNode::erContinue;
				}
				
				if( m_data->m_item )
					return StopOrCont(m_serField && serNode->m_field == m_serField);
				return StopOrCont(m_data->IsCompartible(serNode->m_serType));
			}
		case ntMember:
			{
				if( node == m_node )
				{
					bool ret = (m_data->m_sect == STRUCTDATA_ALLSECTIONS && !m_data->m_item) ? true : m_data->m_staticData;
					return ret ? cNode::erStop : cNode::erSkipChilds;
				}

				cMemberAcc *memberNode = (cMemberAcc *)node;
				if( m_staticData )
					return StopOrCont(memberNode->m_data == m_staticData);
				return StopOrCont((m_data->m_sect == STRUCTDATA_ALLSECTIONS && !m_data->m_item) || m_data->m_staticData);
			}
		case ntItemProp:
			if( m_chkProp )
			{
				cItemPropAcc *prpNode = (cItemPropAcc *)node;
				if( prpNode->m_item == m_data->m_item )
				{
					if( (m_data->m_propMask & CBindProps::prpCustom) && prpNode->m_propID == CBindProps::prpNone )
						return cNode::erStop;
					
					return StopOrCont(!!(prpNode->m_propID & m_data->m_propMask));
				}
			}
			break;
		}

		if( m_data->m_sect == STRUCTDATA_ALLSECTIONS && !m_data->m_ser && !m_data->m_item && !node->IsConst() )
			return cNode::erStop;

		return cNode::erContinue;
	}

public:
	const CStructData *         m_data;
	cNodeExecCtx&               m_ctx;
	const cSerDescriptorField * m_serField;
	const void *                m_staticData;
	bool                        m_chkProp;
	cNode *                     m_node;

public:
	static bool IsConst(cNode *node, cNodeExecCtx &ctx)
	{
		class cConstChecker : public cNode::cNodeEnum
		{
		public:
			cConstChecker(cNodeExecCtx &ctx) : m_ctx(ctx) {}
			
			virtual cNode::EnumRes Enum(cNode *node)
			{
				if( node->Type() == ntConst )
					return cNode::erSkipChilds;
						
				return StopOrCont(!node->IsConst());
			}

			cNodeExecCtx& m_ctx;
		};
		
		return node && node->Enum(&cConstChecker(ctx)) == cNode::erContinue;
	}
};

static bool IsAnyValid(cNode **nodes, size_t count)
{
	for(size_t i = 0; i < count; i++)
		if( nodes[i] )
			return true;
	return false;
}

//////////////////////////////////////////////////////////////////////
// CItemBase

void CItemBase::UpdateData(bool bIn, CStructData *pData)
{
	if( m_pBinding )
		m_pBinding->UpdateData(bIn, pData);

	size_t i, n = m_aItems.size();
	for(i = 0; i < n; i++)
	{
		CItemBase *pChild = m_aItems[i];
		if( !(pChild->m_nFlags & (STYLE_TOP_ITEM|STYLE_SKIP_UPDATEDATA)) )
			pChild->UpdateData(bIn, pData);
	}
}

void CItemBase::InitBinding(LPCSTR sFmtText, const cSerDescriptor* pDesc)
{
	if( !m_pBinding )
	{
		m_pBinding = new CItemBinding(this);
		m_pBinding->InitBinding(pDesc);
	}

/*	CItemBinding * pBinding = Item()->m_pRoot->CreateItemBinding(pLink, pBind);
	pBinding->InitEx(strLink, NULL, false);
	CStructData data (NULL, STRUCTDATA_ALLSECTIONS);
	pBinding->UpdateData(true, data);
	pBinding->Destroy();
*/
}

void CItemBase::AddDataSource(cSerializable* pData, LPCSTR strSect, bool bAddFirst)
{
	if( !m_pBinding )
		m_pBinding = new CItemBinding(this);

	m_pBinding->AddDataSource(pData, strSect, bAddFirst);
}

CFieldsBinder* CItemBase::GetBinder()
{
	if( m_pBinding )
		return m_pBinding->m_pBinder;

	if( m_pParent )
		return m_pParent->GetBinder();

	return NULL;
}

cSerFieldAcc * CItemBase::GetValueHolder()
{
	if( !m_pBinding )
		return NULL;
	
	cNode * pNode = m_pBinding->GetHandler(CItemBinding::hValue);
	if( !pNode )
		return NULL;
	
	return (cSerFieldAcc *)pNode->Find(ntSerField);
}

void CItemBase::OnChangedProp(tDWORD nPropMask)
{
	if( !nPropMask )
		return;

	bool bValue = !!(nPropMask & CBindProps::prpValue);
	if( bValue && m_pBinding )
		m_pBinding->UpdateData(false);

	CStructData data(NULL, STRUCTDATA_ALLSECTIONS, this, nPropMask);

	CItemBase * pUpdateItem = NULL;

/*	if( bValue )
	{
		CFieldsBinder * pBinder = GetBinder();
		for(CItemBase * p = this; p; p = p->m_pParent)
		{
			if( pBinder && p->m_pBinding && p->m_pBinding->m_pBinder == pBinder )
				pUpdateItem = p;
			if( p->m_nFlags & STYLE_TOP_ITEM )
			{
				if( !pUpdateItem )
					pUpdateItem = p;
				break;
			}
		}
	}*/
	
	if( !pUpdateItem )
		pUpdateItem = GetOwner(true);
/*	{
		if( m_nFlags & STYLE_TOP_ITEM )
			pUpdateItem = this;
		else
			for(CItemBase * p = m_pParent; p; p = p->m_pParent)
			{
				if( p->m_nFlags & (STYLE_ENUM|STYLE_TOP_ITEM) )
				{
					pUpdateItem = p;
					break;
				}
			}
	}*/

	if( pUpdateItem )
	{
		if( m_pOwner ) m_pOwner->LockUpdate(true, false);
		pUpdateItem->UpdateData(true, &data);
		if( m_pOwner ) m_pOwner->LockUpdate(false, true);
	}
}

//////////////////////////////////////////////////////////////////////
// CItemBinding

CItemBinding::CItemBinding(CItemBase* pItem, CItemBase* pCtxItem)
{
	memset(this, 0, sizeof(*this));
	m_pItem = pItem;
	m_pBinder = (pCtxItem ? pCtxItem : pItem)->GetBinder();
}

CItemBinding::CItemBinding(const CItemBinding &other)
{
	*this = other;
	memset((void *)&other, 0, sizeof(other));
}

void CItemBinding::Assign(const CItemBinding &other)
{
	CFieldsBinder* binder = m_bOwnBinder ? m_pBinder : NULL;
	*this = other;
	if( binder )
	{
		m_pBinder = binder;
		m_bOwnBinder = true;
	}
	memset((void *)&other, 0, sizeof(other));
}

CItemBinding::~CItemBinding()
{
	if( m_pBinder )
	{
		for(size_t i = 0; i < countof(m_holders); i++)
			if( m_holders[i] )
				m_holders[i]->Destroy(), m_holders[i] = NULL;
		
		if( m_bOwnBinder )
			m_pBinder->Destroy();
	}
}

bool CItemBinding::Init(CItemProps& pProps)
{
	if( m_pItem->m_nFlags & STYLE_DEBUG )
		debugBreak();

	CRootItem *pRoot = m_pBinder->m_pRoot;
	cNodeExecCtx ctx(pRoot, m_pItem);

	if( CGuiPtr<cNode> pNodeNs = InitHolder(m_pItem, pProps, STR_PID_NAMESPACE) )
	{
		cVariant vSer; pNodeNs->Exec(vSer, ctx);
		cSer * pSer = (cSer *)vSer.c_ser();
		if( pSer )
			m_pItem->AddDataSource(pSer, NULL, true);
	}

	if( CGuiPtr<cNode> pNodeOnInit = InitHolder(m_pItem, pProps, STR_PID_ONINIT) )
	{
		cVariant initValue;
		pNodeOnInit->Exec(initValue, ctx);
	}

	{
		cNode *&holder = m_holders[hValue]; holder = InitHolder(m_pItem, pProps, STR_PID_VALUE);
		if( cDependChecker::IsConst(holder, ctx) )
		{
			cVariant constValue; holder->Exec(constValue, ctx);
			if( m_pItem->SetValue(constValue) )
				holder->Destroy(), holder = NULL;
		}

		if( holder && (m_pItem->IsVector() || m_pItem->m_strItemType == GUI_ITEMT_LISTCTRL) )
		{
			cVariant value; holder->Exec(value, ctx);
			if( m_pItem->IsVector() )
			{
				if( value.Type() == cVariant::vtVector )
					m_pItem->SetType(value.c_vector()->m_type);
			}
			else
			{
				if( value.Type() == cVariant::vtSer )
					m_pItem->SetType((tTYPE_ID)value.c_ser()->getIID());
			}
		}
	}
	
	// todo: как-ть поправить костыль. сейчас значение радиоэлементов хранитс€ в CItemBinding в холдере value.
	// чтобы, во врем€ инициализации CItemBinding константные выражени€ могли ссылатьс€ на это значение,
	// нужно, чтобы у элемента была ссылка на биндинг.
	bool bBindingAbsent = !m_pItem->m_pBinding;
	if( bBindingAbsent )
		m_pItem->m_pBinding = this;

	{
		tString text = pProps.Get(STR_PID_TEXT).Get();
		if( text.empty() )
		{
			CItemBase *parent = m_pItem->m_pParent;
			if( (m_pItem->m_nFlags & STYLE_EXT_ITEM) && parent )
				pRoot->GetString(text, PROFILE_LOCALIZE, parent->m_strSection.c_str(), m_pItem->m_strItemId.c_str());

			if( text.empty() )
			{
				if( m_pItem->m_nFlags & STYLE_EXT_ITEM )
					pRoot->GetString(text, PROFILE_LOCALIZE, m_pItem->m_strSection.c_str(), STR_SELF);
				else if( parent )
					pRoot->GetString(text, PROFILE_LOCALIZE, parent->m_strSection.c_str(), m_pItem->m_strItemId.c_str());
			}
		}

		cNode *&holder = m_holders[hText]; holder = InitHolder(m_pItem, text.c_str());
		if( cDependChecker::IsConst(holder, ctx) )
		{
			GetString(holder, ctx, text);
			holder->Destroy();
			holder = NULL;

			m_pItem->SetText(text.c_str());
		}
	}	
	{
		tString tipText = pProps.Get(STR_PID_TIPTEXT).Get();
		if( tipText.empty() )
		{
			tString tipID = m_pItem->m_strItemId; tipID += GUI_STR_AUTOSECT_SEP_STR STR_PID_TIPTEXT;
			CItemBase *parent = m_pItem->m_pParent;
			if( (m_pItem->m_nFlags & STYLE_EXT_ITEM) && parent )
				pRoot->GetString(tipText, PROFILE_LOCALIZE, parent->m_strSection.c_str(), tipID.c_str());
			else
				m_pItem->LoadLocString(tipText, tipID.c_str());
		}
		
		cNode *&holder = m_holders[hTip]; holder = InitHolder(m_pItem, tipText.c_str());
		if( cDependChecker::IsConst(holder, ctx) )
		{
			GetString(holder, ctx, tipText);
			holder->Destroy();
			holder = NULL;

			m_pItem->SetTipText(tipText.c_str());
		}
	}
	{
		cNode *&holder = m_holders[hEnabled]; holder = InitHolder(m_pItem, pProps, STR_PID_ENABLE);
		if( cDependChecker::IsConst(holder, ctx) )
		{
			cVariant constValue; holder->Exec(constValue, ctx);
			holder->Destroy(), holder = NULL;

			m_pItem->Enable(constValue);
		}
	}
	{
		cNode *&holder = m_holders[hVisible]; holder = InitHolder(m_pItem, pProps, STR_PID_VISIBLE);
		if( cDependChecker::IsConst(holder, ctx) )
		{
			cVariant constValue; holder->Exec(constValue, ctx);
			holder->Destroy(), holder = NULL;

			m_pItem->Show(constValue);
		}
	}
	{
		cNode *&holder = m_holders[hIcon]; holder = InitHolder(m_pItem, pProps, STR_PID_ICON);
		if( cDependChecker::IsConst(holder, ctx) )
		{
			cVariant constValue; holder->Exec(constValue, ctx);
			holder->Destroy(), holder = NULL;

			cItemPropAcc::SetIcon(m_pItem, constValue);
		}
	}
	{
		cNode *&holder = m_holders[hFont]; holder = InitHolder(m_pItem, pProps, STR_PID_FONT);
		if( cDependChecker::IsConst(holder, ctx) )
		{
			cVariant constValue; holder->Exec(constValue, ctx);
			holder->Destroy(), holder = NULL;

			m_pItem->SetFont(pRoot->GetFont(variant_strref(constValue).c_str()));
		}
	}
	{
		cNode *&holder = m_holders[hBg]; holder = InitHolder(m_pItem, pProps, STR_PID_BACKGND);
		if( cDependChecker::IsConst(holder, ctx) )
		{
			cVariant constValue; holder->Exec(constValue, ctx);
			holder->Destroy(), holder = NULL;

			m_pItem->SetBackground(variant_strref(constValue).c_str());
		}
	}
	{
		cNode *&holder = m_holders[hProps]; holder = InitHolder(m_pItem, pProps, STR_PID_BIND_PROPS);
		if( cDependChecker::IsConst(holder, ctx) )
		{
			cVariant constValue; holder->Exec(constValue, ctx);
			holder->Destroy(), holder = NULL;
		}
	}

	if( bBindingAbsent )
		m_pItem->m_pBinding = NULL;

	m_holders[hOnClick]    = InitHolder(m_pItem, pProps, STR_PID_ONCLICK);
	m_holders[hOnRClick]   = InitHolder(m_pItem, pProps, STR_PID_ONRCLICK);
	m_holders[hOnSelect]   = InitHolder(m_pItem, pProps, STR_PID_ONSELECT);
	m_holders[hOnChanging] = InitHolder(m_pItem, pProps, STR_PID_ONCHANGING);
	m_holders[hOnChange]   = InitHolder(m_pItem, pProps, STR_PID_ONCHANGE);
	m_holders[hOnOk]       = InitHolder(m_pItem, pProps, STR_PID_ONOK);
	m_holders[hOnClose]    = InitHolder(m_pItem, pProps, STR_PID_ONCLOSE);

	return IsAnyValid(m_holders, countof(m_holders));
}

cNode* CItemBinding::InitHolder(CItemBase * pItem, LPCSTR strProp)
{
	if( !(strProp && *strProp) )
		return NULL;
	
	CNodeFactory factory(pItem);
	return cNode::Parse(strProp, &factory, factory.m_pAlloc);
}

cNode *CItemBinding::InitHolder(CItemBase * pItem, CItemProps &pProps, LPCSTR strPropID)
{
	CItemPropVals &prpText = pProps.Get(strPropID);
	if( prpText.IsValid() )
		return InitHolder(pItem, prpText.GetStr());
	return NULL;
}

bool CItemBinding::InitBinding(const cSerDescriptor* pDesc)
{
	if( !m_bOwnBinder )
	{
		m_pBinder = new CFieldsBinder(m_pItem->m_pRoot, m_pBinder, m_pItem->m_pSink, pDesc);
		m_bOwnBinder = true;
	}
	return true;
}

void CItemBinding::AddDataSource(cSerializable* pData, LPCSTR strSect, bool bAddFirst)
{
	InitBinding(NULL);
	m_pBinder->AddDataSource(pData, strSect, bAddFirst);
}

void CItemBinding::UpdateData(bool bIn, CStructData *pData)
{
	if( m_pItem->m_nFlags & STYLE_DEBUG )
		debugBreak();

	if( !pData )
		pData = CStructData::Default();
	
	CRootItem *pRoot = m_pBinder->m_pRoot;
	cNodeExecCtx ctx(pRoot, m_pItem, pData);

	cDependChecker checker(pData, ctx, NULL);

	if( bIn )
	{
		cVariant res;
		
#define EXEC_HOLDER(HOLDER, ACTIONS)  { cNode * node = m_holders[HOLDER]; checker.m_node = node; \
	if( node && (node->Enum(&checker) == cNode::erStop) ) { node->Exec(res, ctx); ACTIONS; } }

		EXEC_HOLDER(hProps,   )
		EXEC_HOLDER(hText,    res.MakeString(); m_pItem->SetText(res.c_str()))
		EXEC_HOLDER(hEnabled, m_pItem->Enable(res))
		EXEC_HOLDER(hVisible, m_pItem->Show(res))
		EXEC_HOLDER(hIcon,    cItemPropAcc::SetIcon(m_pItem, res))
		EXEC_HOLDER(hFont,    m_pItem->SetFont(pRoot->GetFont(res.c_str())))
		EXEC_HOLDER(hBg,      m_pItem->SetBackground(res.c_str()))
	}
	else
		if( !m_pItem->IsOut() || m_pItem->m_nFlags & STYLE_ENUM_VALUE )
			return;

	// -- value binding --
	cNode *valueHolder = m_holders[hValue];
	checker.m_node = valueHolder;
	if( !valueHolder || valueHolder->Enum(&checker) != cNode::erStop )
		return;

	cVariant value;
	if( m_pItem->IsVector() )
	{
		valueHolder->Exec(value, ctx);
		if( value.Type() == cVariant::vtVector )
		{
			if( bIn )
				m_pItem->SetValue(value);
			else
				m_pItem->GetValue(value);
		}		
		return;
	}
	
	if( bIn )
	{
		valueHolder->Exec(value, ctx);

		bool set = true;
		cVariant curCtlValue;
		if( m_pItem->GetValue(curCtlValue) )
			set = curCtlValue != value;
		
		if( set )
			m_pItem->SetValue(value);
		
		return;
	}
	
	if( m_pItem->GetValue(value) )
		valueHolder->Exec(value, ctx, false);
}

cNode::EnumRes CItemBinding::EnumNodes(cNode::cNodeEnum *enumerator, cNode::type_t type)
{
	for(size_t i = 0; i < countof(m_holders); i++)
		if( m_holders[i] )
		{
			if( cNode::EnumRes res = m_holders[i]->Enum(enumerator, type) )
				return res == cNode::erSkipChilds ? cNode::erContinue : res;
		}

	return cNode::erContinue;
}

//////////////////////////////////////////////////////////////////////
