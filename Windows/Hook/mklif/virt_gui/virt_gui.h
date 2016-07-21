// virt_gui.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cvirt_guiApp:
// See virt_gui.cpp for the implementation of this class
//

class Cvirt_guiApp : public CWinApp
{
public:
	Cvirt_guiApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cvirt_guiApp theApp;