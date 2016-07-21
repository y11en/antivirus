#ifndef USE_WIN32_API
#ifndef DONT_USE_WIN32_API
# Error: one of USE_WIN32_API or DONT_USE_WIN32_API global define needed !!!
#endif // DONT_USE_WIN32_API
#endif // USE_WIN32_API

#ifdef USE_WIN32_API
#include <windows.h>
#endif

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>
#include "../avepack/AvcPacker.h"
#include "../ucl/ucl.h"
#include "../shared/differr.h"
#include "../shared/common.h"
#include "../shared/types.h"
#include "../shared/crc32.h"
#include "../shared/diff.h"
#include "../avepack/fakeoop.h"
#include "../avepack/Sq_su.h"
#include "../zlib/zlib.h"
#include "../shared/prepbase.h"

// ----------------------------------------------------------------------------

// inflate mem-2-mem
static tINT UnpackZLib(tBYTE* Src, tUINT SSize, tBYTE* Dst, tUINT* DSize)
{
  z_stream zstrm;

  memset(&zstrm, 0, sizeof(zstrm));
  zstrm.next_in   = Src;
  zstrm.avail_in  = SSize;
  zstrm.next_out  = Dst;
  zstrm.avail_out = *DSize;

  if ( Z_OK != inflateInit(&zstrm) )
    return(DIFF_ERR_ZLIB);
  if ( Z_STREAM_END != inflate(&zstrm, Z_FINISH) )
    return(DIFF_ERR_ZLIB);

  *DSize = zstrm.total_out;
  return(DIFF_ERR_OK);
}

// ----------------------------------------------------------------------------

// apply diff patch
tINT ApplyDiff(tCHAR* FNOld, tCHAR* FNDiff, tCHAR* FNNew)
{
  tDIFF_HDR Hdr;
  tBYTE* Ring;
  tBYTE* M1;
  tBYTE* Data;
  tBYTE* PDiff;
  tBYTE* UDiff;
  tUINT  S1, S2, S3;
#ifdef USE_WIN32_API
  HANDLE hFile;
  tDWORD Rdt;
  tDWORD Wrt;
#else
  FILE*  F3;
#endif
  tINT   Res;
  tUINT  DSize;
  tBYTE  FS[SIGN_SIZE];

  if ( NULL == FNOld || NULL == FNNew || NULL == FNDiff )
    return(DIFF_ERR_PARAM);

  // read input base file
  if ( DIFF_ERR_OK != (Res = GetBaseFile(FNOld, &M1, &S1, NULL, &FS[0])) )
    return(Res);

  // read diff file
#ifdef USE_WIN32_API
  if ( INVALID_HANDLE_VALUE == (hFile = CreateFile(FNDiff, GENERIC_READ,
    FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) )
  {
    free(M1);
    return(DIFF_ERR_FILE);
  }
  S3 = GetFileSize(hFile, NULL);
#else
  if ( NULL == (F3 = fopen(FNDiff, "rb")) )
  {
    free(M1);
    return(DIFF_ERR_FILE);
  }
  S3 = GetFSize(F3);
#endif

  if ( S3 <= sizeof(tDIFF_HDR) )
  {
    free(M1);
    return(DIFF_ERR_CORRUPT);
  }

  if ( NULL == (PDiff = (tBYTE*)malloc(S3+16)) )
  {
    free(M1);
    return(DIFF_ERR_NOMEM);
  }

#ifdef USE_WIN32_API
  if ( !ReadFile(hFile, PDiff, S3, &Rdt, NULL) || Rdt != S3 )
  {
    free(M1);
    free(PDiff);
    CloseHandle(hFile);
    return(DIFF_ERR_FILE);
  }
  CloseHandle(hFile);
#else
  if ( 0 == fread(PDiff, S3, 1, F3) )
  {
    free(M1);
    free(PDiff);
    fclose(F3);
    return(DIFF_ERR_FILE);
  }
  fclose(F3);
#endif

  // check diff integrity
  memcpy(&Hdr, PDiff, sizeof(tDIFF_HDR));
  if ( memcmp(&Hdr.Sign[0], "DIFF", sizeof(Hdr.Sign)) )
  {
    free(M1);
    free(PDiff);
    return(DIFF_ERR_CORRUPT);
  }
  if ( DIFF_VERSION != Hdr.Version )
  {
    free(M1);
    free(PDiff);
    return(DIFF_ERR_VERSION);
  }
  if ( S3 < (Hdr.DiffPSize + sizeof(tDIFF_HDR)) )
  {
    free(M1);
    free(PDiff);
    return(DIFF_ERR_CORRUPT);
  }
  Data = &PDiff[sizeof(tDIFF_HDR)];
  if ( Hdr.DiffPCrc != ~CRC32(&Data[0], Hdr.DiffPSize, 0xFFFFFFFFL) )
  {
    free(M1);
    free(PDiff);
    return(DIFF_ERR_CORRUPT);
  }

  // check base file
  if ( Hdr.BaseSize != S1 )
  {
    free(M1);
    free(PDiff);
    return(DIFF_ERR_DATA);
  }
  if ( Hdr.BaseCrc != ~CRC32(M1, S1, 0xFFFFFFFFL) )
  {
    free(M1);
    free(PDiff);
    return(DIFF_ERR_DATA);
  }

  // build ring buffer
  S2 = (Hdr.BaseSize + Hdr.CodeSize);
  if ( NULL == (Ring = (tBYTE*)malloc(S2+16)) )
  {
    free(M1);
    free(PDiff);
    return(DIFF_ERR_NOMEM);
  }
  memcpy(&Ring[0], M1, Hdr.BaseSize);
  free(M1);

  // unpack layer-2 diff data
  if ( NULL == (UDiff = (tBYTE*)malloc(Hdr.DiffUSize+16)) )
  {
    free(Ring);
    free(PDiff);
    return(DIFF_ERR_NOMEM);
  }
  DSize = Hdr.DiffUSize;
  Res = UnpackZLib(Data, Hdr.DiffPSize, UDiff, &DSize);
  if ( DIFF_ERR_OK != Res )
  {
    free(Ring);
    free(PDiff);
    free(UDiff);
    return(Res);
  }

  free(PDiff);
  if ( DSize != Hdr.DiffUSize )
  {
    free(Ring);
    free(UDiff);
    return(DIFF_ERR_CORRUPT);
  }

  // check crc again (unpacked)
  if ( Hdr.DiffUCrc != ~CRC32(UDiff, Hdr.DiffUSize, 0xFFFFFFFFL) )
  {
    free(Ring);
    free(UDiff);
    return(DIFF_ERR_CORRUPT);
  }

  // place diff
  S2 = (Hdr.BaseSize + Hdr.CodeSize);
  Res = UCL_Decompress(&Hdr, UDiff, Hdr.DiffUSize, Ring, &S2);
  if ( UCL_E_OK != Res )
  {
    free(Ring);
    free(UDiff);
    return(DIFF_ERR_OTHER);
  }

  free(UDiff);
  
  // check size and crc
  if ( S2 != Hdr.CodeSize ||
       Hdr.CodeCrc != ~CRC32(&Ring[Hdr.BaseSize], S2, 0xFFFFFFFFL) )
  {
    free(Ring);
    return(DIFF_ERR_CORRUPT);
  }

  if ( 0 != (Hdr.Flags & DIFF_FLG_NAMEOFF2LEN) )
	  PrepBase4Diff(&Ring[Hdr.BaseSize], 1);

  // pack file if needed
  if ( 0 != (Hdr.Flags & DIFF_FLG_PACK) )
  {
    if ( 0 != PackInAvc(&Ring[Hdr.BaseSize], S2, FNNew) )
    {
      free(Ring);
      return(DIFF_ERR_AVCPACK);
    }
    free(Ring);
  }
  else
  {
    // save result
#ifdef USE_WIN32_API
    if ( INVALID_HANDLE_VALUE == (hFile = CreateFile(FNNew, GENERIC_WRITE,
      0, NULL, CREATE_ALWAYS, 0, NULL)) )
    {
      free(Ring);
      return(DIFF_ERR_FILE);
    }
    if ( !WriteFile(hFile, &Ring[Hdr.BaseSize], S2, &Wrt, NULL) || Wrt != S2 )
    {
      free(Ring);
      CloseHandle(hFile);
      return(DIFF_ERR_FILE);
    }
    CloseHandle(hFile);
#else
    if ( NULL == (F3 = fopen(FNNew, "wb")) )
    {
      free(Ring);
      return(DIFF_ERR_FILE);
    }
    if ( 0 == fwrite(&Ring[Hdr.BaseSize], S2, 1, F3) )
    {
      free(Ring);
      fclose(F3);
      return(DIFF_ERR_FILE);
    }
    fclose(F3);
#endif
    free(Ring);
  }

  // write digital sign
#ifdef USE_WIN32_API
  if ( INVALID_HANDLE_VALUE == (hFile = CreateFile(FNNew, GENERIC_WRITE,
    0, NULL, OPEN_EXISTING, 0, NULL)) )
  {
    return(DIFF_ERR_FILE);
  }
  SetFilePointer(hFile, 0, NULL, FILE_END);
  if ( !WriteFile(hFile, &Hdr.DigitalSign, SIGN_SIZE, &Wrt, NULL) ||
       Wrt != SIGN_SIZE )
  {
    CloseHandle(hFile);
    return(DIFF_ERR_FILE);
  }
  CloseHandle(hFile);
#else
  if ( NULL == (F3 = fopen(FNNew, "ab")) )
    return(DIFF_ERR_FILE);
  fseek(F3, 0, SEEK_END);
  if ( 0 == fwrite(&Hdr.DigitalSign, SIGN_SIZE, 1, F3) )
  {
    fclose(F3);
    return(DIFF_ERR_FILE);
  }
  fclose(F3);
#endif

  return(DIFF_ERR_OK);
}

// ----------------------------------------------------------------------------
