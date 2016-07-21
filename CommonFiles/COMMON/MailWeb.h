// MailWeb.h: interface for the MailWeb class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAILWEB_H__7947DEB9_14C7_4B54_A5F8_EC53CF53E24A__INCLUDED_)
#define AFX_MAILWEB_H__7947DEB9_14C7_4B54_A5F8_EC53CF53E24A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mapi.h>
#include "../KLUtil/SysTime.h"

#define KAP_COMMON_ERROR		(-1)

struct SendMailProdInfo
{
	std::string strProduct;

	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;

	KLUTIL::CSysTime tmLastUpdate;
	DWORD dwKnownVirusBody;

	bool bHasKey;

	DWORD dwKeyMemberId;
	DWORD dwKeyAppId;
	DWORD dwKeySerNum;
	
	std::string strKeyInfo;
	KLUTIL::CSysTime tmKeyExpiration;
};

COMMONEXPORT bool SendMail(MapiMessage *pNote, HWND hWnd, 
							bool bAddSysInfoToBody = false,
							const SendMailProdInfo *pProdInfo = NULL);
COMMONEXPORT void OpenURL( LPCTSTR szURL );




#endif // !defined(AFX_MAILWEB_H__7947DEB9_14C7_4B54_A5F8_EC53CF53E24A__INCLUDED_)
