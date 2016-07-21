// StdTypeInfo.cpp: implementation of the CStdTypeInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <serialize/kldtser.h>
#include <string.h>

// ---
/*
struct SType {
  
  char* name;
  tDWORD pt;
  char* def;
  tDWORD tt;
  tDWORD size; 
};
*/

struct type_id {
  tDWORD root;
  tDWORD type;
  tDWORD def;
  tDWORD dt;
} g_ltable[CStdTypeInfo::last+1] = {
  { TT_PID_ROOT, TT_PID_TYPEID, TT_PID_TYPEDEFNAME, TT_PID_DTTYPEID },
  { PT_PID_ROOT, PT_PID_TYPEID, PT_PID_TYPEDEFNAME, PT_PID_DTTYPEID },
  { HP_PID_ROOT, HP_PID_TYPEID, HP_PID_TYPEDEFNAME, HP_PID_DTTYPEID },
  { SP_PID_ROOT, SP_PID_TYPEID, SP_PID_TYPEDEFNAME, SP_PID_DTTYPEID },
  { 0,           0,             0,                  0               },
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ---
bool CStdTypeInfo::IsOK() const {
  return 
    CTreeInfo::IsOK()                              &&
    CSpecificTypeInfo(TypeTree(object)).IsOK()     &&
    CSpecificTypeInfo(TypeTree(property)).IsOK()   &&
    CSpecificTypeInfo(TypeTree(obligatory)).IsOK() &&
    CSpecificTypeInfo(TypeTree(predefined)).IsOK();
}



// ---
HDATA CStdTypeInfo::TypeTree( tTypeID id ) const {
  if ( (object <= id) && (id < last) ) {
    AVP_dword a [2];
    a[0] = g_ltable[id].root;
    a[1] = 0;
    return ::DATA_Find( m_curr, a );
  }
  else
    return 0;
}



// ---
bool CSpecificTypeInfo::ReInit() {
  if ( m_curr ) {
    tDWORD id = ::DATA_Get_Id( m_curr, 0 );
    for( tDWORD i=0; i<sizeof(g_ltable)/sizeof(g_ltable[0]); i++ ) {
      if ( g_ltable[i].root == id ) {
        m_type = (CStdTypeInfo::tTypeID)i;
        return true;
      }
    }
  }
  return false;
}


// ---
bool CSpecificTypeInfo::IsOK() const { 
  
  if ( !CTreeInfo::IsOK() )
    return false;

  if ( !((CStdTypeInfo::object <= m_type) && (m_type < CStdTypeInfo::last)) )
    return false;

  HPROP names = ::DATA_Find_Prop( m_curr, 0, AVP_PID_VALUE );
  HPROP ids   = ::DATA_Find_Prop( m_curr, 0, g_ltable[m_type].type );
  HPROP defs  = ::DATA_Find_Prop( m_curr, 0, g_ltable[m_type].def );

  if ( 
    !names || !ids || !defs || 
    !GET_AVP_PID_APP(::PROP_Get_Id(names)) || 
    !GET_AVP_PID_APP(::PROP_Get_Id(ids)) ||
    !GET_AVP_PID_APP(::PROP_Get_Id(defs)) 
  )
    return false;

  tDWORD c = ::PROP_Arr_Count( names );
  if ( c != ::PROP_Arr_Count(ids) )
    return false;

  if ( c != ::PROP_Arr_Count(defs) )
    return false;

  return true;
}



// ---
tDWORD CSpecificTypeInfo::Id( tDWORD type ) const {

  HPROP type_ids   = ::DATA_Find_Prop( m_curr, 0, g_ltable[m_type].type );
  HPROP type_dt    = ::DATA_Find_Prop( m_curr, 0, g_ltable[m_type].dt );

  tDWORD c = ::PROP_Arr_Count( type_ids );

  for( tDWORD i=0; i<c; i++ ) {
    tDWORD id;
    if ( ::PROP_Arr_Get_Items(type_ids,i,&id,sizeof(id)) ) {
      if ( id == type ) {
        if ( sizeof(tDWORD) == ::PROP_Arr_Get_Items(type_dt,i,&id,sizeof(id)) )
          return id;
        else
          return 0;
      }
    }
  }

  return 0;
}



// ---
tDWORD CSpecificTypeInfo::Id( const char* type ) const {

  char  tmp[260];
  HPROP type_names = ::DATA_Find_Prop( m_curr, 0, AVP_PID_VALUE );
  HPROP type_ids   = ::DATA_Find_Prop( m_curr, 0, g_ltable[m_type].type );

  tDWORD c = ::PROP_Arr_Count( type_ids );

  for( tDWORD i=0; i<c; i++ ) {
    if ( ::PROP_Arr_Get_Items(type_names,i,tmp,sizeof(tmp)) ) {
      if ( !::strcmp(tmp,type) ) {
        tDWORD id = 0;
        if ( sizeof(tDWORD) == ::PROP_Arr_Get_Items(type_ids,i,&id,sizeof(tDWORD)) )
          return id;
        else
          return 0;
      }
    }
  }

  return 0;
}



// ---
tDWORD CSpecificTypeInfo::Name( tDWORD type, char* val, tDWORD size ) const {

  if ( type <= tid_LAST_TYPE ) {
    HPROP type_names = ::DATA_Find_Prop( m_curr, 0, AVP_PID_VALUE );
    HPROP type_ids   = ::DATA_Find_Prop( m_curr, 0, g_ltable[m_type].type );

    if ( type_names && type_ids ) {
      tDWORD c = ::PROP_Arr_Count( type_ids );

      if ( val ) 
        val[0] = 0;

      for( tDWORD i=0; i<c; i++ ) {
        tDWORD id;
        ::PROP_Arr_Get_Items( type_ids, i, &id, sizeof(id) );
        if ( type == id ) {
          if ( val )
            return ::PROP_Arr_Get_Items( type_names, i, val, size );
          else
            return ::PROP_Arr_Get_Items( type_names, i, 0, 0 );
        }
      }
    }
  }
  return 0;
}



// ---
tDWORD CSpecificTypeInfo::MnemonicID( tDWORD type, char* val, tDWORD size ) const {

  HPROP type_names = ::DATA_Find_Prop( m_curr, 0, g_ltable[m_type].def );
  HPROP type_ids   = ::DATA_Find_Prop( m_curr, 0, g_ltable[m_type].type );

  if ( type_names && type_ids ) {
    tDWORD c = ::PROP_Arr_Count( type_ids );

    if ( val ) 
      val[0] = 0;

    for( tDWORD i=0; i<c; i++ ) {
      tDWORD id;
      ::PROP_Arr_Get_Items( type_ids, i, &id, sizeof(id) );
      if ( type == id ) {
        if ( val )
          return ::PROP_Arr_Get_Items( type_names, i, val, size );
        else
          return ::PROP_Arr_Get_Items( type_names, i, 0, 0 );
      }
    }
  }

  return 0;
}



// ---
HDATA CSpecificTypeInfo::operator = ( HDATA tree ) {
  Init( tree );
  m_type = CStdTypeInfo::last;
  if ( m_curr ) {
    tDWORD id = ::DATA_Get_Id( m_curr, 0 );
    for( tDWORD i=0; i<sizeof(g_ltable)/sizeof(g_ltable[0]); i++ ) {
      if ( g_ltable[i].root == id ) {
        m_type = (CStdTypeInfo::tTypeID)i;
        break;
      }
    }
  }
  return m_curr;
}



