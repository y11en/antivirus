#include <Windows.h>
#include <string>
using namespace std;
/*
static char iupd1[] = "c:\\iupdater2_set1\\iupdater.exe -u -tl 10 -tf IUpdater2_set1.log -m";
static char iupd2[] = "c:\\iupdater6_set1\\iupdater.exe -u -tl 10 -tf IUpdater6_set1.log -m";
static char iupd3[] = "c:\\iupdater24_set1\\iupdater.exe -u -tl 10 -tf IUpdater24_set1.log -m";
static char iupd4[] = "c:\\iupdater2d_set1\\iupdater.exe -u -tl 10 -tf IUpdater2d_set1.log -m";
static char iupd5[] = "c:\\iupdater2_set2\\iupdater.exe -u -tl 10 -tf IUpdater2_set2.log -m";
static char iupd6[] = "c:\\iupdater6_set2\\iupdater.exe -u -tl 10 -tf IUpdater6_set2.log -m";
static char iupd7[] = "c:\\iupdater24_set2\\iupdater.exe -u -tl 10 -tf IUpdater24_set2.log -m";
static char iupd8[] = "c:\\iupdater2d_set2\\iupdater.exe -u -tl 10 -tf IUpdater2d_set2.log -m";
*/
static char iupd1[] = "c:\\iupdater2_set1\\start.bat";
static char iupd2[] = "c:\\iupdater6_set1\\start.bat";
static char iupd3[] = "c:\\iupdater24_set1\\start.bat";
static char iupd4[] = "c:\\iupdater2d_set1\\start.bat";
static char iupd5[] = "c:\\iupdater2_set2\\start.bat";
static char iupd6[] = "c:\\iupdater6_set2\\start.bat";
static char iupd7[] = "c:\\iupdater24_set2\\start.bat";
static char iupd8[] = "c:\\iupdater2d_set2\\start.bat";

void StartIniUpdater (char* path)
{
/*	
	STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
	CreateProcess(
					NULL,
					const_cast<LPSTR>(path), 
					NULL, 
					NULL, 
					FALSE, 
					CREATE_NEW_CONSOLE, 
					NULL, 
					NULL,
					&si,
					&pi
					);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	//return pi.dwProcessId;
*/
	string objPath (path);
	size_t ind = objPath.find_last_of("\\");
	string file_path = objPath.substr (0, ind);

	ShellExecute(NULL, "open", path, NULL, file_path.c_str(), SW_SHOWNORMAL);
}

int main ()
{
	HANDLE hTimer[3];
	for (int i = 0; i < 3; ++i) hTimer[i] = CreateWaitableTimer (NULL, FALSE, NULL);
	
	LARGE_INTEGER liDueTime;
    liDueTime.QuadPart=10 * 10000000;
	SetWaitableTimer(hTimer[0], &liDueTime, 3600*2*1000, NULL, NULL, 0);
	SetWaitableTimer(hTimer[1], &liDueTime, 3600*6*1000, NULL, NULL, 0);
	SetWaitableTimer(hTimer[2], &liDueTime, 3600*24*1000, NULL, NULL, 0);

	while (true)
	{
		DWORD nj = WaitForMultipleObjects(3, &hTimer[0], FALSE, INFINITE);
		nj -= WAIT_OBJECT_0;
		switch (nj)
		{
		case 0:
			{
				StartIniUpdater (&iupd1[0]);
				StartIniUpdater (&iupd5[0]);
				SYSTEMTIME st;
				GetLocalTime (&st);
				int time = st.wHour*60 + st.wMinute;
				if (time>= 600 && time <= 1110)
				{
					StartIniUpdater (&iupd4[0]);
					StartIniUpdater (&iupd8[0]);
				}

			}	
			break;
		case 1:
			StartIniUpdater (&iupd2[0]);
			StartIniUpdater (&iupd6[0]);
			break;
		case 2:
			StartIniUpdater (&iupd3[0]);
			StartIniUpdater (&iupd7[0]);
			break;
			
		}
	}

	return 0;
}

