// StreamRemover.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "stdio.h"

#include "../../hook/fssync.h"

unsigned int stream_found_count = 0;
unsigned int stream_notfound_count = 0;

#define countof(array) (sizeof(array)/sizeof(array[0]))

LPSTR GetLastErrorStr()
{
		LPSTR lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
			);
		// Process any inserts in lpMsgBuf.
		// ...
		// Display the string.
		//printf("%S", tmp, lpMsgBuf);
		// Free the buffer.
		//LocalFree( lpMsgBuf );
		return lpMsgBuf;
}

//+ ----------------------------------------------------------------------------------------
void FileProceed(WIN32_FIND_DATA *pdf, WCHAR* pchPath, BOOL bDiag, BOOL bRemove)
{
	WCHAR tmp[4096];
	WCHAR tmpMain[4096];
	BOOL bResult;
	FILETIME crt, lac, lwr;
	BOOL bGetttedTime = FALSE;
	wsprintf(tmp, L"\\\\?\\%s%s:KAVICHS", pchPath, pdf->cFileName);

	wsprintf(tmpMain, L"\\\\?\\%s%s", pchPath, pdf->cFileName);
	HANDLE hFile = CreateFile(tmpMain, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		stream_found_count++;
		bGetttedTime = GetFileTime(hFile, &crt, &lac, &lwr);
	}
	else
	{
		stream_notfound_count++;
		printf("%S\n", tmpMain);
	}
	if (bRemove)
		bResult = DeleteFile(tmp);

	if (bDiag)
	{
		if (bResult)
			printf("Stream '%S' removed\n", tmp);
		else
			printf("Failed to remove stream '%S':\n  %S\n", tmp, GetLastErrorStr());
	}

	if (hFile != INVALID_HANDLE_VALUE)
	{
		if (bGetttedTime == TRUE)
			SetFileTime(hFile, &crt, &lac, &lwr);
		CloseHandle(hFile);
	}
}

//+ ----------------------------------------------------------------------------------------
void Go(WCHAR* pchPath, BOOL bRemove)
{
	WIN32_FIND_DATA df;
	WCHAR tmp[MAX_PATH];
	DWORD dwAttr;

	dwAttr = GetFileAttributes(pchPath);
	if (dwAttr != (DWORD)-1)
	{
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (pchPath[0] && pchPath[lstrlen(pchPath)-1]!='\\' && pchPath[lstrlen(pchPath)-1]!='//')
				lstrcat(pchPath, L"\\");
		}
		else
		{
			WCHAR fullname[MAX_PATH];
			GetFullPathName(pchPath, sizeof(fullname), fullname, NULL);
			df.cFileName[0]=0;
			FileProceed(&df, fullname, bRemove, bRemove);
			return;
		}
	}

	if (bRemove)
		printf("%S\n", pchPath);

	wsprintf(tmp, L"\\\\?\\%s%s", pchPath, L"*.*");
	HANDLE hFile = FindFirstFile(tmp, &df);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		BOOL bExit = FALSE;
		while (bExit == FALSE)
		{
			if ((lstrcmp(df.cFileName, L".") != 0) && (lstrcmp(df.cFileName, L"..") != 0))
			{
				if (!(df.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					FileProceed(&df, pchPath, FALSE, bRemove);
				else
				{
					WCHAR Dest2[MAX_PATH];
					wsprintf(Dest2, L"%s%s\\", pchPath, df.cFileName);
					Go(Dest2, bRemove);
				}
			}
			bExit = !FindNextFile(hFile, &df);
		}
		FindClose(hFile);
	}
}

BOOL iCopyFile(WCHAR* pchSrc, WCHAR* pchDst)
{
	HANDLE hFile1, hFile2;
	BYTE buff[0x100];
	DWORD dwRead;
	DWORD dwWritten;

	hFile1 = CreateFile(pchSrc, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile1 == INVALID_HANDLE_VALUE)
	{
		printf("Cannot open file '%S':\n  %S\n", pchSrc, GetLastErrorStr());
		return FALSE;
	}
	hFile2 = CreateFile(pchDst, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile1 == INVALID_HANDLE_VALUE)
	{
		printf("Cannot create file '%S':\n  %S\n", pchDst, GetLastErrorStr());
		CloseHandle(hFile1);
		return FALSE;
	}
	
	while (ReadFile(hFile1, buff, sizeof(buff), &dwRead, NULL) && dwRead!=0)
	{
		if (!WriteFile(hFile2, buff, dwRead, &dwWritten, NULL))
			printf("Cannot write to file '%S':\n  %S\n", pchDst, GetLastErrorStr());
	}

	CloseHandle(hFile1);
	CloseHandle(hFile2);
	return TRUE;
}

void iExtractStream(WCHAR* pchPath)
{
	WCHAR tmp[MAX_PATH];
	WCHAR tmp2[MAX_PATH];
	WCHAR fullname[MAX_PATH];
	GetFullPathName(pchPath, sizeof(fullname), fullname, NULL);
	wsprintf(tmp, L"\\\\?\\%s:KAVICHS", fullname);
	wsprintf(tmp2, L"\\\\?\\%s.KAVICHS", fullname);
	if (iCopyFile(tmp, tmp2))
		printf("Stream copied into %S\n", tmp2);
}

void iHelp()
{
	printf("KAV Stream Management Tool v1.01 "__DATE__" "__TIME__"  [FOR INTERNAL USE ONLY]\n");
	printf("Usage: StreamRemover <switch> <path|filename>\n");
	printf("   -h    this help\n");
#ifndef _NODRV_SUPPORT	
	printf("   -e    extract KAV stream from specified file\n");
#endif // _NODRV_SUPPORT
	printf("   -r    remove KAV stream from specified file\n");
	printf("   -i    gather info\n");
	printf("         or from all files in specified folder and its subfolders\n");
}

//+ ----------------------------------------------------------------------------------------


int main(int argc, char* argv[])
{
	WCHAR wcsPath[MAX_PATH];
	int i;

	BOOL bExtractStream = FALSE;
	BOOL bRemove = FALSE;
	BOOL bHelp = FALSE;
	BOOL bInfo = FALSE;

#ifndef _NODRV_SUPPORT	
	if (!FSDrv_Init())
	{
		printf("Driver init failed\n");
		//return -1;
	}
	
	FSDrv_RegisterInvisibleThread();
#endif // _NODRV_SUPPORT	

	GetCurrentDirectory(sizeof(wcsPath), wcsPath);
	
	for (i=1;i<argc;i++)
	{
		if (argv[i][0]=='/' || argv[i][0]=='-')
		{
			switch(argv[i][1])
			{
			case 'H':
			case 'h':
			case '?':
				bHelp=TRUE;
				break;
			case 'E':
			case 'e':
				bExtractStream=TRUE;
				break;
			case 'R':
			case 'r':
				bRemove=TRUE;
			case 'i':
			case 'I':
				bInfo=TRUE;
				break;
			}
		}
		else
		{
			MultiByteToWideChar(CP_ACP, 0, argv[i], -1, wcsPath, countof(wcsPath));
		}
	}

	if (bHelp)
		iHelp();
	else if (bExtractStream)
		iExtractStream(wcsPath);
	else if (bRemove)
		Go(wcsPath, TRUE);
	else if (bInfo)
		Go(wcsPath, FALSE);
	else
		iHelp();

#ifndef _NODRV_SUPPORT	
	FSDrv_UnRegisterInvisibleThread();
	FSDrv_Done();
#endif // _NODRV_SUPPORT

	if (bInfo)
	{
		printf("\n Total streams found %d, w/o stream %d\n", stream_found_count, stream_notfound_count);
	}
	return 0;
}
