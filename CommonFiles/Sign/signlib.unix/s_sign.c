#include "signport.h"
#include <Sign/CT_FUNCS.h>
#include <Sign/Sign_lib.h>

// ---
int sign_buffer( CTN_Signature* sign, void* buffer, int b_len, int hole, void** notary, int* not_len, int force ) {
  
  MY_TRY {

    int   res;
    int   not_start;
    static char local_notary[ NOTARY_TXT_SIZE ];

    if ( !sign || !buffer || b_len <= 0 || !notary )
      return SIGN_BAD_PARAMETERS;

    if ( !*notary )
      *notary = local_notary;

    if ( not_len )
      *not_len = 0;

    not_start = b_len;
    if ( hole )
      res = SIGN_NOTARY_NOT_FOUND;
    else 
      res = sign_find_notary_str( buffer, not_start, &not_start, (char*)sign->Private.UserName );

    if ( SIGN_OK == res ) {
      if ( force ) 
        sign_check_buffer( buffer, b_len, 0, 0, 0, 0, 0 );
      else {
        short ok_len = PUBKEY_LEN_BUF(sign);
        char open_key[ PUBKEY_LEN_MAX ];
        void* o_k = &open_key;

        res = sign_save_pub_key( sign, &o_k, ok_len, &ok_len, 0 );
        if ( SIGN_OK != res ) 
          return res;

        res = sign_check_buffer( buffer, b_len, 0, 0, &o_k, 1, 1 );
        if ( SIGN_OK == res ) 
          return SIGN_ALREADY_SIGNED;
      }
      *notary = ((char*)buffer) + not_start;
    }
    else if ( SIGN_NOTARY_NOT_FOUND != res ) 
      return res;
    else {
      if ( !hole )
        sign_check_buffer( buffer, b_len, &b_len, 1, 0, 0, 0 );
    }

    sign->Private.Methods = sign->Hash.Method = AVP_SIGN_METHOD;

    sign_hash_buffer( &sign->Hash, buffer, b_len );
    return sign_put_notary( sign, *notary, not_len );
  }
  MY_EXCEPT
}



