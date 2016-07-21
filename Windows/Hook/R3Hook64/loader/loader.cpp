// loader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
		return -1;

	LoadLibraryW(L"user32.dll");

	getchar();

	LoadLibrary(argv[1]);
	LoadLibrary(L"rpcrt4.dll");

	return 0;
}

