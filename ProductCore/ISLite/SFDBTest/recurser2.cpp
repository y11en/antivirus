  #include "windows.h"
#include "prague.h"
#include "debug.h"
#include "IFace\i_io.h"
#include "IFace\i_sfdb.h"
#include "IFace\i_ifilesecurestatus.h"
#include "../nfio/win32_io.h"
#include "../hash_api/md5/hash_md5.h"

#define READ_SIZE	0x8000

//char* fname=NULL;
//BOOL bInfected=FALSE;

extern hSECUREFILEDB hSFDB[];
extern hROOT hRoot;
extern hIFILESECURESTATUS hChecker;

void ByteToStr(
			   DWORD cb, 
			   void* pv, 
			   LPSTR sz)
			   //--------------------------------------------------------------------
			   // Parameters passed are:
			   //    pv is the array of BYTES to be converted.
			   //    cb is the number of BYTEs in the array.
			   //    sz is a pointer to the string to be returned.
			   
{
	//--------------------------------------------------------------------
	//  Declare and initialize local variables.
	
	BYTE* pb = (BYTE*) pv; // local pointer to a BYTE in the BYTE array
	DWORD i;               // local loop counter
	int b;                 // local variable
	
	//--------------------------------------------------------------------
	//  Begin processing loop.
	
	for (i = 0; i<cb; i++)
	{
		b = (*pb & 0xF0) >> 4;
		*sz++ = (b <= 9) ? b + '0' : (b - 10) + 'A';
		b = *pb & 0x0F;
		*sz++ = (b <= 9) ? b + '0' : (b - 10) + 'A';
		pb++;
		if ((i&3) == 3)
			*sz++ = ' ';
		
	}
	*sz++ = 0;
}

int ProcessFileScan(LPSTR szPath,LPSTR szFileName)
{
	hIO hIo = NULL;
	tERROR error;
	char szFullPath[MAX_PATH];
	tDWORD dwLastChangeTime;
	tDWORD dwLastAVCheck;
	
	wsprintf(szFullPath, "%s%s", szPath, szFileName);
	error = CALL_SYS_ObjectCreate(hRoot, &hIo, IID_IO, PID_WIN32_NFIO, SUBTYPE_ANY);
	if (PR_SUCC(error))
		error = CALL_SYS_PropertySetDWord(hIo, pgOBJECT_OPEN_MODE, fOMODE_OPEN_IF_EXIST);
	if (PR_SUCC(error))
		error = CALL_SYS_PropertySetDWord(hIo, pgOBJECT_ACCESS_MODE, fACCESS_READ);
	if (PR_SUCC(error))
		error = CALL_SYS_PropertySetStr(hIo, NULL, pgOBJECT_FULL_NAME, szFullPath, strlen(szFullPath)+1, cCP_ANSI);
	if (PR_SUCC(error))
		error = CALL_SYS_ObjectCreateDone(hIo);
	if (PR_FAIL(error))
	{
		PR_TRACE((hRoot, prtERROR, "Failed to create IO for file '%s'", szFullPath));
		if (hIo != NULL)
			error = CALL_SYS_ObjectClose(hIo);
		return 0;
	}

	error = CALL_iFileSecureStatus_GetStatus(hChecker, hIo, NULL, 0, &dwLastChangeTime, &dwLastAVCheck);
	if (error != errMODULE_UNKNOWN_FORMAT)
	{
		if (PR_FAIL(error))
			PR_TRACE((hRoot, prtERROR, "Checker GetStatus: failed with %08X", error));
		else
			PR_TRACE((hRoot, prtNOTIFY, "Checker GetStatus: dwLastChangeTime=%08X, dwLastAVCheck=%08X", dwLastChangeTime, dwLastAVCheck));

		if (dwLastAVCheck == 0)
			PR_TRACE((hRoot, prtALWAYS_REPORTED_MSG, "Checker: New file %s", szFullPath));
		
		error = CALL_iFileSecureStatus_UpdateStatus(hChecker, hIo, dwLastChangeTime, ++dwLastAVCheck);
		if (PR_FAIL(error))
			PR_TRACE((hRoot, prtERROR, "Checker UpdateStatus: failed with %08X", error));
		else
			PR_TRACE((hRoot, prtNOTIFY, "Checker UpdateStatus: ok"));
		
	}

	error = CALL_SYS_ObjectClose(hIo);
	
	return 0;
}

int ProcessFileScan_Old(LPSTR szPath,LPSTR szFileName)
{
//	hSIGNATURESEARCHER hSigSearcher = NULL;
	char szFullPath[MAX_PATH];
	DWORD dwBytesRead = 1;
	void* pMD5Ctx;
	char md5hash[16];
	DWORD dwHashSize;
	tERROR error;
	
	wsprintf(szFullPath, "%s%s", szPath, szFileName);
	HANDLE hFile = CreateFile(szFullPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hFile == INVALID_HANDLE_VALUE)
	{
		ODS(("Error open file '%s'",szFullPath));
		return -1;
	}
	
	tBYTE* pData = (tBYTE*) malloc(READ_SIZE);

	pMD5Ctx = malloc(hash_api_md5.ContextSize);
	if (!hash_api_md5.Init(0, pMD5Ctx, hash_api_md5.ContextSize, 16, NULL))
	{
		ODS(("hash_api_md5.Init failed!"));
		return 0;
	}

	ODSN(("%s", szFullPath));
	while (dwBytesRead)
	{
		ReadFile(hFile, pData, READ_SIZE, &dwBytesRead, NULL);
		if (dwBytesRead == 0)
			break;
		if (!hash_api_md5.Update(pMD5Ctx, pData, dwBytesRead))
		{
			ODS(("hash_api_md5.Update failed!"));
			break;
		}
		
		break;
	}

	dwHashSize = countof(md5hash);
	memset(pData, 0, 0x80);
	if (!hash_api_md5.Finalize(pMD5Ctx, &dwHashSize, md5hash))
	{
		ODS(("hash_api_md5.Finalize failed!"));
	}
	else
	{
		ByteToStr(dwHashSize, md5hash, (char*)pData+8);
	}

	ODS(("\t%s", pData));

	{
		__int64 hash64;
		hash64 =  *(__int64*)&md5hash[0];
		hash64 += *(__int64*)&md5hash[8];

		error = CALL_SecureFileDatabase_GetRecord(hSFDB[0], &hash64, NULL);
		if (error != errNOT_FOUND)
			ODS(("SecureFileDatabase_GetRecord1: ???"));

		error = CALL_SecureFileDatabase_UpdateRecord(hSFDB[0], &hash64, pData);
		if (PR_FAIL(error))
			ODS(("SecureFileDatabase_UpdateRecord: FAILED"));

		error = CALL_SecureFileDatabase_GetRecord(hSFDB[0], &hash64, NULL);
		if (PR_FAIL(error))
			ODS(("SecureFileDatabase_GetRecord2: FAILED"));
	}
	

	free(pData);
	CloseHandle(hFile);
	return 0;
}

int ProcessFileDir(LPSTR szPath,LPSTR szFileName)
{
	ODS(("%s%s", szPath, szFileName));
	return 0;
}

BOOL ScanDir(LPSTR szPath, 
			 int ProcessFile(LPSTR szPath,LPSTR szFileName),
			 BOOL bScanSubfolders)
{
	CHAR szFullPath[MAX_PATH];
	LPSTR szFilePart;
	HANDLE hFindFile;
	WIN32_FIND_DATA sFindData;

	if (ProcessFile == NULL)
		return FALSE;

	szFullPath[0] = 0;
	GetFullPathName(szPath, MAX_PATH, szFullPath, &szFilePart);
	
	if ( szFilePart == NULL)
	{
		szFilePart = szFullPath + lstrlen(szFullPath);
		if (szFilePart != szFullPath && szFilePart[-1] != '\\')
		{
			lstrcat(szFilePart, "\\");
			szFilePart++;
		}
		lstrcpy(szFilePart, "*.*");
	}
	
	hFindFile = FindFirstFile(szFullPath, &sFindData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (sFindData.cFileName[0] == '.' && (sFindData.cFileName[1] == 0 || (sFindData.cFileName[1] == '.' && sFindData.cFileName[2] == 0)))
				continue;
			lstrcpy(szFilePart, sFindData.cFileName);
			if (sFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (bScanSubfolders == TRUE)
				{
					lstrcat(szFullPath, "\\");
					ScanDir(szFullPath, ProcessFile, bScanSubfolders);
				}
			}
			else
			{
				szFilePart[0] = 0;
				ProcessFile(szFullPath, sFindData.cFileName);
			}
		} while (FindNextFile(hFindFile, &sFindData));
		FindClose(hFindFile);
	}
	return TRUE;
}

BOOL ScanDir2(LPSTR szPath, LPSTR szFileMask,
			 int ProcessFile(LPSTR szPath,LPSTR szFileName),
			 BOOL bScanSubfolders)
{
	CHAR szFullPath[MAX_PATH];
	LPSTR szFilePart;
	HANDLE hFindFile;
	WIN32_FIND_DATA sFindData;
	szFullPath[0] = 0;

	if (ProcessFile == NULL)
		return FALSE;

	GetFullPathName(szPath, MAX_PATH, szFullPath, &szFilePart);
	
	if (szFileMask == NULL || szFileMask[0] == 0)
		szFileMask = "*.*";
	szFilePart = szFullPath + lstrlen(szFullPath);
	if (szFilePart != szFullPath && szFilePart[-1] != '\\') // close path
		*szFilePart++ = '\\';
	lstrcpy(szFilePart, szFileMask);
	
	hFindFile = FindFirstFile(szFullPath, &sFindData);
	szFilePart[0] = 0;
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (sFindData.cFileName[0] == '.' && (sFindData.cFileName[1] == 0 || (sFindData.cFileName[1] == '.' && sFindData.cFileName[2] == 0)))
				continue;
			if ((sFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				ProcessFile(szFullPath, sFindData.cFileName);
		} while (FindNextFile(hFindFile, &sFindData));
		FindClose(hFindFile);
	}


	if (bScanSubfolders == TRUE)
	{
		lstrcpy(szFilePart, "*.*");
		hFindFile = FindFirstFile(szFullPath, &sFindData);
		if (hFindFile != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (sFindData.cFileName[0] == '.' && (sFindData.cFileName[1] == 0 || (sFindData.cFileName[1] == '.' && sFindData.cFileName[2] == 0)))
					continue;
				if (sFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					lstrcpy(szFilePart, sFindData.cFileName);
					ScanDir2(szFullPath, szFileMask, ProcessFile, bScanSubfolders);
				}
			} while (FindNextFile(hFindFile, &sFindData));
			FindClose(hFindFile);
		}
	}

	return TRUE;
}
