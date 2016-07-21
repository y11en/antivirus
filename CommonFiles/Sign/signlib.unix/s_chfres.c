#include "signport.h"


#ifdef _WIN32

#include <windows.h>
#include <sign\sign_lib.h>
#include <sign\ct_supp.h>


// ---
static int destr(  int ret, HANDLE file, char* buffer ) {
  if ( INVALID_HANDLE_VALUE != file ) 
    CloseHandle( file );
  if ( buffer )
    AMemFree( buffer );
  return ret;
}

// --- 
int sign_check_file_res( const char* file_name, unsigned long not_inst, int not_res_id ) {
  
  char* buffer = 0;
  HANDLE file = INVALID_HANDLE_VALUE;

  BOOL  done;
  int   read;
  int   file_len;

  file = CreateFile( file_name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 ); 
  if ( INVALID_HANDLE_VALUE == file ) 
    return destr( SIGN_CANT_READ, file, buffer );

  file_len = GetFileSize( file, 0 );
  buffer = AMemAlloc( file_len );

  if ( !buffer ) 
    return destr( SIGN_NOT_ENOUGH_MEMORY, file, buffer );

  done = ReadFile( file, buffer, file_len, &read, 0 ) && read == file_len;
  if ( !done ) 
    return destr( SIGN_CANT_READ, file, buffer );

  return destr( sign_check_buffer_res(buffer,file_len,0,not_inst,not_res_id), file, buffer );
}



#endif // _WIN32
