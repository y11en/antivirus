#ifdef _WIN32
# include <windows.h>    /* common pe structures only */
#else
# include <kl_types.h>
# include <kl_pe.h>
# define IMAGE_NT_HEADERS               KL_PE_NT_HEADERS
# define IMAGE_SECTION_HEADER           KL_PE_SECTION_HEADER
# define IMAGE_DOS_HEADER               KL_PE_DOS_HEADER
# define IMAGE_DOS_SIGNATURE            KL_PE_DOS_SIGNATURE
# define IMAGE_NT_SIGNATURE             KL_PE_NT_SIGNATURE
# define IMAGE_DIRECTORY_ENTRY_RESOURCE KL_PE_DIRECTORY_ENTRY_RESOURCE
# define IMAGE_DIRECTORY_ENTRY_BASERELOC KL_PE_DIRECTORY_ENTRY_BASERELOC
# define IMAGE_DIRECTORY_ENTRY_IMPORT   KL_PE_DIRECTORY_ENTRY_IMPORT
# define IMAGE_SCN_MEM_WRITE            KL_PE_SCN_MEM_WRITE
# define IMAGE_IMPORT_DESCRIPTOR	KL_PE_IMPORT_DESCRIPTOR
# define IMAGE_ORDINAL_FLAG		KL_PE_ORDINAL_FLAG32
# define _stricmp			strcmp
#endif
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "loader_import.h"
#include "dlist.h"
#include "types.h"

/* ------------------------------------------------------------------------- */

typedef struct _LDRIMP_FUNCTION {
  tBOOL  ByName;
  union {
    tCHAR* Name;
    tUINT  Ordinal;
  };
} LDRIMP_FUNCTION;

typedef struct _LDRIMP_MODULE {
  tCHAR* ModuleName;
  tUINT  NumOfFunctions;
  LDRIMP_FUNCTION Function[1];
} LDRIMP_MODULE;

static const tCHAR szLdrModKernel[]        = "kernel32.dll";
static const tCHAR szLdrGetModuleHandleA[] = "GetModuleHandleA";
static const tCHAR szLdrGetProcAddress[]   = "GetProcAddress";

/* ------------------------------------------------------------------------- */

CLdrImport::CLdrImport()
: ImpList(NULL)
{
  ImpData = NULL;
  ImpTblSize = 0;
  ImpDataSize = 0;
}

CLdrImport::~CLdrImport()
{
  Reset();
}

/* ------------------------------------------------------------------------- */

tVOID CLdrImport::Reset()
{
  if ( ImpData )
  {
    free(ImpData);
    ImpData = NULL;
    ImpTblSize = 0;
    ImpDataSize = 0;
  }

  ImpList.Clear();
}

/* ------------------------------------------------------------------------- */

tBOOL CLdrImport::GrabDataFromImage(tVOID* ImageMap)
{
  IMAGE_DOS_HEADER* DOS;
  IMAGE_NT_HEADERS* PE;
  LDRIMP_MODULE* Module;
  tBYTE* Map = (tBYTE*)(ImageMap);

  Reset();

  DOS = (IMAGE_DOS_HEADER*)(Map);
  PE  = (IMAGE_NT_HEADERS*)(Map + DOS->e_lfanew);

  /* alloc first loader entry */
  Module = (LDRIMP_MODULE*)malloc(sizeof(LDRIMP_MODULE) + sizeof(LDRIMP_FUNCTION));
  if ( !Module )
    return(cFALSE);

  /* alloc first loader entry */
  Module->ModuleName         = (tCHAR*)(szLdrModKernel);
  Module->NumOfFunctions     = 2;
  Module->Function[0].ByName = cTRUE;
  Module->Function[0].Name   = (tCHAR*)(szLdrGetModuleHandleA);
  Module->Function[1].ByName = cTRUE;
  Module->Function[1].Name   = (tCHAR*)(szLdrGetProcAddress);

  /* add first node to array */
  if ( !ImpList.AddHead(Module) )
  {
    free(Module);
    return(cFALSE);
  }

  tUINT ImpRVA = PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
  if ( !ImpRVA )
    return(cTRUE);

  IMAGE_IMPORT_DESCRIPTOR* ImpDir = (IMAGE_IMPORT_DESCRIPTOR*)(Map + ImpRVA);

  for ( ; ImpDir->FirstThunk; ImpDir++ )
  {
    tDWORD* IAT;
    tCHAR*  Name;
    tUINT   ImpVal;

    /* check module name */
    Name = (tCHAR*)(Map + ImpDir->Name);
    if ( ModulePresent(Name) )
      continue;

    /* get import address table */
    if ( ImpDir->OriginalFirstThunk )
      IAT = (tDWORD*)(Map + ImpDir->OriginalFirstThunk);
    else
      IAT = (tDWORD*)(Map + ImpDir->FirstThunk);

    /* check iat */
    if ( 0 == (ImpVal = IAT[0]) )
      continue;

    /* alloc module node */
    if ( NULL == (Module = (LDRIMP_MODULE*)malloc(sizeof(LDRIMP_MODULE))) )
      return(cFALSE);

    /* fill info */
    Module->ModuleName  = Name;
    Module->NumOfFunctions = 1;
    Module->Function[0].ByName = !(ImpVal & IMAGE_ORDINAL_FLAG);
    if ( Module->Function[0].ByName )
      Module->Function[0].Name = (tCHAR*)(Map + 2 + ImpVal);
    else
      Module->Function[0].Ordinal = (ImpVal & ~IMAGE_ORDINAL_FLAG);

    /* add module to list */
    if ( !ImpList.AddTail(Module) )
    {
      free(Module);
      return(cFALSE);
    }
  }

  return(cTRUE);
}

/* ------------------------------------------------------------------------- */

tBOOL  CLdrImport::ModulePresent(tCHAR* ModuleName)
{
  LDRIMP_MODULE** Module;
  tUINT ModNum, I;

  ModNum = ImpList.Count();
  Module = (LDRIMP_MODULE**)ImpList.Array();
  for ( I = 0; I < ModNum; I++ )
  {
    if ( !_stricmp(ModuleName, Module[I]->ModuleName) )
      return(cTRUE);
  }

  return(cFALSE);
}

/* ------------------------------------------------------------------------- */

tBYTE* CLdrImport::GetImportTable(tUINT NewImpRVA, tUINT* DataSize, tUINT* TblSize)
{
  if ( NULL != ImpData )
  {
    /* already created */
    *DataSize = ImpDataSize;
    *TblSize  = ImpTblSize;
    return(ImpData);
  }

  LDRIMP_MODULE** Module;
  tUINT ModNum, I, J;
  tUINT ImpLen;
  tUINT IatNum = 0;
  tUINT StrLen = 0;

  /* calc import size */
  ModNum = ImpList.Count();
  Module = (LDRIMP_MODULE**)ImpList.Array();
  for ( I = 0; I < ModNum; I++ )
  {
    StrLen += (tUINT)strlen(Module[I]->ModuleName) + 1;
    IatNum += Module[I]->NumOfFunctions + 1;

    for ( J = 0; J < Module[I]->NumOfFunctions; J++ )
    {
      if ( !Module[I]->Function[J].ByName )
        continue;

      StrLen += 2 + (tUINT)strlen(Module[I]->Function[J].Name) + 1;
    }
  }

  ImpLen  = ImpTblSize = (ModNum + 1) * sizeof(IMAGE_IMPORT_DESCRIPTOR);
  ImpLen += StrLen + (IatNum * sizeof(tDWORD));
  ImpDataSize = ImpLen;

  /* alloc new import */
  if ( NULL == (ImpData = (tBYTE*)malloc(ImpLen)) )
    return(NULL);

  /* fill table */
  memset(ImpData, 0, ImpLen);
  IMAGE_IMPORT_DESCRIPTOR* ImpDir = (IMAGE_IMPORT_DESCRIPTOR*)(ImpData);
  tUINT   StrOfs = ImpTblSize + (IatNum * sizeof(tDWORD));
  tCHAR*  Str    = (tCHAR*)(ImpData + StrOfs);
  tUINT   IatOfs = ImpTblSize;

  for ( I = 0; I < ModNum; ImpDir++, I++ )
  {
    tUINT Len;

    /* fill import dir */
    ImpDir->ForwarderChain = -1;
    ImpDir->FirstThunk = NewImpRVA + IatOfs;
    ImpDir->Name = NewImpRVA + StrOfs;

    /* copy library name */
    Len = (tUINT)strlen(Module[I]->ModuleName);
    memcpy(ImpData + StrOfs, Module[I]->ModuleName, Len);
    StrOfs += (Len + 1);

    /* process iat */
    tDWORD* IAT = (tDWORD*)(ImpData + IatOfs);
    IatOfs += (Module[I]->NumOfFunctions + 1) * sizeof(tDWORD);
    for ( J = 0; J < Module[I]->NumOfFunctions; J++ )
    {
      if ( !Module[I]->Function[J].ByName )
      {
        *IAT++ = (tDWORD)(Module[I]->Function[J].Ordinal | IMAGE_ORDINAL_FLAG);
        continue;
      }

      /* copy function name */
      *IAT++ = (tDWORD)(NewImpRVA + StrOfs);
      Len = (tUINT)strlen(Module[I]->Function[J].Name);
      memcpy(ImpData + 2 + StrOfs, Module[I]->Function[J].Name, Len);
      StrOfs += (2 + Len + 1);
    }
  }

  /* return new import */
  *DataSize = ImpDataSize;
  *TblSize  = ImpTblSize;
  return(ImpData);
}

/* ------------------------------------------------------------------------- */
