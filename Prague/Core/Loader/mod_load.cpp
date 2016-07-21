#include <stdio.h>
#include <Prague/prague.h>
#include "prformat.h"
#include "loader.h"
#include "mod_load.h"
#include <Prague/pr_reg.h>
#include <Prague/iface/e_ktrace.h>

#include "memmgr.h"

#include <klsys/filemapping/wholefilemapping.h>

using KLSysNS::WholeFileMapping;
using KLSysNS::FileMapping;
//#define SHOW_START_ADDRESS


// ---
extern "C" tBOOL pr_call known_plugin_type( tDWORD plugin_type ) {
  switch ( plugin_type ) {
		case cPRAGUE_PLUGIN_FOUND:
		case cNATIVE_PLUGIN_FOUND:
		#if defined(ELF_SUPPORT)
			case cELF_PLUGIN_FOUND:
		#endif
				return cTRUE;
		default :
			return cFALSE;
  }
}


#include <mod_load_inc.c>

#if defined (OLD_METADATA_SUPPORT)
//
// MetaData functions
//


// ---
extern "C" tERROR pr_call GetPrMetaData( const tVOID* szFileName, tCODEPAGE cp, tPTR hModuleInfo ) {
  HDATA* aModuleInfo = (HDATA*) hModuleInfo; 
  PrFileHeader* PrHeader = 0;
  tERROR error = errOK;
  FileMapping::kl_size_t aPluginSize = 0;
  FileMapping theFileMapping;

	int not_created;
	#if defined(_WIN32)
		if ( cp == cCP_UNICODE )
			not_created = theFileMapping.create( UNI(szFileName), KL_FILEMAP_READ );
		else
	#endif
			not_created = theFileMapping.create( MB(szFileName), KL_FILEMAP_READ );

	PR_TRACE_VARS;

  if ( not_created ){
    PR_TRACE(( 0, prtERROR, "ldr\tGetPrMetaData: Cannot create mapping of Prague module \"%s%S\"", PR_TRACE_PREPARE(szFileName) ));
    return errOBJECT_NOT_FOUND;
  }

  PrHeader = (PrFileHeader* )theFileMapping.map ( 0, 0, &aPluginSize );
  if ( !PrHeader || (PrHeader==(PrFileHeader*)-1 ) || (aPluginSize<sizeof(PrFileHeader)) ) {
    PR_TRACE(( 0, prtERROR, "ldr\tGetPrMetaData: Cannot map Prague module \"%s%S\"", PR_TRACE_PREPARE(szFileName) ));
    return errUNEXPECTED;
  }

  if( PrHeader->dwMetaDataSectionSize && 
      ( ( PrHeader->dwMetaDataSectionOffset + PrHeader->dwMetaDataSectionSize ) <= aPluginSize ) ) {
    HDATA aDataTree = 0;
		AVP_dword res = KLDT_DeserializeFromMemoryUsingSWM( (char*)PrHeader + PrHeader->dwMetaDataSectionOffset,
                                             PrHeader->dwMetaDataSectionSize, &aDataTree );
    if ( res ) 
      *aModuleInfo = aDataTree;
    else {
      PR_TRACE(( 0, prtERROR, "ldr\tDescription for module \"%s%S\" cannot be deserialized !", PR_TRACE_PREPARE(szFileName) ));
      error = errUNEXPECTED;
    }
  }
  else
    error = errMODULE_IS_BAD;

  theFileMapping.unmap ( PrHeader, aPluginSize );
  return error;
}

#endif // OLD_METADATA_SUPPORT
//
// Loading fulctions
//


// ---
tERROR pr_call LoadSections(tBYTE* aPlugin, tUINT aSize, tBYTE* lpBase)
{
  LPPrFileHeader lpHeader;
  SectionTable* SecTable;
  tDWORD i;
  lpHeader = (LPPrFileHeader)lpBase;
  for ( i=0, SecTable=(SectionTable*)(aPlugin + sizeof ( PrFileHeader ) );
        i < (tDWORD)lpHeader->wNumOfObjects;
        i++, SecTable++ ) {
    if ( SecTable->dwSectionRVA != 0 && SecTable->dwSectionRVA < lpHeader->dwPluginVirtualSize ) {
      if ( ( SecTable->dwSectionOffset + SecTable->dwPhisicalSize ) < aSize ) 
        memcpy ( lpBase+SecTable->dwSectionRVA, aPlugin + SecTable->dwSectionOffset, SecTable->dwPhisicalSize );
      else
        return errOBJECT_READ;
    }
  }
  return errOK;
}



// ---
tERROR pr_call RelocateIt( tBYTE* aPlugin, tBYTE* lpBase ) {
  LPPrFileHeader lpHeader;
  LPRelocTable ReloHeader;
  tWORD* pwTmp;
  tDWORD i;
  tDWORD dwDelta;

  lpHeader = (LPPrFileHeader)lpBase;
  if(lpHeader->dwReloSectionSize != 0)
    {
      dwDelta=(tDWORD)lpBase-lpHeader->dwImageBase;
      ReloHeader = (LPRelocTable)(aPlugin + lpHeader->dwReloSectionOffset);
      while (ReloHeader->dwPageRVA)
        {
          for(i=sizeof(tDWORD)*2;i<ReloHeader->dwBlockSize;i+=sizeof(tWORD))
            {
              pwTmp = (tWORD*) (((tBYTE*)ReloHeader)+i);
              switch(*pwTmp &  0xf000)
                {
                case 0:
                  break;
                case 0x3000:
                  *((tDWORD*)(lpBase+ReloHeader->dwPageRVA+(*pwTmp & 0x0fff)))+=dwDelta;
                  break;
                default:
                  return errUNEXPECTED;
                }
            }
          ReloHeader=(LPRelocTable)(((tBYTE*)ReloHeader)+ReloHeader->dwBlockSize);
        }
    }
  return errOK;
}

#define SECTION_EXECUTE                0x20000000  // Section is executable.
#define SECTION_READ                   0x40000000  // Section is readable.
#define SECTION_WRITE                  0x80000000  // Section is writeable.

// ---
tERROR pr_call SetProtection (tBYTE* aPlugin, tBYTE* lpBase)
{
  LPPrFileHeader lpHeader;
  SectionTable* SecTable;
  tDWORD i;
  tUINT  dwProtection = 0;
  lpHeader = (LPPrFileHeader)lpBase;
  for ( i = 0, SecTable = (SectionTable*)(aPlugin + sizeof(PrFileHeader) );
        i < (tDWORD)lpHeader->wNumOfObjects;
        i++, SecTable++ ) {
    if ( SecTable->dwSectionRVA != 0 && SecTable->dwSectionRVA < lpHeader->dwPluginVirtualSize ) {
      dwProtection = MEMORY_PROTECTION_NONE;
      if ( SecTable->dwFlags & SECTION_EXECUTE )
        dwProtection |= MEMORY_PROTECTION_EXECUTE;
      if ( SecTable->dwFlags & SECTION_READ )
        dwProtection |= MEMORY_PROTECTION_READ;
      if ( SecTable->dwFlags & SECTION_WRITE )
        dwProtection |= MEMORY_PROTECTION_WRITE;
      if ( protect ( lpBase+SecTable->dwSectionRVA, SecTable->dwPhisicalSize, dwProtection ) )
        return errUNEXPECTED;
    }
  }
  return errOK;
}



// ---
tERROR pr_call LoadPrPlugin( const tVOID* szPluginName, tCODEPAGE cp, hPLUGININSTANCE* pInst, tPluginInit* pEntryPoint ) {

#if defined (_WIN32)
  DWORD  old_error_mode;
#endif
  tERROR err;
  tDWORD aSize = 0;
#if defined (_DEBUG)
  tDWORD nNameLen = 0; 
  tDWORD nNameLenAligned = 0; 
#endif
  tBYTE* hInst = 0;

	PR_TRACE_VARS;

  PR_TRACE(( 0, prtSPAM, "ldr\tLoadPrPlugin: Loading prague plugin \"%s%S\"", PR_TRACE_PREPARE(szPluginName) ));

  if ( !pInst || !pEntryPoint )
    return errPARAMETER_INVALID;

  *pInst = 0;
  *pEntryPoint = 0;
#if defined (_WIN32)
  old_error_mode = SetErrorMode( SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX );
#endif

  TRY {
    FileMapping theFileMapping;
    PrFileHeader* PrHeader = 0;
    tBYTE* aPlugin = 0;
    FileMapping::kl_size_t aPluginSize = 0;

		int not_created;
		#if defined(_WIN32)
			if ( cp == cCP_UNICODE )
				not_created = theFileMapping.create( UNI(szPluginName), KL_FILEMAP_READ );
			else
		#endif
				not_created = theFileMapping.create( MB(szPluginName), KL_FILEMAP_READ );

    if ( not_created ) {
      PR_TRACE(( 0, prtERROR, "ldr\tLoadPrPlugin: Cannot create mapping of Prague module \"%s%S\"", PR_TRACE_PREPARE(szPluginName) ));
      return errOBJECT_NOT_FOUND;
    }

    aPlugin = (tBYTE*)theFileMapping.map ( 0, 0, &aPluginSize );
    if ( !aPlugin || ( aPlugin == (tPTR)-1 ) ) {
      PR_TRACE(( 0, prtERROR, "ldr\tLoadPrPlugin: Cannot map Prague module \"%s%S\"", PR_TRACE_PREPARE(szPluginName) ));
      return errUNEXPECTED;
    }

    PrHeader = (PrFileHeader* )aPlugin;
    aSize = PrHeader->dwPluginVirtualSize;

		#if defined (_DEBUG)
			#if defined(_WIN32)
				if ( cp == cCP_UNICODE )
					nNameLen = _toui32(UNIS * (1 + wcslen(UNI(szPluginName))));
				else
			#endif
					nNameLen = _toui32(MBS * (1 + strlen(MB(szPluginName))));
			nNameLenAligned =  (nNameLen & 0x0F ? ( nNameLen + 0x10 ) & 0xFFFFFFF0 : nNameLen);
			aSize += nNameLenAligned;
		#endif

    hInst = (tBYTE*)allocate( aSize, MEMORY_PROTECTION_READ|MEMORY_PROTECTION_WRITE );
    if( hInst == NULL ) {
      theFileMapping.unmap (  aPlugin, aPluginSize );
      return errNOT_ENOUGH_MEMORY;
    }

#if defined (_DEBUG)
    memcpy( hInst, szPluginName, nNameLen );
    hInst += nNameLenAligned;
#endif

    if ( aPluginSize < sizeof(PrFileHeader) ) {
      theFileMapping.unmap (  aPlugin, aPluginSize );
      deallocate ( (tPTR)hInst, aSize );
      return errOBJECT_READ;
    }

    memcpy( hInst, PrHeader, sizeof(PrFileHeader) );
    err = LoadSections( aPlugin, _toui32(aPluginSize), hInst );
    if( PR_SUCC(err) )
      err = RelocateIt( aPlugin, hInst );
    if( PR_SUCC(err) )
      err = SetProtection( aPlugin, hInst );
    if( PR_SUCC(err) ) {
      *pInst = (hPLUGININSTANCE)hInst;
      *pEntryPoint = (tPluginInit)(hInst+PrHeader->dwEntryPoint);
    }
    else
      deallocate ( (tPTR)hInst, aSize );
    theFileMapping.unmap ( aPlugin, aPluginSize );
  }
  EXCEPT{
    err = errUNEXPECTED;
    PR_TRACE(( 0, prtERROR, "ldr\tLoadPrPlugin: exception caught on \"%s%S\"", p1, p2 ));
  }

#if defined(SHOW_START_ADDRESS)
  if ( hInst )
    ShowPluginStartAddress( pluginName, *pEntryPoint );
#endif
#if defined (_WIN32)	
  SetErrorMode( old_error_mode );
#endif
  return err;
}



// ---
extern "C" tERROR pr_call LoadPlugin( hOBJECT obj, const tVOID* szPluginName, tCODEPAGE cp, tDWORD plugin_type, hPLUGININSTANCE* pInst, tPluginInit* pEntryPoint, void(**finish_proc)(), tProtector* protect ) {

	tERROR          error;
	tERROR          sync_err = errOK;
	tPluginInit     entryPoint = 0;
	hPLUGININSTANCE inst = 0;

	PR_TRACE_VARS;

  if ( pInst == NULL )	{
    PR_TRACE(( obj, prtERROR, "ldr\tLoadPlugin: invalid parameter (pInst == NULL) for \"%s%S\"", PR_TRACE_PREPARE(szPluginName) ));
    return errPARAMETER_INVALID;
  }
	
  if ( pEntryPoint == NULL )	{
    PR_TRACE(( obj, prtERROR, "ldr\tLoadPlugin: invalid parameter (pEntryPoint == NULL) for \"%s%S\"", PR_TRACE_PREPARE(szPluginName) ));
    return errPARAMETER_INVALID;
  }
	
	if ( !known_plugin_type(plugin_type) ) {
		PR_TRACE(( obj, prtERROR, "ldr\tLoadPlugin: invalid parameter (plugin_type (%d) - unknown)  for \"%s%S\"", plugin_type, PR_TRACE_PREPARE(szPluginName) ));
		return errOBJECT_INCOMPATIBLE;
	}

	error = errOK;
	if ( protect )
		error = unlockProtector( *protect, obj, szPluginName, cp );
	if ( PR_FAIL(error) )
		return error;

	PR_TRACE(( obj, prtNOTIFY, "ldr\tLoadPlugin: \"%s%S\" is going to load", PR_TRACE_PREPARE(szPluginName) ));

  switch( plugin_type ) {
		case cPRAGUE_PLUGIN_FOUND:
			error = LoadPrPlugin( szPluginName, cp, &inst, &entryPoint );
			break;
		case cNATIVE_PLUGIN_FOUND:
			error = LoadDllPlugin( szPluginName, cp, &inst, &entryPoint );
			break;
		#if defined(ELF_SUPPORT)
			case cELF_PLUGIN_FOUND:
			error = LoadElfPlugin( szPluginName, cp, &inst, &entryPoint, finish_proc );
		#endif
  }

	if ( protect )
		sync_err = lockProtector( *protect, obj, szPluginName, cp );

	if ( PR_SUCC(error) ) {
		*pInst = inst;
		*pEntryPoint = entryPoint;
		if ( PR_FAIL(sync_err) )
			error = sync_err;
		PR_TRACE(( obj, prtIMPORTANT, "ldr\tLoadPlugin: \"%s%S\" loaded at base:0x%p", PR_TRACE_PREPARE(szPluginName), inst ));
	}
	else {
		PR_TRACE(( obj, prtERROR, "ldr\tLoadPlugin: error(%terr) loading \"%s%S\"", error, PR_TRACE_PREPARE(szPluginName) ));
	}
  return error;
}



// ---
extern "C" int pr_call CheckPluginByName( const tVOID* szPluginName, tCODEPAGE cp ) {
  int aResult = cUNEXPECTED_ERROR;
  PrFileHeader* PrHeader = 0;
  WholeFileMapping theFileMapping;
	WholeFileMapping::kl_size_t mapped_size = 0;
	PR_TRACE_VARS;


	if ( cp == cCP_UNICODE )
#if defined (_WIN32)
		PrHeader = (PrFileHeader*)theFileMapping.map( UNI(szPluginName), KL_FILEMAP_READ, &mapped_size );
#else
	{
		tCHAR l_name [MAX_PATH] = {0};
		tERROR error = CopyTo (l_name, sizeof(l_name), cCP_ANSI, (tVOID*)szPluginName, 0, cp, cSTRING_CONTENT_ONLY, 0 );
		if (PR_FAIL(error))
			return error;
		PrHeader = (PrFileHeader*)theFileMapping.map( l_name, KL_FILEMAP_READ, &mapped_size );
	}
#endif
	else
		PrHeader = (PrFileHeader*)theFileMapping.map( MB(szPluginName), KL_FILEMAP_READ, &mapped_size );
	
  if ( !PrHeader || (PrHeader == (PrFileHeader*)-1) || (mapped_size < sizeof(PrFileHeader)) ) {
		PR_TRACE(( 0, prtERROR, "ldr\tCheckPluginByName: Cannot map Prague module \"%s%S\"", PR_TRACE_PREPARE(szPluginName) ));
    return aResult;
  }

  if ( PrHeader->dwSignature == cHEADER_SIGNATURE ) {
    if ( PrHeader->wCPUType == cINTEL_32_CPU ) {
      aResult = cPRAGUE_PLUGIN_FOUND;
      PR_TRACE(( 0, prtSPAM, "ldr\tPrague plugin found" ));
    }
    else
      PR_TRACE(( 0, prtERROR, "ldr\tBad CPU Type %X\n", PrHeader->wCPUType ));
  }
  else 
    aResult = checkNativePlugin( theFileMapping.data() );
  return aResult;
}



// ---
extern "C" tERROR pr_call UnloadPlugin( hPLUGININSTANCE hInst, tDWORD plugin_type, void (__cdecl * finish_proc)(void) ) {
	if ( plugin_type == 0 ) {
		if ( *((tDWORD*)hInst) == cHEADER_SIGNATURE )
			plugin_type = cPRAGUE_PLUGIN_FOUND;
		else
			plugin_type = checkNativePlugin(hInst);
	}
  if ( plugin_type == cPRAGUE_PLUGIN_FOUND ) {
    LPPrFileHeader lpHeader;
    lpHeader = (LPPrFileHeader)hInst;
    deallocate ( (void*)hInst, lpHeader->dwPluginVirtualSize );
    return errOK; 
  }
	if ( plugin_type == cNATIVE_PLUGIN_FOUND ) {
		UnloadDllPlugin( hInst );
		return errOK;
	}
	#if defined( ELF_SUPPORT )
		if ( plugin_type == cELF_PLUGIN_FOUND ) {
			if ( finish_proc )
				finish_proc();
			VirtualFree( hInst, 0, MEM_FREE );
			return errOK;
		}
	#endif
  return errMODULE_UNKNOWN_FORMAT;
}
