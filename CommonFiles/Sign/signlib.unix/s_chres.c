#include "signport.h"


#ifdef _WIN32

#include <windows.h>
#include <sign\sign_lib.h>

// ---
int sign_load_notary_res( void** open_key, unsigned long not_inst, int res_id ) {
  
  HGLOBAL res;
  HRSRC   hrsrc;

  if ( !open_key || !not_inst || !res_id )
    return SIGN_BAD_PARAMETERS;

  *open_key = 0;

  if ( 
    ( hrsrc = FindResource((HINSTANCE)not_inst,MAKEINTRESOURCE(res_id),RT_RCDATA) ) != 0 &&
    ( res = LoadResource((HINSTANCE)not_inst,hrsrc) ) != 0 
  ) {
    DWORD size = SizeofResource( (HINSTANCE)not_inst, hrsrc );
    if ( size < (PUBKEY_LEN_MIN-CT_N_LEN_SIGN) || PUBKEY_LEN_MAX < size ) 
      return SIGN_BAD_OPEN_KEY;
    else {
      *open_key = LockResource( res );
      return SIGN_OK;
    }
  }
  else 
    return SIGN_BAD_PARAMETERS;
}


// ---
int sign_check_buffer_res( void* buffer, int buff_len, int* out_len, unsigned long not_inst, int not_res_id ) {
  void* open_key = 0;
  int res = sign_load_notary_res( &open_key, not_inst, not_res_id );
  if ( SIGN_OK == res )
    return sign_check_buffer( buffer, buff_len, out_len, 0, &open_key, 1, 1 );
  else
    return res;
}

#endif // _WIN32


