//#include "../ichecker2/npr_win.h"
#if defined (__unix__)
#if defined (THREADS_SAFE)
#define _REENTRANT
#endif
#include <compat.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#if defined (HAVE_CONFIG_H)
#include "config-unix.h"
#endif
#if !defined (HAVE_FLOCK)
#include <comp_flock.h>
#endif
#else
#define PATH_MAX 512
#include <windows.h>
#endif
#include "../ichecker2/npr.h"
#include "npr_sfdb.h"
#include "npr_time.h"
#include "../../hash/hash_api/md5/hash_md5.h"
#define LOCK_SUFFIX "_Lock"
#define TEMP_FILE_PREFIX "SFDB"
#define CURRENT_DATETIME_GET_FREQUENCY	(1<<8)
#ifdef _DEBUG
#define _FILE_SHARE_MODE (FILE_SHARE_READ)
#else
#define _FILE_SHARE_MODE 0
#endif

#include "flck.h"
/*#if defined (__unix__)*/
#define LOCK_READ() flck ( ldata->LockFile, _F_LCK_R )
#define LOCK_WRITE() flck ( ldata->LockFile, _F_LCK_W ) 
#define RELEASE() flck ( ldata->LockFile, _F_UNLCK )
#  if defined (__MANUAL_SYNC__) 
#  define RELEASE_WRITE() FlushViewOfFile(ldata->pMappingView, 0); flck ( ldata->LockFile, _F_UNLCK )
#  else
#  define RELEASE_WRITE() flck ( ldata->LockFile, _F_UNLCK )
#  endif
/*#else
#define MUTEX_NAME	"NPR_SFDBv2"
#define MUTEX_TIMEOUT	200
#define LOCK_READ() (WaitForSingleObject(ldata->hMutex, MUTEX_TIMEOUT), errOK)
#define LOCK_WRITE() (WaitForSingleObject(ldata->hMutex, MUTEX_TIMEOUT), errOK)
#define RELEASE()    ReleaseMutex(ldata->hMutex)
#define RELEASE_WRITE() ReleaseMutex(ldata->hMutex)
#endif
*/

#if defined (_WIN32)
//#define _USE_TRY_
#if !defined(_DEBUG) || defined(_USE_TRY_) 
#define __TRY __try
#define __TRY_F __try
#define __EXCEPT __except
#define __FINALLY __finally
#else
#define __TRY 
#define __TRY_F __try
#define __EXCEPT(x) if (0)
#define __FINALLY __finally
#undef GetExceptionCode
#define GetExceptionCode() 0
#endif
#else
#define __TRY 
#define __TRY_F 
#define __EXCEPT(x) if (0)
#define __FINALLY 
#undef GetExceptionCode
#define GetExceptionCode() 0
#endif
__inline tERROR GetCurrentDateTime(hi_SecureFileDatabase _this, tDWORD* pdwDate)
{
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	if ((ldata->dwCurrentDateTimeGetCounter++ % CURRENT_DATETIME_GET_FREQUENCY) == 0)
	{
		tDATETIME dt;
		tQWORD qwDays;
		if (PR_FAIL(error = Now(&dt)))
			PR_TRACE((_this, prtERROR, "Now() failed with %terr", error));
		else if (PR_FAIL(error = DTIntervalGet(&dt, 0, &qwDays, (tQWORD)24*(tQWORD)60*(tQWORD)60*(tQWORD)1000000000)))
			PR_TRACE((_this, prtERROR, "DTIntervalGet() failed with %terr", error));
		else
			ldata->dwCurrentDate = (tDWORD)qwDays;
	}
	if (PR_SUCC(error))
		*pdwDate = ldata->dwCurrentDate;
	return error;
}

int pr_call CompareRecords8(const void *data1, const void *data2)
{
	unsigned __int64* i1 = (unsigned __int64*)data1;
	unsigned __int64* i2 = (unsigned __int64*)data2;
	if (*i1 == *i2)
		return 0;
	if (*i1 < *i2)
		return -1;
	return 1;
}

tDWORD pr_call GetIndex8(struct tag_LOCALDATA* ldata, tVOID* pRecordID)
{
	LARGE_INTEGER i = *(LARGE_INTEGER*)pRecordID;
	
	if (ldata->pDB == NULL)
	{
		PR_TRACE(( NULL, prtERROR, "GetIndex8: database not initialized"));
		return errOBJECT_NOT_INITIALIZED;
	}

	return ((ldata->pDB->dwIndexMask & i.HighPart) >> (32 - ldata->pDB->dwIndexBits));
}

tDWORD GetLeftBitPos(tDWORD dwNumber)
{
	if (dwNumber >> 1)
		return GetLeftBitPos(dwNumber >> 1) + 1;
	return 0;
}

tBOOL AssignRecordCompareFunc(hi_SecureFileDatabase _this)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);

	if (ldata->pDB == NULL)
	{
		PR_TRACE(( _this, prtERROR, "AssignRecordCompareFunc: database not initialized"));
		return errOBJECT_NOT_INITIALIZED;
	}
	__TRY
	{
		switch (ldata->pDB->dwRecordIDSize)
		{
		case 8:
			ldata->RecordCompareFunc = CompareRecords8;
			ldata->GetIndex = GetIndex8;
			break;
		default:
			PR_TRACE(( _this, prtERROR, "AssignRecordCompareFunc: incompatible RecordID size (%d)", ldata->pDB->dwRecordIDSize));
			return cFALSE;
		}
	} __EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
		return cFALSE;
	}
	return cTRUE;
}

tERROR RemoveTempFiles(hi_SecureFileDatabase _this)
{
	CHAR szFullPath[MAX_PATH];
	LPSTR szFilePart;
	tCHAR szTempFilePath[MAX_PATH];
	tDWORD dwPathSize;
	WIN32_FIND_DATA sFindData;
	HANDLE hFindFile;
	
	dwPathSize = GetTempPath(countof(szTempFilePath), szTempFilePath);
	if (dwPathSize > countof(szTempFilePath) || dwPathSize == 0){
		PR_TRACE(( _this, prtERROR, "RemoveTempFiles: GetTempFile failed with error=0x%08X", GetLastError()));
		return errUNEXPECTED;
	}
	
	// remove crap
	szFilePart = szFullPath + dwPathSize;
	strcpy(szFullPath, szTempFilePath);
	strcpy(szFilePart, TEMP_FILE_PREFIX "*.tmp");
	hFindFile = FindFirstFile(szFullPath, &sFindData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			lstrcpy(szFilePart, sFindData.cFileName);
			DeleteFile(szFullPath);
			PR_TRACE(( _this, prtNOTIFY, "RemoveTempFiles: DeleteFile %s err=%08X", szFullPath, GetLastError()));
		} while (FindNextFile(hFindFile, &sFindData));
		FindClose(hFindFile);
	}

	return errOK;
}

tERROR GetMappedFileName(hi_SecureFileDatabase _this)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	tCHAR szTempFilePath[MAX_PATH];
	tDWORD dwPathSize;
	tUINT nUnique = 0;
	
	RemoveTempFiles(_this);

	dwPathSize = GetTempPath(countof(szTempFilePath), szTempFilePath);
	if (dwPathSize > countof(szTempFilePath) || dwPathSize == 0){
		PR_TRACE(( _this, prtERROR, "GetMappedFileName: GetTempFile failed with error=0x%08X", GetLastError()));
		return errUNEXPECTED;
	}

	ldata->szSFDBSharedName[0] = 0;
	if (ldata->szSFDBPathName[0])
	{
		tBYTE sMD5Ctx[0x80];
		BYTE byCheckSum[0x10];
		unsigned long ulBytes = sizeof(byCheckSum);
		int i;

		if (!hash_api_md5.Init(0, &sMD5Ctx, sizeof(sMD5Ctx), 16, NULL))
			return errUNEXPECTED;
		if (!hash_api_md5.Update(&sMD5Ctx, ldata->szSFDBPathName, strlen(ldata->szSFDBPathName)))
			return errUNEXPECTED;
		if (!hash_api_md5.Finalize(&sMD5Ctx, &ulBytes, &byCheckSum))
		{
			PR_TRACE(( _this, prtERROR, "GetMappedFileName: dwUnique hash_api_md5.Finalize failed!"));
			return errUNEXPECTED;
		}
		for (i=0; i<4; i++)
			nUnique ^= *(((tDWORD*)byCheckSum)+i);
		wsprintf(ldata->szSFDBSharedName, TEMP_FILE_PREFIX"%08X", nUnique);
		strcpy(ldata->szTempFile, szTempFilePath);
		strcat(ldata->szTempFile, ldata->szSFDBSharedName);
		strcat(ldata->szTempFile, ".tmp");
	}
	else
	if (GetTempFileName(szTempFilePath, TEMP_FILE_PREFIX, 0, ldata->szTempFile) == 0)
	{
		PR_TRACE(( _this, prtERROR, "GetMappedFileName: GetTempFileName failed with error=0x%08X", GetLastError()));
		return errUNEXPECTED;
	}
	else
		DeleteFile(ldata->szTempFile); // remove newly created file

	PR_TRACE(( _this, prtNOTIFY, "GetMappedFileName: szTempFile = %s", ldata->szTempFile));

	return errOK;
}

tERROR CloseMappedFile(hi_SecureFileDatabase _this)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	PR_TRACE(( _this, prtNOTIFY, "CloseMappedFile: %s", ldata->szTempFile));

	if (ldata->pMappingView)
	{
		ldata->pDB->dwActiveClients--; // disconnect
		if (!FlushViewOfFile(ldata->pMappingView, 0))
			PR_TRACE(( _this, prtERROR, "CloseMappedFile: FlushViewOfFile failed with error=0x%08X", GetLastError()));
		if (!UnmapViewOfFile(ldata->pMappingView))
			PR_TRACE(( _this, prtERROR, "CloseMappedFile: UnmapViewOfFile failed with error=0x%08X", GetLastError()));
		ldata->pMappingView = NULL;
		ldata->dwMapViewSize = 0;
	}
	if (ldata->hFileMapping)
	{
		CloseHandle(ldata->hFileMapping);
		ldata->hFileMapping = NULL;
	}
	if (ldata->hTempFile && ldata->hTempFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(ldata->hTempFile);
		ldata->hTempFile = INVALID_HANDLE_VALUE;
	}
	ldata->pDB = NULL;

	RemoveTempFiles(_this);

	return errOK;
}

tERROR CreateMappedFile(hi_SecureFileDatabase _this, SFDB_HDR* pDBHdr)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	tERROR error;
	tDWORD dwWin32Error;
	tBOOL bConnectedToSharedMMF = cFALSE;
	
	CloseMappedFile(_this);
	
	if (PR_FAIL(error = GetMappedFileName(_this)))
	{
		PR_TRACE(( _this, prtERROR, "CreateMappedFile: GetMappedFileName failed with error=0x%08X", error));
		return errUNEXPECTED;
	}

	PR_TRACE((_this, prtNOTIFY, "CreateMappedFile: %s", ldata->szTempFile));
	__TRY 
	{
		
/*		ldata->hTempFile = CreateFile(ldata->szTempFile, GENERIC_READ | GENERIC_WRITE, _FILE_SHARE_MODE, NULL, CREATE_ALWAYS, 0, NULL);
		if (ldata->hTempFile == INVALID_HANDLE_VALUE)
		{
			PR_TRACE(( _this, prtERROR, "CreateMappedFile: CreateFile failed with error=0x%08X", GetLastError()));
			return errUNEXPECTED;
		}
		
		
		ldata->hFileMapping = CreateFileMapping(ldata->hTempFile, NULL, PAGE_READWRITE, 0, pDBHdr->dwFileSize, (ldata->szSFDBSharedName[0] ? ldata->szSFDBSharedName : NULL));
		*/
		ldata->hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, ldata->dwDatabaseSizeLimit, (ldata->szSFDBSharedName[0] ? ldata->szSFDBSharedName : NULL));
		
		dwWin32Error = GetLastError();
		if (dwWin32Error == ERROR_ALREADY_EXISTS)
			bConnectedToSharedMMF = cTRUE;

		if (ldata->hFileMapping == NULL)
		{
			PR_TRACE(( _this, prtERROR, "CreateMappedFile: CreateFileMapping failed with error=0x%08X", GetLastError()));
			return errUNEXPECTED;
		}
		
		ldata->dwMapViewSize = pDBHdr->dwFileSize;
		ldata->pMappingView = MapViewOfFile(ldata->hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, ldata->dwMapViewSize);
		if (ldata->pMappingView == NULL)
		{
			PR_TRACE(( _this, prtERROR, "CreateMappedFile: MapViewOfFile failed with error=0x%08X", GetLastError()));
			return errUNEXPECTED;
		}
		
		// fix troubles under WinME where MMF not inited with zeros while growing...
		if (ldata->bWin9xSystem)
			memset((tBYTE*)ldata->pMappingView, 0, pDBHdr->dwFileSize);

#ifdef _DEBUG_MEMIMAGE
		{
			DWORD dw;
			HANDLE hFile = CreateFile("C:\\memimage.dat", GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				WriteFile(hFile, ldata->pMappingView, pDBHdr->dwFileSize, &dw, 0);
				CloseHandle(hFile);
			}
		}
#endif
		ldata->pDB = (SFDB_HDR*)ldata->pMappingView;

		if (bConnectedToSharedMMF == cFALSE)
		{
			memcpy(ldata->pDB, pDBHdr, sizeof(SFDB_HDR));
			ldata->pDB->dwActiveClients = 1;
		}
		else
			ldata->pDB->dwActiveClients++;

		if (!AssignRecordCompareFunc(_this))
		{
			PR_TRACE(( _this, prtERROR, "CreateMappedFile: AssignRecordCompareFunc failed"));
			return errUNEXPECTED;
		}

#ifdef _DEBUG_MEMIMAGE
		{
			DWORD dw;
			HANDLE hFile = CreateFile("C:\\memimage2.dat", GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				WriteFile(hFile, ldata->pMappingView, pDBHdr->dwFileSize, &dw, 0);
				CloseHandle(hFile);
			}
		}
#endif
	} 
	__EXCEPT (EXCEPTION_EXECUTE_HANDLER) 
	{
		PR_TRACE(( _this, prtERROR, "CreateMappedFile: exception handled 0x%08X", GetExceptionCode()));
		CloseMappedFile(_this);
		return errUNEXPECTED;
	}
	PR_TRACE((_this, prtNOTIFY, "CreateMappedFile: OK. pMappingView=%08X, size=%08X", ldata->pMappingView, pDBHdr->dwFileSize));
	return errOK;
}

tERROR OpenMappedFile(hi_SecureFileDatabase _this)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	SFDB_HDR sDBHdr;
	tDWORD dwBytes;
	tDWORD dwWin32Error;
	tBOOL bConnectedToSharedMMF = cFALSE;
	
	CloseMappedFile(_this);
	
	PR_TRACE((_this, prtNOTIFY, "OpenMappedFile: %s", ldata->szTempFile));

	if (PR_FAIL(GetMappedFileName(_this)))
		return errUNEXPECTED;
	__TRY
	{
		if (ldata->szSFDBSharedName[0])
			ldata->hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, ldata->szSFDBSharedName);
		
		if (ldata->hFileMapping != NULL)
		{
			ldata->pMappingView = MapViewOfFile(ldata->hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SFDB_HDR));
			if (ldata->pMappingView == NULL)
			{
				PR_TRACE(( _this, prtERROR, "OpenMappedFile: MapViewOfFile failed with error=0x%08X", GetLastError()));
				return errUNEXPECTED;
			}
			memcpy(&sDBHdr, ldata->pMappingView, sizeof(SFDB_HDR));
			UnmapViewOfFile(ldata->pMappingView);
			bConnectedToSharedMMF = cTRUE;
			PR_TRACE(( _this, prtNOTIFY, "OpenMappedFile: connected to MMF '%s'", ldata->szSFDBSharedName));
		}
		else
		{
			ldata->hTempFile = CreateFile(ldata->szTempFile, GENERIC_READ | GENERIC_WRITE, _FILE_SHARE_MODE, NULL, OPEN_EXISTING, 0, NULL);
			if (ldata->hTempFile == INVALID_HANDLE_VALUE)
			{
				//PR_TRACE(( _this, prtERROR, "OpenMappedFile: CreateFile failed with error=0x%08X", GetLastError()));
				return errUNEXPECTED;
			}
			
			ReadFile(ldata->hTempFile, &sDBHdr, sizeof(SFDB_HDR), (DWORD*)&dwBytes, NULL);
			if (dwBytes != sizeof(SFDB_HDR))
			{
				PR_TRACE(( _this, prtERROR, "OpenMappedFile: Cannot read database header. Error=0x%08X", GetLastError()));
				return errUNEXPECTED;
			}
			
			ldata->hFileMapping = CreateFileMapping(ldata->hTempFile, NULL, PAGE_READWRITE, 0, sDBHdr.dwFileSize, (ldata->szSFDBSharedName[0] ? ldata->szSFDBSharedName : NULL));
			dwWin32Error = GetLastError();
			if (dwWin32Error == ERROR_ALREADY_EXISTS)
				bConnectedToSharedMMF = cTRUE;
			if (ldata->hFileMapping == NULL)
			{
				PR_TRACE(( _this, prtERROR, "OpenMappedFile: CreateFileMapping failed with error=0x%08X", GetLastError()));
				return errUNEXPECTED;
			}
		}
		
		ldata->dwMapViewSize = sDBHdr.dwFileSize;
		ldata->pMappingView = MapViewOfFile(ldata->hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, ldata->dwMapViewSize);
		if (ldata->pMappingView == NULL)
		{
			PR_TRACE(( _this, prtERROR, "OpenMappedFile: MapViewOfFile failed with error=0x%08X", GetLastError()));
			return errUNEXPECTED;
		}
		
		ldata->pDB = (SFDB_HDR*)ldata->pMappingView;
		if (!AssignRecordCompareFunc(_this))
		{
			PR_TRACE(( _this, prtERROR, "OpenMappedFile: AssignRecordCompareFunc failed"));
			return errUNEXPECTED;
		}

		if (bConnectedToSharedMMF == cFALSE)
			ldata->pDB->dwActiveClients = 1;
		else
			ldata->pDB->dwActiveClients++;
	} 
	__EXCEPT (EXCEPTION_EXECUTE_HANDLER) 
	{
		PR_TRACE(( _this, prtERROR, "OpenMappedFile: exception handled 0x%08X", GetExceptionCode()));
		CloseMappedFile(_this);
		return errUNEXPECTED;
	}

	PR_TRACE((_this, prtNOTIFY, "OpenMappedFile: OK. pMappingView=%08X, size=%08X", ldata->pMappingView, sDBHdr.dwFileSize));
	return errOK;
}

tERROR RemapViewOfFile(hi_SecureFileDatabase _this, tDWORD dwNewViewSize)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	tVOID* pOldMappingView = ldata->pMappingView;
	if (pOldMappingView)
	{
		if (!FlushViewOfFile(pOldMappingView, 0))
			PR_TRACE(( _this, prtERROR, "ExpandMappedFile: FlushViewOfFile failed with error=0x%08X", GetLastError()));
	}

	ldata->pMappingView = MapViewOfFile(ldata->hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, dwNewViewSize);
	if (ldata->pMappingView == NULL)
	{
		PR_TRACE(( _this, prtERROR, "ExpandMappedFile: MapViewOfFile failed with error=0x%08X", GetLastError()));
		return errUNEXPECTED;
	}

	if (pOldMappingView)
	{
		if (!UnmapViewOfFile(pOldMappingView))
			PR_TRACE(( _this, prtERROR, "ExpandMappedFile: UnmapViewOfFile failed with error=0x%08X", GetLastError()));
	}

	ldata->dwMapViewSize = dwNewViewSize;
	ldata->pDB = (SFDB_HDR*)ldata->pMappingView;
	if (!AssignRecordCompareFunc(_this))
	{
		PR_TRACE(( _this, prtERROR, "ExpandMappedFile: AssignRecordCompareFunc failed"));
		CloseMappedFile(_this);
		return errUNEXPECTED;
	}
	
	return errOK;
}

tERROR ExpandMappedFile(hi_SecureFileDatabase _this, tDWORD dwNewFileSize)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);

	if (ldata->hTempFile == NULL)
	{
		PR_TRACE(( _this, prtERROR, "ExpandMappedFile: database not initialized"));
		return errOBJECT_NOT_INITIALIZED;
	}
	
	if (ldata->pMappingView)
	{
		if (!FlushViewOfFile(ldata->pMappingView, 0))
			PR_TRACE(( _this, prtERROR, "ExpandMappedFile: FlushViewOfFile failed with error=0x%08X", GetLastError()));
		if (!UnmapViewOfFile(ldata->pMappingView))
			PR_TRACE(( _this, prtERROR, "ExpandMappedFile: UnmapViewOfFile failed with error=0x%08X", GetLastError()));
	}
		
	if (ldata->hFileMapping)
		CloseHandle(ldata->hFileMapping);
		
	ldata->hFileMapping = CreateFileMapping(ldata->hTempFile, NULL, PAGE_READWRITE, 0, dwNewFileSize, (ldata->szSFDBSharedName[0] ? ldata->szSFDBSharedName : NULL));
	if (ldata->hFileMapping == NULL)
	{
		PR_TRACE(( _this, prtERROR, "ExpandMappedFile: CreateFileMapping failed with error=0x%08X", GetLastError()));
		CloseMappedFile(_this);
		return errUNEXPECTED;
	}
	
	ldata->pMappingView = MapViewOfFile(ldata->hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, dwNewFileSize);
	if (ldata->pMappingView == NULL)
	{
		PR_TRACE(( _this, prtERROR, "ExpandMappedFile: MapViewOfFile failed with error=0x%08X", GetLastError()));
		CloseMappedFile(_this);
		return errUNEXPECTED;
	}

	ldata->pDB = (SFDB_HDR*)ldata->pMappingView;
	if (!AssignRecordCompareFunc(_this))
	{
		PR_TRACE(( _this, prtERROR, "ExpandMappedFile: AssignRecordCompareFunc failed"));
		CloseMappedFile(_this);
		return errUNEXPECTED;
	}
	
	return errOK;
}

tERROR ExpandDatabase(hi_SecureFileDatabase _this)
{
	SFDB_CLUSTER_HDR* pClusterNewH;
	SFDB_CLUSTER_HDR* pClusterNewL;
	SFDB_CLUSTER_HDR* pClusterOld;
	tBYTE* pRecord;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	SFDB_HDR* pDB;
	tDWORD i;
	tERROR error;

	if (ldata->pDB == NULL)
	{
		PR_TRACE(( _this, prtERROR, "ExpandDatabase: database not initialized"));
		return errOBJECT_NOT_INITIALIZED;
	}

	PR_TRACE(( _this, prtNOTIFY, "Enter ExpandDatabase method"));
	__TRY {
		SFDB_HDR sDBHdr;
		tDWORD dwOldSize = ldata->pDB->dwFileSize;
		
		memcpy(&sDBHdr, ldata->pDB, sizeof(SFDB_HDR));

		sDBHdr.dwIndexBits++;
		sDBHdr.dwIndexMask >>= 1;
		sDBHdr.dwIndexMask |= 0x80000000;
		sDBHdr.dwClusters = 1 << sDBHdr.dwIndexBits;
		sDBHdr.dwFileSize = sDBHdr.dwHeaderSize + sDBHdr.dwClusters * sDBHdr.dwClusterSize;
		
		if (sDBHdr.dwFileSize > ldata->dwDatabaseSizeLimit)
			return errNOT_OK;
		//if (PR_FAIL(error = ExpandMappedFile(_this, sDBHdr.dwFileSize)))
		if (PR_FAIL(error = RemapViewOfFile(_this, sDBHdr.dwFileSize)))
		{
			PR_TRACE((_this, prtERROR, "ExpandDatabase: RemapViewOfFile failed with %terr", error));
			return error;
		}

		// fix troubles under WinME where MMF not inited with zeros while growing...
		if (ldata->bWin9xSystem && sDBHdr.dwFileSize > dwOldSize)
			memset((tBYTE*)ldata->pMappingView + dwOldSize, 0, sDBHdr.dwFileSize - dwOldSize);
		
		pDB = ldata->pDB;
		memcpy(pDB, &sDBHdr, sizeof(SFDB_HDR));
		
		pClusterOld = (SFDB_CLUSTER_HDR*)((BYTE*)ldata->pMappingView + pDB->dwHeaderSize + (pDB->dwClusters / 2 - 1) * pDB->dwClusterSize);
		pClusterNewH = (SFDB_CLUSTER_HDR*)((BYTE*)ldata->pMappingView + pDB->dwHeaderSize + (pDB->dwClusters - 1) * pDB->dwClusterSize);
		pClusterNewL = (SFDB_CLUSTER_HDR*)((BYTE*)pClusterNewH - pDB->dwClusterSize);
		
		while (1) 
		{
			if (pClusterOld->dwRecords == 0 && (pClusterNewL->dwRecords | pClusterNewH->dwRecords) != 0)
			{
				// clean up 2 clusters
				pClusterNewL->dwRecords = pClusterNewH->dwRecords = 0;
#ifdef _DEBUG
				memset(pClusterNewL, 0, pDB->dwClusterSize * 2);
#endif
			}
			else
			{
				pRecord = (tBYTE*)pClusterOld+sizeof(SFDB_CLUSTER_HDR);
				for (i=0;i<pClusterOld->dwRecords; i++)
				{
					if (ldata->GetIndex(ldata, pRecord) & 1)
						break;
					pRecord += pDB->dwRecordSize;
				}
				
				pClusterNewH->dwRecords = pClusterOld->dwRecords - i;
				memcpy((tBYTE*)pClusterNewH+sizeof(SFDB_CLUSTER_HDR), pRecord, pClusterNewH->dwRecords * pDB->dwRecordSize);
#ifdef _DEBUG
				memset((tBYTE*)pClusterNewH+sizeof(SFDB_CLUSTER_HDR) + pClusterNewH->dwRecords * pDB->dwRecordSize, 0, (pDB->dwRecordsPerCluster - pClusterNewH->dwRecords)*pDB->dwRecordSize);
#endif				
				pClusterNewL->dwRecords = i;
				memcpy((tBYTE*)pClusterNewL+sizeof(SFDB_CLUSTER_HDR), (tBYTE*)pClusterOld+sizeof(SFDB_CLUSTER_HDR), pClusterNewL->dwRecords*pDB->dwRecordSize);
#ifdef _DEBUG
				memset((tBYTE*)pClusterNewL+sizeof(SFDB_CLUSTER_HDR) + pClusterNewL->dwRecords*pDB->dwRecordSize, 0, (pDB->dwRecordsPerCluster - pClusterNewL->dwRecords)*pDB->dwRecordSize);
#endif				
			}
			
			if (pClusterOld == pClusterNewL) // exit while
				break;
			
			pClusterOld = (SFDB_CLUSTER_HDR*)((BYTE*)pClusterOld - pDB->dwClusterSize);;
			pClusterNewH = (SFDB_CLUSTER_HDR*)((BYTE*)pClusterNewH - pDB->dwClusterSize*2);
			pClusterNewL = (SFDB_CLUSTER_HDR*)((BYTE*)pClusterNewL - pDB->dwClusterSize*2);
		}
	}
	__EXCEPT (EXCEPTION_EXECUTE_HANDLER) 
	{
		PR_TRACE(( _this, prtERROR, "ExpandDatabase: exception handled 0x%08X", GetExceptionCode()));
		return errUNEXPECTED;
	}
	PR_TRACE(( _this, prtNOTIFY, "Leave ExpandDatabase method, ret tERROR = 0x%08x", error));
	return errOK;
}

#define DELETE_OLDEST_RECORDS_IN_CLUSTER_COUNT		3
#define DELETE_OLDEST_RECORDS_IN_CLUSTER_COUNT_MAX	10
tERROR DeleteOldestRecordsInCluster(hi_SecureFileDatabase _this, SFDB_CLUSTER_HDR* pCluster)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	SFDB_HDR* pDB = ldata->pDB;
	tBYTE* pRecord;
	tDWORD dwRecord;
	SFDB_RECORD_INTERNAL_DATA* pRecordInternalData;
	tDWORD dwOldestDate[DELETE_OLDEST_RECORDS_IN_CLUSTER_COUNT];
	tDWORD i;
	tDWORD dwRecordsDeleted = 0;

	for (i=0; i<DELETE_OLDEST_RECORDS_IN_CLUSTER_COUNT; i++)
		dwOldestDate[i] = (tDWORD)-1;

	if (pCluster->dwRecords > ldata->pDB->dwRecordsPerCluster) // validation
	{
		PR_TRACE(( _this, prtERROR, "DeleteOldestRecordsInCluster: Cluster contain %d records? (RecordsPerCluster=%d)", pCluster->dwRecords, pDB->dwRecordsPerCluster));
		return errUNEXPECTED;
	}

	// look for oldest date
	pRecord = (tBYTE*)pCluster+sizeof(SFDB_CLUSTER_HDR);
	for (dwRecord=0; dwRecord<pCluster->dwRecords; dwRecord++){
		pRecordInternalData = (SFDB_RECORD_INTERNAL_DATA*)(pRecord + pDB->dwRecordSize - sizeof(SFDB_RECORD_INTERNAL_DATA) - sizeof(SFDB_RECORD_SESSION_DATA));
		if (dwOldestDate[0] > pRecordInternalData->dwLastUsedDate) 
		{
			for (i=DELETE_OLDEST_RECORDS_IN_CLUSTER_COUNT-1; i>0; i--)
				dwOldestDate[i] = dwOldestDate[i-1];
			dwOldestDate[0] = pRecordInternalData->dwLastUsedDate;
		}
		pRecord += pDB->dwRecordSize;
	}
	
	// delete oldest records
	pRecord = (tBYTE*)pCluster+sizeof(SFDB_CLUSTER_HDR);
	for (dwRecord=0; dwRecord<pCluster->dwRecords; dwRecord++){
		pRecordInternalData = (SFDB_RECORD_INTERNAL_DATA*)(pRecord + pDB->dwRecordSize - sizeof(SFDB_RECORD_INTERNAL_DATA) - sizeof(SFDB_RECORD_SESSION_DATA));
		for (i=0; i<DELETE_OLDEST_RECORDS_IN_CLUSTER_COUNT; i++)
		{
			if (dwOldestDate[i] == pRecordInternalData->dwLastUsedDate) // one of oldest records
			{
				pCluster->dwRecords--;
				memmove(pRecord, pRecord + pDB->dwRecordSize, pDB->dwRecordSize * (pCluster->dwRecords - dwRecord));
				dwRecord--;
				pRecord -= pDB->dwRecordSize;
				if (pCluster->dwRecords == 0)
					return errOK;
				if (++dwRecordsDeleted == DELETE_OLDEST_RECORDS_IN_CLUSTER_COUNT_MAX) // enougth
					return errOK;
				break;
			}
		}
		pRecord += pDB->dwRecordSize;
	}

	return errOK;
}

tERROR CompactDatabase(hi_SecureFileDatabase _this)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	SFDB_HDR* pDB = ldata->pDB;
	tDWORD dwCluster;
	SFDB_CLUSTER_HDR* pCluster;
	tBYTE* pRecord;
	tDWORD dwRecord;
	SFDB_RECORD_INTERNAL_DATA* pRecordInternalData;
	tDWORD dwCurrentDate;
	tERROR error = errNOT_OK;
	
	if (ldata->dwUnusedRecordsLimitTime == 0)
		return errNOT_OK;

	GetCurrentDateTime(_this, &dwCurrentDate);

	for (dwCluster=0; dwCluster<ldata->pDB->dwClusters; dwCluster++)
	{
		pCluster = (SFDB_CLUSTER_HDR*)((tBYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwCluster * pDB->dwClusterSize);
		pRecord = (tBYTE*)pCluster+sizeof(SFDB_CLUSTER_HDR);
		if (pCluster->dwRecords > ldata->pDB->dwRecordsPerCluster) // validation
		{
			PR_TRACE(( _this, prtERROR, "CompactDatabase: Cluster %d contain %d records? (RecordsPerCluster=%d)", dwCluster, pCluster->dwRecords, pDB->dwRecordsPerCluster));
			return errUNEXPECTED;
		}
		for (dwRecord=0; dwRecord<pCluster->dwRecords; dwRecord++){
			pRecordInternalData = (SFDB_RECORD_INTERNAL_DATA*)(pRecord + pDB->dwRecordSize - sizeof(SFDB_RECORD_INTERNAL_DATA) - sizeof(SFDB_RECORD_SESSION_DATA));
			if (dwCurrentDate - pRecordInternalData->dwLastUsedDate > ldata->dwUnusedRecordsLimitTime) // old unused record!
			{
				pCluster->dwRecords--;
				memmove(pRecord, pRecord + pDB->dwRecordSize, pDB->dwRecordSize * (pCluster->dwRecords - dwRecord));
				dwRecord--;
				error = errOK;
			}
			else
			pRecord += pDB->dwRecordSize;
		}
	}

	return error;
}


tERROR SecureFileDatabase_Create(hi_SecureFileDatabase* _this)
{
	(*_this) = (hi_SecureFileDatabase)malloc(sizeof(struct tag_SecureFileDatabase));
	(*_this)->data = (PLOCALDATA)malloc(sizeof(LOCALDATA));

    memset((*_this)->data,0,sizeof(LOCALDATA));

	(*_this)->data->bWin9xSystem = cFALSE;

#if !defined(_DEBUG)
	(*_this)->data->dwDatabaseSizeLimit = 0x1001000; // 16Mb + header
	(*_this)->data->dwUnusedRecordsLimitTime = 180;
#else
	(*_this)->data->dwDatabaseSizeLimit = 0x10000; // <1 Mb
	(*_this)->data->dwUnusedRecordsLimitTime = 10; 
#endif
#if defined (__unix__)
        (*_this)->data->LockFile = -1;
#else
        (*_this)->data->LockFile = INVALID_HANDLE_VALUE;
#endif
/*
	hCRITICAL_SECTION hCriticalSection;
	hMUTEX  hMutex;

	tCHAR	szTempFile[MAX_PATH*2];
	HANDLE	hTempFile;
	HANDLE	hFileMapping;
	VOID*   pMappingView;
	SFDB_HDR* pDB;
	int ( pr_call *RecordCompareFunc ) ( const void *, const void *);
	tDWORD ( pr_call *GetIndex) (struct tag_LOCALDATA* ldata, tVOID* pRecordID);
	tDWORD  dwSerializeOptions;
	tDWORD  dwCryptKey;
	tDWORD  dwCurrentDateTimeGetCounter;
	tDWORD  dwCurrentDate;
	tCHAR   szSFDBPathName[MAX_PATH*2];
	tCHAR   szSFDBSharedName[0x40];
	tDWORD  dwMapViewSize;
	LARGE_INTEGER liSyncOverhit;
	tDWORD  dwSyncCalls;
*/

	return errOK;
}


tERROR SecureFileDatabase_Close(hi_SecureFileDatabase _this)
{
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
    char aFileName [ PATH_MAX ] = {0};
    strncpy ( aFileName, ldata->szSFDBPathName, sizeof ( aFileName ) );
    strncpy ( aFileName + strlen ( aFileName ), LOCK_SUFFIX, sizeof ( aFileName ) - strlen ( aFileName )  - 1 ); 
	if ((ldata->dwSerializeOptions & cSFDB_AUTOSAVE) && ldata->szSFDBPathName[0] && ldata->pDB)
	{
		if (ldata->pDB->dwActiveClients == 1) // this is last client
			SecureFileDatabase_Save(_this, ldata->szSFDBPathName);
	}
#if defined (__unix__)
        if ( !flock ( ldata->LockFile, LOCK_EX | LOCK_NB ) ) {
          flock ( ldata->LockFile, LOCK_UN ); 
          close ( ldata->LockFile );
          unlink ( aFileName );
        }
        else {
          close ( ldata->LockFile );
          ldata->LockFile = -1;
        }
#else
        CloseHandle(ldata->LockFile);
		DeleteFile ( aFileName );
#endif
	CloseMappedFile(_this);

	free(_this->data);
	free(_this);

	return error;
}

tERROR SecureFileDatabase_SetSerializeOptions(hi_SecureFileDatabase _this, tDWORD dwOptions )
{
	LOCALDATA*ldata = (LOCALDATA*)(_this->data);

	ldata->dwSerializeOptions = dwOptions;

	return errOK;
}


//  Метод подготавливает новую базу к работе.
// Parameters are:
// "dwRecordIDAlg"    : Впоследствии, его можно получить через GetDBInfo.
// "dwRecordIDSize"   : Указывается в байтах
// "dwRecordDataSize" : Указывается в байтах
// "dwIndexSize"      : Задает начальный размер индекса базы данных.
tERROR pr_call SecureFileDatabase_InitNew( hi_SecureFileDatabase _this, tDWORD dwRecordIDAlg, tDWORD dwRecordIDSize, tDWORD dwRecordDataSize, tDWORD dwDBInitialSize ) {
	tERROR error = errOK;
	SYSTEM_INFO sSystemInfo;
	SFDB_HDR sDBHdr;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);

	if (PR_FAIL(error = LOCK_WRITE()))
	{
		PR_TRACE((_this, prtERROR, "Syncronisation failed with %terr", error));
		return error;
	}
	__TRY_F {
		__TRY {

			// try to connect to existing MMF
			error = OpenMappedFile(_this);

			// if connect failed, create new MMF
			if (PR_FAIL(error))
			{
				GetSystemInfo(&sSystemInfo);
				sDBHdr.dwSignature = SFDB_SIGNATURE; // SFDB
				sDBHdr.vDBVersion = 0x10000;

				sDBHdr.dwClusterSize = sSystemInfo.dwPageSize;
				
				sDBHdr.dwIndexBits = GetLeftBitPos(dwDBInitialSize/sDBHdr.dwClusterSize);
				if (sDBHdr.dwIndexBits)
					sDBHdr.dwIndexMask = ~((1 << (32-sDBHdr.dwIndexBits)) - 1);
				else
					sDBHdr.dwIndexMask = 0;
				
				sDBHdr.dwClusterHeaderSize = sizeof(SFDB_CLUSTER_HDR);
				sDBHdr.dwClusters = 1 << sDBHdr.dwIndexBits;
				
				sDBHdr.dwHeaderSize = (sizeof(SFDB_HDR) / sDBHdr.dwClusterSize + (sizeof(SFDB_HDR) % sDBHdr.dwClusterSize ?  1 : 0)) * sDBHdr.dwClusterSize;
				sDBHdr.dwFileSize = sDBHdr.dwHeaderSize + sDBHdr.dwClusters * sDBHdr.dwClusterSize;
				
				sDBHdr.dwRecordIDAlg = dwRecordIDAlg;
				sDBHdr.dwRecordIDSize = dwRecordIDSize;
				sDBHdr.dwRecordDataSize = dwRecordDataSize;
				sDBHdr.dwRecordSize = dwRecordIDSize + dwRecordDataSize + sizeof(SFDB_RECORD_INTERNAL_DATA) + sizeof(SFDB_RECORD_SESSION_DATA);
				sDBHdr.dwRecordsPerCluster = (sDBHdr.dwClusterSize - sDBHdr.dwClusterHeaderSize) / sDBHdr.dwRecordSize;
				sDBHdr.dwActiveClients = 1;
				
				error = CreateMappedFile(_this, &sDBHdr);
			}
			
			if (PR_SUCC(error))
			{
				if (ldata->pDB->dwRecordIDAlg != dwRecordIDAlg ||
					ldata->pDB->dwRecordIDSize != dwRecordIDSize ||
					ldata->pDB->dwRecordDataSize != dwRecordDataSize)
				{
					CloseMappedFile(_this);
					error = errOBJECT_ALREADY_EXISTS;
				}
			}
			
		}
		__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
			PR_TRACE(( _this, prtERROR, "SecureFileDatabase_InitNew: exception handled 0x%08X", GetExceptionCode()));
			error = errUNEXPECTED;
		}
	}
	__FINALLY {
		RELEASE_WRITE();
	}
	
	return error;
}
// AVP Prague stamp end



void __inline CryptBuffer(hi_SecureFileDatabase _this, LPCVOID lpBuffer, DWORD nNumberOfBytes)
{
	LOCALDATA* ldata = (LOCALDATA*)_this->data;
	DWORD dwKey = ldata->dwCryptKey;
	DWORD* pdwBuffer = (DWORD*)lpBuffer;
	DWORD i;
	for (i=0; i<(nNumberOfBytes/4);i++)
	{
		pdwBuffer[i] ^= dwKey;
		dwKey += pdwBuffer[i];
		dwKey ^= pdwBuffer[i];
	}
}

void __inline DecryptBuffer(hi_SecureFileDatabase _this, LPCVOID lpBuffer, DWORD nNumberOfBytes)
{
	LOCALDATA* ldata = (LOCALDATA*)_this->data;
	DWORD dwKey = ldata->dwCryptKey;
	DWORD* pdwBuffer = (DWORD*)lpBuffer;
	DWORD i;
	DWORD dwKeyNew;
	for (i=0; i<(nNumberOfBytes/4);i++)
	{
		dwKeyNew = dwKey;
		dwKeyNew += pdwBuffer[i];
		dwKeyNew ^= pdwBuffer[i];
		pdwBuffer[i] ^= dwKey;
		dwKey = dwKeyNew;
	}
	
}

BOOL WriteFileEncrypted(hi_SecureFileDatabase _this, HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
	BOOL bResult;
	DWORD dwKeyDiff = 0;
	LOCALDATA* ldata = (LOCALDATA*)_this->data;
	__TRY {
		if (nNumberOfBytesToWrite >= 4)
			dwKeyDiff = *(DWORD*)lpBuffer;
		CryptBuffer(_this, lpBuffer, nNumberOfBytesToWrite);
		bResult = WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
		DecryptBuffer(_this, lpBuffer, nNumberOfBytesToWrite);
		ldata->dwCryptKey = (ldata->dwCryptKey + dwKeyDiff) ^ dwKeyDiff;
	}
	__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
		PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Save: exception handled 0x%08X", GetExceptionCode()));
		CloseMappedFile(_this);
		bResult = cFALSE;
	}
	return bResult;
}

BOOL ReadFileEncrypted(hi_SecureFileDatabase _this, HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
	BOOL bResult;
	DWORD dwKeyDiff = 0;
	LOCALDATA* ldata = (LOCALDATA*)_this->data;
	__TRY {
		bResult = ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
		DecryptBuffer(_this, lpBuffer, nNumberOfBytesToRead);
		if (nNumberOfBytesToRead >= 4)
			dwKeyDiff = *(DWORD*)lpBuffer;
		ldata->dwCryptKey = (ldata->dwCryptKey + dwKeyDiff) ^ dwKeyDiff;
	}
	__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
		PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Save: exception handled 0x%08X", GetExceptionCode()));
		*lpNumberOfBytesRead = 0;
		bResult = cFALSE;
	}
	return bResult;
}



// AVP Prague stamp begin( External (user called) interface method implementation, Load )
// Parameters are:
tERROR pr_call SecureFileDatabase_Load( hi_SecureFileDatabase _this, tCHAR* szFileName ) {
	tERROR error = errOK;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	tDWORD dwClients = 0;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	char aFileName [ PATH_MAX ] = {0};
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::Load method" );
    strncpy ( aFileName, szFileName, sizeof ( aFileName ) );
    strncpy ( aFileName + strlen ( aFileName ), LOCK_SUFFIX, sizeof ( aFileName ) - strlen ( aFileName )  - 1 ); 
#if defined (__unix__)	
        if ( ldata->LockFile != -1 ) {
          close ( ldata->LockFile ); 
          ldata->LockFile = -1;
        }
        ldata->LockFile = open ( aFileName, O_CREAT|O_RDWR, S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP );
        if ( ldata->LockFile == -1) {
          return errUNEXPECTED;
        }
#else
        if ( ldata->LockFile != INVALID_HANDLE_VALUE ) {
          CloseHandle ( ldata->LockFile ); 
          ldata->LockFile = INVALID_HANDLE_VALUE;
        }
        ldata->LockFile = CreateFile (aFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0 );
        if ( ldata->LockFile == INVALID_HANDLE_VALUE) {
          return errUNEXPECTED;
		}
#endif          
	if (PR_FAIL(error = LOCK_WRITE()))
	{
		PR_TRACE((_this, prtERROR, "Syncronisation failed with %terr", error));
		return error;
	}
	__TRY_F {
		__TRY {
			if (strlen(szFileName) > sizeof(ldata->szSFDBPathName)+1)
			{
				PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Load: filename is too long"));
#if defined (__unix__)	
                                RELEASE_WRITE ();
#endif
				return errUNEXPECTED;
			}
			strcpy(ldata->szSFDBPathName, szFileName);
			
			if (ldata->pDB)
				dwClients = ldata->pDB->dwActiveClients;

			CloseMappedFile(_this);

#ifdef __JUST_COPY__
			if (!CopyFile(szFileName, ldata->szTempFile, FALSE)){
				PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Load: CopyFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
                                RELEASE_WRITE ();
#endif
				return errUNEXPECTED;
			}
			
			if (PR_FAIL(error = OpenMappedFile(_this, ldata->szTempFile))){
				PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Load: OpenMappedFile failed with error=0x%08X", error));
			}
#else
			{
				DWORD dwCluster;
				SFDB_CLUSTER_HDR* pCluster;
				tBYTE* pRecord;
				SFDB_HDR sDBHdr;
				tDWORD dwRecord;
				unsigned long ulBytes;
				tDWORD dwDataSize;
				SFDB_RECORD_SESSION_DATA* pRecordSessionData;
				SFDB_HDR* pDB;
				
				if (PR_SUCC(error = OpenMappedFile(_this))){
					PR_TRACE(( _this, prtALWAYS_REPORTED_MSG, "SecureFileDatabase_Load: connected to existing MMF, clients: %d", ldata->pDB->dwActiveClients));
#if defined (__unix__)	
                                        RELEASE_WRITE ();
#endif
					return error;
				}

				hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
				if (hFile == INVALID_HANDLE_VALUE){
					PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Load: CreateFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
                                        RELEASE_WRITE ();
#endif
					return errUNEXPECTED;
				}
								
				ReadFile(hFile, &sDBHdr, sizeof(SFDB_HDR), (DWORD*)&ulBytes, NULL);
				if (sizeof(SFDB_HDR) != ulBytes){
					PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Load: WriteFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
                                        RELEASE_WRITE ();
#endif
					return errUNEXPECTED;
				}

				if (sDBHdr.dwSignature != SFDB_SIGNATURE){
					PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Load: File signature not recognized. Wrong file name?"));
#if defined (__unix__)	
                                        RELEASE_WRITE ();
#endif
					return errNOT_MATCHED;
				}
				if (sDBHdr.vDBVersion != 0x00010000){
					PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Load: Database version is not supported."));
#if defined (__unix__)	
                                        RELEASE_WRITE ();
#endif
					return errNOT_SUPPORTED;
				}

				error = CreateMappedFile(_this, &sDBHdr);
				if (PR_FAIL(error)){
					PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Load: CreateMappedFile failed with error=0x%08X", error));
#if defined (__unix__)	
                                        RELEASE_WRITE ();
#endif
					return errUNEXPECTED;
				}

				pDB = ldata->pDB;
				
				ldata->dwCryptKey = *(DWORD*)&pDB->byFileCheckSum;
				//dwDataSize = pDB->dwRecordIDSize + pDB->dwRecordDataSize;
				dwDataSize = pDB->dwRecordSize - sizeof(SFDB_RECORD_SESSION_DATA);

				for (dwCluster=0; dwCluster<pDB->dwClusters; dwCluster++){
					pCluster = (SFDB_CLUSTER_HDR*)((tBYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwCluster * pDB->dwClusterSize);
					if (pDB->dwSerializeOptions & cSFDB_SERIALIZE_CRYPT)
						ReadFileEncrypted(_this, hFile, pCluster, sizeof(SFDB_CLUSTER_HDR), (DWORD*)&ulBytes, NULL);
					else
						ReadFile(hFile, pCluster, sizeof(SFDB_CLUSTER_HDR), (DWORD*)&ulBytes, NULL);
	
					PR_TRACE(( _this, prtNOTIFY, "SecureFileDatabase_Load: loading Cluster %d %08X %d", dwCluster, pCluster, pCluster->dwRecords));

					if (sizeof(SFDB_CLUSTER_HDR) != ulBytes){
						PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Load: ReadFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
                                                RELEASE_WRITE ();
#endif
						return errUNEXPECTED;
					}
					
					pRecord = (tBYTE*)pCluster+sizeof(SFDB_CLUSTER_HDR);
					for (dwRecord=0; dwRecord<pCluster->dwRecords; dwRecord++){
						if (pDB->dwSerializeOptions & cSFDB_SERIALIZE_CRYPT)
							ReadFileEncrypted(_this, hFile, pRecord, dwDataSize, (DWORD*)&ulBytes, NULL);
						else
							ReadFile(hFile, pRecord, dwDataSize, (DWORD*)&ulBytes, NULL);
	
						PR_TRACE(( _this, prtNOTIFY, "SecureFileDatabase_Load: loading Record %d %08X", dwRecord, pRecord));
						
						if (ulBytes != dwDataSize){
							PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Load: ReadFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
                                                        RELEASE_WRITE ();
#endif
							return errUNEXPECTED;
						}
						pRecordSessionData = (SFDB_RECORD_SESSION_DATA*)(pRecord + pDB->dwRecordSize - sizeof(SFDB_RECORD_SESSION_DATA));
						memset(pRecordSessionData, 0, sizeof(SFDB_RECORD_SESSION_DATA));
						pRecord += pDB->dwRecordSize;
					}
				}
				
				if (pDB->dwSerializeOptions & cSFDB_SERIALIZE_CHECKSUM)
				{
					BYTE byCheckSum[0x10];
					unsigned long ulBytes;
					tBYTE sMD5Ctx[0x80];
					tBOOL bRes;
					
					memcpy(byCheckSum, pDB->byFileCheckSum, sizeof(byCheckSum));
					memset(pDB->byFileCheckSum, 0, sizeof(pDB->byFileCheckSum));

					if (!hash_api_md5.Init(0, &sMD5Ctx, sizeof(sMD5Ctx), 16, NULL)) {
#if defined (__unix__)	
                                                RELEASE_WRITE ();
#endif
						return errUNEXPECTED;
                                        }
					dwClients = pDB->dwActiveClients;
					pDB->dwActiveClients = 0;
					bRes = hash_api_md5.Update(&sMD5Ctx, pDB, pDB->dwHeaderSize);
					pDB->dwActiveClients = dwClients;
					if (!bRes) {
#if defined (__unix__)	
                                                RELEASE_WRITE ();
#endif
						return errUNEXPECTED;
                                        }
					for (dwCluster=0; dwCluster<pDB->dwClusters; dwCluster++)
					{
						pCluster = (SFDB_CLUSTER_HDR*)((tBYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwCluster * pDB->dwClusterSize);
						PR_TRACE(( _this, prtNOTIFY, "SecureFileDatabase_Load: checking Cluster %d %08X %d", dwCluster, pCluster, pCluster->dwRecords));
						pRecord = (tBYTE*)pCluster+sizeof(SFDB_CLUSTER_HDR);
						for (dwRecord=0; dwRecord<pCluster->dwRecords; dwRecord++)
						{
							PR_TRACE(( _this, prtNOTIFY, "SecureFileDatabase_Load: checking Record %d %08X", dwRecord, pRecord));
							if (!hash_api_md5.Update(&sMD5Ctx, pRecord, dwDataSize)) {
#if defined (__unix__)	
                                                                RELEASE_WRITE ();
#endif
								return errUNEXPECTED;
                                                        }
							pRecord += pDB->dwRecordSize;
						}
					}
					
					// uninitialized variable - fixed by Mike 07.10.2002
					ulBytes = sizeof(pDB->byFileCheckSum);

					if (!hash_api_md5.Finalize(&sMD5Ctx, &ulBytes, pDB->byFileCheckSum))
					{
						PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Load: CheckSum hash_api_md5.Finalize failed!"));
#if defined (__unix__)	
                                                RELEASE_WRITE ();
#endif
						return errUNEXPECTED;
					}
					else if (memcmp(byCheckSum, pDB->byFileCheckSum, sizeof(byCheckSum)) != 0)
					{
						PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Load: CheckSum not match was %08X%08X%08X%08X now %08X%08X%08X%08X", *((tQWORD*)byCheckSum), *(((tQWORD*)byCheckSum)+1), *((tQWORD*)pDB->byFileCheckSum), *(((tQWORD*)pDB->byFileCheckSum)+1)));
#if defined (__unix__)	
                                                RELEASE_WRITE ();
#endif
						return errUNEXPECTED;
					}
				}

				CloseHandle(hFile);
				hFile = INVALID_HANDLE_VALUE;
			}
			
#endif
		}
		__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
			PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Load: exception handled 0x%08X", GetExceptionCode()));
			error = errUNEXPECTED;
		}
	}
	__FINALLY {
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
		RELEASE_WRITE();
	}
	PR_TRACE_A1( _this, "Leave SecureFileDatabase::Load method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Save )
// Parameters are:
tERROR pr_call SecureFileDatabase_Save( hi_SecureFileDatabase _this, tCHAR* szFileName ) {
	tERROR error = errOK;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	tDWORD dwClients;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::Save method" );
	
	if (PR_FAIL(error = LOCK_WRITE()))
	{
		PR_TRACE((_this, prtERROR, "Syncronisation failed with %terr", error));
		return error;
	}
	__TRY_F {
		__TRY {
			if (ldata->pDB == NULL || ldata->pMappingView == NULL){
                          PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Save: database not initialized"));
#if defined (__unix__)	
                                if (hFile != INVALID_HANDLE_VALUE)
                                  CloseHandle(hFile);
                                RELEASE_WRITE();
#endif
				return errOBJECT_NOT_INITIALIZED;
			}

			// sync map view size
			if (ldata->dwMapViewSize != ldata->pDB->dwFileSize)
			{
				if (PR_FAIL(error = RemapViewOfFile(_this, ldata->pDB->dwFileSize)))
				{
					PR_TRACE(( _this, prtERROR, "RemapViewOfFile failed with %terr", error));
#if defined (__unix__)	
                                        if (hFile != INVALID_HANDLE_VALUE)
                                          CloseHandle(hFile);
                                        RELEASE_WRITE();
#endif
					return error;
				}
			}
			
			if (ldata->dwSerializeOptions & cSFDB_SERIALIZE_COMPACT_ON_SAVE)
				CompactDatabase(_this);

			if (!FlushViewOfFile(ldata->pMappingView, 0)){
				PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Save: FlushViewOfFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
                                if (hFile != INVALID_HANDLE_VALUE)
                                  CloseHandle(hFile);
                                RELEASE_WRITE();
#endif
				return errUNEXPECTED;
			}

#ifdef __JUST_COPY__
			if (!CopyFile(ldata->szTempFile, szFileName, FALSE)){
				PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Save: CopyFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
                                if (hFile != INVALID_HANDLE_VALUE)
                                  CloseHandle(hFile);
                                RELEASE_WRITE();
#endif
				return errUNEXPECTED;
			}
#else
			{
				DWORD dwCluster;
				SFDB_CLUSTER_HDR* pCluster;
				tBYTE* pRecord;
				SFDB_HDR* pDB = ldata->pDB;
				tDWORD dwRecord;
				unsigned long ulBytes;
				tDWORD dwDataSize;
				SFDB_RECORD_SESSION_DATA* pRecordSessionData;
				tBYTE sMD5Ctx[0x80];

				hFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
				if (hFile == INVALID_HANDLE_VALUE){
					PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Save: CreateFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
                                        if (hFile != INVALID_HANDLE_VALUE)
                                          CloseHandle(hFile);
                                        RELEASE_WRITE();
#endif
					return errUNEXPECTED;
				}

				//dwDataSize = pDB->dwRecordIDSize + pDB->dwRecordDataSize;
				dwDataSize = pDB->dwRecordSize - sizeof(SFDB_RECORD_SESSION_DATA);

				pDB->dwSerializeOptions = ldata->dwSerializeOptions;

				// cleanup garbage
				for (dwCluster=0; dwCluster<pDB->dwClusters; dwCluster++){
					pCluster = (SFDB_CLUSTER_HDR*)((tBYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwCluster * pDB->dwClusterSize);
					pRecord = (tBYTE*)pCluster+sizeof(SFDB_CLUSTER_HDR);
					if (pCluster->dwRecords > pDB->dwRecordsPerCluster) // validation
					{
						PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Save: Cluster %d contain %d records? (RecordsPerCluster=%d)", dwCluster, pCluster->dwRecords, pDB->dwRecordsPerCluster));
#if defined (__unix__)	
                                                if (hFile != INVALID_HANDLE_VALUE)
                                                  CloseHandle(hFile);
                                                RELEASE_WRITE();
#endif
						return errUNEXPECTED;
					}
					for (dwRecord=0; dwRecord<pCluster->dwRecords; dwRecord++){
						pRecordSessionData = (SFDB_RECORD_SESSION_DATA*)(pRecord + pDB->dwRecordSize - sizeof(SFDB_RECORD_SESSION_DATA));
						memset(pRecordSessionData, 0, sizeof(SFDB_RECORD_SESSION_DATA));
						pRecord += pDB->dwRecordSize;
					}
				}
				memset(pDB->byFileCheckSum, 0, sizeof(pDB->byFileCheckSum));
				
				// calculate database checksum
				if (ldata->dwSerializeOptions & cSFDB_SERIALIZE_CHECKSUM)
				{
					tBOOL bRes;
					if (!hash_api_md5.Init(0, &sMD5Ctx, sizeof(sMD5Ctx), 16, NULL)) {
#if defined (__unix__)	
                                          if (hFile != INVALID_HANDLE_VALUE)
                                            CloseHandle(hFile);
                                          RELEASE_WRITE();
#endif
						return errUNEXPECTED;
                                        }
					dwClients = ldata->pDB->dwActiveClients;
					pDB->dwActiveClients = 0;
					bRes = hash_api_md5.Update(&sMD5Ctx, pDB, pDB->dwHeaderSize);
					ldata->pDB->dwActiveClients = dwClients;
					if (!bRes) {
#if defined (__unix__)	
                                          if (hFile != INVALID_HANDLE_VALUE)
                                            CloseHandle(hFile);
                                          RELEASE_WRITE();
#endif
						return errUNEXPECTED;
                                        }
					for (dwCluster=0; dwCluster<pDB->dwClusters; dwCluster++)
					{
						pCluster = (SFDB_CLUSTER_HDR*)((tBYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwCluster * pDB->dwClusterSize);
						pRecord = (tBYTE*)pCluster+sizeof(SFDB_CLUSTER_HDR);
						for (dwRecord=0; dwRecord<pCluster->dwRecords; dwRecord++)
						{
                                                        if (!hash_api_md5.Update(&sMD5Ctx, pRecord, dwDataSize)) {
#if defined (__unix__)	
                                                          if (hFile != INVALID_HANDLE_VALUE)
                                                            CloseHandle(hFile);
                                                          RELEASE_WRITE();
#endif
								return errUNEXPECTED;
                                                        }
							pRecord += pDB->dwRecordSize;
						}
					}

					// uninitialized variable - fixed by Mike 07.10.2002
					ulBytes = sizeof(pDB->byFileCheckSum);

					if (!hash_api_md5.Finalize(&sMD5Ctx, &ulBytes, pDB->byFileCheckSum))
					{
						PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Save: CheckSum failed"));
#if defined (__unix__)	
                                                if (hFile != INVALID_HANDLE_VALUE)
                                                  CloseHandle(hFile);
                                                RELEASE_WRITE();
#endif
						return errUNEXPECTED;
					}
				}

				// init crypt key
				if (ldata->dwSerializeOptions & cSFDB_SERIALIZE_CRYPT)
					ldata->dwCryptKey = *(DWORD*)&pDB->byFileCheckSum;

				// save database header
				WriteFile(hFile, pDB, sizeof(SFDB_HDR), (DWORD*)&ulBytes, NULL);
				if (sizeof(SFDB_HDR) != ulBytes){
					PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Save: WriteFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
                                        if (hFile != INVALID_HANDLE_VALUE)
                                          CloseHandle(hFile);
                                        RELEASE_WRITE();
#endif
					return errUNEXPECTED;
				}

				// save clusters
				for (dwCluster=0; dwCluster<pDB->dwClusters; dwCluster++){
					pCluster = (SFDB_CLUSTER_HDR*)((tBYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwCluster * pDB->dwClusterSize);
					if (ldata->dwSerializeOptions & cSFDB_SERIALIZE_CRYPT)
						WriteFileEncrypted(_this, hFile, pCluster, sizeof(SFDB_CLUSTER_HDR), (DWORD*)&ulBytes, NULL);
					else
						WriteFile(hFile, pCluster, sizeof(SFDB_CLUSTER_HDR), (DWORD*)&ulBytes, NULL);
					if (sizeof(SFDB_CLUSTER_HDR) != ulBytes){
						PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Save: WriteFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
                                                if (hFile != INVALID_HANDLE_VALUE)
                                                  CloseHandle(hFile);
                                                RELEASE_WRITE();
#endif
						return errUNEXPECTED;
					}

					// save records (in cluster)
					pRecord = (tBYTE*)pCluster+sizeof(SFDB_CLUSTER_HDR);
					for (dwRecord=0; dwRecord<pCluster->dwRecords; dwRecord++){
						if (ldata->dwSerializeOptions & cSFDB_SERIALIZE_CRYPT)
							WriteFileEncrypted(_this, hFile, pRecord, dwDataSize,(DWORD*) &ulBytes, NULL);
						else
							WriteFile(hFile, pRecord, dwDataSize, (DWORD*)&ulBytes, NULL);
						if (ulBytes != dwDataSize){
							PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Save: WriteFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
                                                        if (hFile != INVALID_HANDLE_VALUE)
                                                          CloseHandle(hFile);
                                                        RELEASE_WRITE();
#endif
							return errUNEXPECTED;
						}
						pRecord += pDB->dwRecordSize;
					}
				}

				FlushFileBuffers(hFile);
				CloseHandle(hFile);
				hFile = INVALID_HANDLE_VALUE;
			}
#endif
		}
		__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
			PR_TRACE(( _this, prtERROR, "SecureFileDatabase_Save: exception handled 0x%08X", GetExceptionCode()));
			error = errUNEXPECTED;
		}
	}
	__FINALLY {
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
		RELEASE_WRITE();
	}
	PR_TRACE_A1( _this, "Leave SecureFileDatabase::Save method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetRecord )
// Extended method comment
//  Метод находит в базе данных запись по указанному идентификатору и возвращает данные, хранимые в ней. Размер данных можно получить через GetDBInfo.
// Parameters are:
tERROR pr_call SecureFileDatabase_GetRecord( hi_SecureFileDatabase _this, tVOID* pRecordID, tVOID* pRecordData ) {
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	SFDB_HDR* pDB;
	tDWORD dwIndex;
	SFDB_CLUSTER_HDR* pCluster;
	tBYTE* pRecord;
	SFDB_RECORD_SESSION_DATA* pRecordSessionData;
	
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::GetRecord method" );

	if (PR_FAIL(error = LOCK_READ()))
	{
		PR_TRACE((_this, prtERROR, "Syncronisation failed with %terr", error));
		return error;
	}
	__TRY_F {
		__TRY {
		
			PR_TRACE(( _this, prtNOTIFY, "SecureFileDatabase_GetRecord"));
			if (ldata->pDB == NULL){
				PR_TRACE(( _this, prtERROR, "SecureFileDatabase_GetRecord: database not initialized"));
#if defined (__unix__)		
                                RELEASE ();
#endif
                                return errOBJECT_NOT_INITIALIZED;
			}

			// sync map view size
			if (ldata->dwMapViewSize != ldata->pDB->dwFileSize)
			{
				if (PR_FAIL(error = RemapViewOfFile(_this, ldata->pDB->dwFileSize)))
				{
					PR_TRACE(( _this, prtERROR, "RemapViewOfFile failed with %terr", error));
#if defined (__unix__)		
                                        RELEASE ();
#endif

					return error;
				}
			}

			pDB = ldata->pDB;
			dwIndex = ldata->GetIndex(ldata, pRecordID);
			PR_TRACE(( _this, prtNOTIFY, "SecureFileDatabase_GetRecord: dwIndex=%d", dwIndex));
			pCluster = (SFDB_CLUSTER_HDR*)((BYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwIndex * pDB->dwClusterSize);
			PR_TRACE(( _this, prtNOTIFY, "SecureFileDatabase_GetRecord: pCluster=%08X RecordsInCluster=%d", pCluster, pCluster->dwRecords));
			PR_TRACE(( _this, prtNOTIFY, "SecureFileDatabase_GetRecord: bsearch(%08X, %08X, %08X, %08X, %08X)", pRecordID, (tBYTE*)pCluster+sizeof(SFDB_CLUSTER_HDR), pCluster->dwRecords, pDB->dwRecordSize, ldata->RecordCompareFunc));
			pRecord = (tBYTE*)bsearch(pRecordID, (tBYTE*)pCluster+sizeof(SFDB_CLUSTER_HDR), pCluster->dwRecords, pDB->dwRecordSize, ldata->RecordCompareFunc);

			PR_TRACE(( _this, prtNOTIFY, "SecureFileDatabase_GetRecord: pRecord=%08X", pRecord));
			if (pRecord == NULL){
				PR_TRACE(( _this, prtNOTIFY, "SecureFileDatabase_GetRecord: record not found"));
#if defined (__unix__)		
                                RELEASE ();
#endif

				return errNOT_FOUND;
			}
			
			if (pRecordData != NULL)
				memcpy(pRecordData, pRecord + pDB->dwRecordIDSize, pDB->dwRecordDataSize);
			
			pRecordSessionData = (SFDB_RECORD_SESSION_DATA*)(pRecord + pDB->dwRecordSize - sizeof(SFDB_RECORD_INTERNAL_DATA));
                        pRecordSessionData->dwUsageCount++;

			// set last used date timestamp
			{
				tDWORD dwCurrentDate;
				SFDB_RECORD_INTERNAL_DATA* pRecordInternalData = (SFDB_RECORD_INTERNAL_DATA*)pRecordSessionData - 1;
				GetCurrentDateTime(_this, &dwCurrentDate);
                                pRecordInternalData->dwLastUsedDate = dwCurrentDate;
			}
		}
		__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
			PR_TRACE(( _this, prtERROR, "SecureFileDatabase_GetRecord: exception handled 0x%08X", GetExceptionCode()));
			error = errUNEXPECTED;
		}
	}
	__FINALLY {
		RELEASE();
	}
	PR_TRACE_A1( _this, "Leave SecureFileDatabase::GetRecord method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, UpdateRecord )
// Behaviour comment
//  Если запись с указанным идентификатором в базе отсутствовала, метод добавит новую запись.
// Parameters are:
// "pRecordData" : Данные сохраняются в базе данных в указанной записи
tERROR pr_call SecureFileDatabase_UpdateRecord( hi_SecureFileDatabase _this, tVOID* pRecordID, tVOID* pRecordData ) {
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	SFDB_HDR* pDB;
	tDWORD dwIndex;
	SFDB_CLUSTER_HDR* pCluster;
	tBYTE* pRecord;
	tDWORD i;
	SFDB_RECORD_SESSION_DATA* pRecordSessionData;
	
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::UpdateRecord method" );
	if (PR_FAIL(error = LOCK_WRITE()))
	{
		PR_TRACE((_this, prtERROR, "Syncronisation failed with %terr", error));
		return error;
	}
	__TRY_F {
		__TRY {
		
			if (ldata->pDB == NULL){
				PR_TRACE(( _this, prtERROR, "SecureFileDatabase_UpdateRecord: database not initialized"));
#if defined (__unix__)		
                                RELEASE_WRITE ();
#endif

				return errOBJECT_NOT_INITIALIZED;
			}

			// sync map view size
			if (ldata->dwMapViewSize != ldata->pDB->dwFileSize)
			{
				if (PR_FAIL(error = RemapViewOfFile(_this, ldata->pDB->dwFileSize)))
				{
					PR_TRACE(( _this, prtERROR, "RemapViewOfFile failed with %terr", error));
#if defined (__unix__)		
                                        RELEASE_WRITE ();
#endif

					return error;
				}
			}

			pDB = ldata->pDB;
			dwIndex = ldata->GetIndex(ldata, pRecordID);
			pCluster = (SFDB_CLUSTER_HDR*)((BYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwIndex * pDB->dwClusterSize);
			pRecord = (tBYTE*)bsearch(pRecordID, (tBYTE*)pCluster+sizeof(SFDB_CLUSTER_HDR), pCluster->dwRecords, pDB->dwRecordSize, ldata->RecordCompareFunc);
			if (pRecord == NULL){
				// add new record
				if (pCluster->dwRecords == pDB->dwRecordsPerCluster)
				{
					//cluster is full
                                  if (PR_FAIL(error = DeleteOldestRecordsInCluster(_this, pCluster))) {
#if defined (__unix__)		
                                                RELEASE_WRITE ();
#endif

						return error;
                                  }
				}
				pRecord = (tBYTE*)pCluster+sizeof(SFDB_CLUSTER_HDR);
				for (i=0; i<pCluster->dwRecords; i++){
					if (ldata->RecordCompareFunc(pRecord, pRecordID) > 0){
						// shift records down
						memmove(pRecord+pDB->dwRecordSize, pRecord, (pCluster->dwRecords - i) * pDB->dwRecordSize);
						break;
					}
					pRecord+=pDB->dwRecordSize;
				}
				pRecordSessionData = (SFDB_RECORD_SESSION_DATA*)(pRecord + pDB->dwRecordSize - sizeof(SFDB_RECORD_SESSION_DATA));
				memcpy(pRecord, pRecordID, pDB->dwRecordIDSize);
				memset(pRecordSessionData, 0, sizeof(SFDB_RECORD_SESSION_DATA));
				pCluster->dwRecords++;
			}
			else{
				error = errUSER_DEFINED_WARN + 1; // already exist
			        pRecordSessionData = (SFDB_RECORD_SESSION_DATA*)(pRecord + pDB->dwRecordSize - sizeof(SFDB_RECORD_SESSION_DATA));
			}
			memcpy(pRecord + pDB->dwRecordIDSize, pRecordData, pDB->dwRecordDataSize);
                                
                        pRecordSessionData->dwUsageCount++;

			// set last used date timestamp
			{
				tDWORD dwCurrentDate;
				SFDB_RECORD_INTERNAL_DATA* pRecordInternalData = (SFDB_RECORD_INTERNAL_DATA*)pRecordSessionData - 1;
				GetCurrentDateTime(_this, &dwCurrentDate);
                                pRecordInternalData->dwLastUsedDate = dwCurrentDate;
			}
			
			while (pCluster->dwRecords == pDB->dwRecordsPerCluster){
				if (PR_FAIL(ExpandDatabase(_this)))
					break;
				pDB = ldata->pDB;
				pCluster = (SFDB_CLUSTER_HDR*)((BYTE*)ldata->pMappingView + pDB->dwHeaderSize + ((ldata->GetIndex(ldata, pRecordID)) * pDB->dwClusterSize));
			}
		}
		__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
			PR_TRACE(( _this, prtERROR, "SecureFileDatabase_UpdateRecord: exception handled 0x%08X", GetExceptionCode()));
			error = errUNEXPECTED;
		}
	}
	__FINALLY {
		RELEASE_WRITE();
	}
	
	
	PR_TRACE_A1( _this, "Leave SecureFileDatabase::UpdateRecord method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, DeleteRecord )
// Extended method comment
//  Удаляет указанную запись вместе с ее данными
// Parameters are:
tERROR pr_call SecureFileDatabase_DeleteRecord( hi_SecureFileDatabase _this, tVOID* pRecordID ) {
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	SFDB_HDR* pDB;
	tDWORD dwIndex;
	SFDB_CLUSTER_HDR* pCluster;
	tBYTE* pRecord;
	tBYTE* pNextRecord;
	tBYTE* pLastRecord;
	
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::DeleteRecord method" );

	if (PR_FAIL(error = LOCK_WRITE()))
	{
		PR_TRACE((_this, prtERROR, "Syncronisation failed with %terr", error));
		return error;
	}
	__TRY_F {
		__TRY {
			if (ldata->pDB == NULL){
				PR_TRACE(( _this, prtERROR, "SecureFileDatabase_DeleteRecord: database not initialized"));
				return errOBJECT_NOT_INITIALIZED;
			}
			
			// sync map view size
			if (ldata->dwMapViewSize != ldata->pDB->dwFileSize)
			{
				if (PR_FAIL(error = RemapViewOfFile(_this, ldata->pDB->dwFileSize)))
				{
                                  PR_TRACE(( _this, prtERROR, "RemapViewOfFile failed with %terr", error)); 
#if defined (__unix__)		
                                        RELEASE_WRITE ();
#endif
					return error;
                                  
				}
			}

			pDB = ldata->pDB;
			dwIndex = ldata->GetIndex(ldata, pRecordID);
			pCluster = (SFDB_CLUSTER_HDR*)((BYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwIndex * pDB->dwClusterSize);
			pRecord = (tBYTE*)bsearch(pRecordID, (tBYTE*)pCluster+sizeof(SFDB_CLUSTER_HDR), pCluster->dwRecords, pDB->dwRecordSize, ldata->RecordCompareFunc);
			if (pRecord == NULL){
				PR_TRACE(( _this, prtNOTIFY, "SecureFileDatabase_DeleteRecord: record not found"));
#if defined (__unix__)		
                                RELEASE_WRITE ();
#endif

				return errNOT_FOUND;
			}
			
			pNextRecord = pRecord + pDB->dwRecordSize;
			pLastRecord = (tBYTE*)pCluster + sizeof(SFDB_CLUSTER_HDR) + pDB->dwRecordSize * pCluster->dwRecords;
			
			// shift records up
			memmove(pRecord, pNextRecord, pLastRecord-pNextRecord);
			pCluster->dwRecords--;
#ifdef _DEBUG
			pLastRecord -= pDB->dwRecordSize;
			memset(pLastRecord, 0, pDB->dwRecordSize);
#endif
		}
		__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
			PR_TRACE(( _this, prtERROR, "SecureFileDatabase_DeleteRecord: exception handled 0x%08X", GetExceptionCode()));
			error = errUNEXPECTED;
		}
	}
	__FINALLY {
		RELEASE_WRITE();
	}
	
	PR_TRACE_A1( _this, "Leave SecureFileDatabase::DeleteRecord method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetDBInfo )
// Parameters are:
// "pRecordIDAlg"    : Получает алгоритм определения идентификатора записи, использовавшийся при создании базы данных
// "pRecordIDSize"   : Определяется в байтах
// "pRecordDataSize" : Определяется в байтах
tERROR pr_call SecureFileDatabase_GetDBInfo( hi_SecureFileDatabase _this, tDWORD* pRecordIDAlg, tDWORD* pRecordIDSize, tDWORD* pRecordDataSize ) {
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::GetDBInfo method" );
	
	if (PR_FAIL(error = LOCK_READ()))
	{
		PR_TRACE((_this, prtERROR, "Syncronisation failed with %terr", error));
		return error;
	}
	__TRY_F {
		__TRY {
			if (ldata->pDB == NULL){
				PR_TRACE(( _this, prtERROR, "SecureFileDatabase_GetDBInfo: database not initialized"));
#if defined (__unix__)		
                                RELEASE ();
#endif

				return errOBJECT_NOT_INITIALIZED;
			}

			if (pRecordIDAlg)
				*pRecordIDAlg = ldata->pDB->dwRecordIDAlg;
			
			if (pRecordIDSize)
				*pRecordIDSize = ldata->pDB->dwRecordIDSize;
			
			if (pRecordDataSize)
				*pRecordDataSize = ldata->pDB->dwRecordDataSize;
		}
		__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
			PR_TRACE(( _this, prtERROR, "SecureFileDatabase_GetDBInfo: exception handled 0x%08X", GetExceptionCode()));
			error = errUNEXPECTED;
		}
	}
	__FINALLY {
		RELEASE();
	}
	PR_TRACE_A1( _this, "Leave SecureFileDatabase::GetDBInfo method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



