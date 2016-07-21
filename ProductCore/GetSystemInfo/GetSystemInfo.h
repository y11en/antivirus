//#include "GetSystemInfoMain.h"
#include "SubDefines.h"

extern enum SI_INSIDE_OS_NUMBER;

void				SysInfoInit(void);
void				SysInfoDeInit(void);
SI_INSIDE_OS_NUMBER	IsRunningWindowsNT(void);
DWORD				ScanThread(COUNTSTRUCT *CountStruct);
void				SetToNull(COUNTSTRUCT *CountStruct,char* name);

