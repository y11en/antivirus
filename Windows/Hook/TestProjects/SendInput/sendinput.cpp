#include <windows.h>
#include <shlwapi.h>
#include <vector>

#pragma warning(disable:4996)

std::vector<HWND> g_windows;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	g_windows.push_back(hwnd);
	return TRUE;
}

void MakeSendInput(HWND hWnd, DWORD x, DWORD y)
{
	INPUT Inputs[2];

	RECT Rect;
	if (!GetWindowRect(hWnd, &Rect))
	{
		printf("failed to get window rect!\n");
		return;
	}

// move, press and release
	ZeroMemory(&Inputs, sizeof(Inputs));
	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN;
	Inputs[0].mi.dx = ((Rect.left+x)*0xFFFF)/GetSystemMetrics(SM_CXSCREEN);
	Inputs[0].mi.dy = ((Rect.top+y)*0xFFFF)/GetSystemMetrics(SM_CYSCREEN);
	Inputs[1].type = INPUT_MOUSE;
	Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
	DWORD Result = SendInput(2, Inputs, sizeof(Inputs[0]));

	if (!Result)
		printf("failed to SendInput()!\n");
}

int main(int argc, char* argv[])
{
	if (argc < 4)
	{
		printf("invalid command string!\n");
		printf("usage:\tsendinput.exe WindowPattern x y\n");
		return -1;
	}

	bool bSendInput = true;
	char *pMask = argv[1];
	DWORD	x = StrToIntA(argv[2]),
			y = StrToIntA(argv[3]);

	EnumWindows(EnumWindowsProc, NULL);

	for (DWORD i = 0; i < g_windows.size(); ++i)
	{
		char orig_text[1024];
		char text[1024];

		GetWindowTextA(g_windows[i], orig_text, sizeof(orig_text));
		strcpy(text, orig_text);
		_strlwr(text);

		if (strstr(text, "sendinput.exe"))
			continue;
		if (pMask && strstr(text, pMask) == NULL)
			continue;
		printf("HWND = %08Ix, TEXT = '%s'\n", g_windows[i], text);
		if (bSendInput)
			MakeSendInput(g_windows[i], x, y);
	}
	
	return 0;
}