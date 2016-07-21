#include "prop_in.h"


// ---
DATA_PROC HDATA DATA_PARAM DATA_Find( HDATA data, AVP_dword* addr ) {
  if ( data && addr ) {
    AVP_dword curr = *addr++ & PROP_ID_MASK;
    while ( curr ) {
      AVP_Data* dad = (AVP_Data*)data;
      data = DATA_HANDLE( ((AVP_Data*)data)->child );
      if ( AVP_PID_QMARK == curr ) {
        if ( dad->value.data.type == avpt_group ) {
          AVP_group i = 0;
          AVP_group ind = ((AVP_GROUP_Property*)&dad->value.data)->val;
          while( data && i < ind ) {
            i++;
            data = DATA_HANDLE( ((AVP_Data*)data)->next );
          }
        }
      }
      else {
        while( data && PROP_ID(&((AVP_Data*)data)->value.data) != curr ) 
          data = DATA_HANDLE(((AVP_Data*)data)->next);
      }
      if ( data )
        curr = *addr++ & PROP_ID_MASK;
      else
        return 0;
    }
  }
  return data;
}



