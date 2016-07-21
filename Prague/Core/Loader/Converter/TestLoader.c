#include <windows.h>
#include <stdio.h>
#pragma hdrstop
#include "..\PrLoader.h"

char Usage[]="\nUsage: PrLoader.exe PraguePlugin.ext\n\n";
typedef void (*StartUpProc)(void);

int main(int argc,char* argv[])
{
DWORD dwEntryPoint,dwSize;
void *Data;
hPLUGININSTANCE hInsts;
StartUpProc StartUp;

	printf("\nPrague module loader ver 0.1\n");
	if(argc<2)
	{
		printf(Usage);
		return 1;
	}

	hInsts = LoadPlugin(argv[1], &dwEntryPoint);
	if(hInsts)
	{
		printf("Load Plugin OK\n");
		StartUp=(StartUpProc)dwEntryPoint;
//		StartUp();
	}
	else
		printf("Load Plugin Failed\n");

	if(errOK==GetMetaData(argv[1], &Data,&dwSize))
		printf("GetMetaData OK\n");
	else
		printf("GetMetaData Failed\n");
	UnloadPlugin(hInsts);
	return 0;
}
