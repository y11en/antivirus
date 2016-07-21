#include <stdarg.h>
#include "prop_in.h"


// ---
DATA_PROC AVP_dword* DATA_PARAM DATA_Sequence( AVP_dword* buffer, ... ) {

  if ( buffer ) {
    AVP_dword  id;
    va_list    arglist;
    AVP_dword* cur = (AVP_dword*)buffer;

    va_start( arglist, buffer );
    do {
      id = va_arg( arglist, AVP_dword );
      *cur++ = id;
    } while( id );
    va_end( arglist );
  }
  return buffer;
}




