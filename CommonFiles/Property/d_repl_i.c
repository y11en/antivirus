#include "prop_in.h"


// ---
static AVP_Property* find_prop( AVP_Data* data, AVP_dword id ) {
  AVP_Data* find = data->dad ? data->dad->child : data;
  id &= PROP_ID_MASK;
  while( find ) {
    if ( PROP_ID(&find->value.data) == id )
      return &find->value.data;
		/*
    AVP_Linked_Property* lp;
    lp = &find->value;
    while ( lp ) {
      if ( PROP_ID(&lp->data) == id )
        return &lp->data;
      lp = lp->next;
    }
		*/
    find = find->next;
  }
  return 0;
}


// ---
DATA_PROC AVP_bool DATA_PARAM DATA_Replace_App_ID( HDATA node, AVP_dword* addr, AVP_byte app_id, AVP_dword flags ) {
  AVP_Data* sibls;
  AVP_Data* data;

  FIND_DATA_EX( data, node, addr );

  if ( !(flags & DATA_IF_ROOT_EXCLUDE) ) {
    AVP_Property prop = data->value.data;
    prop.app_id = app_id;
    if ( !find_prop(data,PROP_ID(&prop)) )
      data->value.data.app_id = app_id;
  }
  sibls = data->next;
  while( sibls ) {
    if ( DATA_Replace_App_ID(DATA_HANDLE(sibls),0,app_id,flags&(~(DATA_IF_ROOT_SIBLINGS|DATA_IF_ROOT_EXCLUDE))) )
      sibls = sibls->next;
    else
      return 0;
  }
  if ( !(flags & DATA_IF_DOESNOT_GO_DOWN) && data->child ) 
    return DATA_Replace_App_ID( DATA_HANDLE(data->child), 0, app_id, (flags|DATA_IF_ROOT_SIBLINGS)&(~DATA_IF_ROOT_EXCLUDE) );
  else
    return 1;
}



// ---
DATA_PROC AVP_bool DATA_PARAM DATA_Replace_ID( HDATA node, AVP_dword* addr, AVP_word id ) {
  AVP_Data*     data;
  AVP_Property  prop;

  FIND_DATA_EX( data, node, addr );

  prop = data->value.data;
  prop.id = id;
  if ( find_prop(data,PROP_ID(&prop)) )
    return 0;
  else {
    data->value.data.id = id;
    return 1;
  }
}




