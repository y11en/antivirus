#include "fcache.h"

BOOL OpenCachedFile(IN CHAR* szFileName, OUT CACHED_FILE* pCachedFile)
{
	memset(pCachedFile, 0, sizeof(*pCachedFile));
	lstrcpy(pCachedFile->szFileName, szFileName);
	return RefreshCachedFile(pCachedFile);
}

BOOL RefreshCachedFile(IN OUT CACHED_FILE* pCachedFile)
{
	WIN32_FILE_ATTRIBUTE_DATA sFAD;
	HANDLE hFile;

	if (!GetFileAttributesEx(pCachedFile->szFileName, GetFileExInfoStandard, &sFAD))
		return FALSE;

	sFAD.ftLastAccessTime.dwLowDateTime = 0;
	sFAD.ftLastAccessTime.dwHighDateTime = 0;

	if (memcmp(&pCachedFile->sFAD, &sFAD, sizeof(sFAD)) != 0)
	{
		hFile = CreateFile(pCachedFile->szFileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwRead;
			if (pCachedFile->pFileData == NULL)
				pCachedFile->pFileData = (CHAR*)HeapAlloc(GetProcessHeap(), 0, sFAD.nFileSizeLow+1);
			else
				pCachedFile->pFileData = (CHAR*)HeapReAlloc(GetProcessHeap(), 0, pCachedFile->pFileData, sFAD.nFileSizeLow+1);
			if (pCachedFile->pFileData == NULL)
			{
				CloseHandle(hFile);
				CloseCachedFile(pCachedFile);
				return FALSE;
			}
			pCachedFile->pFileData[sFAD.nFileSizeLow] = 0;
			ReadFile(hFile, pCachedFile->pFileData, sFAD.nFileSizeLow, &dwRead, NULL);
			CloseHandle(hFile);
			memcpy(&pCachedFile->sFAD, &sFAD, sizeof(sFAD));
		}
	}
	return TRUE;
}

BOOL CloseCachedFile(IN OUT CACHED_FILE* pCachedFile)
{
	if (pCachedFile->pFileData)
		HeapFree(GetProcessHeap(), 0, pCachedFile->pFileData);
	pCachedFile->pFileData = NULL;
	memset(&pCachedFile->sFAD, 0, sizeof(pCachedFile->sFAD));
	return TRUE;
}
