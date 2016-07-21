#ifndef __GET_SYSTEM_INFO_H__
#define __GET_SYSTEM_INFO_H__

#ifdef GETSYSTEMINFO_EXPORTS
#define GETSYSTEMINFO_API __declspec(dllexport)
#elif GETSYSTEMINFO_IMPORTS
#define GETSYSTEMINFO_API __declspec(dllimport)
#else
#define GETSYSTEMINFO_API
#endif

typedef long (__stdcall *fCallBackFunc)(int TotalProcent, int CurrentProcent, char* name);
typedef long (__stdcall *fSaveReport)(char* FileName, fCallBackFunc pfCallBack);

extern "C" {
	GETSYSTEMINFO_API long __stdcall SaveReport(char* FileName, fCallBackFunc pfCallBack);
}

#endif // __GET_SYSTEM_INFO_H__