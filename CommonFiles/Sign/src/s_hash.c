#include "signport.h"

#include <memory.h>
#include <Sign/ct_funcs.h>
#include <Sign/Sign_lib.h>

// ---
unsigned long sign_calc_hash_value( void* d, unsigned long c ) {
  unsigned long i = 0;
  unsigned long h = 0;
  for( ; i<c; i++,((unsigned char*)d)++ ) 
    h += *(unsigned char*)d;
  return h;
}

// ---
/*
unsigned long CalculateHashValue( void* data, long len ) {
  unsigned long i = 0;
  unsigned long c = len / sizeof( unsigned long );
  unsigned int  r = len - c * sizeof( unsigned long );
  unsigned long h = 0;
  for( ; i<c; i++,((unsigned long*)data)++ )
    h ^= *(long*)data;
  for( i=0; i<r; i++,((char*)data)++ ) {
    unsigned long v = (unsigned long)(*(unsigned char*)data) >> ( i * sizeof(char) );
    h ^= v;
  }
  return h;
}
*/


// ---
int sign_hash_buffer( CTN_Hash* hash, char* buffer, int len ) {
  int rest = len;
  int now = min( len, (int)0x7000 );
  char* ptr = buffer;

  if ( !hash || !buffer || len < 0 )
    return SIGN_BAD_PARAMETERS;

  MY_TRY {

    s_mset( hash, 0, sizeof(CTN_Hash) );
    CTN_SHAInit( hash );
    hash->Method = AVP_SIGN_METHOD;

    while( now ) {
      CTN_SHABuffer( hash, (CT_PTR)ptr, (CT_INT)now );
      ptr += now;
      rest -= now;
      now = min( rest, (int)0x7000 );
    }

    CTN_SHAEnd( hash );
    hash->flags |= CT_FLAGS_HASHDONE;

    return SIGN_OK;
  }
  MY_EXCEPT
}


