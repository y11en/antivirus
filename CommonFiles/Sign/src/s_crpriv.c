#include "signport.h"

#include <time.h>
#include <Sign\ct_funcs.h>
#include <Sign\Sign_lib.h>
#include <Stuff\text2bin.h>
#include <objbase.h>

// ---
int sign_fill_sgn_file( CTN_Signature* sign, const char* file_name, const char* pass, const char* user ) {
  
  int r;
  char buff[100];
  GUID guid;

  CoInitialize( 0 );
  CoCreateGuid( &guid );
  CoUninitialize();

  memset( buff, 0, sizeof(buff) );
  BinToText( &guid, sizeof(GUID), buff, sizeof(buff) );

  memset( sign, 0, sizeof(CTN_Signature) );
  CTN_SignInitVariables( sign );

  /*
  *sign = CTN_SignConstructor();
  if ( !*sign )
    return SIGN_NOT_ENOUGH_MEMORY;
  */

  switch( (r=CTN_GetPrivateKeyFile(sign,(CT_PTR)file_name)) ) {
    case CT_ERR_FILE_NOT_FOUND:
      return SIGN_FILE_NOT_FOUND;
    case CT_ERR_BAD_FILE:
      return SIGN_BAD_FILE;
    case CT_ERR_READ_ERROR:
      return SIGN_CANT_READ;
    case CT_ERR_OK:
      break;
    default:
      return SIGN_UNKNOWN_ERROR;
  }
  
  sign->Password = (CT_PTR)pass;

  switch( (r=CTN_FillPrivateKeyStructure(sign)) ) {
    case CT_ERR_OK:
    case CT_ERR_ABOUT_EXPIRATION_TIME:
      break;
    case CT_ERR_BAD_FILE:
      return SIGN_BAD_FILE;
    case CT_ERR_PASSWORD:
      return SIGN_BAD_PASSWORD;
    case CT_ERR_EXPIRED_TIME:
    case CT_ERR_DISK_ADDITION_KEY_NEED:
    case CT_ERR_TOUCH_ADDITION_KEY_NEED:
    default:
      return SIGN_UNKNOWN_ERROR;
  }

  strncpy( sign->Private.UserName,                    buff, STR_GUID_TXT_LEN );
  strncpy( sign->Private.UserName + STR_GUID_TXT_LEN, user, CT_N_LEN_USERNAME-STR_GUID_TXT_LEN );

  srand( (unsigned)time(NULL) );

  for( r=0; r<sizeof(buff); r++ ) 
    buff[r] = ' ' + 3 * 32 * rand() / RAND_MAX;

  sign->Private.Methods = AVP_SIGN_METHOD;

  CTN_CreateNewPrivateKey( sign, (CT_PTR)buff, 0 );
  switch( (r=CTN_SavePrivateKeyStructureToFile(sign,(CT_PTR)file_name)) ) {
    case CT_ERR_OK:
      break;
    case CT_ERR_CREATE_PRIVATE_KEY_FILE:
    case CT_ERR_WRITE_ERROR:
      return SIGN_CANT_WRITE;
    default:
      return SIGN_UNKNOWN_ERROR;
  }

  return SIGN_OK;
}

