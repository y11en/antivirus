#include <stdarg.h>
#include "prop_in.h"
#include "Prop_wrp.h"

// ---
CAddr::CAddr() 
  : m_count(0),
    m_ext( 0 )  {
  m_addr[0] = 0;
}



// ---
CAddr::CAddr( AVP_dword id0, ... ) 
  : m_count( 1 ),
    m_ext( 0 )  {

  AVP_dword  id;
  va_list    arglist;
  AVP_dword* cur;

  va_start( arglist, id0 );
  do {
    id = va_arg( arglist, AVP_dword );
    m_count++;
  } while( id );
  va_end( arglist );

  if ( m_count < (sizeof(m_addr)/sizeof(AVP_dword)) )
    cur = m_addr;
  else {
    _ASSERTE( allocator && liberator );
    m_ext = (AVP_dword*)allocator( m_count * sizeof(AVP_dword) );
    _ASSERT( cur = m_ext );
    m_addr[0] = 0;
  }

  va_start( arglist, id0 );
  id = id0;
  do {
    *cur++ = id;
    id = va_arg( arglist, AVP_dword );
  } while( id );
  va_end( arglist );
  *cur++ = id;
}


// ---
CAddr::CAddr( const CAddr& other ) 
  : m_count( other.m_count ) {

  AVP_dword* t;
  const AVP_dword* f;

  if ( other.m_ext ) {
    _ASSERTE( allocator && liberator );
    m_ext = (AVP_dword*)allocator( m_count+1 * sizeof(AVP_dword) );
    _ASSERT( m_ext );
    m_addr[0] = 0;
    t = m_ext;
    f = other.m_ext;
  }
  else {
    t = m_addr;
    f = other.m_addr;
  }

  for( AVP_dword i=0; i<m_count+1; i++ )
    t[i] = f[i];
}



// ---
CAddr::CAddr( const CData& node, AVP_dword* addr, HDATA stop_data ) 
  : m_count( 0 ),
    m_ext( 0 )  {

  AVP_Data*  data = (AVP_Data*)(addr ? DATA_Find((HDATA)node,addr) : ((HDATA)node));
  AVP_Data*  data2;
	AVP_bool   stop_found;

  if ( !data ) {
    m_addr[0] = 0;
    return ;
  }
  data2 = data;

	stop_found = 0;
  while( data2 ) {
    m_count++;
    data2 = data2->dad;
		if ( data2 && ((stop_data && data2 == (AVP_Data*)stop_data) || (!stop_data && !data2->dad)) ) {
			stop_found = 1;
			break;
		}
  }

  if ( stop_found && m_count ) {
    AVP_dword* cur;
    AVP_dword i;

    if ( m_count < (sizeof(m_addr)/sizeof(AVP_dword)) ) 
      cur = m_addr;
    else {
      _ASSERTE( allocator && liberator );
      cur = m_ext = (AVP_dword*)allocator( (m_count + 1)*sizeof(AVP_dword) );
      _ASSERT( cur = m_ext );
      m_addr[0] = 0;
    }
    cur += m_count;
		*cur-- = 0;
    for( i=0; i<m_count; i++,cur-- ) {
      *cur = PROP_ID(&data->value.data);
      data = data->dad;
    }
  }
  else 
    m_addr[0] = 0;
}


// ---
CAddr::~CAddr() {
  if ( m_ext ) {
    _ASSERTE( allocator && liberator );
    liberator( m_ext );
  }
}



// ---
AVP_dword* CAddr::Add( AVP_dword id0, ... ) {

  va_list    arglist;
  AVP_dword* seq;
  AVP_dword  id;
  AVP_dword  add_count = 0;

  va_start( arglist, id0 );
  do {
    id = va_arg( arglist, AVP_dword );
    add_count++;
  } while( id );
  va_end( arglist );

  if ( (m_count + add_count) >= sizeof(m_addr)/sizeof(AVP_dword) ) {
    if ( !m_ext ) {
      _ASSERTE( allocator && liberator );
      m_ext = (AVP_dword*)allocator( (m_count + add_count) * sizeof(AVP_dword) );
      _ASSERT( seq = m_ext );
      for( AVP_dword i=0; i<m_count; i++ )
        m_ext[i] = m_addr[i];
      m_addr[0] = 0;
    }
    seq = m_ext + m_count;
  }
  else
    seq = m_addr + m_count;

  va_start( arglist, id0 );
  do {
    id = va_arg( arglist, AVP_dword );
    *seq++ = id;
  } while( id );
  va_end( arglist );

  return *this;
}


// ---
bool CAddr::operator ==( const CAddr& o ) {

  if ( m_count != o.m_count )
    return false;
  else {
    const AVP_dword* t;
    const AVP_dword* f;
    if ( o.m_ext ) {
      t = m_ext;
      f = o.m_ext;
    }
    else {
      t = m_addr;
      f = o.m_addr;
    }

    for( AVP_dword i=0; i<m_count+1; i++ )
      if ( t[i] != f[i] )
        return false;
    return true;
  }
}



// ---
CData::~CData() {
  if ( m_data && !::DATA_Get_Dad(m_data,0) )
    ::DATA_Remove( m_data, 0 );
}



// ---
HDATA CData::Go( AVP_dword id0, ... ) {
  AVP_dword  id;
  va_list    arglist;
  AVP_dword* cur;
  AVP_dword  addr[10];
  AVP_dword  count = 1;

  va_start( arglist, id0 );
  do {
    id = va_arg( arglist, AVP_dword );
    count++;
  } while( id );
  va_end( arglist );

  if ( count < (sizeof(addr)/sizeof(AVP_dword)) )
    cur = addr;
  else {
    _ASSERTE( allocator && liberator );
    cur = (AVP_dword*)allocator( count * sizeof(AVP_dword) );
    _ASSERT( cur );
  }

  va_start( arglist, id0 );
  id = id0;
  do {
    *cur++ = id;
    id = va_arg( arglist, AVP_dword );
  } while( id );
  va_end( arglist );
  *cur++ = id;

  HDATA data = Go( cur );

  if ( cur != addr )
    liberator( cur );

  return data;
}







