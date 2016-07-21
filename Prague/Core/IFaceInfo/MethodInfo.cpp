// MethodInfo.cpp: implementation of the CMethodInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ---
bool CMethodInfo::IsPublic() const {
  return m_en_type == ifPubMethod;
}



// ---
bool CMethodInfo::IsInternal() const {
  return m_en_type == ifIntMethod;
}

// ---
bool CMethodInfo::IsExtended() const { 
  return BoolAttr( I(M_EXTENDED) ); 
}


// ---
HDATA CMethodInfo::FirstParam() const {
  if ( m_curr ) 
    return ::DATA_Get_FirstEx( m_curr, 0, DATA_IF_ROOT_CHILDREN );
  else
    return 0;
}



// ---
bool CMethodInfo::Selected() const {
  if ( m_en_type == ifPubMethod )
    return true;
  else
    return BoolAttr( I(M_SELECTED));
}



// ---
tDWORD CMethodInfo::MethodID() const {
  return DWAttr( I(M_METHOD_ID) );
}


// ---
bool CMethodInfo::SetMethodID( tDWORD id ) {
  return SetDWAttr( I(M_METHOD_ID), id );
}


// ---
bool CMethodParamInfo::IsConst() const {
  return BoolAttr( I(MP_ISCONST) );
}


// ---
bool CMethodParamInfo::IsPointer() const {
  return BoolAttr( I(MP_ISPOINTER) );
}


// ---
bool CMethodParamInfo::IsDoublePointer() const {
  return BoolAttr( I(MP_IS_DOUBLE_POINTER) );
}


