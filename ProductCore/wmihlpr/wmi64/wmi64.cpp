// wmi64.cpp : Defines the entry point for the console application.
//

#include "../wmihlpr.cpp"

int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	if (__argc != 4)
		return S_FALSE;

	LPCSTR pguidID = __argv[1];
	DWORD  eComponent = atoi(__argv[2]);
	DWORD  eStatus = atoi(__argv[3]);

	return wmih_UpdateStatusEx(cGUIDStr(pguidID), eComponent, eStatus);
}
