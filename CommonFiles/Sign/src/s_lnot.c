#include "signport.h"

#include <memory.h>
#include <byteorder.h>
#include <Avp_defs.h>
#include <Sign/Sign_lib.h>
#include <Stuff/text2bin.h>

unsigned int __cdecl my_strl( const char * str );

// ---
int SIGNAPI sign_load_open_key( void* notary, void* open_key, int* open_key_length ) {
  long* hash;
  short len;
  long  version = 1;
  short tmp_len;
  char* ptr;
  char  tmp_key[ PUBKEY_LEN_MAX ];
  CTN_Notary* not = (CTN_Notary*)notary;

  MY_TRY {

    if ( !open_key )
      return SIGN_BAD_PARAMETERS;

    version = sign_get_txt_stream_version( open_key );
    if ( version < 0 )
      return version;

    if ( version > 0 ) {
      tmp_len = my_strl( ((char*)open_key)+4 );
      if ( T2B_LENGTH(tmp_len) > sizeof(tmp_key) )
        return SIGN_BAD_OPEN_KEY;
      tmp_len = TextToBin( ((char*)open_key)+4, tmp_len, tmp_key, sizeof(tmp_key) );
      ptr = tmp_key;
    }
    else
      ptr = (char*)open_key;

    hash = (long*)ptr;    
    ptr += sizeof(long);

    len  = ReadWordPtr( (short*)ptr ); //*(short*)ptr; 

    if ( len <= (PUBKEY_LEN_MIN-(4*CT_N_LEN_SIGN)) || PUBKEY_LEN_MAX < len )
      return SIGN_BAD_OPEN_KEY;

    if ( open_key_length )
      *open_key_length = len;

    if ( ReadDWordPtr(hash) != (long)sign_calc_hash_value(ptr,len-sizeof(long)) )
      return SIGN_BAD_OPEN_KEY;

    ptr += sizeof(short);

    version = ReadDWordPtr( (long*)ptr );
// *(long*)ptr;
    ptr += sizeof(long);
    if ( version > AVP_VERSION_NOW  )
      return SIGN_BAD_KEY_VERSION;

    tmp_len = ReadWordPtr ((short*)ptr);
 //*(short*)ptr;
    ptr += sizeof(short);

    if ( tmp_len <= STR_GUID_TXT_LEN || CT_N_LEN_USERNAME < tmp_len )
      return SIGN_BAD_OPEN_KEY;

    if ( not ) {
      s_mcpy( (char*)not->Public.UserSN, szKaspUserSN, sizeofKaspUserSN );
      s_mcpy( (char*)not->Public.UserName, ptr, tmp_len );
      not->Public.UserName[ tmp_len ] = 0;
      ptr += tmp_len;

      s_mcpy( not->RS.UserSN, szKaspUserSN, CT_N_LEN_SN );
      not->RS.RSType = not->Public.Methods = AVP_SIGN_METHOD;
      not->flags |= CT_FLAGS_PUBLICKEYREAD; 

      tmp_len = min( len-(short)(ptr-(char*)open_key), CT_N_LEN_SIGN );
      if ( tmp_len ) {
        s_mcpy( not->Public.B1, ptr, tmp_len ); 
        ptr += tmp_len;

        tmp_len = min( len-(short)(ptr-(char*)open_key), CT_N_LEN_SIGN );
        if ( tmp_len ) {
          s_mcpy( not->Public.B2, ptr, tmp_len ); 
          ptr += tmp_len;

          tmp_len = min( len-(short)(ptr-(char*)open_key), CT_N_LEN_SIGN );
          if ( tmp_len ) {
            s_mcpy( not->Public.B3, ptr, tmp_len ); 
            ptr += tmp_len;

            tmp_len = min( len-(short)(ptr-(char*)open_key), CT_N_LEN_SIGN );
            if ( tmp_len ) 
              s_mcpy( not->Public.B4, ptr, tmp_len ); 
          }
        }
      }
    }
  }
  MY_EXCEPT
  return SIGN_OK;
}

