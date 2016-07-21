// lddll.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	if ( argc < 2 )
	{
		_tprintf( _T("\nusage:\tldll dllname\n") );
		return -1;
	}

	TCHAR* tcLibName = argv[1];
	HMODULE hMod = LoadLibrary( tcLibName );
	if ( !hMod )
		_tprintf( _T("failed to LoadLibrary(%s)\n"), tcLibName );
	else
	{
		_tprintf( _T("success LoadLibrary(%s)\n"), tcLibName );
		_tprintf( _T("waiting for key to unload..\n") );
		getchar();
		FreeLibrary(hMod);
		_tprintf( _T("unloaded.\n") );
	}

	return 0;
}

