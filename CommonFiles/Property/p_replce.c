#include "prop_in.h"

// ---
static AVP_Property* find_prop_2( AVP_Data* data, AVP_Property* prop, AVP_dword pid ) {
  if ( &data->value.data == (AVP_Property*)prop ) {
    AVP_Data* f = data->dad ? data->dad->child : data;
    while( f ) {
      if ( PROP_ID(&f->value.data) == pid )
        return &f->value.data;
      else
        f = f->next;
    }
  }
  else {
    AVP_Linked_Property* lp = data->value.next;
    while( lp ) {
      if ( PROP_ID(&lp->data) == pid )
        return &lp->data;
      else
        lp = lp->next;
    }
  }
  return 0;
}


// ---
DATA_PROC AVP_bool DATA_PARAM PROP_Replace_App_ID ( HPROP prop, AVP_byte app_id ) {
  AVP_Data* data = (AVP_Data*)PROP_Get_Dad( prop );
  if ( data ) {
    AVP_Property pr = *(AVP_Property*)prop;
    pr.app_id = app_id;

    if ( find_prop_2(data,(AVP_Property*)prop,PROP_ID(&pr)) )
      return 0;
    else {
      ((AVP_Property*)prop)->app_id = app_id;
      return 1;
    }
  }
  else
    return 0;
}



// ---
DATA_PROC AVP_bool DATA_PARAM PROP_Replace_ID ( HPROP prop, AVP_word id ) {
  AVP_Data* data = (AVP_Data*)PROP_Get_Dad( prop );
  if ( data ) {
    AVP_Property pr = *(AVP_Property*)prop;
    pr.id = id;

    if ( find_prop_2(data,(AVP_Property*)prop,PROP_ID(&pr)) )
      return 0;
    else {
      ((AVP_Property*)prop)->id = id;
      return 1;
    }
  }
  else
    return 0;
}




