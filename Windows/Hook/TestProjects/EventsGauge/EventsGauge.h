// EventsGauge.h : main header file for the EVENTSGAUGE application
//

#if !defined(AFX_EVENTSGAUGE_H__3402A86B_EDAE_4EFD_BED5_21BEB3A003E0__INCLUDED_)
#define AFX_EVENTSGAUGE_H__3402A86B_EDAE_4EFD_BED5_21BEB3A003E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CEventsGaugeApp:
// See EventsGauge.cpp for the implementation of this class
//

class CEventsGaugeApp : public CWinApp
{
public:
	CEventsGaugeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEventsGaugeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CEventsGaugeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTSGAUGE_H__3402A86B_EDAE_4EFD_BED5_21BEB3A003E0__INCLUDED_)
