#include "stdafx.h"


// ---
tDWORD CMsgClassInfo::Id() const { 
	return DWAttr( I(M_MESSAGECLASSID) ); 
}


// ---
bool CMsgClassInfo::IsNative() const { 
  if ( BoolAttr( VE_PID_NATIVE ) )
	  return true;
  return BoolAttr( I(MC_NATIVE) ); 
}


// ---
HDATA CMsgClassInfo::FirstMsg() const {
  if ( m_curr ) 
    return ::DATA_Get_FirstEx( m_curr, 0, DATA_IF_ROOT_CHILDREN );
  else
    return 0;
}



// ---
tDWORD CMsgInfo::Id() const { 
	return DWAttr( I(M_MESSAGEID) ); 
}


// ---
bool CMsgInfo::IsNative() const {
  if ( BoolAttr( VE_PID_NATIVE ) )
	  return true;
  return BoolAttr( I(M_NATIVE) ); 
}



// ---
tDWORD CMsgInfo::SendPointComment( char* val, tDWORD size ) {
	return StrAttr( I(M_MESSAGE_SEND_POINT), val, size );
}



// ---
tDWORD CMsgInfo::ContextComment( char* val, tDWORD size ) {
	return StrAttr( I(M_MESSAGECONTEXT), val, size );
}



// ---
tDWORD CMsgInfo::DataComment( char* val, tDWORD size ) {
	return StrAttr( I(M_MESSAGEDATA), val, size );
}



