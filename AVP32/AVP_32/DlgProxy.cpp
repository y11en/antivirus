// DlgProxy.cpp : implementation file
//

#include "stdafx.h"
#include "Avp32.h"
#include "DlgProxy.h"
#include "Avp32Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAvp32DlgAutoProxy

IMPLEMENT_DYNCREATE(CAvp32DlgAutoProxy, CCmdTarget)

CAvp32DlgAutoProxy::CAvp32DlgAutoProxy()
{
	EnableAutomation();
	
	// To keep the application running as long as an automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();

	// Get access to the dialog through the application's
	//  main window pointer.  Set the proxy's internal pointer
	//  to point to the dialog, and set the dialog's back pointer to
	//  this proxy.
	ASSERT (AfxGetApp()->m_pMainWnd != NULL);
	ASSERT_VALID (AfxGetApp()->m_pMainWnd);
	ASSERT_KINDOF(CAvp32Dlg, AfxGetApp()->m_pMainWnd);
	m_pDialog = (CAvp32Dlg*) AfxGetApp()->m_pMainWnd;
	m_pDialog->m_pAutoProxy = this;
}

CAvp32DlgAutoProxy::~CAvp32DlgAutoProxy()
{
	// To terminate the application when all objects created with
	// 	with automation, the destructor calls AfxOleUnlockApp.
	//  Among other things, this will destroy the main dialog
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CAvp32DlgAutoProxy::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CAvp32DlgAutoProxy, CCmdTarget)
	//{{AFX_MSG_MAP(CAvp32DlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAvp32DlgAutoProxy, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CAvp32DlgAutoProxy)
	DISP_FUNCTION(CAvp32DlgAutoProxy, "ProcessObject", ProcessObject, VT_BOOL, VTS_I4 VTS_I4 VTS_BSTR VTS_PI4 VTS_PBSTR)
	DISP_FUNCTION(CAvp32DlgAutoProxy, "ReloadBase", ReloadBase, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CAvp32DlgAutoProxy, "CancelProcessObject", CancelProcessObject, VT_BOOL, VTS_I4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IAvp32 to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {13C51EC7-AE3D-11D3-A4CD-0000E8E1E96D}
static const IID IID_IAvp32 =
{ 0x13c51ec7, 0xae3d, 0x11d3, { 0xa4, 0xcd, 0x0, 0x0, 0xe8, 0xe1, 0xe9, 0x6d } };

BEGIN_INTERFACE_MAP(CAvp32DlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CAvp32DlgAutoProxy, IID_IAvp32, Dispatch)
END_INTERFACE_MAP()

// The IMPLEMENT_OLECREATE2 macro is defined in StdAfx.h of this project
// {13C51EC5-AE3D-11D3-A4CD-0000E8E1E96D}
IMPLEMENT_OLECREATE2(CAvp32DlgAutoProxy, "Avp32.Application", 0x13c51ec5, 0xae3d, 0x11d3, 0xa4, 0xcd, 0x0, 0x0, 0xe8, 0xe1, 0xe9, 0x6d)

/////////////////////////////////////////////////////////////////////////////
// CAvp32DlgAutoProxy message handlers

BOOL CAvp32DlgAutoProxy::ProcessObject(long Type, long MFlags, LPCTSTR Name, long FAR* RFlags, BSTR FAR* VirusName) 
{
	// TODO: Add your dispatch handler code here

	return TRUE;
}

BOOL CAvp32DlgAutoProxy::ReloadBase() 
{
	// TODO: Add your dispatch handler code here

	return TRUE;
}

BOOL CAvp32DlgAutoProxy::CancelProcessObject(long Code) 
{
	// TODO: Add your dispatch handler code here

	return TRUE;
}
