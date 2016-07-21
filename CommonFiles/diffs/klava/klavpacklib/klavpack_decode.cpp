#include <memory.h>
#include <vector>
#include <lzmalib/lzma_decode.h>
#include "klavpack_decode.h"
#include "loader.h"
#include "pelib.h"
#include "types.h"

/* -------------------------------------------------------------------------- */

bool KlavPack_IsPacked(
			const std::vector<unsigned char> &SourceVector
		)
{
	return Diff_DLL_IsPacked (&SourceVector [0], SourceVector.size ());
}

bool Diff_DLL_IsPacked (
			const unsigned char *Source,
			size_t SourceSize
		)
{
  IMAGE_DOS_HEADER* DOS;
  IMAGE_NT_HEADERS* PE;
  tUINT  Offs;

  /* check header size */
  if ( SourceSize <= sizeof(IMAGE_DOS_HEADER) )
    return(false);
  DOS = (IMAGE_DOS_HEADER*)(Source);
  if ( DOS->e_magic != IMAGE_DOS_SIGNATURE )
    return(false);
  Offs = (tUINT)(DOS->e_lfanew);
  if ( Offs >= SourceSize || (Offs + sizeof(IMAGE_NT_HEADERS)) >= SourceSize )
    return(false);
  PE = (IMAGE_NT_HEADERS*)(Source + Offs);
  if ( PE->Signature != IMAGE_NT_SIGNATURE )
    return(false);

  /* check marker */
  if ( memcmp(&PE->FileHeader.PointerToSymbolTable, "KLAV", 4) ||
       memcmp(&PE->FileHeader.NumberOfSymbols,      "PACK", 4) )
  {
    return(false);
  }

  /* packed */
  return(true);
}

/* -------------------------------------------------------------------------- */

static bool KlavPack_DecodeSections(CPELibrary* PELib, ASMLDR_DATA* LdrData)
{
  std::vector<unsigned char> UnpackVector;
  ASMLDR_SECTION* Section;
  tBYTE* UnpackBuff;
  tUINT  SrcLen;
  tBYTE* DstBuf;
  tBYTE* Map;
  tUINT  RVA;
  tINT   Res;

  Map = PELib->GetMap();
  UnpackVector.resize(LdrData->MaxPackedSize);
  UnpackBuff = (tBYTE*)(&UnpackVector[0]);

  /* decode sections */
  for ( Section = &LdrData->Section[0]; RVA = Section->SectionRVA; Section++ )
  {
    /* copy packed data */
    SrcLen = Section->PackedSize;
    DstBuf = (Map + RVA);
    memcpy(UnpackBuff, DstBuf, SrcLen);

    /* decode section data */
    Res = LzmaDecodeMem2Mem(UnpackBuff, SrcLen, DstBuf, Section->UnpackedSize);
    if ( LZMA_DECODE_ERR_OK != Res )
      return(false); /* fail */
  }

  return(true);
}

/* ------------------------------------------------------------------------- */

bool Do_DLL_Unpack (
			const unsigned char * Source,
			size_t SourceSize,
			std::vector <unsigned char>& DestinationVector
		)
{
  CPELibrary PELib;

  ASMLDR_HEADER*  LdrHdr;
  ASMLDR_DATA*    LdrData;
  tBYTE* Map;
  tUINT  MapSize;
  tUINT  RVA;
  tUINT  Len;

  /* map pe image from file buffer */
  if ( !PELib.MapFromFileBuffer(Source, (tUINT) SourceSize) )
    return(false);

  /* find loader */
  Map = PELib.GetMap();
  MapSize = PELib.GetMapSize();
  IMAGE_NT_HEADERS* PE = PELib.GetMapPEHeader();
  RVA = PE->OptionalHeader.AddressOfEntryPoint;
  if ( RVA >= MapSize || (RVA + sizeof(ASMLDR_HEADER)) >= MapSize )
    return(false);
  LdrHdr = (ASMLDR_HEADER*)(Map + RVA);
  if ( LdrHdr->HdrDeltaSize != sizeof(ASMLDR_DATA) )
    return(false); /* wrong context size */
  LdrData = &LdrHdr->LdrData;
  if ( LdrData->LoaderVersion != 1 )
    return(false); /* wrong loader version */

  /* wipe header */
  RVA = ((IMAGE_DOS_HEADER*)Map)->e_lfanew;
  Len = sizeof(PE->Signature) + PE->FileHeader.SizeOfOptionalHeader +
    sizeof(PE->FileHeader) + (PE->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER));
  memset(Map + RVA, 0, Len);

  /* unpack original header */
  if ( LZMA_DECODE_ERR_OK != LzmaDecodeMem2Mem(Map + LdrData->HeaderDataRVA,
    LdrData->HeaderDataPLen, Map + RVA, LdrData->HeaderDataULen) )
  {
    /* fail */
    return(false);
  }

  /* unpack sections */
  if ( !KlavPack_DecodeSections(&PELib, LdrData) )
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

bool KlavPack_DecodeVector(
		const std::vector<unsigned char> &SourceVector,
		std::vector<unsigned char> &DestinationVector
	)
{
	return Do_DLL_Unpack (& SourceVector [0], SourceVector.size (), DestinationVector);
}

bool Diff_DLL_Unpack (
		const unsigned char *pk_data,
		size_t pk_size,
		Diff_Buffer *unp_buf
	)
{
	memset (unp_buf, 0, sizeof (Diff_Buffer));

	std::vector <unsigned char> * outbuf = new std::vector <unsigned char> ();
	if (outbuf == 0)
		return false;

	if (! Do_DLL_Unpack (pk_data, pk_size, *outbuf))
	{
		delete outbuf;
		return false;
	}

	unp_buf->m_data = & ((*outbuf) [0]);
	unp_buf->m_size = outbuf->size ();
	unp_buf->m_pimpl = outbuf;

	return true;
}

/* ------------------------------------------------------------------------- */
