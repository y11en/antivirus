#include "signport.h"

#include <io.h>
#include <fcntl.h>
#include <errno.h>
#include <Sign/Sign_lib.h>
#include <Sign/ct_supp.h>

extern CT_PTR (CTAPI *ptrAMemAlloc)( CT_ULONG );
extern void   (CTAPI *ptrAMemFree)( CT_PTR );

// ---
static int destr( int ret, int file, char* buffer ) {
  if ( -1 != file ) 
    _close( file );
  if ( buffer )
    ptrAMemFree( buffer );
  return ret;
}

// ---
int sign_check_file( const char* file_name, int kasp_key, void* open_keys[], int count, int all_of ) {
  
  char* buffer = 0;
  int   file = -1;
  long  file_len;
  int   file_read;

  file = _open( file_name, O_RDONLY|O_BINARY ); 
  if ( -1 == file ) {
    switch( errno ) {
      case ENOENT : return SIGN_FILE_NOT_FOUND;
      case EACCES : return SIGN_CANT_READ;
      default     : return SIGN_UNKNOWN_ERROR;
    }
  }

  file_len = _filelength( file );
  buffer = ptrAMemAlloc( file_len );

  if ( !buffer ) 
    return destr( SIGN_NOT_ENOUGH_MEMORY, file, buffer );

  file_read = _read( file, buffer, file_len );
  if ( file_read != file_len ) 
    return destr( SIGN_CANT_READ, file, buffer );

  return destr( sign_check_buffer(buffer,file_read,0,kasp_key,open_keys,count,all_of), file, buffer );
}


