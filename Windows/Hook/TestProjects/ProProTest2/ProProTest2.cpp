// ProProTest2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>
#include <tchar.h>

#include "../../Hook/FSDrvLib.h"
#include "../../Hook/IDriver.h"
#include "../../hook\HookSpec.h"
#include "../../hook\Funcs.h"

int main(int argc, char* argv[])
{
	bool bRegister = !!FSDrvInterceptorInitEx(_CLIENT_FLAG_POPUP_TYPE | _CLIENT_FLAG_WITHOUTWATCHDOG, AVPG_Driver_Specific, AVPG_STANDARTRIORITY, 0);
	if (!bRegister)
		return -1;

	ULONG AppID = FSDrvGetAppID();
	HANDLE hDevice = FSDrvGetDeviceHandle();

	bool bActive = !!IDriverProProtRequest(hDevice, AppID, TRUE, PROPROT_FLAG_OPEN | PROPROT_FLAG_AUTOACTIVATE);
	if (!bActive)
	{
		::MessageBox(NULL, "protection malfunction", "pro2", 0);
	}
	else
	{

		HANDLE hProc = OpenProcess(SYNCHRONIZE, FALSE, GetCurrentProcessId());
		if (hProc != NULL)
		{
			::MessageBox(NULL, "PROC can be opened for synchronize", "pro2", 0);
			CloseHandle(hProc);
		}

		::MessageBox(NULL, "play sound? (ding.wav)", "pro2", 0);
		PlaySound("ding.wav", NULL, SND_ASYNC);
		::MessageBox(NULL, "play sound complete", "pro2", 0);


		::MessageBox(NULL, "waiting...", "pro2", 0);
	}


	FSDrvInterceptorDone();

	return 0;
}
