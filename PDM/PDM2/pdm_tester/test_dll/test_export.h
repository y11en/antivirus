#include "stdafx.h"

#define DllExport __declspec( dllexport )
extern "C" {
DllExport BOOL SetHook();
DllExport BOOL DelHook();
DllExport BOOL SetOldHook();
DllExport BOOL DelOldHook();
DllExport BOOL SetHookByThreadID(DWORD ThreadID);
DllExport BOOL DelHookByThreadID();

}
