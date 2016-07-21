// SimpleMessage.h: interface for the CSimpleMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLEMESSAGE_H__68B7FD03_7360_11D4_AD09_006052067816__INCLUDED_)
#define AFX_SIMPLEMESSAGE_H__68B7FD03_7360_11D4_AD09_006052067816__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "windows.h"
#include "mapihelper.h"
//#include <mapix.h>
//#pragma comment(lib, "exchsdk.lib")

class CSimpleMessage  
{
public:
	void Destroy();
	enum CErrorReason	{NoError, NoMAPISession, NoPrivateStore, NoOutbox, NoMessage, NoSearchKey, NoAddrList, NoModRecip, NoMessageBody, NoSubject, NoSave, NotAllSet, NoSubmit, NoMemory, NoAddrBook, NoResolve, NoAttach, NotImplemented, NoMethod};
	enum CSndrType		{TOField, CCField, BCCField};
	enum CAttachMethod	{AsFile, AsMessage};

	void GetErrorMessage(IN CErrorReason reason, OUT LPTSTR message, IN int size);
	CErrorReason Logon( LPTSTR lpszProfileName, LPTSTR lpszPassword = NULL );
	void Logoff();

	CErrorReason Send(BOOL bDeleteAfterSubmit = TRUE);
	CErrorReason Attach(LPCTSTR lpszFullFileName = NULL, LPMESSAGE lpMessage = NULL, CAttachMethod amMethod = AsFile, LPCTSTR lpszDisplayName = NULL);
	CErrorReason AttachValue(LPSBinary lpsbValue = NULL, LPCTSTR lpszDisplayName = NULL);
	CErrorReason SetSubject(LPCTSTR lpszSubject);
	CErrorReason SetBody(LPCTSTR lpszBody);
	CErrorReason AddRecipients(LPADRLIST lpAdrList);
	CErrorReason AddRecipient(LPCTSTR lpszDisplayName, LPCTSTR lpszAddrType, LPCTSTR lpszEmail, CSndrType stSndrType);
	CErrorReason Create(LPMAPISESSION lphSession = NULL, LPMDB lpMDB = NULL, LPADRBOOK lpAddrBook = NULL, LPMAPIFOLDER lpOutboxFolder = NULL);
	CSimpleMessage();
	virtual ~CSimpleMessage();

protected:
	LPMAPISESSION	m_pSession;
	LPADRBOOK m_lpAddrBook;
	BOOL m_bIsSubmitted;
	BOOL m_bRecipIsSet;
	BOOL m_bSubjectIsSet;
	BOOL m_bBodyIsSet;
	LPMESSAGE m_lpMessage;
	LPMAPIFOLDER m_lpOutbox;
	LPMDB m_lpMDB;
};

#endif // !defined(AFX_SIMPLEMESSAGE_H__68B7FD03_7360_11D4_AD09_006052067816__INCLUDED_)
