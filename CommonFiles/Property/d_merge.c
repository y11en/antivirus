#ifdef __MEGRE_FUNC__

#include "prop_in.h"

// ---
DATA_PROC AVP_bool DATA_PARAM DATA_Merge( 
  HDATA             data, 
  AVP_dword*        addr, 
  HDATA             data2, 
  AVP_dword*        addr2, 
  HDATA*            result, 
  AVP_dword         flags, 
  AVP_Merge_Param*  param
) {

  if ( !data || !data2 || (result && *result == data) ) {
    _RPT0( _CRT_ASSERT, "Bad parameters" );
    return 0;
  }

  FIND_DATA( data,  addr  );
  FIND_DATA( data2, addr2 );

  if ( !result )
    result = &data;

  _RPT0( _CRT_ASSERT, "Merge is not working at the moment !" );
  return 0;
}


#endif

