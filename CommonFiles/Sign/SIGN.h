#ifndef __sign_h
#define __sign_h

//#define EXCEPTION_ENABLE

#if defined( _WIN32 ) && defined( EXCEPTION_ENABLE )
  #define MY_TRY      __try
  #define MY_EXCEPT   __except( process_sign_exception(GetExceptionCode()) ) { return SIGN_UNKNOWN_ERROR; }
  
  #if defined( __cplusplus )
  extern "C" 
  #endif
  unsigned long process_sign_exception( unsigned long code );

#else
  #define MY_TRY      
  #define MY_EXCEPT   
#endif

#define SIGN_E_BASE                     -1000

#define SIGN_OK                         0
#define SIGN_UNKNOWN_ERROR              (SIGN_E_BASE - 0)
#define SIGN_NOT_MATCH                  (SIGN_E_BASE - 1)
#define SIGN_BAD_OPEN_KEY               (SIGN_E_BASE - 2)
#define SIGN_NOT_ENOUGH_MEMORY          (SIGN_E_BASE - 3)
#define SIGN_BAD_KEY_VERSION            (SIGN_E_BASE - 4)
#define SIGN_CANT_SIGN                  (SIGN_E_BASE - 5)
#define SIGN_BAD_PARAMETERS             (SIGN_E_BASE - 6)
#define SIGN_CANT_READ                  (SIGN_E_BASE - 7)
#define SIGN_CANT_WRITE                 (SIGN_E_BASE - 8)
#define SIGN_ALREADY_SIGNED             (SIGN_E_BASE - 9)
#define SIGN_BUFFER_HAS_NO_NOTARIES     (SIGN_E_BASE - 10)
#define SIGN_FILE_NOT_FOUND             (SIGN_E_BASE - 11)
#define SIGN_BAD_FILE                   (SIGN_E_BASE - 12)
#define SIGN_BAD_PASSWORD               (SIGN_E_BASE - 13)
#define SIGN_NOTARY_NOT_FOUND           (SIGN_E_BASE - 14)
#define SIGN_CALLBACK_FAILED            (SIGN_E_BASE - 15)
#define SIGN_BUFFER_TOO_SMALL           (SIGN_E_BASE - 16)


#define STR_GUID_TXT_LEN  (22)

#define AVP_SIGN_METHOD       CT_N_MET_LANS

#define NOTARY_LEN_R          ( CT_N_RS_LEN_LANS )
#define NOTARY_LEN_S          ( CT_N_HASH_LEN_LANS )
#define NOTARY_VALUE_BIN_LEN  ( NOTARY_LEN_R + NOTARY_LEN_S )
#define NOTARY_VALUE_TXT_LEN  ( (((NOTARY_LEN_R+NOTARY_LEN_S)+2)/3)*4 )

#define NOTARY_LEN_R_TXT      ( ((CT_N_RS_LEN_LANS+2)/3)*4-2 )
#define NOTARY_LEN_S_TXT      ( ((CT_N_HASH_LEN_LANS+2)/3)*4-2 )

#define NOTARY_TXT            ( 58 )
#define NOTARY_TXT_BEG        ( 4 )
#define NOTARY_TXT_END        ( 2 )
#define NOTARY_TXT_LEN        ( NOTARY_TXT_BEG + NOTARY_TXT )
#define NOTARY_TXT_SIZE       ( NOTARY_TXT_LEN + NOTARY_TXT_END )

#define MAX_NOT_COUNT         20
//#define NOTARY_MAXIMUM_LEN    ( sizeof(GUID)+CT_N_LEN_MAXRS+CT_N_LEN_MAXRS )

#define AVP_SIGN_CHAR_0       ((unsigned char)0x0d)
#define AVP_SIGN_CHAR_1       ((unsigned char)0x0a)
#define AVP_SIGN_CHAR_2       ((unsigned char)';')
#define AVP_SIGN_CHAR_3       ((unsigned char)' ')
#define AVP_SIGN_DWORD        ((unsigned long)( (AVP_SIGN_CHAR_0<<0) | (AVP_SIGN_CHAR_1<<8) | (AVP_SIGN_CHAR_2<<16) | (AVP_SIGN_CHAR_3<<24) ))

#define AVP_SIGN_CHAR_P       ((unsigned char)0xad)
#define AVP_SIGN_CHAR_L       ((unsigned char)0xad)
#define AVP_SIGN_WORD_L       ((unsigned short)( (AVP_SIGN_CHAR_P<<0) | (AVP_SIGN_CHAR_L<<8) ))

#if !defined (MACHINE_IS_BIG_ENDIAN)

	#define IS_BEG_OF_NOT( p )    ( (*((unsigned long*)(p))) == AVP_SIGN_DWORD )
	#define SET_BEG_OF_NOT( p )   ( (*((unsigned long*)(p))) = AVP_SIGN_DWORD )
	#define IS_END_OF_NOT( p )    ( (*((unsigned short*)(p))) == AVP_SIGN_WORD_L )
	#define SET_END_OF_NOT( p )   ( (*((unsigned short*)(p))) = AVP_SIGN_WORD_L )

#else

	#define IS_BEG_OF_NOT( p ) \
		( \
		( *((unsigned char*)((p)+0)) == AVP_SIGN_CHAR_0 ) && \
		( *((unsigned char*)((p)+1)) == AVP_SIGN_CHAR_1 ) && \
		( *((unsigned char*)((p)+2)) == AVP_SIGN_CHAR_2 ) && \
		( *((unsigned char*)((p)+3)) == AVP_SIGN_CHAR_3 ) )

	#define SET_BEG_OF_NOT( p ) \
		( \
		*(((unsigned char*)p)+0) = AVP_SIGN_CHAR_0, \
		*(((unsigned char*)p)+1) = AVP_SIGN_CHAR_1, \
		*(((unsigned char*)p)+2) = AVP_SIGN_CHAR_2, \
		*(((unsigned char*)p)+3) = AVP_SIGN_CHAR_3 )

	#define IS_END_OF_NOT( p ) \
		( \
		( *((unsigned char*)((p)+0)) == AVP_SIGN_CHAR_P ) && \
		( *((unsigned char*)((p)+1)) == AVP_SIGN_CHAR_L ) )

	#define SET_END_OF_NOT( p ) \
		( \
		*(((unsigned char*)p)+0) = AVP_SIGN_CHAR_P, \
		*(((unsigned char*)p)+1) = AVP_SIGN_CHAR_L )

#endif /* MACHINE_IS_BIG_ENDIAN */

#define PUBKEY_LEN_BUF(a) PUBKEY_LEN(min(strlen((char*)a->Private.UserName),CT_N_LEN_USERNAME))


// hash + length + version + len of(id+usr_name) + ( id + usr_name ) + sign
#define PUBKEY_LEN(name_len) ( \
  sizeof(unsigned long) + \
  sizeof(short)         + \
  sizeof(unsigned long) + \
  sizeof(short)         + \
  (name_len)            + \
  4 * CT_N_LEN_SIGN       \
)

#define PUBKEY_LEN_MIN    PUBKEY_LEN(STR_GUID_TXT_LEN)
#define PUBKEY_LEN_MAX    PUBKEY_LEN(CT_N_LEN_USERNAME)

#ifndef SIGNAPI
	#if defined (__unix__)
		#define SIGNAPI
	#else
		#define SIGNAPI __cdecl
	#endif /* __unix__ */
#endif /* SIGNAPI */

#if defined( __cplusplus )
extern "C" {
#endif

// Length of pub key buffer must be at least PUBKEY_LEN_MAX


// --------------------------------------------------------------------
// Initialization of memory management routines 
void sign_init_mem( void* (*memalloc)(size_t), void (*memfree)(void*) );

// --------------------------------------------------------------------
// Information about buffer :
//    int* out_len - lenght of buffer without any notaries
//    int* not_count - number of notaries
int SIGNAPI  sign_buffer_info( void* buffer, int buff_len, int* out_len, int* not_count );

// --------------------------------------------------------------------
// Validates buffer for set of pub keys
//   void*  buffer      - input buffer
//   int    buff_len    - input buffer length
//   int*   out_len     - length of buffer without any notaries
//   int    kasp_key    - do I have to use embeded Kaspersky Lab pub key
//   void*  pub_keys[]  - array of pointers to pub keys
//   int    key_count   - number of pub keys
//   int    all_of      - 0 - returns SIGN_OK if ONE of pub keys matched
//                        1 - returns SIGN_OK if ALL of pub keys matched
int SIGNAPI sign_check_buffer( void* buffer, int buff_len, int* out_len, int kasp_key, void* pub_keys[], int key_count, int all_of );

// --------------------------------------------------------------------
// Validates buffer with callback for set of pub keys
//   sign_check_call_back cb          - callback function - has to return number of bytes placed to buffer ( (-1) - error )
//   void*                params      - callback params
//   void*                buffer      - input buffer
//   int                  buff_len    - input buffer length
//   int*                 out_len     - length of buffer without any notaries
//   int                  kasp_key    - do I have to use embeded Kaspersky Lab pub key
//   void*                pub_keys[]  - array of pointers to pub keys
//   int                  key_count   - number of pub keys
//   int                  all_of      - 0 - returns SIGN_OK if ONE of pub keys matched
//                                      1 - returns SIGN_OK if ALL of pub keys matched
typedef int (SIGNAPI* sign_check_call_back)( void* params, void* buffer, int buff_len );
int SIGNAPI sign_check_buffer_callback( sign_check_call_back cb, void* params, void* buffer, int buff_len, int* out_len, int kasp_key, void* keys[], int key_count, int all_of );

// --------------------------------------------------------------------
// Validates file for set of pub keys
//   const char* file       - input buffer
//   int         kasp_key   - do I have to use embeded Kaspersky Lab pub key
//   void*       pub_keys[] - array of pointers to pub keys
//   int         key_count  - number of pub keys
//   int         all_of     - 0 - returns SIGN_OK if ONE of pub keys matched
//                            1 - returns SIGN_OK if ALL of pub keys matched
//
int SIGNAPI sign_check_file( const char* file, int kasp_key, void* pub_keys[], int key_count, int all_of );

int SIGNAPI sign_load_open_key( void* notary, void* pub_key, int* open_key_length );


#ifdef _WIN32 // only visible under WIN32
  // --------------------------------------------------------------------
  // Returns pointer to pub key loaded from resource
  //   void**                   pub_key     - pointer to loaded key
  //                                          this pointer does not allocated - do not free it !!!
  //   unsigned long(HINSTANCE) not_inst    - HINSTANCE of module with resource
  //   int                      not_res_id  - identificator of resource
  int SIGNAPI sign_load_notary_res( void** pub_key, unsigned long not_inst, int not_res_id );

  // --------------------------------------------------------------------
  // Validates buffer for pub key from resource
  //   void*                    buffer      - input buffer
  //   int                      buff_len    - input buffer length
  //   int*                     out_len     - length of buffer without any notaries
  //   unsigned long(HINSTANCE) not_inst    - HINSTANCE of module with resource
  //   int                      not_res_id  - identificator of resource
  int SIGNAPI sign_check_buffer_res( void* buffer, int buff_len, int* out_len, unsigned long not_inst, int not_res_id );

#if defined(UNICODE) || defined(_UNICODE)
#define sign_check_file sign_check_fileW
  int SIGNAPI sign_check_fileW( const wchar_t* file, int kasp_key, void* pub_keys[], int key_count, int all_of );
#endif
	
  // --------------------------------------------------------------------
  // Validates file for set of pub keys
  //   const char*              file        - input buffer
  //   unsigned long(HINSTANCE) not_inst    - HINSTANCE of module with resource
  //   int                      not_res_id  - identificator of resource
  int SIGNAPI sign_check_file_res( const char* file, unsigned long not_inst, int not_res_id );


#endif // _WIN32


#if defined( __cplusplus )
}
#endif


#endif
