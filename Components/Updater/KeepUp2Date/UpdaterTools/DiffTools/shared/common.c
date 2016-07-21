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

#include "../makediff/sqze.h"
#include "../avepack/sq_su.h"
#include <diffs.h>

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

/* ------------------------------------------------------------------------- */

#ifdef DG_DEBUG
static tUINT DmpCnt = 0;
tVOID DebugDumpBlock(tVOID* Buff, tUINT Cnt)
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

/* ------------------------------------------------------------------------- */

// open and read file to memory, unpack if needed
tINT GetBaseFile(tCHAR* FName, tBYTE** Buff, tUINT* Size,
                 tBOOL* Pack,  tBOOL* HasSign, tBYTE Sign[SIGN_SIZE], tUINT* pnPackFormat)
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
	tUINT  I;
	tUINT  nPackFormat = PACK_FORMAT_NUL;
	
	if ( NULL != HasSign )
		*HasSign = cFALSE;
	if ( NULL != Pack )
		*Pack = cFALSE;
	if ( NULL != Sign )
		memset(Sign, 0, SIGN_SIZE);
	if ( NULL != Buff )
		*Buff = NULL;
	if ( NULL != Size )
		*Size = 0;
	
	/* try find sign */
	if ( NULL != Sign && NULL != HasSign )
	{
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
		
		/* read digital sign */
		if ( FS > SIGN_SIZE )
		{
#ifdef USE_WIN32_API
			SetFilePointer(hFile, (FS-SIGN_SIZE), NULL, FILE_BEGIN);
			if ( !ReadFile(hFile, &Sign[0], SIGN_SIZE, &Rdt, NULL) || Rdt!=SIGN_SIZE )
			{
				CloseHandle(hFile);
				return(DIFF_ERR_FILE);
			}
			CloseHandle(hFile);
#else
			fseek(BF, (FS-SIGN_SIZE), SEEK_SET);
			if ( 0 == fread(Sign, SIGN_SIZE, 1, BF) )
			{
				fclose(BF);
				return(DIFF_ERR_FILE);
			}
			fclose(BF);
#endif
			/* generic check for sign */
			if ( Sign[0] != 0x0D || Sign[1] != 0x0A )
				goto no_sign; /* not signed */
			if ( Sign[2] != ';'  || Sign[3] != 0x20 )
				goto no_sign; /* not signed */
			if ( Sign[SIGN_SIZE-2] != 0xAD || Sign[SIGN_SIZE-1] != 0xAD )
				goto no_sign; /* not signed */
			for ( I = 4; I < (SIGN_SIZE-2); I++ )
			{
				tBYTE SB = Sign[I];
				tBOOL Good = cFALSE;
				
				if ( SB >= '0' && SB <= '9' )
					Good = cTRUE;
				else if ( SB >= 'a' && SB <= 'z' )
					Good = cTRUE;
				else if ( SB >= 'A' && SB <= 'Z' )
					Good = cTRUE;
				else if ( SB == '+' || SB == '/' )
					Good = cTRUE;
				
				if ( !Good )
					goto no_sign; /* invalid char in sign */
			}
			
			if ( NULL != HasSign )
				*HasSign = cTRUE;
		}
	}
	
no_sign:
	
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
		//printf("Used already unpacked base\n");
#endif
		
		// try read file
#ifdef USE_WIN32_API
		if ( INVALID_HANDLE_VALUE == (hFile = CreateFile(FName, GENERIC_READ,
			FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)) )
		{
			return(DIFF_ERR_FILE);
		}
		FS = GetFileSize(hFile, NULL);
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
#else
		if ( NULL == (BF = fopen(FName, "rb")) )
			return(DIFF_ERR_FILE);
		FS = GetFSize(BF);
		fseek(BF, 0, SEEK_SET);
#endif
		
		if ( NULL != HasSign && *HasSign != cFALSE )
			FS -= SIGN_SIZE;
		
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
		NPack = cFALSE;
	}
	else
	{
		PrepBase4Diff(*Buff, 0);
		
#ifdef DG_DEBUG
		//    printf("Used base after unpacking\n");
		//    DebugDumpBlock(*Buff, *Size);
#endif
		
		/* realloc data block */
		if ( NULL == (Mem = (tBYTE*)malloc(*Size + 16)) )
		{
			FreeBuffer(*Buff);
			return(DIFF_ERR_NOMEM);
		}
		memcpy(Mem, *Buff, *Size);
		FreeBuffer(*Buff);
		NPack = cTRUE;
	}
	
	// unpack SQZE format
	if (*Size > sizeof(CSqueezeHeader))
	{
		CSqueezeHeader* pSQZE = (CSqueezeHeader*)Mem;
		if ((pSQZE->dwSignature == SQZE_HEADER_SIGNATURE) && (pSQZE->dwVersion == 1))
		{
			if (pSQZE->dwCrc == ~CRC32((char*)pSQZE + pSQZE->dwHeaderSize, pSQZE->dwCompressedSize, 0xffffffff))
			{
				CSqueezeHeader* pSQZU = (CSqueezeHeader*)malloc(pSQZE->dwHeaderSize + pSQZE->dwOriginalSize);
				if (!pSQZU)
				{
					free(Mem);
					return (DIFF_ERR_NOMEM);
				}
				pSQZU = (CSqueezeHeader*)pSQZU;
				memcpy(pSQZU, pSQZE, pSQZE->dwHeaderSize);
				pSQZU->dwSignature = SQZU_HEADER_SIGNATURE;
				
				*Size = unsqu((char*)pSQZE + pSQZE->dwHeaderSize, (char*)pSQZU + pSQZU->dwHeaderSize);
				free(Mem);
				if (pSQZU->dwOriginalSize != *Size)
				{
					free(pSQZU);
					return (DIFF_ERR_CORRUPT);
				}
				Mem = (tBYTE*)pSQZU;
				*Size = pSQZU->dwHeaderSize + pSQZU->dwOriginalSize;
			}
		}
	}

	// unpack
	if (Diff_DLL_IsPacked(Mem, *Size))
		nPackFormat = PACK_FORMAT_DLL;
	else if (Diff_KFB_IsPacked(Mem, *Size))
		nPackFormat = PACK_FORMAT_KFB;
	if (nPackFormat != PACK_FORMAT_NUL)
	{ 
		struct Diff_Buffer out_buf;
		bool res = 0;
		if (nPackFormat == PACK_FORMAT_DLL)
			res = Diff_DLL_Unpack(Mem, *Size, &out_buf);
		else if (nPackFormat == PACK_FORMAT_KFB)
			res = Diff_KFB_Unpack(Mem, *Size, &out_buf);
		free(Mem);
		if (!res)
			return (DIFF_ERR_NOMEM);
		*Size = out_buf.m_size;
		Mem = malloc(out_buf.m_size);
		if (Mem)
			memcpy(Mem, out_buf.m_data, out_buf.m_size);
		Diff_Buffer_Free(&out_buf);
		if (!Mem)
			return (DIFF_ERR_NOMEM);
	}

	*Buff = Mem;
	if ( NULL != Pack )
		*Pack = NPack;
	
	return(DIFF_ERR_OK);
}

// ----------------------------------------------------------------------------
