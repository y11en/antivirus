// SerializableView.h: interface for the CSerializableView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIALIZABLEVIEW_H__82304003_83F0_439E_810B_A827B7CB3894__INCLUDED_)
#define AFX_SERIALIZABLEVIEW_H__82304003_83F0_439E_810B_A827B7CB3894__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Prague/pr_serializable.h>
#include <Prague/pr_time.h>

#include <ProductCore/Interpret.h>

//////////////////////////////////////////////////////////////////////

#define DECLARE_FIND_BY_ONE_FIELD_FUNCTOR(functorName, objType, fieldType, fieldName) \
struct functorName \
{ \
	functorName(const fieldType &p_##fieldName) : fieldName(p_##fieldName) {} \
	bool operator ()(const objType &info) { return fieldName == info.fieldName; } \
	const fieldType &fieldName; \
};

template<class _Ty> inline void swap_objmem(_Ty &_Left, _Ty &_Right)
{
	void *ptr = alloca(sizeof(_Ty));
	memcpy(ptr, &_Left, sizeof(_Ty));
	memcpy(&_Left, &_Right, sizeof(_Ty));
	memcpy(&_Right, ptr, sizeof(_Ty));
}

template<class _Ty> void remove_quotes(_Ty &str)
{
	size_t sz = str.size();
	if( sz > 1 && str[0] == '"' && str[sz - 1] == '"' )
	{
		str.erase(sz - 1);
		str.erase(0, 1);
	}
}

#define STRUCTDATA_ALLSECTIONS ((LPCSTR)-1)

//////////////////////////////////////////////////////////////////////
// cFldData

struct cFldData
{
	cSerializable *       m_pStruct;
	cSerializable *       m_pFldStruct;
	cSerDescriptorField * m_pField;
	tTYPE_ID              m_type;
	tUINT                 m_size;
	tPTR                  m_pData;
	tDWORD                m_nPolicyBit;
	unsigned              m_bLocked : 1;
	unsigned              m_bMandatoried : 1;
};

//////////////////////////////////////////////////////////////////////
// CValData

struct CValData : public cFldData
{
	CValData(){ memset((cFldData *)this, 0, sizeof(cFldData)); }

	tQWORD          GetInteger();
	bool            SetInteger(tQWORD v);
	
	tQWORD          GetIntegerNative()
	{
		switch(m_type)
		{
		case tid_BOOL:       return m_size == sizeof(bool) ? *(bool *)m_pData : *(tBOOL *)m_pData;
		case tid_BYTE:       return *(tBYTE *)m_pData;
		case tid_SHORT:
		case tid_WORD:       return *(tWORD *)m_pData;
		case tid_INT:      
		case tid_UINT:     
		case tid_LONG:
		case tid_ERROR:
		case tid_DWORD:      return *(tDWORD *)m_pData;
		case tid_QWORD:
		case tid_LONGLONG:   return *(tQWORD *)m_pData;
		case tid_DOUBLE:     return (tQWORD)*(tDOUBLE *)m_pData;
		case tid_PTR:        return (tQWORD)*(tPTR *)m_pData;
		}

		return 0;
	}
	
	tSTRING         GetStrPtr() { return *(tSTRING *)m_pData; }
	cStringObj&     GetString() { return *(cStringObj*)m_pData; }
	tMemChunk&      GetVector() { return *(tMemChunk *)m_pData; }
	cDateTime&      GetDateTime() { return *(cDateTime *)m_pData; }
	cSerializable * GetSer(bool bPtr = false) { return bPtr ? *(cSerializable **)m_pData : (cSerializable *)m_pData; }

	bool IsEqual(CValData& src, tDWORD strFlags = 0)
	{
		if( m_type != src.m_type )
			return false;
		if( !m_pField || !src.m_pField || !m_pData || !src.m_pData )
			return false;
		if( m_pField->m_flags != src.m_pField->m_flags )
			return false;

		if( m_pField->m_flags & SDT_FLAG_POINTER )
			return false;
		
		if( IS_SERIALIZABLE(m_type) )
			return !!GetSer()->isEqual(src.GetSer());
		
		if( m_type == tid_STRING_OBJ )
			return !GetString().compare(src.GetString(), strFlags);
		
		return GetIntegerNative() == src.GetIntegerNative();
	}
};

//////////////////////////////////////////////////////////////////////
// CStructId

struct CStructId
{
	CStructId(tDWORD serId = cSerializable::eIID, LPCSTR strSect = NULL):
		m_sect(strSect), m_serId(serId){}
	
	bool operator !=(const CStructId& id)
	{
		if( m_serId != id.m_serId )
			return true;
		if( strcmp(m_sect ? m_sect : "", id.m_sect ? id.m_sect : "") != 0 )
			return true;
		return false;
	}

	LPCSTR         m_sect;
	tDWORD         m_serId;
};

//////////////////////////////////////////////////////////////////////
// CStructData

struct CStructData
{
	CStructData(cSerializable * pData, LPCSTR strSect = NULL, CItemBase *item = NULL, tDWORD propMask = 0, CFieldsBinder* binder = NULL):
		m_ser(pData), m_sect(strSect), m_item(item), m_propMask(propMask), m_staticData(false), m_binder(binder) {}
	
	bool operator !=(const CStructId& id)
	{
		if( !m_ser )
			return true;

		if( !m_ser->isBasedOn(id.m_serId) )
			return true;

		if( m_sect == STRUCTDATA_ALLSECTIONS )
			return false;

		if( strcmp(m_sect ? m_sect : "", id.m_sect ? id.m_sect : "") != 0 )
			return true;
		return false;
	}
	
	bool IsCompartible(const CStructId& id) const
	{
		if( m_sect == STRUCTDATA_ALLSECTIONS )
			return true;
		
		if( strcmp(m_sect ? m_sect : "", id.m_sect ? id.m_sect : "") != 0 )
			return false;
		
		if( !m_ser )
			return true;
		
		return m_ser->isBasedOn(id.m_serId);
	}
	
	bool IsCompartible(tTYPE_ID type) const
	{
		if( m_sect == STRUCTDATA_ALLSECTIONS )
			return true;

		if( !m_ser )
			return true;

		return m_ser->isBasedOn(type);
	}

	static CStructData *Default() { static CStructData data(NULL, STRUCTDATA_ALLSECTIONS); return &data; }
	
	LPCSTR          m_sect;
	cSerializable * m_ser;
	CItemBase     * m_item;     // item, whose properties has been changed
	tDWORD          m_propMask; // set of changed properties
	CFieldsBinder * m_binder;
	unsigned        m_staticData : 1;
};

//////////////////////////////////////////////////////////////////////
// CBindProps

typedef unsigned id_t; 
typedef id_t propid_t;

struct CBindProps
{
	enum
	{
		prpNone         = 0x0000,
		prpText         = 0x0001,
		prpValue        = 0x0002,
		prpIcon         = 0x0004,
		prpFont         = 0x0008,
		prpBackground   = 0x0010,
		prpVisible      = 0x0020,
		prpEnabled      = 0x0040,
		prpAnimated     = 0x0080,
		prpSelected     = 0x0100,
		prpHotlight     = 0x0200,
		prpCustom       = 0x8000,
	};
	static propid_t ConvertPropID(const char *propName);
	static propid_t StateMask2PropID(tDWORD nStateMask);

	CBindProps(enStrCnv nCnvType = escDef) 
          : m_nCnvType ( nCnvType ),
       m_pIcon ( 0 ),
       m_pFont ( 0 ),
       m_pBackground ( 0 ),
       m_bHide ( 0 ),
	   m_bDisabled ( 0 ),
       m_bDefault ( 0 ),
       m_bAnimate ( 0 ),
	   m_bHotlight ( 0 ),
       m_bError ( 0 ),
	   m_bVal ( 0 )
	{        
	}
	
	enStrCnv      m_nCnvType;
	CIcon *       m_pIcon;
	CFontStyle *  m_pFont;
	CBackground * m_pBackground;
	tString       m_strTipText;
	tString       m_strValue;
	unsigned      m_bHide : 2;
	unsigned      m_bDisabled : 2;
	unsigned      m_bDefault : 2;
	unsigned      m_bAnimate : 2;
	unsigned      m_bHotlight : 2;
	unsigned      m_bError : 1;
	unsigned      m_bVal : 1;
};

//////////////////////////////////////////////////////////////////////
// cNodeExecCtx

class cNodeExecCtx
{
public:
	cNodeExecCtx(CRootItem *root, CItemBase *item = NULL, CStructData *data = NULL, bool ext = false) :
		m_root(root),
		m_item(item),
		m_data(data),
		m_ext(ext),
		m_datachanged(0)
	{
		if( !m_data )
			m_data = CStructData::Default();
	}

	CRootItem   * m_root;
	CItemBase   * m_item;
	CStructData * m_data;
	unsigned      m_ext : 1;
	unsigned      m_datachanged : 1;
};

//////////////////////////////////////////////////////////////////////
// CHolderBinder

struct tFieldPathItem
{
	const cSerDescriptorField * m_p;
	tDWORD                      m_nPolicyBit;
};

typedef std::vector<tFieldPathItem> tFieldPath;

struct CFieldsBinder
{
public:
	typedef std::vector<cSerializable*>  Structs;
	typedef std::map<tString,Structs>    Sections;

public:
	CFieldsBinder(CRootItem * pRoot, CFieldsBinder * pParent = NULL, CItemSink * pSink = NULL, const cSerDescriptor* pDesc = NULL) : m_pRoot(pRoot), m_pParent(pParent), m_pSink(pSink), m_pDesc(pDesc) {}
	virtual ~CFieldsBinder() {}
	
	virtual void Destroy();

	virtual void  AddDataSource(cSerializable* pData, LPCSTR strSect = NULL, bool bAddFirst = false);
	virtual bool  GetDataSource(CStructId& nStructId, cSerializable** pStruct);
	virtual bool  CheckDataSource(CStructId& nStructId, cSerializable* pStruct, bool bRecurse = true);
	virtual void  ClearSource(cSerializable* pData);
	virtual void  ClearSources(bool bClearSect = true);
	
	virtual Structs * GetNamespace(const char **ns = NULL);

public:
	CRootItem *           m_pRoot;
	CFieldsBinder *       m_pParent;
	CItemSink *           m_pSink;
	const cSerDescriptor* m_pDesc;

	Sections              m_pSections;
};

//////////////////////////////////////////////////////////////////////
// CNodeFactory

class cItemPropAcc;
class CNodeFactory : public cNode::cNodeFactory
{
public:
	CNodeFactory(CItemBase *pItem, CFieldsBinder *pBinder = NULL);

protected:
	virtual cNode *CreateOperand(const char *name, cNode *container, cAlloc *al);
	virtual cNode *CreateOperator(const char *name, cNode **args, cAlloc *al);
	virtual bool   FinalInit();
	virtual void   ParseError(const char *expr, const char *err, unsigned int pos);

public:
	CItemBase *           m_pItem;
	CRootItem *           m_pRoot;
	CFieldsBinder *       m_pBinder;
	cAlloc *              m_pAlloc;

private:
	bool                  InitItemPropAcc();
	tString               m_sItemPropAccPath;
	cItemPropAcc *        m_pItemPropAcc;
};

//////////////////////////////////////////////////////////////////////
// CItemBinding

struct CItemBinding
{
public:
	CItemBinding(CItemBase* pItem, CItemBase* pCtxItem = NULL);
	CItemBinding(const CItemBinding &other);
	~CItemBinding();

	void    Assign(const CItemBinding &other);

public:
	bool    Init(CItemProps& pProps);
	void    UpdateData(bool bIn, CStructData *pData = NULL);

	bool    InitBinding(const cSerDescriptor* pDesc);
	void    AddDataSource(cSerializable* pData, LPCSTR strSect, bool bAddFirst);

	cNode * GetHandler(id_t holderID);
	bool    ProcessReflection(id_t holderID, cVar& res = TOR(cVar, ()), CStructData *data = NULL);

	cNode::EnumRes EnumNodes(cNode::cNodeEnum *enumerator, cNode::type_t type = ntAny);

public:
	static cNode * InitHolder(CItemBase * pItem, LPCSTR strProp);
	static cNode * InitHolder(CItemBase * pItem, CItemProps &pProps, LPCSTR strPropID);

public:
	CFieldsBinder * m_pBinder;
	CItemBase *     m_pItem;

protected:
	bool            m_bOwnBinder;
	
public:
	enum
	{
		hValue, hText, hTip, hEnabled, hVisible, hIcon, hFont, hBg, hProps,
		hOnClick, hOnRClick, hOnSelect, hOnChanging, hOnChange, hOnOk, hOnClose,
		hLast,
		hPropLast = hProps + 1
	};
	
	cNode *         m_holders[hLast];
};

inline cNode * CItemBinding::GetHandler(id_t holderID)
{
	return m_holders[holderID];
}

inline bool CItemBinding::ProcessReflection(id_t holderID, cVar& res, CStructData *data)
{
	if( m_pItem->m_nFlags & STYLE_DEBUG )
		debugBreak();

	cNode * pHolder = GetHandler(holderID);
	if( !pHolder )
		return false;
	
	cNodeExecCtx ctx(m_pBinder->m_pRoot, m_pItem, data);
	pHolder->Exec(res, ctx);
	
	if( ctx.m_datachanged && (holderID == hOnClick || holderID == hOnRClick) )
	{
		m_pItem->SetChangedData();
		CItemBase * pTop = m_pItem->GetOwner(true);
		if( pTop )
			pTop->UpdateData(true, CStructData::Default());
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////
// CViewBinding

template<class T = cSerializable>
class CDialogBindingViewT : public CDialogSink
{
public:
	typedef T StructType;

	CDialogBindingViewT(T * pStruct = NULL, tDWORD nActionsMask = 0) :
		CDialogSink(nActionsMask), m_pStruct(pStruct)
	{}

protected:
	void UpdateData(bool bIn)                       { Item()->UpdateData(bIn); }
	void UpdateData(cSerializable* pData, bool bIn) { Item()->UpdateData(bIn, &CStructData(pData)); }
 
	void OnCreate()
	{
		CDialogSink::OnCreate();
		Item()->AddDataSource(m_pStruct);
	}
	
protected:
	T *  m_pStruct;
};

template<class T = cSerializable>
class CDialogBindingT : public CDialogBindingViewT<T>, public cSerObj<T>
{
public:
	using cSerObj<T>::m_ser;
	using CDialogBindingViewT<T>::m_pStruct;
	using CDialogBindingViewT<T>::UpdateData;
	using CDialogBindingViewT<T>::m_pValidator;
	using CItemSinkT<CDialogItem>::Item;
	
	typedef T StructType;

	CDialogBindingT(T * pStruct = NULL, tDWORD nActionsMask = 0) :
		CDialogBindingViewT<T>(pStruct, nActionsMask)
	{}

protected:
	void UpdateStruct()
	{
		CDialogBindingViewT<T>::UpdateData(false);
		if( m_pStruct && m_ser )
			m_pStruct->assign(*m_ser, false);
	}
	
	void OnCreate()
	{
		CDialogSink::OnCreate();

		if( m_pStruct && !m_ser )
		{
			init(m_pStruct->getIID());
			assign(m_pStruct);
		}

		Item()->AddDataSource(m_ser);
	}
	
	bool OnOk()
	{
		UpdateStruct();
		if( m_pValidator && !m_pValidator->Validate(*this, m_pStruct) )
			return false;
		return true;
	}

	virtual void OnCustomizeData(cSerializable * pData, bool bIn) {}
};

//////////////////////////////////////////////////////////////////////////
// Nodes

enum eNodeTypeEx
{
	ntSerField = ntLast + 1,
	ntSerFieldEx,
	ntNamespace,
	ntVectorFormat,
	ntVectorSize,
	ntVectorAt,
	ntVectorFind,
	ntTreeFind,
	ntItemProp,
	ntMember,
	ntGlobalVar,
	ntNamedArg,
	ntConst,
	ntSwitch,
	ntInvert,
	ntMask,
	ntShift,
	ntDebug,
	ntBindok,
	ntGetIcon,
	ntGetDisplayName,
	ntTime,
	ntDateGroup,
	ntNodeClose,
	ntSer,
	ntSerId,
	ntInOut,
	ntCache,
	ntLastEx,
};

//////////////////////////////////////////////////////////////////////////
// cSerFieldNamespace

class cSerFieldNamespace : public cNode
{
public:
	NODE_TYPE(ntNamespace);
	cSerFieldNamespace(const char *ns, const CFieldsBinder::Structs &structs) : m_namespace(ns), m_structs(structs) {}
	virtual void   Exec(cVariant &result, cNodeExecCtx &ctx, bool get);
	virtual cSer * GetStruct(tTYPE_ID type);

	const char *                   m_namespace;
	const CFieldsBinder::Structs & m_structs;
};

//////////////////////////////////////////////////////////////////////////
// cSerFieldAcc

class cSerFieldAcc : public cNodeArgs<1>
{
public:
	NODE_TYPE(ntSerField);
	cSerFieldAcc(cNode *container, const cSerDescriptorField * field, tTYPE_ID serType);
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
	virtual bool IsConst() const { return false; }
	cSer *       GetStruct();
	cSer *       GetStruct(cNodeExecCtx &ctx);
	cSer *       GetBinderStruct();

	static cNode * Init(CFieldsBinder *binder, cNode *container, const char *name);
	static cNode * Init(cNode *container, const char *name, cSer *ser);
	static cNode * Init(cNode *container, const char *name, tTYPE_ID serType);
	static bool    Bind(const char *name, tTYPE_ID &serType, const cSerDescriptorField *&field);

	static void    CopyData(cVariant &result, tTYPE_ID type, tDWORD flags, tPTR data, bool get);
	static void    ExecSerFldData(cVariant &result, cSer * pStruct, tFieldPath& fieldPath, bool get, cNodeExecCtx &ctx);

	CFieldsBinder *             m_binder;	
	const cSerDescriptorField * m_field;
	tTYPE_ID                    m_serType;

private:
	static cNode * InitEx(CFieldsBinder *binder, cNode *container, const char *name);
};

//////////////////////////////////////////////////////////////////////////
// cSerFieldAccEx

class cSerFieldAccEx : public cSerFieldAcc
{
public:
	NODE_TYPE(ntSerFieldEx);
	cSerFieldAccEx(cNode *container, const char * name) : cSerFieldAcc(container, NULL, (tTYPE_ID)cSer::eIID), m_name(name) {};
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);

	tString m_name;
};

//////////////////////////////////////////////////////////////////////////
// cMemberAcc

class cMemberAcc : public cNode
{
public:
	NODE_TYPE(ntMember);
	cMemberAcc(tTYPE_ID type, tDWORD flags, tPTR data) : m_type(type), m_flags(flags), m_data(data) {}
	virtual void Exec(cVariant &result, cNodeExecCtx &ctx, bool get) { cSerFieldAcc::CopyData(result, m_type, m_flags, m_data, get); }
	virtual bool IsConst() const { return false; }

	tTYPE_ID    m_type;
	tDWORD      m_flags;
	tPTR        m_data;
};

#define BIND_MEMBER_MAP_BEGIN()                    virtual cNode *CreateOperand(const char *name, cNode *container, cAlloc *al) {
#define BIND_MEMBER(member, flags, type, bindName) if( !strcmp(bindName, name) ) return m_pItem->m_pRoot->CreateNode((tTYPE_ID)type, (tDWORD)flags, (tPTR)&member);
#define BIND_VALUE(member, type, bindName)         BIND_MEMBER(member, 0, type, bindName)
#define BIND_VECTOR(member, type, bindName)        BIND_MEMBER(member, SDT_FLAG_VECTOR, type, bindName)
#define BIND_MEMBER_MAP_END(baseClass)             return baseClass::CreateOperand(name, container, al); }

#define FUNC_NODE_CONST(_Class, _NumArgs) \
class _Class : public cNodeArgs<_NumArgs> \
{ public: \
	_Class(cNode **args = NULL) : cNodeArgs<_NumArgs>(args) {} \
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get) { value.Clear(); if( get ) Func(value, ctx); } \
	inline void Func(cVariant& value, cNodeExecCtx &ctx); \
}; \
inline void _Class::Func(cVariant& value, cNodeExecCtx &ctx)

#define FUNC_NODE(_Class, _NumArgs) \
class _Class : public cNodeArgs<_NumArgs> \
{ public: \
	_Class(cNode **args = NULL) : cNodeArgs<_NumArgs>(args) {} \
	bool IsConst() const { return false; } \
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get) { value.Clear(); if( get ) Func(value, ctx); } \
	inline void Func(cVariant& value, cNodeExecCtx &ctx); \
}; \
inline void _Class::Func(cVariant& value, cNodeExecCtx &ctx)

#define FUNC_NODE_PRM(_i)                          m_args[_i]->ExecGet(ctx)

#define MEMBER_NODE(_Class) class _Class : public cNode { public: void Destroy() {} bool IsConst() const { return false; } void Exec(cVariant &value, cNodeExecCtx &ctx, bool get); }; friend class _Class; _Class

//////////////////////////////////////////////////////////////////////////
// cNodeNamedArg

class cNodeNamedArg : public cNodeArgs<1>
{
public:
	NODE_TYPE(ntNamedArg);
	cNodeNamedArg(cNode **args, const char *name) : cNodeArgs<1>(args), m_name(name) {}
	void Exec(cVariant &result, cNodeExecCtx &ctx, bool get) { m_args[0]->Exec(result, ctx, get); }
	tString m_name;
};

//////////////////////////////////////////////////////////////////////////
// cSerTreeFind

class cSerTreeFind
{
public:
	cSerTreeFind(CRootItem *root, cNode *children, cNode *condition) :
		m_sd(NULL),
		m_ec(root, NULL, &m_sd),
		m_children(children),
		m_condition(condition)
	{}

	cSer * Find(cSer *ser)
	{
		m_sd.m_ser = ser;
		if( m_condition->ExecGet(m_ec) )
			return ser;

		cVectorHnd childrenV;
		if( cSerTreeFind::GetChildren(ser, childrenV) )
			for(size_t i = 0, n = childrenV.size(); i < n; i++)
				if( ser = cSerTreeFind::Find((cSer *)childrenV.at(i)) )
					return ser;

		return NULL;
	}
	
	bool GetChildren(cSer *ser, cVectorHnd &vect)
	{
		if( ser && m_children )
		{
			m_sd.m_ser = ser;
			if( const cVectorData *vd = m_children->ExecGet(m_ec).c_vector() )
			{
				if( vd->m_type == ser->getIID() )
				{
					vect.attach(vd->m_v, vd->m_type, vd->m_flags);
					return true;
				}
			}
		}
		return false;
	}

private:
	cNodeExecCtx m_ec;
	CStructData  m_sd;
	cNode *      m_children;
	cNode *      m_condition;
};

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_SERIALIZABLEVIEW_H__82304003_83F0_439E_810B_A827B7CB3894__INCLUDED_)

//////////////////////////////////////////////////////////////////////
