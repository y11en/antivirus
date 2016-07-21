////////////////////////////////////////////////////////////////////
//
//  AVP_DLL.H
//  AVP 3.0  Engine: AVPBASE.DLL API
//  AVP project
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __AVP_DLL_H
#define __AVP_DLL_H

#ifdef __cplusplus
#      define C extern "C"
#else
#      define C
#endif


#ifdef AVP_DLL
#      define DllExternal C __declspec(dllimport)
#      define DllInternal C __declspec(dllexport)
#else
#      define DllInternal C __declspec(dllimport)
#      define DllExternal C __declspec(dllexport)
#endif


DllInternal LONG AVP_RegisterCallback(HWND hWnd);
// This is strongly necessary function to make Callback available.

typedef LRESULT (WINAPI t_AvpCallback)(WPARAM wParam,LPARAM lParam);

DllInternal LONG AVP_RegisterCallbackPtr(t_AvpCallback* cbf_ptr_);
// This is strongly necessary function to make Callback available.

DllInternal LONG AVP_LoadBase(const char* name, int format); 
// name		- name of set file;
// format	- must be 0; 

DllInternal LONG AVP_UnloadBase(const char* name, int format);
// name     - must be NULL;


DllInternal LONG AVP_ProcessObject(AVPScanObject* ScanObject);
// this is what you want :-)

DllInternal void AVP_CancelProcessObject(LONG code);
// this is for aborting

DllInternal void AVP_SetCallbackNames(BOOL cn);
DllInternal void AVP_SetCallbackRecords(DWORD cn);

DllInternal LONG AVP_Dispatch(AVPScanObject* ScanObject, BYTE SubType);
// this function is for our purposes;

DllInternal char* AVP_MakeFullName(char* dest,const char* src);
// AVP_ProcessObject returns short names of viruses.
// to get full name use this function.
// dest must be at least MAX_PATH

DllInternal int AVP_AddMask(const char* mask);
// mask     - like  *.com;
// if ==NULL all masks clears.

DllInternal int AVP_AddExcludeMask(const char* mask);
// mask     - like  *.com;
// if ==NULL all masks clears.

DllInternal BYTE* AVP_GetPagePtr();
// this pointer is for our purposes;


DllInternal int AVP_CheckFileIntegrity(const char* fileName);

DllInternal BOOL AVP_SetTempPath(char* name);
// NULL - sets to default.(system TEMP)

DllInternal void AVP_Destroy();
// calls BAvpDestroy()

#endif//__AVP_DLL_H
