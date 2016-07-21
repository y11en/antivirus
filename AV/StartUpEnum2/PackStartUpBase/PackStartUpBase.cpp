// PackStartUpBase.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "stdio.h"
#include <tchar.h>

TCHAR* GetFullName(TCHAR Path[],TCHAR* NewPath);
TCHAR* _GetFullName(TCHAR Path[],TCHAR* NewPath,bool* Find);

int main(int argc, char* argv[])
{
	TCHAR	FileFrom[MAX_PATH];
	TCHAR	FileTo[MAX_PATH];
	HANDLE	hFrom=NULL;
	HANDLE	hTo=NULL;

	if (argc!=2)
	{
		printf("USAGE: psub [<file path>]<file name>\n");
		return 0;
	}
	GetFullName(argv[1],FileTo);
	if ((hTo=CreateFile(FileTo,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) 
	{
		printf("Can not find file.\n");
		return 0;
	}
	CloseHandle(hTo);
	strcat(strcpy(FileFrom,FileTo),".backup");
	if (CopyFile(FileTo,FileFrom,FALSE)==0)
	{
		printf("Can not backup old file - %d.\n",GetLastError());
		return 0;
	}
	if ((hTo=CreateFile(FileTo,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) 
	{
		printf("Can not find file.\n");
		return 0;
	}
	if ((hFrom=CreateFile(FileFrom,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL))==INVALID_HANDLE_VALUE) 
	{
		CloseHandle(hTo);
		printf("Can not find file.\n");
		return 0;
	}
	DWORD dRealRead=0;
	DWORD hSize=GetFileSize(hFrom,NULL);
	PBYTE pMask=(PBYTE)malloc(hSize);
	if (pMask!=NULL)
	{
		if (ReadFile(hFrom,pMask,hSize,&dRealRead,NULL)!=0)
		{
			if (dRealRead==hSize)
			{
				DWORD i;
				for (i=0;i<hSize;i++) *(pMask+i)^=0xaa;
				if (WriteFile(hTo,pMask,hSize,&dRealRead,NULL)!=0)
				{
					if (dRealRead!=hSize) printf("Error write source file <%s> - unknown",FileFrom);
				}
				else printf("Error write source file <%s> = %d",FileTo,GetLastError());
			}
			else printf("Error read source file <%s> - unknown",FileFrom);
		}
		else printf("Error read source file <%s> = %d",FileFrom,GetLastError());
	}
	else printf("Not enough memory.\n");
	CloseHandle(hFrom);
	CloseHandle(hTo);
	return 0;
}

TCHAR* GetFullName(TCHAR Path[],TCHAR* NewPath)
{
	bool Find=false;
	__try
	{
		NewPath=_GetFullName(Path,NewPath,&Find);
		if (!Find)
		{
			char	chExt[MAX_PATH];
			_splitpath(Path,NULL,NULL,NULL,chExt);
			if ((strlen(chExt)==0)&(Path[strlen(Path)-1]!='.'))
			{
				TCHAR	TryPath[MAX_PATH];
				TCHAR	SecTryPath[MAX_PATH];
				strcat(strcpy(SecTryPath,Path),".ini");
				_GetFullName(SecTryPath,(TCHAR*)TryPath,&Find);
				if (Find) strcpy(NewPath,TryPath);
			}
		}
		return NewPath;
	}
	__finally
	{
		return NewPath;
	}
}

TCHAR* _GetFullName(TCHAR Path[],TCHAR* NewPath,bool* Find)
{
	TCHAR TryPath[MAX_PATH];
	WIN32_FIND_DATA df;
	HANDLE hHand;
	if (Find!=NULL) *Find=false;
	if (GetModuleFileName(NULL,TryPath,_MAX_PATH) ) 
	{
		char Dir[MAX_PATH];
		char fPath[MAX_PATH];
		_splitpath(TryPath,Dir,fPath,NULL,NULL);
		_makepath(TryPath,Dir,fPath,NULL,NULL);
		if (TryPath[strlen(TryPath)-1]!='\\') strcat(TryPath,"\\");
		strcat(TryPath,Path);
		hHand=FindFirstFile(TryPath,&df);
		if (hHand!=INVALID_HANDLE_VALUE)
		{
			FindClose(hHand);
			_tcscpy(NewPath,TryPath);
			if (Find!=NULL) *Find=true;
			return (NewPath);
		}
	}
	if (GetCurrentDirectory(_MAX_PATH,TryPath)) 
	{
		if (TryPath[strlen(TryPath)-1]!='\\') strcat(TryPath,"\\");
		strcat(TryPath,Path);
		hHand=FindFirstFile(TryPath,&df);
		if (hHand!=INVALID_HANDLE_VALUE)
		{
			FindClose(hHand);
			_tcscpy(NewPath,TryPath);
			if (Find!=NULL) *Find=true;
			return (NewPath);
		}
	} 
	if (GetWindowsDirectory(TryPath,_MAX_PATH)) 
	{
		if (TryPath[strlen(TryPath)-1]!='\\') strcat(TryPath,"\\");
		strcat(TryPath,Path);
		hHand=FindFirstFile(TryPath,&df);
		if (hHand!=INVALID_HANDLE_VALUE)
		{
			FindClose(hHand);
			_tcscpy(NewPath,TryPath);
			if (Find!=NULL) *Find=true;
			return (NewPath);
		}
	}
	if (GetSystemDirectory(TryPath,_MAX_PATH)) 
	{
		if (TryPath[strlen(TryPath)-1]!='\\') strcat(TryPath,"\\");
		strcat(TryPath,Path);
		hHand=FindFirstFile(TryPath,&df);
		if (hHand!=INVALID_HANDLE_VALUE)
		{
			FindClose(hHand);
			_tcscpy(NewPath,TryPath);
			if (Find!=NULL) *Find=true;
			return (NewPath);
		}
	}
	OSVERSIONINFO	verInfo = {0};
	verInfo.dwOSVersionInfoSize = sizeof (verInfo);
	GetVersionEx(&verInfo);
	if(verInfo.dwPlatformId==VER_PLATFORM_WIN32_NT) 
	{
		if (GetWindowsDirectory(TryPath,_MAX_PATH)) 
		{
			if (TryPath[strlen(TryPath)-1]!='\\') strcat(TryPath,"\\");
			strcat(strcat(TryPath,"SYSTEM\\"),Path);
			hHand=FindFirstFile(TryPath,&df);
			if (hHand!=INVALID_HANDLE_VALUE)
			{
				FindClose(hHand);
				_tcscpy(NewPath,TryPath);
				if (Find!=NULL) *Find=true;
				return (NewPath);
			}
		}
	}
	_searchenv(Path,"PATH",TryPath);
	if (strlen(TryPath)==0) strcpy(NewPath,Path);
	else 
	{
		if (Find!=NULL) *Find=true;
		strcpy(NewPath,TryPath);
	}
	return (NewPath);
}
