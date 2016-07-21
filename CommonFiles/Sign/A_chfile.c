//For inclusion only!!!
//
// Should be defined :
// SIGN_BUF_LEN
// SignCloseHandle
// SignReadFile
// SignCreateFile
#if (defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__)))
	#include <os2.h>
	#include <..\..\avp32\typedef.h>
	#include <_avpio.h>
	
	#define SIGN_BUF_LEN 0x2000
	#define  SignCloseHandle        AvpCloseHandle
	#define  SignReadFile           AvpReadFile
	#define  SignCreateFile         AvpCreateFile

#elif defined (__unix__)
	#include <AVPPort.h>
	#include <compat_unix.h>
	#include <_AVPIO.h>
	
	#define SIGN_BUF_LEN 0x2000
	#define  SignCloseHandle        AvpCloseHandle
	#define  SignReadFile           AvpReadFile
	#define  SignCreateFile         AvpCreateFile
	
#endif

#include "CT_SUPP.h"
#include "SIGN.h"


// ---
static int destr( int ret, HANDLE file, BYTE* buffer ) {
  if ( INVALID_HANDLE_VALUE != file )
    SignCloseHandle( file );
  if ( buffer )
    AMemFree( buffer );
  return ret;
}


int SIGNAPI Sign_check_call_back( void* params, void* buffer, int buff_len )
{
  DWORD read;
  if(SignReadFile( params, buffer, buff_len, &read, 0 ))
          return read;
  return -1;

}

int SIGNAPI sign_check_file( const char* file_name, int kasp_key, void* open_keys[], int count, int all_of ) {

  BYTE* buffer = 0;
  HANDLE file = INVALID_HANDLE_VALUE;

  file = SignCreateFile( file_name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
  if ( INVALID_HANDLE_VALUE == file )
    return destr( SIGN_CANT_READ, file, buffer );

  buffer = AMemAlloc( SIGN_BUF_LEN );
  if ( !buffer )
    return destr( SIGN_NOT_ENOUGH_MEMORY, file, buffer );

  return destr( sign_check_buffer_callback(&Sign_check_call_back,file,buffer,SIGN_BUF_LEN,0,kasp_key,open_keys,count,all_of), file, buffer );
}
/*
// ---
int SIGNAPI sign_check_file( const char* file_name, int kasp_key, void* open_keys[], int count, int all_of ) {

  BYTE* buffer = 0;
  HANDLE file = INVALID_HANDLE_VALUE;

  BOOL  done;
  DWORD read;
  int   file_len;

  file = AvpCreateFile( file_name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
  if ( INVALID_HANDLE_VALUE == file )
    return destr( SIGN_CANT_READ, file, buffer );

  file_len = AvpGetFileSize( file, 0 );
  buffer = AMemAlloc( file_len );

  if ( !buffer )
    return destr( SIGN_NOT_ENOUGH_MEMORY, file, buffer );

  done = AvpReadFile( file, buffer, file_len, &read, 0 ) && read == file_len;
  if ( !done )
    return destr( SIGN_CANT_READ, file, buffer );

  return destr( sign_check_buffer(buffer,file_len,0,kasp_key,open_keys,count,all_of), file, buffer );
}



*/
