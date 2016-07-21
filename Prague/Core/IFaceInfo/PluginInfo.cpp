// PluginInfo.cpp: implementation of the CPluginInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ---
bool CPluginInfo::Selected() const {
  return BoolAttr( I(_SELECTED) );
}


// ---
tDWORD CPluginInfo::PluginID() const {
  return DWAttr( VE_PID_PL_DIGITALID );
}


// ---
tDWORD CPluginInfo::Version() const {
  return DWAttr( VE_PID_PL_VERSIONID );
}


// ---
tDWORD CPluginInfo::MnemonicID( char* val, tDWORD size ) const {
  tDWORD ret = StrAttr( VE_PID_PL_MNEMONICID, val, size );
  if ( ret )
    return ret;
  else if ( (0!=(ret=DWAttr(VE_PID_PL_DIGITALID))) ) {
    if ( val && size ) {
      if ( size >= 11 )
        return ::sprintf( val, "0x%08x", ret ) + 1;
      else
        return 0;
    }
    else
      return 11;
  }
  else
    return 0;
}


// ---
tDWORD CPluginInfo::VendorID() const {
  return DWAttr( VE_PID_PL_VENDORID );
}


// ---
tDWORD CPluginInfo::VendorMnemonicID( char* val, tDWORD size ) const {
  tDWORD ret = StrAttr( VE_PID_PL_VENDORMNEMONICID, val, size );
  if ( ret )
    return ret;
  else if ( (0!=(ret=DWAttr(VE_PID_PL_VENDORID))) ) {
    if ( val && size ) {
      if ( size >= 11 )
        return ::sprintf( val, "0x%08x", ret ) + 1;
      else
        return 0;
    }
    else
      return 11;
  }
  else
    return 0;
}


// ---
tDWORD CPluginInfo::VendorName( char* val, tDWORD size ) const {
  return StrAttr( VE_PID_PL_VENDORNAME, val, size );
}


// ---
tDWORD CPluginInfo::DefCodePage() const {
  return DWAttr( VE_PID_PL_CODEPAGEID );
}


// ---
tDWORD CPluginInfo::DefDateFormatID() const {
  return DWAttr( VE_PID_PL_DATEFORMATID );
}


// ---
tDWORD CPluginInfo::Author( char* val, tDWORD size ) const {
  return StrAttr( VE_PID_PL_AUTHORNAME, val, size );
}


// ---
tDWORD CPluginInfo::Project( char* val, tDWORD size ) const {
  return StrAttr( VE_PID_PL_PROJECTNAME, val, size );
}


// ---
tDWORD CPluginInfo::SubProject( char* val, tDWORD size ) const {
  return StrAttr( VE_PID_PL_SUBPROJECTNAME, val, size );
}


// ---
HDATA CPluginInfo::FirstIFace() const {
  if ( m_curr ) 
    return ::DATA_Get_FirstEx( m_curr, 0, DATA_IF_ROOT_CHILDREN );
  else
    return 0;
}


// ---
HDATA CPluginInfo::StaticIFace() const {
  if ( m_curr ) {
    HDATA iface = ::DATA_Get_FirstEx( m_curr, 0, DATA_IF_ROOT_CHILDREN );
    while( iface ) {
      HPROP hPropInerfaceName = ::DATA_Find_Prop( iface, NULL, VE_PID_IF_INTERFACENAME);
      HPROP hPropPluginStatic = ::DATA_Find_Prop( iface, NULL, VE_PID_IF_PLUGIN_STATIC);
      if ( !hPropInerfaceName && hPropPluginStatic )
        return iface;
      iface = ::DATA_Get_Next( iface, NULL );
    }
  }
  return 0;
}


