// appwizard.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "appwizard.h"
#include "appwizardaw.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif									        

static AFX_EXTENSION_MODULE AppwizardDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("APPWIZARD.AWX Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(AppwizardDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(AppwizardDLL);

		// Register this custom AppWizard with MFCAPWZ.DLL
		SetCustomAppWizClass(&Appwizardaw);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("APPWIZARD.AWX Terminating!\n");

		// Terminate the library before destructors are called
		AfxTermExtensionModule(AppwizardDLL);
	}
	return 1;   // ok
}
