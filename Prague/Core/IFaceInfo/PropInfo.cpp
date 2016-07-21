// PropInfo.cpp: implementation of the CPropInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ---
tDWORD CPropInfo::DigitalID() const {
  return DWAttr( I(P_DIGITALID) );
}


// ---
tDWORD CPropInfo::TypifiedID( const CStdTypeInfo& ti ) const {
  return CPropTypeInfo(ti).Id( TypeID() );
}


// ---
CPropInfo::ScopeType CPropInfo::Scope() const {
  return (ScopeType)DWAttr( I(P_SCOPE) );
}


// ---
bool CPropInfo::Required() const {
  return BoolAttr( I(PLS_REQUIRED) );
}


// ---
tDWORD CPropInfo::MemberName( char* val, tDWORD size ) const {
  return StrAttr( I(PLS_MEMBER), val, size );
}


// ---
CPropInfo::Mode CPropInfo::FuncMode() const {
  return (CPropInfo::Mode)DWAttr( I(PLS_MODE) );
}


// ---
tDWORD CPropInfo::ReadFuncName( char* val, tDWORD size ) const {
  return StrAttr( I(PLS_READFUNC), val, size );
}


// ---
tDWORD CPropInfo::WriteFuncName( char* val, tDWORD size ) const {
  return StrAttr( I(PLS_WRITEFUNC), val, size );
}


// ---
tDWORD CPropInfo::VarName( char* val, tDWORD size ) const {
  return StrAttr( I(PSS_VARIABLENAME), val, size );
}


// ---
bool CPropInfo::Native() const {
  if ( BoolAttr( VE_PID_NATIVE ) )
	  return true;
  return BoolAttr( I(P_NATIVE) );
}


// ---
CPropInfo::Mode CPropInfo::MemberMode() const {
  return (CPropInfo::Mode)DWAttr( I(PLS_MEMBER_MODE) );
}


// ---
bool CPropInfo::Predefined() const {
  return BoolAttr( I(P_PREDEFINED) );
}


// ---
bool CPropInfo::HardPredefined() const {
  return BoolAttr( I(P_HARDPREDEFINED) );
}


// ---
bool CPropInfo::WritableOnInit() const {
  return BoolAttr( I(PLS_WRITABLEDURINGINIT) );
}


// ---
tDWORD CPropInfo::BinValue( const CSpecificTypeInfo& ti, char* val, tDWORD size, tDWORD* type ) const {
  
  tDWORD local_type;
  
  if ( !val || !size ) {
    val = 0;
    size = 0;
  }

  if ( !type ) {
    local_type = 0;
    type = &local_type;
  }

  *type = ti.Id( TypeID() );

  if ( *type ) {
    HPROP prop = ::DATA_Find_Prop( m_curr, 0, (*type = I(PSS_VARTYPIFIEDDEFVALUE)(*type)) );
    if ( prop ) 
      return ::PROP_Get_Val( prop, val, size );
  }

  return 0;
}


