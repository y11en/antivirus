#include "prop_in.h"

// ---
void DATA_PARAM DATA_Free( AVP_Data* data ) {
  while( data->value.next ) {
    AVP_Linked_Property* prop = data->value.next;
    data->value.next = prop->next;
    PROP_Free( &prop->data );
    liberator( prop );
  }

  PROP_Free( &data->value.data );
}




