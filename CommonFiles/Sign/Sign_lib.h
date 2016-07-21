#ifndef __sign_lib_h
#define __sign_lib_h

#if defined( __unix__ )
	#include <compat_unix.h>	
#endif	

#if !defined(min)
  #define min(a,b) (((a) < (b)) ? (a) : (b))
#endif


#include <Sign/CT_STRCT.h>
#include <Sign/SIGN.h>
#include <Stuff/TEXT2BIN.h>

#ifndef GUID_DEFINED
#define GUID_DEFINED

typedef struct _GUID {          // size is 16
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID;

#endif // !GUID_DEFINED


#if defined( __cplusplus )
extern "C" {
#endif

int           sign_load_signature( CTN_Signature* sign, void* buffer, int buff_len, const char* password );

int           sign_save_pub_key( CTN_Signature* sign, void* pub_key, short length, short* out_len, int convert_to_text );
              
int           sign_buffer( CTN_Signature* sign, void* buffer, int b_len, int whole, void** notary, int* not_len, int force );

int           sign_find_notary( void* buffer, int buff_len, int* not_start, GUID* sign );
int           sign_find_notary_str( const void* buffer, int buff_len, int* not_start, const char* guid );
unsigned long sign_calc_hash_value( void* data, unsigned long len );
int           sign_put_notary( CTN_Signature* sign, void* notary, int* not_len );
int           sign_hash_buffer( CTN_Hash* hash, void* buffer, int len );
int           sign_collect_notaries( void* buffer, int buff_len, void* notary[], int* count, int* sign_start );

int           sign_get_kl_key( void* buffer, int buf_len, int* buf_len_out );

int           sign_get_txt_stream_version( const void* stream );

#ifdef _WIN32
int           sign_load_file_signature( CTN_Signature* sign, const char* file, const char* password );
int           sign_load_pub_key_file( const char* pub_key_file, void* pub_key );
int           sign_file( CTN_Signature* sign, const char* file, int force );
int           sign_fill_sgn_file( CTN_Signature* sign, const char* file_name, const char* pass, const char* user );
#endif // _WIN32

#if defined( N_PLAT_NLM )
  typedef void (*ROTATE_FUNCTION_POINTER)(void);
  extern ROTATE_FUNCTION_POINTER RotatePtr;
  void InitRotatePtr( ROTATE_FUNCTION_POINTER aPtr );
#endif

#if defined( MY_MEMUTIL )
  void* SIGNAPI s_mcpy( void* dst, const void* src, unsigned size );
  void* SIGNAPI s_mset( void* dst, unsigned fill, unsigned size );
  int   SIGNAPI s_mcmp( const void* c1, const void* c2, unsigned size );
#else
  #define s_mcpy memcpy
  #define s_mset memset
  #define s_mcmp memcmp
#endif


#if defined( __cplusplus )
}
#endif

#endif


