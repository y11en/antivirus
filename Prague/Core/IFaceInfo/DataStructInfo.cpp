// DataStructInfo.cpp: implementation of the CDataStructInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IFaceInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ---
// ---
HDATA CDataStructInfo::FirstMember() const {
  if ( m_curr ) {
    AVP_dword id[2] = { I(_DATASTRUCTUREMEMBERS), 0 };
    return ::DATA_Get_FirstEx( m_curr, id, DATA_IF_ROOT_CHILDREN );
  }
  else
    return 0;
}



// ---
HDATA CDataStructInfo::FirstInnerStruct() const {
  if ( m_curr ) {
    AVP_dword id[2] = { I(_DATASTRUCTURES), 0 };
    return ::DATA_Get_FirstEx( m_curr, id, DATA_IF_ROOT_CHILDREN );
  }
  else
    return 0;
}



// ---
bool CDataMemberInfo::IsArray() const {
  return BoolAttr( I(DM_ISARRAY ));
}


// ---
tDWORD CDataMemberInfo::ArrayBound( char* val, tDWORD size ) const {
  return StrAttr( I(DM_ARRAYBOUND), val, size );
}


