#include <stdlib.h>
#include <memory.h>
#include <vector>
#include <lzmalib/lzma_encode.h>
#include "klavpack_encode.h"
#include "section_writer.h"
#include "loader_import.h"
#include "loader.h"
#include "pelib.h"
#include "types.h"

#ifdef _DEBUG
# include <stdio.h>
#endif

/* ------------------------------------------------------------------------- */

static bool KlavPack_CanPackObj(IMAGE_NT_HEADERS* PE, IMAGE_SECTION_HEADER* Obj)
{
  tUINT ObjRVA = Obj->VirtualAddress;
  tUINT ObjLen = Obj->SizeOfRawData;
  tUINT ResRVA = PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
  tUINT ResLen = PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size;

  if ( ResRVA >= ObjRVA && ResRVA < (ObjRVA + ObjLen) )
    return(false); /* don't pack rsrc section */

  return(true);
};

/* ------------------------------------------------------------------------- */

static bool KlavPack_EncodeSections(CPELibrary* PELib, ASMLDR_DATA* LdrData)
{
  IMAGE_SECTION_HEADER* Obj;
  IMAGE_NT_HEADERS* PE;
  std::vector<unsigned char> PackVector;
  tUINT  ObjNum, I;
  tUINT  MaxPackedSize;
  tUINT  PackedObjNum;
  tBYTE* Map;
  tINT   Res;

  PE  = PELib->GetMapPEHeader();
  Obj = PELib->GetMapPESections();
  Map = PELib->GetMap();
  MaxPackedSize = 0;
  PackedObjNum  = 0;

  /* encode sections */
  ObjNum = PE->FileHeader.NumberOfSections;
  for ( I = 0; I < ObjNum; I++ )
  {
    tBYTE* PackBuff;
    tUINT  UnpackedSize;
    tUINT  PackedSize;
    tUINT  ObjRVA;

    /* mark section as writable */
    Obj[I].Characteristics |= IMAGE_SCN_MEM_WRITE;

    if ( 0 == (UnpackedSize = Obj[I].SizeOfRawData) )
      continue; /* skip empty section */
    if ( 0 == (ObjRVA = Obj[I].VirtualAddress) )
      continue; /* skip dummy section */
    if ( !KlavPack_CanPackObj(PE, &Obj[I]) )
      continue; /* skip section */

    /* check sections count */
    if ( PackedObjNum > ASMLDR_MAX_SECTIONS )
      return(false);

    /* encode section data */
    PackVector.resize(PackedSize = UnpackedSize + 1024);
    PackBuff = &PackVector[0];
    Res = LzmaEncodeMem2Mem(Map + ObjRVA, UnpackedSize, PackBuff, &PackedSize);
    if ( LZMA_ENCODE_ERR_OK != Res )
        continue; /* fail */

#if 0
    printf("%d : USize: 0x%X, PSize: 0x%X\n", I+1, UnpackedSize, PackedSize);
#endif

    /* check result size */
    if ( PackedSize > UnpackedSize )
      continue; /* unpackable */

    /* save result to map */
    memcpy(Map + ObjRVA, PackBuff, PackedSize);
    memset(Map + ObjRVA + PackedSize, 0, UnpackedSize - PackedSize);

    /* write section info to loader */
    LdrData->Section[PackedObjNum].SectionRVA = ObjRVA;
    LdrData->Section[PackedObjNum].PackedSize = PackedSize;
    LdrData->Section[PackedObjNum].UnpackedSize = UnpackedSize;
    PackedObjNum += 1;

    /* update max packed size */
    if ( MaxPackedSize < PackedSize )
      MaxPackedSize = PackedSize;
  }

  /* write max packed size to loader */
  LdrData->MaxPackedSize = MaxPackedSize;
  return(true);
}

/* ------------------------------------------------------------------------- */

bool Do_DLL_Pack (
		const unsigned char *Source,
		size_t SourceSize,
		std::vector<unsigned char> &DestinationVector
	)
{
  CPELibrary PELib;
  CKlavPackLoader KlavLoader;

  /* map pe image from file buffer */
  if ( !PELib.MapFromFileBuffer(Source, (tUINT) SourceSize) )
    return(false);

  /* wipe crap */
  if ( !PELib.NormalizeImageMap() )
    return(false);

  /* setup loader writer */
  IMAGE_NT_HEADERS* PE = PELib.GetMapPEHeader();
  CSectionWriter ObjWrt(PE->OptionalHeader.SizeOfImage);
  tUINT LoaderObjRVA = ObjWrt.GetCurrentRVA();

  CLdrImport LdrImp;
  tBYTE* ImpData;
  tUINT  ImpDataLen;
  tUINT  ImpDirSize;
  tUINT  ImpDirRVA;

  /* create new import table */
  if ( !LdrImp.GrabDataFromImage(PELib.GetMap()) )
    return(false);
  ImpDirRVA = ObjWrt.GetCurrentRVA();
  if ( !(ImpData = LdrImp.GetImportTable(ImpDirRVA, &ImpDataLen, &ImpDirSize)) )
    return(false);

  /* write import to loader section */
  if ( !ObjWrt.WriteData(ImpData, ImpDataLen) || !ObjWrt.AlignOffset(4) )
    return(false);

  /* gen fake relocations */
  tDWORD RelTbl[2] = { 0, 0 };
  tUINT  RelDirRVA = ObjWrt.GetCurrentRVA();
  tUINT  RelTblLen = (2 * 4);
  RelTbl[0] = LoaderObjRVA;
  RelTbl[1] = RelTblLen;

  /* write fake fixups to loader section */
  if ( !ObjWrt.WriteData(&RelTbl[0], RelTblLen) || !ObjWrt.AlignOffset(4) )
    return(false);

  /* get original pe header */
  tUINT  OrgHdrUSize = 0;
  tBYTE* OrgHdrUData = PELib.GetOriginalPEHeader(&OrgHdrUSize, NULL);
  std::vector<unsigned char> HdrPack;
  unsigned int OrgHdrPSize = (OrgHdrUSize + 4096);
  HdrPack.resize(OrgHdrPSize);
  tBYTE* OrgHdrPData = &HdrPack[0];

  /* encode header */
  if ( LZMA_ENCODE_ERR_OK != LzmaEncodeMem2Mem(OrgHdrUData, OrgHdrUSize, OrgHdrPData, &OrgHdrPSize) )
    return(false);

  /* write packed header to loader */
  tUINT OrgPEHdrRVA = ObjWrt.GetCurrentRVA();
  if ( !ObjWrt.WriteData(OrgHdrPData, OrgHdrPSize) || !ObjWrt.AlignOffset(4) )
    return(false);

  tBYTE* Loader = NULL;
  tUINT  LoaderSize = 0;

  /* get loader body */
  tUINT  NewEntryRVA = ObjWrt.GetCurrentRVA();
  if ( NULL == (Loader = KlavLoader.GetLoaderBody(&LoaderSize)) )
    return(false);

  /* get loader data header */
  ASMLDR_DATA* LdrData = &((ASMLDR_HEADER*)Loader)->LdrData;

  /* fill loader data */
  LdrData->ImageBase         = PE->OptionalHeader.ImageBase;
  LdrData->OriginalEntryRVA  = PE->OptionalHeader.AddressOfEntryPoint;
  LdrData->OriginalFixUpsRVA = PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
  LdrData->OriginalFixUpsLen = PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
  LdrData->OriginalImportRVA = PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
  LdrData->MaxPackedSize     = MAX(LdrData->MaxPackedSize, OrgHdrPSize);
  LdrData->LoaderRVA         = NewEntryRVA;
  LdrData->HeaderDataRVA     = OrgPEHdrRVA;
  LdrData->HeaderDataPLen    = OrgHdrPSize;
  LdrData->HeaderDataULen    = OrgHdrUSize;

  /* encode sections */
  if ( !KlavPack_EncodeSections(&PELib, LdrData) )
    return(false);

  /* update header */
  PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress = RelDirRVA;
  PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size = RelTblLen;
  PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = ImpDirRVA;
  PE->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = ImpDirSize;
  PE->OptionalHeader.AddressOfEntryPoint = NewEntryRVA;

  /* mark file as packed */
  memcpy(&PE->FileHeader.PointerToSymbolTable, "KLAV", 4);
  memcpy(&PE->FileHeader.NumberOfSymbols,      "PACK", 4);

  /* write loader */
  if ( !ObjWrt.WriteData(Loader, LoaderSize) || !ObjWrt.AlignOffset(4) )
    return(false);

  /* embedd loader section */
  if ( !PELib.AddLoaderSection(&ObjWrt) )
    return(false);

  /* alloc file buffer */
  tUINT ResultSize = PELib.GetNewFileSize();
  DestinationVector.resize(ResultSize);
  tBYTE* Result = (tBYTE*)(&DestinationVector[0]);

  /* save result */
  if ( !PELib.SaveMapToBuffer(Result, ResultSize) )
    return(false);

  /* all ok */
  return(true);
}

/* ------------------------------------------------------------------------- */

bool KlavPack_EncodeVector(
		const std::vector<unsigned char> &SourceVector,
		std::vector<unsigned char> &DestinationVector
	)
{
	return Do_DLL_Pack (& SourceVector [0], SourceVector.size (), DestinationVector);
}

bool Diff_DLL_Pack (
		const unsigned char *unp_data,
		size_t unp_size,
		Diff_Buffer *pk_buf
	)
{
	memset (pk_buf, 0, sizeof (Diff_Buffer));

	std::vector <unsigned char> * outbuf = new std::vector <unsigned char> ();
	if (outbuf == 0)
		return false;

	if (! Do_DLL_Pack (unp_data, unp_size, *outbuf))
	{
		delete outbuf;
		return false;
	}

	pk_buf->m_data = & ((*outbuf) [0]);
	pk_buf->m_size = outbuf->size ();
	pk_buf->m_pimpl = outbuf;

	return true;
}

/* ------------------------------------------------------------------------- */
