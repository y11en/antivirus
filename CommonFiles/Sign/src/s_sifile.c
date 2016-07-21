#include "signport.h"
#include <Sign\Sign_lib.h>
#include <Sign\ct_supp.h>
#include <Stuff\text2bin.h>
#include <Avp_base.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

extern CT_PTR (CTAPI *ptrAMemAlloc)( CT_ULONG );
extern void   (CTAPI *ptrAMemFree)( CT_PTR );

int correct_base_header( int file, AVP_BaseHeader* base );

// ---
static int destr( int ret, void* file_buf, int file ) {
  if ( file_buf )
    ptrAMemFree( file_buf );
  if ( -1 != file )
    _close( file );
  return ret;
}


// ---
int sign_file( CTN_Signature* sign, const char* file_name, int force ) {

  int   res;
  int   sign_start;
  int   sign_len;
  long  file_size;
  int   file = -1;
  void* file_buf = 0;

  MY_TRY {
    file = _open( file_name, O_RDWR | O_BINARY );
    if ( -1 == file ) 
      return destr( SIGN_CANT_READ, file_buf, file );

    file_size = _filelength( file );
    file_buf = ptrAMemAlloc( file_size + NOTARY_TXT_SIZE );

    if ( file_size == _read(file,file_buf,file_size) ) {
      void* signature;

      sign_start = file_size;
      res = sign_find_notary_str( file_buf, sign_start, &sign_start, (char*)sign->Private.UserName );
      if ( SIGN_OK == res ) {
        if ( !force ) {
          char  open_key[ PUBKEY_LEN_MAX ];
          void* o_k = open_key;
          short ok_len = (short)PUBKEY_LEN_BUF(sign);

          s_mset( open_key, 0, sizeof(open_key) );
          res = sign_save_pub_key( sign, open_key, ok_len, &ok_len, 0 );
          if ( SIGN_OK != res ) 
            return destr( res, file_buf, file );

          res = sign_check_buffer( file_buf, file_size, 0, 0, &o_k, 1, 1 );
          if ( SIGN_OK == res ) 
            return destr( SIGN_ALREADY_SIGNED, file_buf, file );

        }
        if ( -1 == _lseek(file,sign_start,SEEK_SET) ) 
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

          if ( -1 == _lseek(file,file_size,SEEK_SET) ) 
            return destr( SIGN_CANT_WRITE, file_buf, file );
        }
        //sign_check_buffer( file_buf, file_size, 0, 0, 0, 0 );
      }

      signature = file_buf;
      res = sign_buffer( sign, file_buf, file_size, 0, &signature, &sign_len, 1 );
      if ( (SIGN_OK == res) && (sign_len != _write(file,signature,sign_len)) )
        res = SIGN_CANT_WRITE;
    }
    else
      res = SIGN_CANT_READ;

    return destr( res, file_buf, file );
  }
  MY_EXCEPT
}


unsigned long CalcSum( unsigned char* mass, unsigned long len );

// ---
static int correct_base_header( int file, AVP_BaseHeader* base ) {

  long done;
  long offs = (long)&((AVP_BaseHeader*)0)->Magic;
  long f_pointer = _lseek( file, 0, SEEK_CUR );

  base->HeaderCRC = CalcSum( (unsigned char*)&base->Magic, sizeof(AVP_BaseHeader)-0x84 );
  
  if ( offs != _lseek(file,offs,SEEK_SET) ) 
    return 0;
  done = _write( file, &base->Magic, sizeof(AVP_BaseHeader)-offs );
  if ( done != (long)(sizeof(AVP_BaseHeader)-offs) ) 
    return 0;
  if ( f_pointer != _lseek(file,f_pointer,SEEK_SET) ) 
    return 0;

  return 1;
}





static unsigned long CalcSum( unsigned char* mass, unsigned long len ) {
  register unsigned long  i;
  register unsigned long  s = 0;
  register unsigned char* m = mass;

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


