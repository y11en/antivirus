#include "prop_in.h"


// ---
DATA_PROC AVP_dword DATA_PARAM DATA_Get_Id( HDATA node, AVP_dword* addr ) {
  AVP_Data* data;
  FIND_DATA_EX( data, node, addr );
  return PROP_ID( &data->value.data );
}


// ---
DATA_PROC HDATA DATA_PARAM DATA_Get_Dad( HDATA node, AVP_dword* addr ) {
  AVP_Data* data;
  FIND_DATA_EX( data, node, addr );
  return DATA_HANDLE( data->dad );
}



// ---
DATA_PROC HDATA DATA_PARAM DATA_Get_FirstEx( HDATA node, AVP_dword* addr, AVP_dword flags ) {
  AVP_Data* data;
  FIND_DATA_EX( data, node, addr );
  if ( flags & DATA_IF_ROOT_SIBLINGS ) {
    AVP_Data* prev = data->dad;
    if ( prev ) 
      return DATA_HANDLE( prev->child );
    else {
      _RPT0( _CRT_ASSERT, "Cannot find first node for root chain" );  
      return 0;                                  
    }
  }
  else
    return DATA_HANDLE( data->child );
}



// ---
DATA_PROC HDATA DATA_PARAM DATA_Get_LastEx( HDATA node, AVP_dword* addr, AVP_dword flags ) {
  AVP_Data* data;
  FIND_DATA_EX( data, node, addr );
  if ( !(flags & DATA_IF_ROOT_SIBLINGS) ) {
		data = data->child;
		if ( !data ) 
			return 0;
	}

	while( data->next )
		data = data->next;
  return DATA_HANDLE( data );
}



// ---
DATA_PROC HDATA DATA_PARAM DATA_Get_Next( HDATA node, AVP_dword* addr ) {
  AVP_Data* data;
  FIND_DATA_EX( data, node, addr );
  return DATA_HANDLE( data->next );
}



// ---
DATA_PROC HDATA DATA_PARAM DATA_Get_Prev( HDATA node, AVP_dword* addr ) {
  AVP_Data* data;
  AVP_Data* prev;
  
  FIND_DATA_EX( data, node, addr );
  
  prev = data->dad;
  if ( prev ) {
    prev = prev->child;
    if ( prev == data ) 
      return 0;
    else {
      while( prev && prev->next != data ) 
        prev = prev->next;
      return DATA_HANDLE( prev );
    }
  }
  else {
    _RPT0( _CRT_ASSERT, "Cannot find previous node for root chain" );  
    return 0;                                  
  }
}



// ---
DATA_PROC AVP_TYPE DATA_PARAM DATA_Get_Type( HDATA data, AVP_dword* addr, AVP_dword pid ) {
  return PROP_Get_Type( DATA_Find_Prop(data,addr,pid) );
}



// ---
DATA_PROC HPROP DATA_PARAM DATA_Get_First_Prop( HDATA data, AVP_dword* addr ) {
  FIND_DATA( data, addr );
  return PROP_HANDLE( &((AVP_Data*)data)->value.data );
}



// ---
HPROP DATA_PARAM PROP_Get_Next( HPROP prop ) {
  if ( prop ) {
    AVP_Linked_Property* lp = ((AVP_Linked_Property*)(((char*)prop) - sizeof(AVP_Linked_Property*)))->next;
    return lp && (PROP_ID(&lp->data) != AVP_PID_END) ? PROP_HANDLE(&lp->data) : 0;
  }
  else {
    _RPT0( _CRT_ASSERT, "Property handle is zero" );
    return 0;
  }
}



// ---
DATA_PROC HDATA DATA_PARAM PROP_Get_Dad( HPROP prop ) {
  if ( prop ) {
    AVP_Linked_Property* lp = (AVP_Linked_Property*)(((char*)prop) - sizeof(AVP_Linked_Property*));
    while( lp->next )
      lp = lp->next;
    if ( PROP_ID(&lp->data) == AVP_PID_END )
      return DATA_HANDLE( ((AVP_DWORD_Property*)&lp->data)->val );
    else {
      _RPT0( _CRT_ASSERT, "Bad property list" );
      return 0;
    }
  }
  else {
    _RPT0( _CRT_ASSERT, "Property handle is zero" );
    return 0;
  }
}




