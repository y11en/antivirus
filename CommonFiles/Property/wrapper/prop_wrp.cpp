#include <stdarg.h>
#include "prop_in.h"
#include "AVPPort.h"
#include "prop_wrp.h"


// ---
CAddr::CAddr( AVP_dword* seq, ... ) 
  : m_count(0),
    m_addr( 0 ),
    m_ext( 0 )  {
}



// ---
CAddr::CAddr( AVP_dword id0, ... ) {
  AVP_dword  count;
  AVP_dword  id;
  va_list    arglist;
  AVP_dword* cur;
  AVP_dword* buffer;

  _ASSERTE( allocator && liberator );

  count = 1;
  va_start( arglist, id0 );
  do {
    id = va_arg( arglist, AVP_dword );
    count++;
  } while( id );
  va_end( arglist );

  buffer = (AVP_dword*)allocator( count * sizeof(AVP_dword) );
  _ASSERT( buffer );

  cur = buffer;

  va_start( arglist, id0 );
  id = id0;
  do {
    *cur++ = id;
    id = va_arg( arglist, AVP_dword );
  } while( id );
  va_end( arglist );
  *cur++ = id;

  return buffer;
}



~CAddr();

operator AVP_dword*();

AVP_dword* Add( ... );
AVP_dword* Make( HDATA data, AVP_dword* addr, HDATA stop_data, AVP_dword** seq );
const CAddr& operator =( const CAddr& other );
bool operator ==( const CAddr& other );
bool operator !=( const CAddr& other );
