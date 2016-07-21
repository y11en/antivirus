#include "prop_in.h"

/*
#define DATA_IF_CHILDREN_FIRST  0
#define DATA_IF_SIBLINGS_FIRST  1
#define DATA_IF_ROOT_EXCLUDE    2
#define DATA_IF_ROOT_INCLUDE    0
#define DATA_IF_ROOT_SIBLINGS   4
#define DATA_IF_ROOT_CHILDREN   0
#define DATA_IF_DOESNOT_GO_DOWN 8
*/



// ---
DATA_PROC AVP_bool DATA_PARAM PROP_Compare( HPROP p1, HPROP p2 ) {

  if ( !p1 != !p2 )
    return 0;

  if ( !p1 )
    return 1;

  if ( PROP_ID(p1) == AVP_PID_END ) 
    return 1;

  switch( ((AVP_Property*)p1)->type ) {
    case avpt_nothing : return 1;
    case avpt_char    : return ( ((AVP_CHAR_Property*)p1)->val     == ((AVP_CHAR_Property*)p2)->val );
    case avpt_wchar   : return ( ((AVP_WCHAR_Property*)p1)->val    == ((AVP_WCHAR_Property*)p2)->val );
    case avpt_short   : return ( ((AVP_SHORT_Property*)p1)->val    == ((AVP_SHORT_Property*)p2)->val );
    case avpt_long    : return ( ((AVP_LONG_Property*)p1)->val     == ((AVP_LONG_Property*)p2)->val );
    case avpt_byte    : return ( ((AVP_BYTE_Property*)p1)->val     == ((AVP_BYTE_Property*)p2)->val );
    case avpt_group   : return ( ((AVP_GROUP_Property*)p1)->val    == ((AVP_GROUP_Property*)p2)->val );
    case avpt_word    : return ( ((AVP_WORD_Property*)p1)->val     == ((AVP_WORD_Property*)p2)->val );
    case avpt_dword   : return ( ((AVP_DWORD_Property*)p1)->val    == ((AVP_DWORD_Property*)p2)->val );
    case avpt_qword   : return ( ((AVP_QWORD_Property*)p1)->val    == ((AVP_QWORD_Property*)p2)->val );
    case avpt_longlong: return ( ((AVP_LONGLONG_Property*)p1)->val == ((AVP_LONGLONG_Property*)p2)->val );
		case avpt_size_t  : return ( ((AVP_SIZE_T_Property*)p1)->val   == ((AVP_SIZE_T_Property*)p2)->val );
    case avpt_bool    : return ( ((AVP_BOOL_Property*)p1)->val     == ((AVP_BOOL_Property*)p2)->val );
    case avpt_int     : return ( ((AVP_INT_Property*)p1)->val      == ((AVP_INT_Property*)p2)->val );
    case avpt_uint    : return ( ((AVP_UINT_Property*)p1)->val     == ((AVP_UINT_Property*)p2)->val );
    case avpt_date    : return ( !memcmp(&((AVP_DATE_Property*)p1)->val,    &((AVP_DATE_Property*)p2)->val,sizeof(AVP_date)) );
    case avpt_time    : return ( !memcmp(&((AVP_TIME_Property*)p1)->val,    &((AVP_TIME_Property*)p2)->val,sizeof(AVP_time)) );
    case avpt_datetime: return ( !memcmp(&((AVP_DATETIME_Property*)p1)->val,&((AVP_DATETIME_Property*)p2)->val,sizeof(AVP_datetime)) );
    case avpt_str     : return ( !strcmp(((AVP_STR_Property*)p1)->val,     ((AVP_STR_Property*)p2)->val) );
    case avpt_wstr    : return ( !wcscmp(((AVP_WSTR_Property*)p1)->val,    ((AVP_WSTR_Property*)p2)->val) );
    case avpt_bin     : 
      return ( 
        (((AVP_BIN_Property*)p1)->val.size == ((AVP_BIN_Property*)p2)->val.size) &&
        !memcmp( ((AVP_BIN_Property*)p1)->val.data, ((AVP_BIN_Property*)p2)->val.data, ((AVP_BIN_Property*)p1)->val.size ) 
      );
    default : 
      _RPT0( _CRT_ASSERT, "Bad property type" );
      return 0;
  }
}




// ---
DATA_PROC AVP_bool DATA_PARAM DATA_Compare( HDATA d1, AVP_dword* a1, HDATA d2, AVP_dword* a2, AVP_dword flags ) {

  if ( !d1 != !d2 )
    return 0;

  if ( !d1 )
    return 1;

  if ( a1 || a2 ) {
    if ( a1 && d1 )
      d1 = DATA_Find( d1, a1 );
    if ( a2 && d2 )
      d2 = DATA_Find( d2, a2 );
    if ( !d1 != !d2 )
      return 0;
    if ( !d1 )
      return 1;
  }


  while( 1 ) {

    if ( flags & DATA_IF_ROOT_EXCLUDE )
      flags &= ~DATA_IF_ROOT_EXCLUDE;

    else {
      AVP_Linked_Property* p1 = &((AVP_Data*)d1)->value;
      AVP_Linked_Property* p2 = &((AVP_Data*)d2)->value;

      while( 1 ) {

        if ( !p1 != !p2 )
          return 0;

        if ( !p1 )
          break;

        if ( PROP_ID(&p1->data) != PROP_ID(&p2->data) )
          return 0;

        if ( PROP_ID(&p1->data) == AVP_PID_END )
          break;

        if ( !PROP_Compare(PROP_HANDLE(&p1->data),PROP_HANDLE(&p2->data)) )
          return 0;

        p1 = p1->next;
        p2 = p2->next;
      }
    }

    if ( !(flags & DATA_IF_DOESNOT_GO_DOWN) ) {
      if ( !((AVP_Data*)d1)->child != !((AVP_Data*)d2)->child )
        return 0;
      if ( ((AVP_Data*)d1)->child && !DATA_Compare(DATA_HANDLE(((AVP_Data*)d1)->child),0,DATA_HANDLE(((AVP_Data*)d2)->child),0,flags|DATA_IF_ROOT_SIBLINGS) )
        return 0;
    }

    if ( flags & DATA_IF_ROOT_SIBLINGS ) {
      d1 = DATA_HANDLE( ((AVP_Data*)d1)->next );
      d2 = DATA_HANDLE( ((AVP_Data*)d2)->next );
      if ( !d1 != !d2 )
        return 0;
      if ( !d1 )
        break;
    }
    else
      break;
  }

  return 1;
}



