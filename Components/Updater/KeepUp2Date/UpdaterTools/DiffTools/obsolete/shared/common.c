#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>
#include "differr.h"
#include "common.h"
#include "types.h"
#include "crc32.h"
#include "../avepack/AvcPacker.h"
#include "prepbase.h"

#ifndef USE_WIN32_API
#ifndef DONT_USE_WIN32_API
# Error: one of USE_WIN32_API or DONT_USE_WIN32_API global define needed !!!
#endif // DONT_USE_WIN32_API
#endif // USE_WIN32_API

#ifdef USE_WIN32_API
# include <windows.h>
#endif

// ----------------------------------------------------------------------------

// calc file size
tDWORD GetFSize(FILE* F)
{
  tDWORD Cur, Res;

  Cur = ftell(F);
  fseek(F, 0, SEEK_END);
  Res = ftell(F);
  fseek(F, Cur, SEEK_SET);

  return(Res);
}

// ----------------------------------------------------------------------------

// open and read file to memory, unpack if needed
tINT GetBaseFile(tCHAR* FName, tBYTE** Buff, tUINT* Size,
                 tBOOL* Pack,  tBYTE* Sign)
{
  tBOOL  NPack;
  tUINT  FS;
#ifdef USE_WIN32_API
  HANDLE hFile;
  tDWORD Rdt;
#else
  FILE*  BF;
#endif
  tINT   Res;
  tBYTE* Mem;

#ifdef USE_WIN32_API
  if ( INVALID_HANDLE_VALUE == (hFile = CreateFile(FName, GENERIC_READ,
    FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) )
  {
    return(DIFF_ERR_FILE);
  }
  FS = GetFileSize(hFile, NULL);
#else
  if ( NULL == (BF = fopen(FName, "rb")) )
    return(DIFF_ERR_FILE);
  FS = GetFSize(BF);
#endif
    
  if ( NULL != Sign )
  {
    // read digital sign
    if ( FS <= SIGN_SIZE )
    {
#ifdef USE_WIN32_API
      CloseHandle(hFile);
#else
      fclose(BF);
#endif
      return(DIFF_ERR_OTHER);
    }

#ifdef USE_WIN32_API
    SetFilePointer(hFile, (FS-SIGN_SIZE), NULL, FILE_BEGIN);
    if ( !ReadFile(hFile, &Sign[0], SIGN_SIZE, &Rdt, NULL) || Rdt!=SIGN_SIZE )
    {
      CloseHandle(hFile);
      return(DIFF_ERR_FILE);
    }
#else
    fseek(BF, (FS-SIGN_SIZE), SEEK_SET);
    if ( 0 == fread(Sign, SIGN_SIZE, 1, BF) )
    {
      fclose(BF);
      return(DIFF_ERR_FILE);
    }
#endif

    FS -= (SIGN_SIZE);
  }

#ifdef USE_WIN32_API
  CloseHandle(hFile);
#else
  fclose(BF);
#endif

  // try unpack file
  if ( 0 != (Res = UnpackAvc(FName, Buff, Size)) )
  {
    if ( 2 != Res ) // error
    {
#ifdef DG_DEBUG
      printf("Error %d while unpacking base\n", Res);
#endif
      if ( ERR_AVP_BLOCKHEADER_CORRUPTED == Res )
        return(DIFF_ERR_BAD16);
      return(DIFF_ERR_AVCPACK);
    }

#ifdef DG_DEBUG
    printf("Used already unpacked base\n");
#endif

    // try read file
#ifdef USE_WIN32_API
    if ( INVALID_HANDLE_VALUE == (hFile = CreateFile(FName, GENERIC_READ,
      FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) )
    {
      return(DIFF_ERR_FILE);
    }
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
#else
    if ( NULL == (BF = fopen(FName, "rb")) )
      return(DIFF_ERR_FILE);
    fseek(BF, 0, SEEK_SET);
#endif

    if ( NULL == (Mem = (tBYTE*)malloc(FS+16)) )
    {
#ifdef USE_WIN32_API
      CloseHandle(hFile);
#else
      fclose(BF);
#endif
      return(DIFF_ERR_NOMEM);
    }

#ifdef USE_WIN32_API
    if ( cFALSE == ReadFile(hFile, Mem, FS, &Rdt, NULL) || Rdt != FS )
    {
      free(Mem);
      CloseHandle(hFile);
      return(DIFF_ERR_FILE);
    }
    CloseHandle(hFile);
#else
    if ( 0 == fread(Mem, FS, 1, BF) )
    {
      free(Mem);
      fclose(BF);
      return(DIFF_ERR_FILE);
    }
    fclose(BF);
#endif

    *Size = FS;
    NPack = (cFALSE);
  }
  else
  {

	PrepBase4Diff(*Buff, 0);
#ifdef DG_DEBUG
    printf("Used base after unpacking\n");
    DebugDumpBlock(*Buff, *Size);
#endif

    // unpacked file
    NPack = (cTRUE);
    if ( NULL == (Mem = (tBYTE*)malloc(*Size + 16)) )
    {
      FreeBuffer(*Buff);
      return(DIFF_ERR_NOMEM);
    }
    memcpy(Mem, *Buff, *Size);
    FreeBuffer(*Buff);
  }

  if ( NULL != Pack )
    *Pack = (NPack);

  *Buff = Mem;
  return(DIFF_ERR_OK);
}

// ----------------------------------------------------------------------------
