#include "signport.h"


#ifdef _WIN32

#include <windows.h>
#include <Sign\Sign_lib.h>
#include <Sign\ct_supp.h>

extern CT_PTR (CTAPI *ptrAMemAlloc)( CT_ULONG );
extern void   (CTAPI *ptrAMemFree)( CT_PTR );

// ---
static int destr( int ret, HANDLE file, char* buffer ) {
  if ( INVALID_HANDLE_VALUE != file ) 
    CloseHandle( file );
  if ( buffer )
    ptrAMemFree( buffer );
  return ret;
}

// ---
int sign_check_fileW( const wchar_t* file_name, int kasp_key, void* open_keys[], int count, int all_of ) {
  
  char* buffer = 0;
  HANDLE file = INVALID_HANDLE_VALUE;

  BOOL  done;
  int   read;
  int   file_len;

  file = CreateFileW( file_name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 ); 
  if ( INVALID_HANDLE_VALUE == file ) {

    DWORD err = GetLastError();
    switch( err ) {

      case ERROR_FILE_NOT_FOUND :
        return SIGN_FILE_NOT_FOUND;

      case ERROR_ACCESS_DENIED :
        return SIGN_CANT_READ;

      default :
        return SIGN_UNKNOWN_ERROR;

    }

  }

  file_len = GetFileSize( file, 0 );

  buffer = ptrAMemAlloc( file_len );

  if ( !buffer ) 
    return destr( ERROR_NOT_ENOUGH_MEMORY, file, buffer );

  done = ReadFile( file, buffer, file_len, &read, 0 ) && read == file_len;
  if ( !done ) 
    return destr( SIGN_CANT_READ, file, buffer );

  return destr( sign_check_buffer(buffer,file_len,0,kasp_key,open_keys,count,all_of), file, buffer );
}


#endif // _WIN32
