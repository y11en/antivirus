// SerializableView.h: interface for the CSerializableView class.
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Variant.h"

struct cAlloc
{
	virtual void * Alloc(size_t) = 0;
	virtual void   Free(void *) = 0;
};

class cNodeExecCtx;

//////////////////////////////////////////////////////////////////////////
// nodes

enum eNodeType
{
	ntInvalid = 0,
	ntAny = ntInvalid,
	
	ntCustom,
	ntNumber,
	ntString,
	ntIdent,
	ntAdd,
	ntSub,
	ntMul,
	ntDiv,
	ntRem,
	ntNeg,
	ntBitAnd,
	ntBitXor,
	ntBitOr,
	ntBitNeg,
	ntLogicAnd,
	ntLogicOr,
	ntLogicNeg,
	ntIF,
	ntEQ,
	ntCmpEQ,
	ntCmpNE,
	ntCmpLT,
	ntCmpGT,
	ntCmpLE,
	ntCmpGE,
	ntShiftRight,
	ntShiftLeft,
	ntSequence,
	ntRedirect,
	ntAgregate,
	ntLike,
	ntLast,
};

enum eNodeEvent
{
	neLast,
};

#define NODE_TYPE(_type) virtual type_t Type() const { return _type; }

class cNode
{
public:
	typedef unsigned int type_t;

	enum EnumRes { erContinue = 0, erStop, erSkipChilds };
	class cNodeEnum
	{
	public:
		virtual EnumRes Enum(cNode *node) = 0;
		static  EnumRes StopOrCont(bool stop) { return stop ? erStop : erContinue; }
	};

	class cFindFirst : public cNodeEnum
	{
	public:
		cFindFirst(const cNode *node) : m_node(node), m_result(NULL) {}
		EnumRes Enum(cNode *node) { m_result = node; return m_node ? StopOrCont(m_node == node) : erStop; }
		const cNode * m_node;
		cNode *       m_result;
	};

	class cNodeFactory
	{
	public:
		virtual cNode *CreateOperand(const char *name, cNode *container, cAlloc *al) { return NULL; }
		virtual cNode *CreateOperator(const char *name, cNode **args, cAlloc *al)    { return NULL; }
		virtual bool   FinalInit()                                                   { return true; }
		virtual void   ParseError(const char *expr, const char *err, unsigned int pos)                 {}
	};

public:
	virtual ~cNode() {}
	virtual void    Destroy() { delete this; }

	virtual type_t  Type() const { return ntCustom; }
	virtual void    Exec(cVariant &value, cNodeExecCtx &ctx, bool get = true) = 0;
	virtual EnumRes Enum(cNodeEnum *enumerator, type_t type = ntAny);
	virtual bool    IsConst() const { return true; }
	virtual cNode * Find(type_t type, const cNode *node = NULL);
	virtual void    ProcessEvent(id_t id, void * data = NULL) {}
	
	cVariant &      ExecGet(cNodeExecCtx &ctx, cVariant &value = TOR(cVariant, ())) { Exec(value, ctx) ; return value; }

public:
	static cNode *  Parse(const char *expression, cNodeFactory *nodeFactory = NULL, cAlloc *nodeAllocator = NULL, tString *error = NULL);
};

inline cNode *cNode::Find(type_t type, const cNode *node)
{
	cFindFirst ff(node);
	return Enum(&ff, type) == erStop ? ff.m_result : NULL;
}

inline cNode::EnumRes cNode::Enum(cNodeEnum *enumerator, type_t type)
{
	if( type == ntAny || type == Type() )
		return enumerator->Enum(this);
	return erContinue;
}

#define NODE_MAX_ARGS	7

template <size_t _NumArgs> class cNodeArgs : public cNode
{
public:
	cNodeArgs(cNode **args = NULL)
	{
		if( args )
		{
			memcpy(m_args, args, sizeof(m_args));
			memset(args, 0, sizeof(m_args));
		}
		else
			memset(m_args, 0, sizeof(m_args));
	}
	
	~cNodeArgs()
	{
		for(size_t i = 0; i < countof(m_args); i++)
			if( m_args[i] )
				m_args[i]->Destroy();
		memset(m_args, 0, sizeof(m_args));
	}

	virtual EnumRes Enum(cNodeEnum *enumerator, type_t type)
	{
		if( EnumRes res = cNode::Enum(enumerator, type) )
			return res == erSkipChilds ? erContinue : res;
		for(size_t i = 0; i < countof(m_args); i++)
			if( m_args[i] )
				if( EnumRes res = m_args[i]->Enum(enumerator, type) )
					return res;
		return erContinue;
	}

	size_t getArgc()
	{
		size_t argc = 0;
		while( argc < countof(m_args) && m_args[argc] )
			argc++;
		return argc;
	}

	cNode * m_args[_NumArgs];
};


//////////////////////////////////////////////////////////////////////////
// operands

class cNodeInt : public cNode
{
public:
	NODE_TYPE(ntNumber);
	cNodeInt(int64_t val = 0) : m_val (val) {}
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get) { if( get ) value = m_val; }
	
	int64_t m_val;
};

class cNodeStr : public cNode
{
public:
	NODE_TYPE(ntString);
	cNodeStr(LPCSTR val = 0){ if(val) m_str = val; }
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get) { if( get ) value = m_str; }

	tString m_str;
};

class cNodeIdent : public cNodeStr
{
	NODE_TYPE(ntIdent);
};

class cNodeAgregate : public cNode
{
public:
	NODE_TYPE(ntAgregate);
	cNodeAgregate(cNode *node) : m_node(node) {}
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get){ m_node->Exec(value, ctx, get); }
	cNode * m_node;
};

//////////////////////////////////////////////////////////////////////////
// operators

class cOperatorUnary : public cNodeArgs<1>
{
public:
	NODE_TYPE(m_type);
	cOperatorUnary(type_t type, cNode **args = NULL) : cNodeArgs<1>(args), m_type(type) {}
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
	type_t m_type;
};

class cOperatorBinary : public cNodeArgs<2>
{
public:
	NODE_TYPE(m_type);
	cOperatorBinary(type_t type, cNode **args = NULL) : cNodeArgs<2>(args), m_type(type) {}
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
	type_t m_type;
};

class cOperatorTernary : public cNodeArgs<3>
{
public:
	NODE_TYPE(m_type);
	cOperatorTernary(type_t type) : cNodeArgs<3>(), m_type(type) {}
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
	type_t m_type;
};

class cOperatorSequence : public cNodeArgs<2>
{
public:
	NODE_TYPE(ntSequence);
	cOperatorSequence(cNode **args) : cNodeArgs<2>(args) {}
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
};

//////////////////////////////////////////////////////////////////////

class cLikeNode : public cOperatorBinary
{
public:
	NODE_TYPE(ntLike);
	cLikeNode(cNode **args) : cOperatorBinary(ntLike, args) {}
	virtual void Exec(cVariant &value, cNodeExecCtx &ctx, bool get);
};


//////////////////////////////////////////////////////////////////////
