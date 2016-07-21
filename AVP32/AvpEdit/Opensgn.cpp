#include "stdafx.h"
#include <Sign/ct_funcs.h>
#include "resource.h"
/*
#define IDS_ENTER_PASSWORD          100
#define IDS_REENTER_PASSWORD        101
#define IDS_ERR_INCORRECT_PASSWORD  102
*/
#define MAX_ATTEMPTS (3)

int GetData( HWND dad, int secure, const char* file, const char* prompt, char* data, int max_len );

CTN_Signature* OpenSgnFile( char* private_file, char* passw, HWND hwnd ) {

  int bad = 0;
  CT_INT res;
  CTN_Signature* sign = CTN_SignConstructor();
  HINSTANCE inst = 0;

  if( sign == NULL )
    return 0;

  res = CTN_GetPrivateKeyFile( sign, (CT_PTR)private_file );
  if ( res == CT_ERR_OK ) {
    int   i;
                int   dlg_res = IDOK;
    char  password[100];
    char  load_buffer[40];

    if ( passw )
      lstrcpy( password, passw );
    else
                  password[0] = 0;
                sign->Password = (CT_PTR)password;
    for ( i=0; i<MAX_ATTEMPTS && sign; i++ ) {
                        res = CTN_FillPrivateKeyStructure( sign );
                        if ( res ==     CT_ERR_PASSWORD ) {
        if ( !inst )
          inst = (HINSTANCE)GetWindowLong( hwnd, GWL_HINSTANCE );
        LoadString( inst, i == 0 ? IDS_ENTER_PASSWORD : IDS_REENTER_PASSWORD, load_buffer, sizeof(load_buffer) );
        dlg_res = GetData( 0, 1, private_file, load_buffer, password, sizeof(password) );
        if ( dlg_res != IDOK ) {
          bad = 1;
          break;
        }
                                continue;
                        }
      else if ( res != CT_ERR_OK ) {
        bad = 1;
        break;
                        }
                        else
                                break;
                };
                if ( i == MAX_ATTEMPTS ) {
      if ( !inst )
        inst = (HINSTANCE)GetWindowLong( hwnd, GWL_HINSTANCE );
      LoadString( inst, IDS_ERR_INCORRECT_PASSWORD, load_buffer, sizeof(load_buffer) );
      MessageBox( hwnd, load_buffer, "", MB_OK );
      bad = 1;
                }
  }
  else
    bad = 1;

  if ( bad ) {
    CTN_SignDestructor( sign );
    sign = 0;
  }

        return sign;
}


