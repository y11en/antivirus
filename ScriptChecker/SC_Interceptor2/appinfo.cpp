#include <windows.h>
#include "appinfo.h"
#include "debug.h"

static unsigned long s_nApplicationFlags = 0;
static bool s_bInited = false;
static char s_fn[MAX_PATH] = {0};
static char* s_fp = "<unknown>";

typedef struct tag_APP_INFO
{
	char* name;
	unsigned long flags;	
} APP_INFO;


static APP_INFO s_AppInfoArr[] = {
	{"wscript.exe",  APP_FLAG_USE_GETITEMINFO },
	{"cscript.exe",  APP_FLAG_USE_GETITEMINFO },
	{"iexplore.exe", APP_FLAG_BROWSER | APP_FLAG_IEXPLORER },
	{"avant.exe",    APP_FLAG_BROWSER },
	{"maxthon.exe",  APP_FLAG_BROWSER },
	{"mmc.exe",      APP_FLAG_DONT_HOOK },
};

#ifndef countof
  #define countof(array) (sizeof(array)/sizeof((array)[0]))
#endif

static bool iGetAppInfo()
{
	int i;
	if (s_bInited)
		return true;
	s_bInited = true;
	if (!GetModuleFileName(NULL, s_fn, countof(s_fn)))
		return false;
	s_fn[countof(s_fn)-1] = 0;
	s_fp = strrchr(s_fn, '\\');
	if (!s_fp)
		s_fp = s_fn;
	else
		s_fp++;
	if (*s_fp == 0)
		return false;
	for (i=0; i<countof(s_AppInfoArr); i++)
	{	
		if (0 == _stricmp(s_AppInfoArr[i].name, s_fp))
		{
			s_nApplicationFlags = s_AppInfoArr[i].flags;
			ODS(("SC: found %s, flags=%x", s_AppInfoArr[i].name, s_AppInfoArr[i].flags));
			return true;
		}
	}
	ODS(("SC: unknown app"));
	return true;
}


unsigned long GetAppFlags()
{
	iGetAppInfo();
	return s_nApplicationFlags;
}

unsigned long SetAppFlags(unsigned long nFlags)
{
	iGetAppInfo();
	s_nApplicationFlags |= nFlags;
	return s_nApplicationFlags;
}

bool CheckAppFlags(unsigned long nFlags)
{
	iGetAppInfo();
	return !!(s_nApplicationFlags & nFlags);
}

char* GetAppName()
{
	iGetAppInfo();
	return s_fp;
}

