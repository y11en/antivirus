#include <stdio.h>
#include "prague.h"
#include "prformat.h"
#include "mod_load.h"
#include "pr_reg.h"
//#include <plugin/p_win32loader.h>
#include <iface/e_ktrace.h>

#include "loader.h"
#include "memmgr.h"

#include <klsys/filemapping.h>

//#define SHOW_START_ADDRESS

#include <mod_load.inc>


//
// MetaData functions
//
tERROR pr_call GetMetaData( const tCHAR* szFileName, tPTR hModuleInfo )
{
  tERROR error;
  switch (CheckPluginByName(szFileName)){
  case cPRAGUE_PLUGIN_FOUND:
    error = GetPrMetaData(szFileName, hModuleInfo );
    break;
  case cNATIVE_PLUGIN_FOUND:
    error = GetDllMetaData(szFileName, hModuleInfo );
    break;
  default :
    PR_TRACE( (0,prtERROR,"CheckPlugin failed \"%s\"", szFileName) );
    error = errOBJECT_INCOMPATIBLE;
      
  }
  return error;
}

tERROR pr_call GetPrMetaData( const tCHAR* szFileName, tPTR hModuleInfo ) {
  HDATA* aModuleInfo = (HDATA*) hModuleInfo; 
  PrFileHeader* PrHeader = 0;
  tERROR error = errOK;
  kl_size_t aPluginSize = 0;
  struct FileMapping aFileMapping;

  if ( fm_create ( &aFileMapping, szFileName, KL_FILEMAP_READ, ahNormalAccess ) ){
    PR_TRACE( (0,prtERROR,"LoadPrPlugin: Cannot create mapping of Prague module \"%s\"", szFileName) );
    return errOBJECT_NOT_FOUND;
  }
  PrHeader = (PrFileHeader* )fm_map ( &aFileMapping, 0, 0, &aPluginSize );
  if ( !PrHeader || ( PrHeader == (PrFileHeader* )-1 ) || ( aPluginSize < sizeof ( PrFileHeader )  ) ) {
    fm_destroy ( &aFileMapping );
    PR_TRACE( (0,prtERROR,"LoadPrPlugin: Cannot map Prague module \"%s\"", szFileName ) );
    return errUNEXPECTED;
  }
  if( PrHeader->dwMetaDataSectionSize && 
      ( ( PrHeader->dwMetaDataSectionOffset + PrHeader->dwMetaDataSectionSize ) <= aPluginSize ) ) {
    HDATA aDataTree = 0;
    if ( KLDT_DeserializeFromMemoryUsingSWM( (char*)PrHeader + PrHeader->dwMetaDataSectionOffset,
                                             PrHeader->dwMetaDataSectionSize, &aDataTree ) ) 
      *aModuleInfo = aDataTree;
    else {
      PR_TRACE(( 0, prtERROR, "Description for module \"%s\" cannot be deserialized !", szFileName ));
      error = errUNEXPECTED;
    }
  }
  else
    error = errMODULE_IS_BAD;

  fm_unmap ( PrHeader, aPluginSize );
  fm_destroy ( &aFileMapping );
  
  return error;
}



//
// Loading fulctions
//

tERROR pr_call pr_call LoadPlugin( const tCHAR* szPluginName, tDWORD plugin_type, hPLUGININSTANCE* pInst, tPluginInit* pEntryPoint ) {
  tERROR error;

  PR_TRACE( (0,prtFUNCTION_FRAME,"Enter LoadPlugin with param \"%s\", %p", szPluginName, pEntryPoint) );

  if ( pInst == NULL )	{
    PR_TRACE( (0,prtERROR,"LoadPlugin: invalid parameter (pInst == NULL)") );
    return errPARAMETER_INVALID;
  }
	
  if ( pEntryPoint == NULL )	{
    PR_TRACE( (0,prtERROR,"LoadPlugin: invalid parameter (pEntryPoint == NULL)") );
    return errPARAMETER_INVALID;
  }

  switch ( plugin_type ) {
  case cPRAGUE_PLUGIN_FOUND:
    error = LoadPrPlugin( szPluginName, pInst, pEntryPoint );
    break;
  case cNATIVE_PLUGIN_FOUND:
    error = LoadDllPlugin( szPluginName, pInst, pEntryPoint );
    break;
  default :
    PR_TRACE( (0,prtERROR,"CheckPlugin failed \"%s\"", szPluginName) );
    error = errOBJECT_INCOMPATIBLE;
    break;
  }
	
  return error;
}

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

tERROR pr_call RelocateIt(tBYTE* aPlugin, tBYTE* lpBase)
{
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
tERROR pr_call LoadPrPlugin( const tCHAR* szPluginName, hPLUGININSTANCE* pInst, tPluginInit* pEntryPoint ) {
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

  PR_TRACE(( 0, prtSPAM, "LoadPrPlugin: Loading prague plugin \"%s\"", szPluginName ));

  if ( !pInst || !pEntryPoint )
    return errPARAMETER_INVALID;

  *pInst = 0;
  *pEntryPoint = 0;
#if defined (_WIN32)
  old_error_mode = SetErrorMode( SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX );
#endif

  TRY {
    struct FileMapping aFileMapping;
    PrFileHeader* PrHeader = 0;
    tBYTE* aPlugin = 0;
    kl_size_t aPluginSize = 0;

    if ( fm_create ( &aFileMapping, szPluginName, KL_FILEMAP_READ, ahNormalAccess ) ){
      PR_TRACE( (0,prtERROR,"LoadPrPlugin: Cannot create mapping of Prague module \"%s\"", szPluginName) );
      return errOBJECT_NOT_FOUND;
    }
    aPlugin = (tBYTE*)fm_map ( &aFileMapping, 0, 0, &aPluginSize );
    if ( !aPlugin || ( aPlugin == (tPTR)-1 ) ) {
      fm_destroy ( &aFileMapping );
      PR_TRACE( (0,prtERROR,"LoadPrPlugin: Cannot map Prague module \"%s\"", szPluginName) );
      return errUNEXPECTED;
    }
    PrHeader = (PrFileHeader* )aPlugin;
    aSize = PrHeader->dwPluginVirtualSize;
#if defined (_DEBUG)
    nNameLen = strlen(szPluginName) + 1;
    nNameLenAligned =  (nNameLen & 0x0F ? ( nNameLen + 0x10 ) & 0xFFFFFFF0 : nNameLen);
    aSize += nNameLenAligned;
#endif
    hInst = (tBYTE*)allocate( aSize, MEMORY_PROTECTION_READ|MEMORY_PROTECTION_WRITE );
    if( hInst == NULL ) {
      fm_unmap (  aPlugin, aPluginSize );
      fm_destroy ( &aFileMapping );
      return errNOT_ENOUGH_MEMORY;
    }
#if defined (_DEBUG)
    memcpy( hInst, szPluginName, nNameLen);
    hInst += nNameLenAligned;
#endif
    if ( aPluginSize < sizeof(PrFileHeader) ) {
      fm_unmap (  aPlugin, aPluginSize );
      fm_destroy ( &aFileMapping );
      deallocate ( (tPTR)hInst, aSize );
      return errOBJECT_READ;
    }
    memcpy( hInst, PrHeader, sizeof(PrFileHeader) );
    err = LoadSections( aPlugin, aPluginSize, hInst );
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
    fm_unmap (  aPlugin, aPluginSize );
    fm_destroy ( &aFileMapping );
  }
  EXCEPT( EXCEPTION_EXECUTE_HANDLER ) {
    err = errUNEXPECTED;
    PR_TRACE(( 0, prtERROR, "LoadPrPlugin: exception caught on \"%s\"", szPluginName) );
  }

#if defined(SHOW_START_ADDRESS)
  if ( hInst )
    ShowPluginStartAddress( szPluginName, *pEntryPoint );
#endif
#if defined (_WIN32)	
  SetErrorMode( old_error_mode );
#endif
  return err;
}

// ---
int pr_call CheckPluginByName( const tCHAR* szFileName ) 
{
  int aResult = cUNEXPECTED_ERROR;
  PrFileHeader* PrHeader = 0;
  kl_size_t aPluginSize = 0;
  struct FileMapping aFileMapping;

  if ( fm_create ( &aFileMapping, szFileName, KL_FILEMAP_READ, ahNormalAccess ) ){
    PR_TRACE( (0,prtERROR,"LoadPrPlugin: Cannot create mapping of Prague module \"%s\"", szFileName) );
    return aResult;
  }

  PrHeader = (PrFileHeader*)fm_map ( &aFileMapping, 0, sizeof ( PrFileHeader ), &aPluginSize );
  if ( !PrHeader || ( PrHeader == (PrFileHeader*)-1 ) || ( aPluginSize < sizeof ( PrFileHeader )  ) ) {
    fm_destroy ( &aFileMapping );
    PR_TRACE( (0,prtERROR,"LoadPrPlugin: Cannot map Prague module \"%s\"", szFileName ) );
    return aResult;
  }
  if(PrHeader->dwSignature == cHEADER_SIGNATURE) {
    if(PrHeader->wCPUType == cINTEL_32_CPU) {
      aResult = cPRAGUE_PLUGIN_FOUND;
      PR_TRACE( (0,prtSPAM,"Prague plugin found") );
    }
    else
      PR_TRACE( (0,prtERROR,"Bad CPU Type %X\n", PrHeader->wCPUType) );
  }
  else 
    aResult = checkNativePlugin ( PrHeader);
  fm_unmap ( PrHeader, aPluginSize );
  fm_destroy ( &aFileMapping );
  return aResult;
}

tERROR pr_call UnloadPlugin(hPLUGININSTANCE hInst)
{
  if(*((LPDWORD)hInst) == cHEADER_SIGNATURE){
    LPPrFileHeader lpHeader;
    lpHeader = (LPPrFileHeader)hInst;
    deallocate ( (void*)hInst, lpHeader->dwPluginVirtualSize );
    return errOK;
  }
  UnloadDllPlugin ( hInst );
  return errOK;
}
