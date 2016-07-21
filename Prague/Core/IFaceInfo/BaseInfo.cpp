#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OB '{'
#define CB '}'

#if !defined(E_INVALIDARG)
  #define E_INVALIDARG 0x80070057L
#endif

#if !defined(S_OK)
  #define S_OK 0L
#endif

// ---
tDWORD mIIDFromString ( const char* lpsz, tGUID& guid ) {
  // {4B3A638D-B2DF-4fca-8B9E-9A6A85982FF3}
  char* ptr;
  
  guid.Data1 = 0;
  guid.Data2 = 0;
  guid.Data3 = 0;
  ::memset( guid.Data4, sizeof(guid.Data4), 0 );
  if ( *lpsz == OB )
    lpsz++;
  guid.Data1 = strtoul( lpsz, &ptr, 16 );
  if ( !ptr )
    return E_INVALIDARG;
  if ( *ptr == '-' )
    ptr++;
  guid.Data2 = (tWORD)strtoul( ptr, &ptr, 16 );
  if ( !ptr )
    return E_INVALIDARG;
  if ( *ptr == '-' )
    ptr++;
  guid.Data3 = (tWORD)strtoul( ptr, &ptr, 16 );
  if ( !ptr )
    return E_INVALIDARG;
  if ( *ptr == '-' )
    ptr++;

  unsigned int i;
  typedef tBYTE bytearray[8];
  bytearray& arr = guid.Data4;

  for( i=0; i<sizeof(arr); i++ ) {
    char strbyte[3];
    char* p;
    strbyte[0] = *(ptr+0);
    strbyte[1] = *(ptr+1);
    strbyte[2] = 0;
    arr[i] = (tBYTE)strtoul( strbyte, &p, 16 );
    if ( !ptr )
      return E_INVALIDARG;
    ptr += 2;
    if ( *ptr == '-' )
      ptr++;
  }
  return S_OK;
}



/*
switch( m_en_type ) {
case ifPlugin           :  ; break;
case ifIFace            :  ; break;
case ifType             :  ; break;
case ifConstant         :  ; break;
case ifErrCode          :  ; break;
case ifProperty         :  ; break;
case ifPubMetod         :  ; break;
case ifIntMethod        :  ; break;
case ifMethodParam      :  ; break;
case ifDataStruct       :  ; break;
case ifDataStructMember :  ; break;
}
*/



// ---
char* CBaseInfo::StrVal( tDWORD (CBaseInfo::*func)(char*,tDWORD) const, char* buff, int len ) const {
  if ( len ) {
    if ( (this->*func)(buff,len) )
      return buff;
    else
      return 0;
  }
  else if ( 0 == (len=(this->*func)(0,0)) ) 
    return 0;
  else {
    char* ret;
    (this->*func)( ret=new char[len], len );
    return ret;
  }
}




// ---
char* CBaseInfo::StrVal( tDWORD (CBaseInfo::*func)(const CSpecificTypeInfo* ti, char* val, tDWORD size ) const, const CSpecificTypeInfo* ti, char* buff, int len ) const {
  if ( len ) {
    if ( (this->*func)(ti,buff,len) )
      return buff;
    else
      return 0;
  }
  else if ( 0 == (len=(this->*func)(ti,0,0)) ) 
    return 0;
  else {
    char* ret;
    (this->*func)( ti, ret=new char[len], len );
    return ret;
  }
}




// ---
bool CBaseInfo::ReInit() {
  if ( m_curr ) {
    tDWORD en = DetectEnType( m_curr );
    if ( m_en_type == ifAny )
      m_en_type = en;
    else if ( m_en_type != en ) {
      if ( en == ifAny )
        SetDWAttr( VE_PID_NODETYPE, en );
      else if ( (en == ifIFace) && (m_en_type != ifPlugin) && (m_en_type != ifMethodParam) && (m_en_type != ifDataStructMember) ) {
        CFaceInfo ii(m_curr,false);
        switch( m_en_type ) {
          case ifType       : StartHere(ii.FirstType());       break;
          case ifConstant   : StartHere(ii.FirstConstant());   break;
          case ifErrCode    : StartHere(ii.FirstErrCode());    break;
          case ifProperty   : StartHere(ii.FirstProperty());   break;
          case ifPubMethod  : StartHere(ii.FirstPubMethod());  break;
          case ifIntMethod  : StartHere(ii.FirstIntMethod());  break;
          case ifDataStruct : StartHere(ii.FirstDataStruct()); break;
					case ifMsgClass   :	StartHere(ii.FirstMsgClass());   break;
          default           : StartHere(0); m_en_type = ifAny; return false;
        }
      }
      else if ( (en==ifPubMethod) && (m_en_type==ifMethodParam) ) {
        CPubMethodInfo mi( m_curr, false );
        StartHere( mi.FirstParam() );
      }
      else if ( (en==ifIntMethod) && (m_en_type==ifMethodParam) ) {
        CIntMethodInfo mi( m_curr, false );
        StartHere( mi.FirstParam() );
      }
      else if ( (en==ifDataStruct) && (m_en_type==ifDataStructMember) ) {
        CDataStructInfo dsi( m_curr, false );
        StartHere( dsi.FirstMember() );
      }
      else {
        m_en_type = ifAny;
        StartHere( 0 );
      }
    }
    tDWORD len = Name(0,0);
    if ( len ) {
      m_name = new char[len];
      Name( m_name, len );
    }
    else
      m_name = 0;
  }
  else
    m_name = 0;
  return true;
}



// ---
tDWORD CBaseInfo::DetectEnType( HDATA tree ) {
  
  if ( tree ) {
    HPROP type_prop = ::DATA_Find_Prop( tree, 0, VE_PID_NODETYPE );
    if ( type_prop ) {
      AVP_dword type;
      ::PROP_Get_Val( type_prop, &type, sizeof(type) );
      switch( type ) {
        case VE_NT_PLUGIN               : return ifPlugin;      // (0 ) Interface
        case VE_NT_INTERFACE            : return ifIFace;       // (1 ) Interface
        case VE_NT_PROPERTY             : return ifProperty;    // (2 ) Interface Property
        case VE_NT_METHOD               : return ifPubMethod;   // (3 ) Interface Method
        case VE_NT_METHODPARAM          : return ifMethodParam; // (4 ) Interface Method Paremeter
        case VE_NT_DATASTRUCTURE        : return ifDataStruct;  // (5 ) Interface Data Structure
        case VE_NT_DATASTRUCTUREMEMBER  : return ifDataStructMember; // (6 ) Interface Data Structure Member
        case VE_NT_TYPE                 : return ifType;        // (7 ) Interface Type
        case VE_NT_CONSTANT             : return ifConstant;    // (8 ) Interface Constant
        case VE_NT_SYSTEMMETHOD         : return ifIntMethod;   // (9 ) Interface System Method
        case VE_NT_SYSTEMMETHODPARAM    : return ifMethodParam; // (10) Interface System Method  Paremeter
        case VE_NT_ERRORCODE            : return ifErrCode;     // (11) Interface error Code
        case VE_NT_MESSAGECLASS         : return ifMsgClass;    // (12) Interface message class
        case VE_NT_MESSAGE              : return ifMsg;         // (13) Interface message
				default                         : return type;          // i don't known what's the type !
      }
    }
    else if ( ::DATA_Find_Prop(tree,0,VE_PID_PL_PLUGINNAME) )
      return ifPlugin;
    else if ( ::DATA_Find_Prop(tree,0,I(_INTERFACENAME)) )
      return ifIFace;
    else if ( ::DATA_Find_Prop(tree,0,I(T_TYPENAME)) )
      return ifType;
    else if ( ::DATA_Find_Prop(tree,0,I(C_CONSTANTNAME)) )
      return ifConstant;
    else if ( ::DATA_Find_Prop(tree,0,I(E_ERRORCODENAME)) )
      return ifErrCode;
    else if ( ::DATA_Find_Prop(tree,0,I(P_PROPNAME)) )
      return ifProperty;
    else if ( ::DATA_Find_Prop(tree,0,I(M_MESSAGECLASSNAME)) )
      return ifMsgClass;
    else if ( ::DATA_Find_Prop(tree,0,I(M_MESSAGENAME)) )
      return ifMsg;
    else if ( ::DATA_Find_Prop(tree,0,I(M_METHODNAME)) ) {
      HDATA dad = ::DATA_Get_Dad( tree, 0 );
      if ( dad ) {
        tDWORD id = ::DATA_Get_Id( dad, 0 );
        if ( id == I(_SYSTEMMETHODS) )
          return ifIntMethod;
        else if ( id == I(_METHODS) )
          return ifPubMethod;
      }
    }
    else if ( ::DATA_Find_Prop(tree,0,I(MP_PARAMETERNAME)) )
      return ifMethodParam;
    else if ( ::DATA_Find_Prop(tree,0,I(D_DATASTRUCTURENAME)) )
      return ifDataStruct;
    else if ( ::DATA_Find_Prop(tree,0,I(DM_MEMBERNAME)) )
      return ifDataStructMember;
  }
  return ifAny;
}



// --
bool CBaseInfo::IsSeparator() const {
  return m_curr ? (!!::DATA_Find_Prop(m_curr,0,VE_PID_SEPARATOR)) : false;
}



// ---
bool CBaseInfo::SetSeparator( bool set ) {
  if ( !m_curr )
    return false;
  return !!::DATA_Set_Val( m_curr, 0, VE_PID_SEPARATOR, (AVP_bool)1, sizeof(AVP_bool) );
}



// ---
const char* CBaseInfo::Name() const {
  return m_name;
}



// ---
tDWORD CBaseInfo::Name( char* val, tDWORD size ) const {
  tDWORD id;
  if ( !m_curr )
    return 0;
  switch( m_en_type ) {
    case ifPlugin           : id = VE_PID_PL_PLUGINNAME; break; 
    case ifIFace            : id = I(_INTERFACENAME); break;
    case ifType             : id = I(T_TYPENAME); break;
    case ifConstant         : id = I(C_CONSTANTNAME); break;
    case ifErrCode          : id = I(E_ERRORCODENAME); break;
    case ifProperty         : id = I(P_PROPNAME); break;
    case ifPubMethod        : id = I(M_METHODNAME); break;
    case ifIntMethod        : id = I(M_METHODNAME); break;
    case ifMethodParam      : id = I(MP_PARAMETERNAME); break;
    case ifDataStruct       : id = I(D_DATASTRUCTURENAME); break;
    case ifDataStructMember : id = I(DM_MEMBERNAME); break;
		case ifMsgClass         : id = I(M_MESSAGECLASSNAME); break;
		case ifMsg              : id = I(M_MESSAGENAME); break;
    default                 : return 0;
  }
  return StrAttr( id, val, size );
}



// ---
bool CBaseInfo::SetName( const char* val ) {
  tDWORD id;
  if ( !m_curr )
    return 0;
  switch( m_en_type ) {
    case ifPlugin           : id = VE_PID_PL_PLUGINNAME; break; 
    case ifIFace            : id = I(_INTERFACENAME); break;
    case ifType             : id = I(T_TYPENAME); break;
    case ifConstant         : id = I(C_CONSTANTNAME); break;
    case ifErrCode          : id = I(E_ERRORCODENAME); break;
    case ifProperty         : id = I(P_PROPNAME); break;
    case ifPubMethod        : id = I(M_METHODNAME); break;
    case ifIntMethod        : id = I(M_METHODNAME); break;
    case ifMethodParam      : id = I(MP_PARAMETERNAME); break;
    case ifDataStruct       : id = I(D_DATASTRUCTURENAME); break;
    case ifDataStructMember : id = I(DM_MEMBERNAME); break;
		case ifMsgClass         : id = I(M_MESSAGECLASSNAME); break;
    case ifMsg              : id = I(M_MESSAGENAME); break;
    default                 : return 0;
  }
  int len;
  if ( val && *val )
    len = (int)(strlen(val) + 1);
  else
    len = 0;
  if ( len )
    m_name = (char*)memcpy( new char[len], val, len );
  else
    m_name = 0;
  return SetStrAttr( id, val );
}



// ---
tDWORD CBaseInfo::TypeID() const {
  tDWORD id;
  
  if ( !m_curr )
    return 0;
  
  switch( m_en_type ) {
  case ifPlugin           : 
  case ifIFace            : return 0;
  case ifType             : id = I(T_BASETYPE); break;
  case ifConstant         : id = I(C_BASETYPE); break;
    
  case ifErrCode          :
  case ifMsgClass         :
  case ifMsg              : return tid_DWORD;
    
  case ifProperty         : id = I(P_TYPE); break;
  case ifPubMethod        : id = I(M_RESULTTYPE); break;
  case ifIntMethod        : id = I(M_RESULTTYPE); break;
  case ifMethodParam      : id = I(MP_TYPE); break;
  case ifDataStruct       : 
  case ifDataStructMember : 
  default                 : return 0;
  }
  
  return DWAttr( id );
}



// ---
bool CBaseInfo::SetTypeID( tDWORD type ) {
  tDWORD id;
  
  if ( !m_curr )
    return 0;
  
  switch( m_en_type ) {
    case ifPlugin           : 
    case ifIFace            : return false;
    case ifType             : id = I(T_BASETYPE); break;
    case ifConstant         : id = I(C_BASETYPE); break;
    
    case ifErrCode          :
    case ifMsgClass         :
    case ifMsg              : return false;
    
    case ifProperty         : id = I(P_TYPE); break;
    case ifPubMethod        : id = I(M_RESULTTYPE); break;
    case ifIntMethod        : id = I(M_RESULTTYPE); break;
    case ifMethodParam      : id = I(MP_TYPE); break;
    case ifDataStruct       : 
    case ifDataStructMember : 
    default                 : return false;
  }
  
  return SetDWAttr( id, type );
}




// ---
tDWORD CBaseInfo::TypeOfType() const {
  return m_curr ? DWAttr( VE_PID_NODETYPETYPE, 0 ) : 0;
}



// ---
bool CBaseInfo::SetTypeOfType( tDWORD type_of_type ) {
  if ( !m_curr )
    return false;
  
  switch( m_en_type ) {
    case ifPlugin           : 
    case ifIFace            :
    
    case ifErrCode          :
    case ifMsgClass         :
    case ifMsg              :
    
    case ifDataStruct       : 
    case ifDataStructMember :

    default                 : return false;

    case ifAny              :
    case ifType             :
    case ifConstant         :
    case ifProperty         :
    case ifPubMethod        :
    case ifIntMethod        :
    case ifMethodParam      : break;
  }
  return SetDWAttr( VE_PID_NODETYPETYPE, type_of_type );
}



// ---
tDWORD CBaseInfo::TypeName( const CSpecificTypeInfo* ti, char* val, tDWORD size ) const {
  
  if ( !m_curr )
    return 0;

  tDWORD type_id      = TypeID();
  tDWORD type_of_type = TypeOfType();

  if ( (type_of_type == VE_NTT_GLOBALTYPE) && (type_id != tid_VOID) && ti && *ti ) {
    tDWORD ret = ti->Name( type_id, val, size );
    if ( ret ) 
      return ret;
  }

  tDWORD id;
  switch( m_en_type ) {
    case ifPlugin           : return 0;
    case ifIFace            : id = I(_OBJECTTYPE); break;
    case ifType             : id = I(T_BASETYPENAME); break;
    case ifConstant         : id = I(C_BASETYPENAME); break;
    case ifErrCode          :
		case ifMsgClass         :
		case ifMsg              :
      if ( val && size ) { 
        if ( size < 7 ) {
          *val = 0;
          return 0;
        }
        else
          ::memcpy( val, "tDWORD", 7 );
      }
      return 7;
    case ifProperty         : id = I(P_TYPENAME); break;
    case ifPubMethod        : id = I(M_RESULTTYPENAME); break;
    case ifIntMethod        : id = I(M_RESULTTYPENAME); break;
    case ifMethodParam      : id = I(MP_TYPENAME); break;
    case ifDataStruct       : id = I(D_DATASTRUCTURENAME); break;
    case ifDataStructMember : id = I(DM_TYPENAME); break;
    default                 : return 0;
  }
  return StrAttr( id, val, size );
}



// ---
bool CBaseInfo::SetTypeName( const char* val ) {
  if ( !m_curr )
    return false;
  
  tDWORD id;
  switch( m_en_type ) {
    case ifPlugin           : return false;
    case ifIFace            : id = I(_OBJECTTYPE); break;
    case ifType             : id = I(T_BASETYPENAME); break;
    case ifConstant         : id = I(C_BASETYPENAME); break;
    case ifErrCode          :
		case ifMsgClass         :
    case ifMsg              : return false;
    case ifProperty         : id = I(P_TYPENAME); break;
    case ifPubMethod        : id = I(M_RESULTTYPENAME); break;
    case ifIntMethod        : id = I(M_RESULTTYPENAME); break;
    case ifMethodParam      : id = I(MP_TYPENAME); break;
    case ifDataStruct       : id = I(D_DATASTRUCTURENAME); break;
    case ifDataStructMember : id = I(DM_TYPENAME); break;
    default                 : return 0;
  }
  return SetStrAttr( id, val );
}




// ---
tDWORD CBaseInfo::Value( char* val, tDWORD size ) const {
  tDWORD id;
  
  if ( !m_curr )
    return 0;
  
  switch( m_en_type ) {
    case ifPlugin           : 
    case ifIFace            : 
    case ifType             : return 0;
    case ifConstant         : id = I(C_VALUE); break;
    case ifErrCode          : return DWAttrAsStr( I(E_ERRORCODEVALUE), val, size, false );
    case ifProperty         : id = I(PSS_VARIABLEDEFVALUE); break;
    case ifPubMethod        : 
    case ifIntMethod        : id = I(M_RESULTDEFVALUE);     break;
		case ifMsgClass         : return DWAttrAsStr( I(M_MESSAGECLASSID), val, size, false ); break;
		case ifMsg              : return DWAttrAsStr( I(M_MESSAGEID),      val, size, false ); break;
    case ifMethodParam      : 
    case ifDataStruct       : 
    case ifDataStructMember : 
    default                 : return 0;
  }
  return StrAttr( id, val, size );
}



// ---
bool CBaseInfo::SetStrValue( const char* val ) {
  tDWORD id;
  
  if ( !m_curr )
    return false;
  
  switch( m_en_type ) {
    case ifPlugin           : 
    case ifIFace            : 
    case ifType             :
    case ifErrCode          :
    case ifMsgClass         :
    case ifMsg              :
    case ifMethodParam      :
    case ifDataStruct       :
    case ifDataStructMember :
    default                 : return false;

    case ifConstant         : id = I(C_VALUE); break;
    case ifProperty         : id = I(PSS_VARIABLEDEFVALUE); break;
    case ifPubMethod        :
    case ifIntMethod        : id = I(M_RESULTDEFVALUE);     break;
  }
  return SetStrAttr( id, val );
}



// ---
bool CBaseInfo::SetDWValue( tDWORD val, bool hex ) {
  tDWORD id;
  char  sval[20];
  char* pval = 0;
  const char* fmt;
  tDWORD size = sizeof(tDWORD);
  
  if ( !m_curr )
    return false;
  fmt = hex ? "0x%08x" : "%u";

  switch( m_en_type ) {
    case ifPlugin           : 
    case ifIFace            : 
    case ifType             : return false;

    case ifErrCode          : id = I(E_ERRORCODEVALUE); pval = (char*)val; size = sizeof(tDWORD); break;
    case ifMsgClass         : id = I(M_MESSAGECLASSID); pval = (char*)val; size = sizeof(tDWORD); break;
    case ifMsg              : id = I(M_MESSAGEID);      pval = (char*)val; size = sizeof(tDWORD); break;
      
    case ifConstant         : id = I(C_VALUE);              size = 1 + sprintf( pval=sval, fmt, val ); break;
    case ifProperty         : id = I(PSS_VARIABLEDEFVALUE); size = 1 + sprintf( pval=sval, fmt, val ); break;
    case ifPubMethod        : 
    case ifIntMethod        : id = I(M_RESULTDEFVALUE);     size = 1 + sprintf( pval=sval, fmt, val ); break;

    case ifMethodParam      : 
    case ifDataStruct       : 
    case ifDataStructMember : 
    default                 : return false;
  }
  return SetAttr( id, pval, size );
}



// ---
tDWORD CBaseInfo::ShortComment ( char* val, tDWORD size ) const {
  return m_curr ? StrAttr( VE_PID_SHORTCOMMENT, val, size ) : 0;
}



// ---
bool CBaseInfo::SetShortComment( const char* val ) {
  return m_curr ? SetStrAttr( VE_PID_SHORTCOMMENT, val ) : false;
}


// ---
tDWORD CBaseInfo::ExtComment( char* val, tDWORD size ) const {
  return m_curr ? StrAttr( VE_PID_LARGECOMMENT, val, size ) : 0;
}


// ---
bool CBaseInfo::SetExtComment( const char* val ) {
  return m_curr ? SetStrAttr( VE_PID_LARGECOMMENT, val ) : false;
}


// ---
tDWORD CBaseInfo::ValueComment( char* val, tDWORD size ) const {
  return m_curr ? StrAttr( VE_PID_VALUECOMMENT, val, size ) : 0;
}



// ---
bool CBaseInfo::SetValueComment( const char* val ) {
  return m_curr ? SetStrAttr( VE_PID_VALUECOMMENT, val ) : false;
}



// ---
tDWORD CBaseInfo::BehaviorComment( char* val, tDWORD size ) const {
  return m_curr ? StrAttr( VE_PID_BEHAVIORCOMMENT, val, size ) : 0;
}



// ---
bool CBaseInfo::SetBehaviorComment( const char* val ) {
  return m_curr ? SetStrAttr( VE_PID_BEHAVIORCOMMENT, val ) : false;
}



// ---
bool CBaseInfo::Guid( tGUID& val ) const {
  tCHAR  str_guid[40];
  tDWORD len = StrAttr( VE_PID_UNIQUEID, str_guid, sizeof(str_guid) );

  if ( !len )
    return false;

#if 0 && defined (_WIN32)
  {
    tWCHAR wstr_guid[40];
    ::mbstowcs( wstr_guid, str_guid, len );
    ::CoInitialize( 0 );
    ::IIDFromString( wstr_guid, &val );
    ::CoUninitialize();
  }
#else
  ::mIIDFromString( str_guid, val );
#endif
  return true;
}



// ---
tDWORD CBaseInfo::Guid( char* val, tDWORD size ) const {
  return StrAttr( VE_PID_UNIQUEID, val, size );
}


// ---
bool CTypeInfo::IsNative() const {
  if ( BoolAttr( VE_PID_NATIVE ) )
	  return true;
  return BoolAttr( I(T_NATIVE) );
}



// ---
tDWORD CTypeInfo::Value( char* v, tDWORD s, bool hex ) const { 
  if (v&&s) 
     ::memset(v,0,s); 
  return 0; 
}
