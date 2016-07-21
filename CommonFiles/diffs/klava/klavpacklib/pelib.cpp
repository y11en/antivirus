#ifndef _WIN32
# include <kl_types.h>
#endif
#include <memory.h>
#include "pelib.h"
#include "section_writer.h"
#include "types.h"

#ifndef _WIN32
# define IMAGE_DATA_DIRECTORY			KL_PE_DATA_DIRECTORY
# define IMAGE_DIRECTORY_ENTRY_DEBUG		KL_PE_DIRECTORY_ENTRY_DEBUG
# define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG	KL_PE_DIRECTORY_ENTRY_LOAD_CONFIG
# define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT	KL_PE_DIRECTORY_ENTRY_BOUND_IMPORT
# define IMAGE_DIRECTORY_ENTRY_IAT		KL_PE_DIRECTORY_ENTRY_IAT
# define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT	13
# define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR	14
# define IMAGE_SCN_CNT_CODE			KL_PE_SCN_CNT_CODE
# define IMAGE_SCN_CNT_INITIALIZED_DATA		KL_PE_SCN_CNT_INITIALIZED_DATA
# define IMAGE_SCN_CNT_UNINITIALIZED_DATA	KL_PE_SCN_CNT_UNINITIALIZED_DATA
# define IMAGE_SCN_MEM_EXECUTE			KL_PE_SCN_MEM_EXECUTE
# define IMAGE_SCN_MEM_READ			KL_PE_SCN_MEM_READ
#endif

#ifndef MIN_FILE_ALIGN
# define MIN_FILE_ALIGN (512)          /* minimum section alignment */
#endif
#ifndef ALIGN
# define ALIGN(X,Y)     (((X)+(Y)-1)&(~((Y)-1)))
#endif

/* ------------------------------------------------------------------------- */

CPELibrary::CPELibrary()
{
  Map = NULL;
  Overlay = NULL;
  OverlaySize = 0;
  MapSize = 0;
  OriginalHeader = NULL;
  OriginalHeaderSize = 0;
  OriginalHeaderOffs = 0;
}

CPELibrary::~CPELibrary()
{
  Reset();
}

/* ------------------------------------------------------------------------- */

tVOID CPELibrary::Reset()
{
  if ( Map )
  {
    MemFree(Map);
    MapSize = 0;
    Map = NULL;
  }
  if ( OriginalHeader )
  {
    MemFree(OriginalHeader);
    OriginalHeader = NULL;
    OriginalHeaderSize = 0;
    OriginalHeaderOffs = 0;
  }

  Overlay = NULL;
  OverlaySize = 0;
}

/* ------------------------------------------------------------------------- */

tBOOL CPELibrary::MapFromFileBuffer(const tVOID* Buffer, tUINT BufferSize)
{
  tBYTE* Src = (tBYTE*)(Buffer);
  IMAGE_SECTION_HEADER* Obj;
  IMAGE_DOS_HEADER* DOS;
  IMAGE_NT_HEADERS* PE;
  tUINT ImageSize;
  tUINT MinRawOfs;
  tUINT Offset;
  tUINT HdrLen;
  tUINT Size;
  tUINT NObj;
  tUINT FMax;
  tUINT RA;
  tUINT VA;
  tUINT I;

  Reset(); /* flush previous data */

  /* check dos header */
  if ( BufferSize <= sizeof(IMAGE_DOS_HEADER) )
    return(cFALSE);
  DOS = (IMAGE_DOS_HEADER*)(Src);
  if ( DOS->e_magic != IMAGE_DOS_SIGNATURE )
    return(cFALSE);

  /* check pe header */
  Offset = DOS->e_lfanew;
  if ( !Offset || Offset >= BufferSize )
    return(cFALSE);
  if ( (Offset + sizeof(IMAGE_NT_HEADERS)) >= BufferSize )
    return(cFALSE);
  PE = (IMAGE_NT_HEADERS*)(Src + Offset);
  if ( PE->Signature != IMAGE_NT_SIGNATURE )
    return(cFALSE);

  /* alloc image */
  ImageSize = MapSize = PE->OptionalHeader.SizeOfImage;
  if ( NULL == (Map = MemAlloc(ImageSize)) )
    return(cFALSE);

  /* copy mz/pe headers */
  memset(Map, 0, ImageSize);
  NObj = PE->FileHeader.NumberOfSections;
  Size = Offset + sizeof(PE->Signature) + PE->FileHeader.SizeOfOptionalHeader +
    sizeof(PE->FileHeader) + (NObj * sizeof(IMAGE_SECTION_HEADER));
  memcpy(Map, Src, Size);

  /* copy sections */
  MinRawOfs = (tUINT)(-1);
  Obj = GetMapPESections();
  RA = PE->OptionalHeader.FileAlignment;
  VA = PE->OptionalHeader.SectionAlignment;
  for ( FMax = 0, I = 0; I < NObj; I++ )
  {
    /* copy section to image */
    memcpy(Map + Obj[I].VirtualAddress, Src + Obj[I].PointerToRawData,
      Obj[I].SizeOfRawData);

    /* update section header */
#ifdef _WIN32
    Obj[I].Misc.VirtualSize = ALIGN(Obj[I].Misc.VirtualSize, VA);
    Obj[I].SizeOfRawData    = ALIGN(Obj[I].SizeOfRawData, RA);
#else
    Obj[I].VirtualSize = ALIGN(Obj[I].VirtualSize, VA);
    Obj[I].SizeOfRawData    = ALIGN(Obj[I].SizeOfRawData, RA);
#endif    
    Obj[I].PointerToRawData &= ~0x1ff; /* cluster size */

    /* update overlay position */
    if ( FMax < (Obj[I].PointerToRawData + Obj[I].SizeOfRawData) )
      FMax = (Obj[I].PointerToRawData + Obj[I].SizeOfRawData);

    /* update minimum raw offset */
    if ( Obj[I].PointerToRawData && Obj[I].PointerToRawData < MinRawOfs )
      MinRawOfs = Obj[I].PointerToRawData;

    /* update raw data offset */
    Obj[I].PointerToRawData = Obj[I].VirtualAddress;
  }

  /* copy last header part */
  HdrLen = DOS->e_lfanew + sizeof(PE->Signature) + sizeof(PE->FileHeader) +
    PE->FileHeader.SizeOfOptionalHeader + (PE->FileHeader.NumberOfSections *
    sizeof(IMAGE_SECTION_HEADER));
  if ( HdrLen < MinRawOfs )
    memcpy(Map + HdrLen, Src + HdrLen, (MinRawOfs - HdrLen));


  /* check for overlay */
  if ( 0 != (OverlaySize = (BufferSize - FMax)) )
  {
    Overlay = Src + FMax;
  }

  /* normalize header */
  if ( !OptimizePEHeader() )
    return(cFALSE);

  /* save original header */
  if ( !CopyOriginalHeader() )
    return(cFALSE);

  return(cTRUE);
}

/* ------------------------------------------------------------------------- */

IMAGE_NT_HEADERS* CPELibrary::GetMapPEHeader()
{
  if ( !Map )
    return(NULL);

  return (IMAGE_NT_HEADERS*)(Map + ((IMAGE_DOS_HEADER*)Map)->e_lfanew);
}

IMAGE_SECTION_HEADER* CPELibrary::GetMapPESections()
{
  IMAGE_NT_HEADERS* PE;
  tUINT Offset;

  if ( !Map )
    return(NULL);

  Offset = ((IMAGE_DOS_HEADER*)Map)->e_lfanew;
  PE = (IMAGE_NT_HEADERS*)(Map + Offset);
  Offset += sizeof(PE->Signature) + sizeof(PE->FileHeader) +
    PE->FileHeader.SizeOfOptionalHeader;

  return (IMAGE_SECTION_HEADER*)(Map + Offset);
}

/* ------------------------------------------------------------------------- */

/* remove crap from image */
tBOOL CPELibrary::NormalizeImageMap()
{
  IMAGE_DATA_DIRECTORY* Dir;
  IMAGE_NT_HEADERS* PE;
  tUINT Len;
  tUINT RA;

  if ( !(PE = GetMapPEHeader()) )
    return(cFALSE);

  Dir = &PE->OptionalHeader.DataDirectory[0];
  Len = sizeof(IMAGE_DATA_DIRECTORY);

  /* wipe unneeded directoryes */
  memset(&Dir[IMAGE_DIRECTORY_ENTRY_DEBUG],          0, Len);
  memset(&Dir[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG],    0, Len);
  memset(&Dir[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT],   0, Len);
  memset(&Dir[IMAGE_DIRECTORY_ENTRY_IAT],            0, Len);
  memset(&Dir[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT],   0, Len);
  memset(&Dir[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR], 0, Len);

  /* update raw alignment */
  RA = PE->OptionalHeader.FileAlignment;
  RA = MIN(MIN_FILE_ALIGN, RA);
  PE->OptionalHeader.FileAlignment = RA;

  return(cTRUE);
}

/* ------------------------------------------------------------------------- */

tUINT CPELibrary::GetNewHeadSize()
{
  IMAGE_DOS_HEADER* DOS;
  IMAGE_NT_HEADERS* PE;
  tUINT HeadSize;
  tUINT RA;

  DOS = (IMAGE_DOS_HEADER*)(Map);
  PE  = (IMAGE_NT_HEADERS*)(Map + DOS->e_lfanew);
  HeadSize = DOS->e_lfanew + sizeof(PE->Signature) + sizeof(PE->FileHeader) +
    PE->FileHeader.SizeOfOptionalHeader + (PE->FileHeader.NumberOfSections *
    sizeof(IMAGE_SECTION_HEADER));

  RA  = PE->OptionalHeader.FileAlignment;
  return ALIGN(HeadSize, RA);
}

/* ------------------------------------------------------------------------- */

tUINT CPELibrary::GetNewFileSize()
{
  IMAGE_SECTION_HEADER* Obj;
  IMAGE_NT_HEADERS* PE;
  tUINT FileSize;
  tUINT ObjNum;
  tUINT RA;
  tUINT I;

  /* calc headers size */
  PE = GetMapPEHeader();
  FileSize = GetNewHeadSize();

  /* scan sections */
  Obj = GetMapPESections();
  RA = PE->OptionalHeader.FileAlignment;
  ObjNum = PE->FileHeader.NumberOfSections;
  for ( I = 0; I < ObjNum; I++ )
  {
    tBYTE* ObjBuf;
    tUINT  ObjLen;

    /* check section raw size */
    ObjBuf = Map + Obj[I].PointerToRawData;
    ObjLen = Obj[I].SizeOfRawData;
    ObjLen = ALIGN(ObjLen, RA);

    /* remove padding */
    while( ObjLen && ObjBuf[ObjLen - 1] == 0 )
      ObjLen--;

    /* update file size */
    FileSize += ALIGN(ObjLen, RA);
  }

  return(FileSize + OverlaySize);
}

/* ------------------------------------------------------------------------- */

tBOOL CPELibrary::SaveMapToBuffer(tVOID* Buffer, tUINT BufferSize)
{
  IMAGE_SECTION_HEADER* Obj;
  IMAGE_NT_HEADERS* PE;
  tBYTE* Result = (tBYTE*)Buffer;
  tUINT  LastOffset;
  tUINT  ObjNum, RA;
  tUINT  BlockSize;
  tUINT  I;

  if ( !Map )
    return(cFALSE);

  /* write mz/pe headers */
  memset(Buffer, 0, BufferSize);
  LastOffset = BlockSize = GetNewHeadSize();
  if ( BlockSize ) memcpy(Result, Map, BlockSize);

  /* setup buffer pe header */
  PE  = (IMAGE_NT_HEADERS*)(Result + ((IMAGE_DOS_HEADER*)Result)->e_lfanew);
  Obj = (IMAGE_SECTION_HEADER*)((tBYTE*)PE + sizeof(PE->Signature) +
    sizeof(PE->FileHeader) + PE->FileHeader.SizeOfOptionalHeader);

  /* update headers size */
  PE->OptionalHeader.SizeOfHeaders = LastOffset;

  RA = PE->OptionalHeader.FileAlignment;
  ObjNum = PE->FileHeader.NumberOfSections;
  for ( I = 0; I < ObjNum; I++ )
  {
    tBYTE* ObjBuf;
    tUINT  ObjLen;

    /* update section raw position */
    Obj[I].PointerToRawData = LastOffset;

    /* check section raw size */
    ObjBuf = Map + Obj[I].VirtualAddress;
    ObjLen = Obj[I].SizeOfRawData;
    ObjLen = ALIGN(ObjLen, RA);

    /* remove padding */
    while( ObjLen && ObjBuf[ObjLen - 1] == 0 )
      ObjLen--;

    /* copy raw data */
    if ( 0 != (ObjLen = ALIGN(ObjLen, RA)) )
      memcpy(Result + LastOffset, Map + Obj[I].VirtualAddress, ObjLen);

    /* update section header */
    Obj[I].SizeOfRawData = ObjLen;
    LastOffset += ObjLen;
  }

  /* copy overlay */
  if ( OverlaySize )
    memcpy(Result + LastOffset, Overlay, OverlaySize);

  return(cTRUE);
}

/* ------------------------------------------------------------------------- */

tBOOL CPELibrary::AddLoaderSection(CSectionWriter* ObjWrt)
{
  IMAGE_SECTION_HEADER* Obj;
  IMAGE_NT_HEADERS* PE;
  tUINT NewMapSize;
  tUINT ObjLen;
  tUINT ObjNum;
  tUINT VA;

  if ( !Map )
    return(cFALSE);

  /* calc new sections size */
  VA = GetMapPEHeader()->OptionalHeader.SectionAlignment;
  ObjLen = ObjWrt->GetDataSize();
  ObjLen = ALIGN(ObjLen, VA);
  ObjWrt->AlignOffset(VA);

  /* realloc image */
  NewMapSize = MapSize + ObjLen;
  if ( NULL == (Map = MemRealloc(Map, NewMapSize)) )
    return(cFALSE);

  /* copy section data */
  memcpy(Map + MapSize, ObjWrt->GetDataBuffer(), ObjLen);

  /* add section header */
  Obj = GetMapPESections();
  PE  = GetMapPEHeader();
  ObjNum = PE->FileHeader.NumberOfSections;
  PE->FileHeader.NumberOfSections += 1;
  Obj += ObjNum;

  /* update pe header */
  PE->OptionalHeader.SizeOfImage += ObjLen;

  /* fill section header */
  memset(Obj, 0, sizeof(IMAGE_SECTION_HEADER));
  Obj->Name[0] = '.'; Obj->Name[1] = 'p';
  Obj->Name[2] = 'k'; Obj->Name[3] = 'l';
  Obj->Name[4] = 'a'; Obj->Name[5] = 'v';
  Obj->PointerToRawData = MapSize;
  Obj->VirtualAddress   = MapSize;
#ifdef _WIN32
  Obj->Misc.VirtualSize = ObjLen;
#else
  Obj->VirtualSize = ObjLen;
#endif
  Obj->SizeOfRawData    = ObjLen;
  Obj->Characteristics  = (IMAGE_SCN_CNT_CODE |
    IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_CNT_UNINITIALIZED_DATA |
    IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ);

  MapSize = NewMapSize;
  return(cTRUE);
}

/* ------------------------------------------------------------------------- */

tBOOL CPELibrary::CopyOriginalHeader()
{
  IMAGE_NT_HEADERS* PE;

  if ( !Map )
    return(cFALSE);

  if ( OriginalHeader )
  {
    MemFree(OriginalHeader);
    OriginalHeader = NULL;
  }

  PE = GetMapPEHeader();
  OriginalHeaderOffs = ((IMAGE_DOS_HEADER*)(Map))->e_lfanew;
  OriginalHeaderSize = sizeof(PE->Signature) + sizeof(PE->FileHeader) +
    PE->FileHeader.SizeOfOptionalHeader + ((PE->FileHeader.NumberOfSections + 1) *
    sizeof(IMAGE_SECTION_HEADER));

  if ( NULL == (OriginalHeader = MemAlloc(OriginalHeaderSize)) )
    return(cFALSE);

  memcpy(OriginalHeader, PE, OriginalHeaderSize);
  return(cTRUE);
}

/* ------------------------------------------------------------------------- */

tBYTE* CPELibrary::GetOriginalPEHeader(tUINT* HdrSize, tUINT* HdrOffs)
{
  if ( HdrSize )
    *HdrSize = OriginalHeaderSize;
  if ( HdrOffs )
    *HdrOffs = OriginalHeaderOffs;

  return(OriginalHeader);
}

/* ------------------------------------------------------------------------- */

tBOOL CPELibrary::OptimizePEHeader()
{
  IMAGE_SECTION_HEADER* Obj;
  IMAGE_NT_HEADERS* PE;
  tUINT  ObjNum, RA, VA;
  tUINT  I;

  if ( !Map )
    return(cFALSE);

  /* setup buffer pe header */
  PE  = (IMAGE_NT_HEADERS*)(Map + ((IMAGE_DOS_HEADER*)Map)->e_lfanew);
  Obj = (IMAGE_SECTION_HEADER*)((tBYTE*)PE + sizeof(PE->Signature) +
    sizeof(PE->FileHeader) + PE->FileHeader.SizeOfOptionalHeader);

  RA = PE->OptionalHeader.FileAlignment;
  VA = PE->OptionalHeader.SectionAlignment;
  ObjNum = PE->FileHeader.NumberOfSections;

  for ( I = 0; I < ObjNum; I++ )
  {
    tBYTE* ObjBuf;
    tUINT  ObjLen;

    /* check section raw size */
    ObjBuf = Map + Obj[I].VirtualAddress;
    ObjLen = Obj[I].SizeOfRawData;
    ObjLen = ALIGN(ObjLen, RA);

    /* remove padding */
    while( ObjLen && ObjBuf[ObjLen - 1] == 0 )
      ObjLen--;

    /* update section header */
    Obj[I].SizeOfRawData = ALIGN(ObjLen, RA);
#ifdef _WIN32
    Obj[I].Misc.VirtualSize = ALIGN(Obj[I].Misc.VirtualSize, VA);
#else
    Obj[I].VirtualSize = ALIGN(Obj[I].VirtualSize, VA);
#endif
  }

  return(cTRUE);
}

/* ------------------------------------------------------------------------- */
