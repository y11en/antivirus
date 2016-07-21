#if !defined (__unix__)
	#include "signport.h"
	#include <objbase.h>
	#include <sign\ct_funcs.h>
	#include <sign\ct_supp.h>
	#include <sign\sign_lib.h>
	#include <stuff\text2bin.h>
	#include <avp_defs.h>
#else
	#include <AVPPort.h>
	#include "signport.h"	
	#include <Sign/CT_FUNCS.h>
	#include <Sign/CT_SUPP.h>
	#include <Sign/Sign_lib.h>
	#include <Stuff/TEXT2BIN.h>
	#include "avp_defs.h"
#endif

extern char kl_ok[256];

typedef unsigned char byte;
typedef byte* pbyte;
typedef byte  not_arr[MAX_NOT_COUNT][NOTARY_TXT_SIZE];


//void* SIGNAPI s_mcpy( void* dst, const void* src, unsigned size );

#if defined( MY_MEMUTIL )
void* SIGNAPI s_mcpy( void* dst, const void* src, size_t size ); 
void* SIGNAPI s_mset( void* dst, int fill, size_t size ); 
int   SIGNAPI s_mcmp( const void* c1, const void* c2, unsigned size ); 
#endif

static int free_not( void* not, void* notaries, int ret ) {
  if ( not )
    AMemFree( not );
  if ( notaries )
    AMemFree( notaries );
  return ret;
}


#define CMP(ptr,o) (*((unsigned char*)((ptr)+o)) == AVP_SIGN_CHAR_##o)

// ---
static pbyte find_beg_of_notary( void* buffer, int buff_len ) {

  pbyte ptr = (pbyte)buffer;

  for( ; buff_len>=NOTARY_TXT_BEG; buff_len--,ptr++ ) 
    if ( IS_BEG_OF_NOT(ptr) )
      return ptr;
//  buff_len;
  if ( buff_len >= (NOTARY_TXT_BEG-1) ) {
    if ( CMP(ptr,0) && CMP(ptr,1) && CMP(ptr,2) )
      return ptr;
    buff_len--;
    ptr++;
  }
  if ( buff_len >= (NOTARY_TXT_BEG-2) ) {
    if ( CMP(ptr,0) && CMP(ptr,1) )
      return ptr;
    buff_len--;
    ptr++;
  }
  if ( buff_len >= (NOTARY_TXT_BEG-3) ) {
    if ( CMP(ptr,0) )
      return ptr;
  }
  return 0;

}


// ---
static void hash_buffer( CTN_Hash* hash, pbyte ptr, int buff_len ) {
  int rest = buff_len;
  int now = (buff_len < (int)0x7000) ? buff_len : (int)0x7000;
  while( now > 0 ) {
    CTN_SHABuffer( hash, (CT_PTR)ptr, (CT_INT)now );
    ptr += now;
    rest -= now;
    now = (rest < (int)0x7000) ? rest : (int)0x7000;
  }
}

//#define NOT_ERROR     ( -1 )
#define NOTARY_NOT_FOUND ( -2 )
#define NOTARY_CONTINUED ( -3 )
#define NOTARY_FOUND     ( -4 )

// ---
static int count_notaries( pbyte buffer, int buff_len, int* continue_len, int* count, not_arr* notaries, int* content ) { 
  
  pbyte ptr = 0;
  int next_round;
  int whole_buff_len = buff_len;
//  pbyte whole_buffer = buffer;

  *content = buff_len;

  if ( buff_len <= 0 )
    return NOTARY_NOT_FOUND;

  if ( *continue_len ) {

    if ( buff_len >= (NOTARY_TXT_SIZE-*continue_len) ) {

      int rest = NOTARY_TXT_BEG - *continue_len;

      next_round = 1;

      if ( rest > 0 ) {
        s_mcpy( &(*notaries)[*count][*continue_len], buffer, rest );
        if ( IS_BEG_OF_NOT(&(*notaries)[*count][0]) ) {
          buffer += rest;
          buff_len -= rest;
          *content -= rest;
          *continue_len = NOTARY_TXT_BEG;
        }
        else {
          *continue_len = 0;
          *count = 0;
          *content = whole_buff_len;
          next_round = 0;
        }
      }

      if ( next_round ) {
        rest = NOTARY_TXT_SIZE - *continue_len;

        if ( rest > 0 ) {
          s_mcpy( &(*notaries)[*count][*continue_len], buffer, rest );
          if ( IS_END_OF_NOT(&(*notaries)[*count][NOTARY_TXT_LEN]) ) {
            buffer += rest;
            buff_len -= rest;
            (*count)++;
            *continue_len = 0;
            *content -= rest;
            if ( buff_len == 0 ) {
              *content = 0;
              return NOTARY_FOUND;
            }
          }
          else 
            *count = 0;
        }
        else 
          *count = 0;
      }

    }
    else {
      *count = 0;
      *continue_len = 0;
      *content = whole_buff_len;
      return NOTARY_NOT_FOUND;
    }
  }

  while( 1 ) {
    next_round = 0;
    ptr = find_beg_of_notary( buffer, buff_len );
    if ( ptr ) {
    
      if ( *count && ptr > buffer ) {
        *count = 0;
        *continue_len = 0;
        *content = whole_buff_len;
      }

      buff_len -= ptr - buffer;

      while ( buff_len > 0 ) {
        if ( buff_len < NOTARY_TXT_BEG ) {
          int c = 1;
          int r = 0;
          if ( r++,*(ptr++) == AVP_SIGN_CHAR_0 ) 
            (*notaries)[*count][0] = AVP_SIGN_CHAR_0;
          else 
            c = 0;
          if ( c && buff_len > 1 ) {
            if ( r++,*(ptr++) == AVP_SIGN_CHAR_1 ) 
              (*notaries)[*count][1] = AVP_SIGN_CHAR_1;
            else 
              c = 0;
          }
          if ( c && buff_len > 2 ) {
            if ( r++,*(ptr++) == AVP_SIGN_CHAR_2 ) 
              (*notaries)[*count][2] = AVP_SIGN_CHAR_2;
            else 
              c = 0;
          }
          if ( c ) {
            *continue_len = buff_len;
            *content -= buff_len;
            return NOTARY_CONTINUED;
          }
          else {
            *count = 0;
            *continue_len = 0;
            *content = whole_buff_len;
            buffer = ptr;
            buff_len -= r;
            next_round = 1;
            break;
          }
        }
        else if ( IS_BEG_OF_NOT(ptr) ) {
          if ( buff_len >= NOTARY_TXT_SIZE ) {
            if ( IS_END_OF_NOT(ptr+NOTARY_TXT_LEN) ) {
              s_mcpy( &(*notaries)[*count][0], ptr, NOTARY_TXT_SIZE );
              (*count)++;
              ptr += NOTARY_TXT_SIZE;
              buff_len -= NOTARY_TXT_SIZE;
              *content -= NOTARY_TXT_SIZE;
            }
            else {
              *count = 0;
              *continue_len = 0;
              *content = whole_buff_len;
              buffer = ptr+1;
              buff_len--;
              next_round = 1;
              break;
            }
          }
          else {
            s_mcpy( &(*notaries)[*count][0], ptr, buff_len );
            *continue_len = buff_len;
            *content -= buff_len;
            return NOTARY_CONTINUED;
          }
        }
        else {
          *count = 0;
          *continue_len = 0;
          *content = whole_buff_len;
          buffer = ptr;
          next_round = 1;
          break;
        }
      }
      if ( !next_round ) {
        if ( buff_len == 0 ) 
          return NOTARY_FOUND;
        else {
          *count = 0;
          *continue_len = 0;
          *content = whole_buff_len;
          return NOTARY_NOT_FOUND;
        }
      }
    }
    else {
      *count = 0;
      *continue_len = 0;
      *content = whole_buff_len;
      return NOTARY_NOT_FOUND;
    }
  }
}


// ---
int SIGNAPI sign_check_buffer_callback( sign_check_call_back cb, void* params, void* buffer, int buff_len, int* out_len, int kasp_key, void* keys[], int key_count, int all_of ) {
  
  int i, found_not, found_not_prev;
  int rest_of_buf;
  int found_one_of;
  CTN_Notary* not = 0;
  CTN_Hash    hash;
  int         contin = 0;
  int         sign_count = 0;
  not_arr*    notaries = 0;
  char        conv_buffer[ NOTARY_VALUE_BIN_LEN+2 ];

  MY_TRY {

    int j, res, local_len, loaded, stop;

    if ( buff_len <= NOTARY_TXT_SIZE )
      return free_not( not, notaries, SIGN_BUFFER_TOO_SMALL );

    if ( keys && !key_count )
      return free_not( not, notaries, SIGN_BAD_PARAMETERS );

    if ( !keys && !kasp_key )
      return free_not( not, notaries, SIGN_BAD_PARAMETERS );

    if ( !out_len )
      out_len = &local_len;

    *out_len = 0;

    if ( cb ) {
      loaded = cb( params, buffer, buff_len );
      if ( loaded < 0 || loaded > buff_len )
        return free_not( not, notaries, SIGN_CALLBACK_FAILED );
      else {
        stop = loaded < buff_len;
        buff_len = loaded;
      }
    }
    else
      stop = 1;


    notaries = (not_arr*)AMemAlloc( sizeof(not_arr) );
    if ( notaries ) 
      s_mset( notaries, 0, sizeof(not_arr) );
    else
      return free_not( not, notaries, SIGN_NOT_ENOUGH_MEMORY );

    not = (CTN_Notary*)AMemAlloc( sizeof(CTN_Notary) );
    if ( !not )
      return free_not( not, notaries, SIGN_NOT_ENOUGH_MEMORY );

    CTN_NotInitVariables( not );

    CTN_SHAInit( &hash );

    found_not_prev = NOTARY_NOT_FOUND;
    rest_of_buf = 0;

    while( 1 ) {
    
      int content_of_buf;

      content_of_buf = buff_len;
      found_not = count_notaries( buffer, buff_len, &contin, &sign_count, notaries, &content_of_buf );

      if ( content_of_buf || found_not_prev==NOTARY_NOT_FOUND ) {
        *out_len += rest_of_buf + content_of_buf;
        rest_of_buf = buff_len - content_of_buf;
        s_mcpy( &not->Hash, &hash, sizeof(hash) );
      }
      else
        rest_of_buf += buff_len;

      hash_buffer( &hash, buffer, buff_len );

      if ( 0 < content_of_buf && content_of_buf < buff_len )
        hash_buffer( &not->Hash, buffer, content_of_buf );

      if ( cb && !stop ) {
        loaded = cb( params, buffer, buff_len );
        if ( loaded < 0 || loaded > buff_len )
          return free_not( not, notaries, SIGN_CALLBACK_FAILED );
        else if ( loaded < buff_len ) {
          stop = 1;
          buff_len = loaded;
        }
      }
      else {
        if ( buff_len > 0 ) {
          if ( found_not != NOTARY_FOUND ) 
            sign_count = 0;
        }
        else if ( found_not_prev != NOTARY_FOUND )
          sign_count = 0;
        break;
      }

      found_not_prev = found_not;
    }

    CTN_SHAEnd( &not->Hash );
    not->Hash.flags |= CT_FLAGS_HASHDONE;

    /* possible way to check
    s_mset( &hash, 0, sizeof(hash) );
    CTN_SHAInit( &hash );
    hash.HashLen = not->Hash.HashLen;
    hash.Method = not->Hash.Method;
    hash.flags = not->Hash.flags;
    s_mcpy( &hash.HashValue, &not->Hash.HashValue, sizeof(not->Hash.HashValue) );
    hash.CT_N_u_Hash.SHA.posh = not->Hash.CT_N_u_Hash.SHA.posh;
    hash.CT_N_u_Hash.SHA.poshash = not->Hash.CT_N_u_Hash.SHA.poshash;
    s_mcpy( &hash.CT_N_u_Hash.SHA.HV, &not->Hash.CT_N_u_Hash.SHA.HV, sizeof(not->Hash.CT_N_u_Hash.SHA.HV) );
    */
    s_mcpy( &hash, &not->Hash, sizeof(hash) );

    found_one_of = 0;
    kasp_key = !!kasp_key;
    for( i=0; i<key_count + kasp_key; i++ ) {
      void* key;
      if ( kasp_key && i==0 )
        key = kl_ok;
      else 
        key = keys[i-kasp_key];
      //void* key = (i<key_count) ? keys[i] : kl_ok;

      if ( !key ) 
        return free_not( not, notaries, SIGN_BAD_PARAMETERS );

      CTN_NotInitVariables( not );
      s_mcpy( &not->Hash, &hash, sizeof(hash) );
      res = sign_load_open_key( not, key, 0 );

      if ( res == SIGN_OK ) {
        int found_all_of = 0;

        res = CT_ERR_BAD_FILE;
        for( j=0; j<sign_count; j++ ) {
          char* p = ((char*)&(*notaries)[j]) + NOTARY_TXT_BEG;
          if ( !s_mcmp(not->Public.UserName,p,STR_GUID_TXT_LEN) ) {
            found_one_of = 1;
            found_all_of = 1;
            s_mset( not->RS.r, 0, CT_N_LEN_MAXRS );
            s_mset( not->RS.s, 0, CT_N_LEN_MAXRS );
            TextToBin( p + STR_GUID_TXT_LEN, NOTARY_VALUE_TXT_LEN, conv_buffer, NOTARY_VALUE_BIN_LEN+2 );
            s_mcpy( not->RS.r, conv_buffer,              NOTARY_LEN_R );
            s_mcpy( not->RS.s, conv_buffer+NOTARY_LEN_R, NOTARY_LEN_S );
            res = CTN_LANSNot( not );
            break;
          }
        }
        if ( all_of ) {
          if ( res != CT_ERR_OK ) {
            if ( found_all_of )
              return free_not( not, notaries, SIGN_NOT_MATCH );
            else
              return free_not( not, notaries, SIGN_NOTARY_NOT_FOUND );
          }
        }
        else if ( res == CT_ERR_OK )
          return free_not( not, notaries, SIGN_OK );
      }
      else
        return res/*SIGN_BAD_PARAMETERS*/;
    }
    if ( all_of )
      return free_not( not, notaries, SIGN_OK );
    else {
      if ( found_one_of )
        return free_not( not, notaries, SIGN_NOT_MATCH );
      else
        return free_not( not, notaries, SIGN_NOTARY_NOT_FOUND );
    }
  }
  MY_EXCEPT
}



// ---
/*
void* SIGNAPI s_mcpy( void* dst, const void* src, unsigned size ) {
  unsigned i;
  void*  ret = dst;
#if defined (MACHINE_IS_BIG_ENDIAN)
  for( i=0; i<size; i++ )
    *((*(unsigned char **)&dst)++) = *((*(unsigned char**)&src)++);
#else 
  unsigned u_size = size / sizeof(unsigned);
  for( i=0; i<u_size; i++ )
    *((*(unsigned**)&dst)++) = *((*(unsigned**)&src)++);
  u_size = size % sizeof(unsigned);
  for( i=0; i<u_size; i++ )
    *((*(unsigned char**)&dst)++) = *((*(unsigned char**)&src)++);
#endif
  return ret;
}
*/
#if defined( MY_MEMUTIL )
// ---
void* SIGNAPI s_mset( void* dst, int fill, size_t size ) {
  unsigned i;
  void*  ret = dst;
#if defined (MACHINE_IS_BIG_ENDIAN)
  for( i=0; i<size; i++ )
    *((*(unsigned char **)&dst)++) = fill;
#else
  unsigned u_size = size / sizeof(unsigned);
  if ( u_size ) {
    unsigned u_fill;
    if ( sizeof(short) < sizeof(unsigned) )
      u_fill = ((unsigned char)fill) | (((unsigned char)fill)<<8) | (((unsigned char)fill)<<16) | (((unsigned char)fill)<<24);
    else
      u_fill = ((unsigned char)fill) | (((unsigned char)fill)<<8);
    for( i=0; i<u_size; i++ )
      *(((unsigned*)dst)++) = u_fill;
  }
  u_size = size % sizeof(unsigned);
  for( i=0; i<u_size; i++ )
    *(((unsigned char*)dst)++) = (unsigned char)fill;
#endif
  return ret;
}

// ---
int SIGNAPI s_mcmp( const void* c1, const void* c2, unsigned size ) {
  unsigned i;
  int  ret = 0;
#if defined (MACHINE_IS_BIG_ENDIAN)
  for( i=0; i<size; i++ )
   {
    ret = *((*(unsigned char**)&c1)++) - *((*(unsigned char**)&c2)++);
    if ( ret )
      return ret;
   }
#else
  unsigned u_size = size / sizeof(unsigned);
  for( i=0; i<u_size; i++ ) {
    ret = *((*(unsigned**)&c1)++) - *((*(unsigned**)&c2)++);
    if ( ret )
      return ret;
  }
  u_size = size % sizeof(unsigned);
  for( i=0; i<u_size; i++ ) {
    ret = *((*(unsigned char**)&c1)++) - *((*(unsigned char**)&c2)++);
    if ( ret )
      return ret;
  }
#endif
  return 0;
}

#endif
