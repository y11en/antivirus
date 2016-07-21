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
#include "../shared/md5.h"
#include "../makediff/mdiftbl.h"
#include "../lzma/lzmapack.h"
#include "../zlib121/zlib.h"

#ifndef USE_WIN32_API
#ifndef DONT_USE_WIN32_API
# Error: one of USE_WIN32_API or DONT_USE_WIN32_API global define needed !!!
#endif // DONT_USE_WIN32_API
#endif // USE_WIN32_API

#ifdef USE_WIN32_API
#undef N
# include <windows.h>
#endif

#include "../avepack/base.h"

// constants and definitions
#define MAX_RBUFF_NEEDED(X)       (X+1024*1024)
#define DIFF_TEST_FILE            "difftest.tmp"

// callback data
typedef struct sCB_INFO {
	tBOOL  First;
	tUINT  Total;
} tCB_INFO, *pCB_INFO;

#ifdef DG_DEBUG
extern tVOID DebugDumpBlock(tVOID* DTbl, tUINT DSize);
#else
# define DebugDumpBlock(DTbl, DSize);
#endif

extern void _Trace(unsigned long level, const char* format, ...);
extern void _DiffTrace(const char* format, ...);

// ----------------------------------------------------------------------------

// main processing function
tINT  MakeDiff(tCHAR* FNOld, tCHAR* FNNew, tCHAR* FNDiff, tUINT nFlags, tUINT nLimitSize, tUINT* pnResultSize)
{
	struct sDIFF_HDR Hdr; // header
#ifdef USE_WIN32_API
	HANDLE hFile;
	tDWORD Wrt;
#else
	FILE*  F3;            // diff file
#endif
	tULONG S1, S2;        // size of files
	tBYTE* DTbl;          // diff tbl buffer
	tBYTE* PDif;          // packed diff buffer
	tUINT  PSize;         // packed diff size
	tINT   Res;           // result code
	tBYTE* M1;            // base file
	tBYTE* M2;            // new file
	tBOOL  NP;            // need pack to avc
	tUINT  DSize;         // pure diff size
	tBYTE  NS[SIGN_SIZE]; // new digital sign
	tBOOL  HS;            // has sign
#if defined(USE_WIN32_API) && defined(DG_DEBUG)
	DWORD nStartTime = GetTickCount();
	DWORD nTicksRead, nTickDiff, nTicksCompress, nTicksWrite, nTicksTest;
#endif

	if (pnResultSize)
		*pnResultSize = 0;

	if ( NULL == FNOld || NULL == FNNew || NULL == FNDiff )
		return(DIFF_ERR_PARAM);
	
	// read input files
	if ( DIFF_ERR_OK != (Res = GetBaseFile(FNOld, &M1, &S1, &NP, NULL, NULL, NULL)) )
		return(Res);
	if ( DIFF_ERR_OK != (Res = GetBaseFile(FNNew, &M2, &S2, &NP, &HS, &NS[0], NULL)) )
	{
		free(M1);
		M1 = NULL;
		return(Res);
	}
	
	/* prepare diff header */
	memset(&Hdr, 0, sizeof(tDIFF_HDR));
	memcpy(&Hdr.Sign[0], DIFF_SIGNATURE, 4);
	Hdr.Version  = DIFF_VERSION;
	
	if ( NP ) Hdr.Flags |= (DIFF_FLG_PACK|DIFF_FLG_NAMEOFF2LEN);
	if ( HS ) Hdr.Flags |= (DIFF_FLG_SIGN30);
	CalcDiffMD5Hash(M1, S1, &Hdr.BaseHash[0]);
	CalcDiffMD5Hash(M2, S2, &Hdr.CodeHash[0]);
	Hdr.BaseSize = S1;
	Hdr.CodeSize = S2;

#if defined(USE_WIN32_API) && defined(DG_DEBUG)
	nTicksRead = GetTickCount();
#endif

	/* create diff table */
	Res = MakeDiffTable(M1, S1, M2, S2, &DTbl, &DSize);
	free(M2); free(M1); M1 = NULL; M2 = NULL;
	if ( DIFF_ERR_OK != Res )
		return(Res);
	
	//DebugDumpBlock(DTbl, DSize);
	
	/* update diff header */
	Hdr.TblUSize  = DSize;
	Hdr.TblUCrc32 = ~CRC32(DTbl, DSize, 0xFFFFFFFFL);
	
#ifdef DG_DEBUG
	_DiffTrace( "Info: Diff-Tables created, size: %u bytes\n", DSize);
#endif
	
#if defined(USE_WIN32_API) && defined(DG_DEBUG)
	nTickDiff = GetTickCount();
#endif

#ifndef DIFF_SKIP_TABLEPACK
	if (nFlags & (DIFF_FLG_TBLPACK_LZMA | DIFF_FLG_TBLPACK_ZLIB))
	{
		/* alloc pack buffer */
		//PSize = (DSize + 1024*1024);
		PSize = DSize;
		if ( NULL == (PDif = (tBYTE*)malloc(PSize + 16)) )
		{
			free(DTbl); DTbl = NULL;
			return(DIFF_ERR_NOMEM);
		}
		
		//	  {
		//		  tUINT _lzma, _zlib;
		//		  _lzma = PSize;
		//		  _zlib = PSize;
		//		  Res = LzmaPackMem2Mem(DTbl, DSize, PDif, &_lzma);
		//		  Res = (Z_OK == compress2(PDif, &_zlib, DTbl, DSize, 9) ? DIFF_ERR_OK : DIFF_ERR_PACK);
		//		  _Trace( 0, "Compressed Diff-Tables size: %u bytes LZMA, %u bytes ZLIB\n", _lzma, _zlib);
		//	  }
		
		/* compress data */
		if (nFlags & DIFF_FLG_TBLPACK_LZMA)
		{
			/* pack diff tables by lzma */
			Hdr.Flags |= DIFF_FLG_TBLPACK_LZMA;
			Res = LzmaPackMem2Mem(DTbl, DSize, PDif, &PSize);
		}
		else if (nFlags & DIFF_FLG_TBLPACK_ZLIB)
		{
			/* pack diff tables by zlib */
			Hdr.Flags |= DIFF_FLG_TBLPACK_ZLIB;
			Res = (Z_OK == compress(PDif, &PSize, DTbl, DSize) ? DIFF_ERR_OK : DIFF_ERR_PACK);
		}
		else
			Res = DIFF_ERR_PARAM;
		
		free(DTbl); DTbl = NULL;
		if ( Res != DIFF_ERR_OK )
		{
			free(PDif); PDif = NULL;
			return(Res);
		}
#ifdef DG_DEBUG
		_DiffTrace( "Info: Compressed Diff-Tables size: %u bytes\n", PSize);
#endif
	}
#else /* !DIFF_SKIP_TABLEPACK */
	/* use unpacked tables */
#ifdef DG_DEBUG
	_DiffTrace( "Info: Diff-Tables compression skipped\n");
#endif
	PSize = DSize;
	PDif  = DTbl;
  }
#endif
  
  /* update header */
  Hdr.DiffSize = (PSize + sizeof(tDIFF_HDR));
  if ( Hdr.Flags & DIFF_FLG_SIGN30 ) Hdr.DiffSize += SIGN_SIZE;
  Hdr.DiffCrc32 = CRC32((tBYTE*)(&Hdr) + 8, sizeof(tDIFF_HDR) - 8, 0xFFFFFFFF);
  if ( Hdr.Flags & DIFF_FLG_SIGN30 )
	  Hdr.DiffCrc32 = CRC32(&NS[0], SIGN_SIZE, Hdr.DiffCrc32);
  Hdr.DiffCrc32 = ~CRC32(PDif, PSize, Hdr.DiffCrc32);
  
  if (nLimitSize && Hdr.DiffSize >= nLimitSize)
  {
	  _DiffTrace( "Error: diff size %d >= limit size %d\n", Hdr.DiffSize, nLimitSize);
	  free(PDif);
	  return DIFF_ERR_SIZE;
  }

#if defined(USE_WIN32_API) && defined(DG_DEBUG)
	nTicksCompress = GetTickCount();
#endif

  /* create output file */
#ifdef USE_WIN32_API
  if ( INVALID_HANDLE_VALUE == (hFile = CreateFile(FNDiff, GENERIC_WRITE,
	  0, NULL, CREATE_ALWAYS, 0, NULL)) )
  {
	  free(PDif); PDif = NULL;
	  return(DIFF_ERR_FILE);
  }
  SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
  if ( cFALSE == WriteFile(hFile, &Hdr, sizeof(tDIFF_HDR), &Wrt, NULL) )
  {
	  CloseHandle(hFile);
	  DeleteFile(hFile);
	  free(PDif); PDif = NULL;
	  return(DIFF_ERR_FILE);
  }
  if ( (Hdr.Flags & DIFF_FLG_SIGN30) &&
	  cFALSE == WriteFile(hFile, &NS[0], SIGN_SIZE, &Wrt, NULL) )
  {
	  CloseHandle(hFile);
	  DeleteFile(hFile);
	  free(PDif); PDif = NULL;
	  return(DIFF_ERR_FILE);
  }
  if ( cFALSE == WriteFile(hFile, PDif, PSize, &Wrt, NULL) )
  {
	  CloseHandle(hFile);
	  DeleteFile(FNDiff);
	  free(PDif); PDif = NULL;
	  return(DIFF_ERR_FILE);
  }
  CloseHandle(hFile);
#else
  if ( NULL == (F3 = fopen(FNDiff, "wb")) )
  {
	  free(PDif); PDif = NULL;
	  return(DIFF_ERR_FILE);
  }
  fseek(F3, 0, SEEK_SET);
  if ( 0 == fwrite(&Hdr, sizeof(tDIFF_HDR), 1, F3) )
  {
	  fclose(F3);
	  unlink(FNDiff);
	  free(PDif); PDif = NULL;
	  return(DIFF_ERR_FILE);
  }
  if ( (Hdr.Flags & DIFF_FLG_SIGN30) &&
	  0 == fwrite(&NS[0], SIGN_SIZE, 1, F3) )
  {
	  fclose(F3);
	  unlink(FNDiff);
	  free(PDif); PDif = NULL;
	  return(DIFF_ERR_FILE);
  }
  if ( 0 == fwrite(PDif, PSize,  1, F3) )
  {
	  fclose(F3);
	  unlink(FNDiff);
	  free(PDif); PDif = NULL;
	  return(DIFF_ERR_FILE);
  }
  fclose(F3);
#endif
  
  free(PDif);
  PDif = NULL;
  
#if defined(USE_WIN32_API) && defined(DG_DEBUG)
	nTicksWrite = GetTickCount();
#endif

	// test unpack
//  _Trace( 0, "Test unpacking... ");
  if ( DIFF_ERR_OK != (Res = ApplyDiff(FNOld, FNDiff, DIFF_TEST_FILE)) )
  {
#ifdef USE_WIN32_API
	  DeleteFile(FNDiff);
	  DeleteFile(DIFF_TEST_FILE);
#else
	  unlink(FNDiff);
	  unlink(DIFF_TEST_FILE);
#endif
	  _DiffTrace( "Error: Unpacking test failed\n");
	  return(Res);
  }
  else
  {
#ifndef DG_DEBUG
#ifdef USE_WIN32_API
	  DeleteFile(DIFF_TEST_FILE);
#else
	  unlink(DIFF_TEST_FILE);
#endif
#endif /* DG_DEBUG */
//	  _Trace( 0, "Ok\n");
  }
#if defined(USE_WIN32_API) && defined(DG_DEBUG)
	nTicksTest = GetTickCount();
	_DiffTrace( "Info: Times read=%d, diff=%d, compr=%d, write=%d, test=%d\n", 
		nTicksRead-nStartTime, 
		nTickDiff-nTicksRead, 
		nTicksCompress-nTickDiff, 
		nTicksWrite-nTicksCompress, 
		nTicksTest-nTicksWrite);
#endif

	if (pnResultSize)
	  *pnResultSize = Hdr.DiffSize;  
  return(DIFF_ERR_OK);
}

// ----------------------------------------------------------------------------

