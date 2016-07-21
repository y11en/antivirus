#include <windows.h>
#include <tchar.h>
#include "RegisterService.h"

void ServiceRegister (void)
{
	try
	{
		HINSTANCE hInstKernel;
		DWORD (__stdcall *pRegisterServiceProcess) (DWORD, DWORD);

		hInstKernel = LoadLibrary (_T("KERNEL32.DLL"));

		if (hInstKernel)
		{
			pRegisterServiceProcess = (DWORD (__stdcall *) (DWORD, DWORD))GetProcAddress (hInstKernel, "RegisterServiceProcess");
			if (pRegisterServiceProcess)  pRegisterServiceProcess (NULL, RSP_SIMPLE_SERVICE);
			FreeLibrary (hInstKernel);
		}
	}
	catch(...)
	{
	}
}