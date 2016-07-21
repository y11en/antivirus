#ifndef __pr_cpp_h
#define __pr_cpp_h

#if defined(__cplusplus)

#include <Prague/prague.h>
#include <Prague/iface/i_reg.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_string.h>

#include <string.h> 
#include <time.h> 
#include <crt_s.h> 

#ifndef __APPLE__
#pragma intrinsic(memcpy)
#endif

#if defined(ITS_LOADER)
#ifndef IMPEX_EXPORT_LIB
	#define IMPEX_EXPORT_LIB_UNDEF	
	#define IMPEX_EXPORT_LIB
#endif // IMPEX_EXPORT_LIB
#include <Prague/plugin/p_string.h>
#ifdef IMPEX_EXPORT_LIB_UNDEF
	#undef IMPEX_EXPORT_LIB
#endif // IMPEX_EXPORT_LIB_UNDEF

	#define PR_CPP_CF( name, id ) true
#else // ITS_LOADER
#ifndef IMPEX_IMPORT_LIB
	#define IMPEX_IMPORT_LIB_UNDEF	
	#define IMPEX_IMPORT_LIB
#endif // IMPEX_IMPORT_LIB
#include <Prague/plugin/p_string.h>

#ifdef IMPEX_IMPORT_LIB_UNDEF
	#undef IMPEX_IMPORT_LIB
#endif // IMPEX_IMPORT_LIB_UNDEF
	inline bool pr_cpp_check_f( tVOID** func, tUINT id ) {
		if ( !*func )
			::g_root->ResolveImportFunc( (tFUNC_PTR*)func, PID_STRING, id, ((tPID)(2)) );
		return 0 != *func;
	}
	#define PR_CPP_CF( name, id ) pr_cpp_check_f((tVOID**)&name,id)
#endif // ITS_LOADER

class cStringObj;
class cStringPtr;

template <class Type, int BSIZE> class cBuff;
template <int BuffSize>          class cCharBuff;

typedef cCharBuff<256>    cStrBuff;
typedef cStringObj        cStrObj;
typedef cBuff<tCHAR, 512> tChunckBuff;
typedef cBuff<tCHAR, 512> cChunkBuff;


// ---
template <class TObj> inline tERROR close_object( TObj*& obj ) {
	cERROR err;
	if ( obj ) {
		err = obj->sysCloseObject();
		obj = 0;
	}
	return err;
}


// ---------------------------------------------
class cObjLocker {

protected:
	const cObj* m_obj;
	tUINT       m_locked;

public:
	cObjLocker( const cObj* obj, bool lock_immediately = true ) : m_obj(obj), m_locked(0) { if (lock_immediately) lock(); }
	~cObjLocker()	{ unlock(); }

	bool lock( tIID iid = IID_ANY, tPID pid = PID_ANY ) {
		if ( m_obj && PR_SUCC(CALL_SYS_ValidateLock(::g_root,const_cast<cObj*>(m_obj), iid, pid )) ) {
			++m_locked;
			return true;
		}
		return false;
	}

	bool store( const cObj* obj, bool lock_immediately = true ) {
		//while( unlock() )
		//	;
		unlock();
		m_obj = obj;
		if ( lock_immediately )
			return lock();
		return true;
	}

	const cObj* operator = ( const cObj* obj ) { store(obj,true); return m_obj; }
	cObj* operator -> () const { return const_cast<cObj*>(m_obj); }
	operator cObj* () const { return const_cast<cObj*>(m_obj); }

	bool unlock() {
		if ( m_obj && m_locked ) {
			--m_locked;
			CALL_SYS_Release(const_cast<cObj*>(m_obj));
			return true;
		}
		return false;
	}

	const cObj* relinquish() { const cObj* obj = m_obj; m_locked = 0; m_obj = 0; return obj; }
	tUINT lock_count() const { return m_obj ? m_locked : 0; }
};



// -----------------------------------------------------
struct cMemChunk : public tMemChunk
{
protected:
	const iObj*  m_allocator;

public:
	cMemChunk( const iObj* obj=NULL ) : m_allocator(obj ? obj : (iObj*)::g_root) { m_used = m_allocated = 0; m_ptr = 0; }
	cMemChunk( const cMemChunk& src ) { m_allocator = src.m_allocator; m_used = m_allocated = 0; m_ptr = 0; operator = ( src ); }
	virtual ~cMemChunk() { clean(); }

	virtual tPTR   reserve( tUINT count, bool realloc );
	virtual tPTR   exact_size( tUINT count, bool realloc );
	virtual void   clean ();
	tPTR           drop_unused()     { return exact_size(m_used,true); }

	const iObj*&   owner ()          { return m_allocator; }
	operator       tPTR  ()          { return m_ptr; }
	operator const tPTR  ()    const { return m_ptr; }
	
	const tVOID*   ptr   ()    const { return m_ptr; }
	tUINT          used  ()    const { return m_used; }
	tVOID          used  ( tDWORD count ) { if (count > m_allocated) count = m_allocated; m_used = count; }
	tUINT          size  ()    const { return m_allocated; }

	tUINT          count ()    const { return used(); }
	tUINT          allocated() const { return m_allocated; }

	// ---
	cMemChunk& operator = ( const tMemChunk& src )
	{
		tPTR ptr = exact_size( src.m_used, false );
		if ( ptr )
			memcpy( ptr, src.m_ptr, src.m_used );
		return *this;
	}

	friend bool operator == ( const tMemChunk& lhp, const tMemChunk& rhp );
	friend bool operator != ( const tMemChunk& lhp, const tMemChunk& rhp );
};

inline bool operator == ( const tMemChunk& lhp, const tMemChunk& rhp )
{
	return lhp.m_used == rhp.m_used &&
		lhp.m_ptr != NULL && rhp.m_ptr != NULL && !memcmp(lhp.m_ptr, rhp.m_ptr, lhp.m_used);
}

inline  bool operator != ( const tMemChunk& lhp, const tMemChunk& rhp ) 
{
	return !operator==(lhp, rhp); 
}



// -----------------------------------------------------
template <int ASIZE>
class cMemChunkAligned : public cMemChunk
{
public:
	cMemChunkAligned( const iObj* obj=NULL ) : cMemChunk(obj) {}
	virtual tPTR   reserve( tUINT count, bool realloc )
	{
		count = (count%ASIZE ? (count + ASIZE - count%ASIZE) : count);
		return cMemChunk::reserve(count, realloc);
	}
};



// -----------------------------------------------------
template <class Type, int BSIZE> class cBuff : public cMemChunk {
	
public:
	Type m_buff[BSIZE];
	
public:
	cBuff( const iObj* obj=NULL ) : cMemChunk(obj) { m_ptr = m_buff; m_used = 0; m_allocated = sizeof(m_buff), ::memset(m_buff,0,BSIZE); }
	cBuff( const cBuff& l_buf ) { m_ptr = m_buff; m_used = 0; m_allocated = BSIZE * sizeof(Type); operator = ( l_buf );} 
	virtual ~cBuff() { clean(); m_ptr = 0; }

	operator       Type* ()       { return (Type*)m_ptr; }
	operator const Type* () const { return (Type*)m_ptr; }
	
	// ---
	const Type*    ptr   () const { return (Type*)m_ptr; }
	tUINT          used  () const { return cMemChunk::used()/sizeof(Type); }
	tVOID          used  ( tDWORD count ) { cMemChunk::used( count * sizeof(Type) ); }
	tUINT          size  () const { return cMemChunk::size()/sizeof(Type); }
	tUINT          count () const { return used(); }
	
	virtual tPTR   reserve( tUINT count, bool realloc );
	virtual tPTR   exact_size( tUINT count, bool realloc );
	virtual void   clean ();

	Type*          get( tUINT count, bool realloc );
	Type*          set( tUINT count, const Type* src );
	Type*          add( tUINT count = 1 );
	Type*          add( tUINT count, const Type* src );
	Type*          once_more();
	
	// ---
	cBuff& operator = ( const cBuff& src ) {
		Type* ptr = get( src.used(), false );
		if ( ptr )
			memcpy( ptr, src.m_ptr, src.used() );
		return *this;
	}
	
};




// -----------------------------------------------------
template <int BuffSize>
class cCharBuff : public cBuff<tCHAR, BuffSize> {
public:
    typedef cBuff<tCHAR, BuffSize> Base;
    using Base::m_used;
    using Base::ptr;
    using Base::size;
	cCharBuff( const cStringObj& str, tCODEPAGE cp ) : cBuff<tCHAR,BuffSize>( (iObj*)0)  { init ( str, cp ); }
	cCharBuff( const cStringObj* str, tCODEPAGE cp ) : cBuff<tCHAR,BuffSize>( (iObj*)0 ) { str ? init ( *str, cp ):init(0, cp ); }

	cCharBuff( const cString* str=NULL, tCODEPAGE cp=cCP_ANSI, tDWORD pos=0 ) : cBuff<tCHAR,BuffSize>((iObj*)str)     { init(str,cp,pos); }
	
	cCharBuff( const tCHAR* str ) : cBuff<tCHAR,BuffSize>((iObj*)0) { init(str); }
	cCharBuff( const tWCHAR* str ) : cBuff<tCHAR,BuffSize>((iObj*)0) { init(str); }
	
	tERROR init( const cString* str, tCODEPAGE cp=cCP_ANSI, tDWORD pos=0 );
	tERROR init( const cStringObj& str, tCODEPAGE cp=cCP_ANSI, tDWORD pos=0 );
	tERROR init( const tCHAR* str );
	tERROR init( const tWCHAR* str );
	
	operator tWCHAR *(){ return (tWCHAR*)ptr(); }
};




// -----------------------------------------------------
template <int BuffSize>
class cWCharBuff : public cBuff<tWCHAR, BuffSize> {
public:
    typedef cBuff<tWCHAR, BuffSize> Base;
    using Base::m_used;
    using Base::ptr;
    using Base::size;
	cWCharBuff( const cStringObj& str )                 : cBuff<tWCHAR,BuffSize>((iObj*)0)    { init( str ); }
	cWCharBuff( const cStringObj* str )                 : cBuff<tWCHAR,BuffSize>((iObj*)0)    { str ? init( *str ) : init ( 0 ); }
	cWCharBuff( const cString* str=NULL, tDWORD pos=0 ) : cBuff<tWCHAR,BuffSize>((iObj*)str)  { init(str,pos); }
	
	cWCharBuff( const tWCHAR* str ) : cBuff<tWCHAR,BuffSize>((iObj*)0) { init(str); }
	
	tERROR init( const cString* str, tDWORD pos=0 );
	tERROR init( const cStringObj& str, tDWORD pos=0 );
	tERROR init( const tWCHAR* str );// { cStrObj lstr(str); return init(lstr); }
};

// -----------------------------------------------------


class cStringObj {
public:
	enum { npos = cSTRING_EMPTY_LENGTH, whole = cSTRING_WHOLE_LENGTH };


	explicit cStringObj(const iObj* dad = 0 )       {}

	cStringObj(const tCHAR *str)                    { assign ( str, cCP_ANSI ); } 
	cStringObj(const tWCHAR *str)                   { assign ( str, cCP_UNICODE ); }
	cStringObj(const iObj* obj, tUINT propid )      { if ( obj ) assign(const_cast<iObj*>(obj),propid); } 
	cStringObj(const tVOID* str, tCODEPAGE cp )     { assign( str, cp ); }

	cStringObj(const cString* str, tDWORD pos = 0, tDWORD size = whole )    { assign ( str, pos, size ); }
	cStringObj(const cStringObj& str, tDWORD pos = 0, tDWORD size = whole ) { assign ( str, pos, size ); }

	explicit cStringObj(tCODEPAGE cp)               {}  
	~cStringObj()                                   { deallocate (); }
	tERROR    init ( const iObj* dad = 0 )          { return errOK; }
	tERROR    clear()                               { return clean ( 0, whole ); }
	tERROR    clean(tDWORD pos=0, tDWORD size=whole);
	tERROR    erase(tDWORD pos=0, tDWORD size=whole){ return clean(pos,size);	}
	bool      empty()                         const { return m_str.m_len == 0; }
	tCODEPAGE CP()                            const { return cCP_UNICODE; }
	tERROR    setCP( tCODEPAGE cp )                 { return errOK; }

	tSYMBOL  operator[](tDWORD idx) const           { return at(idx); }
	const tWCHAR* data()                      const { return getData(); }

	cStringObj& operator = (const cStringObj& c)    { assign(c); return *this; }
	cStringObj& operator = (const cString* c)       { assign(c); return *this; }
	cStringObj& operator = (const tCHAR *str)       { assign((tPTR)str,cCP_ANSI); return *this; }
	cStringObj& operator = (const tWCHAR *str)      { assign((tPTR)str,cCP_UNICODE); return *this; }

	bool operator == (const cStringObj& str)  const { return !compare(str, 0); }
	bool operator == (const tCHAR* str)       const { return !compare(str); }
	bool operator == (const tWCHAR* str)      const { return !compare(str); }
	bool operator == (cString* str)           const { return !compare(0, whole, str); }
	bool operator != (const cStringObj& str)  const { return !(operator == (str)); }
	bool operator != (const tCHAR* str)       const { return !(operator == (str)); }
	bool operator != (const tWCHAR* str)      const { return !(operator == (str)); }
	bool operator != (cString* str)           const { return !(operator == (str)); }

	bool operator < (const cStringObj& str)   const { return compare(str, 0) == cSTRING_COMP_LT; }
	bool operator < (const tCHAR* str)        const { return compare(str) == cSTRING_COMP_LT; }
	bool operator < (const tWCHAR* str)       const { return compare(str) == cSTRING_COMP_LT; }

	cStringObj& operator += (const cStringObj& str) { append(str); return *this; }
	cStringObj& operator += (tCHAR symb)            { append(symb); return *this; }
	cStringObj& operator += (const tSTRING str)     { append((const tCHAR*)str); return *this; }
	cStringObj& operator += (const tWSTRING str)    { append((const tWCHAR*)str); return *this; }
	cStringObj& operator += (const tCHAR* str)      { append(str); return *this; }
	cStringObj& operator += (const tWCHAR* str)     { append(str); return *this; }
	cStringObj& operator += (cString* str)          { append(str); return *this; }

	tDWORD length()                           const { return m_str.m_len; }
	tDWORD size()                             const { return length(); }

	cStrBuff c_str( tCODEPAGE cp )            const { return cStrBuff ( *this, cp ); } 

	tSYMBOL      at( tDWORD idx, tCODEPAGE cp=cCP_SYSTEM_DEFAULT, tDWORD flags=0 ) const;
	tDWORD       memsize( tCODEPAGE cp ) const;

	tERROR       assign( const cStringObj& str, tDWORD pos=0, tDWORD size=whole );
	tERROR       assign( const cString *str, tDWORD pos=0, tDWORD size=whole );
	tERROR       assign( iObj *obj, tPROPID propid );
	tERROR       assign( const tVOID* buff, tCODEPAGE cp, tDWORD len = 0 );
	tERROR       assign( cRegistry *reg, tRegKey key, const tCHAR* val_name );

	tERROR       copy( cStringObj& str, tDWORD pos=0, tDWORD size=whole ) const;
	tERROR       copy( tVOID* buff, tDWORD buff_size, tCODEPAGE cp, tDWORD pos=0, tDWORD size=whole ) const;
	tERROR       copy( tCHAR* buff, tDWORD buff_size, tDWORD pos=0, tDWORD size=whole ) const;
	tERROR       copy( tWCHAR* buff, tDWORD buff_size, tDWORD pos=0, tDWORD size=whole ) const;
	tERROR       copy( iObj *obj, tPROPID propid, tDWORD pos=0, tDWORD size=whole ) const;
	tERROR       copy( cString *str, tDWORD pos=0, tDWORD size=whole ) const;
	tERROR       copy( cRegistry* reg, tRegKey key, const tCHAR* val_name, tBOOL create_if_not_exist ) const;

	tERROR       append( const cStringObj& str );
	tERROR       append( tCHAR symb );
	tERROR       append( const tVOID* str, tCODEPAGE cp, tDWORD len = 0 );
	tERROR       append( const tCHAR* str );
	tERROR       append( const tWCHAR* str );
	tERROR       append( const cString* str );

	tSTR_COMPARE compare( tDWORD pos, tDWORD size, const cStringObj& other_str, tDWORD other_pos=0, tDWORD other_size=whole, tDWORD flags=0 ) const;
	tSTR_COMPARE compare( tDWORD pos, tDWORD size, const cString* other_str, tDWORD other_pos=0, tDWORD other_size=whole, tDWORD flags=0 ) const;
	tSTR_COMPARE compare( tDWORD pos, tDWORD size, const tCHAR* str, tDWORD flags=0 ) const;
	tSTR_COMPARE compare( const cStringObj& str, tDWORD flags=0 ) const;
	tSTR_COMPARE compare( const cString* str, tDWORD flags=0 ) const;
	tSTR_COMPARE compare( const tCHAR* str, tDWORD flags=0 ) const;
	tSTR_COMPARE compare( const tWCHAR* str, tDWORD flags=0 ) const;

	tERROR       replace_one( const cStringObj& pattern, tDWORD pattern_flags, const cStringObj& replace_with );
	tERROR       replace_one( const tVOID* pattern, tDWORD pattern_flags, const tVOID* replace_with, tCODEPAGE cp );
	tERROR       replace_one( const cString* pattern, tDWORD pattern_flags, const cString* replace_with );
	tERROR       replace_one( const tCHAR* pattern, tDWORD pattern_flags, const tCHAR* replace_with );
	tERROR       replace_one( const tWCHAR* pattern, tDWORD pattern_flags, const tWCHAR* replace_with );

	tERROR       replace_all( const cStringObj& pattern, tDWORD pattern_flags, const cStringObj& replace_with );
	tERROR       replace_all( const tVOID* pattern, tDWORD pattern_flags, const tVOID* replace_with, tCODEPAGE cp );
	tERROR       replace_all( const cString* pattern, tDWORD pattern_flags, const cString* replace_with );
	tERROR       replace_all( const tCHAR* pattern, tDWORD pattern_flags, const tCHAR* replace_with );
	tERROR       replace_all( const tWCHAR* pattern, tDWORD pattern_flags, const tWCHAR* replace_with );

	tDWORD       find( const cStringObj& str, tDWORD pos=0, tDWORD size=whole, tDWORD flags=fSTRING_FIND_FORWARD ) const;
	tDWORD       find( const cString* str, tDWORD pos=0, tDWORD size=whole, tDWORD flags=fSTRING_FIND_FORWARD ) const;
	tDWORD       find( const tCHAR* str, tDWORD pos=0, tDWORD size=whole, tDWORD flags=fSTRING_FIND_FORWARD ) const;
	tDWORD       find( const tWCHAR* str, tDWORD pos=0, tDWORD size=whole, tDWORD flags=fSTRING_FIND_FORWARD ) const;
	tDWORD       find( const tVOID* str, tCODEPAGE cp, tDWORD pos=0, tDWORD size=whole, tDWORD flags=fSTRING_FIND_FORWARD ) const;
	tDWORD       find_first_of( const tCHAR* symb_set, tDWORD pos=0, tDWORD size=whole ) const;
	tDWORD       find_last_of( const tCHAR* symb_set, tDWORD pos=0, tDWORD size=whole ) const;
	tDWORD       find_first_not_of( const tCHAR* symb_set, tDWORD pos=0, tDWORD size=whole ) const;
	tDWORD       find_last_not_of( const tCHAR* symb_set, tDWORD pos=0, tDWORD size=whole ) const;
	tDWORD       find_first_of( const tWCHAR* symb_set, tDWORD pos=0, tDWORD size=whole ) const;
	tDWORD       find_last_of( const tWCHAR* symb_set, tDWORD pos=0, tDWORD size=whole ) const;
	tDWORD       find_first_not_of( const tWCHAR* symb_set, tDWORD pos=0, tDWORD size=whole ) const;
	tDWORD       find_last_not_of( const tWCHAR* symb_set, tDWORD pos=0, tDWORD size=whole ) const;

	tERROR       toupper();
	tERROR       tolower();

	cStringObj   substr(tDWORD pos=0, tDWORD size=whole) const;

	tERROR       check_last_slash(bool enable = true);
	tERROR       set_path( const cStringObj& path );
	tERROR       set_path( const tVOID* path, tCODEPAGE cp, tDWORD len = 0 );
	tERROR       set_path( cString* path );
	tERROR       add_path_sect( const cStringObj& sect );
	tERROR       add_path_sect( const tVOID* sect, tCODEPAGE cp, tDWORD len = 0 );
	tERROR       add_path_sect( const cString* sect );

	tERROR       format( tCODEPAGE format_cp, const tVOID* format_str, ... );
	tERROR       format_v( tCODEPAGE format_cp, const tVOID* format_str, tPTR args );

	tERROR       resize( tDWORD size );
	tERROR       allocate( tDWORD size );
	void         deallocate();
private:
	sString m_str;

	tERROR       checkRange( tDWORD& pos, tDWORD& size, bool evaluate_size = true) const;
	void         checkLength();
	void         swap( cStringObj& str );
	tWCHAR *     getData() const { static tWCHAR g_empty = 0; return ( m_str.m_data ) ? m_str.m_data : &g_empty; }
};

class cAutoString : public cAutoObj<cString>
{
	typedef cAutoObj<cString> Base;
public:
	explicit cAutoString ( const tCHAR* content ) : m_str(0) { if (content && PR_SUCC(init())) m_obj->ImportFromBuff(0,(tPTR)content,0,cCP_ANSI,cSTRING_Z); }
	explicit cAutoString ( const tWCHAR* content ) : m_str(0) { if (content && PR_SUCC(init())) { m_obj->ImportFromBuff(0,(tPTR)content,0,cCP_UNICODE,cSTRING_Z); } }
	explicit cAutoString ( bool createString = true ) : m_str ( 0 ) { if ( createString ) init (); };
	explicit cAutoString ( const cStringObj& l_str, bool createString = true ) : m_str ( const_cast <cStringObj*> ( &l_str ) ) { if ( createString ) init (); };
	cAutoString ( cAutoString& l_str ) { operator = ( l_str ); }
	~cAutoString () { if ( m_str ) m_str->assign ( m_obj ); }
	cAutoString& operator =( cAutoString& l_str ) { 
		m_str = l_str.m_str;
		Base::operator = ( l_str );
		return *this;
	}
	tERROR init () {
		if ( m_obj ) 
			return errOBJECT_ALREADY_EXISTS;
		tERROR l_err = g_root->sysCreateObjectQuick ( reinterpret_cast <cObject**> ( &m_obj ), IID_STRING );
		if ( PR_FAIL ( l_err ) )
			return l_err;
		if ( m_str ) 
			l_err = m_str->copy ( m_obj );
		return l_err;
	};
	cAutoString& operator = ( const tCHAR* content ) { init(); if ( m_obj ) m_obj->ImportFromBuff(0,(tPTR)content,0,cCP_ANSI,cSTRING_Z); if ( m_str ) { *m_str = content; } return *this; }
	cAutoString& operator = ( const tWCHAR* content ) { init(); if ( m_obj ) m_obj->ImportFromBuff(0,(tPTR)content,0,cCP_UNICODE,cSTRING_Z); if ( m_str ) { *m_str = content; } return *this; }

private:
	cStringObj*  m_str;
};

// ---
template <int BuffSize>
cCharBuff<BuffSize> c_str( const cString* str, tCODEPAGE cp ) { 
	return cCharBuff<BuffSize>(str,cp); 
}

// ---
template <int BuffSize>
cCharBuff<BuffSize> c_str( const cStringObj& str, tCODEPAGE cp ) { 
	return cCharBuff<BuffSize>(str,cp); 
}

// -----------------------------------------------------
class cStringPtr {

private:
	cStringObj& m_str;
	tUINT       m_pos;

public:
	
	enum {  npos = cStringObj::npos, whole = cStringObj::whole };

	cStringPtr(cStringObj& str, tUINT pos=0) : m_str(str), m_pos(pos){}

	cStringPtr& operator = (const cStringPtr& c) { m_str = c.m_str; m_pos = c.m_pos; return *this; }
		
	operator tUINT()                { return m_pos; }
	
	tINT operator ++ ()             { return ++m_pos; }
	tINT operator ++ (int)          { return m_pos++; }
	tINT operator -- ()             { return --m_pos; }
	tINT operator -- (int)          { return m_pos--; }
	tINT operator - (cStringPtr &p) { return m_pos - p.m_pos; }
	
	cStringPtr operator + (int p)   { return cStringPtr(m_str, m_pos + p); }
	cStringPtr operator - (int p)   { return cStringPtr(m_str, m_pos - p); }
	
	tSYMBOL operator *() const                { return at(0); }
	tSYMBOL operator[](int idx) const         { return at(idx); }
	
	tDWORD size() const                       { return m_str.size()-m_pos; }

	tSYMBOL at( tDWORD idx, tCODEPAGE cp=cCP_SYSTEM_DEFAULT, tDWORD flags=0 ) const { return m_str.at(m_pos+idx, cp, flags); }
        //        cStrBuff c_str(tCODEPAGE cp) const        { return cStrBuff(m_str, cp, m_pos); }
	
	tERROR copy(iObj *obj, tPROPID propid)    { return m_str.copy(obj, propid, m_pos); }
	tERROR copy(cString *str)                 { return m_str.copy(str, m_pos); }

    cStringPtr find_last_of(tSTRING symb_set) { return cStringPtr(m_str, m_str.find_last_of(symb_set, m_pos)); }
	cStringPtr find_last_of(tWSTRING symb_set) { return cStringPtr(m_str, m_str.find_last_of(symb_set, m_pos)); }	
    cStringPtr find(cStringPtr &str)          { return cStringPtr(m_str, m_str.find(str.m_str, m_pos)); }

	tSTR_COMPARE compare(tSTRING str, tDWORD size=whole, tDWORD flags=0) const     { return m_str.compare(m_pos, size, str, flags);}
	tSTR_COMPARE compare(const cStringPtr &str, tDWORD size=whole, tDWORD flags=0) const { return m_str.compare(m_pos, size, str.m_str, str.m_pos, size, flags); }
};



// ---
inline tPTR cMemChunk::reserve( tUINT size, bool realloc ) {
	if ( size == (tUINT)-1 )
		return m_ptr;
	
	if ( size > m_allocated ) {
		if ( realloc )
			const_cast<iObj*>(m_allocator)->heapRealloc( &m_ptr, m_ptr, size );
		else {
			if ( m_ptr )
				const_cast<iObj*>(m_allocator)->heapFree( m_ptr );
			const_cast<iObj*>(m_allocator)->heapAlloc( &m_ptr, size );
		}
		if ( !m_ptr )
			size = 0;
		m_allocated = size;
	}
	m_used = size;
	return m_ptr;
}



// ---
inline tPTR cMemChunk::exact_size( tUINT count, bool realloc ) {
	if ( !count ) {
		clean();
		return 0;
	}

	if ( count == (tUINT)-1 )
		count = m_used;
	if ( count == m_allocated )
		return m_ptr;
	if ( count > m_allocated )
		return reserve( count, realloc );
	if ( realloc )
		const_cast<iObj*>(m_allocator)->heapRealloc( &m_ptr, m_ptr, count );
	else {
		if ( m_ptr )
			const_cast<iObj*>(m_allocator)->heapFree( m_ptr );
		const_cast<iObj*>(m_allocator)->heapAlloc( &m_ptr, count );
	}
	if ( !m_ptr )
		count = 0;
	m_allocated = m_used = count;
	return m_ptr;
}


// ---
inline void cMemChunk::clean() {
	if ( m_ptr )
		const_cast<iObj*>(m_allocator)->heapFree( m_ptr );
	m_ptr = 0;
	m_used = 0;
	m_allocated = 0;
}


// -----------------------------------------------------
// cBuff members
template <class Type, int BSIZE>
tPTR cBuff<Type,BSIZE>::reserve( tUINT count, bool realloc ) {
	
	if ( count == (tUINT)-1 )
		return m_ptr;
	
	tUINT size = count * sizeof(Type);
	if ( size > m_allocated ) {
		tUINT copy = 0;
		if ( m_ptr == m_buff ) {
			if ( realloc )
				copy = m_used;
			m_ptr = 0;
		}
		cMemChunk::reserve( size, realloc );
		if ( m_ptr && copy )
			memcpy( m_ptr, m_buff, copy );
	}
	if ( m_ptr )
		m_used = size;
	return m_ptr;
}


// ---
template <class Type, int BSIZE>
tPTR cBuff<Type,BSIZE>::exact_size( tUINT count, bool realloc ) {
	if ( count != (tUINT)-1 )
		count *= sizeof(Type);
	return exact_size( count, realloc );
}


// ---
template <class Type, int BSIZE>
void cBuff<Type,BSIZE>::clean() {
	if ( m_ptr != m_buff )
		const_cast<iObj*>(m_allocator)->heapFree( m_ptr );
	m_ptr = m_buff;
	m_used = 0;
	m_allocated = BSIZE * sizeof(Type);
}


// ---
template <class Type, int BSIZE>
inline Type* cBuff<Type,BSIZE>::get( tUINT count, bool realloc ) {
	return (Type*)reserve( count, realloc );
}


	
// ---
template <class Type, int BSIZE>
Type* cBuff<Type,BSIZE>::set( tUINT count, const Type* src ) {
	Type* ptr = get( count + 1, false );
	if ( ptr ) {
		memcpy( ptr,       src, count * sizeof(Type) );
		memset( ptr+count, 0,   sizeof(Type) );
		m_used -= sizeof(Type);
	}
	return ptr;
}

// ---
template <class Type, int BSIZE>
Type* cBuff<Type,BSIZE>::add( tUINT count ) {
	tUINT used = cBuff<Type,BSIZE>::used();
	Type* ptr = get( used + count, true );
	if ( ptr )
		return ptr + used;
	return 0;
}

// ---
template <class Type, int BSIZE>
Type* cBuff<Type,BSIZE>::add( tUINT count, const Type* src ) {
	Type* ptr = add( count + 1 );
	if ( ptr ) {
		memcpy( ptr,       src, count );
		memset( ptr+count, 0,   sizeof(Type) );
		m_used -= sizeof(Type);
	}
	return ptr;
}

// ---
template <class Type, int BSIZE>
Type* cBuff<Type,BSIZE>::once_more() {
	Type* ptr;
	if ( m_used == m_allocated ) {
		tUINT used = m_used;
		ptr = add( BSIZE );
		if ( ptr )
			m_used = used + sizeof(Type);
	}
	else {
		ptr = const_cast<Type*>(cBuff<Type,BSIZE>::ptr()) + used();
		m_used += sizeof(Type);
	}
	return ptr;
}

// ----------------------------------------------------------------

template<typename T>
tDWORD strSize( const T* str )
{
	if ( !str )
	return 0;
	tDWORD l_len = 0;
	while ( *str++ ) 
            l_len++;
	return l_len;
}

// ----------------------------------------------------------------
// cStringObj inlines

inline tERROR cStringObj::resize( tDWORD size )
{
	if( m_str.m_len == size )
		return errOK;
	
	tERROR err = allocate(size);
	if( PR_FAIL(err) )
		return err;
	
	if( !m_str.m_data )
		return errOK;
	
	tWCHAR * p = m_str.m_data;
	p[m_str.m_len] = 0;
	m_str.m_len = size;
	p[size] = 0;
	return errOK;
}

inline tERROR cStringObj::allocate ( tDWORD size )
{
	if( !m_str.m_data )
	{
		tERROR l_err = g_root->heapAlloc ( reinterpret_cast <tVOID**> ( &m_str.m_data ), ( size + 1 ) * sizeof ( tWCHAR ));
		if ( PR_FAIL ( l_err ) )
			return l_err;
		m_str.m_size = size;
		return errOK;
	}
	
	if( m_str.m_size < size )
	{
		size += size/2;
		tWCHAR* l_data = 0;  
		tERROR l_err = g_root->heapRealloc ( reinterpret_cast <tVOID**> ( &l_data ), m_str.m_data, ( size + 1 ) * sizeof ( tWCHAR ) );
		if ( PR_FAIL ( l_err ) )
			return l_err;
		m_str.m_data = l_data;
		m_str.m_size = size;
		return errOK;
	}
	
	return errOK;
}

inline void cStringObj::deallocate (){
  if ( m_str.m_data )
    g_root->heapFree ( m_str.m_data );
  m_str.m_data = 0;
  m_str.m_len = m_str.m_size = 0;
}

inline tERROR cStringObj::checkRange ( tDWORD& pos, tDWORD& size, bool evaluate_size ) const
{
  if ( pos > m_str.m_len )
    return errSTR_OUT_OF_RANGE;
  if ( size == whole ) {
    if ( evaluate_size )
      size = m_str.m_len - pos;
    else
      size = 0;
  }
  if ( ( pos + size ) > m_str.m_len )
    return errSTR_OUT_OF_RANGE;
  return errOK;
}

inline void cStringObj::checkLength ()
{
  if ( ( m_str.m_data == 0 ) || ( m_str.m_len == 0 ) )
    return;
  memset ( m_str.m_data + m_str.m_len, 0, ( m_str.m_size - m_str.m_len ) * sizeof ( tWCHAR ) );
  while ( m_str.m_len && ( m_str.m_data [ m_str.m_len - 1 ] == 0 ) )
    --m_str.m_len;
}

inline void cStringObj::swap ( cStringObj& str ) {
  tWCHAR* l_str = m_str.m_data;
  m_str.m_data = str.m_str.m_data;
  str.m_str.m_data = l_str;
  tDWORD l_tmp = m_str.m_size;
  m_str.m_size = str.m_str.m_size;
  str.m_str.m_size = l_tmp;
  l_tmp = m_str.m_len;
  m_str.m_len = str.m_str.m_len;
  str.m_str.m_len = l_tmp;
}

inline tERROR cStringObj::clean(tDWORD pos, tDWORD size ) { 
  tERROR l_err = checkRange ( pos, size );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( ( m_str.m_data == 0 ) || ( m_str.m_len == 0 ) )
    return errOK;
  if ( ( pos == 0 ) && ( size == m_str.m_len ) ) {
    memset ( m_str.m_data, 0, m_str.m_len * sizeof ( tWCHAR ) ); 
    m_str.m_len = 0;
    return errOK;
  }
  memcpy ( m_str.m_data + pos, m_str.m_data + pos + size, ( m_str.m_len - pos - size ) * sizeof ( tWCHAR ) );
  m_str.m_len -= size;
  memset ( m_str.m_data + m_str.m_len, 0, ( m_str.m_size - m_str.m_len ) * sizeof ( tWCHAR ) ); 
  return errOK;
}

inline tSYMBOL cStringObj::at( tDWORD idx, tCODEPAGE cp, tDWORD flags ) const
{
	if( !m_str.m_data || ( idx > m_str.m_len ) )
		return 0;

	tSYMBOL l_sym = m_str.m_data [idx];
	
	if( cp != cCP_UNICODE )
	{
		if( !PR_CPP_CF(CopyTo, 0x10a92e27l) )
			return 0;
		tSYMBOL l_src = l_sym;
		if( PR_FAIL(CopyTo(&l_sym, sizeof(l_sym), cp, &l_src, sizeof(l_src), cCP_UNICODE, cSTRING_CONTENT_ONLY, 0)) )
			return 0;
	}

	if( flags )
	{
		if( flags & cSTRING_TO_UPPER )
		{
			if( !PR_CPP_CF(ToUpperCase, 0xdae8a8d0l) )
				return 0;
			if( PR_FAIL(ToUpperCase(&l_sym, sizeof(l_sym), cp)) )
				return 0;
		}
		else if( flags & cSTRING_TO_LOWER )
		{
			if( !PR_CPP_CF(ToLowerCase, 0xba2d0c6fl) )
				return 0;
			if( PR_FAIL(ToLowerCase(&l_sym, sizeof(l_sym), cp)) )
				return 0;
		}
	}

	return l_sym;
}

// ---
inline tDWORD cStringObj::memsize( tCODEPAGE cp ) const {
  tDWORD l_size = m_str.m_len;
  if ( !l_size )
    return 0;
  l_size = ( l_size + 1 ) * sizeof ( tWCHAR );
  
  if ( cp == cCP_UNICODE )
    return l_size;
  if ( !PR_CPP_CF ( ConvertLen, 0xab7270acl ) )
    return 0;
  if ( PR_FAIL ( ConvertLen ( l_size, &l_size, cCP_UNICODE, cp ) ) )
    return 0;
  return l_size;
}

// ---
inline tERROR cStringObj::assign( const cStringObj& str, tDWORD pos, tDWORD size ) {
  if ( str.empty () ) {
    clear ();
    return errOK;
  }  
  if ( !empty () ) {
    cStringObj l_str;
    tERROR l_err = l_str.assign ( str, pos, size );
    if ( PR_FAIL ( l_err ) )
      return l_err;
    swap ( l_str );
    return errOK;
  }
  tERROR l_err = str.checkRange ( pos, size );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  l_err = allocate ( size );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  memcpy ( m_str.m_data, str.data () + pos, size * sizeof ( tWCHAR ) );
  m_str.m_len = size;
  checkLength ();
  return errOK;
}

// ---
inline tERROR cStringObj::assign( const cString *str, tDWORD pos, tDWORD size ) {
  if ( !str ) {
    clear ();
    return errOK;
  }
  if ( !empty () ) {
    cStringObj l_str;
    tERROR l_err = l_str.assign ( str, pos, size );
    if ( PR_FAIL ( l_err ) )
      return l_err;
    swap ( l_str );
    return errOK;
  }
  tDWORD l_size = 0;
  tERROR l_err = CALL_String_LengthEx ( const_cast <cString*>(str), &l_size, STR_RANGE(pos, size), cCP_UNICODE, cSTRING_CONTENT_ONLY );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( l_size == 0 ) {
    clear ();
    return errOK;
  }
  l_err = allocate ( l_size / sizeof ( tWCHAR ) );  
  if ( PR_FAIL ( l_err ) )
    return l_err;
  l_err = CALL_String_ExportToBuff ( const_cast<cString*>(str), &l_size, STR_RANGE(pos, size), 
                                     m_str.m_data, l_size, cCP_UNICODE, cSTRING_CONTENT_ONLY );
  if ( PR_SUCC ( l_err ) || ( l_err != errBUFFER_TOO_SMALL ) )
    m_str.m_len = l_size / sizeof ( tWCHAR );
  checkLength ();
  return l_err;
}

// ---
inline tERROR cStringObj::assign( iObj *obj, tPROPID propid ) {
  if ( !obj ) {
    clear ();
    return errOK;
  }
  if ( !empty () ) {
    cStringObj l_str;
    tERROR l_err = l_str.assign ( obj, propid );
    if ( PR_FAIL ( l_err ) )
      return l_err;
    swap ( l_str );
    return errOK;
  }

  if ( ( ( propid & pTYPE_MASK ) != pTYPE_STRING ) && 
       ( ( propid&pTYPE_MASK ) != pTYPE_WSTRING ) )
    return errPARAMETER_INVALID;

  tDWORD l_size = 0;
  tERROR l_err = CALL_SYS_PropertyGetStr ( obj, &l_size, propid, 0, 0, cCP_UNICODE );
  if ( PR_FAIL ( l_err ) && ( l_err != errBUFFER_TOO_SMALL ) )
    return l_err;
  if ( l_size == 0 ) {
    clear ();
	  return errOK;
  }
  l_err = allocate ( l_size / sizeof ( tWCHAR ) );  
  if ( PR_FAIL ( l_err ) )
    return l_err;
  l_err = CALL_SYS_PropertyGetStr ( obj, &l_size, propid, m_str.m_data, l_size, cCP_UNICODE );
  if ( PR_SUCC ( l_err ) || ( l_err != errBUFFER_TOO_SMALL ) )
    m_str.m_len = l_size / sizeof ( tWCHAR );
  checkLength ();
  return l_err;
}

// ---
inline tERROR cStringObj::assign( const tVOID* buff, tCODEPAGE cp, tDWORD len ) {
  if( !buff ) {
    clear ();
    return errOK;
  }
  if ( !empty () ) {
    cStringObj l_str;
    tERROR l_err = l_str.assign ( buff, cp, len );
    if ( PR_FAIL ( l_err ) )
      return l_err;
    swap ( l_str );
    return errOK;
  }

  tDWORD l_len = 0;
  if ( !PR_CPP_CF ( CopyTo, 0x10a92e27l ) || !PR_CPP_CF ( CalcImportLen, 0x5c806ee3l ) || !PR_CPP_CF ( CalcExportLen, 0x8eefa2b8l ) )
    return errUNEXPECTED;
  tERROR l_err = CalcImportLen ( const_cast<tVOID*> ( buff ), cp, cSTRING_CONTENT_ONLY, len, &l_len );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( l_len == 0 )
    return errOK;
  tDWORD l_size = 0;
  l_err = CalcExportLen ( const_cast <tVOID*> ( buff ), l_len, cp, cCP_UNICODE, cSTRING_CONTENT_ONLY, &l_size );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( l_size == 0 ) {
    clear ();
    return errOK;
  }
  l_err = allocate ( l_size / sizeof ( tWCHAR ) );  
  if ( PR_FAIL ( l_err ) )
    return l_err;
  l_err = CopyTo ( m_str.m_data, l_size, cCP_UNICODE, const_cast <tVOID*> ( buff ), l_len, cp, cSTRING_CONTENT_ONLY, &l_size );
  if ( PR_SUCC ( l_err ) || ( l_err != errBUFFER_TOO_SMALL ) )
    m_str.m_len = l_size / sizeof ( tWCHAR );
  checkLength ();
  return l_err;
}

// ---
inline tERROR cStringObj::assign( cRegistry *reg, tRegKey key, const tCHAR* val_name ) {
  if ( !reg ) {
    clear ();
    return errOK;
  }
  if ( !empty () ) {
    cStringObj l_str;
    tERROR l_err = l_str.assign ( reg, key, val_name );
    if ( PR_FAIL ( l_err ) )
      return l_err;
    swap ( l_str );
    return errOK;
  }
  tTYPE_ID l_type = tid_WSTRING;
  tDWORD l_size = 0;
  tERROR l_err = CALL_Registry_GetValue( reg, &l_size, key, val_name, &l_type, 0, 0 );
  if ( PR_FAIL ( l_err ) && ( l_err != errBUFFER_TOO_SMALL ) )
    return l_err;
  if ( ( l_size == sizeof ( tWCHAR ) ) || ( l_size == 0 ) ) {
    clear ();
    return errOK;
  }
	if ( l_type == tid_WSTRING ) {
		l_err = allocate ( l_size / sizeof ( tWCHAR ) );  
		if ( PR_FAIL ( l_err ) )
			return l_err;
		l_err = CALL_Registry_GetValue( reg, &l_size, key, val_name, &l_type, m_str.m_data, l_size );
		if ( PR_SUCC ( l_err ) || ( l_err != errBUFFER_TOO_SMALL ) )
			m_str.m_len = l_size / sizeof ( tWCHAR );
	}
	else if ( l_type == tid_STRING ) {
		tChunckBuff buff( reg );
		tCHAR* ptr = buff.get( l_size, false );
		if ( !ptr )
			return errNOT_ENOUGH_MEMORY;
		l_err = CALL_Registry_GetValue( reg, &l_size, key, val_name, &l_type, ptr, l_size );
		if ( PR_SUCC(l_err) || (l_err != errBUFFER_TOO_SMALL) )
			l_err = assign( ptr, cCP_ANSI, l_size );
	}
	else
		l_err = errUNEXPECTED;
  checkLength();
  return l_err;
}

// ---
inline tERROR cStringObj::copy( cStringObj& str, tDWORD pos, tDWORD size ) const {
  return str.assign ( *this, pos, size );
}

// ---
inline tERROR cStringObj::copy( tVOID* buff, tDWORD buff_size, tCODEPAGE cp, tDWORD pos, tDWORD size ) const {
  if ( !buff )
    return errPARAMETER_INVALID;
  tERROR l_err = checkRange ( pos, size, false );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( !PR_CPP_CF ( CopyTo, 0x10a92e27l ) )
    return errUNEXPECTED;
  return CopyTo ( buff, buff_size, cp, getData () + pos, size * sizeof ( tWCHAR ), cCP_UNICODE, cSTRING_Z, 0 );
}

// ---
inline tERROR cStringObj::copy( tCHAR* buff, tDWORD buff_size, tDWORD pos, tDWORD size ) const {
  return copy ( buff, buff_size, cCP_ANSI, pos, size );
}

// ---
inline tERROR cStringObj::copy( tWCHAR* buff, tDWORD buff_size, tDWORD pos, tDWORD size ) const {
  return copy ( buff, buff_size, cCP_UNICODE, pos, size );
}

// ---
inline tERROR cStringObj::copy( iObj *obj, tPROPID propid, tDWORD pos, tDWORD size ) const {
  if ( !obj )
    return errPARAMETER_INVALID;
  tERROR l_err = checkRange ( pos, size, false );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  return obj->propSetStr( 0, propid, getData () + pos, size * sizeof ( tWCHAR ), cCP_UNICODE );
}

// ---
inline tERROR cStringObj::copy( cString *str, tDWORD pos, tDWORD size ) const {
  if ( !str )
    return errPARAMETER_INVALID;
  tERROR l_err = checkRange ( pos, size, false );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  return CALL_String_ImportFromBuff ( str, 0, getData() + pos, size * sizeof ( tWCHAR ), cCP_UNICODE, cSTRING_Z );
}

// ---
inline tERROR cStringObj::copy( cRegistry* reg, tRegKey key, const tCHAR* val_name, tBOOL create_if_not_exist ) const {
  if ( !reg )
    return errPARAMETER_INVALID;
  return CALL_Registry_SetValue( reg, key, val_name, tid_WSTRING, getData (), ( m_str.m_len + 1 ) * sizeof ( tWCHAR ), create_if_not_exist );
}

// ---
inline tERROR cStringObj::append( const cStringObj& str ) {
  tDWORD size = m_str.m_len + str.m_str.m_len;
  tERROR l_err = allocate ( size );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  memcpy ( m_str.m_data + m_str.m_len, str.m_str.m_data, str.m_str.m_len * sizeof ( tWCHAR ) ); 
  m_str.m_len += str.m_str.m_len;
  checkLength ();
  return errOK;
}

// ---
inline tERROR cStringObj::append( tCHAR symb ) {
  if ( !PR_CPP_CF ( CopyTo, 0x10a92e27l ) )
    return errUNEXPECTED;
  tERROR l_err = allocate ( m_str.m_len + 1 );  
  if ( PR_FAIL ( l_err ) )
    return l_err;
  tDWORD l_size = 0;
  l_err = CopyTo ( m_str.m_data + m_str.m_len, sizeof ( tWCHAR ) ,cCP_UNICODE, &symb, sizeof ( symb ), cCP_ANSI, cSTRING_CONTENT_ONLY, &l_size );  
  m_str.m_len += l_size / sizeof ( tWCHAR );
  checkLength ();
  return l_err;
}

// ---
inline tERROR cStringObj::append( const tVOID* str, tCODEPAGE cp, tDWORD len ) {
  if ( !str )
    return errOK;
  
  tDWORD l_len = 0;
  if ( !PR_CPP_CF ( CopyTo, 0x10a92e27l ) || !PR_CPP_CF ( CalcImportLen, 0x5c806ee3l ) || !PR_CPP_CF ( CalcExportLen, 0x8eefa2b8l ) )
    return errUNEXPECTED;
  tERROR l_err = CalcImportLen ( const_cast<tVOID*> ( str ), cp, cSTRING_CONTENT_ONLY, len, &l_len );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( l_len == 0 )
    return errOK;
  tDWORD l_size = 0;
  l_err = CalcExportLen ( const_cast <tVOID*> ( str ), l_len, cp, cCP_UNICODE, cSTRING_CONTENT_ONLY, &l_size );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( l_size == 0 )
    return errOK;
  l_err = allocate ( m_str.m_len + l_size / sizeof ( tWCHAR ) );  
  if ( PR_FAIL ( l_err ) )
    return l_err;
  l_err = CopyTo ( m_str.m_data + m_str.m_len, l_size, cCP_UNICODE, const_cast <tVOID*> ( str ), l_len, cp, cSTRING_CONTENT_ONLY, &l_size );
  if ( PR_SUCC ( l_err ) || ( l_err != errBUFFER_TOO_SMALL ) )
    m_str.m_len += l_size / sizeof ( tWCHAR );
  checkLength ();
  return l_err;
}

// ---
inline tERROR cStringObj::append( const tCHAR* str ) {  
  return append( str, cCP_ANSI );
}

// ---
inline tERROR cStringObj::append( const tWCHAR* str ) {
  return append( str, cCP_UNICODE );
}

// ---
inline tERROR cStringObj::append( const cString* str ) {
  if ( !str )
    return errOK;
  tDWORD l_size = 0;
  tERROR l_err = CALL_String_ExportToBuff ( const_cast<cString*>(str), &l_size, STR_RANGE(0, cSTRING_WHOLE_LENGTH), 0, 0, cCP_UNICODE, cSTRING_CONTENT_ONLY );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  if ( l_size == 0 )
    return errOK;
  l_err = allocate ( m_str.m_len + l_size / sizeof ( tWCHAR ) );  
  if ( PR_FAIL ( l_err ) )
    return l_err;
  l_err = CALL_String_ExportToBuff ( const_cast<cString*>(str), &l_size, STR_RANGE(0, cSTRING_WHOLE_LENGTH), 
                                     m_str.m_data + m_str.m_len, l_size, cCP_UNICODE, cSTRING_CONTENT_ONLY );
  if ( PR_SUCC ( l_err ) || ( l_err != errBUFFER_TOO_SMALL ) )
    m_str.m_len += l_size / sizeof ( tWCHAR );
  checkLength ();
  return l_err;
}

// ---
inline tSTR_COMPARE cStringObj::compare( tDWORD pos, tDWORD size, const cStringObj& other_str, tDWORD other_pos, tDWORD other_size, tDWORD flags ) const {
  tSTR_COMPARE l_res = cSTRING_COMP_UNDEF;
  if ( PR_FAIL ( other_str.checkRange ( other_pos, other_size, false ) ) )
    return l_res;
  if ( !PR_CPP_CF ( CompareBuff, 0xcb15477bl ) )
	  return l_res;
  CompareBuff ( &m_str, pos, size, other_str.m_str.m_data + other_pos, other_size * sizeof ( tWCHAR ), cCP_UNICODE, flags, &l_res );
  return l_res;  
}

// ---
inline tSTR_COMPARE cStringObj::compare( tDWORD pos, tDWORD size, const cString* other_str, tDWORD other_pos, tDWORD other_size, tDWORD flags ) const {
  tSTR_COMPARE l_res = cSTRING_COMP_UNDEF;
  if ( !PR_CPP_CF ( CompareStr, 0x65bbab8bl ) )
	  return l_res;  	
  CompareStr ( &m_str, pos, size, const_cast <cString*> ( other_str ), other_pos, other_size, flags, &l_res );
  return l_res;
}
// ---
inline tSTR_COMPARE cStringObj::compare( const cStringObj& str, tDWORD flags ) const {
  return compare ( 0, whole, str, 0, whole, flags );
}
// ---
inline tSTR_COMPARE cStringObj::compare( const cString* str, tDWORD flags ) const {
  return compare ( 0, whole, str, 0, whole, flags );
}

// ---
inline tSTR_COMPARE cStringObj::compare( const tCHAR* str, tDWORD flags ) const {
  return compare ( 0, whole, str, flags );
}

// ---
inline tSTR_COMPARE cStringObj::compare( const tWCHAR* str, tDWORD flags ) const {
  tSTR_COMPARE l_res = cSTRING_COMP_UNDEF;
  if ( !PR_CPP_CF ( CompareBuff, 0xcb15477bl ) )
	  return l_res;  	
  CompareBuff ( &m_str, 0, whole, str, 0, cCP_UNICODE, flags, &l_res );
  return l_res;  
}

// ---
inline tSTR_COMPARE cStringObj::compare( tDWORD pos, tDWORD size, const tCHAR* str, tDWORD flags ) const {
  tSTR_COMPARE l_res = cSTRING_COMP_UNDEF;
  if ( !PR_CPP_CF ( CompareBuff, 0xcb15477bl ) )
	  return l_res;  	
  CompareBuff ( &m_str, pos, size, str, 0, cCP_ANSI, flags, &l_res );
  return l_res;
}

// ---
inline tERROR cStringObj::replace_one( const cStringObj& pattern, tDWORD pattern_flags, const cStringObj& replace_with ) {
  if ( !PR_CPP_CF ( ReplaceBuff, 0x0d5e9cc5l ) )
	  return errUNEXPECTED;  	
  return ReplaceBuff ( &m_str, pattern.m_str.m_data, pattern.m_str.m_len * sizeof ( tWCHAR ), cCP_UNICODE, pattern_flags, replace_with.m_str.m_data, replace_with.m_str.m_len * sizeof ( tWCHAR ), cCP_UNICODE );
}

// ---
inline tERROR cStringObj::replace_one( const tVOID* pattern, tDWORD pattern_flags, const tVOID* replace_with, tCODEPAGE cp ) {
  if ( !PR_CPP_CF ( ReplaceBuff, 0x0d5e9cc5l ) )
	  return errUNEXPECTED;  	
  return ReplaceBuff ( &m_str, pattern, 0, cp, pattern_flags, replace_with, 0, cp );
}

// ---
inline tERROR cStringObj::replace_one( const cString* pattern, tDWORD pattern_flags, const cString* replace_with ) {
  if ( !PR_CPP_CF ( ReplaceStr, 0x22b9a7b1l ) )
	  return errUNEXPECTED;  	
  return ReplaceStr ( &m_str, const_cast <cString*> ( pattern ), 0, whole, pattern_flags, const_cast <cString*> ( replace_with ), 0, whole );
}

// ---
inline tERROR cStringObj::replace_one( const tCHAR* pattern, tDWORD pattern_flags, const tCHAR* replace_with ) {
  return replace_one ( pattern, pattern_flags, replace_with, cCP_ANSI );
}

// ---
inline tERROR cStringObj::replace_one( const tWCHAR* pattern, tDWORD pattern_flags, const tWCHAR* replace_with ) {
  return replace_one ( pattern, pattern_flags, replace_with, cCP_UNICODE );
}

// ---
inline tERROR cStringObj::replace_all( const cStringObj& pattern, tDWORD pattern_flags, const cStringObj& replace_with ) {
  if ( !PR_CPP_CF ( ReplaceBuffInPos, 0x0a02700bl ) )
	  return errUNEXPECTED;  	
  bool isFound = false;
  tDWORD l_pos = 0;
  tDWORD l_end = whole;
  for (;;) {
    l_pos = find ( pattern, l_pos, l_end, pattern_flags );    
    if ( l_pos == npos )
      return ( isFound ) ? errOK : errNOT_FOUND;
    isFound = true;
    ReplaceBuffInPos ( &m_str, l_pos, pattern.m_str.m_len, replace_with.m_str.m_data, replace_with.m_str.m_len * sizeof ( tWCHAR ), cCP_UNICODE );
    if ( pattern_flags & fSTRING_FIND_BACKWARD ) {
      l_end = l_pos;
      l_pos = 0;
    }
    else
      l_pos += replace_with.m_str.m_len;
  }
}

// ---
inline tERROR cStringObj::replace_all( const tVOID* pattern, tDWORD pattern_flags, const tVOID* replace_with, tCODEPAGE cp ) {
  if ( !PR_CPP_CF ( CopyTo, 0x10a92e27l ) )
    return errUNEXPECTED;
  tDWORD l_size = 0;
  tERROR l_err = CopyTo ( 0, 0, cCP_UNICODE, const_cast <tVOID*> ( pattern ), 0, cp, cSTRING_CONTENT_ONLY, &l_size );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  l_size /= sizeof ( tWCHAR );
  tDWORD l_replace_size = 0;
  l_err = CopyTo ( 0, 0, cCP_UNICODE, const_cast <tVOID*> ( replace_with ), 0, cp, cSTRING_CONTENT_ONLY, &l_replace_size );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  l_replace_size /= sizeof ( tWCHAR );
  if ( !PR_CPP_CF ( ReplaceBuffInPos, 0x0a02700bl ) )
	  return errUNEXPECTED;  	
  bool isFound = false;
  tDWORD l_pos = 0;
  tDWORD l_end = whole;
  for (;;) {
    l_pos = find ( pattern, cp, l_pos, l_end, pattern_flags );    
    if ( l_pos == npos )
      return ( isFound ) ? errOK : errNOT_FOUND;
    isFound = true;
    ReplaceBuffInPos ( &m_str, l_pos, l_size, replace_with, 0, cp );
    if ( pattern_flags & fSTRING_FIND_BACKWARD ) {
      l_end = l_pos;
      l_pos = 0;
    }
    else
      l_pos += l_replace_size;
  }
  return errOK;
}

// ---
inline tERROR cStringObj::replace_all( const cString* pattern, tDWORD pattern_flags, const cString* replace_with ) {
  tDWORD l_size = 0;
  tERROR l_err = CALL_String_LengthEx ( const_cast <cString*>(pattern), &l_size, STR_RANGE ( 0, whole ), cCP_UNICODE, cSTRING_CONTENT_ONLY );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  l_size /= sizeof ( tWCHAR );
  tDWORD l_replace_size = 0;
  l_err = CALL_String_LengthEx ( const_cast <cString*>(replace_with), &l_replace_size, STR_RANGE ( 0, whole ), cCP_UNICODE, cSTRING_CONTENT_ONLY );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  l_replace_size /= sizeof ( tWCHAR );
  if ( !PR_CPP_CF ( ReplaceStrInPos, 0x9fcbb7ecl ) )
	  return errUNEXPECTED;  	
  bool isFound = false;
  tDWORD l_pos = 0;
  tDWORD l_end = whole;
  for (;;) {
    l_pos = find ( pattern, l_pos, l_end, pattern_flags );    
    if ( l_pos == npos )
      return ( isFound ) ? errOK : errNOT_FOUND;
    isFound = true;
    ReplaceStrInPos ( &m_str, l_pos, l_size, const_cast <cString*>( replace_with ), 0, whole );
    if ( pattern_flags & fSTRING_FIND_BACKWARD ) {
      l_end = l_pos;
      l_pos = 0;
    }
    else
      l_pos += l_replace_size;
  }
  return errOK;
}

// ---
inline tERROR cStringObj::replace_all( const tCHAR* pattern, tDWORD pattern_flags, const tCHAR* replace_with ) {
  return replace_all ( pattern, pattern_flags, replace_with, cCP_ANSI );
}

// ---
inline tERROR cStringObj::replace_all( const tWCHAR* pattern, tDWORD pattern_flags, const tWCHAR* replace_with ) {
  return replace_all ( pattern, pattern_flags, replace_with, cCP_UNICODE );
}

// ---
inline tDWORD cStringObj::find_first_not_of( const tCHAR* symb_set, tDWORD pos, tDWORD size ) const {
  tDWORD l_pos = npos;
  if ( !PR_CPP_CF ( FindOneOfBuff, 0x7ca05251l ) )
	  return l_pos;
  FindOneOfBuff ( &m_str, pos, size, symb_set, 0, cCP_ANSI, fSTRING_FIND_FORWARD, cFALSE, &l_pos );
  return l_pos;
}

// ---
inline tDWORD cStringObj::find_last_not_of( const tCHAR* symb_set, tDWORD pos, tDWORD size ) const {
  tDWORD l_pos = npos;
  if ( !PR_CPP_CF ( FindOneOfBuff, 0x7ca05251l ) )
	  return l_pos;
  FindOneOfBuff ( &m_str, pos, size, symb_set, 0, cCP_ANSI, fSTRING_FIND_BACKWARD, cFALSE, &l_pos );
  return l_pos;
}

// ---
inline tDWORD cStringObj::find_first_of( const tCHAR* symb_set, tDWORD pos, tDWORD size ) const {
  tDWORD l_pos = npos;
  if ( !PR_CPP_CF ( FindOneOfBuff, 0x7ca05251l ) )
	  return l_pos;
  FindOneOfBuff ( &m_str, pos, size, symb_set, 0, cCP_ANSI, fSTRING_FIND_FORWARD, cTRUE, &l_pos );
  return l_pos;
}

// ---
inline tDWORD cStringObj::find_last_of( const tCHAR* symb_set, tDWORD pos, tDWORD size ) const {
  tDWORD l_pos = npos;
  if ( !PR_CPP_CF ( FindOneOfBuff, 0x7ca05251l ) )
	  return l_pos;
  FindOneOfBuff ( &m_str, pos, size, symb_set, 0, cCP_ANSI, fSTRING_FIND_BACKWARD, cTRUE, &l_pos );
  return l_pos;
}

// ---
inline tDWORD cStringObj::find_first_not_of( const tWCHAR* symb_set, tDWORD pos, tDWORD size ) const {
  tDWORD l_pos = npos;
  if ( !PR_CPP_CF ( FindOneOfBuff, 0x7ca05251l ) )
	  return l_pos;
  FindOneOfBuff ( &m_str, pos, size, symb_set, 0, cCP_UNICODE, fSTRING_FIND_FORWARD, cFALSE, &l_pos );
  return l_pos;
}

// ---
inline tDWORD cStringObj::find_last_not_of( const tWCHAR* symb_set, tDWORD pos, tDWORD size ) const {
  tDWORD l_pos = npos;
  if ( !PR_CPP_CF ( FindOneOfBuff, 0x7ca05251l ) )
	  return l_pos;
  FindOneOfBuff ( &m_str, pos, size, symb_set, 0, cCP_UNICODE, fSTRING_FIND_BACKWARD, cFALSE, &l_pos );
  return l_pos;
}

// ---
inline tDWORD cStringObj::find_first_of( const tWCHAR* symb_set, tDWORD pos, tDWORD size ) const {
  tDWORD l_pos = npos;
  if ( !PR_CPP_CF ( FindOneOfBuff, 0x7ca05251l ) )
	  return l_pos;
  FindOneOfBuff ( &m_str, pos, size, symb_set, 0, cCP_UNICODE, fSTRING_FIND_FORWARD, cTRUE, &l_pos );
  return l_pos;
}

// ---
inline tDWORD cStringObj::find_last_of( const tWCHAR* symb_set, tDWORD pos, tDWORD size ) const {
  tDWORD l_pos = npos;
  if ( !PR_CPP_CF ( FindOneOfBuff, 0x7ca05251l ) )
	  return l_pos;
  FindOneOfBuff ( &m_str, pos, size, symb_set, 0, cCP_UNICODE, fSTRING_FIND_BACKWARD, cTRUE, &l_pos );
  return l_pos;
}

// ---
inline tDWORD cStringObj::find( const cStringObj& str, tDWORD pos, tDWORD size, tDWORD flags ) const {
  tDWORD l_res = npos;
  if ( !PR_CPP_CF ( FindBuff, 0x3ee390d3l ) )
	  return l_res;  	
  FindBuff ( &m_str, pos, size, str.m_str.m_data, str.m_str.m_len * sizeof ( tWCHAR ), cCP_UNICODE, flags, &l_res );
  return l_res;
}

// ---
inline tDWORD cStringObj::find( const cString* str, tDWORD pos, tDWORD size, tDWORD flags ) const {
  tDWORD l_res = npos;
  if ( !PR_CPP_CF ( FindStr, 0x9ca854dcl ) )
	  return l_res;  	
  FindStr ( &m_str, pos, size, const_cast<cString*> ( str ), 0, whole, flags, &l_res );
  return l_res;
}

// ---
inline tDWORD cStringObj::find( const tCHAR* str, tDWORD pos, tDWORD size, tDWORD flags ) const {
  return find ( str, cCP_ANSI, pos, size, flags );}

// ---
inline tDWORD cStringObj::find( const tWCHAR* str, tDWORD pos, tDWORD size, tDWORD flags ) const {
  return find ( str, cCP_UNICODE, pos, size, flags );
}

// ---
inline tDWORD cStringObj::find( const tVOID* str, tCODEPAGE cp, tDWORD pos, tDWORD size, tDWORD flags ) const {
  tDWORD l_res = npos;
  if ( !PR_CPP_CF ( FindBuff, 0x3ee390d3l ) )
	  return l_res;  	
  FindBuff ( &m_str, pos, size, str, 0, cp, flags, &l_res );
  return l_res;
}

// ---
inline tERROR cStringObj::toupper() {
  if ( !PR_CPP_CF ( ToUpperCase, 0xdae8a8d0l ) )
	  return errUNEXPECTED;  	
  return ToUpperCase ( m_str.m_data, m_str.m_len * sizeof ( tWCHAR ), cCP_UNICODE );
}

// ---
inline tERROR cStringObj::tolower() {
  if ( !PR_CPP_CF ( ToLowerCase, 0xba2d0c6fl ) )
    return errUNEXPECTED;
  return ToLowerCase ( m_str.m_data, m_str.m_len * sizeof ( tWCHAR ), cCP_UNICODE );
}

// ---
inline cStringObj cStringObj::substr( tDWORD pos, tDWORD size ) const {
  cStringObj str;
  str.assign( *this, pos, size);
  return str;
}

// ---
inline tERROR cStringObj::check_last_slash(bool enable) {
#if defined (_WIN32)
#define PATH_DELIMETER_STRING L"\\/"
#else
#define PATH_DELIMETER_STRING L"/"
#endif
  if ( !m_str.m_len )
    return errOK;

  tDWORD l_pos = find_last_of ( (tCHAR*)PATH_DELIMETER_STRING );
  bool l_islastslash = ( l_pos != npos ) && ( l_pos == m_str.m_len - 1 );
  if ( enable )
  {
    if ( !l_islastslash )
    {
      tWCHAR cSlash = l_pos != npos ? m_str.m_data[ l_pos ] : PATH_DELIMETER_WIDE[ 0 ];
      append ( &cSlash, cCP_UNICODE, 1 );
    }
  }
  else
  {
    if ( l_islastslash )
      erase ( m_str.m_len - 1, 1 );
  }
  return errOK;
}

// ---
inline tERROR cStringObj::set_path( const cStringObj& path ) {
  tERROR l_err = assign ( path );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  return check_last_slash ();
}

// ---
inline tERROR cStringObj::set_path( const tVOID* path, tCODEPAGE cp, tDWORD len ) {
  tERROR l_err = assign ( path, cp, len );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  return check_last_slash ();}

// ---
inline tERROR cStringObj::set_path( cString* path ) {
  tERROR l_err = assign ( path );
  if ( PR_FAIL ( l_err ) )
    return l_err;
  return check_last_slash ();
}

// ---
inline tERROR cStringObj::add_path_sect( const cStringObj& sect ) {
  check_last_slash();
  return append ( sect );
}

// ---
inline tERROR cStringObj::add_path_sect( const tVOID* sect, tCODEPAGE cp, tDWORD len ) {
  check_last_slash();
  return append ( sect, cp, len );}

// ---
inline tERROR cStringObj::add_path_sect( const cString* sect ) {
  check_last_slash();
  return append ( sect );
}

// ---
inline tERROR cStringObj::format( tCODEPAGE format_cp, const tVOID* format_str, ... ) {
  cAutoString l_str ( *this );
  va_list argptr; 
  va_start( argptr, format_str ); 
  tERROR error = CALL_String_VFormat( l_str, format_cp, format_str, VA_LIST_ADDR(argptr) ); 
  va_end( argptr ); 
  return error; 
}

// ---
inline tERROR cStringObj::format_v( tCODEPAGE format_cp, const tVOID* format_str, tPTR args ) {
  cAutoString l_str ( *this );
  return CALL_String_VFormat( l_str, format_cp, format_str, args );
}




// -----------------------------------------------------
// cCharBuff inlines
// ---
template <int BuffSize>
inline tERROR cCharBuff<BuffSize>::init( const cString* str, tCODEPAGE cp, tDWORD pos ) {
	if( !str )
		return errOK;
	m_used = 0;
	tSTR_RANGE range = STR_RANGE(pos, cSTRING_WHOLE_LENGTH);
	tERROR error = const_cast<cString*>(str)->ExportToBuff( &m_used, range, (tPTR)ptr(), size(), cp, cSTRING_Z );
	if( error == errBUFFER_TOO_SMALL ) {
		tCHAR* p = get( m_used, false );
		error = !p ? errNOT_ENOUGH_MEMORY : const_cast<cString*>(str)->ExportToBuff( &m_used, range, p, size(), cp, cSTRING_Z );
	}
	if( PR_FAIL(error) )
		*((tCHAR*)(*this)) = 0;
	return error;
}

template <int BuffSize>
inline tERROR cCharBuff<BuffSize>::init( const cStringObj& str, tCODEPAGE cp, tDWORD pos ) {
  m_used = str.memsize ( cp ) - pos * sizeof ( tCHAR );
  tCHAR* p = get( m_used, false );
  if ( !p )
    return errNOT_ENOUGH_MEMORY;
  return str.copy ( p, m_used, cp, pos, cStringObj::whole );
}

template <int BuffSize>
inline tERROR cCharBuff<BuffSize>::init( const tCHAR* str )
{
  if( !str )
    return errOK;
  m_used = ( strSize ( str ) + 1 ) * sizeof ( tCHAR );
  tCHAR* p = get( m_used, false );
  tERROR error = errOK;
  if ( p )
    memcpy ( p, str, m_used );
  else
    error = errNOT_ENOUGH_MEMORY;
  if( PR_FAIL(error) )
    *((tCHAR*)(*this)) = 0;
  return error;
}

template <int BuffSize>
inline tERROR cCharBuff<BuffSize>::init( const tWCHAR* str )
{
  if( !str )
    return errOK;
  if( !str )
    return errOK;
  m_used = ( strSize ( str ) + 1 ) * sizeof ( tWCHAR );
  tCHAR* p = get( m_used, false );
  tERROR error = errOK;
  if ( p )
    memcpy ( p, str, m_used );
  else
    error = errNOT_ENOUGH_MEMORY;
  if( PR_FAIL(error) )
    *((tCHAR*)(*this)) = 0;
  return error;
}


// -----------------------------------------------------
// cWCharBuff inlines
// ---
template <int BuffSize>
inline tERROR cWCharBuff<BuffSize>::init( const cString* str, tDWORD pos ) {
	if( !str )
		return errOK;
	m_used = 0;
	tSTR_RANGE range = STR_RANGE(pos, cSTRING_WHOLE_LENGTH);
	tERROR error = const_cast<cString*>(str)->ExportToBuff( &m_used, range, (tPTR)ptr(), size(), cCP_UNICODE, cSTRING_Z );
	if( error == errBUFFER_TOO_SMALL ) {
		tWCHAR* p = get( m_used, false );
		error = !p ? errNOT_ENOUGH_MEMORY : const_cast<cString*>(str)->ExportToBuff( &m_used, range, p, size(), cCP_UNICODE, cSTRING_Z );
	}
	if( PR_FAIL(error) )
		*((tWCHAR*)(*this)) = 0;
	return error;
}

template <int BuffSize>
inline tERROR cWCharBuff<BuffSize>::init( const cStringObj& str, tDWORD pos ) {
  m_used = str.memsize ( cCP_UNICODE ) - pos * sizeof ( tWCHAR );
  tWCHAR* p = get( m_used, false );
  if ( !p )
    return errNOT_ENOUGH_MEMORY;
  return str.copy ( p, m_used, cCP_UNICODE, pos, cStringObj::whole );
}

template <int BuffSize>
inline tERROR cWCharBuff<BuffSize>::init( const tWCHAR* str )
{
	if( !str )
		return errOK;
  	if( !str )
		return errOK;
	m_used = strSize ( str ) + 1;
    tWCHAR* p = get( m_used, false );
    tERROR error = errOK;
    if ( p )
        memcpy ( p, str, m_used * sizeof ( tWCHAR ) );
    else
        error = errNOT_ENOUGH_MEMORY;
	if( PR_FAIL(error) )
		*((tWCHAR*)(*this)) = 0;
	return error;
}


// ---
inline void * __cdecl my_memmove ( void * dst, const void * src, size_t count ) {
	void* ret = dst;
	
	if ( (dst <= src) || ((char *)dst >= ((char *)src + count)) ) {
		while (count--) {
			*(char *)dst = *(char *)src;
			dst = (char *)dst + 1;
			src = (char *)src + 1;
		}
	}
	else {
		dst = (char *)dst + count - 1;
		src = (char *)src + count - 1;
		while (count--) {
			*(char *)dst = *(char *)src;
			dst = (char *)dst - 1;
			src = (char *)src - 1;
		}
	}
	return ret;
}

#endif // __cplusplus
#endif // __pr_cpp_h
