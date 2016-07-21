#include "signport.h"
#include <Sign/Sign_lib.h>
#include <Sign/CT_FUNCS.h>



// ---
int sign_load_signature( CTN_Signature* sign, void* buffer, int buff_len, const char* password ) {

  int res;

  if ( !sign || !buffer )
    return SIGN_BAD_PARAMETERS;
  else if ( buff_len < CT_N_LEN_PRIVATEKEYFILE )
    return SIGN_BUFFER_TOO_SMALL;

  MY_TRY {
    CTN_SignInitVariables( sign );

    memcpy( sign->PrivateKeyBuffer, buffer, CT_N_LEN_PRIVATEKEYFILE );
    sign->flags |= CT_FLAGS_PRIVATEKEYREAD;

    sign->Password = (CT_PTR)password;
    res = CTN_FillPrivateKeyStructure( sign );
  }
  MY_EXCEPT

  if ( res == CT_ERR_BAD_FILE )
    return SIGN_BAD_FILE;
  else if ( res == CT_ERR_PASSWORD )
    return SIGN_BAD_PASSWORD;
  else
    return SIGN_OK;

}




