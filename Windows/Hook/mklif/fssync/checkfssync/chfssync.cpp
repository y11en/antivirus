// dpf.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <devioctl.h>
#include <shtypes.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <stdio.h>

#pragma comment(lib, "fssync.lib")
#include "..\fssync.h"

int _cdecl
main (
    __in int argc,
    __in_ecount(argc) char *argv[]
    )
{
	PVOID pContext = NULL;

	HRESULT hResult = FSDrv_UniInit( &pContext );
	if (SUCCEEDED( hResult ))
	{
		FSDrv_UniRegInv( pContext );
		FSDrv_UniUnRegInv( pContext );
		FSDrv_UniDone( &pContext );
	}

	return 0;
}

