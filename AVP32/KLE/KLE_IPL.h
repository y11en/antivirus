#ifndef __KLE_IPL_h__
#define __KLE_IPL_h__

#include "../KLE/KLEapi.h"

#ifdef NT_DRIVER
#define String KUstring
#else
#define String char
#endif

typedef struct KLE_SCAN_OBJECT{
	KLE_F_CALLBACK* fnCallback;
	DWORD Status;
	BOOL  StopScan;
    String* ObjectName;
    String* TempPath;
    char* ArchiveName;
    char* VirusName;
    char* VirusType;
    void* UserData;
	void* RefData;
}KLE_SCAN_OBJECT;

LRESULT WINAPI KLE_AvpCallback(WPARAM wParam,LPARAM lParam);
BOOL WaitIfBusy();
void ReleaseWait();


extern KLE_FUNCTION_TABLE FunctionTable;
extern PCHAR KLE_SYS_Regname;
extern PCHAR KLE_SYS_Version;
extern PCHAR KLE_SYS_Compilation;
	
#endif//__KLE_IPL_h__