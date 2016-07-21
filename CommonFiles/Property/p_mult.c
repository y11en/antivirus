#include "prop_in.h"

// ---
void DATA_PARAM PROP_Multiplication( AVP_Data* d1, AVP_Data* d2 ) {
  AVP_Linked_Property* prop = d1->value.next;
  while( prop ) {
    AVP_Linked_Property* next = prop->next;
    if ( !DATA_Find_Prop(DATA_HANDLE(d2),0,PROP_ID(&prop->data)) )
      DATA_Remove_Prop_H( DATA_HANDLE(d1), 0, PROP_HANDLE(prop) );
    prop = next;
  }
}




