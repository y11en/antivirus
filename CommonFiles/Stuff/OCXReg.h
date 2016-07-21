////////////////////////////////////////////////////////////////////
//
//  OCXReg.h
//  OCX registration checking utility
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __OCXREG_H__
#define __OCXREG_H__


#include <windows.h>



struct CRInfoStruct {
				DWORD      dwStructSize;
				HWND			 hWndOwner;
	const TCHAR			 *pszCLSID;
	const TCHAR			 *pszFileName;
	      BOOL        bLeaveLoaded;
	const TCHAR      *pszMessageTitle;
	const TCHAR			 *pszNotRegisteredErrorTxt;
	const TCHAR			 *pszRegistrationErrorTxt;
	const TCHAR			 *pszOpenDlgTitle;
	const TCHAR			 *pszOpenDlgFilter;
				BOOL        bParanoidCheck;
};


BOOL CheckAndRegisterRequiredOCX( CRInfoStruct *pInfo );
BOOL CheckAndRegisterRequiredCOM( CRInfoStruct *pInfo );

/*
Description of a behavior :

The "CheckAndRegisterRequiredOCX" (and "CheckAndRegisterRequiredCOM" similarly) function checks 
if ActiveX (COM) with given CLSID (pszCLSID) and file name (pszFileName) has been registered 
and can be successfully loaded into memory. 

If "bParanoidCheck" is FALSE, functions only check whether object is registered.

If hasn't - the function creates error message from "pszNotRegisteredErrorTxt" and "pszFileName" 
and displays the message box with style MB_YESNO and "hWndOwner" as a parent window. 
So it's strictly recommended that "pszNotRegisteredErrorTxt" has a text like described
 in "Example" section.

If user clicks the "Yes" button on message box the function displays FileOpen dialog with given title
(pszOpenDlgTitle) and filter (pszOpenDlgFilter) and "hWndOwner" as a parent window. As a InitialDir 
for this dialog the folder AVPSharedFiles is used. In case of success of the FileOpen dialog 
the function tryes to register ActiveX. 

If registration fails the function creates error message from "pszRegistrationErrorTxt" and file name
user chose and displays the message box with style MB_YESNO. So it's also strictly recommended that 
"pszRegistrationErrorTxt" has	a text like described in "Example" section.

If registration performed successfully the function checks again if the ActiveX is registered. 
If not registered - the cycle repeates.

If ActiveX registered and field "bLeaveLoaded" is FALSE the function unloads ActiveX module.

The return results and its reasons :
TRUE  - Given ActiveX (COM) has been registered and can be successfully loaded;
FALSE - Given ActiveX (COM) hasn't been registered or can't be loaded and user clicked the button "No"
        on message box or "Cancel" on FileOpen dialog.

Example of structure filling :

dwStructSize							= sizeof(CRInfoStruct)
hWndOwner									= NULL;
*pszCLSID							    = _T("{D2B34AE8-B9CF-11D2-96B0-00104B5B66AA}");
*pszFileName							= _T("AvpCTT.ocx");
bLeaveLoaded							= TRUE;
*pszMessageTitle					= _T("Prague Visual Editor");
*pszNotRegisteredErrorTxt = _T("Required ActiveX module ""%s"" is not registered.\nWould you like to find and register it now?");
*pszRegistrationErrorTxt	= _T("Error registation ActiveX module\n""%s"".\nWould you like to choose and register another one?");
*pszOpenDlgTitle					= _T("Choose ActiveX module");
*pszOpenDlgFilter					= _T("ActiveX modules (*.ocx)|*.ocx|All files (*.*)|*.*||");

or 
{0x60FE9D12,0x06EA,0x11D4,{0x96,0xB1,0x00,0xD0,0xB7,0x1D,0xDD,0xF6}}

dwStructSize							= sizeof(CRInfoStruct)
hWndOwner									= NULL;
*pszCLSID							    = _T("{60FE9D12-06EA-11D4-96B1-00D0B71DDDF6}");
*pszFileName							= _T("IDServer.dll");
bLeaveLoaded							= TRUE;
*pszMessageTitle					= _T("Prague Visual Editor");
*pszNotRegisteredErrorTxt = _T("Required COM module ""%s"" is not registered.\nWould you like to find and register it now?");
*pszRegistrationErrorTxt	= _T("Error registation COM module\n""%s"".\nWould you like to choose and register another one?");
*pszOpenDlgTitle					= _T("Choose COM module");
*pszOpenDlgFilter					= _T("COM modules (*.dll)|*.dll|All files (*.*)|*.*||");

It is true that all strings should be localized.

*/

#endif // __OCXREG_H__
