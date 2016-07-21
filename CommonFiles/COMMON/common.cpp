//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#include "stdafx.h"
#include <afxdllx.h>
#include "defs.h"
#include "LockedBitmap.h"
#include "resource.h"

HMODULE g_hCommonLocRes = LoadLibrary("COLOC.dll");

static AFX_EXTENSION_MODULE COMMONDLLRes = { NULL, NULL};
static AFX_EXTENSION_MODULE COMMONDLL = { NULL, NULL };

COMMONEXPORT HINSTANCE g_hInstanceCommon;

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
extern "C" int APIENTRY DllMain ( HINSTANCE hInstance, DWORD dwReason, LPVOID )
{
     if   ( dwReason == DLL_PROCESS_ATTACH )
     {
          g_hInstanceCommon = hInstance;

          if   ( !AfxInitExtensionModule ( COMMONDLL, hInstance ) )
          {
               return 0;
          }
          new CDynLinkLibrary ( COMMONDLL );

		  if ( g_hCommonLocRes && AfxInitExtensionModule(COMMONDLLRes, g_hCommonLocRes) )
			  new CDynLinkLibrary(COMMONDLLRes);
		  
		  
		  g_hLockBitmap = (HBITMAP)::LoadImage(hInstance, 
			  MAKEINTRESOURCE(IDB_COMMON_LOCK_BITMAP), IMAGE_BITMAP, 0, 0, LR_SHARED);
		  
		  g_hLockBitmapMask = (HBITMAP)::LoadImage(hInstance, 
			  MAKEINTRESOURCE(IDB_COMMON_LOCK_BITMAP_MASK), IMAGE_BITMAP, 0, 0, LR_SHARED);
		  
		  BITMAP bm = {0}; 
		  ::GetObject(g_hLockBitmap, sizeof(BITMAP), &bm);

		  g_LockBitmapSize.cx = bm.bmWidth;
		  g_LockBitmapSize.cy = bm.bmHeight;

		  
     }
     else if ( dwReason == DLL_PROCESS_DETACH )
     {
          AfxTermExtensionModule ( COMMONDLL );
		  AfxTermExtensionModule ( COMMONDLLRes );

		  if (g_hCommonLocRes)
		  {
			  FreeLibrary(g_hCommonLocRes);
			  g_hCommonLocRes = NULL;
		  }
		  
     }
     return 1;
}

//==============================================================================
