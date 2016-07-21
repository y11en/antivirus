// ErrInfo.cpp: implementation of the CErrInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ---
tDWORD CErrInfo::TypeID() const {
  if ( m_curr )
    return tid_DWORD;
  else
    return tid_VOID;
}


// ---
tDWORD CErrInfo::TypeName( const CSpecificTypeInfo*, char* val, tDWORD size ) const {
  if ( m_curr ) {
    if ( val && size ) {
      if ( size >= 7 )
        ::memcpy( val, "tDWORD", 7 );
      else
        return 0;
    }
    return 7;
  }
  else
    return 0;
}


// ---
tDWORD CErrInfo::Value() const {
  return DWAttr( I(E_ERRORCODEVALUE) );
}




// ---
bool CErrInfo::IsWarning() const {
  return BoolAttr( I(E_ISWARNING) );
}

// ---
bool CErrInfo::IsNative() const {
  if ( BoolAttr( VE_PID_NATIVE ) )
	  return true;
  return BoolAttr( I(E_NATIVE) );
}



