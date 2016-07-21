#ifndef __Instance_Queue_h
#define __Instance_Queue_h

#include <windows.h>

HINSTANCE GetEngineInstance(OLECHAR* DllName, WCHAR* wcsLanguage);
HINSTANCE IsEngineLoaded(OLECHAR* DllName);
void FreeEngineQueue();
BOOL EngineCanUnload();

#endif