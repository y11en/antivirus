/*
 *	Lªøûøªð ôû  ÿ¨õò¨ð•õýø  þñv¢ýv¿ ô¨ðùòõ¨þò ò ô¨ðùòõ¨ð, úþªþ¨võ üþóºª ñvª¹ ÷ðó¨ºöõýv 
 *  ¦ðú ÷ðó¨º÷¢øúþü Kl1, ªðú ø þÿõ¨ð¡øþýýþù ¸ø¸ªõüþù.

 *  Lûóþ¨øªü ¨ðñþªv :
 *  1. +ñýºûøª¹ CheckSum PE-÷ðóþûþòúð.
 *  2. ¦þô¸¢øªðª¹ HASH ÿþ ¯ðùûº ø ôþñðòøª¹ õóþ ò úþýõ¡ ¯ðùûð
 *  3. ¦þô¸¢øªðª¹ úþýªþ¨þû¹ýº¦ ¸ºüüº ø ÷ðÿõ¸ðª¹ õõ ò PE-÷ðóþûþòþú.
 */

#include "stdafx.h"

BOOL
WriteCheckSum(char* FileName, bool ZeroSum)
{
	HANDLE hFile;	
	HANDLE hMapFile;
	LPVOID lpMapAddress;
	LARGE_INTEGER   FileSize;
	
	hFile = CreateFile(
		FileName, 
		GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL,  
		NULL );

	if ( hFile == INVALID_HANDLE_VALUE )
	{
		printf ("unable to open%s\n", FileName);
		return FALSE;
	}
    
    GetFileSizeEx(hFile, &FileSize);	
	
	hMapFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, "MyMappingObject");
	
	if (hMapFile == NULL) 
	{ 
		return FALSE;
	}
	
	lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0 );
	
	if (lpMapAddress == NULL) 
	{ 
		return FALSE;
	} 
	
	
	PIMAGE_DOS_HEADER   Dos_header = (PIMAGE_DOS_HEADER)lpMapAddress;
	PIMAGE_NT_HEADERS   PE_header = (PIMAGE_NT_HEADERS)( (char*)lpMapAddress + Dos_header->e_lfanew );
	DWORD	dwHeaderSum;	
	
	if ( ZeroSum )
		PE_header->OptionalHeader.CheckSum = 0;
	else
		CheckSumMappedFile( lpMapAddress, (DWORD)FileSize.QuadPart, &dwHeaderSum, &PE_header->OptionalHeader.CheckSum );	
	
	UnmapViewOfFile( lpMapAddress );
	CloseHandle( hMapFile );
	CloseHandle( hFile );

	return TRUE;
}

void
ShowUsage()
{
	printf ("Usage   : klsumfix <mod name> [zero|fix]\n");
	printf ("example : klsumfix klick.sys zero\n");
	printf ("example : klsumfix klick.sys fix\n");
}

int main(int argc, char* argv[])
{	
	if ( argc < 2 )
	{
		ShowUsage();
		return 0;
	}

	if ( 0 == _stricmp( argv[2], "fix" ) )
	{
		WriteCheckSum(argv[1], false);
		printf ("fixed %s\n", argv[1]);
		return 0;
	}

	if ( 0 == _stricmp( argv[2], "zero" ) )
	{
		WriteCheckSum(argv[1], true);
		printf ("zeroed %s\n", argv[1]);
		
		return 0;
	}

	ShowUsage();
	
	return -1;
}
