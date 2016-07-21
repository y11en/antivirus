// Face.cpp: implementation of the CFaceInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define OK(c) (0!=(c))

tDWORD s_nav[][CFaceInfo::n_last] = {
//n_list                    n_name                  n_type_name           n_type_id            n_value
  {I(_TYPES),               I(T_TYPENAME),          I(T_BASETYPENAME),    I(T_BASETYPE),       (tDWORD)-1,             },
  {I(_CONSTANTS),           I(C_CONSTANTNAME),      I(C_BASETYPENAME),    I(C_BASETYPE),       I(C_VALUE),             },
  {I(_ERRORCODES),          I(E_ERRORCODENAME),     (tDWORD)-1,           (tDWORD)-1,          I(E_ERRORCODEVALUE),    },
  {I(_PROPERTIES),          I(P_PROPNAME),          I(P_TYPENAME),        I(P_TYPE),           I(PSS_VARIABLEDEFVALUE),},
  {I(_METHODS),             I(M_METHODNAME),        I(M_RESULTTYPENAME),  I(M_RESULTTYPE),     I(M_RESULTDEFVALUE),    },
  {I(_SYSTEMMETHODS),       I(M_METHODNAME),        I(M_RESULTTYPENAME),  I(M_RESULTTYPE),     I(M_RESULTDEFVALUE),    },
  {(tDWORD)-1,              I(MP_PARAMETERNAME),    I(MP_TYPENAME),       I(MP_TYPE),          (tDWORD)-1,             },
  {I(_DATASTRUCTURES),      I(D_DATASTRUCTURENAME), I(DM_TYPENAME),       (tDWORD)-1,          (tDWORD)-1,             },
  {I(_DATASTRUCTUREMEMBERS),I(DM_MEMBERNAME),       I(DM_TYPENAME),       (tDWORD)-1,          (tDWORD)-1,             }, 
  {VE_PID_PL_MESSAGES,      I(M_MESSAGECLASSNAME),  (tDWORD)-1,           (tDWORD)-1,          (tDWORD)-1,             }, 
  {(tDWORD)-1,              I(M_MESSAGENAME),       (tDWORD)-1,           (tDWORD)-1,          (tDWORD)-1,             }, 
};



// ---
tDWORD CFaceInfo::NumberOf( tDWORD entity ) const {
  
  tDWORD num  = 0;
  HDATA data = EntityList( entity, 0 );

  while( data ) {
    num++;
    data = ::DATA_Get_Next( data, 0 );
  }
	return num;
}



// ---
tDWORD CFaceInfo::NameOf( tDWORD entity, tDWORD index, char* val, tDWORD size ) const {
  return EntityStr( entity, index, val, size, n_name );  
}


// ---
tDWORD CFaceInfo::TypeStrOf( tDWORD entity, tDWORD index, char* val, tDWORD size ) const {
  return EntityStr( entity, index, val, size, n_type_name );  
}


// ---
tDWORD CFaceInfo::TypeIDOf( tDWORD entity, tDWORD index ) const {

  tDWORD i, ind = 0;
  HDATA d = EntityList( entity, &ind );

  if ( !d )
    return 0;

  for( i=0; i<index && d; i++, d=::DATA_Get_Next(d,0) );
  if ( i < index )
    return 0;
  else {
    tDWORD v;
    HPROP p;
    i = s_nav[ind][n_type_id];
    if ( i == ((tDWORD)-1) ) 
      return tid_DWORD;
    else if ( OK(p=::DATA_Find_Prop(d,0,i)) && ::PROP_Get_Val(p,&v,sizeof(v)) )
	    return v;
    else
      return 0;
  }
}



// ---
tDWORD CFaceInfo::ValueOf( tDWORD entity, tDWORD index, char* val, tDWORD size, bool hex ) const {
	HDATA  d;
	tDWORD ind, i;

	switch( entity ) {
		case ifType :
		case ifMethodParam :
		case ifDataStructMember :
		case ifMsg :
			return 0;
		case ifErrCode :
		case ifMsgClass :
			d = EntityList( entity, (tDWORD*)&ind );
      for( i=0; i<index && d; i++,d=::DATA_Get_Next(d,0) );
			if ( (i == index) && (((tDWORD)-1) != (i=s_nav[ind][n_value])) )
				return CTreeInfo(d).DWAttrAsStr( i, val, size, hex );
			else
				return 0;
		default: 
			return EntityStr( entity, index, val, size, n_value );
	}
}



// ---
tDWORD CFaceInfo::IFaceID() const {
  return CBaseInfo::DWAttr( I(_DIGITALID) );
}


// ---
bool CFaceInfo::Selected() const {
  return BoolAttr( I(_SELECTED) );
}


// ---
bool CFaceInfo::Linked() const {
	return !!::DATA_Find_Prop( Data(), 0, I(_LINKEDBYINTERFACE) );
}


// ---
bool CFaceInfo::Static() const {
  return BoolAttr( I(_STATIC) );
}


// ---
bool CFaceInfo::System() const {
  return BoolAttr( I(_SYSTEM) );
}


// ---
bool CFaceInfo::TransferPropUp() const {
  return BoolAttr( I(_TRANSFERPROPUP) );
}


// ---
bool CFaceInfo::LastCallProp() const {
  return BoolAttr( I(_LASTCALLPROP) );
}


// ---
tDWORD CFaceInfo::LastCallPropRFunc( char* val, tDWORD size ) const {
  return StrAttr( I(LCP_READFUNC), val, size );
}


// ---
tDWORD CFaceInfo::LastCallPropWFunc( char* val, tDWORD size ) const {
  return StrAttr( I(LCP_WRITEFUNC), val, size );
}


// ---
bool CFaceInfo::UnSwapable() const {
  return BoolAttr( I(_NONSWAPABLE) );
}


// ---
tDWORD CFaceInfo::Synchronized() const {
  return DWAttr( I(_PROTECTED_BY) );
}


// ---
tDWORD CFaceInfo::IncludeFile( tDWORD i, char* val, tDWORD size ) const {
  HPROP p;
  if ( m_curr && (0!=(p=::DATA_Find_Prop(m_curr,0,I(_INCLUDENAMES)))) ) {
    tDWORD l;
    if ( (i < ::PROP_Arr_Count(p)) && (0!=(l=::PROP_Arr_Get_Items(p,i,0,0))) ) {
      if ( val && size )
        return ::PROP_Arr_Get_Items( p, i, val, size );
      else
        return l;
    }
  }
  return 0;
}


// ---
tDWORD CFaceInfo::PluginID() const {
  HDATA d;
  HPROP p;
  tDWORD v;
  if ( m_curr && OK(d=::DATA_Get_Dad(m_curr,0)) && OK(p=::DATA_Find_Prop(d,0,VE_PID_PL_DIGITALID)) && OK(::PROP_Get_Val(p,&v,sizeof(v))) )
	  return v;
  else
    return 0;
}


// ---
tDWORD CFaceInfo::VendorID() const {
  return DWAttr( I(_VENDORID) );
}


// ---
tDWORD CFaceInfo::SubType() const {
  return DWAttr( I(_SUBTYPEID) );
}


// ---
tDWORD CFaceInfo::SubTypeStr( char* val, tDWORD size ) const {
  tDWORD v;
  if ( OK(v=SubType()) ) {
    if ( val && size ) {
      if ( size >= 11 )
        return ::sprintf( val, "0x%08x", v ) + 1;
      else
        return 0;
    }
    else
      return 11;
  }
	return 0;
  //else
  //  return StrAttr( I(_SUBTYPESTRID), val, size );
}


// ---
tDWORD CFaceInfo::MnemonicID( char* val, tDWORD size ) const {
  return StrAttr( I(_MNEMONICID), val, size );
}


// ---
tDWORD CFaceInfo::Revision() const {
  return DWAttr( I(_SUBTYPEID) );
}


// ---
bool CFaceInfo::CreationDate( tDATETIME* pVal ) const {
 HPROP p;
 if ( pVal && m_curr && OK(p=::DATA_Find_Prop(m_curr,0,I(_CREATIONDATE))) && ::PROP_Get_Val(p,pVal,sizeof(tDATETIME)) )
   return true;
 else
   return false;
}


// ---
tDWORD CFaceInfo::Author( char* val, tDWORD size ) const {
  return StrAttr( I(_AUTORNAME), val, size );
}


// ---
tDWORD CFaceInfo::HeaderGUID( char* val, tDWORD size ) const {
  return StrAttr( I(_HEADERGUID), val, size );
}


// ---
tDWORD CFaceInfo::HeaderName( char* val, tDWORD size ) const {
  return StrAttr( I(_INCLUDEFILE), val, size );
}


// ---
tDWORD CFaceInfo::SourceFileName( char* val, tDWORD size ) const {
  return StrAttr( I(_OUTPUTFILE), val, size );
}


// ---
HDATA CFaceInfo::EntityList( tDWORD entity, tDWORD* index ) const {

  tDWORD local_ind;
  tDWORD* ind = index ? index : &local_ind;
  AVP_dword id[2] = { 0, 0 };
  switch( entity ) {
    case ifPlugin           : 
    case ifIFace            : *ind = 0; return 0;
    case ifType             : *ind = 0; break;
    case ifConstant         : *ind = 1; break;
    case ifErrCode          : *ind = 2; break;
    case ifProperty         : *ind = 3; break;
    case ifPubMethod        : *ind = 4; break;
    case ifIntMethod        : *ind = 5; break;
    case ifMethodParam      : *ind = 0; return 0; // *ind = 6; 
    case ifDataStruct       : *ind = 7; break;
		case ifMsgClass         : *ind = 9; break;
    case ifMsg              : *ind = 0; return 0;
    case ifDataStructMember : *ind = 8; 
    default                 : *ind = 0; return 0;
  }
  id[0] = s_nav[ *ind ][ n_list ];
  return m_curr ? ::DATA_Find( m_curr, id ) : 0;
}


// ---
tDWORD CFaceInfo::EntityStr( tDWORD entity, tDWORD index, char* val, tDWORD size, NavType type ) const {
  tDWORD i, ind;
  HDATA data = EntityList( entity, &ind );
  
  if ( !data )
    return 0;

  for( i=0; i<index && data; i++,data=::DATA_Get_Next(data,0) );

  if ( i == index ) {
    HPROP prop;
    tDWORD id = s_nav[ind][type];
    if ( (id != ((tDWORD)-1)) && (0 != (prop=::DATA_Find_Prop(data,0,id))) ) {
      tDWORD len = ::DATA_Get_Val( data, 0, id, 0, 0 );
      if ( len ) {
        if ( val && size ) {
          if ( size >= len )
            return ::DATA_Get_Val( data, 0, id, val, len );
        }
        else
          return len;
      }
    }
  }
  return 0;
}



// ---
HDATA CFaceInfo::FirstType() const {
  HDATA data = EntityList( ifType, 0 );
  if ( data ) 
    data = ::DATA_Get_FirstEx( data, 0, DATA_IF_ROOT_CHILDREN );
  return data;
}


// ---
HDATA CFaceInfo::FirstConstant() const {
  HDATA data = EntityList( ifConstant, 0 );
  if ( data ) 
    data = ::DATA_Get_FirstEx( data, 0, DATA_IF_ROOT_CHILDREN );
  return data;
}


// ---
HDATA CFaceInfo::FirstMsgClass() const {
  HDATA data = EntityList( ifMsgClass, 0 );
  if ( data ) 
    data = ::DATA_Get_FirstEx( data, 0, DATA_IF_ROOT_CHILDREN );
  return data;
}



// ---
HDATA CFaceInfo::FirstErrCode() const {
  HDATA data = EntityList( ifErrCode, 0 );
  if ( data ) 
    data = ::DATA_Get_FirstEx( data, 0, DATA_IF_ROOT_CHILDREN );
  return data;
}


// ---
HDATA CFaceInfo::FirstProperty() const {
  HDATA data = EntityList( ifProperty, 0 );
  if ( data ) 
    data = ::DATA_Get_FirstEx( data, 0, DATA_IF_ROOT_CHILDREN );
  return data;
}


// ---
HDATA CFaceInfo::FirstPubMethod() const {
  HDATA data = EntityList( ifPubMethod, 0 );
  if ( data ) 
    data = ::DATA_Get_FirstEx( data, 0, DATA_IF_ROOT_CHILDREN );
  return data;
}


// ---
HDATA CFaceInfo::FirstIntMethod() const {
  HDATA data = EntityList( ifIntMethod, 0 );
  if ( data ) 
    data = ::DATA_Get_FirstEx( data, 0, DATA_IF_ROOT_CHILDREN );
  return data;
}


// ---
HDATA CFaceInfo::FirstDataStruct() const {
  HDATA data = EntityList( ifDataStruct, 0 );
  if ( data ) 
    data = ::DATA_Get_FirstEx( data, 0, DATA_IF_ROOT_CHILDREN );
  return data;
}


// ---
bool CConstantInfo::IsNative() const { 
  if ( BoolAttr( VE_PID_NATIVE ) )
	  return true;
  return BoolAttr( I(C_NATIVE) ); 
}


