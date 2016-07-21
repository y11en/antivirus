#include "signport.h"

#include <memory.h>
#include <avp_defs.h>
#include <Sign\Sign_lib.h>
#include <Stuff\text2bin.h>

unsigned int __cdecl my_strl( const char * str );


// ---
int sign_save_pub_key( CTN_Signature* sign, void* open_key, short length, short* out_len, int text ) {

  CT_INT          res;
  CTN_Notary      not;
  short           len;
  short           name_len;
  short*          len_ptr;
  unsigned long*  hash_ptr;
  unsigned char*  data = (char*)open_key;
  unsigned char*  ptr = (char*)open_key;
  unsigned long   version = AVP_VERSION_NOW;

  name_len = my_strl( (char*)sign->Private.UserName );

  if ( text )
    len = 4 + B2T_LENGTH( PUBKEY_LEN(name_len) );
  else
    len = PUBKEY_LEN(name_len);

  if ( length < len )
    return SIGN_BAD_PARAMETERS;

  CTN_NotInitVariables( &not );
  res = CTN_FillPublicKeyStructure( sign, AVP_SIGN_METHOD, &not );

  if ( res != CT_ERR_OK ) 
    return SIGN_BAD_PARAMETERS;

  if ( text ) {
    s_mcpy( ptr, "0001", 4 ); 
    ptr += 4;
  }

  hash_ptr = (unsigned long*)(data = ptr);              ptr += sizeof(unsigned long);
  len_ptr  = (short*)ptr;                               ptr += sizeof(short);

  s_mcpy( ptr, &version, sizeof(unsigned long) );       ptr += sizeof(unsigned long);
  s_mcpy( ptr, &name_len, sizeof(short) );              ptr += sizeof(short);
  s_mcpy( ptr, (char*)not.Public.UserName, name_len );  ptr += name_len;

  s_mcpy( ptr, not.Public.B1, CT_N_LEN_SIGN );   ptr += CT_N_LEN_SIGN;
  s_mcpy( ptr, not.Public.B2, CT_N_LEN_SIGN );   ptr += CT_N_LEN_SIGN;
  s_mcpy( ptr, not.Public.B3, CT_N_LEN_SIGN );   ptr += CT_N_LEN_SIGN;
  s_mcpy( ptr, not.Public.B4, CT_N_LEN_SIGN );   ptr += CT_N_LEN_SIGN;

  len = (short)(((char*)ptr) - ((char*)hash_ptr));
  while( len && data[len-1] == 0 )
    len--;

  *len_ptr = len;
  *hash_ptr = sign_calc_hash_value( hash_ptr + 1, len - sizeof(unsigned long) );

  if ( text ) 
    len = BinToTextEx( hash_ptr, len, hash_ptr, length-4 );

  if ( out_len ) {
    *out_len = len;
    if ( text )
      *out_len += 4;
  }

  return SIGN_OK;
}



