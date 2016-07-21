#include "stdafx.h"
#include <serialize/kldtser.h>
#include <stdio.h>

#define OK(c) (0!=(c))

int operator == (const tGUID& guidOne, const tGUID& guidOther)
{
    return !memcmp(&guidOne,&guidOther,sizeof(tGUID));
}

// ---

CTreeInfo::CTreeInfo( HDATA tree, bool own ) : m_own(false), m_tree(0), m_first(0), m_curr(0) {
  Init( tree, own );
}


// ---
CTreeInfo::CTreeInfo( tDWORD len, const BYTE* stream ) :  m_own(false), m_tree(0), m_first(0), m_curr(0) {
  Init( len, stream );
}



// ---
CTreeInfo::~CTreeInfo() {
  Init(0,false);
}


// ---
bool CTreeInfo::Init( HDATA tree, bool own ) {
  HDATA old = m_own ? m_tree : 0;

  if ( tree ) {
    m_own = own;
    m_tree = m_first = m_curr = tree;
  }
  else {
    m_own = false;
    m_tree = m_first = m_curr = 0;
  }

  if ( old ) 
    ::DATA_Remove( old, 0 );

  return ReInit();
}


// ---
bool CTreeInfo::Init( tDWORD len, const BYTE* stream ) {
  if ( len && stream ) {
    if ( (len > sizeof(AVP_SW_MAGIC)) && (*(tDWORD*)stream == AVP_SW_MAGIC) ) {
      HDATA tree = 0;
			::KLDT_DeserializeFromMemoryUsingSWM( (char*)stream, len, &tree );
      return Init( tree, true );
    }

		else if ( (len > sizeof(WORD)) && (*(WORD*)stream == 0xadad) )
      return Init( ::DATA_Deserialize(0,(void*)stream,len,0), true );

	}
  return false;
}



// ---
HDATA CTreeInfo::Dad() const {
	return ::DATA_Get_Dad( m_tree, 0 );
}



// ---
HDATA CTreeInfo::Next() const {
  if ( m_curr )
    return ::DATA_Get_Next( m_curr, 0 );
  else
    return 0;
}



// ---
bool CTreeInfo::GoFirst() {
  m_curr = m_first;
  return ReInit();
}



// ---
bool CTreeInfo::GoNext() {
  if ( !m_curr )
    return false;
  else {
    m_curr = ::DATA_Get_Next( m_curr, 0 );
    return ReInit();
  }
}



// ---
HDATA CTreeInfo::Add( tDWORD id, tDWORD val, tDWORD size ) {
  return m_curr ? ::DATA_Add(m_curr,0,id,val,size) : 0;
}



// ---
HDATA CTreeInfo::Copy( HDATA copy, tDWORD flags ) {
  return m_curr ? ::DATA_Copy(m_curr,0,copy,flags) : 0;
}



// ---
bool CTreeInfo::Remove() {
  if ( !m_curr )
    return false;
  else {
    HDATA next = ::DATA_Get_Next( m_curr, 0 );
    ::DATA_Remove( m_curr, 0 );
    if ( m_curr == m_first )
      m_first = next;
    m_curr = next;
    return ReInit();
  }
}



// ---
bool CTreeInfo::GoPrev() {
  if ( m_curr == m_first ) {
    HDATA next = m_curr;
    if ( GoFirst() ) {
      HDATA n = ::DATA_Get_Next( m_curr, 0 );
      while( n ) {
        if ( n == next ) 
          return ReInit();
        n = ::DATA_Get_Next( m_curr=n, 0 );
      }
      if ( !next )
        return ReInit();
    }
  }
  m_curr = 0;
  return false;
}



// ---
tDWORD CTreeInfo::Attr( tDWORD id, void* pVal, tDWORD size, AVP_dword* addr ) const {
  HPROP p;

  if ( pVal && size )
    ::memset( pVal, 0, size );

  HDATA d = m_curr ? (addr ? ::DATA_Find(m_curr,addr) : m_curr) : 0;
  if ( d && OK(p=::DATA_Find_Prop(d,0,id)) ) 
    return ::PROP_Get_Val( p, pVal, size );
  else
    return 0;
}



// ---
bool CTreeInfo::SetAttr( tDWORD aid, void* pVal, tDWORD size, AVP_dword* addr ) {
  HDATA d = m_curr ? (addr ? ::DATA_Find(m_curr,addr) : m_curr) : 0;
  if ( !d )
    return false;
  return !!DATA_Set_Val( d, 0, aid, (AVP_dword)pVal, size );
}



// ---
bool CTreeInfo::BoolAttr( tDWORD id, AVP_dword* addr ) const {
  BOOL v;
  HPROP p;
  HDATA d = m_curr ? (addr ? ::DATA_Find(m_curr,addr) : m_curr) : 0;
  if ( d && OK(p=::DATA_Find_Prop(d,0,id)) && OK(::PROP_Get_Val(p,&v,sizeof(v))) ) 
    return v != 0;
  else
    return 0;
}



// ---
bool CTreeInfo::SetBoolAttr( tDWORD aid, bool val, AVP_dword* addr ) {
  HDATA d = m_curr ? (addr ? ::DATA_Find(m_curr,addr) : m_curr) : 0;
  if ( !d )
    return false;
  return !!DATA_Set_Val( d, 0, aid, val, sizeof(val) );
}



// ---
tDWORD CTreeInfo::DWAttr( tDWORD id, AVP_dword* addr ) const {
  tDWORD v;
  HPROP p;
  HDATA d = m_curr ? (addr ? ::DATA_Find(m_curr,addr) : m_curr) : 0;
  if ( d && OK(p=::DATA_Find_Prop(d,0,id)) && OK(::PROP_Get_Val(p,&v,sizeof(v))) ) 
    return v;
  else
    return 0;
}



// ---
bool CTreeInfo::SetDWAttr( tDWORD aid, tDWORD val, AVP_dword* addr ) {
  HDATA d = m_curr ? (addr ? ::DATA_Find(m_curr,addr) : m_curr) : 0;
  if ( !d )
    return false;
  HPROP p = ::DATA_Find_Prop( d, 0, aid );
  if ( p )
    return !!PROP_Set_Val( p, val, sizeof(val) );
  else
    return !!::DATA_Add_Prop( d, 0, aid, val, sizeof(val) );
}



// ---
tDWORD CTreeInfo::DWAttrAsStr( tDWORD aid, char* val, tDWORD size, bool hex, AVP_dword* addr ) const {
  tCHAR buff[20];
  tUINT len;
  
  if ( hex )
    len = ::sprintf( buff, "0x08%x", DWAttr(aid,addr) ) + 1;
  else
    len = ::sprintf( buff, "%u", DWAttr(aid,addr) ) + 1;
  
  if ( val && size ) {
    if ( size >= len )
      ::memcpy( val, buff, len );
    else
      len = 0;
  }
  return len;
}




// ---
tDWORD CTreeInfo::StrAttr( tDWORD id, char* val, tDWORD size, AVP_dword* addr ) const {
  HPROP p;
  HDATA d = m_curr ? (addr ? ::DATA_Find(m_curr,addr) : m_curr) : 0;
  if ( d && OK(p=::DATA_Find_Prop(d,0,id)) ) {
    if ( val && size ) 
      return ::PROP_Get_Val( p, val, size );
    else
      return ::PROP_Get_Val( p, 0, 0 );
  }
  else
    return 0;
}



// ---
bool CTreeInfo::SetStrAttr( tDWORD aid, const char* val, UINT len, AVP_dword* addr ) {
  HDATA d = m_curr ? (addr ? ::DATA_Find(m_curr,addr) : m_curr) : 0;
  if ( !d )
    return false;
  if ( !len && val )
    len = (UINT)(strlen(val) + 1);
  HPROP p = ::DATA_Find_Prop( d, 0, aid );
  if ( p )
    return !!::PROP_Set_Val( p, (tDWORD)val, len );
  else
    return !!::DATA_Add_Prop( d, 0, aid, (tDWORD)val, len );
}



// ---
bool CTreeInfo::DelAttr( tDWORD aid, AVP_dword* addr ) {
  HDATA d = m_curr ? (addr ? ::DATA_Find(m_curr,addr) : m_curr) : 0;
  if ( !d )
    return false;
  return !!DATA_Remove_Prop_ID( d, 0, aid );
}



// ---
HPROP CTreeInfo::MainProp() const {
  return m_curr ? ::DATA_Find_Prop(m_curr,0,0) : 0;
}



// ---
HPROP CTreeInfo::FindProp( tDWORD pid ) const {
  return m_curr ? ::DATA_Find_Prop(m_curr,NULL,pid) : 0;
}


