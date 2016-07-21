#include "signport.h"


// ---
unsigned int __cdecl my_strl( const char * str ) {
  const char *eos = str;
  while( *eos++ );
  return (int)(eos - str - 1);
}
