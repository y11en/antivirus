#include "prop_in.h"

// ---
typedef struct {
  First_That_Proc proc;
  void*           data;
} Find_Data;

// ---
static AVP_dword find_condition( HDATA data, void* user ) {
  if ( !((Find_Data*)user)->proc || ((Find_Data*)user)->proc(data,((Find_Data*)user)->data) )
    return (AVP_dword)data;
  else
    return 0;
}

// ---
DATA_PROC HDATA DATA_PARAM DATA_First_That( HDATA node, AVP_dword* addr, AVP_dword flags, First_That_Proc proc, void* user_data ) {
  Find_Data data;
  data.proc = proc;
  data.data = user_data;
  return (HDATA)DATA_For_Each( node, addr, flags, (For_Each_Proc)find_condition, &data );
}



