/**************************************************************************

   File:          Globals.h
   
**************************************************************************/

/**************************************************************************
   global variables
**************************************************************************/

extern HINSTANCE  g_hInst;
extern UINT       g_DllRefCount;

#if !defined(ARRAYSIZE)
	#define ARRAYSIZE(a)    (sizeof(a)/sizeof(a[0]))
#endif

#define EB_CLASS_NAME 	(TEXT("ExplorerBarSampleClass"))
