// Formats.h: interface for the CFormats class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMATS_H__8C1EA003_2436_4691_9A3B_1ACAC4079B40__INCLUDED_)
#define AFX_FORMATS_H__8C1EA003_2436_4691_9A3B_1ACAC4079B40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <ProductCore/binding.h>

//////////////////////////////////////////////////////////////////////////
// cNodeConst

class cNodeConst : public cNodeArgs<1>
{
public:
	NODE_TYPE(ntConst);
	cNodeConst(cNode **args) : cNodeArgs<1>(args) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
};

//////////////////////////////////////////////////////////////////////////
// cNodeDebug

class cNodeDebug : public cNodeArgs<1>
{
public:
	NODE_TYPE(ntDebug);
	cNodeDebug(cNode **args) : cNodeArgs<1>(args) {}
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
};

//////////////////////////////////////////////////////////////////////////
// cItemPropAcc

class cItemPropAcc : public cNode
{
public:
	NODE_TYPE(ntItemProp);
	cItemPropAcc(CItemBase *item) : m_propID(CBindProps::prpNone), m_item(item) {}
	virtual void Exec(cVariant &result, cNodeExecCtx &ctx, bool get);
	virtual bool IsConst() const { return false; }
	
public:
	static void  SetIcon(CItemBase *item, const cVariant &icon);

	propid_t    m_propID;
	CItemBase * m_item;
};

//////////////////////////////////////////////////////////////////////////
// cGlobalVar

class cGlobalVar : public cNode
{
public:
	NODE_TYPE(ntGlobalVar);
	cGlobalVar(cVariant& var) : m_var(var){}
	void Exec(cVariant &result, cNodeExecCtx &ctx, bool get); 
	bool IsConst() const { return false; }

private:
	cVariant& m_var;
};

//////////////////////////////////////////////////////////////////////////
// cNodePrmArray

class cNodePrmArray : public cNode
{
public:
	class CNf : public CNodeFactory
	{
	public:
		CNf(cNodePrmArray * p, CItemBase *pItem, CFieldsBinder *pBinder = NULL, LPCSTR strArgs = NULL);
		cNode * CreateOperand(const char *name, cNode *container, cAlloc *al);

		cNodePrmArray * m_p;
		cStrData        m_a[NODE_MAX_ARGS + 1];
	};

	class cRedirArg : public cNode
	{
	public:
		NODE_TYPE(ntRedirect);

		cRedirArg(cNode *arg = NULL) : m_arg(arg) {}
		~cRedirArg() { if( m_arg ) m_arg->Destroy(); }
		
		void Exec(cVariant &value, cNodeExecCtx &ctx, bool get = true);
		void Destroy() {}
		EnumRes Enum(cNodeEnum *enumerator, type_t type) { return m_arg ? m_arg->Enum(enumerator, type) : erContinue; }

		cNode *  m_arg;
		cVariant m_v;
	};

public:
	cNodePrmArray();
	~cNodePrmArray();
	
	void    Reset();
	void    Init(cNode **args);
	cNode * GetArg(tDWORD idx);
	void    Exec(cVariant &value, cNodeExecCtx &ctx, bool get = true) { if( get ) Reset(); }
	EnumRes Enum(cNodeEnum *enumerator, type_t type);
	EnumRes EnumChildren(cNodeEnum *enumerator, type_t type);

	cRedirArg   m_arg1;
	cRedirArg * m_args;
	tDWORD      m_args_n;
};

//////////////////////////////////////////////////////////////////////////
// cNodeBindok

class cNodeBindok : public cNodeArgs<1>
{
public:
	NODE_TYPE(ntBindok);
	cNodeBindok(cNode **args, CRootItem *root, CFieldsBinder *binder);
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);

	CGuiPtr<cNode> m_pNode;
};

//////////////////////////////////////////////////////////////////////////
// cNodeFormat

struct cNodeFormat : public cNodeArgs<2>
{
	cNodeFormat(cNode **args);
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
	tString m_fmt;
};

//////////////////////////////////////////////////////////////////////////
// cBinderNode

template <size_t _NumArgs> class cBinderNode : public cNodeArgs<_NumArgs>
{
public:
	cBinderNode(cNode **args = NULL) : cNodeArgs<_NumArgs>(args), m_binder(NULL) {}
	~cBinderNode() { if( m_binder ) m_binder->Destroy(), m_binder = NULL; }
	CFieldsBinder * m_binder;
};

//////////////////////////////////////////////////////////////////////////
// cNodeVectorFormat

class cNodeVectorFormat : public cBinderNode<3>
{
public:
	NODE_TYPE(ntVectorFormat);
	cNodeVectorFormat(cNode **args) : cBinderNode<3>(args) {}
	
	bool Init(cAlloc *al, CFieldsBinder * binder);
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
	
	tString         m_separator;
};

//////////////////////////////////////////////////////////////////////////
// cNodeVectorSize

class cNodeVectorSize : public cBinderNode<2>
{
public:
	NODE_TYPE(ntVectorSize);
	cNodeVectorSize(cNode **args) : cBinderNode<2>(args) {}
	
	bool Init(CRootItem *root, cAlloc *al);
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
};

//////////////////////////////////////////////////////////////////////////
// cNodeVectorAt

class cNodeVectorAt : public cNodeArgs<2>
{
public:
	NODE_TYPE(ntVectorAt);
	cNodeVectorAt(cNode **args = NULL) : cNodeArgs<2>(args) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
};

//////////////////////////////////////////////////////////////////////////
// cNodeVectorFind

class cNodeVectorFind : public cBinderNode<3>
{
public:
	NODE_TYPE(ntVectorFind);
	cNodeVectorFind(cNode **args) : cBinderNode<3>(args) {}
	
	bool Init(CItemBase *item, cAlloc *al);
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
};

//////////////////////////////////////////////////////////////////////////
// cNodeTreeFind

class cNodeTreeFind : public cBinderNode<3>
{
public:
	NODE_TYPE(ntTreeFind);
	cNodeTreeFind(cNode **args) : cBinderNode<3>(args) {}

	bool Init(CItemBase *item, cAlloc *al);
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
	
	cSer * Find(cSer *ser, cNodeExecCtx &ctx);
};

//////////////////////////////////////////////////////////////////////////
// cNodeSwitch

#define notInitialized ((cNode *)-1)
#define useNext        ((cNode *)-2)

class cNodeSwitch : public cNodePrmArray
{
public:
	enum eKnownCases
	{
		caseEmpty = 0xcc6f8249,            // is a crc32 of "<empty>"
	};
	
	struct cCaseItem
	{
		tQWORD  m_id;
		cNode * m_node;

		DECLARE_FIND_BY_ONE_FIELD_FUNCTOR(cFindByID, cCaseItem, tQWORD, m_id);
	};
	typedef std::vector<cCaseItem> cCaseItems;

public:
	NODE_TYPE(ntSwitch);
	cNodeSwitch(cNode **args, CItemBase *item, CFieldsBinder *binder, LPCSTR strSecton = NULL, bool bidirect = false);
	~cNodeSwitch();
	
	bool    Init(cNodeExecCtx &ctx);
	void    Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
	EnumRes Enum(cNodeEnum *enumerator, type_t type);
	
	cNode * CreateCaseNode(struct tagIniLine *line, cNodeExecCtx &ctx);
	cNode * PrepareNode(cNode *& node, size_t i, cNodeExecCtx &ctx);
	void    ExecBiSet(cVariant &value, cNodeExecCtx &ctx);
	cNode * GetCaseNode(cNodeExecCtx &ctx);
	EnumRes EnumChildren(cNodeEnum *enumerator, type_t type, cNodeExecCtx &ctx);

	tString                m_section;
	cCaseItems             m_cases;
	cNode *                m_defaultCase;
	CItemBase *            m_item;
	CFieldsBinder *        m_binder;
	unsigned               m_inited : 1;
	unsigned               m_string : 1;
	unsigned               m_bidirect : 1;
};

//////////////////////////////////////////////////////////////////////////
// cNodeInvert

class cNodeInvert : public cNodeArgs<1>
{
public:
	NODE_TYPE(ntInvert);
	cNodeInvert(cNode **args) : cNodeArgs<1>(args) {}
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
};

//////////////////////////////////////////////////////////////////////////
// cNodeMask

class cNodeMask : public cNodeArgs<2>
{
public:
	NODE_TYPE(ntMask);
	cNodeMask(cNode **args, bool bBool = false);
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);

	tQWORD   m_mask;
	unsigned m_bool;
};

//////////////////////////////////////////////////////////////////////////
// cNodeShift

class cNodeShift : public cNodeArgs<2>
{
public:
	NODE_TYPE(ntShift);
	cNodeShift(cNode **args) : cNodeArgs<2>(args), m_distance(0) {}
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
	tDWORD m_distance;
};

//////////////////////////////////////////////////////////////////////////
// cNodeGetIcon

class cNodeGetIcon : public cNodeArgs<4>
{
public:
	NODE_TYPE(ntGetIcon);
	cNodeGetIcon(CRootItem *root, cNode **args, bool sync = true);
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
	
	CGuiPtr<CIcon> m_defIcon;
	unsigned       m_sync : 1;
	unsigned       m_large : 1;
};

//////////////////////////////////////////////////////////////////////////
// cNodeGetDisplayName

class cNodeGetDisplayName : public cNodeArgs<1>
{
public:
	NODE_TYPE(ntGetDisplayName);
	cNodeGetDisplayName(cNode **args, bool desctiption) : cNodeArgs<1>(args), m_desctiption(desctiption) {}
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
	
	unsigned m_desctiption : 1;
};

//////////////////////////////////////////////////////////////////////////
// cNodeTime

struct cNodeTime : public cNodeArgs<1>
{
	NODE_TYPE(ntTime);
	cNodeTime(cNode **args, DTT_TYPE dttType, bool seconds = true, bool local = true) : cNodeArgs<1>(args), m_dttType(dttType), m_seconds(seconds), m_local(local) {}
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);

protected:
	DTT_TYPE   m_dttType;
	unsigned   m_seconds : 1;
	unsigned   m_local : 1;
};

//////////////////////////////////////////////////////////////////////////
// cNodeDateGroup

struct cNodeDateGroup : public cNodeArgs<1>
{
	NODE_TYPE(ntDateGroup);
	cNodeDateGroup(cNode **args);
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);

protected:
	cDateTime  m_now;
	tDWORD     m_weekno;
};

//////////////////////////////////////////////////////////////////////////
// cNodeIterate

struct cNodeIterate : public cNodeArgs<NODE_MAX_ARGS>
{
	cNodeIterate(cNode **args) : cNodeArgs<NODE_MAX_ARGS>(args) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
};

//////////////////////////////////////////////////////////////////////////
// cNodeCrypt

struct cNodeCrypt : public cNodeArgs<1>
{
	cNodeCrypt(cNode **args, bool bidirect = false) : cNodeArgs<1>(args), m_bidirect(bidirect) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
	unsigned m_bidirect : 1;
};

//////////////////////////////////////////////////////////////////////////
// cNodeCryptCmp

struct cNodeCryptCmp : public cNodeArgs<2>
{
	cNodeCryptCmp(cNode **args) : cNodeArgs<2>(args) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
};


//////////////////////////////////////////////////////////////////////////
// cNodeFileSize

struct cNodeFileSize : public cNodeArgs<2>
{
	cNodeFileSize(cNode **args, CRootItem * root);
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);

	cVector<tString, cSimpleMover<tString> > m_loc;
};

//////////////////////////////////////////////////////////////////////////
// cNodeSkinFunc

struct cNodeSkinFunc : public cNodePrmArray
{
	cNodeSkinFunc() : m_pNode(NULL) {}
	~cNodeSkinFunc() { if( m_pNode ) m_pNode->Destroy(); }

	EnumRes Enum(cNodeEnum *enumerator, type_t type);
	bool Init(cNode **args, cAlloc *al, CRootItem *root, CItemBase *item, CFieldsBinder *binder, const char *name);
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);

	cNode * m_pNode;
};

//////////////////////////////////////////////////////////////////////////
// cNodeSer

class cNodeSer : public cNodeArgs<1>
{
public:
	NODE_TYPE(ntSer);
	cNodeSer(cNode **args) : cNodeArgs<1>(args) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
};

//////////////////////////////////////////////////////////////////////////
// cNodeSerId

class cNodeSerId : public cNodeArgs<1>
{
public:
	NODE_TYPE(ntSerId);
	cNodeSerId(cNode **args) : cNodeArgs<1>(args) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
};

//////////////////////////////////////////////////////////////////////////
// cNodeInOut

class cNodeInOut : public cNodeArgs<2>
{
public:
	NODE_TYPE(ntInOut);
	cNodeInOut(cNode **args) : cNodeArgs<2>(args) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get)
	{
		m_args[get ? 0 : 1]->Exec(value, ctx, get);
	}
};

//////////////////////////////////////////////////////////////////////

struct cNodeCache : public cNodeArgs<2>
{
	NODE_TYPE(ntCache);
	cNodeCache(cNode **args, CRootItem::cCache& cache) : cNodeArgs<2>(args), m_cache(cache) {}
	void Exec(cVariant &value, cNodeExecCtx &ctx, bool get){ if( get ) m_cache.get(value, ctx, m_args); }

private:
	CRootItem::cCache& m_cache;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_FORMATS_H__8C1EA003_2436_4691_9A3B_1ACAC4079B40__INCLUDED_)
