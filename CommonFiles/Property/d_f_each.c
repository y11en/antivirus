#include "prop_in.h"


// ---
static AVP_dword DATA_PARAM iterate_sblings( AVP_Data* data, AVP_dword flags, For_Each_Proc proc, void* user_data ) {
  if ( flags & DATA_IF_ROOT_SIBLINGS ) {
    AVP_Data* sibls = data->next;
    while( sibls ) {
      AVP_dword result = DATA_For_Each( DATA_HANDLE(sibls), 0, flags&(~(DATA_IF_ROOT_SIBLINGS|DATA_IF_ROOT_EXCLUDE)), proc, user_data );
      if ( result )
        return result;
      else
        sibls = sibls->next;
    }
  }
  return 0;
}


// ---
DATA_PROC AVP_dword DATA_PARAM DATA_For_Each( HDATA node, AVP_dword* addr, AVP_dword flags, For_Each_Proc proc, void* user_data ) {
  AVP_dword result;
  AVP_Data* data;

// gcc 3.x "global optimization" bugfix to prevent compiler crash
#if (__GNUC__ < 3)
  if ( !proc ) {
    _RPT0( _CRT_ASSERT, "Bad parameters" );
    return 0;
  }
#endif
  FIND_DATA_EX( data, node, addr );

  result = 0;
  if ( !(flags & DATA_IF_ROOT_EXCLUDE) ) {
    result = proc( DATA_HANDLE(data), user_data );
    if ( result )
      return result;
  }
  if ( flags & DATA_IF_SIBLINGS_FIRST ) {
    result = iterate_sblings( data, flags, proc, user_data );
    if ( result )
      return result;
    if ( !(flags & DATA_IF_DOESNOT_GO_DOWN) && data->child ) 
      return DATA_For_Each( DATA_HANDLE(data->child), 0, (flags|DATA_IF_ROOT_SIBLINGS)&(~DATA_IF_ROOT_EXCLUDE), proc, user_data );
  }
  else {
    if ( !(flags & DATA_IF_DOESNOT_GO_DOWN) && data->child ) {
      result = DATA_For_Each( DATA_HANDLE(data->child), 0, (flags|DATA_IF_ROOT_SIBLINGS)&(~DATA_IF_ROOT_EXCLUDE), proc, user_data );
      if ( result )
        return result;
    }
    result = iterate_sblings( data, flags, proc, user_data );
    if ( result )
      return result;
  }
  return result;
}



