#include "prop_util.h"



// ---
AVP_bool GetBoolPropVal( HDATA data, AVP_dword* addr, AVP_dword id ) {

  AVP_bool val;
  HPROP prop = DATA_Find_Prop( data, addr, id );

  if ( prop ) {
    
    id = PROP_Get_Id( prop );

    if ( 
      (GET_AVP_PID_TYPE(id) == avpt_bool)                    &&
      (GET_AVP_PID_ARR(id) == 0)                             &&
      (sizeof(val) == PROP_Get_Val(prop,&val,sizeof(val)))
    ) 
      return !!val;
    else
      return 0;
  }
  else
    return 0;
}




// ---
AVP_bool GetDwordPropVal( HDATA data, AVP_dword* addr, AVP_dword id, AVP_dword* val ) {

  HPROP prop = DATA_Find_Prop( data, addr, id );

  if ( prop ) {
    id = PROP_Get_Id( prop );
    return 
      (GET_AVP_PID_TYPE(id) == avpt_dword)                   &&
      (GET_AVP_PID_ARR(id) == 0)                             &&
      (sizeof(AVP_dword) == PROP_Get_Val(prop,val,sizeof(AVP_dword)));
  }
  else
    return 0;
}




// ---
AVP_bool GetStrPropVal( HDATA data, AVP_dword* addr, AVP_dword id, char* buff, AVP_dword len, AVP_dword* olen ) {

  HPROP prop = DATA_Find_Prop( data, addr, id );

  if ( prop ) {
    id = PROP_Get_Id( prop );
    if ( (GET_AVP_PID_TYPE(id) == avpt_str) && (GET_AVP_PID_ARR(id)==0) ) {
      len = PROP_Get_Val( prop, buff, len );
      if ( olen )
        *olen = len;
      return len != 0;
    }
  }

  if ( buff )
    *buff = 0;

  if ( olen )
    *olen = 0;

  return 0;

}



/*
// ---
AVP_bool GetPropVal( HDATA data, AVP_dword* addr, AVP_dword id, char* buff, AVP_dword len, AVP_dword* olen ) {

  if ( olen )
    *olen = 0;

  if ( data ) {
    HPROP prop = DATA_Find_Prop( data, addr, id );

    if ( prop ) {

      AVP_dword pid = PROP_Get_Id( prop );
  
      if ( GET_AVP_PID_ARR(pid) )
        return 0;

      switch( GET_AVP_PID_TYPE(pid) ) {

        case avpt_str   : 
          return GetStrPropVal( data, addr, id, buff, len, olen );

        case avpt_dword : 
      
          if ( olen )
            *olen = sizeof(AVP_dword);

          if ( buff && len < sizeof(AVP_dword) ) 
            return 0;
          else
            return GetDwordPropVal( data, addr, id, (AVP_dword*)buff );

        case avpt_bool :

          if ( olen )
            *olen = sizeof(AVP_bool);

          if ( buff && len < sizeof(AVP_bool) ) 
            return 0;
          else 
            return *(AVP_bool*)buff = GetBoolPropVal( data, addr, id );
      }
    }
  }

  return 0;
}


*/
