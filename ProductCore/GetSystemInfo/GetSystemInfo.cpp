// GetSystemInfo.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "GetSystemInfo.h"

#include "SoftWareInfo.h"
#include "MemoryScan.h"
#include "HardWareInfo.h"
#include "StartUpLinks.h"

OSVERSIONINFOEX		versionInfo;
bool				ExType;
SI_INSIDE_OS_NUMBER	os;

HMODULE				hKernel=NULL;
FILE*				LFile=NULL;
HANDLE				Sync=NULL;

fCallBackFunc		pfCallBackFunc;

void SysInfoInit(void)
{
	__try
	{
		Sync=CreateEvent(NULL,FALSE,FALSE,"KL System Information collector");
		os=IsRunningWindowsNT();
		hKernel=LoadLibrary("kernel32.dll");
		return;
	}
	__finally
	{
	}
}

void SysInfoDeInit(void)
{
	__try
	{
		if (Sync!=NULL) CloseHandle(Sync);
		if (hKernel!=NULL) FreeLibrary(hKernel);
		hKernel=NULL;
		return;
	}
	__finally
	{
	}
}

SI_INSIDE_OS_NUMBER IsRunningWindowsNT(void)
{
	SI_INSIDE_OS_NUMBER	Ret=OS_UNKNOWN;
	_try
	{
		versionInfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
		if ((::GetVersionEx((OSVERSIONINFO*)&versionInfo))==0)
		{
			ExType=false;
			versionInfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
			if ((GetVersionEx((OSVERSIONINFO*)&versionInfo))==0) return Ret;
		}
		switch (versionInfo.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_NT:
			if (versionInfo.dwMajorVersion<=3) Ret=OS_WINNT351; //WinNT
			else
			{
				if (versionInfo.dwMajorVersion==4) Ret=OS_WINNT40; //WinNT
				else
				{
					if ((versionInfo.dwMajorVersion==5)&&(versionInfo.dwMinorVersion==0)) Ret=OS_WIN2000; //Win2000
					else
					{
						if ((versionInfo.dwMajorVersion==5)&&(versionInfo.dwMinorVersion==1)) Ret=OS_WINXP; //WinXP
						else
						{
							if ((versionInfo.dwMajorVersion==5)&&(versionInfo.dwMinorVersion==2)) Ret=OS_WINXPNET; //WinXP
							else Ret=OS_WIN_UNKNOWN;
						}
					}
				}
			}
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
			if ((versionInfo.dwMajorVersion==4)&&(versionInfo.dwMinorVersion>=90)) Ret=OS_WINME; //WinME
			else
			{
				if ((versionInfo.dwMajorVersion==4)&&(versionInfo.dwMinorVersion>=10)) Ret=OS_WIN98; //Win98
				else
				{
					if ((versionInfo.dwMajorVersion==4)&&(versionInfo.dwMinorVersion>=0)) Ret=OS_WIN95; //Win95
					else Ret=OS_WIN_UNKNOWN;
				}
			}
			break;
		case VER_PLATFORM_WIN32s:
			Ret=OS_WIN32;
			break;
		default:
			break;
		}
		return Ret;
	}
	__finally
	{
	}
	return Ret;
}

GETSYSTEMINFO_API long __stdcall SaveReport(char* FileName, fCallBackFunc pfCallBack)
{
	long			Ret=0;
	COUNTSTRUCT		CountStruct;
	__try
	{
		pfCallBackFunc=pfCallBack;
		LFile=fopen(FileName,"w");
		if (LFile!=NULL) 
		{
			CountStruct.LocalStep=0;
			CountStruct.LocalStepCount=0;
			CountStruct.GlobalStep=-1;
			CountStruct.GlobalStepCount=INFO_TYPE_COUNT;
			CountStruct.ObjectName=NULL;
			Ret=ScanThread(&CountStruct);
		}
		return Ret;
	}
	__finally
	{
		pfCallBackFunc=NULL;
		if (LFile!=NULL) fclose(LFile);
	}
	return Ret;
}

char	CharTable[INFO_TYPE_COUNT][38]={
	"Get processor type.",
	"Get processor number.",
	"Get BIOS information.",
	"Get system date information.",
	"Get memory information.",
	"Get hard drives information.",
	"Get logical structure of hard drives.",
	"Get video devices information.",
	"Get network adapters information.",
	"Get modems information.",
	"Get multimedia devices information.",
	"Get printer information.",
	"Get removable devices information.",
	"Get operating system version.",
	"Get environment variables.",
	"Get network protocols.",
	"Get installed applications.",
	"Get running drivers.",
	"Get running processes and modules.",
	"Get system default language.",
	"Get operating system localisation.",
	"Get installed keyboard locales.",
	"Get installed code pages.",
	"Get StartUp link objects.",
};

DWORD ScanThread(pCOUNTSTRUCT CountStruct)
{
	bool	Stopped=false;
	HANDLE	hEv=NULL;
	__try
	{
		if ((hEv=OpenEvent(EVENT_ALL_ACCESS,FALSE,"KL System Information collector"))==NULL) return 0;
//		SetToNull(CountStruct,(char*)CharTable[0]);
//		if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
//		os=IsRunningWindowsNT();
		hKernel=LoadLibrary("kernel32.dll");
		if (hKernel!=NULL)
		{
			fprintf(LFile,"HARDWARE Information:\n---------------------\n");
			fflush(LFile);
			SetToNull(CountStruct,(char*)CharTable[0]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (ProcessorInfo(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[1]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (ProcessNumberInfo(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[2]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (BIOSInfo(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[3]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (GetDateInfo(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[4]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (MemoryInfo(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[5]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (GetHardDiskInfo(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[6]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (GetFreeSystemDiskSizeInfo(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[7]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (GetVideoInfo(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[8]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (GetNetworkAdaptersInfo(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[9]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (GetModemInfo(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[10]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (GetMultimediaInformation(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[11]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (GetPrinterInformation(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[12]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (GetRemovablesInfo(NULL,CountStruct,hEv)) return 0;
			
			fprintf(LFile,"SOFTWARE Information:\n---------------------\n");
			fflush(LFile);
			SetToNull(CountStruct,(char*)CharTable[13]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (ReadOSVersion(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[14]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (GetEnvironmentVariables(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[15]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (GetNetProtocolInfo(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[16]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (GetInstalledAppInfo(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[17]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (DriversEnum(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[18]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (ProcessEnum(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[19]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (OSSysDefLang(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[20]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (OSLocaleInfo(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[21]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (GetKeyboardLocaleInfo(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,(char*)CharTable[22]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (GetCodePageInfo(NULL,CountStruct,hEv)) return 0;
			fflush(LFile);

			fprintf(LFile,"\nStartUp link objects:\n---------------------\n");
			fflush(LFile);
			SetToNull(CountStruct,(char*)CharTable[23]);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			if (WriteStartUpInfo(NULL,CountStruct,hEv)) return 0;
			SetToNull(CountStruct,NULL);
			if (CheckStop(NULL,0,CountStruct,true,hEv)) return 0;
			fflush(LFile);
		}
//		if (!Stopped) PostMessage((HWND)lpParam,WM_COMMAND,(DWORD)IDOK,0);
		return 0;
	}
	__finally
	{
		if (hKernel!=NULL) FreeLibrary(hKernel);
	}
	return 0;
}

void SetToNull(pCOUNTSTRUCT CountStruct,char* name)
{
	if (!CountStruct) return;
	CountStruct->LocalStep=0;
	CountStruct->LocalStepCount=0;
	CountStruct->ObjectName=name;
	return;
}
