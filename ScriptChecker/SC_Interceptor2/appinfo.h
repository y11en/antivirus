#ifndef __SC_APPINFO_H__
#define __SC_APPINFO_H__

#define APP_FLAG_USE_GETITEMINFO      0x00000001
#define APP_FLAG_BROWSER              0x00000004
#define APP_FLAG_IEXPLORER            0x00000008
#define APP_FLAG_DONT_HOOK            0x00000010

unsigned long GetAppFlags();
unsigned long SetAppFlags(unsigned long nFlags);
bool CheckAppFlags(unsigned long nFlags);
char* GetAppName();


#endif // __SC_APPINFO_H__