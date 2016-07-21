// Formats.cpp: implementation of the CFormats class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Formats.h"
#include "Graphics.h"
#include <Prague/iface/e_ktrace.h>
#include "../../Prague/IniLib/IniLib.h"
#include <ProductCore/structs/s_settings.h>
#include <ProductCore/common/MatchStrings.h>
#include <hashutil/hashutil.h>
#include <algorithm>

#if defined (__unix__)
#include <unix/compatutils/compatutils.h>
#endif

static bool MatchWithPattern(const char * Pattern, const char * Name, bool bPatternWithAsterics);

static cNode * GetOrigNode(cNode * pNode)
{
	if( !pNode || pNode->Type() != ntRedirect )
		return pNode;
	return GetOrigNode(((cNodePrmArray::cRedirArg *)pNode)->m_arg);
}

//////////////////////////////////////////////////////////////////////
// cNodeDebug

void cNodeDebug::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
//	__asm int 3;
	m_args[0]->Exec(value, ctx, get);
}

//////////////////////////////////////////////////////////////////////////
// cNodeConst

void cNodeConst::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	m_args[0]->Exec(value, ctx, get);
}

//////////////////////////////////////////////////////////////////////
// cItemPropAcc

void cItemPropAcc::Exec(cVariant &result, cNodeExecCtx &ctx, bool get)
{
	if( get )
	{
		switch(m_propID)
		{
		case CBindProps::prpValue:      m_item->GetValue(result); break;
		case CBindProps::prpVisible:    result = !(m_item->m_nState & ISTATE_HIDE); break;
		case CBindProps::prpEnabled:    result = !(m_item->m_nState & ISTATE_DISABLED); break;
		case CBindProps::prpSelected:   result = !!(m_item->m_nState & ISTATE_SELECTED); break;
		case CBindProps::prpHotlight:   result = !!(m_item->m_nState & ISTATE_HOTLIGHT); break;
		case CBindProps::prpText:       result = m_item->m_strText; break;
		case CBindProps::prpNone:       result.Init(m_item); break;
		}
		return;
	}

	switch(m_propID)
	{
	case CBindProps::prpValue:          m_item->SetValue(result); break;
	case CBindProps::prpIcon:           SetIcon(m_item, result); break;
	case CBindProps::prpFont:           m_item->SetFont(ctx.m_root->GetFont(result.c_str())); break;
	case CBindProps::prpBackground:     m_item->SetBackground(result.c_str()); break;
	case CBindProps::prpVisible:        m_item->Show(result); break;
	case CBindProps::prpEnabled:        m_item->Enable(result); break;
	case CBindProps::prpAnimated:       m_item->m_pRoot->AnimateItem(m_item, result); break;
	case CBindProps::prpHotlight:       m_item->Hotlight(result); break;
	case CBindProps::prpSelected:       m_item->Select(result); break;
	case CBindProps::prpText:           m_item->SetText(result.c_str()); break;
	}
}

void cItemPropAcc::SetIcon(CItemBase *item, const cVariant &icon)
{
	if( icon.Type() == cVariant::vtIco )
		item->SetIcon(icon.c_ico());
	else
		item->SetIcon(icon.c_str());
}

//////////////////////////////////////////////////////////////////////
// cSerFieldAcc

tPTR offsetField(tPTR data, const cSerDescriptorField * field)
{
	if( IS_SERIALIZABLE(field->m_id) && field->m_flags & SDT_FLAG_POINTER )
		data = *(cSerializable**)SADDR(data, field);
	else
		data = (cSerializable*)SADDR(data, field);
	return data;
}

cSerFieldAcc::cSerFieldAcc(cNode *container, const cSerDescriptorField * field, tTYPE_ID serType) :
	cNodeArgs<1>(&container),
 	m_field(field),
	m_serType(serType),
	m_binder(NULL)
{}

void cSerFieldAcc::Exec(cVariant &result, cNodeExecCtx &ctx, bool get)
{
	if( cSer *ser = GetStruct(ctx) )
	{
		CopyData(result, (tTYPE_ID)m_field->m_id, m_field->m_flags, offsetField(ser, m_field), get);
		if( !get )
			ctx.m_datachanged = 1;
	}
	else
		if( get )
			result.Clear();
}

cSer *cSerFieldAcc::GetStruct(cNodeExecCtx &ctx)
{
	cSer *ser = NULL;
	if( m_args[0] )
	{
		if( m_args[0]->Type() == ntNamespace )
			ser = ((cSerFieldNamespace *)m_args[0])->GetStruct(m_serType);
		else
			ser = const_cast<cSer *>(m_args[0]->ExecGet(ctx).c_ser());
	}
	else
	{
		ser = ctx.m_data ? ctx.m_data->m_ser : NULL;
		if( !ser || !ser->isBasedOn(m_serType) )
			ser = GetBinderStruct();
	}
	return ser;
}

cSer * cSerFieldAcc::GetStruct()
{
	if( !IS_SERIALIZABLE(m_field->m_id) )
		return NULL;

	cSer * ser = NULL;
	if( cNode *container = m_args[0] )
	{
		switch(container->Type())
		{
		case ntSerField:  ser = ((cSerFieldAcc *)container)->GetStruct(); break;
		case ntNamespace: ser = ((cSerFieldNamespace *)container)->GetStruct(m_serType); break;
		case ntMember:    ser = (cSer *)((cMemberAcc *)container)->m_data; break;
		}
	}
	else if( m_binder )
		ser = GetBinderStruct();
	
	if( ser )
		ser = (cSer *)offsetField(ser, m_field);
	return ser;
}

cSer * cSerFieldAcc::GetBinderStruct()
{
	if( CFieldsBinder::Structs *structs = m_binder->GetNamespace() )
	{
		for(size_t i = 0, n = structs->size(); i < n; i++)
		{
			cSer *ser = structs->at(i);
			if( ser->isBasedOn(m_serType) )
				return ser;
		}
	}
	return NULL;
}

cNode * cSerFieldAcc::Init(CFieldsBinder *binder, cNode *container, const char *name)
{
	if( container )
	{
		if( container->Type() == ntSerField )
		{
			cSerFieldAcc *filedNode = (cSerFieldAcc *)container;
			const cSerDescriptorField * field = filedNode->m_field;
			if( !IS_SERIALIZABLE(field->m_id) )
				return NULL;
			if( field->m_id == cSer::eIID && field->m_flags & SDT_FLAG_POINTER )
			{
				if( cSer * ser = filedNode->GetStruct() )
					return Init(container, name, ser);
				return NULL;
			}
			return Init(container, name, (tTYPE_ID)field->m_id);
		}
		
		if( container->Type() == ntNamespace )
		{
			const CFieldsBinder::Structs &structs = ((cSerFieldNamespace *)container)->m_structs;
			for(size_t i = 0, n = structs.size(); i < n; i++)
			{
				if( cSer * ser = structs[i] )
					if( cNode *acc = Init(container, name, ser) )
						return acc;
			}
			return NULL;
		}

		if( container->Type() == ntMember )
		{
			cMemberAcc *memberNode = (cMemberAcc *)container;
			if( IS_SERIALIZABLE(memberNode->m_type) )
				return Init(container, name, memberNode->m_type);
			return NULL;
		}

		return new cSerFieldAccEx(container, name);
	}

	return InitEx(binder, container, name);
}

cNode * cSerFieldAcc::InitEx(CFieldsBinder *binder, cNode *container, const char *name)
{
	if( CFieldsBinder::Structs *structs = binder->GetNamespace(&name) )
		return new cSerFieldNamespace(name, *structs);

	cNode *acc = NULL;
	if( binder->m_pDesc )
		acc = Init(container, name, (tTYPE_ID)binder->m_pDesc->m_unique);

	if( !acc )
		if( CFieldsBinder::Structs *structs = binder->GetNamespace() )
		{
			for(size_t i = 0, n = structs->size(); i < n; i++)
				if( acc = Init(container, name, structs->at(i)) )
					break;
		}

	if( cSerFieldAcc *sfa = (cSerFieldAcc *)acc )
		sfa->m_binder = binder;
	
	if( !acc && binder->m_pParent )
		acc = InitEx(binder->m_pParent, container, name);

	return acc;
}

cNode * cSerFieldAcc::Init(cNode *container, const char *name, tTYPE_ID serType)
{
	const cSerDescriptorField * field;
	if( Bind(name, serType, field) )
		return new cSerFieldAcc(container, field, serType);
	return NULL;
}

cNode * cSerFieldAcc::Init(cNode *container, const char *name, cSer *ser)
{
	return Init(container, name, (tTYPE_ID)ser->getIID());
}

bool cSerFieldAcc::Bind(const char *name, tTYPE_ID &serType, const cSerDescriptorField *&serField)
{
	cSer * data = NULL;

	const cSerDescriptor *desc = cSerializableObj::getDescriptor(serType);

	for(const cSerDescriptorField * field = desc->m_fields; field->m_flags != SDT_FLAG_ENDPOINT; field++)
	{
		if( field->m_flags & SDT_FLAG_BASE )
		{
			tTYPE_ID subType = (tTYPE_ID)field->m_id; const cSerDescriptorField * subField = NULL;
			if( Bind(name, subType, subField) )
			{
				serType = subType;
				serField = subField;
				return true;
			}
			continue;
		}

		if( field->m_name && !strcmp(field->m_name, name) )
		{
			serField = field;
			return true;
		}
	}

	return false;
}

void cSerFieldAcc::CopyData(cVariant &result, tTYPE_ID type, tDWORD flags, tPTR data, bool get)
{
	if( flags & SDT_FLAG_VECTOR )
	{
		if( !get )
			return;

		cVectorData vector;
		vector.m_v     = (tMemChunk *)data;
		vector.m_type  = type;
		vector.m_flags = flags;
		result.Init(vector);
		return;
	}

	if( IS_SERIALIZABLE(type) )
	{
		if( !get )
			return;
		
		result.Init((cSer *)data);
		return;
	}

	if( get )
	{
		switch(type)
		{
		case tid_STRING_OBJ: result = ((cStringObj *)data)->data(); break;
		case tid_BYTE:       result = tQWORD(*(tBYTE *)data); break;
		case tid_SHORT:
		case tid_WORD:       result = tQWORD(*(tWORD *)data); break;
		case tid_BOOL:
		case tid_INT:
		case tid_UINT:
		case tid_LONG:
		case tid_ERROR:
		case tid_DWORD:      result = tQWORD(*(tDWORD *)data); break;
		case tid_QWORD:
		case tid_LONGLONG:   result = *(tQWORD *)data; break;
		case tid_DATETIME:   result = (tQWORD)(time_t)cDateTime((tDATETIME *)data); break;
		case tid_PTR:        result = (tQWORD)*(tPTR *)data; break;
		case tid_DOUBLE:     result = (tQWORD)*(tDOUBLE *)data; break;
		}
	}
	else
	{
		switch(type)
		{
		case tid_STRING_OBJ: result.ToStringObj(*(cStringObj *)data); break;
		case tid_BYTE:       *(tBYTE *)data = (tBYTE)result.ToInt(); break;
		case tid_SHORT:
		case tid_WORD:       *(tWORD *)data = (tWORD)result.ToInt(); break;
		case tid_BOOL:
		case tid_INT:
		case tid_UINT:
		case tid_LONG:
		case tid_ERROR:
		case tid_DWORD:      *(tDWORD *)data = (tDWORD)result.ToInt(); break;
		case tid_QWORD:
		case tid_LONGLONG:   *(tQWORD *)data = (tQWORD)result.ToInt(); break;
		case tid_DATETIME:   cDateTime((time_t)result.ToInt()).CopyTo(*(tDATETIME *)data); break;
		case tid_PTR:        *(tPTR *)data = (tPTR)result.ToQWORD(); break;
		case tid_DOUBLE:     *(tDOUBLE *)data = (tDOUBLE)result.ToQWORD(); break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// cSerFieldAccEx

void cSerFieldAccEx::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( cSer *ser = const_cast<cSer *>(m_args[0]->ExecGet(ctx).c_ser()) )
	{
		if( !m_field && m_name.size() )
		{
			m_serType = (tTYPE_ID)ser->getIID();
			cSerFieldAcc::Bind(m_name.c_str(), m_serType, m_field);

			m_name.clear();
		}

		if( m_field && ser->isBasedOn(m_serType) )
		{
			CopyData(value, (tTYPE_ID)m_field->m_id, m_field->m_flags, offsetField(ser, m_field), get);
			if( !get )
				ctx.m_datachanged = 1;
			return;
		}
	}
	
	if( get )
		value.Clear();
}

//////////////////////////////////////////////////////////////////////////
// cSerFieldNamespace

void cSerFieldNamespace::Exec(cVariant &result, cNodeExecCtx &ctx, bool get)
{
	result.Clear();
	
	if( get && m_structs.size() )
		result.Init(m_structs[0]);
}

cSer * cSerFieldNamespace::GetStruct(tTYPE_ID type)
{
	for(size_t i = 0, n = m_structs.size(); i < n; i++)
		if( m_structs[i] && m_structs[i]->isBasedOn(type) )
			return m_structs[i];
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// cNodePrmArray

cNodePrmArray::CNf::CNf(cNodePrmArray * p, CItemBase *pItem, CFieldsBinder *pBinder, LPCSTR strArgs) :
	CNodeFactory(pItem, pBinder), m_p(p)
{
	tDWORD i = 0;
	
	if( strArgs )
	{
		bool fPrm = false;
		for(; i < countof(m_a) - 1; strArgs++)
		{
			char c = *strArgs;
			cStrData& si = m_a[i];
			
			if( _kav_iscsym(c) )
			{
				if( !fPrm )
				{
					si.p = strArgs;
					fPrm = true;
				}
			}
			else
			{
				if( fPrm )
				{
					si.n = strArgs - si.p;
					i++;
					fPrm = false;
				}
			}

			if( !c )
				break;
		}
	}

	m_a[i].p = NULL;
}

cNode * cNodePrmArray::CNf::CreateOperand(const char *name, cNode *container, cAlloc *al)
{
	int len = strlen(name);
	if( !len )
		return NULL;
	
	cNodePrmArray * owner = m_p;

	for(cStrData * si = m_a; si->p; si++)
		if( !si->cmp(name, len) )
			return owner->GetArg(si - m_a);

	if( *name == STR_SELF_SYMB )
		return owner->GetArg(atol(name + 1));

	return CNodeFactory::CreateOperand(name, container, al);
}

//////////////////////////////////////////////////////////////////////////

void cNodePrmArray::cRedirArg::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !get )
	{
		if( m_arg )
			m_arg->Exec(value, ctx, false);
		return;
	}
	
	if( m_v.Type() == cVariant::vtVoid && m_arg )
		m_arg->Exec(m_v, ctx);
	value = m_v;
}

//////////////////////////////////////////////////////////////////////////

cNodePrmArray::cNodePrmArray() : m_args(NULL), m_args_n(0) {}
cNodePrmArray::~cNodePrmArray() { if( m_args ) delete [] m_args; }

void cNodePrmArray::Init(cNode **args)
{
	if( args[0] )
		m_arg1.m_arg = args[0], args[0] = NULL;

	for(int i = 1; args[i]; i++)
		m_args_n++;

	if( m_args_n )
		m_args = new cRedirArg[m_args_n];

	for(tDWORD i = 0; i < m_args_n; i++)
		m_args[i].m_arg = args[i + 1], args[i + 1] = NULL;
}

cNode * cNodePrmArray::GetArg(tDWORD idx)
{
	if( !idx )
		return &m_arg1;
	
	static cRedirArg g_nullArg;
	return (idx - 1 < m_args_n) ? (m_args + idx - 1) : &g_nullArg;
}

void cNodePrmArray::Reset()
{
	m_arg1.m_v.Clear();
	for(tDWORD i = 0; i < m_args_n; i++)
		m_args[i].m_v.Clear();
}

cNode::EnumRes cNodePrmArray::Enum(cNodeEnum *enumerator, type_t type)
{
	if( EnumRes res = cNode::Enum(enumerator, type) )
		return res == erSkipChilds ? erContinue : res;
	return EnumChildren(enumerator, type);
}

cNode::EnumRes cNodePrmArray::EnumChildren(cNodeEnum *enumerator, type_t type)
{
	EnumRes res;
	if( res = m_arg1.Enum(enumerator, type) )
		return res;

	for(tDWORD i = 0; i < m_args_n; i++)
		if( res = m_args[i].Enum(enumerator, type) )
			return res;
	
	return erContinue;
}

//////////////////////////////////////////////////////////////////////////
// cNodeFormat

cNodeFormat::cNodeFormat(cNode **args) : cNodeArgs<2>(args)
{
	if( m_args[0] && m_args[0]->Type() == ntString )
	{
		swap_objmem(m_fmt, ((cNodeStr *)m_args[0])->m_str);
		m_args[0]->Destroy(), m_args[0] = NULL;
		m_fmt.insert(0, "%");
	}
}

void cNodeFormat::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !get || !m_args[1] )
		return;
	
	cVar v; m_args[1]->Exec(v, ctx);
	
	tString &strValue = value.MakeString();
	strValue.resize(100);
	_snprintf(&strValue[0], strValue.size(), m_fmt.c_str(), v.ToInt());
	strValue.resize(strlen(strValue.c_str()));
}

//////////////////////////////////////////////////////////////////////////
// InitVectorBinder

static CFieldsBinder * InitVectorBinder(CRootItem *root, cNode *dataAcc)
{
	if( dataAcc->Type() == ntSerField )
	{
		cSerFieldAcc *vectorAcc = (cSerFieldAcc *)dataAcc;
		CFieldsBinder *parentBinder = vectorAcc->m_binder;
		const cSerDescriptorField *vectorField = vectorAcc->m_field;

		if( !IS_SERIALIZABLE(vectorField->m_id) || !(vectorField->m_flags & SDT_FLAG_VECTOR) )
			return NULL;

		return new CFieldsBinder(root, parentBinder, NULL, cSerializableObj::getDescriptor(vectorField->m_id));
	}

	if( dataAcc->Type() == ntMember )
	{
		cMemberAcc *vectorAcc = (cMemberAcc *)dataAcc;
		
		if( !IS_SERIALIZABLE(vectorAcc->m_type) || !(vectorAcc->m_flags & SDT_FLAG_VECTOR) )
			return NULL;

		return new CFieldsBinder(root, NULL, NULL, cSerializableObj::getDescriptor(vectorAcc->m_type));
	}
	
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// cNodeVectorFormat

bool cNodeVectorFormat::Init(cAlloc *al, CFieldsBinder * binder)
{
	m_binder = binder;
	if( !m_binder )
		return false;

	if( m_args[1]->Type() != ntString )
		return false;

	bool defaultSep = true;
	if( m_args[2] )
	{
		if( m_args[2]->Type() == ntString )
		{
			swap_objmem(m_separator, ((cNodeStr *)m_args[2])->m_str);
			defaultSep = false;
		}
		m_args[2]->Destroy(), m_args[2] = NULL;
	}
	if( defaultSep )
		m_separator = ", ";
	
	cNode *formatter = m_binder->m_pRoot->CreateNode(((cNodeStr *)m_args[1])->m_str.c_str(), m_binder);
	m_args[1]->Destroy(), m_args[1] = formatter;

	return !!formatter;
}

void cNodeVectorFormat::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !get || !m_binder )
	{
		value.Clear();
		return;
	}
	
	m_args[0]->Exec(value, ctx);
	const cVectorData *vd = value.c_vector();
	if( !vd )
	{
		value.Clear();
		return;
	}

	cVectorHnd vect; vect.attach(vd->m_v, vd->m_type, vd->m_flags);

	value.MakeString(); tString &strRes = value.strRef();
	
	CStructData data(NULL);
	cNodeExecCtx ec(NULL, NULL, &data);
	for(size_t i = 0, n = vect.size(); i < n; i++)
	{
		data.m_ser = (cSer *)vect.at(i);
		cVariant el; m_args[1]->Exec(el, ec); el.MakeString();
		
		const char *str = el.c_str();
		if( str && *str )
		{
			if( strRes.size() )
				strRes += m_separator;
			strRes += str;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// cNodeVectorSize

bool cNodeVectorSize::Init(CRootItem *root, cAlloc *al)
{
	if( !m_args[1] )
		return true;

	if( m_args[1]->Type() != ntString )
		return false;

	m_binder = InitVectorBinder(root, GetOrigNode(m_args[0]));
	if( !m_binder )
		return false;
	
	cNode *condition = m_binder->m_pRoot->CreateNode(((cNodeStr *)m_args[1])->m_str.c_str(), m_binder);
	m_args[1]->Destroy(), m_args[1] = condition;
	return !!m_args[1];
}

void cNodeVectorSize::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !get )
	{
		value.Clear();
		return;
	}
	
	m_args[0]->Exec(value, ctx);
	const cVectorData *vd = value.c_vector();
	if( !vd )
	{
		value.Clear();
		return;
	}
	
	cVectorHnd vect; vect.attach(vd->m_v, vd->m_type, vd->m_flags);
	
	if( !m_args[1] )
	{
		value = vect.size();
		return;
	}

	tDWORD nCount = 0;
	
	CStructData data(NULL);
	cNodeExecCtx ec(NULL, NULL, &data);
	for(size_t i = 0, n = vect.size(); i < n; i++)
	{
		data.m_ser = (cSer *)vect.at(i);
		m_args[1]->Exec(value, ec);
		if( value )
			nCount++;
	}

	value = nCount;
}

//////////////////////////////////////////////////////////////////////
// cNodeVectorAt

void cNodeVectorAt::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	m_args[0]->Exec(value, ctx);
	const cVectorData *vd = value.c_vector();
	if( !vd )
	{
		value.Clear();
		return;
	}
	cVectorHnd vect; vect.attach(vd->m_v, vd->m_type, vd->m_flags);

	m_args[1]->Exec(value, ctx);
	tDWORD idx = value.ToDWORD();
	
	if( idx >= vect.size() )
	{
		value.Clear();
		return;
	}

	value.Init((cSer *)vect.at(idx));
}

//////////////////////////////////////////////////////////////////////////
// cNodeVectorFind

bool cNodeVectorFind::Init(CItemBase *item, cAlloc *al)
{
	if( !m_args[1] || m_args[1]->Type() != ntString )
		return false;

	m_binder = InitVectorBinder(item->m_pRoot, GetOrigNode(m_args[0]));
	if( !m_binder )
		return false;

	cNode *condition = m_binder->m_pRoot->CreateNode(((cNodeStr *)m_args[1])->m_str.c_str(), m_binder, item);
	m_args[1]->Destroy(), m_args[1] = condition;
	return !!m_args[1];
}

void cNodeVectorFind::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( get )
	{
		m_args[0]->Exec(value, ctx);
		if( const cVectorData *vd = value.c_vector() )
		{
			cVectorHnd vect; vect.attach(vd->m_v, vd->m_type, vd->m_flags);

			CStructData data(NULL);
			cNodeExecCtx ec(ctx.m_root, NULL, &data);
			for(tSIZE_T i = 0, n = vect.size(); i < n; i++)
			{
				data.m_ser = (cSer *)vect.at(i);
				m_args[1]->Exec(value, ec);
				if( value )
				{
					value.Init(data.m_ser);
					return;
				}
			}
		}
	}

	value.Clear();
}


//////////////////////////////////////////////////////////////////////////
// cNodeTreeFind

bool cNodeTreeFind::Init(CItemBase *item, cAlloc *al)
{
	CRootItem *root = item->m_pRoot;

	if( getArgc() < 3 || m_args[1]->Type() != ntString || m_args[2]->Type() != ntString )
		return false;

	cSerFieldAcc *sf = (cSerFieldAcc *)m_args[0];
	if( sf->Type() == ntSerField )
	{
		tTYPE_ID tid = (tTYPE_ID)sf->m_field->m_id;
		if( IS_SERIALIZABLE(tid) )
			m_binder = new CFieldsBinder(root, item->GetBinder(), NULL, cSerializableObj::getDescriptor(tid));
	}
	if( !m_binder )
		return false;

	cNode *children = root->CreateNode(((cNodeStr *)m_args[1])->m_str.c_str(), m_binder, item);
	m_args[1]->Destroy(), m_args[1] = children;

	cNode *condition = root->CreateNode(((cNodeStr *)m_args[2])->m_str.c_str(), m_binder, item);
	m_args[2]->Destroy(), m_args[2] = condition;

	return m_args[1] && m_args[2];
}

void cNodeTreeFind::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	value.Clear();
	if( !get )
		return;

	if( cSer * root = const_cast<cSer *>(m_args[0]->ExecGet(ctx).c_ser()) )
		value.Init(Find(root, ctx));
}

cSer * cNodeTreeFind::Find(cSer *ser, cNodeExecCtx &ctx)
{
	m_binder->AddDataSource(ser);
	if( m_args[2]->ExecGet(ctx) )
	{
		m_binder->ClearSource(ser);
		return ser;
	}
	
	cVectorHnd children;
	if( const cVectorData *vd = m_args[1]->ExecGet(ctx).c_vector() )
		children.attach(vd->m_v, vd->m_type, vd->m_flags);

	m_binder->ClearSource(ser);

	if( size_t n = children.size() )
		for(size_t i = 0; i < n; i++)
			if( ser = Find((cSer *)children.at(i), ctx) )
				return ser;

	return NULL;
}

//////////////////////////////////////////////////////////////////////
// cNodeSwitch

static bool _IsKeyExpression(LPCSTR key)
{
	for( ; *key; key++)
	{
		if( !_kav_iscsym(*key) )
			return false;
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////

cNodeSwitch::cNodeSwitch(cNode **args, CItemBase *item, CFieldsBinder *binder, LPCSTR strSecton, bool bidirect) :
	m_item(item),
	m_binder(binder),
	m_defaultCase(NULL),
	m_inited(false),
	m_string(false),
	m_bidirect(bidirect)
{
	if( !args[0] )
		return;

	if( strSecton )
		m_section = strSecton;
	else if( args[1] && args[1]->Type() == ntString )
	{
		swap_objmem(m_section, ((cNodeStr *)args[1])->m_str);
		args[1]->Destroy();
		memmove(&args[1], &args[2], (NODE_MAX_ARGS + 1 - 2)*sizeof(args[0]));
	}

	cNodePrmArray::Init(args);
}

cNodeSwitch::~cNodeSwitch()
{
	for(size_t i = 0; i < m_cases.size(); i++)
	{
		cNode *node = m_cases[i].m_node;
		if( node && node != notInitialized && node != useNext )
			node->Destroy();
	}
	
	if( m_defaultCase && m_defaultCase != notInitialized && m_defaultCase != useNext )
		m_defaultCase->Destroy();
}

bool cNodeSwitch::Init(cNodeExecCtx &ctx)
{
	sIniSection * sect = ctx.m_root->GetIniSection(PROFILE_LOCALIZE, m_section.c_str());
	if( !sect || !sect->line_head )
	{
		m_inited = true;
		return false;
	}

	tString keyExpression;
	for(sIniLine * line = sect->line_head; line; line = line->next)
	{
		LPCSTR key = line->name;
		if( *key == '$' )
			continue;
		
		if( !strcmp(key, "default") || !strcmp(key, "def") )
		{
			m_defaultCase = (cNode *)notInitialized;
			continue;
		}

		bool bMacro = false;
		bool bExpr = false;
		
		if( strcmp(key, "<empty>") )
		{
			for(LPCSTR p = key; *p; p++) { if( *p == '#' ) { bMacro = true; break; } }
			if( bMacro )
			{
				keyExpression = key; m_binder->m_pRoot->_GetString(keyExpression, PROFILE_LOCALIZE, NULL, "", NULL/*m_pResolver*/);
				key = keyExpression.c_str();
			}
				
			for(LPCSTR p = key; *p; p++) { if( !_kav_iscsym(*p) ) { bExpr = true; break; } }
			if( bExpr )
			{
				CNodeFactory factory(m_item, m_binder);
				cNode * pNode = cNode::Parse(key, &factory, factory.m_pAlloc);
				if( pNode )
				{
					cVariant v; pNode->Exec(v, ctx);
					v.ToString(keyExpression);
					pNode->Destroy();
					key = keyExpression.c_str();
				}
			}
				
			if( !*key )
			{
				m_binder->m_pRoot->ReportError(prtERROR, "Parsing error: case %s of switch %s is invalid", line->name, sect->name);
				continue;
			}
		}
		
		cCaseItem& item = *m_cases.insert(m_cases.end(), cCaseItem());
		item.m_id = m_string ? (tQWORD)iCountCRC32str(key) : GetIntFromString(key);
		item.m_node = m_bidirect ? CreateCaseNode(line, ctx) : (cNode *)notInitialized;
	}

	m_inited = true;
	return true;
}

cNode * cNodeSwitch::CreateCaseNode(struct tagIniLine *line, cNodeExecCtx &ctx)
{
	if( !*line->value )
		return (cNode *)useNext;
	
	tString expression = line->value;
	m_binder->m_pRoot->_GetString(expression, PROFILE_LOCALIZE, m_section.c_str());

	// struct A { cSer * m_pB; }; в описателе тип B есть cSer::eIID
	// чтобы было возможно прибиндиться к полянкам, лежащим в структуре m_pB
	// нужно, чтобы биндер имел возможность адресовать эту структуру и получить
	// ее идентификатор.
	
	// ToDo: наверное, нужно в каждой ноде, которая сама создает дочерние ноды,
	// добавлять структуру в биндер на время создания дочерних нод.
	
	// чтобы иметь возможность прибиндиться к структурам разных типов
	// (в зависимости от ключа, по которому работает switch)
	// нужно, биндеру указать описатель соответствующей структуры

	const cSerDescriptor *originDesc = NULL;
	CStructData *data = ctx.m_data;
	if( data && data->m_ser && m_binder->m_pDesc && m_binder->m_pSections.empty() )
	{
		originDesc = m_binder->m_pDesc;
		m_binder->m_pDesc = cSerializableObj::getDescriptor(data->m_ser->getIID());
		m_binder->AddDataSource(data->m_ser);
	}
	
	CNf factory(this, m_item, m_binder);
	cNode *res = cNode::Parse(expression.c_str(), &factory, factory.m_pAlloc);
	
	if( originDesc )
	{
		m_binder->ClearSources();
		m_binder->m_pDesc = originDesc;
	}

	return res;
}

cNode * cNodeSwitch::PrepareNode(cNode *& node, size_t i, cNodeExecCtx &ctx)
{
	node = NULL;
	
	sIniSection * sect = ctx.m_root->GetIniSection(PROFILE_LOCALIZE, m_section.c_str());
	if( !sect )
		return node;

	sIniLine *line = sect->line_head;
	if( &node == &m_defaultCase )
	{
		line = IniGetLine(sect, "default", NULL);
		if( !line )
			line = IniGetLine(sect, "def", NULL);
	}
	else
		for(size_t idx = i; line; line = line->next)
		{
			if( *line->name == '$' )
				continue;
			
			if( !idx )
				break;
			idx--;
		}
	
	if( line )
		node = CreateCaseNode(line, ctx);
	return node;
}

cNode * cNodeSwitch::GetCaseNode(cNodeExecCtx &ctx)
{
	if( !m_arg1.m_arg )
		return NULL;
	
	cNodePrmArray::Reset();
	
	cVariant key; m_arg1.Exec(key, ctx);

	if( !m_inited )
	{
		if( key.Type() == cVariant::vtVoid )
			return NULL;

		m_string = key.Type() == cVariant::vtString;
		if( !Init(ctx) )
			return NULL;
	}
	
	tQWORD keyID;
	if( m_string )
	{
		const tString& keystr = variant_strref(key);
		keyID = keystr.empty() ? caseEmpty : iCountCRC32str(keystr.c_str());
	}
	else
		keyID = key.ToQWORD();

	cNode * node = NULL;

	size_t i, n = m_cases.size();
	for(i = 0; i < n; i++)
	{
		cCaseItem& item = m_cases[i];
		if( !node && item.m_id != keyID )
			continue;

		if( item.m_node == notInitialized )
			PrepareNode(item.m_node, i, ctx);

		node = item.m_node;
		if( !node )
			return NULL;

		if( node != useNext )
			break;
	}

	if( node == useNext )
		node = NULL;

	if( !node )
	{
		if( m_defaultCase == notInitialized )
			PrepareNode(m_defaultCase, 0, ctx);

		if( m_defaultCase != useNext )
			node = m_defaultCase;
	}

	return node;
}

void cNodeSwitch::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !m_arg1.m_arg )
		return;
	
	if( m_bidirect && !get )
	{
		if( !m_inited )
		{
			cVariant key; m_arg1.Exec(key, ctx);
			if( key.Type() == cVariant::vtVoid )
				return;
			
			m_string = key.Type() == cVariant::vtString;
			if( !Init(ctx) )
				return;
		}

		ExecBiSet(value, ctx);
		return;
	}

	if( cNode * pCaseNode = GetCaseNode(ctx) )
		pCaseNode->Exec(value, ctx, get);
	else
		value.Clear();
}

void cNodeSwitch::ExecBiSet(cVariant &value, cNodeExecCtx &ctx)
{
//	value.Clear();
	
	for(size_t i = 0; i < m_cases.size(); i++)
	{
		cCaseItem& item = m_cases[i];
		if( !item.m_node || item.m_node == useNext )
			continue;
		
		cVariant rVal; item.m_node->Exec(rVal, ctx);
		if( rVal == value )
		{
			// Временно возвращаем всегда число, пока нет кейсов бидирект строковых свичей
			value = item.m_id;
			break;
		}
	}

	m_arg1.Exec(value, ctx, false);
}

cNode::EnumRes cNodeSwitch::Enum(cNodeEnum *enumerator, type_t type)
{
	EnumRes res = erContinue;
	if( res = cNodePrmArray::Enum(enumerator, type) )
		return res;

	size_t i, n = m_cases.size();
	for(i = 0; i < n; i++)
	{
		cNode *node = m_cases[i].m_node;
		if( node && node != notInitialized && node != useNext )
			if( res = node->Enum(enumerator, type) )
				return res;
	}
	
	if( m_defaultCase && m_defaultCase != notInitialized && m_defaultCase != useNext )
		if( res = m_defaultCase->Enum(enumerator, type) )
			return res;

	return res;
}

cNode::EnumRes cNodeSwitch::EnumChildren(cNodeEnum *enumerator, type_t type, cNodeExecCtx &ctx)
{
	EnumRes res = erContinue;
	if( res = cNodePrmArray::EnumChildren(enumerator, type) )
		return res;

	if( cNode * pNodeCase = GetCaseNode(ctx) )
		res = pNodeCase->Enum(enumerator, type);
	return res;
}

//////////////////////////////////////////////////////////////////////////
// cNodeBindok

cNodeBindok::cNodeBindok(cNode **args, CRootItem *root, CFieldsBinder *binder) :
	cNodeArgs<1>(args)
{
	if( m_args[0]->Type() != ntString || !root || !binder )
		return;
	
	tString fieldPath; swap_objmem(fieldPath, ((cNodeStr *)m_args[0])->m_str);
	if( fieldPath.size() )
		m_pNode = root->CreateNode(fieldPath.c_str(), binder);
}

void cNodeBindok::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !get )
	{
		value.Clear();
		return;
	}

	if( m_pNode )
	{
		m_pNode->Exec(value, ctx);
		value = value.Type() != cVar::vtVoid;
	}
	else
		value = false;
}

//////////////////////////////////////////////////////////////////////
// cNodeInvert

void cNodeInvert::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( get )
	{
		m_args[0]->Exec(value, ctx, true);
		value = !value;
	}
	else
	{
		value = !value;
		m_args[0]->Exec(value, ctx, false);
	}
}

//////////////////////////////////////////////////////////////////////
// cNodeMask

cNodeMask::cNodeMask(cNode **args, bool bBool) : cNodeArgs<2>(args), m_mask(0), m_bool(bBool) {}

void cNodeMask::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !m_mask && m_args[1] )
	{
		cVariant mask; m_args[1]->Exec(mask, ctx);
		m_mask = mask.ToQWORD();

		m_args[1]->Destroy();
		m_args[1] = NULL;
	}
	
	if( get )
	{
		m_args[0]->Exec(value, ctx, true);
		value = value.ToQWORD() & m_mask;
		if( m_bool )
			value = (bool)value;
	}
	else
	{
		tQWORD newValue = m_bool ? (value ? m_mask : 0) : (value.ToQWORD() & m_mask);

		m_args[0]->Exec(value, ctx, true);
		value = (value.ToQWORD() & ~m_mask)|newValue;
		m_args[0]->Exec(value, ctx, false);
	}
}

//////////////////////////////////////////////////////////////////////
// cNodeShift

void cNodeShift::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( m_args[1] )
	{
		cVariant distance; m_args[1]->Exec(distance, ctx);
		m_distance = distance.ToDWORD();

		m_args[1]->Destroy();
		m_args[1] = NULL;
	}

	if( get )
	{
		m_args[0]->Exec(value, ctx, true);
		value >>= m_distance;
	}
	else
	{
		value <<= m_distance;
		m_args[0]->Exec(value, ctx, false);
	}
}

//////////////////////////////////////////////////////////////////////
// cNodeGetIcon

cNodeGetIcon::cNodeGetIcon(CRootItem *root, cNode **args, bool sync) :
	cNodeArgs<4>(args),
	m_sync(sync),
	m_large(false)
{
	if( cNode *&defIcon = m_args[1] )
	{
		if( defIcon->Type() == ntString )
		{
			m_defIcon = root->GetIcon(((cNodeStr *)defIcon)->m_str.c_str());
			if( m_defIcon )
				m_defIcon->AddRef();
			defIcon->Destroy(), defIcon = NULL;
		}
	}

	if( cNode *&largeIcon = m_args[2] )
	{
		if( largeIcon->Type() == ntNumber )
			m_large = !!((cNodeInt *)largeIcon)->m_val;
		largeIcon->Destroy(), largeIcon = NULL;
	}

	if( cNode *&asyncIcon = m_args[3] )
	{
		m_sync = !asyncIcon->ExecGet(TOR(cNodeExecCtx, (root)));
		asyncIcon->Destroy(), asyncIcon = NULL;
	}
}

void cNodeGetIcon::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !get )
		return;

	if( !ctx.m_item )
		return;

	m_args[0]->Exec(value, ctx);
	if( value )
		value.Init(ctx.m_root->ExtractIcon(value.c_str(), 0, m_large ? OBJINFO_LARGEICON : 0, m_sync ? NULL : ctx.m_item, true, m_defIcon));
	else
		value.Init(m_defIcon);
}

//////////////////////////////////////////////////////////////////////
// cNodeMergeIcon

FUNC_NODE_CONST(cNodeMergeIcon, 2)
{
	CImageBase * pImageSrc[countof(m_args)] = {NULL, };
	tDWORD nIdx[countof(pImageSrc)] = {0, };
	tString strImageId;
	
	for(tDWORD i = 0; i < countof(pImageSrc); i++)
	{
		if( !m_args[i] )
			break;
		
		cVar vImage; m_args[i]->Exec(vImage, ctx);
		if( vImage.Type() == cVar::vtIco )
			pImageSrc[i] = vImage.c_ico();
		else if( vImage.Type() == cVar::vtString )
		{
			CItemPropVals prpImage; prpImage.Parse(vImage.c_str(), 0, true);
			pImageSrc[i] = ctx.m_root->GetIcon(prpImage.GetStr());
			nIdx[i] = (tDWORD)prpImage.GetLong(1);
		}

		if( !pImageSrc[i] )
			continue;

		if( strImageId.size() )
			strImageId += ",";
		strImageId += ctx.m_root->GetIconKey(pImageSrc[i]);
		
		if( nIdx[i] )
		{
			tCHAR szTmp[20]; _snprintf(szTmp, countof(szTmp), ".%u", nIdx[i]);
			strImageId += szTmp;
		}
	}

	if( strImageId.empty() )
	{
		value.Clear();
		return;
	}
	
	CImageBase * pImage = ctx.m_root->GetIcon(strImageId.c_str());
	if( !pImage )
	{
		pImage = new CMergedImage();
		for(tDWORD i = 0; i < countof(pImageSrc); i++)
			pImage->Merge(pImageSrc[i], nIdx[i]);
		ctx.m_root->PutIconInCache(strImageId.c_str(), pImage);
	}
	
	value.Init(pImage);
}

//////////////////////////////////////////////////////////////////////
// cNodeGetDisplayName

void cNodeGetDisplayName::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !get )
		return;

	m_args[0]->Exec(value, ctx);
	if( !value )
		return;

	CObjectInfo oi;
	value.ToStringObj(oi.m_strName);
	oi.m_nQueryMask = m_desctiption ? OBJINFO_DESCRIPTION : OBJINFO_DISPLAYNAME;
	if( ctx.m_root->GetObjectInfo(&oi) )
		value = m_desctiption ? oi.m_strDescription.data() : oi.m_strDisplayName.data();
}

//////////////////////////////////////////////////////////////////////
// cNodeTime

extern LPCSTR GetDateTimeStr(CRootItem* pRoot, cDateTime& prDt, tString& str, DTT_TYPE eType, bool bSeconds, bool bLocal, LPCSTR strEstDaysFmt);

void cNodeTime::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !get )
		return;

	m_args[0]->Exec(value, ctx, get);

	cDateTime prDt(cDateTime::zero);

	time_t tTime = (time_t)value.ToInt();
	switch(m_dttType)
	{
	case DTT_TIME:
	case DTT_TIMESHORT:
	case DTT_TIMESTAMP:
		if( IS_TIME_VALID(tTime) )
			prDt = tTime;
		break;

	default:
		if( IS_DATETIME_VALID(tTime) )
			prDt = tTime;
	}

	value.MakeString();

	CRootItem * pRoot = ctx.m_root;
	if( prDt == cDateTime::nil() )
		pRoot->GetFormatedText(value.strRef(), pRoot->GetString(TOR_tString, PROFILE_LOCALIZE, NULL, "TimeUnknown"), (cSer *)NULL);
	else
		GetDateTimeStr(pRoot, prDt, value.strRef(), m_dttType, !!m_seconds, !!m_local, "EstimatedDays");
};

//////////////////////////////////////////////////////////////////////
// cNodeDateGroup

cNodeDateGroup::cNodeDateGroup(cNode **args) : cNodeArgs<1>(args)
{
	m_now.Init(cDateTime::current_local);
	m_weekno = m_now.WeekNo(cDateTime::tz_utc, cFALSE);
}

void cNodeDateGroup::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !get )
		return;

	m_args[0]->Exec(value, ctx, get);

	cDateTime date((time_t)value.ToDWORD());

	tDWORD dwYear, dwMonth, dwDay;
	bool bNegative = false;
	if( m_now.DiffGet(date, &dwYear, &dwMonth, &dwDay) == errDT_DIFF_NEGATIVE )
	{
		bNegative = true;
		date.DiffGet(m_now, &dwYear, &dwMonth, &dwDay);
	}

	tLONG weekdiff = m_weekno - date.WeekNo(cDateTime::tz_utc, cFALSE);

	tLONG ret = 0;
	if( dwYear || dwMonth > 1 )
		ret = 1000;
	else if( dwMonth == 1 || weekdiff > 3 )
		ret = 100;
	else if( weekdiff == 3 )
		ret = 13;
	else if( weekdiff == 2 )
		ret = 12;
	else if( weekdiff == 1 )
		ret = 11;
	else if( !dwDay )
		ret = 0;
	else if( dwDay == 1 )
		ret = 1;
	else
		ret = date.WeekDay(cDateTime::tz_utc, cFALSE) + 2;

	if( bNegative )
		ret = -ret;

	value = (tQWORD)-ret;
}

//////////////////////////////////////////////////////////////////////
// cNodeIterate

void cNodeIterate::Exec(cVariant& value, cNodeExecCtx &ctx, bool get)
{
	if( !get )
		return;

	if( !m_args[0] )
		return;
	
	cVariant cur; m_args[0]->Exec(cur, ctx, true);

	cVariants args;
	{
		size_t argc = 0;
		for(cNode **a = m_args + 1; *a; a++, argc++);

		if( argc )
		{
			args.resize(argc);
			for(size_t i = 0; i < args.size(); i++)
				m_args[i + 1]->Exec(args[i], ctx);
		}
		else
		{
			args.resize(2);
			args[0] = false;
			args[1] = true;
		}
	}

	size_t i = 0;
	for(; i < args.size(); i++)
		if( cur == args[i] )
			break;

	m_args[0]->Exec((i + 1 < args.size()) ? args[i + 1] : args[0], ctx, false);
}

//////////////////////////////////////////////////////////////////////////
// cNodeCrypt

void cNodeCrypt::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	CRootSink *sink = (CRootSink *)ctx.m_root->m_pSink;
	if( !sink )
		return;
	
	if( !m_bidirect )
	{
		if( !get )
			return;
		
		m_args[0]->Exec(value, ctx, get);
		
		cStringObj s; value.ToStringObj(s);
		if( sink->CryptDecrypt(s, true) )
			value = s.data();
		return;
	}
	
	if( get )
	{
		m_args[0]->Exec(value, ctx, get);

		if( ((CRootItem *)ctx.m_root)->m_nGuiFlags & GUIFLAG_NOCRYPT )
		{
			if( ctx.m_item )
				ctx.m_item->Enable(false);

			value = "";
		}
		else
		{
			cStringObj s; value.ToStringObj(s);
			if( sink->CryptDecrypt(s, false) )
				value = s.data();
		}
	}
	else
	{
		cStringObj s; value.ToStringObj(s);
		if( sink->CryptDecrypt(s, true) )
			value = s.data();

		m_args[0]->Exec(value, ctx, get);
	}
}

//////////////////////////////////////////////////////////////////////////
// cNodeCryptCmp

void cNodeCryptCmp::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	CRootSink *sink = (CRootSink *)ctx.m_root->m_pSink;
	if( !get || !sink )
		return;

	tString pswd;
	m_args[0]->Exec(value, ctx, get);
	cStringObj temp;
	value.ToStringObj(temp);
	sink->CryptDecrypt(temp, false);
	utf16_to_utf8(temp.data(), temp.size(), pswd);

	m_args[1]->Exec(value, ctx, get);

	value = variant_strref(value) == pswd;
}

//////////////////////////////////////////////////////////////////////////
// cNodeFileSize

cNodeFileSize::cNodeFileSize(cNode **args, CRootItem * root) :
	cNodeArgs<2>(args)
{
	if( !m_args[0] || !m_args[1] )
		return;

	if( m_args[1]->Type() != ntString )
	{
		m_args[0]->Destroy(); m_args[0] = NULL;
		m_args[1]->Destroy(); m_args[1] = NULL;
		return;
	}

	sIniSection * sect = root->GetIniSection(PROFILE_LOCALIZE, ((cNodeStr *)m_args[1])->m_str.c_str());
	m_args[1]->Destroy(); m_args[1] = NULL;

	if( sect )
		for(sIniLine * line = sect->line_head; line; line = line->next)
		{
			tString& str = m_loc.push_back();
			str = line->value;
			root->_GetString(str, PROFILE_LOCALIZE, sect->name);
			
			if( str.size() && str[0] == '\"' )
				str.erase(0, 1);
			
			if( str.size() && str[str.size() - 1] == '\"' )
				str.erase(str.size() - 1, 1);
		}
}
	
void cNodeFileSize::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !get || !m_args[0] )
		return;
	
	cVariant nVal; m_args[0]->Exec(nVal, ctx);
	
	tLONGLONG nSize	= nVal.ToInt();
	tDOUBLE dValue	= (tDOUBLE)nSize;
	
	value.MakeString();
	tString &strValue = value.strRef();
	strValue.resize(MAX_FLOAT_STRSIZE + 3);

	if( m_loc.size() )
	{
		tDWORD i = 0, n = m_loc.size();
		for(; i + 1 < n; i++)
		{
			if( (dValue / 1024.0F ) < 1.0F )
				break;
			dValue /= 1024.0F;
		}
		
		tString& strLoc = m_loc[i];
		if( dValue - (tLONGLONG)dValue >= 0.1F )
			_snprintf(&strValue[0], strValue.size(), "%.1f %s", dValue, strLoc.c_str());
		else
			_snprintf(&strValue[0], strValue.size(), PRINTF_LONGLONG " %s", (tLONGLONG)dValue, strLoc.c_str());
	}
	else
		_snprintf(&strValue[0], strValue.size(), "%d", nSize);

	strValue.resize(strlen(strValue.c_str()));
}

//////////////////////////////////////////////////////////////////////////
// cNodeSkinFunc

bool cNodeSkinFunc::Init(cNode **args, cAlloc *al, CRootItem *root, CItemBase *item, CFieldsBinder *binder, const char *name)
{
	sIniSection * sect = root->GetIniSection(PROFILE_LOCALIZE, name);
	if( !sect )
		return false;

	sIniLine * lineBody = IniGetLine(sect, "ret");
	if( !lineBody )
		return false;
	
	sIniLine * lineArgs = sect->line_head;
	if( lineArgs == lineBody )
		lineArgs = NULL;
	
	cNodePrmArray::Init(args);
	
	tString expression = lineBody->value; root->_GetString(expression, PROFILE_LOCALIZE, name);

	CNf factory(this, item, binder, lineArgs ? lineArgs->name : NULL);
	m_pNode = cNode::Parse(expression.c_str(), &factory, al);
	return !!m_pNode;
}

void cNodeSkinFunc::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	cNodePrmArray::Exec(value, ctx, get);
	if( m_pNode ) m_pNode->Exec(value, ctx, get);
}

cNode::EnumRes cNodeSkinFunc::Enum(cNodeEnum *enumerator, type_t type)
{
	if( EnumRes res = cNodePrmArray::Enum(enumerator, type) )
		return res;
	return m_pNode ? m_pNode->Enum(enumerator, type) : erContinue;
}

//////////////////////////////////////////////////////////////////////////
// cNodeSer

void cNodeSer::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !get )
		return;

	const cSer * ser = NULL;
	if( m_args[0]->Type() == ntSerField )
		ser = ((cSerFieldAcc *)m_args[0])->GetStruct(ctx);
	else if( ctx.m_data )
		ser = ctx.m_data->m_ser;

	value.Init(ser);
}

//////////////////////////////////////////////////////////////////////////
// cNodeSerId

void cNodeSerId::Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
{
	if( !get )
		return;
	
	const cSer * ser = NULL;
	if( m_args[0] )
	{
		m_args[0]->Exec(value, ctx);
		ser = value.c_ser();
	}
	else if( ctx.m_data )
		ser = ctx.m_data->m_ser;
	
	if( ser )
		value = (tDWORD)ser->getIID();
	else
		value = (tDWORD)cSer::eIID;
}

void cGlobalVar::Exec(cVariant &result, cNodeExecCtx &ctx, bool get)
{
#ifdef _DEBUG
	LPCSTR strName = "";
	CRootItem::tMapVariants &vars = ctx.m_root->m_mapVariants;
	for(CRootItem::tMapVariants::iterator i = vars.begin(); i != vars.end(); i++)
	{
		if( &i->second == &m_var )
		{
			strName = i->first.c_str();
			break;
		}
	}
#endif

	if( get )
		result = m_var;
	else
		m_var = result;
} 

//////////////////////////////////////////////////////////////////////
// cNodeAddtext

/*COMMAND_ITEM(cNodeAddtext)
{
	if( bGuiController && GUI_COM_RO(this) ) return errNOT_OK;

	tString strTextId;
	CGuiPtr<CItemProps> pProps = CreateProps(strParams, strTextId);
	LPCSTR strItemPath = pProps->Get("id").GetStr();
	
	if( !strItemPath || !*strItemPath || strTextId.empty() )
		return errPARAMETER_INVALID;

	CEditItem * pEdit = (CEditItem *)_GetItemByPath(strItemPath, pItem);
	if( !pEdit || pEdit->m_strItemType != GUI_ITEMT_EDIT )
		return errOBJECT_NOT_FOUND;

	pEdit->AddText(pItem->LoadLocString(TOR_tString, strTextId.c_str()));
	return errOK;
}*/

//////////////////////////////////////////////////////////////////////////
// Simple nodes

class cNodeShowWindow : public cNodeArgs<NODE_MAX_ARGS>
{
public:
	cNodeShowWindow(cNode **args, CItemBase * item, bool modal = true, bool singleton = true) : cNodeArgs<NODE_MAX_ARGS>(args), m_item(item), m_modal(modal), m_singleton(singleton) {}
	
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		value.Clear();

		if( !m_args[0] )
			return;

		CItemBase * pOwner = m_modal ? ctx.m_item->GetOwner(true) : ctx.m_item->m_pRoot;
		if( !pOwner )
			return;

		// В случае немодального окна предотвращаем использование данных (синков, элементов, ...) не от рута
		CSinkCreateCtx sinkCrCtx(m_modal ? ctx.m_item : ctx.m_item->m_pRoot, NULL, m_args, getArgc(), &ctx);

		tDWORD nLoadFlags = 0;
		tString strPrm; FUNC_NODE_PRM(0).ToString(strPrm);
		LPSTR strSection = (LPSTR)strPrm.c_str();
		
		tString strId;
		if( m_args[1] )
			FUNC_NODE_PRM(1).ToString(strId);

		if( strchr(strSection, '[') )
			nLoadFlags = LOAD_FLAG_INFO;

		LPSTR strSelectPages = strchr(strSection, ':');
		if( strSelectPages )
			*strSelectPages = 0, strSelectPages++;
		else
			strSelectPages = "";

		CDialogItem * pItem = NULL;
		if( m_singleton )
		{
			for(tDWORD i = 0; i < pOwner->m_aItems.size(); i++)
			{
				CItemBase * p = pOwner->m_aItems[i];
				if( !(p->m_nFlags & STYLE_TOP_ITEM) )
					continue;
				if( p->m_strSection != strSection )
					continue;
				if( strId.size() && p->m_strItemId != strId )
					continue;
				
				pItem = (CDialogItem *)p;
				break;
			}
		}
		
		if( !pItem )
			pOwner->LoadItem(NULL, (CItemBase *&)pItem, strSection, strId.c_str(), nLoadFlags, &sinkCrCtx);
		if( !pItem )
			return;

		if( pItem->m_strItemType != GUI_ITEMT_DIALOG )
		{
			pItem->Destroy();
			return;
		}

		if( *strSelectPages )
		{
			pItem->LockUpdate(true);
			
			for(CItemBase * pSheetArea = pItem; ;)
			{
				LPSTR str = strchr(strSelectPages, ':');
				if( str )
					*str = 0, str++;

				CItemBase * pSheet = pSheetArea->GetByFlagsAndState(STYLE_SHEET_PAGES);
				if( pSheet )
				{
					CItemBase * pPageSelector = pSheetArea->GetItem(strSelectPages);
					if( pPageSelector )
						pPageSelector->Select();

					pSheetArea = pSheet->GetItem(strSelectPages, NULL, false);
					if( !pSheetArea )
						break;
					
					pSheetArea->Select();
				}
				else
				{
					pSheet = pSheetArea->GetByFlagsAndState(STYLE_ENUM);
					if( !pSheet )
						break;

					pSheetArea = pSheet->GetItem(strSelectPages);
					if( !pSheetArea )
						break;

					CItemBase * pEnumVal = pSheetArea->GetByFlagsAndState(STYLE_ENUM_VALUE);
					if( !pEnumVal )
						break;

					pEnumVal->Select();
				}

				if( !str )
					break;
				
				strSelectPages = str;
			}
			
			pItem->LockUpdate(false);
		}

		pItem->Activate();

		if( m_modal )
			value = pItem->DoModal() == DLG_ACTION_OK;
	}

	CItemBase * m_item;
	unsigned    m_modal : 1;
	unsigned    m_singleton : 1;
};

class cNodeMessage : public cNodeArgs<2>
{
public:
	cNodeMessage(cNode **args, CItemBase * item, bool balloon = false) : cNodeArgs<2>(args), m_item(item), m_balloon(balloon) {}

	bool IsConst() const { return false; }
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		cVariant vMsgId; if( m_args[0] ) m_args[0]->Exec(vMsgId, ctx);

		if( m_balloon )
		{
			ctx.m_root->ShowBalloon(ctx.m_item, vMsgId.ToString().c_str());
			value = (tDWORD)0;
			return;
		}

		cVariant vMsgPrm; if( m_args[1] ) m_args[1]->Exec(vMsgPrm, ctx);

		tDWORD nMsgPrm = vMsgPrm.ToDWORD();

		tDWORD nRes = (tDWORD)ctx.m_root->ShowMsgBox(ctx.m_item, vMsgId.ToString().c_str(), NULL, nMsgPrm);
		switch(nRes)
		{
		case DLG_ACTION_OK:		value = ((nMsgPrm & ~MB_OK) & MB_TYPEMASK) ? (tDWORD)1 : (tDWORD)0; break;
		case DLG_ACTION_YES:	value = (tDWORD)1; break;
		case DLG_ACTION_NO:		value = (tDWORD)2; break;
		default:				value = (tDWORD)0;
		}
	}
	
	CItemBase * m_item;
	unsigned    m_balloon : 1;
};

FUNC_NODE_CONST(cNodeClose, 1)
{
	if( !ctx.m_item )
		return;

	CDialogItem * pDialog = (CDialogItem *)ctx.m_item->GetOwner(true);
	if( pDialog && pDialog->m_strItemType == GUI_ITEMT_DIALOG )
	{
		if( m_args[0] )
		{
			m_args[0]->Exec(value, ctx);
			if( value.ToDWORD() == DLG_ACTION_OK )
				pDialog->Close(DLG_ACTION_OK);
		}
		pDialog->CloseConfirm();
	}
}

// FUNC_NODE_CONST(cNodeMinimize)
// {
// 	if( GUI_CHK_COM() ) return errNOT_OK;
// 
// 	CItemBase * pTop = ctx.m_item ? ctx.m_item->GetOwner(true) : NULL;
// 	if( !pTop )
// 		return errNOT_OK;
// 
// 	((CDialogItem *)pTop)->SetShowMode(CDialogItem::showMinimized);
// 	return errOK;
// }

FUNC_NODE_CONST(cNodeMenu, NODE_MAX_ARGS)
{
	ctx.m_item->LoadItemMenu(NULL, LOAD_MENU_FLAG_SHOW|LOAD_MENU_FLAG_DESTROY|LOAD_MENU_FLAG_BOTTOMOFITEM, NULL, &CSinkCreateCtx(ctx.m_item, NULL, m_args, getArgc(), &ctx));
}

FUNC_NODE(cNodeUserName, 1)
{
	if( !m_args[0] )
		return;
	
	m_args[0]->Exec(value, ctx);

	CObjectInfo _UsrInfo(value.c_str(), SHELL_OBJTYPE_USERACCOUNT);
	cStrObj& res = ctx.m_root->GetObjectInfo(&_UsrInfo) ? _UsrInfo.m_strDisplayName : _UsrInfo.m_strName;
	value = res.data();
}

FUNC_NODE_CONST(cNodeWildcard, 2)
{
	if( !m_args[0] )
		return;
	
	cVariant varPattern; 

	m_args[0]->Exec(varPattern, ctx);
	if( 0 == varPattern.strRef().length())
	{
		value = (tDWORD)1;
		return;
	}
	
	cVariant varName;
	m_args[1]->Exec(varName, ctx);

	value = MatchWithPattern(varPattern.c_str(), varName.c_str(), false) ? (tDWORD)1 : (tDWORD)0;
}

FUNC_NODE_CONST(cNodeUrl, 1)
{
	if( m_args[0] ) ctx.m_root->OpenURL(FUNC_NODE_PRM(0).c_str());
}

FUNC_NODE_CONST(cNodeHelp, 1)
{
//	if( bGuiController && GUI_COM_RO(this) ) return errNOT_OK;

	tWORD nHelpId = m_args[0] ? (tWORD)FUNC_NODE_PRM(0).ToDWORD() : 0;
	if( !nHelpId && ctx.m_item )
	{
		CItemBase * pHelpOwner = ctx.m_item->GetOwner(true);
		if( pHelpOwner )
		{
			if( CItemBase * pSheet = pHelpOwner->GetByFlagsAndState(STYLE_SHEET_ITEM) )
				if( CItemBase * pSheetPage = pSheet->GetByFlagsAndState(STYLE_ALL, ISTATE_SELECTED, NULL, CItemBase::gbfasNoRecurse) )
					pHelpOwner = pSheetPage;

			if( !pHelpOwner->m_pSink || !pHelpOwner->m_pSink->OnGetHelpId(nHelpId) )
				nHelpId = pHelpOwner->m_nHelpId;
		}
	}
	
	if( ctx.m_root->m_pSink )
		((CRootSink *)ctx.m_root->m_pSink)->OnGetHelpId(nHelpId);
	
	value = ctx.m_root->ShowContextHelp(nHelpId, ctx.m_item);
}

FUNC_NODE_CONST(cNodeSound, 1)
{
	value = ctx.m_root->DoSound(m_args[0] ? FUNC_NODE_PRM(0).c_str() : NULL);
}

struct cNodeMail : public cNodeArgs<1>
{
	cNodeMail(cNode **args = NULL) : cNodeArgs<1>(args) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		if( !get || !m_args[0] )
			return;
		
		tString sMailto; FUNC_NODE_PRM(0).ToString(sMailto);

		CSendMailInfo pInfo;
		pInfo.m_aAddresses.push_back(GetAddress(sMailto).c_str());
		pInfo.m_strSubject             = GetHeader(sMailto, "subject").c_str();
		pInfo.m_strBody                = GetHeader(sMailto, "body").c_str();
			
		tERROR err = errOK;
		
		if( ctx.m_item )
		{
			/*cAttachFiles Attaches;
			Attaches.m_pItem      = ctx.m_item;
			Attaches.m_aAattaches = &pInfo.m_aAattaches;
			Attaches.m_err        = errOK;
			ctx.m_item->SendEvent(EVENTID_ATTACH_FILES, &Attaches, false);
			err = Attaches.m_err;*/
			
			if( PR_SUCC(err) )
				err = ctx.m_root->SendMail(pInfo, ctx.m_item);
		}
		else
			err = ctx.m_root->SendMail(pInfo, ctx.m_root);

		if( PR_FAIL(err) )
		{
			PR_TRACE((g_root, prtERROR, "gui\tBaseGui::NodeMail. SendMail returned %terr", err));
		}

		value = (tDWORD)err;
	}

	static tString GetAddress(tString &sMailto)
	{
		UINT nPos = sMailto.find('?');
		return nPos == (UINT)-1 ? sMailto : sMailto.substr(0, nPos);
	}

	static tString GetHeader(tString &sMailto, tString sHeader)
	{
		sHeader += '=';

		UINT nPos = sMailto.find(sHeader);
		if(nPos == (UINT)-1)
			return tString();
		nPos += sHeader.size();

		UINT nEnd = sMailto.find('&', nPos);
		if(nEnd == (UINT)-1)
			nEnd = sMailto.size();

		return sMailto.substr(nPos, nEnd - nPos);
	}
};

//////////////////////////////////////////////////////////////////////////

class cNodeFormatStruct : public cNodeArgs<1>
{
public:
	cNodeFormatStruct(cNode **args, CRootItem* pRoot, CFieldsBinder* pBinder) : cNodeArgs<1>(args), m_pNode(NULL)
	{
		m_pBinder = pRoot->CreateFieldsBinder();
		m_pBinder->m_pParent = pBinder;

		if( args[1]->Type() == ntString )
		{
			tString sFmt = ((cNodeStr *)args[1])->m_str;
			sFmt.insert(sFmt.begin(), '$');
			pRoot->GetString(m_sFormat, PROFILE_LOCALIZE, NULL, sFmt.c_str());		
			args[1]->Destroy(), args[1] = NULL;
		}
	}

	~cNodeFormatStruct()
	{
		if( m_pBinder ) m_pBinder->Destroy();
		if( m_pNode )   m_pNode->Destroy();
	}

	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		m_args[0]->Exec(value, ctx, get);

		cSer* pSer = (cSer*)value.c_ser();
		if( !m_pNode && pSer )
		{
			m_pBinder->AddDataSource(pSer);
			m_pNode = ctx.m_root->CreateNode(m_sFormat.c_str(), m_pBinder);
		}

		if( !m_pNode )
			return;
		
		CStructData pData(pSer);
		cNodeExecCtx ctx1(ctx);
		ctx1.m_data = &pData;
		m_pNode->Exec(value, ctx1, get);
	}

private:
	CFieldsBinder*       m_pBinder;
	cNode*               m_pNode;
	tString              m_sFormat;
};

//////////////////////////////////////////////////////////////////////////
// cNodePath

extern void    ConvertPath(PATHOP_TYPE eType, LPCSTR strText, tString& strRes);
extern tCHAR * CompressPath(const tCHAR *pStr, int len, bool bExclusive = true, tCHAR *pDst = NULL, DWORD dwDstLen = 0);

struct cNodePath : public cNodeArgs<1>
{
	cNodePath(cNode **args, PATHOP_TYPE eType = PATHOP_ORIGINAL, bool bCompact = false) : cNodeArgs<1>(args), m_eType(eType), m_bCompact(bCompact) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		if( !get || !m_args[0] )
			return;
		
		tString &strValue = value.MakeString();

		ConvertPath(m_eType, FUNC_NODE_PRM(0).c_str(), strValue);
		if( m_bCompact )
		{
			LPCSTR strC = CompressPath(strValue.c_str(), 50);
			strValue = strC ? strC : "";
		}
	}

	PATHOP_TYPE m_eType;
	unsigned    m_bCompact : 1;
};

//////////////////////////////////////////////////////////////////////////

void CRootItem::cCache::get(cVariant &value, cNodeExecCtx &ctx, cNode** args)
{
	cVariant val;
	args[0]->Exec(val, ctx);
	tQWORD key = val.ToQWORD();
	if( !key )
		return;

	cItem *m = m_d, *e = (cItem *)((char*)m_d + sizeof(m_d));
	tDWORD tmMin = 0xFFFFFFFF;
	for(cItem* p = m_d; p < e; p++)
	{
		tQWORD k = p->m_key;

		if( p->m_hit < tmMin )
			tmMin = p->m_hit, m = p;

		if( !k )
			break;

		if( k == key )
		{
			p->m_hit = PrGetTickCount();
			value = p->m_val;
			return;
		}
	}

	m->m_key = key;
	m->m_hit = PrGetTickCount();
	args[1]->Exec(m->m_val, ctx);

	if( m->m_val.Type() == cVariant::vtSer )
	{
		m->m_obj.assign(m->m_val.c_ser());
		m->m_val.Init(m->m_obj.ptr_ref());
	}

	value = m->m_val;
}

//////////////////////////////////////////////////////////////////////////
// CNodeFactory

const char *strchrlast(const char *str, char ch)
{
	const char *res = NULL;
	for(; str = strchr(str, ch); str++)
		res = str;
	return res;
}

//////////////////////////////////////////////////////////////////////

CNodeFactory::CNodeFactory(CItemBase *pItem, CFieldsBinder *pBinder) :
	m_pItem(pItem),
	m_pBinder(pBinder),
	m_pItemPropAcc(NULL)
{
	if( !m_pBinder && m_pItem )
		m_pBinder = m_pItem->GetBinder();
	m_pRoot = m_pBinder->m_pRoot;
	m_pAlloc = NULL;
}

cNode * CNodeFactory::CreateOperator(const char *name, cNode **args, cAlloc *al)
{
	sswitch(name)
	scase("const")                  return new cNodeConst(args); sbreak;
	scase("invert")	                return new cNodeInvert(args); sbreak;
	scase("mask")	                return new cNodeMask(args); sbreak;
	scase("bmask")	                return new cNodeMask(args, true); sbreak;
	scase("shift")	                return new cNodeShift(args); sbreak;
	scase("switch")	                return new cNodeSwitch(args, m_pItem, m_pBinder); sbreak;
	scase("bswitch")	            return new cNodeSwitch(args, m_pItem, m_pBinder, NULL, true); sbreak;
	scase("debug")	                return new cNodeDebug(args); sbreak;
	scase("getIcon")                return new cNodeGetIcon(m_pRoot, args); sbreak;
	scase("getIconAsync")           return new cNodeGetIcon(m_pRoot, args, false); sbreak;
	scase("mergeIcon")              return new cNodeMergeIcon(args); sbreak;
	scase("getDisplayName")         return new cNodeGetDisplayName(args, false); sbreak;
	scase("getDescription")         return new cNodeGetDisplayName(args, true); sbreak;
	scase("bindok")	                return new cNodeBindok(args, m_pRoot, m_pBinder); sbreak;
	scase("datetime")               return new cNodeTime(args, DTT_DT); sbreak;
	scase("datetime_notz")          return new cNodeTime(args, DTT_DT, true, false); sbreak;
	scase("date")                   return new cNodeTime(args, DTT_DATE); sbreak;
	scase("time")                   return new cNodeTime(args, DTT_TIME); sbreak;
	scase("timeshort")              return new cNodeTime(args, DTT_TIMESHORT, false); sbreak;
	scase("timestamp")              return new cNodeTime(args, DTT_TIMESTAMP); sbreak;
	scase("timestampshort")         return new cNodeTime(args, DTT_TIMESTAMP, false, false); sbreak;
	scase("dategroup")              return new cNodeDateGroup(args); sbreak;
	scase("iterate")	            return new cNodeIterate(args); sbreak;
	scase("close")	                return new cNodeClose(args); sbreak;
	scase("bicrypt")	            return new cNodeCrypt(args, true); sbreak;
	scase("crypt")	                return new cNodeCrypt(args); sbreak;
	scase("cryptcmp")	            return new cNodeCryptCmp(args); sbreak;
	scase("filesize")	            return new cNodeFileSize(args, m_pRoot); sbreak;
	scase("ser")                    return new cNodeSer(args); sbreak;
	scase("serid")                  return new cNodeSerId(args); sbreak;
	scase("inout")                  return new cNodeInOut(args); sbreak;
	scase("username")               return new cNodeUserName(args); sbreak;
	scase("msg")                    return new cNodeMessage(args, m_pRoot); sbreak;
	scase("balloon")                return new cNodeMessage(args, m_pRoot, true); sbreak;
	scase("dialog")                 return new cNodeShowWindow(args, NULL, true); sbreak;
	scase("window")                 return new cNodeShowWindow(args, NULL, false); sbreak;
	scase("menu")                   return new cNodeMenu(args); sbreak;
	scase("wildcard")               return new cNodeWildcard(args); sbreak;
	scase("url")					return new cNodeUrl(args); sbreak;
	scase("mail")					return new cNodeMail(args); sbreak;
	scase("internalIndexOperator")  return new cNodeVectorAt(args); sbreak;
	scase("at")                     return new cNodeVectorAt(args); sbreak;
	scase("format_struct")          return new cNodeFormatStruct(args, m_pRoot, m_pBinder); sbreak;
	scase("help")					return new cNodeHelp(args); sbreak;
	scase("sound")					return new cNodeSound(args); sbreak;
	scase("objdir")					return new cNodePath(args, PATHOP_DIR); sbreak;
	scase("objfile")				return new cNodePath(args, PATHOP_FILE); sbreak;

	scase("size")
		cNodeVectorSize * pNode = new cNodeVectorSize(args);
		if( !pNode->Init(m_pRoot, al) )
			pNode->Destroy(), pNode = NULL;
		return pNode;
		sbreak;

	scase("format")
		if( CFieldsBinder * vectBinder = InitVectorBinder(m_pRoot, GetOrigNode(args[0])) )
		{
			cNodeVectorFormat * pNode = new cNodeVectorFormat(args);
			if( !pNode->Init(al, vectBinder) )
				pNode->Destroy(), pNode = NULL;
			return pNode;
		}
		return new cNodeFormat(args);
		sbreak;
	
	scase("find")
		cNodeVectorFind * pNode = new cNodeVectorFind(args);
		if( !pNode->Init(m_pItem, al) )
			pNode->Destroy(), pNode = NULL;
		return pNode;
		sbreak;
	
	scase("treeFind")
		cNodeTreeFind * pNode = new cNodeTreeFind(args);
		if( !pNode->Init(m_pItem, al) )
			pNode->Destroy(), pNode = NULL;
		return pNode;
		sbreak;
	
	scase("env")
		cNode * pNode = NULL;
		if( args[0] && args[0]->Type() == ntString && m_pRoot->m_pSink )
		{
			tString &envStr = ((cNodeStr *)args[0])->m_str;
			envStr.insert(envStr.begin(), '%');
			envStr.insert(envStr.end(), '%');
			cStringObj expStr; utf8_to_utf16(envStr.c_str(), envStr.size(), expStr);
			if( ((CRootSink *)m_pRoot->m_pSink)->ExpandEnvironmentString(expStr) )
			{
				pNode = new cNodeStr();
				utf16_to_utf8(expStr.data(), expStr.size(), ((cNodeStr *)pNode)->m_str);
				args[0]->Destroy(); args[0] = NULL;
			}
		}		
		return pNode;
		sbreak;
	send;

	if( !strncmp(name, "s_", sizeof("s_") - 1) )
		return new cNodeSwitch(args, m_pItem, m_pBinder, name);
	if( !strncmp(name, "bs_", sizeof("bs_") - 1) )
		return new cNodeSwitch(args, m_pItem, m_pBinder, name);
	if( !strncmp(name, "p_", sizeof("p_") - 1) )
		return new cNodeNamedArg(args, name + sizeof("p_") - 1);
	if( !strncmp(name, "c_", 2) )
	{
		CRootItem::tMapCaches &vars = m_pRoot->m_mapCaches;
		return new cNodeCache(args, vars.insert(CRootItem::tMapCaches::value_type(
			name, CRootItem::cCache())).first->second);
	}
	if( !strncmp(name, "f_", sizeof("f_") - 1) )
	{
		cNodeSkinFunc * pNode = new cNodeSkinFunc();
		if( !pNode->Init(args, al, m_pRoot, m_pItem, m_pBinder, name) )
			pNode->Destroy(), pNode = NULL;
		return pNode;
	}

	if( args[0] )
	{
		CItemBase* item = NULL;
		if( args[0]->Type() == ntGlobalVar )
		{
			cVariant ctl;
			args[0]->Exec(ctl, TOR(cNodeExecCtx, (m_pRoot)));
			if( ctl.Type() == cVariant::vtCtl )
				item = ctl.c_ctl();
		}
		else if( args[0]->Type() == ntItemProp )
		{
			if( !InitItemPropAcc() )
				return NULL;
			
			cItemPropAcc * pPropAcc = (cItemPropAcc *)args[0];
			if( pPropAcc->m_propID == CBindProps::prpNone )
				item = pPropAcc->m_item;
		}

		if( item )
			if( cNode * node = item->CreateOperator(name, args, al) )
				return node;
	}
	
	if( m_pItem )
	{
		for(CItemBase* pItem = m_pItem; pItem; pItem = pItem->m_pParent)
			if( cNode * node = pItem->CreateOperator(name, args, al) )
				return node;
	}
	else
	{
		if( cNode * node = m_pRoot->CreateOperator(name, args, al) )
			return node;
	}
	
	return NULL;
}

cNode * CNodeFactory::CreateOperand(const char *name, cNode *container, cAlloc *al)
{
	if( container )
	{
		if( container->Type() == ntItemProp )
		{
			if( m_sItemPropAccPath.size() )
				m_sItemPropAccPath += '.';
			m_sItemPropAccPath += name;
			return container;
		}

		if( container->Type() == ntGlobalVar )
		{
			cVariant ctl;
			container->Exec(ctl, TOR(cNodeExecCtx, (m_pRoot)));
			if( ctl.Type() == cVariant::vtCtl )
			{
				container->Destroy();
				if( !InitItemPropAcc() )
					return NULL;
				
				return new cItemPropAcc(ctl.c_ctl());
			}
		}
	}

	if( !strcmp(name, "ctl") )
	{
		if( !InitItemPropAcc() )
			return NULL;
		
		return m_pItemPropAcc = new cItemPropAcc(m_pItem);
	}
	
	if( !strcmp(name, "global") )
	{
		LPCSTR strNs = "";
		if( CFieldsBinder::Structs *structs = m_pRoot->GetBinder()->GetNamespace(&strNs) )
			return new cSerFieldNamespace(strNs, *structs);
	}

	if( !strncmp(name, "g_", 2) )
		return new cGlobalVar(*m_pRoot->GetGlobalVar(name, true));

	for(CItemBase* pItem = m_pItem; pItem; pItem = pItem->m_pParent)
		if( cNode * node = pItem->CreateOperand(name, container, al) )
			return node;

	return cSerFieldAcc::Init(m_pBinder, container, name);
}

bool CNodeFactory::FinalInit()
{
	return InitItemPropAcc();
}

void CNodeFactory::ParseError(const char *expr, const char *err, unsigned int pos)
{
	m_pRoot->ReportError(prtERROR, "Parsing error: %s in expression: %s", err, expr);

	if( !(m_pRoot->m_nGuiFlags & GUIFLAG_EDITOR) )
	{
		tString strErr = err;
		strErr += " in expression: ";
		strErr += expr;
		GUI_ASSERT_MSG(m_pRoot, 0, strErr.c_str());
	}
}

bool CNodeFactory::InitItemPropAcc()
{
	if( !m_pItemPropAcc )
		return true;
	
	size_t propName = m_sItemPropAccPath.rfind('.');
	if( propName == tString::npos )
		propName = 0;

	m_pItemPropAcc->m_propID = CBindProps::ConvertPropID(m_sItemPropAccPath.c_str() + (propName ? propName + 1 : 0));
	if( m_pItemPropAcc->m_propID != CBindProps::prpNone )
		m_sItemPropAccPath.erase(propName);

	if( m_sItemPropAccPath.size() )
		m_pItemPropAcc->m_item = m_pItemPropAcc->m_item->GetItemByPath(m_sItemPropAccPath.c_str());
	
	bool res = !!m_pItemPropAcc->m_item;
	
	m_pItemPropAcc = NULL;
	m_sItemPropAccPath.clear();
	
	return res;
}

//////////////////////////////////////////////////////////////////////
// support for cWildCard
inline bool MatchOkayA(char ch)
{
	if( ch && ch!='*' )
		return false;
	return true;
}


bool MatchWithPattern(const char * Pattern, const char * Name, bool bPatternWithAsterics)
{
	char ch;
	char pc;

	if (!Pattern || !Name)
		return false;

	if(*Pattern=='*')
	{
		Pattern++;
		while(*Name && *Pattern)
		{
			pc = *Pattern;
			ch = *Name;
			if ((ch == '*') || (pc == ch) || (pc == '?'))
			{
				if(MatchWithPattern(Pattern+1,Name+1, bPatternWithAsterics)) 
					return true;
			}

			Name++;
		}
		return MatchOkayA(*Pattern);
	} 

	while(*Name && *Pattern != '*')
	{
		pc = *Pattern;
		ch = *Name;
		if(pc == ch || pc == '?')
		{
			Pattern++;
			Name++;
		} 
		else
		{
			if (pc == 0 && bPatternWithAsterics)
			{
				if (ch == '\\' || ch == '/')
					return true;
			}
			return false;
		}

	}
	if(*Name)
		return MatchWithPattern(Pattern,Name,bPatternWithAsterics);

	return MatchOkayA(*Pattern);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
