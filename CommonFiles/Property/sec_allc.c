#include <stdarg.h>
#include "prop_in.h"


// ---
DATA_PROC AVP_dword* DATA_PARAM DATA_Alloc_Sequence( AVP_dword id0, ... ) {

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




// ---
DATA_PROC AVP_bool DATA_PARAM DATA_Remove_Sequence( AVP_dword* seq ) {
  _ASSERTE( allocator && liberator );
  liberator( seq );
  return 1;
}

