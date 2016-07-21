#include "signport.h"
#include <Sign/CT_FUNCS.h>
#include <Sign/Sign_lib.h>
#include <Stuff/TEXT2BIN.h>

// ---
int sign_put_notary( CTN_Signature* sign, void* notary, int* not_len ) {
  MY_TRY {
    int res;
    char buffer[ NOTARY_LEN_R + NOTARY_LEN_S + 1 ];
    memset( buffer, 0, sizeof(buffer) );

    CTN_LANSSign( sign );

    SET_BEG_OF_NOT( notary );  
    (*(char**)&notary) += NOTARY_TXT_BEG;

    memcpy( notary, sign->Private.UserName, STR_GUID_TXT_LEN );
    (*(char**)&notary) += STR_GUID_TXT_LEN;

    memcpy( buffer,              sign->RS.r, NOTARY_LEN_R );
    memcpy( buffer+NOTARY_LEN_R, sign->RS.s, NOTARY_LEN_S );

    res = BinToText( buffer, NOTARY_VALUE_BIN_LEN, notary, NOTARY_VALUE_TXT_LEN );
    if ( res < NOTARY_VALUE_BIN_LEN )
      return SIGN_CANT_SIGN;

    (*(char**)&notary) += res;
    SET_END_OF_NOT( notary );

    if ( not_len )
      *not_len = NOTARY_TXT_SIZE; //lens[0] + sizeof(lens);

    return SIGN_OK;
  }
  MY_EXCEPT
}



