// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__4361D0AD_9740_11D3_9CF4_0000E8905EBC__INCLUDED_)
#define AFX_STDAFX_H__4361D0AD_9740_11D3_9CF4_0000E8905EBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <winioctl.h>

#include <Stuff\Layout.h>

#define _AVP_GUARD	"KL Guard"
#include "Reestr.h"
extern CReestr *g_pReestr;

#include "Localize.h"
extern CLocalize* g_pLocalize;

extern OSVERSIONINFO g_OSVer;

#define _REQUEST_SAVE		0
#define _REQUEST_APLLY		1
#define _REQUEST_CANCEL		2

#define FOP_WRITE_EXE		1
#define FOP_DEL_EXE			2
#define FOP_RENAME_EXE		3

#define DEFAULT_TIMEOUT		300 //In sec

#include <AVPCONTROLS\wascmess.h>
#include <hook\avpgcom.h>
#include <hook\funcs.h>
#include <hook\HookSpec.h>
#include <hook\NETID.h>

#include "FiltersStorage.h"

#include "property/property.h"
#include "datatreeutils/dtutils.h"

extern APP_REGISTRATION AppReg;

extern CHAR* FillFunctionsList(CStringList *pStringList, CHAR  **FuncNameArr,DWORD FuncNameArrSize, DWORD CurrentPos, CHAR* pstrFun = NULL, DWORD* Idx = NULL);
extern CHAR* FillFuncList(CStringList *pStringList, DWORD Func, DWORD FuncMj, DWORD FuncMi, CHAR* pstrFun = NULL, DWORD* Idx = NULL);
extern CHAR* FillMajList(CStringList *pStringList, DWORD Func, DWORD FuncMj, DWORD FuncMi, CHAR* pstrFun = NULL, DWORD* Idx = NULL);
extern CHAR* FillMinList(CStringList *pStringList, DWORD Func, DWORD FuncMj, DWORD FuncMi, CHAR* pstrFun = NULL, DWORD* Idx = NULL);
extern CString GetParamString(PFILTER_TRANSMIT pFilter);
extern CString GetFilterDesciptions(PFILTER_TRANSMIT pFilter);
extern CString GetRequestTimeoutString(PFILTER_TRANSMIT pFilter);
extern void SetItemText(char** ptch, CString* pstr);
extern bool String2Long(CString* str, long* lng);
extern bool String2US(CString* str, unsigned short* lng);

#define WU_STARTINIT	WM_USER + 101
#define WU_REFRESHTREE	WM_USER + 102 

extern APP_REGISTRATION AppReg;

#define FILTER_BUFFER_SIZE 4096

extern PCHAR GetObjNameFromFilter(PFILTER_TRANSMIT pFilter);
extern BOOL ChangeFilterParamValue(PFILTER_TRANSMIT pFilter, PFILTER_PARAM	pNewParam);
extern PFILTER_PARAM GetFilterParam(PFILTER_TRANSMIT pFilter, ULONG ParamID);

typedef struct _IPV4
{
	BYTE	ip[4];
}IPV4, *PIPV4;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__4361D0AD_9740_11D3_9CF4_0000E8905EBC__INCLUDED_)
