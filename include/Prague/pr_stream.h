#ifndef __pr_stream_h
#define __pr_stream_h

#if !defined ( __cplusplus )
	#error C++ compiler required.
#endif

#include <m_utils.h>

#define _INC_STDLIB
#include <utf8.h>

#include <Prague/prague.h>

#define LAST_STREAM_MAGIC        ('mrts')
#define LAST_STREAM_VERSION      ((tUINT)2)

#define STREAM_OPS_PACK_DWORD    0x00000001
#define STREAM_OPS_PACK_WSTRING  0x00000002
#define STREAM_OPS_DEFAULT       (STREAM_OPS_PACK_DWORD | STREAM_OPS_PACK_WSTRING)


#define STREAM_STACK_BUFF_SIZE 0x80


class cMemOperators {
public:
  inline void *__cdecl operator new(size_t, void *_)  { return _; }    
  inline void __cdecl operator delete(void *, void *) {return; }
};



// ---
class cOutStream {
protected:
	tDWORD  m_op_flags;

public:
	virtual tERROR push_bytes( const tVOID* val, tDWORD size ) = 0;
	virtual tDWORD size()   const = 0;
	virtual tBOOL  filled() const = 0;
	virtual tBOOL  is_for_count_only() const = 0;
	virtual tERROR overlap( tDWORD offs, const tVOID* val, tDWORD size ) = 0;

	tDWORD offs() const { return size(); }

	tERROR push( bool   val ) { return push_bytes( &val, sizeof(bool)  ); }
	tERROR push( tBYTE  val ) { return push_bytes( &val, sizeof(tBYTE) ); }
	tERROR push( tWORD  val ) { return push_bytes( &val, sizeof(tWORD) ); }
	tERROR push( tULONG val );
	tERROR push( tDWORD val );
	tERROR push( tQWORD val );

	tERROR push_str( const tCHAR*  astr, tDWORD len=0 );
	tERROR push_str( const tWCHAR* wstr, tDWORD len=0 );

protected:
	cOutStream( tDWORD ops ) : m_op_flags(ops) {}
};



// ---
template<class IntegralType> 
	inline tERROR operator << ( cOutStream& out, IntegralType val ) { return out.push(val); }



// ---
class cOutStreamPreallocatedBuff : public cOutStream {
	
protected:
	tBYTE* m_buff;
	tUINT  m_size;
	tUINT  m_cp;
	
public:
	cOutStreamPreallocatedBuff( tBYTE* preallocated_buff, tDWORD preallocated_size, tDWORD op_flags=STREAM_OPS_DEFAULT );
	
public:
	virtual tERROR push_bytes( const tVOID* val, tDWORD size );
	virtual tDWORD size()   const;
	virtual tBOOL  filled() const;
	virtual tBOOL  is_for_count_only() const;
	virtual tERROR overlap( tDWORD offs, const tVOID* val, tDWORD size );
};



// ---
class cOutStreamAlloc : public cOutStream, public cMemOperators {

protected:
	tBYTE*  m_buff;
	tUINT   m_size;
	tUINT   m_cp;
	tUINT   m_cluster_size;
	hOBJECT m_mem;

public:
	cOutStreamAlloc( hOBJECT p_mem_allocator, tDWORD p_cluster_size, tDWORD op_flags=STREAM_OPS_DEFAULT );
	~cOutStreamAlloc();

public:
	virtual tERROR push_bytes( const tVOID* val, tDWORD size );
	virtual tDWORD size()   const;
	virtual tBOOL  filled() const;
	virtual tBOOL  is_for_count_only() const;
	virtual tERROR overlap( tDWORD offs, const tVOID* val, tDWORD size );

	const tBYTE* ptr() const;
	tBYTE* relinquish( tDWORD& size );

protected:
	tERROR check( tDWORD offs, tDWORD size );
};




// ---
class cInStream : public cMemOperators {

protected:
	const tBYTE* m_buff;
	tUINT        m_size;
	tUINT        m_cp;
	tUINT        m_ver;
	tDWORD       m_op_flags;

public:
	cInStream( const tBYTE* p_buff, tUINT p_size );
	cInStream( const cInStream& o, tDWORD end_ofset=-1 ) 
		: m_buff(const_cast<cInStream&>(o).ptr()), m_size(o.avail(end_ofset)), m_cp(0), m_ver(o.ver()), m_op_flags(o.op_flags()) {}
	cInStream( const tBYTE* p_buff, tUINT p_size, tUINT op_flags, tUINT ver ) 
		: m_buff(p_buff), m_size(p_size), m_cp(0), m_ver(ver), m_op_flags(op_flags) {}

	tDWORD       accepted()                   const { return m_cp; }
	tDWORD       ver()                        const { return m_ver; }
	tDWORD       op_flags()                   const { return m_op_flags; }
	tDWORD       cp()                         const { return accepted();  }
	tDWORD       size()                       const { return m_size;      }
	const tBYTE* ptr()                              { return m_buff+m_cp; }
	tDWORD       avail( tDWORD end_point=-1 ) const { tUINT lp = (m_size<end_point) ? m_size : end_point; return lp > m_cp ? lp-m_cp : 0; }
	tERROR       advance( tDWORD diff, tDWORD end_point=-1 );

	tERROR pop_bytes( tPTR val, tDWORD size, tDWORD end_offset=-1 );
	tERROR pop( bool&   val, tDWORD end_offset=-1 );
	tERROR pop( tBYTE&  val, tDWORD end_offset=-1 );
	tERROR pop( tWORD&  val, tDWORD end_offset=-1 );
	tERROR pop( tULONG& val, tDWORD end_offset=-1 );
	tERROR pop( tDWORD& val, tDWORD end_offset=-1 );
	tERROR pop( tQWORD& val, tDWORD end_offset=-1 );

	tERROR pop_str( tCHAR* buff, tDWORD buff_len, tDWORD end_offset=-1, tDWORD* len=0 );
	tERROR pop_str( tWCHAR* buff, tDWORD buff_len, tDWORD end_offset=-1, tDWORD* len=0 );
};



// ---
inline tERROR cOutStream::push( tULONG val ) {

	if ( !(m_op_flags & STREAM_OPS_PACK_DWORD) )
		return push_bytes( &val, sizeof(tULONG) );

	tBYTE b;
	tUINT p = 0;
	tBYTE stream[sizeof(tULONG)+1];

	do {
		b = (tBYTE)(val & 0x7F);
		val >>= 7;
		if ( p )
			b |= 0x80;
		stream[sizeof(stream) - ++p] = b;
	} while( val );  
	return push_bytes( stream+sizeof(stream)-p, p );

}



// ---
inline tERROR cOutStream::push( tDWORD val ) {
	if ( !(m_op_flags & STREAM_OPS_PACK_DWORD) )
		return push_bytes( &val, sizeof(tDWORD) );

	tBYTE b;
	tUINT p = 0;
	tBYTE stream[sizeof(tDWORD)+1];

	do {
		b = (tBYTE)(val & 0x7F);
		val >>= 7;
		if ( p )
			b |= 0x80;
		stream[sizeof(stream) - ++p] = b;
	} while( val );  
	return push_bytes( stream+sizeof(stream)-p, p );

}




// ---
inline tERROR cOutStream::push( tQWORD val ) {
	if ( !(m_op_flags & STREAM_OPS_PACK_DWORD) )
		return push_bytes( &val, sizeof(tQWORD) );

	tBYTE b;
	tUINT p = 0;
	tBYTE stream[sizeof(tQWORD)+2];

	do {
		b = (tBYTE)(val & 0x7F);
		val >>= 7;
		if ( p )
			b |= 0x80;
		stream[sizeof(stream) - ++p] = b;
	} while( val );  
	return push_bytes( stream+sizeof(stream)-p, p );

}




// ---
inline tERROR cOutStream::push_str( const tCHAR* str, tDWORD len ) {
	if ( !str )
		return push( (tDWORD)0 );
  if ( len == 0 )
		len = (tDWORD)strlen( str );
  ++len;
  cERROR err = push( len );
	if ( PR_SUCC(err) )
		err = push_bytes( str, len );
	return err;
}



// ---
inline tERROR cOutStream::push_str( const tWCHAR* wstr, tDWORD len ) {

	if ( !wstr )
		return push( (tDWORD)0 );
	
  if ( len == 0 ) {
		const tWCHAR* p = wstr;
		for( ; *p; ++p )
			;
		len = (tDWORD)(sizeof(tWCHAR) * (1 + (p - wstr)));
  }

	cERROR err = push( len );
	if ( PR_SUCC(err) ) {
		if ( m_op_flags & STREAM_OPS_PACK_WSTRING ) {
			tCHAR utf8[STREAM_STACK_BUFF_SIZE*UTF8_MAX_GROW_FACTOR+1];
			len /= sizeof(tWCHAR);
			while( len ) {
				tUINT l = len > STREAM_STACK_BUFF_SIZE ? STREAM_STACK_BUFF_SIZE : len;
				tINT  r = utf8_encode_string( wstr, l, utf8 );
				if ( !r )
					return errUNEXPECTED;
				err = push_bytes( utf8, r );
				if ( PR_FAIL(err) )
					return err;
				len -= l;
				wstr += l;
			}
		}
		else if ( PR_SUCC(err) )
			err = push_bytes( wstr, len );
	}

  return err;
}




// ---
inline cOutStreamPreallocatedBuff::cOutStreamPreallocatedBuff( tBYTE* preallocated_buff, tDWORD preallocated_size, tDWORD op_flags ) : cOutStream(op_flags), m_buff(preallocated_buff), m_size(preallocated_size), m_cp(0) {
	tDWORD magic = LAST_STREAM_MAGIC;
	push_bytes( &magic, sizeof(tDWORD) );
	magic = LAST_STREAM_VERSION;
	push_bytes( &magic, sizeof(tDWORD) );
	push_bytes( &op_flags, sizeof(tDWORD) );
}


// ---
inline tERROR cOutStreamPreallocatedBuff::push_bytes( const tVOID* val, tDWORD size ) {
	if ( !val )
		return errPARAMETER_INVALID;
	if ( m_buff && ((m_cp + size) <= m_size) )
		::memcpy( m_buff+m_cp, val, size );
	m_cp += size;
  return errOK;
}


// ---
inline tDWORD cOutStreamPreallocatedBuff::size() const {
	return m_cp;
}


// ---
inline tBOOL cOutStreamPreallocatedBuff::filled() const {
	return m_cp <= m_size;
}


// ---
inline tBOOL cOutStreamPreallocatedBuff::is_for_count_only() const {
	if ( m_buff && (m_cp < m_size) )
		return cTRUE;
	return cFALSE;
}


// ---
inline tERROR cOutStreamPreallocatedBuff::overlap( tDWORD offs, const tVOID* val, tDWORD size ) {
	if ( !val )
		return errPARAMETER_INVALID;
	if ( m_buff && ((offs + size) <= m_size) )
		::memcpy( m_buff+offs, val, size );
  return errOK;
}


// ---
inline cOutStreamAlloc::cOutStreamAlloc( hOBJECT p_mem_allocator, tDWORD p_cluster_size, tDWORD op_flags ) : cOutStream(op_flags), m_buff(0), m_size(0), m_cp(0), m_cluster_size(p_cluster_size), m_mem(p_mem_allocator) {
	tDWORD magic = LAST_STREAM_MAGIC;
	push_bytes( &magic, sizeof(tDWORD) );
	magic = LAST_STREAM_VERSION;
	push_bytes( &magic, sizeof(tDWORD) );
	push_bytes( &op_flags, sizeof(tDWORD) );
}


// ---
inline cOutStreamAlloc::~cOutStreamAlloc() {
  if ( m_buff )
		m_mem->heapFree( m_buff );
}


// ---
inline tERROR cOutStreamAlloc::push_bytes( const tVOID* val, tDWORD size ) {
  cERROR err = check( m_cp, size );
  if ( PR_SUCC(err) ) {
		if ( m_buff )
			::memcpy( m_buff+m_cp, val, size );
		m_cp += size;
  }
  return err;
}


// ---
inline const tBYTE* cOutStreamAlloc::ptr() const {
  return m_buff;
}


// ---
inline tDWORD cOutStreamAlloc::size() const {
	return m_cp;
}


// ---
inline tBOOL cOutStreamAlloc::filled() const {
	return m_cp <= m_size;
}


// ---
inline tBOOL cOutStreamAlloc::is_for_count_only() const {
	return cFALSE;
}


// ---
inline tERROR cOutStreamAlloc::overlap( tDWORD offs, const tVOID* val, tDWORD size ) {
  cERROR err = check( offs, size );
  if ( PR_SUCC(err) ) {
		if ( m_buff )
			::memcpy( m_buff+offs, val, size );
  }
  return err;
}


// ---
inline tBYTE* cOutStreamAlloc::relinquish( tDWORD& size ) { 
  tBYTE* ret = m_buff;
  size = m_cp;
  m_buff = 0;
  m_size = m_cp = 0;
  return ret;
}


// ---
inline tERROR cOutStreamAlloc::check( tDWORD offs, tDWORD size ) {
  cERROR err;
  if ( ((offs + size) > m_size) ) {
	  m_size = (((offs+size) / m_cluster_size) + 1) * m_cluster_size;
    err = m_mem->heapRealloc( (tPTR*)&m_buff, m_buff, m_size );
  }
  return err;
}



// ---
inline cInStream::cInStream( const tBYTE* p_buff, tDWORD p_size ) : m_buff(p_buff), m_size(p_size), m_cp(0), m_ver(0), m_op_flags(0) {
	if ( p_buff && (p_size>=2*sizeof(tDWORD)) ) {
		tDWORD magic = LAST_STREAM_MAGIC;
		if ( !::memcmp(p_buff,&magic,sizeof(tDWORD)) ) {
			::memcpy( &m_ver, p_buff+sizeof(tDWORD), sizeof(tDWORD) );
			::memcpy( &m_op_flags, p_buff+2*sizeof(tDWORD), sizeof(tDWORD) );
			m_cp = 3*sizeof(m_ver);
		}
	}
}


// ---
inline tERROR cInStream::advance( tDWORD diff, tDWORD end_offset )	{ 
	if ( m_cp == end_offset )
		return errEOF;
	tUINT end = m_cp + diff;
	if ( end > end_offset )
		return errOUT_OF_SPACE; //errEOF;
	if ( end > m_size )
		return errOUT_OF_SPACE;
	m_cp = end;
	return errOK;
}


// ---
inline tERROR cInStream::pop_bytes( tPTR val, tDWORD size, tDWORD end_offset ) {
	if ( m_cp == end_offset )
		return errEOF;
	tUINT end = m_cp + size;
	if ( end > end_offset )
		return errOUT_OF_SPACE; //errEOF;
	if ( end > m_size )
		return errOUT_OF_SPACE;
	if ( val && size )
		::memcpy( val, m_buff+m_cp, size );
	m_cp = end;
	return errOK;
}


// ---
inline tERROR cInStream::pop( bool& val, tDWORD end_offset ) {
	if ( m_cp == end_offset )
		return errEOF;
	tUINT end = m_cp + sizeof(bool);
	if ( end > end_offset )
		return errOUT_OF_SPACE; //errEOF;
	if ( end > m_size )
		return errOUT_OF_SPACE;
	val = *(bool*)(m_buff+m_cp);
	m_cp = end;
	return errOK;
}



// ---
inline tERROR cInStream::pop( tBYTE& val, tDWORD end_offset ) {
	if ( m_cp == end_offset )
		return errEOF;
	tUINT end = m_cp + sizeof(tBYTE);
	if ( end > end_offset )
		return errOUT_OF_SPACE; //errEOF;
	if ( end > m_size )
		return errOUT_OF_SPACE;
	val = *(tBYTE*)(m_buff+m_cp);
	m_cp = end;
	return errOK;
}



// ---
inline tERROR cInStream::pop( tWORD& val, tDWORD end_offset ) {
	if ( m_cp == end_offset )
		return errEOF;
	tUINT end = m_cp + sizeof(tWORD);
	if ( end > end_offset )
		return errOUT_OF_SPACE; //errEOF;
	if ( end > m_size )
		return errOUT_OF_SPACE;
	val = *(tWORD*)(m_buff+m_cp);
	m_cp = end;
	return errOK;
}


#define CBYTES( type )  (((sizeof(type)*8)/7) + !!((sizeof(type)*8)%7))

// ---
inline tERROR cInStream::pop( tULONG& val, tDWORD end_point ) {

	if ( !(m_op_flags & STREAM_OPS_PACK_DWORD) )
		return pop_bytes( &val, sizeof(tULONG), end_point );

	tBYTE  b;
	tERROR e;
	tULONG v = 0;
	tUINT  c = 0;
	do {
		if ( (++c) <= CBYTES(tULONG) ) {
			e = pop( b, end_point );
			if ( e == errEOF )
				e = errOUT_OF_SPACE;
		}
		else
			e = errOUT_OF_OBJECT;
		if ( PR_FAIL(e) )
			break;
		v <<= 7;
		v |= (b & 0x7F);
	} while (b & 0x80); 
	val = v;
	return e;

}


// ---
inline tERROR cInStream::pop( tDWORD& val, tUINT end_point ) {

	if ( !(m_op_flags & STREAM_OPS_PACK_DWORD) )
		return pop_bytes( &val, sizeof(tDWORD), end_point );

	if ( m_cp == end_point )
		return errEOF;

	tBYTE  b;
	tERROR e;
	tDWORD v = 0;
	tUINT  c = 0;
	do {
		if ( (++c) <= CBYTES(tDWORD) ) {
			e = pop( b, end_point );
			if ( e == errEOF )
				e = errOUT_OF_SPACE;
		}
		else
			e = errOUT_OF_OBJECT;
		if ( PR_FAIL(e) )
			break;
		v <<= 7;
		v |= (b & 0x7F);
	} while (b & 0x80); 
	val = v;
	return e;
}



// ---
inline tERROR cInStream::pop( tQWORD& val, tUINT end_point ) {

	if ( !(m_op_flags & STREAM_OPS_PACK_DWORD) )
		return pop_bytes( &val, sizeof(tQWORD), end_point );

	tBYTE  b;
	tERROR e;
	tQWORD v = 0;
	tUINT  c = 0;
	do {
		if ( (++c) <= CBYTES(tQWORD) ) {
			e = pop( b, end_point );
			if ( e == errEOF )
				e = errOUT_OF_SPACE;
		}
		else
			e = errOUT_OF_OBJECT;
		if ( PR_FAIL(e) )
			break;
		v <<= 7;
		v |= (b & 0x7F);
	} while (b & 0x80); 
	val = v;
	return e;
}



// ---
inline tERROR cInStream::pop_str( tCHAR* buff, tDWORD buff_len, tDWORD end_offset, tDWORD* plen ) {
	
	tDWORD llen;
	if ( !plen )
		plen = &llen;
	*plen = 0;

	tUINT pos = m_cp;
	cERROR err = pop( *plen, end_offset );
  if ( PR_FAIL(err) || (err == errEOF) )
		return err;

  if ( *plen > buff_len ) {
		m_cp = pos;
		return errBUFFER_TOO_SMALL;
  }

  if ( *plen ) {
		tUINT end = m_cp + *plen;
		if ( (m_cp > end_offset) || (end > m_size) ) {
			m_cp = pos;
			return errOUT_OF_SPACE;
		}
		if ( end > end_offset ) {
			end = end_offset;
			*plen = end - m_cp;
			err = errOUT_OF_SPACE; //errEOF;
		}
		::memcpy( buff, m_buff+m_cp, *plen );
		m_cp = end;
	}
	return err;
}




// ---
inline tERROR cInStream::pop_str( tWCHAR* buff, tDWORD buff_len, tDWORD end_offset, tDWORD* plen ) {
  tDWORD llen;
  if ( !plen )
		plen = &llen;

	*plen = 0;
	tUINT pos = m_cp;
	cERROR err = pop( *plen, end_offset );
  if ( PR_FAIL(err) || (err == errEOF) )
		return err;

  if ( *plen > buff_len ) {
		m_cp = pos;
		return errBUFFER_TOO_SMALL;
  }

  if ( *plen ) {
		if ( m_op_flags & STREAM_OPS_PACK_WSTRING ) {
			tUINT s_len = (tUINT)(sizeof(tCHAR) + strlen( ((tCHAR*)(m_buff+m_cp)) ));
			tUINT end = m_cp + s_len;
			if ( (m_cp > end_offset) || (end > m_size) ) {
				m_cp = pos;
				return errOUT_OF_SPACE;
			}
			if ( end > end_offset ) {
				end = end_offset;
				s_len = end - m_cp;
				err = errOUT_OF_SPACE; //errEOF;
			}
			s_len = utf8_decode_string( (const tCHAR*)(m_buff+m_cp), s_len, buff );
			if ( s_len )
				m_cp = end;
		}
		else
			err = pop_bytes( buff, *plen, end_offset );
	}
  return err;
}






#endif
