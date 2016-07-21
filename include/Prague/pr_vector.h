#if !defined( __pr_vector_h ) && defined( __cplusplus )
#define __pr_vector_h

#include <Prague/pr_serdescriptor.h>
#include <Prague/pr_serializable.h>
#include <Prague/iface/e_ktrace.h>

// -----------------------------------------------------
// cVector

// ---
template <class Type>
struct cCtrFactory {

	// ---
	static tDWORD type_size()                                { return sizeof(Type); }
	static tVOID constructor( Type& obj, const Type& other ) { new (&obj) Type( other ); }
	static tVOID constructor( Type& obj )                    { new (&obj) Type(); }
	static tVOID destructor( Type& obj )                     { obj.~Type(); }
	static bool equal( Type& obj1, const Type& obj2 )        { return !!(obj1 == obj2); }

	// ---
	static tVOID move( Type* dst, const Type* src, tUINT count ) {
		if ( !src || (dst == src) || !count )
			return;
		tINT dir;
		if ( (dst < src) || (dst >= (src + count)) )
			dir = 1;
		else {
			dst += count - 1;
			src += count - 1;
			dir = -1;
		}
		for( tUINT i=0; i<count; ++i,src+=dir,dst+=dir ) {
			constructor(*dst, *src);
			destructor(*(Type *)src);
		}
	}
};

// ---
//void * __cdecl my_memmove ( void * dst, const void * src, tUINT count );

// ---
template <class Type>
struct cSimpleMover : public cCtrFactory<Type>{
	using  cCtrFactory<Type>::type_size;
	// ---
	static tVOID move( Type* dst, const Type* src, tUINT count ) {
		if ( !src || (dst == src) || !count )
			return;
		my_memmove( dst, src, count*type_size() );
	}

};


// ---
//template <class Type>
//struct cBufferMover {
//	tVOID operator()( tCopyType, Type* dst, const Type* src, tUINT count ) {
//		tUINT i;
//		if ( (dst == src) || !count )
//			return;
//		if ( ((dst < src) && (src < (dst+count))) || ((src < dst) && (dst < (src+count))) ) { // !!! overlapped
//			tINT dir;
//			if ( dst < src )
//				dir = 1;
//			else {
//				dir = -1;
//				dst += count - 1;
//				src += count - 1;
//			}
//			for( i=0; i<count; ++i,src+=dir,dst+=dir ) {
//				my_memmove( dst, src, sizeof(cBuff<Type,BSIZE>) );
//				if ( src->m_ptr == (void*)src->m_buff )
//					dst->m_ptr = dst->m_buff;
//			}
//			return;
//		}
//		my_memmove( dst, src, count*sizeof(cBuff<Type,BSIZE>) );
//		for( i=0; i<count; ++i,++src,++dst ) {
//			if ( src->m_ptr == (void*)src->m_buff )
//				dst->m_ptr = dst->m_buff;
//		}
//	}
//};

template <class T> 
struct tMemChunkT {
	T*      m_ptr;
	tDWORD  m_used;
	tDWORD  m_allocated;
//	operator tMemChunk*() { return reinterpret_cast<tMemChunk*>(this); }
//	operator const tMemChunk*() const { return reinterpret_cast<const tMemChunk*>(this); }
};


// ---
template <class T, class cFactory=cCtrFactory<T> >
class cVector : private tMemChunkT<T> {
	typedef tMemChunkT<T> base;
	using base::m_ptr;
	using base::m_used;
	using base::m_allocated;
public:
	enum { npos = -1 };

public:
	cVector()                 { m_ptr = NULL; m_used = 0; m_allocated = 0; }
	cVector(const cVector& c) { m_ptr = NULL; m_used = 0; m_allocated = 0; assign(c); }
	~cVector()                { clear(); }
	
	void clear();

	cVector& operator =(const cVector& c) { assign(c); return *this; }
	bool     operator == (const cVector& c) const;
	bool     operator != (const cVector& c) const { return !(operator == (c)); }

	T *       data ()       { return (T *) m_ptr; }
	const T * data () const { return (const T *) m_ptr; }

//	         operator tPTR()             { return m_ptr; }
//	         operator T* ()              { return (T*)m_ptr; }
//	const T& operator[](tINT idx)  const { return at(idx); }
//	const T& operator[](long unsigned int idx)  const { return at(idx); }
//	const T& operator[](tUINT idx) const { return at(idx); }
//	const T& operator[](tQWORD idx) const { return at(idx); }
	const T& operator[](tSIZE_T idx) const { return at(idx); }

//	T&       operator[](tINT idx)        { return at(idx); }
//	T&       operator[](long unsigned int idx)  { return at(idx); }
//	T&       operator[](tUINT idx)       { return at(idx); }
//	T&       operator[](tQWORD idx)       { return at(idx); }
	T&       operator[](tSIZE_T idx)      { return at(idx); }
		
	tDWORD dimension()             const { return m_used; }
	tDWORD size()                  const { return m_used/cFactory::type_size(); }
	tDWORD count()                 const { return m_used/cFactory::type_size(); }
	tDWORD capacity()              const { return m_allocated/cFactory::type_size(); }
	bool   empty()                 const { return !m_used; }
	T&     at(tSIZE_T idx)         const { PR_ASSERT(idx < capacity()); return *(((T*)m_ptr) + idx); }

	tERROR alloc( tDWORD size );
	tERROR resize(tDWORD p_size);
	
	tERROR assign( const cVector &c );
	T&     push_back();
	tUINT  remove( tINT from, tINT to ); // returns amount of removed elements
	T&     insert(tDWORD pos);
	T&     insert(tDWORD pos, const T &el)	{ return insert(pos) = el; }
	
	typedef bool (*tCmpFunc)( const T& el, tCPTR params );
	tINT   find( const T& el, tUINT start_index = 0, tUINT stop_index = npos ) const;
	tINT   find( tCmpFunc cmp_func, tCPTR params, tUINT start_index = 0, tUINT stop_index = npos ) const;

	T&     push_back( const T& val ) { T& pval = push_back(); pval=val; return pval; }
	tUINT  remove_by_value( const T& el )  { tINT ind = find(el); if (npos==ind) return 0; return remove(ind); }
//	tUINT  remove( tINT ind )        { return remove(ind,ind); }
	tUINT  remove( tSIZE_T ind )        { return remove((tINT)ind,(tINT)ind); }
};

#define cVectorSimple(_Class) cVector<_Class, cSimpleMover<_Class> >

// ---
// members

template <class T, class cFactory>
void cVector<T,cFactory>::clear() {
	tUINT l_size = size();
	for( tUINT i = 0; i<l_size; ++i )
		cFactory::destructor(at(i));
	if( m_ptr )
		::g_root->heapFree(m_ptr);
	m_ptr = NULL;
	m_allocated = m_used = 0;
}



// ---
template <class T, class cFactory>
tERROR cVector<T,cFactory>::assign(const cVector &c) {
	tUINT i;
	tUINT l_src_size = c.size();
	tUINT l_dst_size = size();
	tERROR error = alloc(l_src_size);
	if( PR_FAIL(error) )
		return error;
	for( i=0; (i<l_dst_size) && (i<l_src_size); ++i )
		at(i) = c.at(i);
	for( ; i<l_src_size; ++i )
		cFactory::constructor(at(i), c.at(i));
	for( ; i<l_dst_size; ++i )
		cFactory::destructor(at(i));
	m_used = c.m_used;
	return errOK;
}



// ---
template <class T, class cFactory>
tERROR cVector<T,cFactory>::alloc( tDWORD p_size ) {
	tPTR ptr; tERROR error; tUINT allocated;
	if( p_size <= capacity() )
		return errOK;
	if ( PR_FAIL(error=::g_root->heapAlloc((tPTR*)&ptr,allocated=p_size*cFactory::type_size())) )
		return error;
	cFactory::move( (T*)ptr, (const T*)m_ptr, size() );
	if ( m_ptr )
		::g_root->heapFree( m_ptr );
	m_ptr = static_cast<T*>(ptr);
	m_allocated = allocated;
	return errOK;
}




// ---
template <class T, class cFactory>
tERROR cVector<T,cFactory>::resize( tDWORD p_size ) {
	cERROR err;
	tUINT count = size();
	if ( (count < p_size) && PR_SUCC(err=alloc(p_size)) ) {
		for( tUINT i=count; i<p_size; ++i )
			cFactory::constructor(at(i));
	}
	if ( PR_SUCC(err) )
		m_used = p_size * cFactory::type_size();
	return err;
}




// ---
template <class T, class cFactory>
T& cVector<T,cFactory>::push_back() {
	tUINT l_size = size();
	if( l_size == capacity() )
		alloc((l_size+1)<<1);
	cFactory::constructor(at(l_size));
	m_used += cFactory::type_size();
	return at(l_size);
}



// ---
template <class T, class cFactory>
T& cVector<T,cFactory>::insert( tDWORD pos ) {
	tUINT l_size = size();
	if( pos >= l_size )
		return push_back();
	if( l_size == capacity() )
		alloc( (l_size+1) << 1 );
	cFactory::move( (((T*)m_ptr) + (pos+1)), (((T*)m_ptr) + pos), l_size-pos );
	cFactory::constructor(at(pos));
	m_used += cFactory::type_size();
	return at(pos);
}



// ---
template <class T, class cFactory>
tUINT cVector<T,cFactory>::remove( tINT from, tINT to ) {
	tINT diff, count = size();
	if ( to >= count )
		to = count;
	else
		++to;
	if( (diff=to-from) <= 0 ) 
		return 0;
	for( tINT i=from; i<to; ++i )
		cFactory::destructor(at(i));
	if ( to < count )
		cFactory::move( (((T*)m_ptr) + from), (((T*)m_ptr) + to), count - to );

	m_used -= diff*cFactory::type_size();
	return diff;
}



// ---
template <class T, class cFactory>
tINT cVector<T,cFactory>::find( const T& el, tUINT start_index, tUINT stop_index ) const {
	tUINT cnt = count();
	if ( (stop_index < start_index) || (stop_index > cnt) )
		stop_index = cnt;
	for( ; start_index<stop_index; ++start_index ) {
		if ( cFactory::equal(at(start_index), el) ) 
			return start_index;
	}
	return npos;
}



// ---
template <class T, class cFactory>
tINT cVector<T,cFactory>::find( tCmpFunc cmp_func, const tVOID* params, tUINT start_index, tUINT stop_index ) const {
	tUINT cnt = count();
	if ( (stop_index < start_index) || (stop_index > cnt) )
		stop_index = cnt;
	for( ; start_index<stop_index; ++start_index ) {
		if ( cmp_func(at(start_index),params) ) 
			return start_index;
	}
	return npos;
}



// ---
template <class T, class cFactory>
bool cVector<T,cFactory>::operator == (const cVector& c) const {
	if(size() != c.size())
		return false;
	for(tDWORD i = 0; i < size(); i++)
		if(!cFactory::equal(at(i), c.at(i)))
			return false;
	return true;
}


typedef cVector< cStringObj >  cStringObjVector;
typedef cVector< tCHAR      >  cCharVector;
typedef cVector< tDWORD     >  cDwordVector;
typedef cVector< tQWORD     >  cQwordVector;
//typedef cVector< tSTRING    >  cStringVector;

// -----------------------------------------------------

#define VECTORAPI_NEW      0x01
#define VECTORAPI_MOVE     0x02
#define VECTORAPI_CLEARSRC 0x01
#define VECTORAPI_CLEARDST 0x02

class cVectorHnd
{
public:
	cVectorHnd() : m_v(NULL), m_type(tid_VOID), m_flags(0), m_type_size(0), m_desc(NULL) {}
	
	tVOID    attach(tMemChunk * pVect, tTYPE_ID nType, tUINT flags = 0);
	tVOID    detach();

	tDWORD   size() const { return m_v ? (m_v->m_used/m_type_size) : 0; }
	tPTR     at(tSIZE_T idx) const { return m_v ? ((tBYTE *)m_v->m_ptr + idx*m_type_size) : NULL; }
	tPTR     at_safe(tSIZE_T idx) const { return (idx < size()) ? at(idx) : NULL; }
	
	cSerializable * ser_ptr(tPTR p);

	tERROR   clear();
	tDWORD   remove(tDWORD pos, tDWORD num = 1);
	tPTR     insert(tDWORD pos, tDWORD num = 1);
	tERROR   alloc(tDWORD num);
	tERROR   assign(cVectorHnd& src, tDWORD flags = VECTORAPI_CLEARDST);
	bool     isequal(cVectorHnd& v);

public:
	tERROR   elem_assign(tPTR obj, tPTR other = NULL, tDWORD flags = 0);
	tVOID    elem_destructor(tPTR obj);
	bool     elem_isequal(tPTR obj1, tPTR obj2);

public:
	tMemChunk *      m_v;
	tTYPE_ID         m_type;
	tUINT            m_flags;
	
	tDWORD           m_type_size;
	cSerDescriptor * m_desc;
};

// ---
// members

inline tVOID cVectorHnd::attach(tMemChunk * pVect, tTYPE_ID nType, tUINT flags)
{
	m_v = pVect;
	m_type = nType;
	m_flags = flags;
	
	m_type_size = _toui32((m_flags & SDT_FLAG_POINTER) ? sizeof(tPTR) : pr_sizeometer(m_type, NULL));
	m_desc = IS_SERIALIZABLE(m_type) ? cSerializableObj::getDescriptor(m_type) : NULL;
}

inline tVOID cVectorHnd::detach()
{
	m_v = NULL;
	m_type = tid_VOID;
	m_flags = 0;
}

inline tERROR cVectorHnd::clear()
{
	remove(0, size());
	tPTR ptr = m_v->m_ptr;
	m_v->m_ptr = NULL;
	m_v->m_allocated = m_v->m_used = 0;
	return ::g_root->heapFree(ptr);
}

inline tDWORD cVectorHnd::remove(tDWORD pos, tDWORD num)
{
	tDWORD n = size(), to = pos + num;
	if( pos >= n )
		return 0;

	if( to > n )
		to = n;

	tDWORD i; 
	for( i = pos; i < to; i++)
		elem_destructor(at(i));
	
	for(tDWORD k = pos; i < n; i++, k++)
		elem_assign(at(k), at(i), VECTORAPI_NEW|VECTORAPI_MOVE);

	m_v->m_used -= num*m_type_size;
	return num;
}

inline tPTR cVectorHnd::insert(tDWORD pos, tDWORD num)
{
	tDWORD n = size(), to;
	if( pos > n )
		pos = n;
	to = pos + num;

	alloc(n + num);

	tDWORD i,k;
	for(k = n + num, i = n; i > pos; i--, k--)
		elem_assign(at(k - 1), at(i - 1), VECTORAPI_NEW|VECTORAPI_MOVE);
	
	for(i = pos; i < to; i++)
		elem_assign(at(i), NULL, VECTORAPI_NEW);

	m_v->m_used += num*m_type_size;
	return at(pos);
}

inline tERROR cVectorHnd::alloc(tDWORD num)
{
	tDWORD mem_size = num*m_type_size;
	if( mem_size <= m_v->m_allocated )
		return errOK;
	
	tPTR ptr; tERROR error;
	if( PR_FAIL(error = ::g_root->heapAlloc((tPTR *)&ptr, mem_size)) )
		return error;
	
	for(tDWORD i = 0; i < size(); i++)
		elem_assign((tBYTE *)ptr + i*m_type_size, at(i), VECTORAPI_NEW|VECTORAPI_MOVE);

	if( m_v->m_ptr )
		::g_root->heapFree(m_v->m_ptr);
	
	m_v->m_ptr = ptr;
	m_v->m_allocated = mem_size;
	return errOK;
}

inline tERROR cVectorHnd::assign(cVectorHnd& src, tDWORD flags)
{
	if( m_type != src.m_type || m_flags != src.m_flags )
		return errPARAMETER_INVALID;
	
	if( flags & VECTORAPI_CLEARDST )
		clear();
	
	int pos = size(), add = src.size();
	insert(pos, add);
	
	for(int i = 0; i < add; i++)
		elem_assign(at(pos + i), src.at(i), 0);

	if( flags & VECTORAPI_CLEARSRC )
		src.clear();

	return errOK;
}

inline bool cVectorHnd::isequal(cVectorHnd& v)
{
	if( m_type != v.m_type || m_flags != v.m_flags )
		return false;

	tDWORD n = size();
	if( n != v.size() )
		return false;
	
	for(tDWORD i = 0; i < n; i++)
		if( !elem_isequal(at(i), v.at(i)) )
			return false;
	
	return true;
}

inline tERROR cVectorHnd::elem_assign(tPTR obj, tPTR other, tDWORD flags)
{
	tERROR nErr = errOK;
	
	if( m_desc )
	{
		if( !(m_flags & SDT_FLAG_POINTER) )
		{
			if( flags & VECTORAPI_NEW )
				m_desc->m_constructor((cSerializable *)obj, (cSerializable *)other);
			else
				nErr = ((cSerializable *)obj)->assign(*(cSerializable *)other, true);
			
			if( flags & VECTORAPI_MOVE )
				m_desc->m_destructor((cSerializable *)other);
			return nErr;
		}
		
		if( flags & VECTORAPI_MOVE )
		{
			if( !(flags & VECTORAPI_NEW) )
				((cSerializableObj *)obj)->~cSerializableObj();
			memcpy(obj, other, m_type_size);
		}
		else
		{
			if( flags & VECTORAPI_NEW )
			{
				if( !other )
					new ((cSerializableObj *)obj) cSerializableObj();
				else
					new ((cSerializableObj *)obj) cSerializableObj(*(const cSerializableObj *)other);
			}
			else
				nErr = ((cSerializableObj *)obj)->assign(*(const cSerializableObj *)other);
		}
	
		return nErr;
	}
	
	if( IS_SERIALIZABLE(m_type) )
		return errNOT_OK;
	
	switch( m_type )
	{
	case tid_STRING_OBJ:
		if( flags & VECTORAPI_MOVE )
		{
			if( !(flags & VECTORAPI_NEW) )
				((cStringObj *)obj)->~cStringObj();
			memcpy(obj, other, m_type_size);
		}
		else
		{
			if( flags & VECTORAPI_NEW )
			{
				if( !other )
					new ((cStringObj *)obj) cStringObj();
				else
					new ((cStringObj *)obj) cStringObj(*(const cStringObj *)other);
			}
			else
				nErr = ((cStringObj *)obj)->assign(*(const cStringObj *)other);
		}

		return nErr;
	default: break;
	} // switch m_type

	if( !other )
		memset(obj, 0, m_type_size);
	else
		memcpy(obj, other, m_type_size);
	return nErr;
}

inline bool cVectorHnd::elem_isequal(tPTR obj1, tPTR obj2)
{
	if( m_desc )
	{
		if( !(m_flags & SDT_FLAG_POINTER) )
			return !!((cSerializable *)obj1)->isEqual((cSerializable *)obj2);
		
		cSerializable * pSer1 = *(cSerializable **)obj1;
		cSerializable * pSer2 = *(cSerializable **)obj2;
		if( !pSer1 || !pSer2 )
			return false;
		
		return !!pSer1->isEqual(pSer2);
	}
	
	if( IS_SERIALIZABLE(m_type) )
		return false;
	
	switch( m_type )
	{
	case tid_STRING_OBJ:
		return *(cStringObj *)obj1 == *(cStringObj *)obj2;
	default: break;
	}

	return !!memcmp(obj1, obj2, m_type_size);
}

inline tVOID cVectorHnd::elem_destructor(tPTR obj)
{
	if( m_desc )
	{
		if( !(m_flags & SDT_FLAG_POINTER) )
			m_desc->m_destructor((cSerializable *)obj);
		else
			((cSerializableObj *)obj)->~cSerializableObj();
	
		return;
	}
	
	switch( m_type )
	{
	case tid_STRING_OBJ:
		((cStringObj *)obj)->~cStringObj();
		break;
	default: break;
	}
}

inline cSerializable * cVectorHnd::ser_ptr(tPTR p)
{
	if( !p || !IS_SERIALIZABLE(m_type) )
		return NULL;

	if( m_flags & SDT_FLAG_POINTER )
		return *(cSerializable **)p;

	return (cSerializable *)p;
}

// -----------------------------------------------------

#endif  //__pr_vector_h
