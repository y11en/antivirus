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
#include "../zlib/zlib.h"

#ifndef USE_WIN32_API
#ifndef DONT_USE_WIN32_API
# Error: one of USE_WIN32_API or DONT_USE_WIN32_API global define needed !!!
#endif // DONT_USE_WIN32_API
#endif // USE_WIN32_API

#ifdef USE_WIN32_API
#undef N
# include <windows.h>
#endif

// constants and definitions
#define MAX_RBUFF_NEEDED(X)       (X+1024*1024)
#define DIFF_TEST_FILE            "difftest.tmp"

// callback data
typedef struct sCB_INFO {
  tBOOL  First;
  tUINT  Total;
} tCB_INFO, *pCB_INFO;

// drop debug
// #define DG_DEBUG

#ifdef DG_DEBUG
  static tUINT DmpCnt = 0;
  static DebugDumpBlock(tVOID* Buff, tUINT Cnt)
  {
    FILE* DF;
    tCHAR Name[1024];

    sprintf(&Name[0], "debdmp%2.2u.tmp", DmpCnt++);
    if ( NULL != (DF = fopen(Name, "wb")) )
    {
      fwrite(Buff, Cnt, 1, DF);
      fclose(DF);
    }
  }
#else
#  define DebugDumpBlock(a,b)
#endif

// ----------------------------------------------------------------------------

// compression callback 
tVOID CallBack(tUINT Rdt, tUINT Wrt, tINT X, tVOID* User)
{
  tCB_INFO* Info = (tCB_INFO*)(User);

  if ( cFALSE != Info->First )
  {
    Info->First = (cFALSE);
    printf("Processing data...\n");
  }

  printf("Rdt: %u (of %u total) :: Wrt: %u\r", Rdt, Info->Total, Wrt);
}

// ----------------------------------------------------------------------------

// creade diff from ring buffer
static tINT PackDiff(tBYTE* Buff, tUINT BuffSize, tBYTE* Res, tUINT* ResSize,
                     tUINT CodeOffs, tUINT* Delta)
{
  tUCL_CALLBACK CB;     // ucl callback
  tCB_INFO  CBInf;      // callback data
  tINT Result;

  memset(&CB, 0, sizeof(CB));
  memset(&CBInf, 0, sizeof(CBInf));
  CBInf.First = (cTRUE);
  CBInf.Total = BuffSize;
  CB.CallBack = CallBack;
  CB.User     = (tVOID*)(&CBInf);

  // create diff thunk (slow!)
  Result = UCL_Compress(Buff, BuffSize, Res, ResSize, CodeOffs, Delta, &CB);

  // convert result code
  if ( UCL_E_OK == Result )
    Result = (DIFF_ERR_OK);
  else
    Result = (DIFF_ERR_OTHER);

  return(Result);
}

// ----------------------------------------------------------------------------

// deflate mem-2-mem
static tINT PackZLib(tBYTE* Src, tUINT SSize, tBYTE* Dst, tUINT* DSize)
{
  z_stream zstrm;

  // init zlib context
  memset(&zstrm, 0, sizeof(zstrm));
  zstrm.next_in   = Src;
  zstrm.avail_in  = SSize;
  zstrm.next_out  = Dst;
  zstrm.avail_out = *DSize;

  // compress stream
  if ( Z_OK != deflateInit(&zstrm, 9) )
    return(DIFF_ERR_ZLIB);
  if ( Z_STREAM_END != deflate(&zstrm, Z_FINISH) )
    return(DIFF_ERR_ZLIB);

  *DSize = zstrm.total_out;
  return(DIFF_ERR_OK);
}

// ----------------------------------------------------------------------------

// main processing function
tINT  MakeDiff(tCHAR* FNOld, tCHAR* FNNew, tCHAR* FNDiff)
{
  struct sDIFF_HDR Hdr; // header
#ifdef USE_WIN32_API
  HANDLE hFile;
  tDWORD Wrt;
#else
  FILE*  F3;            // diff file
#endif
  tULONG S1, S2, S3;    // size of files
  tBYTE* Ring;          // ring buffer
  tBYTE* RBuf;          // result buffer
  tINT   Res;           // result code
  tBYTE* M1;            // base file
  tBYTE* M2;            // new file
  tBOOL  NP;            // need pack to avc
  tBYTE* UBuff;         // unpacked diff buff
  tBYTE* PBuff;         // packed diff buff
  tUINT  PMax;          // pack buff size
  tUINT  DSize;         // pure diff size
  tBYTE  NS[SIGN_SIZE]; // new digital sign

  if ( NULL == FNOld || NULL == FNNew || NULL == FNDiff )
    return(DIFF_ERR_PARAM);

  // read input files
  if ( DIFF_ERR_OK != (Res = GetBaseFile(FNOld, &M1, &S1, &NP, &NS[0])) )
    return(Res);
  memset(&NS[0], 0, SIGN_SIZE);
  if ( DIFF_ERR_OK != (Res = GetBaseFile(FNNew, &M2, &S2, &NP, &NS[0])) )
  {
    free(M1);
    return(Res);
  }
  
  if (S1 == S2 && memcmp(M1, M2, S1) == 0)
  {
    printf("Files are identical\n\n");
    free(M2);
    free(M1);
    return(DIFF_ERR_FILE);
  }

  // check size
  if ( (S1+S2) > MAX_THUNK_SIZE )
  {
    free(M2);
    free(M1);
    return(DIFF_ERR_SIZE);
  }

  // alloc window
  if ( NULL == (Ring = (tBYTE*)malloc(S1+S2+16)) )
  {
    free(M2);
    free(M1);
    return(DIFF_ERR_NOMEM);
  }

  // create ring buffer
  memcpy(&Ring[0], M1, S1);
  memcpy(&Ring[S1],M2, S2);

  free(M2);
  free(M1);

  // alloc result buffer
  S3 = MAX_RBUFF_NEEDED(S1+S2);
  if ( NULL == (RBuf = (tBYTE*)malloc(S3)) )
  {
    free(Ring);
    return(DIFF_ERR_NOMEM);
  }

  // prepare diff header
  memset(&Hdr, 0, sizeof(tDIFF_HDR));
  memcpy(&Hdr.Sign[0], "DIFF", sizeof(Hdr.Sign));
  memcpy(&Hdr.DigitalSign[0], &NS[0], SIGN_SIZE);
  Hdr.Version  = DIFF_VERSION;
  Hdr.BaseSize = S1;
  Hdr.CodeSize = S2;
  Hdr.BaseCrc  = ~CRC32(&Ring[0], S1, 0xFFFFFFFFL);
  Hdr.CodeCrc  = ~CRC32(&Ring[S1],S2, 0xFFFFFFFFL);
  Hdr.Flags   |= ((cFALSE != NP) ? (DIFF_FLG_PACK) : (0));
  Hdr.Flags   |= DIFF_FLG_NAMEOFF2LEN;
  DSize        = S3;

  // create diff thunk (slow!)
  Res = PackDiff(Ring, (S1+S2), RBuf, &DSize, S1, &Hdr.Delta);
  if ( DIFF_ERR_OK != Res )
  {
    free(RBuf);
    free(Ring);
    return(Res);
  }

  // update diff header
  Hdr.DiffUSize = (DSize + Hdr.Delta);
  if ( NULL == (UBuff = (tBYTE*)malloc(Hdr.DiffUSize + 16)) )
  {
    free(RBuf);
    free(Ring);
    return(DIFF_ERR_NOMEM);
  }

  // create diff data block
  memcpy(UBuff, &Ring[S1], Hdr.Delta);
  memcpy(&UBuff[Hdr.Delta], RBuf, DSize);
  free(RBuf);
  free(Ring);

  DebugDumpBlock(UBuff, Hdr.DiffUSize);

  // update unpacked diff crc
  Hdr.DiffUCrc = ~CRC32(UBuff, Hdr.DiffUSize, 0xFFFFFFFFL);

  // alloc layer-2 buffer
  PMax = MAX_RBUFF_NEEDED(Hdr.DiffUSize);
  if ( NULL == (PBuff = (tBYTE*)malloc(PMax + 16)) )
  {
    free(UBuff);
    return(DIFF_ERR_NOMEM);
  }

  printf("\nPacking layer 2... ");
  Hdr.DiffPSize = PMax;
  Res = PackZLib(UBuff, Hdr.DiffUSize, PBuff, &Hdr.DiffPSize);
  if ( DIFF_ERR_OK != Res )
  {
    printf("Failed\n\n");
    free(PBuff);
    free(UBuff);
    return(Res);
  }
  else
  {
    printf("Ok, NewSize: %u\n\n", Hdr.DiffPSize);
  }

  free(UBuff);
  DebugDumpBlock(PBuff, Hdr.DiffPSize);

  // update packed diff crc-32
  Hdr.DiffPCrc = ~CRC32(PBuff, Hdr.DiffPSize, 0xFFFFFFFFL);

  // create output file
#ifdef USE_WIN32_API
  if ( INVALID_HANDLE_VALUE == (hFile = CreateFile(FNDiff, GENERIC_WRITE,
    0, NULL, CREATE_ALWAYS, 0, NULL)) )
  {
    return(DIFF_ERR_FILE);
  }
  SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
  if ( cFALSE == WriteFile(hFile, &Hdr, sizeof(tDIFF_HDR), &Wrt, NULL) )
  {
    CloseHandle(hFile);
    DeleteFile(hFile);
    free(PBuff);
    return(DIFF_ERR_FILE);
  }
  if ( cFALSE == WriteFile(hFile, PBuff, Hdr.DiffPSize, &Wrt, NULL) )
  {
    CloseHandle(hFile);
    DeleteFile(FNDiff);
    free(PBuff);
    return(DIFF_ERR_FILE);
  }
  CloseHandle(hFile);
#else
  if ( NULL == (F3 = fopen(FNDiff, "wb")) )
  {
    free(PBuff);
    return(DIFF_ERR_FILE);
  }

  // write diff
  fseek(F3, 0, SEEK_SET);
  if ( 0 == fwrite(&Hdr, sizeof(tDIFF_HDR), 1, F3) ||
       0 == fwrite(&PBuff, Hdr->DiffPSize,  1, F3) )
  {
    fclose(F3);
    free(PBuff);
    unlink(FNDiff);
    return(DIFF_ERR_FILE);
  }
  fclose(F3);
#endif

  // test unpack
  printf("\nTest unpacking... ");
  if ( DIFF_ERR_OK != (Res = ApplyDiff(FNOld, FNDiff, DIFF_TEST_FILE)) )
  {
#ifdef USE_WIN32_API
	DeleteFile(FNDiff);
    DeleteFile(DIFF_TEST_FILE);
#else
	unlink(FNDiff);
    unlink(DIFF_TEST_FILE);
#endif
    printf("Failed\n");
    return(Res);
  }
  else
  {
#ifdef USE_WIN32_API
    DeleteFile(DIFF_TEST_FILE);
#else
    unlink(DIFF_TEST_FILE);
#endif
    printf("Ok\n");
  }

  return(DIFF_ERR_OK);
}

// ----------------------------------------------------------------------------

