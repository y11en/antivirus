////////////////////////////////////////////////////////////////////
//
//  PragueServerSaveDB.cpp
//  Save interfaces DB stuff implementation
//  Prague Interfaces Server
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include "Resource.h"
#include <serialize/kldtser.h>
#include <datatreeutils/dtutils.h>
#include <_avpio.h>
#include <property/property.h>
#include <AVPComID.h>
#include <Stuff\FFind.h>
#include <Stuff\SArray.h>
#include <Stuff\PathStr.h>
#include <StuffIO\IOUtil.h>

#include "..\AVPPveID.h"

#include "..\PGIServerInterface.h"
#include "..\PISrvError.h"
#include "PragueIServer.h"
#include "PrepareDBTree.h"

#include "Prague/prague.h"
#include "../../Core/metadata/metautils.h"
#include "../../Core/IFaceInfo/ifaceinfo.h"
#include "../../Core/loader/prop_util.h"
#include "../../Core/loader/prop_util.c"
tBOOL IsStaticIface( HDATA iface ) 
{
	if ( !::DATA_Find_Prop(iface,0,VE_PID_IF_PLUGIN_STATIC) && ::DATA_Find_Prop(iface,0,VE_PID_IF_DIGITALID) && !IsLinkedByInterface(iface) )
		return cFALSE;
	return cTRUE;
}

HDATA GetStaticIface(HDATA info) 
{
  if ( !info )
    return 0;
  
  info = DATA_Get_FirstEx( info, 0, DATA_IF_ROOT_CHILDREN );
  while( info ) {
    if ( IsStaticIface(info) )
      return info;
    info = DATA_Get_Next( info, 0 );
  }
  return 0;
}

HDATA GetIface( HDATA info, int ind ) 
{
  int i = 0;
  if ( !info )
    return 0;
  
  info = ::DATA_Get_FirstEx( info, 0, DATA_IF_ROOT_CHILDREN );
  while( info ) 
  {
    if ( IsStaticIface(info) )
			;
    else if ( i == ind )
      return info;
    else
      i++;
    info = ::DATA_Get_Next( info, 0 );
  }
  return 0;
}

void RegisterExportFunctions(HDATA Data, tDWORD* pOut, tDWORD iMaxSize, tDWORD* result ) 
{
	tDWORD exp_count = 0;
	HDATA iface_data = GetStaticIface(Data);
	tUINT  mcount = 0;
	CFaceInfo iface( iface_data, false );
	CPubMethodInfo method( iface );

	exp_count = 0;

	if ( iface_data ) 
	{
		while( method ) 
		{
		  if ((pOut != 0) && (mcount*4 < iMaxSize))
			  pOut[mcount] = method.MethodID(); 

		  mcount++;
		  method.GoNext();
		}
	} //	if ( iface_data ) 


	if ( result )
		*result = mcount;
}

void prop_iface_count(HDATA info, tDWORD* out_size, tCHAR* buffer, tDWORD size ) 
{
  tDWORD iface_count;

  iface_count = 0;

  if ( buffer ) 
  {
      info = ::DATA_Get_FirstEx( info, 0, DATA_IF_ROOT_CHILDREN );
      for( ; info; info=::DATA_Get_Next(info,0) ) 
	  {
        if ( !IsStaticIface(info) )
          iface_count++;
      }
    *(tDWORD*)buffer = iface_count;
  }
  *out_size = sizeof(tDWORD);
}


tERROR GetStaticIfacePropValByID( HDATA iface, tPID prop_id, tPTR buffer, tDWORD count, ULONG* olen ) {

  ULONG     addr[] = { VE_PID_IF_PROPERTIES, 0 };
  HDATA     prop_data = ::DATA_Get_FirstEx( iface, addr, DATA_IF_ROOT_CHILDREN );
  AVP_dword prop_type = (prop_id & pTYPE_MASK) >> 20;

  prop_id &= ~( pRANGE_MASK | pTYPE_MASK );

  while( prop_data ) {

    AVP_dword val;

    if (
      GetDwordPropVal(prop_data,0,VE_PID_IFP_SCOPE,&val)     && (val == VE_IFP_SHARED) &&
      GetDwordPropVal(prop_data,0,VE_PID_IFP_TYPE,&val)      && (val == prop_type) &&
      GetDwordPropVal(prop_data,0,VE_PID_IFP_DIGITALID,&val) && (val == prop_id)
    ) {

      AVP_dword id;
      HPROP     prop;
      tDWORD    size;

      switch( prop_type ) {

        case tid_CHAR    :
        case tid_BYTE    :
        case tid_SBYTE   :
          id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_byte );
          size = sizeof( AVP_byte );
          break;

        case tid_WORD    :
        case tid_WCHAR   :
        case tid_SHORT   :
          id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_word );
          size = sizeof( AVP_word );
          break;

        case tid_LONGLONG:
        case tid_QWORD   :
          id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_qword );
          size = sizeof( AVP_qword );
          break;

        case tid_STRING  :
          id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_str );
          size = 0;
          break;

        case tid_WSTRING :
          id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_wstr );
          size = 0;
          break;

        case tid_BINARY :
          id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_bin );
          size = 0;
          break;

        default :
          id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_dword );
          size = sizeof( AVP_dword );;
          break;

      }

      prop = DATA_Find_Prop( prop_data, 0, id );
      if ( !prop )
        return errPROPERTY_NOT_FOUND;

      if ( !size )
        size = PROP_Get_Val( prop, 0, 0 );

      if ( !olen )
        olen = &val;
      
      switch( prop_type ) {
        
      case tid_CHAR    :
      case tid_SBYTE   :
      case tid_BYTE    :
      case tid_SHORT   :
      case tid_WORD    :
      case tid_WCHAR   :
      case tid_INT     :
      case tid_LONG    :
      case tid_BOOL    :
      case tid_DWORD   :
      case tid_CODEPAGE:
      case tid_ERROR   :
      case tid_UINT    :
      case tid_IID     :
      case tid_PID     :
      case tid_ORIG_ID :
      case tid_OS_ID   :
      case tid_VID     :
      case tid_PROPID  :
      case tid_VERSION :
      case tid_DATA    :
      case tid_LONGLONG :
      case tid_QWORD    :
      case tid_DATETIME :
        *olen = size;
        if ( buffer ) {
          if ( count < size ) {
            *olen = 0;
            return errBUFFER_TOO_SMALL;
          }
          if ( !PROP_Get_Val(prop,buffer,size) )
            return errUNEXPECTED;
        }
        return errOK;
        
      case tid_STRING  :
      case tid_WSTRING :
      case tid_BINARY  :
        if ( buffer ) {
          if ( count > size )
            count = size;
          if ( !(*olen = PROP_Get_Val(prop,buffer,count)) )
            return errUNEXPECTED;
        }
        else
          *olen = size;
        return errOK;
        
      case tid_IFACEPTR :
      case tid_PTR      :
      case tid_OBJECT   :
        *olen = 0;
        return errINTERFACE_NOT_LOADED;
        
      default           :
        *olen = 0;
        return errNOT_IMPLEMENTED;
      }
    }
    
    prop_data = DATA_Get_Next( prop_data, 0 );
  }
  
  return errPROPERTY_NOT_FOUND;
}

tERROR GetPluginProp( HDATA Data, tDWORD* result, tPROPID prop_id, tPTR buffer, tDWORD count ) {

  AVP_dword pid;

  // String types
  if ( (pTYPE_MASK & prop_id) == pTYPE_STRING ) 
  {
    switch( prop_id ) 
	{
      case pgPLUGIN_COMMENT  : 
        pid = VE_PID_SHORTCOMMENT; break;
      case pgVENDOR_NAME : 
        pid = VE_PID_PL_AUTHORNAME; break; // ??? VE_PID_PL_VENDORNAME;
      default : 
        return -1;
    }
    if ( !Data || !GetStrPropVal(Data,0,pid,(char*)buffer,count,(ULONG*)result) )
      return errPROPERTY_NOT_FOUND;
    else
      return errOK;
  }

  // IDs in properties
  switch( prop_id ) 
  {
    case pgPLUGIN_ID      :
        pid = VE_PID_PL_DIGITALID;
        break;
	case pgPLUGIN_CODEPAGE : pid = VE_PID_PL_CODEPAGEID; break;
    case pgPLUGIN_VERSION  : pid = VE_PID_PL_VERSIONID;  break;
    case pgVENDOR_ID       : pid = VE_PID_PL_VENDORID;   break;
    case pgAUTO_START      : pid = VE_PID_PL_AUTOSTART;  break;
    default                : 
		return -1;
  }
  if ( !buffer ) 
  {
    if ( GET_AVP_PID_TYPE(pid) == avpt_dword ) 
	{
      *result = sizeof(tDWORD);
      return errOK;
    }
    if ( GET_AVP_PID_TYPE(pid) == avpt_bool ) 
	{
      *result = sizeof(tBOOL);
      return errOK;
    }
    *result = 0;
    return errPROPERTY_NOT_FOUND;
  }
  if ( GET_AVP_PID_TYPE(pid) == avpt_dword ) 
  {
    if ( count < sizeof(tDWORD) ) 
	{
      *result = sizeof(tDWORD);
      return errBUFFER_TOO_SMALL;
    }
    if ( !Data || !GetDwordPropVal(Data,0,pid,(ULONG*)buffer) ) 
	{
      *result = 0;
      return errPROPERTY_NOT_FOUND;
    }

    else if ( (prop_id == pgPLUGIN_CODEPAGE) && (*(tCODEPAGE*)buffer == 0) )
      *(tCODEPAGE*)buffer = cCP_ANSI;
    *result = sizeof(tDWORD);
    return errOK;
  }
  
  return errPROPERTY_NOT_FOUND;
}

tERROR GetInterfaceProperty( HDATA Data, tDWORD* result, tDWORD index, tPROPID prop_id, tPTR buffer, tDWORD count ) 
{
	tERROR error;
	tDWORD ret_val = 0;
	HDATA info;

	info = GetIface(Data, index);

	PR_TRACE_A0( this, "Enter Plugin::GetInterfaceProperty method" );

	error = errOK;

#define PLUGIN_LEVEL(p) ( ((p)==pgPLUGIN_VERSION) || ((p)==pgPLUGIN_ID) || ((p)==pgVENDOR_ID) || ((p)==pgVENDOR_NAME) || ((p)==pgPLUGIN_NAME) )
   if ( PLUGIN_LEVEL(prop_id) )
    return GetPluginProp( Data, &ret_val, prop_id, buffer, count );


    if ( !info )
      error = errINTERFACE_NO_MORE_ENTRIES; //errINTERFACE_NOT_FOUND;
    else 
	{
      switch( prop_id ) 
	  {
        case pgINTERFACE_ID       :
        case pgINTERFACE_SUBTYPE  :
        case pgINTERFACE_FLAGS    :
        case pgINTERFACE_CODEPAGE :
          if ( buffer && (count < sizeof(AVP_dword)) )
            error = errBUFFER_TOO_SMALL;
          break;
        }

        if ( error == errOK ) {
          tDWORD val;
          switch( prop_id ) {
          case pgINTERFACE_ID       :
            if ( !GetDwordPropVal(info,0,VE_PID_IF_DIGITALID,(AVP_dword*)buffer) )
              error = errPROPERTY_NOT_FOUND;
            break;
          case pgINTERFACE_SUBTYPE  :
            if ( !GetDwordPropVal(info,0,VE_PID_IF_SUBTYPEID,(AVP_dword*)buffer) )
              error = errPROPERTY_NOT_FOUND;
            break;
          case pgINTERFACE_FLAGS    :
            *((tDWORD*)buffer) = 0;
            //if ( GetBoolPropVal(info,0,VE_PID_IF_STATIC) )
            //  *((tDWORD*)buffer) |= IFACE_STATIC;
            if ( GetBoolPropVal(info,0,VE_PID_IF_SYSTEM) )
              *((tDWORD*)buffer) |= IFACE_SYSTEM;
            if ( GetBoolPropVal(info,0,VE_PID_IF_TRANSFERPROPUP) )
              *((tDWORD*)buffer) |= IFACE_PROP_TRANSFER;
            if ( GetBoolPropVal(info,0,VE_PID_IF_NONSWAPABLE) )
              *((tDWORD*)buffer) |= IFACE_UNSWAPPABLE;
            if ( GetDwordPropVal(info,0,VE_PID_IF_PROTECTED_BY,(ULONG*)&val) ) {
              switch( val ) {
              case VE_IFF_PROTECTED_BY_CS    : *((tDWORD*)buffer) |= IFACE_PROTECTED_BY_CS;    break;
              case VE_IFF_PROTECTED_BY_MUTEX : *((tDWORD*)buffer) |= IFACE_PROTECTED_BY_MUTEX; break;
              case VE_IFF_PROTECTED_BY_NONE  :
              default                        : break;
              }
            }
            break;
          default :
            error = GetStaticIfacePropValByID( info, prop_id, buffer, count, (ULONG*)&count );
            break;
        }
      }
    }
  if ( result )
    *result = PR_SUCC(error) ? count : 0;

	if ( result )
		*result = ret_val;
	return error;
}

void static_prop_count(HDATA iface, tDWORD* out_size, tCHAR* buffer, tDWORD size ) 
{
  tDWORD iface_count;

  iface_count = 0;
  ULONG     addr[] = { VE_PID_IF_PROPERTIES, 0 };

  if ( buffer ) 
  {
      iface = ::DATA_Get_FirstEx( iface, addr, DATA_IF_ROOT_CHILDREN );
      
      for( ; iface; iface=::DATA_Get_Next(iface,0) ) 
	  {
        AVP_dword scope;
        if ( GetDwordPropVal(iface,0,VE_PID_IFP_SCOPE,&scope) && (scope == VE_IFP_SHARED) )
          iface_count++;
      }
    *(tDWORD*)buffer = iface_count;
  }
  *out_size = sizeof(tDWORD);
}

tERROR enum_static_properties ( HDATA iface, StaticProperty *pOut, tDWORD* p_prop_count )
{
  if ( p_prop_count )
    *p_prop_count = 0;

  tDWORD size;  

  tDWORD icount;

  static_prop_count( iface, &size, reinterpret_cast<tCHAR*>(&icount), sizeof(tDWORD) );

  if ( p_prop_count )
    *p_prop_count = icount;

  if ( !pOut )
    return errOK;

  ULONG     addr[] = { VE_PID_IF_PROPERTIES, 0 };
  HDATA     iface_data = ::DATA_Get_FirstEx( iface, addr, DATA_IF_ROOT_CHILDREN );
  
  tDWORD i = 0;  
  while ( i < icount ) { 

    AVP_dword scope;

    if ( GetDwordPropVal(iface_data,0,VE_PID_IFP_SCOPE,&scope) && (scope == VE_IFP_SHARED) ) {
      AVP_dword prop_type; 
      if ( !GetDwordPropVal(iface_data,0,VE_PID_IFP_TYPE,&prop_type) )
          return errUNEXPECTED;
      AVP_dword prop_id;
      if ( !GetDwordPropVal(iface_data,0,VE_PID_IFP_DIGITALID,&prop_id) )
          return errUNEXPECTED;

      pOut [i].thePropertyID = ( ( prop_type << pTYPE_SHIFT ) | prop_id | pRANGE_GLOBAL );
      AVP_dword id;
      
      switch( prop_type ) {

        case tid_CHAR    :
        case tid_BYTE    :
        case tid_SBYTE   :
          id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_byte );
          pOut [i].thePropertySize = sizeof( AVP_byte );
          break;

        case tid_WORD    :
        case tid_WCHAR   :
        case tid_SHORT   :
          id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_word );
          pOut [i].thePropertySize = sizeof( AVP_word );
          break;

        case tid_LONGLONG:
        case tid_QWORD   :
          id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_qword );
          pOut [i].thePropertySize = sizeof( AVP_qword );
          break;

        case tid_STRING  :
          id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_str );
          break;

        case tid_WSTRING :
          id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_wstr );
          break;

        case tid_BINARY :
          id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_bin );
          break;

        case tid_BOOL :
          id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_bool );
          pOut [i].thePropertySize = sizeof( AVP_bool );
          break;

        default :
          id = VE_PID_IFPSS_VARTYPIFIEDDEFVALUE( avpt_dword );
          pOut [i].thePropertySize = sizeof( AVP_dword );
          break;

      }

      HPROP prop = DATA_Find_Prop( iface_data, 0, id );
      if ( !prop )
        return errPROPERTY_NOT_FOUND;

      if ( !pOut [i].thePropertySize )
        pOut [i].thePropertySize = PROP_Get_Val( prop, 0, 0 );
      
      if ( !pOut[i].allocateValue () )
        return errNOT_ENOUGH_MEMORY;

      switch( prop_type ) {
      case tid_CHAR    :
      case tid_SBYTE   :
      case tid_BYTE    :
      case tid_SHORT   :
      case tid_WORD    :
      case tid_WCHAR   :
      case tid_INT     :
      case tid_LONG    :
      case tid_BOOL    :
      case tid_DWORD   :
      case tid_CODEPAGE:
      case tid_ERROR   :
      case tid_UINT    :
      case tid_IID     :
      case tid_PID     :
      case tid_ORIG_ID :
      case tid_OS_ID   :
      case tid_VID     :
      case tid_PROPID  :
      case tid_VERSION :
      case tid_DATA    :
      case tid_LONGLONG :
      case tid_QWORD    :
      case tid_DATETIME :
      case tid_STRING  :
      case tid_WSTRING :
      case tid_BINARY  :
        if ( !PROP_Get_Val(prop,pOut[i].thePropertyValue,pOut[i].thePropertySize) )
          return errUNEXPECTED;
        break;

      case tid_IFACEPTR :
      case tid_PTR      :
      case tid_OBJECT   :
        break;//return errINTERFACE_NOT_LOADED;
        
      default           :
        break;//return errNOT_IMPLEMENTED;
      }
      ++i;
    }
    
    iface_data = DATA_Get_Next( iface_data, 0 );
  }
  return errOK;
}

void enum_interfaces( HDATA Data, InterfaceMetadata *pOut, tDWORD* p_iface_count ) 
{

	tDWORD size;
	tDWORD icount;
	tDWORD i;
	tERROR error;
    tCODEPAGE plugin_codepage;

    if ( p_iface_count )
		*p_iface_count = 0;

	prop_iface_count( Data, &size, (tCHAR*)&icount, sizeof(tDWORD) );

    if (p_iface_count != 0)
	  *p_iface_count = icount;

	if (pOut == 0)
		return;

    error = GetDwordPropVal(Data,0,VE_PID_PL_CODEPAGEID, &plugin_codepage );

    if ( PR_FAIL ( error ) ) {
      PR_TRACE(( this, prtNOTIFY, "(%terr): cannot get iface CODEPAGE id from metadata (plugin = \"%s\"(\"%s\"), iface number = %u, %tiid, %tpid", error, m_name, comment, i-1, iid, pid ) );
      plugin_codepage = cCP_ANSI;
    }

	for( i=0; i<icount; i++ ) 
	{
				
		tIID iid;
		
        error = GetInterfaceProperty(Data, 0, i, pgINTERFACE_ID, &iid, sizeof(iid) ); // index zero is for the plugin properties
		if ( PR_FAIL(error) ) 
		{ 
			PR_TRACE(( this, prtERROR, "Error reading iface description (plugin = \"%s\", iface item number - %u, %terr)", m_name, i-1, error ));
		}
		else if ( (iid == IID_ANY) || (iid == IID_NONE) ) 
		{
			continue;
		}
		else 
		{
			tPID     pid;
			tDWORD   sub;
			tVERSION ver;
			tVID     vid;
			tDWORD   flags;
			tIID     compat;
            tCODEPAGE codepage;

			if ( PR_SUCC(error=GetInterfaceProperty(Data, 0,i, pgPLUGIN_ID,&pid,sizeof(pid))) ) // plugin ID
			{

				if ( PR_FAIL(error=GetInterfaceProperty(Data, 0,i,pgINTERFACE_SUBTYPE,&sub,sizeof(sub))) ) {// subtype of interface in plugin
					sub = 0;
					PR_TRACE(( this, prtERROR, "(%terr): error in metadata: cannot get subtype (plugin = \"%s\"(\"%s\"), iface number = %u, %tiid, %tpid", error, m_name, comment, i-1, iid, pid ) );
				}

				if ( PR_FAIL(error=GetInterfaceProperty(Data, 0,i,pgINTERFACE_VERSION,&ver,sizeof(ver))) ) { // version
					ver = 1;
					PR_TRACE(( this, prtIMPORTANT, "(%terr): error in metadata: cannot get iface VERSION from metadata (plugin = \"%s\"(\"%s\"), iface number = %u, %tiid, %tpid", error, m_name, comment, i-1, iid, pid ) );
				}

				if ( PR_FAIL(error=GetInterfaceProperty(Data, 0,i,pgVENDOR_ID,&vid,sizeof(vid))) ) { // vendor id
					vid = VID_KASPERSKY_LAB;
					PR_TRACE(( this, prtNOTIFY, "(%terr): error in metadata: cannot get VENDOR ID from metadata (plugin = \"%s\"(\"%s\"), iface number = %u, %tiid, %tpid", error, m_name, comment, i-1, iid, pid ) );
				}

				if ( PR_FAIL(error=GetInterfaceProperty(Data, 0,i,pgINTERFACE_FLAGS,&flags,sizeof(flags))) ) { // interface flags
					flags = 0;
					PR_TRACE(( this, prtNOTIFY, "(%terr): error in metadata: cannot get iface FLAGS from metadata (plugin = \"%s\"(\"%s\"), iface number = %u, %tiid, %tpid", error, m_name, comment, i-1, iid, pid ) );
				}
				
				if ( PR_FAIL(error=GetInterfaceProperty(Data, 0,i,pgINTERFACE_COMPATIBILITY,&compat,sizeof(compat))) ) { // interface compatibility
					compat = PID_ANY;
					PR_TRACE(( this, prtNOTIFY, "(%terr): cannot get iface COMPATIBILITY id from metadata (plugin = \"%s\"(\"%s\"), iface number = %u, %tiid, %tpid", error, m_name, comment, i-1, iid, pid ) );
				}
                
                if ( PR_FAIL(error=GetInterfaceProperty(Data, 0,i,pgINTERFACE_CODEPAGE,&codepage,sizeof(codepage))) ) { // interface code page
					codepage = plugin_codepage;
					PR_TRACE(( this, prtNOTIFY, "(%terr): cannot get iface CODEPAGE id from metadata (plugin = \"%s\"(\"%s\"), iface number = %u, %tiid, %tpid", error, m_name, comment, i-1, iid, pid ) );
				}

                HDATA iface = GetIface ( Data, i );  
               	enum_static_properties(iface, 0, & pOut [i].theStaticPropertiesCount );
	            if ( pOut[i].theStaticPropertiesCount )
	            {
		          if ( !pOut[i].allocateStaticProperties () )
                    return;
		          enum_static_properties(iface, pOut[i].theStaticProperties, 0);
	            }
		        pOut[i].thePID = pid;
		        pOut[i].theSub = sub;
		        pOut[i].theVersion = ver;
		        pOut[i].theVID = vid;
		        pOut[i].theIID = iid;
		        pOut[i].theFlags = flags;
		        pOut[i].theCompat = compat;
                pOut[i].theCodePage = codepage;
      }
    }
  } 
  
  return;
}


void ProcessMe(HDATA Data, const char* pcName)
{
    PluginMetadata aPragueMetadata;
	int i;
	char szName[MAX_PATH];
    
	RegisterExportFunctions(Data, 0, 0, &aPragueMetadata.theExportsCount);
	
	if ( aPragueMetadata.theExportsCount )
	{
		if ( !aPragueMetadata.allocateExports () )
		  return;
		RegisterExportFunctions(Data, aPragueMetadata.theExports, sizeof(tDWORD) * aPragueMetadata.theExportsCount, 0);
	}

	enum_interfaces(Data, 0, &aPragueMetadata.theInterfacesCount);
	if ( aPragueMetadata.theInterfacesCount )
	{
		if ( !aPragueMetadata.allocateInterfaces () )
          return;
		enum_interfaces(Data, aPragueMetadata.theInterfaces, 0);
	}

	aPragueMetadata.theAutoStart = GetBoolPropVal(Data,0,VE_PID_PL_AUTOSTART);
    GetDwordPropVal(Data,0,VE_PID_PL_CODEPAGEID, &aPragueMetadata.theCodePage );
    GetInterfaceProperty(Data, 0,0,pgPLUGIN_ID,&aPragueMetadata.thePluginID,sizeof(aPragueMetadata.thePluginID));

	strcpy(szName, pcName);
	for (i = strlen(pcName); (i > 0) && (szName[i] != '\\') && (szName[i] != '/'); i--)
	{
		if (szName[i] == '.')
		{
			szName[i] = '\x00';
			break;
		}
	}

	strcat(szName, ".meta");

	save ( aPragueMetadata, szName );
}

HRESULT CPragueIServer::SaveInterfacesDB(LPCTSTR pContainerName, DWORD dwSize, BYTE *pInterfaces) {
	HRESULT hError = E_INVALIDARG;

	if ( dwSize && pInterfaces ) {
		hError = PISRV_E_INTERNALERROR;
		HDATA hRootData = ::DeserializeDTree( pInterfaces, dwSize );
		if ( hRootData ) {
			HDATA hResultData = ::PrepareDBTree( hRootData );
			if ( hResultData ) {
				ProcessMe(hRootData, pContainerName);
				CPathStr rcNewFileName = pContainerName;
				TCHAR *pExt = ::IOSFindFileExtention( rcNewFileName, true );
				if ( pExt )
					lstrcpy( pExt, _T("dsc") );
				else
					rcNewFileName += _T(".dsc");

				CheckOutFile(rcNewFileName);
				AVP_bool nResult = ::KLDT_SerializeUsingSWM( rcNewFileName, hResultData, _T("") );

				::DATA_Remove( hResultData, NULL );

				if ( !nResult ) {
					m_rcLastErrorStr = rcNewFileName;
					hError = PISRV_E_SAVEDESCRIPTIONDB;
				}
				else
					hError = S_OK;
			}
			::DATA_Remove( hRootData, NULL );
		}
	}

	return hError;
}

