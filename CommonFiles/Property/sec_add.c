#include <stdarg.h>
#include "prop_in.h"

// ---
DATA_PROC AVP_dword* DATA_PARAM DATA_Add_Sequence( AVP_dword* sequence, ... ) {
  if ( sequence ) {
    va_list    arglist;
    AVP_dword* seq = sequence;
    AVP_dword  id;

    while( *seq )
      seq++;

    va_start( arglist, sequence );
    do {
      id = va_arg( arglist, AVP_dword );
      *seq++ = id;
    } while( id );
    va_end( arglist );
  }
  return sequence;
}



