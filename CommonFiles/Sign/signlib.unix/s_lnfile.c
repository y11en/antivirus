#include "signport.h"


#ifdef _WIN32

#include <sign\sign_lib.h>
#include <windows.h>

// ---
static int destr( int ret, HANDLE file ) {
  if ( INVALID_HANDLE_VALUE != file )
    CloseHandle( file );
  return ret;
}


// ---
int sign_load_pub_key_file( const char* pub_key_file, void* pub_key ) {
  HANDLE file = INVALID_HANDLE_VALUE;
  MY_TRY {
    file = CreateFile( pub_key_file, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
    if ( INVALID_HANDLE_VALUE != file ) {
      DWORD done;
      DWORD size = GetFileSize( file, 0 );
      
      if ( size <= (PUBKEY_LEN_MIN-CT_N_LEN_SIGN) || PUBKEY_LEN_MAX < size )
        return destr( SIGN_BAD_OPEN_KEY, file );

      if ( ReadFile(file,pub_key,size,&done,0) && size == done ) 
        return destr( SIGN_OK, file );
      else 
        return destr( SIGN_CANT_READ, file );
    }
    else 
      return destr( SIGN_CANT_READ, file );
  }
  MY_EXCEPT
}

#endif // _WIN32


