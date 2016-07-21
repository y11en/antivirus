/*#include "cmpimages.h"
#include "stuff/debug.h"
#include "../../../../release/klava/klava/kernel/treesearchlib/tree.h"

typedef struct tag_TREE_SEARCH_CONTEXT { 
	HANDLE hFile1;
	unsigned long dwScanStartPos;
	unsigned long dwScanSize;
	HANDLE hFile2;
	bool  bFound;
	unsigned long dwFoundPos;
} TREE_SEARCH_CONTEXT;

#define BUFF_SIZE 0x1000
#define MAX_SCAN_FILE_SIZE 1024*1024 // 1Mb

// ---------------------------------------------------------------
// heap helpers

void* MyHeapAlloc(void* pHeap, unsigned int dwBytes)
{
	return HeapAlloc(pHeap, HEAP_ZERO_MEMORY, dwBytes);
}

void* MyHeapReAlloc(void* pHeap, void* pMem, unsigned int dwBytes)
{
	if (pMem)
		return HeapReAlloc(pHeap, HEAP_ZERO_MEMORY, pMem, dwBytes); 
	else
		return HeapAlloc(pHeap, HEAP_ZERO_MEMORY, dwBytes);
}

void  MyHeapFree(void* pHeap, void* pMem)
{
	HeapFree(pHeap, 0, pMem);
}

// ---------------------------------------------------------------

bool IsEqualFileData(HANDLE hFile1, unsigned long dwPos1, unsigned long dwSize1,
					  HANDLE hFile2, unsigned long dwPos2, unsigned long dwSize2)
{
	__int8  bBuff1[BUFF_SIZE], bBuff2[BUFF_SIZE];
	unsigned long dwRead, dwSize;
	unsigned long dwBytes1, dwBytes2;

	if (dwSize1 != dwSize2 || dwSize1 == 0)
	{
		DbPrint(( DCB_BL, DL_ERROR, _T("IsEqualFileData: Images size mismatch (%X!=%X)"), dwSize1, dwSize2));
		return false;
	}
	if (dwPos1 != SetFilePointer(hFile1, dwPos1, NULL, FILE_BEGIN))
	{
		DbPrint(( DCB_BL, DL_ERROR, _T("IsEqualFileData: SetFilePointer for file1 failed with err=%08X"), GetLastError()));
		return false;
	}
	if (dwPos2 != SetFilePointer(hFile2, dwPos2, NULL, FILE_BEGIN))
	{
		DbPrint(( DCB_BL, DL_ERROR, _T("IsEqualFileData: SetFilePointer for file2 failed with err=%08X"), GetLastError()));
		return false;
	}

	dwSize = dwSize1;
	while (dwSize) 
	{
		dwRead = min(BUFF_SIZE, dwSize); 
		ReadFile(hFile1, bBuff1, dwRead, &dwBytes1, NULL);
		ReadFile(hFile2, bBuff2, dwRead, &dwBytes2, NULL);
		if (dwBytes1 != dwRead || dwBytes2 != dwRead)
		{
			DbPrint(( DCB_BL, DL_ERROR, _T("IsEqualFileData: Data size mismatch")));
			return false;
		}
		if (memcmp(bBuff1, bBuff2, dwRead) != 0)
		{
			DbPrint(( DCB_BL, DL_NOTIFY, _T("IsEqualFileData: Data mismatch")));
			return false;
		}
		dwSize -= dwRead;
	};

	return true;
}


bool GetImageSize(HANDLE hFile, unsigned long dwPos, __int8* pBuff, unsigned long dwBuffSize, unsigned long* pdwImageSize)
{
	IMAGE_DOS_HEADER* pImageDosHeader = (IMAGE_DOS_HEADER*)pBuff;
	WORD* pWordBuffer = (WORD*)pBuff;
	unsigned long dwBytes;
	unsigned long dwImageSize = 0;

	if (dwPos != SetFilePointer(hFile, dwPos, NULL, FILE_BEGIN))
	{
		DbPrint(( DCB_BL, DL_ERROR, _T("GetImageSize: SetFilePointer for file1 failed with err=%08X"), GetLastError()));
		return false;
	}

	if (!ReadFile(hFile, pBuff, BUFF_SIZE, &dwBytes, NULL))
	{
		DbPrint(( DCB_BL, DL_ERROR, _T("GetImageSize: ReadFile for file1 failed with err=%08X"), GetLastError()));
		return false;
	}

	if (dwBytes < sizeof(IMAGE_DOS_HEADER))
	{
		DbPrint(( DCB_BL, DL_ERROR, _T("GetImageSize: dwBytes(0x%x) < sizeof(IMAGE_DOS_HEADER)"), dwBytes));
		return false;
	}

	if( pImageDosHeader->e_magic!=0x4D5A && pImageDosHeader->e_magic!=0x5A4D) // MZ or ZM
	{
		DbPrint(( DCB_BL, DL_ERROR, _T("GetImageSize: pImageDosHeader->e_magic(%x) != 'MZ'"), pImageDosHeader->e_magic));
		return false;
	}

	{
		unsigned long dwHeaderOffset = dwPos + (unsigned long)(pImageDosHeader->e_lfanew);
		PIMAGE_NT_HEADERS pPE = (PIMAGE_NT_HEADERS)pBuff;
		PIMAGE_SECTION_HEADER pSH;
		WORD wSections,i;

		if (dwHeaderOffset != SetFilePointer(hFile, dwHeaderOffset, NULL, FILE_BEGIN))
		{
			DbPrint(( DCB_BL, DL_ERROR, _T("GetImageSize: SetFilePointer for file1 failed with err=%08X"), GetLastError()));
			return false;
		}

		if (!ReadFile(hFile, pBuff, sizeof(IMAGE_NT_HEADERS), &dwBytes, NULL))
		{
			DbPrint(( DCB_BL, DL_ERROR, _T("GetImageSize: ReadFile for file1 failed with err=%08X"), GetLastError()));
			return false;
		}

		if (dwBytes < sizeof(IMAGE_NT_HEADERS))
		{
			DbPrint(( DCB_BL, DL_ERROR, _T("GetImageSize: dwBytes(0x%x) < sizeof(IMAGE_NT_HEADERS)"), dwBytes));
			return false;
		}
		
		wSections = pPE->FileHeader.NumberOfSections;
		dwBytes = 0;
		for(i=0; i<wSections; i++)
		{
			if (dwBytes < sizeof(IMAGE_SECTION_HEADER))
			{
				if (!ReadFile(hFile, pBuff, min(wSections*sizeof(IMAGE_SECTION_HEADER), (BUFF_SIZE/sizeof(IMAGE_SECTION_HEADER))*sizeof(IMAGE_SECTION_HEADER)), &dwBytes, NULL))
				{
					DbPrint(( DCB_BL, DL_ERROR, _T("GetImageSize: ReadFile for file1 failed with err=%08X"), GetLastError()));
					return false;
				}
				if (dwBytes < sizeof(IMAGE_SECTION_HEADER))
				{
					DbPrint(( DCB_BL, DL_ERROR, _T("GetImageSize: dwBytes(0x%x) < sizeof(IMAGE_SECTION_HEADER)"), dwBytes));
					return false;
				}
				pSH = (PIMAGE_SECTION_HEADER)pBuff;
			}

			if(pSH->PointerToRawData + pSH->SizeOfRawData > dwImageSize)
				dwImageSize = pSH->PointerToRawData + pSH->SizeOfRawData;
			
			pSH++;
			dwBytes -= sizeof(IMAGE_SECTION_HEADER);
		}
	}

	*pdwImageSize = dwImageSize;
	DbPrint(( DCB_BL, DL_NOTIFY, _T("GetImageSize: dwImageSize = 0x%x"), dwImageSize));

	return true;
}

bool IsEqualImages(HANDLE hFile1, unsigned long dwPos1, 
					  HANDLE hFile2, unsigned long dwPos2)
{
	__int8 bBuff[BUFF_SIZE];
	unsigned long dwSize1, dwSize2;

	if (!GetImageSize(hFile1, dwPos1, bBuff, BUFF_SIZE, &dwSize1))
		return false;
	if (!GetImageSize(hFile2, dwPos2, bBuff, BUFF_SIZE, &dwSize2))
		return false;

	return IsEqualFileData(hFile1, dwPos1, dwSize1, hFile2, dwPos2, dwSize2);
}

bool iSearchWithTreeCallback(IN CONST VOID* pCallbackCtx, IN tSIGNATURE_ID SignatureID, IN tINT dwPosInBuffer, IN tQWORD qwPosLinear, IN unsigned int dwSignatureLen)
{
	TREE_SEARCH_CONTEXT* pSrchCtx = (TREE_SEARCH_CONTEXT*)pCallbackCtx;
	pSrchCtx->dwFoundPos = (unsigned long)qwPosLinear;

	if (IsEqualImages(pSrchCtx->hFile1, pSrchCtx->dwFoundPos, pSrchCtx->hFile2, 0))
	{
		pSrchCtx->bFound = true;
		return false;
	}
	return true;
}

bool iSearchWithTree2(sRT_CTX* pRTCtx, TREE_SEARCH_CONTEXT* pTreeSrchCtx)
{
	bool bRes = false;
	tERROR error;
	BYTE  bBuff[BUFF_SIZE];
	unsigned long dwBytes;
	unsigned long dwRead;
	const unsigned long cRawSize = 0x400;
	unsigned int dwSigSize;
	unsigned char* pSig = NULL;

	sSEARCHER_RT_CTX* pSeacherRTCtx;
	tSIGNATURE_ID SigID;

	
	if (pTreeSrchCtx->dwScanStartPos != SetFilePointer(pTreeSrchCtx->hFile1, pTreeSrchCtx->dwScanStartPos, NULL, FILE_BEGIN))
		return false;

	if (0 != SetFilePointer(pTreeSrchCtx->hFile2, 0, NULL, FILE_BEGIN))
		return false;

	ReadFile(pTreeSrchCtx->hFile2, bBuff, cRawSize, &dwBytes, NULL);
	if (dwBytes != cRawSize)
		return false;


	error = TreeConvertRaw2Signature(bBuff, dwBytes, NULL, 0, &dwSigSize);
	if (PR_SUCC(error))
		pSig = (unsigned char*) HeapAlloc(GetProcessHeap(), 0, dwSigSize);
	if (pSig == NULL)
		error = errNOT_ENOUGH_MEMORY;
	if (PR_SUCC(error))
		error = TreeConvertRaw2Signature(bBuff, dwBytes, pSig, dwSigSize, NULL);
	if (PR_SUCC(error))
		error = TreeAddSignature(pRTCtx, pSig, dwSigSize, 1, &SigID);
	if (PR_SUCC(error))
		error = TreeGetSeacherContext(pRTCtx, NULL, &pSeacherRTCtx);
	if (PR_SUCC(error))
	{
		unsigned long dwScanSize = pTreeSrchCtx->dwScanSize;
		while (dwScanSize) 
		{
			dwRead = min(BUFF_SIZE, dwScanSize); 
			ReadFile(pTreeSrchCtx->hFile1, bBuff, dwRead, &dwBytes, NULL);
			if (dwBytes != dwRead)
				break;
			
			TreeSearchData(pSeacherRTCtx, bBuff, dwBytes, 0, (unsigned long)-1, iSearchWithTreeCallback, pTreeSrchCtx);
			if (pTreeSrchCtx->bFound)
			{
				bRes = true;
				break;
			}
			dwScanSize -= dwRead;
		};
		TreeDoneSeacherContext(pSeacherRTCtx);
	}
	if (pSig)
		HeapFree(GetProcessHeap(), 0, pSig);

	return bRes;
}

bool iSearchWithTree(HANDLE hFile1, TREE_SEARCH_CONTEXT* pTreeSrchCtx)
{
	bool bRes = false;
	tERROR error;
	sRT_CTX* pRTCtx;
	sHEAP_CTX sHeapCtx;

	sHeapCtx.pHeap = GetProcessHeap();
	sHeapCtx.Alloc = MyHeapAlloc;
	sHeapCtx.Realloc = MyHeapReAlloc;
	sHeapCtx.Free = MyHeapFree;
	
	error = TreeInit(&sHeapCtx, &pRTCtx);
	if (PR_SUCC(error))
	{
		bRes = iSearchWithTree2(pRTCtx, pTreeSrchCtx);

		error = TreeUnload_Any(pRTCtx);
		error = TreeDone(pRTCtx);
	}
	return bRes;
}

bool iDoesImageContain2nd(HANDLE hFile1, HANDLE hFile2, unsigned long dwScanSize, unsigned long* pdwPos)
{
	bool bRes;
	unsigned long dwFileSize;
	TREE_SEARCH_CONTEXT sTreeSrchCtx;

	if (pdwPos)
		*pdwPos = 0;

	if (dwScanSize == 0)
		return IsEqualImages(hFile1, 0, hFile2, 0);

	dwFileSize = GetFileSize(hFile1, NULL);

	memset(&sTreeSrchCtx, 0, sizeof(sTreeSrchCtx));
	sTreeSrchCtx.hFile1 = hFile1;
	sTreeSrchCtx.hFile2 = hFile2;
	sTreeSrchCtx.dwScanStartPos = 0;
	sTreeSrchCtx.dwScanSize = min(dwFileSize, dwScanSize);
	sTreeSrchCtx.bFound = false;
	
	bRes = iSearchWithTree(hFile1, &sTreeSrchCtx);
	if (bRes && pdwPos)
		*pdwPos = sTreeSrchCtx.dwFoundPos;
	return bRes;
}


// ---------------------------------------------------------------

bool DoesImageContain2nd(PWCHAR pImageName1, PWCHAR pImageName2, unsigned long dwScanSize, unsigned long* pdwPos)
{
	HANDLE hFile1, hFile2;
	bool bRes = false;

	hFile1 = CreateFile(pImageName1, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile1 != INVALID_HANDLE_VALUE)
	{
		hFile2 = CreateFile(pImageName2, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile2 != INVALID_HANDLE_VALUE)
		{
			bRes = iDoesImageContain2nd(hFile1, hFile2, dwScanSize, pdwPos);

			CloseHandle(hFile2);
		}
		else
		{
			DbPrint(( DCB_BL, DL_ERROR, _T("DoesImageContain2nd: CreateFile for file2 failed with err=%08X"), GetLastError()));
		}
		CloseHandle(hFile1);
	}
	else
	{
		DbPrint(( DCB_BL, DL_ERROR, _T("DoesImageContain2nd: CreateFile for file1 failed with err=%08X"), GetLastError()));
	}
	return bRes;
}
*/