// ChangeDebugLevel.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"
#include <hook\FSDrvLib.h>
#include <hook\IDriver.h>

#include "windows.h"
#define  NullStr "\0"
#include "../../debug.h"


int main(int argc, char* argv[])
{
	int level;
	if (argc != 2)
	{
		printf("Required parameter - info level (int)!\n");
		return 0;
	}
	if (FSDrvInterceptorInit() == FALSE)
	{
		printf("Init avpg driver failed! Trying tsp...\n");
		if (FSDrvInterceptorInitTsp() == FALSE)
		{
			printf("Init driver failed!\n");
			return 0;
		}
	}
	
	HANDLE hDriver;
	hDriver = FSDrvGetDeviceHandle();
	
	level = atoi(argv[1]);
	if (level != 0)
	{
		IDriverChangeDebugInfoLevel(hDriver, level);
	}
	else
	{
		printf("Default settings for filters and events!\n");
		IDriverChangeDebugInfoLevel(hDriver, DL_INFO);
		IDriverChangeDebugInfoMask(hDriver, DC_EVENT | DC_FILTER);
	}

	FSDrvInterceptorDone();
	return 0;
}
