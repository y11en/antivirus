#ifndef USE_WIN32_API
#ifndef DONT_USE_WIN32_API
# Error: one of USE_WIN32_API or DONT_USE_WIN32_API global define needed !!!
#endif // DONT_USE_WIN32_API
#endif // USE_WIN32_API

#ifdef USE_WIN32_API
# include <windows.h>
#endif

#include "../makediff/sqze.h"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>
#include "../avepack/AvcPacker.h"
#include "../shared/differr.h"
#include "../shared/common.h"
#include "../shared/types.h"
#include "../shared/crc32.h"
#include "../shared/diff.h"
#include "../avepack/fakeoop.h"
#include "../avepack/Sq_su.h"
#include "../shared/prepbase.h"
#include "../shared/md5.h"
#include "../appldiff/adiftbl.h"
#include "../lzma/7zip/Compress/LZMA_C/LzmaDecode.h"
#include "../zlib121/zlib.h"

#include <diffs.h>

/* ------------------------------------------------------------------------- */

static tINT LzmaUnpack(tBYTE* BSrc,tUINT SrcSize,tBYTE* BDst,tUINT DstSize)
{
  tINT   lp, lc, pb;
  tBYTE* LzmaCtx;
  tUINT  CtxSize;
  tUINT  Written;
  tINT   Res;

  /* check params */
  if ( NULL == BSrc || !SrcSize || NULL == BDst || !DstSize )
    return(DIFF_ERR_PARAM);

  lc = 3;
  lp = 0;
  pb = 2;

  /* alloc lzma context */
  CtxSize = (LZMA_BASE_SIZE + (LZMA_LIT_SIZE << (lc + lp))) * sizeof(CProb);
  if ( NULL == (LzmaCtx = (tBYTE*)malloc(CtxSize + 1024 + 16)) )
    return(DIFF_ERR_NOMEM);

  /* unpack buffer */
  memset(LzmaCtx, 0, CtxSize + 1024);
  Res = LzmaDecode(LzmaCtx, CtxSize, lc, lp, pb, BSrc, SrcSize, BDst, DstSize, &Written);
  free(LzmaCtx);

  /* check result */
  if ( Res != LZMA_RESULT_OK || Written != DstSize )
    return(DIFF_ERR_CORRUPT);

  /* all ok */
  return(DIFF_ERR_OK);
}

/* ------------------------------------------------------------------------- */

/* apply diff patch */
tINT ApplyDiff(tCHAR* FNOld, tCHAR* FNDiff, tCHAR* FNNew)
{
  tDIFF_HDR Hdr;
  tBYTE  DiffHash[8];
  tBYTE* RBuf;
  tUINT  RSize;
  tBYTE* M1;
  tBYTE* Data;
  tBYTE  Sign[SIGN_SIZE];
  tBYTE* PDiff;
  tBYTE* UDiff;
  tUINT  S1, S2;
#ifdef USE_WIN32_API
  HANDLE hFile;
  tDWORD Rdt;
  tDWORD Wrt;
#else
  FILE*  F3;
#endif
  tINT   Res;
  tUINT nPackFormat = PACK_FORMAT_NUL;

  if ( NULL == FNOld || NULL == FNNew || NULL == FNDiff )
    return(DIFF_ERR_PARAM);

  /* read input base file */
  if ( DIFF_ERR_OK != (Res = GetBaseFile(FNOld, &M1, &S1, NULL, NULL, NULL, &nPackFormat)) )
    return(Res);

  /* read diff file */
#ifdef USE_WIN32_API
  if ( INVALID_HANDLE_VALUE == (hFile = CreateFile(FNDiff, GENERIC_READ,
    FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) )
  {
    free(M1);
    return(DIFF_ERR_FILE);
  }
  S2 = GetFileSize(hFile, NULL);
#else
  if ( NULL == (F3 = fopen(FNDiff, "rb")) )
  {
    free(M1);
    return(DIFF_ERR_FILE);
  }
  S2 = GetFSize(F3);
#endif

  if ( S2 <= sizeof(tDIFF_HDR) )
  {
    free(M1);
    return(DIFF_ERR_CORRUPT);
  }
  if ( NULL == (PDiff = (tBYTE*)malloc(S2+16)) )
  {
    free(M1);
    return(DIFF_ERR_NOMEM);
  }

#ifdef USE_WIN32_API
  if ( !ReadFile(hFile, PDiff, S2, &Rdt, NULL) || Rdt != S2 )
  {
    CloseHandle(hFile);
    free(PDiff);
    free(M1);
    return(DIFF_ERR_FILE);
  }
  CloseHandle(hFile);
#else
  if ( 0 == fread(PDiff, S2, 1, F3) )
  {
    free(PDiff);
    fclose(F3);
    free(M1);
    return(DIFF_ERR_FILE);
  }
  fclose(F3);
#endif

  /* check diff integrity */
  memcpy(&Hdr, PDiff, sizeof(tDIFF_HDR));
  if ( memcmp(&Hdr.Sign[0], "DIFF", 4) )
  {
    free(PDiff);
    free(M1);
    return(DIFF_ERR_CORRUPT);
  }
  if ( DIFF_VERSION != Hdr.Version )
  {
    free(PDiff);
    free(M1);
    return(DIFF_ERR_VERSION);
  }
  if ( S2 < Hdr.DiffSize )
  {
    free(PDiff);
    free(M1);
    return(DIFF_ERR_CORRUPT);
  }
  if ( Hdr.DiffCrc32 != ~CRC32(PDiff + 8, Hdr.DiffSize - 8, 0xFFFFFFFFL) )
  {
    free(PDiff);
    free(M1);
    return(DIFF_ERR_CORRUPT);
  }

  /* check base file */
  if ( Hdr.BaseSize != S1 )
  {
    free(PDiff);
    free(M1);
    return(DIFF_ERR_DATA);
  }
  CalcDiffMD5Hash(M1, S1, DiffHash);
  if ( memcmp(Hdr.BaseHash, DiffHash, sizeof(DiffHash)) )
  {
    free(PDiff);
    free(M1);
    return(DIFF_ERR_DATA);
  }

  /* alloc buffer for unpacked diff */
  if ( NULL == (UDiff = (tBYTE*)malloc(Hdr.TblUSize+16)) )
  {
    free(PDiff);
    free(M1);
    return(DIFF_ERR_NOMEM);
  }

  /* setup pointers */
  Data = &PDiff[sizeof(tDIFF_HDR)];
  if ( Hdr.Flags & DIFF_FLG_SIGN30 )
  {
    memcpy(Sign, Data, SIGN_SIZE);
    Data += SIGN_SIZE;
  }

  if ( Hdr.Flags & (DIFF_FLG_TBLPACK_LZMA | DIFF_FLG_TBLPACK_ZLIB) )
  {
    tUINT PSize;

    /* calc packed table size */
    PSize = Hdr.DiffSize - sizeof(tDIFF_HDR);
    if ( Hdr.Flags & DIFF_FLG_SIGN30 ) PSize -= SIGN_SIZE;

    /* unpack layer-2 diff data */
    memset(UDiff, 0, Hdr.TblUSize);
	if (Hdr.Flags & DIFF_FLG_TBLPACK_LZMA)
	    Res = LzmaUnpack(Data, PSize, UDiff, Hdr.TblUSize);
	else if (Hdr.Flags & DIFF_FLG_TBLPACK_ZLIB)
		Res = (Z_OK == uncompress(UDiff, &Hdr.TblUSize, Data, PSize) ? DIFF_ERR_OK : DIFF_ERR_CORRUPT);
	else
		Res = DIFF_ERR_VERSION;
		
    if ( Res != DIFF_ERR_OK )
    {
      free(UDiff);
      free(PDiff);
      free(M1);
      return(Res);
    }
  }
  else
  {
    /* use unpacked diff */
    memcpy(UDiff, Data, Hdr.TblUSize);
  }

  free(PDiff);
  PDiff = NULL;

  /* check crc again (unpacked) */
  if ( Hdr.TblUCrc32 != ~CRC32(UDiff, Hdr.TblUSize, 0xFFFFFFFFL) )
  {
    free(UDiff);
    free(M1);
    return(DIFF_ERR_CORRUPT);
  }

  /* place diff */
  Res = ApplyDiffTable(M1, S1, UDiff, Hdr.TblUSize, &RBuf, &RSize);
  free(UDiff); UDiff = NULL;
  free(M1); M1 = NULL;
  if ( DIFF_ERR_OK != Res )
    return(DIFF_ERR_OTHER);

  /* check size and crc */
  CalcDiffMD5Hash(RBuf, RSize, DiffHash);
  if ( RSize != Hdr.CodeSize || memcmp(Hdr.CodeHash, DiffHash, sizeof(DiffHash)) )
  {
    free(RBuf); RBuf = NULL;
    return(DIFF_ERR_CORRUPT);
  }

  /* apply filter */
  if ( Hdr.Flags & DIFF_FLG_NAMEOFF2LEN )
	  PrepBase4Diff(RBuf, 1);

  switch(nPackFormat)
  {
  case PACK_FORMAT_NUL: // dummy
	  break;
  case PACK_FORMAT_DLL:
  case PACK_FORMAT_KFB:
	  {
		  struct Diff_Buffer out_buf;
		  bool res = 0;
		  if (nPackFormat == PACK_FORMAT_DLL)
			  res = Diff_DLL_Pack(RBuf, RSize, &out_buf);
		  else if (nPackFormat == PACK_FORMAT_KFB)
			  res = Diff_KFB_Pack(RBuf, RSize, &out_buf);
		  free(RBuf);
		  if (!res)
			  return (DIFF_ERR_NOMEM);
		  RSize = out_buf.m_size;
		  RBuf = malloc(RSize);
		  if (RBuf)
			  memcpy(RBuf, out_buf.m_data, RSize);
		  Diff_Buffer_Free(&out_buf);
		  if (!RBuf)
			  return (DIFF_ERR_NOMEM);
	  }
  }

  // pack SQZE format
  if (RSize > sizeof(CSqueezeHeader))
  {
	  CSqueezeHeader* pSQZU = (CSqueezeHeader*)RBuf;
	  if ((pSQZU->dwSignature == SQZU_HEADER_SIGNATURE) && pSQZU->dwVersion == 1)
	  {
		  SMemFile uf;
		  SMemFile pf;
		  CSqueezeHeader* pSQZE = (CSqueezeHeader*)malloc(pSQZU->dwHeaderSize + pSQZU->dwCompressedSize);
		  if (!pSQZE)
		  {
			  free(RBuf);
			  return (DIFF_ERR_NOMEM);
		  }
		  memcpy(pSQZE, pSQZU, pSQZU->dwHeaderSize);
		  pSQZE->dwSignature = SQZE_HEADER_SIGNATURE;
		  
		  MemFileAttach(&uf, (char*)pSQZU + pSQZU->dwHeaderSize, pSQZU->dwOriginalSize);
		  MemFileAttach(&pf, (char*)pSQZE + pSQZU->dwHeaderSize, pSQZU->dwCompressedSize);
		  RSize = squeeze(&uf, &pf, pSQZU->dwOriginalSize, 0);
		  free(RBuf);
		  if ((pSQZE->dwCompressedSize != RSize)
			  || (pSQZE->dwCrc != ~CRC32((char*)pSQZE + pSQZE->dwHeaderSize, pSQZE->dwCompressedSize, 0xffffffff)))
		  {
			  free(pSQZE);
			  return (DIFF_ERR_CORRUPT);
		  }
		  RBuf = (tBYTE*)pSQZE;
		  RSize = pSQZE->dwHeaderSize + pSQZE->dwCompressedSize;
	  }
  }

  /* pack file to avc if needed */
  if ( 0 != (Hdr.Flags & DIFF_FLG_PACK) )
  {
    Res = PackInAvc(RBuf, RSize, FNNew);
    free(RBuf); RBuf = NULL;
    if ( 0 != Res )
      return(DIFF_ERR_AVCPACK);
  }
  else
  {
    // save result
#ifdef USE_WIN32_API
    if ( INVALID_HANDLE_VALUE == (hFile = CreateFile(FNNew, GENERIC_WRITE,
      0, NULL, CREATE_ALWAYS, 0, NULL)) )
    {
      free(RBuf); RBuf = NULL;
      return(DIFF_ERR_FILE);
    }
    Res = WriteFile(hFile, RBuf, RSize, &Wrt, NULL);
    CloseHandle(hFile);
    if ( !Res || Wrt != RSize )
    {
      free(RBuf); RBuf = NULL;
      return(DIFF_ERR_FILE);
    }
#else
    if ( NULL == (F3 = fopen(FNNew, "wb")) )
    {
      free(RBuf); RBuf = NULL;
      return(DIFF_ERR_FILE);
    }
    Res = fwrite(RBuf, RSize, 1, F3);
    fclose(F3);
    if ( 0 == Res )
    {
      free(RBuf); RBuf = NULL;
      return(DIFF_ERR_FILE);
    }
#endif
    free(RBuf);
    RBuf = NULL;
  }

  /* write digital sign */
  if ( Hdr.Flags & DIFF_FLG_SIGN30 )
  {
#ifdef USE_WIN32_API
    if ( INVALID_HANDLE_VALUE == (hFile = CreateFile(FNNew, GENERIC_WRITE,
      0, NULL, OPEN_EXISTING, 0, NULL)) )
    {
      return(DIFF_ERR_FILE);
    }
    SetFilePointer(hFile, 0, NULL, FILE_END);
    Res = WriteFile(hFile, &Sign[0], SIGN_SIZE, &Wrt, NULL);
    CloseHandle(hFile);
    if ( !Res || Wrt != SIGN_SIZE )
    {
      return(DIFF_ERR_FILE);
    }
#elif DONT_USE_WIN32_API
    if ( NULL == (F3 = fopen(FNNew, "ab")) )
      return(DIFF_ERR_FILE);
    fseek(F3, 0, SEEK_END);
    Res = fwrite(&Sign[0], SIGN_SIZE, 1, F3);
    fclose(F3);
    if ( 0 == Res )
    {
      return(DIFF_ERR_FILE);
    }
#else
# error "define somethig about io"
#endif
  }


  return(DIFF_ERR_OK);
}

// ----------------------------------------------------------------------------
