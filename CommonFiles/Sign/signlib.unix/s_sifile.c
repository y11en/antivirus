#include "signport.h"


#ifdef _WIN32

#include <windows.h>
#include <sign\sign_lib.h>
#include <sign\ct_supp.h>
#include <stuff\text2bin.h>
#include <avp_base.h>


int correct_base_header( HANDLE file, AVP_BaseHeader* base );

// ---
static int destr( int ret, void* file_buf, HANDLE file ) {
  if ( file_buf )
    AMemFree( file_buf );
  if ( INVALID_HANDLE_VALUE != file )
    CloseHandle( file );
  return ret;
}


// ---
int sign_file( CTN_Signature* sign, const char* file_name, int force ) {

  int     res;
  BOOL    ok;
  int     done;
  int     sign_start;
  int     sign_len;
  int     file_size;
  HANDLE  file = INVALID_HANDLE_VALUE;
  void*   file_buf = 0;

  MY_TRY {
    file = CreateFile( file_name, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
    if ( INVALID_HANDLE_VALUE == file ) 
      return destr( SIGN_CANT_READ, file_buf, file );

    file_size = GetFileSize( file, 0 );
    file_buf = AMemAlloc( file_size + NOTARY_TXT_SIZE );
    ok = ReadFile( file, file_buf, file_size, &done, 0 ) && done == file_size;

    if ( ok ) {
      void* signature;
      ok = FALSE;

      sign_start = file_size;
      res = sign_find_notary_str( file_buf, sign_start, &sign_start, (char*)sign->Private.UserName );
      if ( SIGN_OK == res ) {
        if ( !force ) {
          char  open_key[ PUBKEY_LEN_MAX ];
          void* o_k = open_key;
          short ok_len = PUBKEY_LEN_BUF(sign);

          memset( open_key, 0, sizeof(open_key) );
          res = sign_save_pub_key( sign, open_key, ok_len, &ok_len, 0 );
          if ( SIGN_OK != res ) 
            return destr( res, file_buf, file );

          res = sign_check_buffer( file_buf, file_size, 0, 0, &o_k, 1, 1 );
          if ( SIGN_OK == res ) 
            return destr( SIGN_ALREADY_SIGNED, file_buf, file );

        }
        if ( 0xffffffff == SetFilePointer(file,sign_start,0,FILE_BEGIN) ) 
          return destr( SIGN_CANT_READ, file_buf, file );
      }
      else if ( SIGN_NOTARY_NOT_FOUND != res )
        return destr( res, file_buf, file );
      else {
        AVP_BaseHeader* base_header = (AVP_BaseHeader*)file_buf;
        if ( base_header->Magic == AVP_MAGIC ) {
          
          base_header->FileSize += NOTARY_TXT_SIZE;

          if ( !correct_base_header(file,base_header) )
            return destr( SIGN_CANT_WRITE, file_buf, file );

          if ( 0xffffffff == SetFilePointer(file,file_size,0,FILE_BEGIN) ) 
            return destr( SIGN_CANT_WRITE, file_buf, file );
        }
        //sign_check_buffer( file_buf, file_size, 0, 0, 0, 0 );
      }

      signature = file_buf;
      res = sign_buffer( sign, file_buf, file_size, 0, &signature, &sign_len, 1 );
      if ( SIGN_OK == res ) {
        ok = WriteFile( file, signature, sign_len, &done, 0 ) && sign_len == done;
        if ( !ok )
          res = SIGN_CANT_WRITE;
      }
    }
    else
      res = SIGN_CANT_READ;

    return destr( res, file_buf, file );
  }
  MY_EXCEPT
}


DWORD CalcSum( BYTE* mass, DWORD len );

// ---
static int correct_base_header( HANDLE file, AVP_BaseHeader* base ) {

  DWORD done;
  DWORD offs = (DWORD)&((AVP_BaseHeader*)0)->Magic;
  DWORD f_pointer = SetFilePointer( file, 0, 0, FILE_CURRENT );

  base->HeaderCRC = CalcSum( (BYTE*)&base->Magic, sizeof(AVP_BaseHeader)-0x84 );
  
  if ( offs != SetFilePointer(file,offs,0,FILE_BEGIN) ) 
    return 0;
  if ( !WriteFile(file,&base->Magic,sizeof(AVP_BaseHeader)-offs,&done,0) || sizeof(AVP_BaseHeader)-offs != done ) 
    return 0;
  if ( f_pointer != SetFilePointer(file,f_pointer,0,FILE_BEGIN) ) 
    return 0;

  return 1;
}





static DWORD CalcSum( BYTE* mass, DWORD len ) {
  register DWORD i;
  register DWORD s = 0;
  register BYTE* m = mass;

  if ( len < 4 ) {
    for ( i=0; i<len; i++,m++ ) {
      s ^= s << 8;
      s ^= *m;
    }
    return s;
  }

  for( i=0; i<4; i++,m++ ) {
    s ^= s << 8;
    s ^= *m;
  }
  for ( ; i<len; i++,m++ ) {
    s ^= s << 1;
    s ^= *m;
  }
  return s;
}


#endif // _WIN32


