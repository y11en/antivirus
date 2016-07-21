// klmount.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

#include <stdio.h>

#include "../../Hook/avpgcom.h"
#include "../../Hook/Funcs.h"
#include "../../Hook/HookSpec.h"
#include "../../kldefs.h"

#include "../../Hook/FSDrvLib.h"
#include "../../Hook/IDriver.h"

int ShowUsage()
{
	printf("Usage: klmount <volume letter>\n");
	return ERROR_INVALID_PARAMETER;
}

int CantConnectToDriver()
{
	printf("Can't connect to driver to generate fake mount!\n");
	return ERROR_DEVICE_NOT_CONNECTED;
}

char EventBuffer[0x4096];
PFILTER_EVENT_PARAM pParam = (PFILTER_EVENT_PARAM) EventBuffer;
PSINGLE_PARAM pSingleParam;
VERDICT Verdict;

//+ ------------------------------------------------------------------------------------------
int FakeMountNT(char Letter)
{
//+ ------------------------------------------------------------------------------------------
	WCHAR DrvName[3];
	DrvName[0] = Letter;
	DrvName[1] = L':';
	DrvName[2] = 0;

	typedef DWORD (WINAPI *_pQueryDosDeviceW)(LPCWSTR,  LPWSTR, DWORD);	
	static _pQueryDosDeviceW pQueryDosDeviceW = NULL;
	
	if (pQueryDosDeviceW == NULL)
	{
		HMODULE hKernel = GetModuleHandle("kernel32.dll");
		if (hKernel != NULL)
			pQueryDosDeviceW = (_pQueryDosDeviceW) GetProcAddress(hKernel, "QueryDosDeviceW");
	}
	
	if (pQueryDosDeviceW == NULL)
		return GetLastError();

	WCHAR TargetPath[MAX_PATH];
	DWORD dwLen = pQueryDosDeviceW(DrvName, TargetPath, MAX_PATH);
	if (dwLen == 0)
		return GetLastError();

	//+ ------------------------------------------------------------------------------------------

	
	FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_NFIOR, IRP_MJ_FILE_SYSTEM_CONTROL, IRP_MN_VERIFY_VOLUME, PostProcessing, 1);
	lstrcpy(pParam->ProcName, "klmount");
					
	pSingleParam = (PSINGLE_PARAM) pParam->Params;
	SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
	pSingleParam->ParamSize = sizeof(WCHAR) * dwLen;
	memcpy(pSingleParam->ParamValue, TargetPath, pSingleParam->ParamSize);

	if (IDriverFilterEvent(FSDrvGetDeviceHandle(), pParam, FALSE, &Verdict))
	{
		if (Verdict_NotFiltered == Verdict)
		{
			printf("Event sended but not filtered\n");
			return ERROR_INVALID_HOOK_FILTER;
		}
		else
			printf("Verdict from driver %d\n", Verdict);
		
		return ERROR_SUCCESS;
	}
	
	return GetLastError();
}

int FakeMount9x(char Letter)
{
	FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_IFS, IFSFN_CONNECT, 0, PostProcessing, 1);
	pSingleParam = (PSINGLE_PARAM) pParam->Params;
	
	SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL);
	lstrcpy((char*) pSingleParam->ParamValue, "A:");
	pSingleParam->ParamValue[0] = Letter;
	pSingleParam->ParamSize = strlen((char*) pSingleParam->ParamValue) + 1;

	if (IDriverFilterEvent(FSDrvGetDeviceHandle(), pParam, FALSE, &Verdict))
	{
		if (Verdict_NotFiltered == Verdict)
		{
			printf("Event sended but not filtered\n");
			return ERROR_INVALID_HOOK_FILTER;
		}
		else
			printf("Verdict from driver %d\n", Verdict);

		return ERROR_SUCCESS;
	}

	return GetLastError();
}
//+ ------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	if (argc != 2)
		return ShowUsage();
	
	int nRet = 0;

	OSVERSIONINFO osVer;
	osVer.dwOSVersionInfoSize = sizeof(osVer);
	GetVersionEx(&osVer);

	char Letter = argv[1][0];
	Letter &= ~0x20;

	printf("klmount: Generate fake mount for drive %c:\n", Letter);

	if (!FSDrvInterceptorInit())
		return CantConnectToDriver();

	if (VER_PLATFORM_WIN32_NT == osVer.dwPlatformId)
		nRet =  FakeMountNT(Letter);
	else
		nRet =  FakeMount9x(Letter);

	FSDrvInterceptorDone();

	if (ERROR_SUCCESS != nRet)
		printf("\nSend fake mount failed. Error code %d\n", nRet);
	else
		printf("\nSend fake mount completed. All ok.\n");

	return nRet;
}
