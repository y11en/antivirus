// SimpleMessage.cpp: implementation of the CSimpleMessage class.
//
//////////////////////////////////////////////////////////////////////

//#include <edk.h>
#include "..\..\stdafx.h"
#include "SimpleMessage.h"
#include "MAPIHelper.h"

//DEFINE_GUID(CLSID_MailMessage, 0x00020D0B, 0x0000, 0x0000, 0xC0, 0x00, 0x0, 0x00, 0x0, 0x00, 0x00, 0x46);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimpleMessage::CSimpleMessage()
{
	m_pSession = NULL;
	m_lpMDB = NULL;
	m_lpOutbox = NULL;
	m_lpMessage = NULL;
	m_lpAddrBook = NULL;
	m_bBodyIsSet = FALSE;
	m_bRecipIsSet = FALSE;
	m_bSubjectIsSet = FALSE;
	m_bIsSubmitted = FALSE;
}

CSimpleMessage::~CSimpleMessage()
{
	Destroy();
}

CSimpleMessage::CErrorReason CSimpleMessage::Logon( LPTSTR lpszProfileName, LPTSTR lpszPassword )
{
	CErrorReason	erRes		=	NoError;
	
	MAPIINIT_0 MAPIINIT = { 0, MAPI_MULTITHREAD_NOTIFICATIONS };
	HRESULT hr = g_pMAPIEDK->pMAPIInitialize(&MAPIINIT);
	
	if ( SUCCEEDED(hr) ) {
		
		hr = g_pMAPIEDK->pMAPILogonEx( 
			0, 
			lpszProfileName, 
			lpszPassword, 
			MAPI_EXTENDED | MAPI_NEW_SESSION | MAPI_NT_SERVICE | MAPI_EXPLICIT_PROFILE, 
			&m_pSession 
			);
		if ( FAILED(hr) ) 
			erRes = NoMAPISession;
	}
	else
		erRes = NoMAPISession;
	
	return erRes;
}

void CSimpleMessage::Logoff()
{
	if(m_pSession)
		m_pSession->Logoff(0, 0, 0);
	
	m_pSession = NULL;
	
	g_pMAPIEDK->pMAPIUninitialize();
}

CSimpleMessage::CErrorReason CSimpleMessage::Create(LPMAPISESSION lphSession, LPMDB lpMDB, LPADRBOOK lpAddrBook, LPMAPIFOLDER lpOutboxFolder)
{
	CErrorReason	erRes		=	NoError;
	HRESULT			hr			=	NOERROR;
	LPENTRYID		lpeid		=	NULL;
	ULONG			cbeid		=	0;
	ULONG			ulObjType	=	0;

	if (!lphSession)
	{
		lphSession = m_pSession;
	}

	if (lpOutboxFolder == NULL)
	{
		if (lpMDB == NULL)
		{
			if (lphSession != NULL)
			{
				hr = g_pMAPIEDK->pHrOpenExchangePrivateStore(lphSession, &m_lpMDB);

				if (SUCCEEDED(hr))
				{
					hr = g_pMAPIEDK->pHrMAPIFindOutbox(m_lpMDB, &cbeid, &lpeid);

					if (SUCCEEDED(hr))
					{
						hr = m_lpMDB->OpenEntry(
							cbeid,
							lpeid,
							NULL,
							MAPI_BEST_ACCESS,
							&ulObjType,
							reinterpret_cast<LPUNKNOWN *>(&m_lpOutbox));

						if ((SUCCEEDED(hr)) && (ulObjType == MAPI_FOLDER))
						{
							hr = m_lpOutbox->CreateMessage(
								NULL,
								MAPI_DEFERRED_ERRORS,
								&m_lpMessage);

							if (FAILED(hr))
								erRes = NoMessage;
						}
						else
							erRes = NoOutbox;

						MAPIFREEBUFFER(lpeid);
					}
					else
						erRes = NoOutbox;
				}
				else
					erRes = NoPrivateStore;
			}
			else
				erRes = NoMAPISession;
		}
		else
		{
			hr = g_pMAPIEDK->pHrMAPIFindOutbox(lpMDB, &cbeid, &lpeid);

			if (SUCCEEDED(hr))
			{
				hr = lpMDB->OpenEntry(
					cbeid,
					lpeid,
					NULL,
					MAPI_BEST_ACCESS,
					&ulObjType,
					reinterpret_cast<LPUNKNOWN *>(&m_lpOutbox));

				if ((SUCCEEDED(hr)) && (ulObjType == MAPI_FOLDER))
				{
					hr = m_lpOutbox->CreateMessage(
						NULL,
						MAPI_DEFERRED_ERRORS,
						&m_lpMessage);

					if (FAILED(hr))
						erRes = NoMessage;
				}
				else
					erRes = NoOutbox;

				MAPIFREEBUFFER(lpeid);
			}
			else
				erRes = NoOutbox;
		}
	}
	else
	{
		hr = lpOutboxFolder->CreateMessage(
			NULL,
			MAPI_DEFERRED_ERRORS,
			&m_lpMessage);

		if (FAILED(hr))
			erRes = NoMessage;
	}

	if (lpAddrBook != NULL)
	{
		m_lpAddrBook = lpAddrBook;
		m_lpAddrBook->AddRef();
	}
	else
	{
		if (lphSession != NULL)
		{
			hr = lphSession->OpenAddressBook(0, NULL, AB_NO_DIALOG, &m_lpAddrBook);

			if (FAILED(hr))
				erRes = NoAddrBook;
		}
		else
			erRes = NoMAPISession;
	}

	return erRes;
}

CSimpleMessage::CErrorReason CSimpleMessage::AddRecipient(LPCTSTR lpszDisplayName, LPCTSTR lpszAddrType, LPCTSTR lpszEmail, CSndrType stSndrType)
{
	CErrorReason	erRes			=	NoError;
	HRESULT			hr				=	NOERROR;
	LPADRLIST		lpAddrList		=	NULL;
	LPSPropValue	lpspvAddrProp	=	NULL;

	//    enum {SNDR_NAME, SNDR_ADDRTYPE, SNDR_ADDR, SNDR_EID, SNDR_SKEY, SNDR_TYPE, NUM_PROPS};
	enum {SNDR_NAME, SNDR_ADDRTYPE, SNDR_ADDR, SNDR_EID, SNDR_SKEY, SNDR_TYPE, SNDR_TNEF, NUM_PROPS};
	
	hr = g_pMAPIEDK->pMAPIAllocateBuffer(CbSPropValue(NUM_PROPS), reinterpret_cast<LPVOID *>(&lpspvAddrProp));

	if (SUCCEEDED(hr))
	{
		lpspvAddrProp[SNDR_NAME].ulPropTag = PR_DISPLAY_NAME;
		lpspvAddrProp[SNDR_NAME].Value.LPSZ = const_cast<LPTSTR>(lpszDisplayName);
		lpspvAddrProp[SNDR_ADDRTYPE].ulPropTag = PR_ADDRTYPE;
		lpspvAddrProp[SNDR_ADDRTYPE].Value.LPSZ = const_cast<LPTSTR>(lpszAddrType);
		lpspvAddrProp[SNDR_ADDR].ulPropTag = PR_EMAIL_ADDRESS;
		lpspvAddrProp[SNDR_ADDR].Value.LPSZ = const_cast<LPTSTR>(lpszEmail);
		lpspvAddrProp[SNDR_EID].ulPropTag = PR_ENTRYID;
		lpspvAddrProp[SNDR_EID].Value.bin.cb = 0;
		lpspvAddrProp[SNDR_EID].Value.bin.lpb = NULL;
		lpspvAddrProp[SNDR_TYPE].ulPropTag = PR_RECIPIENT_TYPE;
		lpspvAddrProp[SNDR_TNEF].ulPropTag = PR_SEND_RICH_INFO;
		lpspvAddrProp[SNDR_TNEF].Value.b = FALSE;
		if (stSndrType == TOField)
			lpspvAddrProp[SNDR_TYPE].Value.ul = MAPI_TO;
		else if (stSndrType == CCField)
			lpspvAddrProp[SNDR_TYPE].Value.ul = MAPI_CC;
		else if (stSndrType == BCCField)
			lpspvAddrProp[SNDR_TYPE].Value.ul = MAPI_BCC;

		lpspvAddrProp[SNDR_SKEY].ulPropTag = PR_SEARCH_KEY;

		hr = HrCreateSearchKey(lpspvAddrProp, const_cast<LPTSTR>(lpszAddrType), const_cast<LPTSTR>(lpszEmail), reinterpret_cast<LPTSTR *>(&(lpspvAddrProp[SNDR_SKEY].Value.bin.lpb)));

		if (SUCCEEDED(hr))
		{
			lpspvAddrProp[SNDR_SKEY].Value.bin.cb = cbStrLen(reinterpret_cast<LPTSTR>(lpspvAddrProp[SNDR_SKEY].Value.bin.lpb));

			hr = g_pMAPIEDK->pHrMAPICreateAddressList(NUM_PROPS, lpspvAddrProp, &lpAddrList);

			if (SUCCEEDED(hr))
			{
				hr = m_lpAddrBook->ResolveName(0, 0, NULL, lpAddrList);

				if (SUCCEEDED(hr))
				{
					hr = m_lpMessage->ModifyRecipients(MODRECIP_ADD, lpAddrList);

					if (SUCCEEDED(hr))
						m_bRecipIsSet = TRUE;
					else
						erRes = NoModRecip;
				}
				else
					erRes = NoResolve;

				FREEPADRLIST(lpAddrList);
			}
			else
				erRes = NoAddrList;
		}
		else
			erRes = NoSearchKey;

		MAPIFREEBUFFER(lpspvAddrProp);
	}
	else
		erRes = NoMemory;

	return erRes;
}

CSimpleMessage::CErrorReason CSimpleMessage::SetBody(LPCTSTR lpszBody)
{
	CErrorReason	erRes	=	NoError;
	HRESULT			hr		=	NOERROR;

	if (lpszBody != NULL)
	{
		hr = g_pMAPIEDK->pHrMAPISetPropString(m_lpMessage, PR_BODY, static_cast<LPVOID>(const_cast<LPTSTR>(lpszBody)));

		if (SUCCEEDED(hr))
			m_bBodyIsSet = TRUE;
		else
			erRes = NoMessageBody;
	}
	else
		erRes = NoMessageBody;

	return erRes;
}

CSimpleMessage::CErrorReason CSimpleMessage::SetSubject(LPCTSTR lpszSubject)
{
	CErrorReason	erRes	=	NoError;
	HRESULT			hr		=	NOERROR;

	if (lpszSubject != NULL)
	{
		hr = g_pMAPIEDK->pHrMAPISetPropString(m_lpMessage, PR_SUBJECT, static_cast<LPVOID>(const_cast<LPTSTR>(lpszSubject)));

		if (SUCCEEDED(hr))
			m_bSubjectIsSet = TRUE;
		else
			erRes = NoSubject;
	}
	else
		erRes = NoSubject;

	return erRes;
}

HRESULT SetPR_SENTMAIL_ENTRYID(LPMAPIPROP lpMessage, LPMDB lpMDB )
{
	LPMAPIFOLDER pSentItemsFolder = NULL;
	HRESULT hr = S_OK;
	hr = g_pMAPIEDK->pHrMAPIOpenFolderEx(
		lpMDB, 
		'\\', 
		_T("@PR_IPM_SENTMAIL_ENTRYID"), 
		&pSentItemsFolder
		); 
	if ( SUCCEEDED(hr) && pSentItemsFolder )
	{
		LPSPropValue pProp = NULL;
		HRESULT hr = g_pMAPIEDK->pHrGetOneProp(pSentItemsFolder, PR_ENTRYID, &pProp);
		if ( SUCCEEDED(hr) && pProp )
		{
			g_pMAPIEDK->pHrMAPISetPropBinary(
				lpMessage, 
				PR_SENTMAIL_ENTRYID, 
				pProp->Value.bin.cb, 
				pProp->Value.bin.lpb
				);
		}
		MAPIFREEBUFFER(pProp);
	}
	ULRELEASE(pSentItemsFolder);
    return hr;
}

CSimpleMessage::CErrorReason CSimpleMessage::Send(BOOL bDeleteAfterSubmit)
{
	CErrorReason	erRes	=	NoError;
	HRESULT			hr		=	NOERROR;

	g_pMAPIEDK->pHrMAPISetPropString(m_lpMessage, PR_MESSAGE_CLASS, "IPM.Note");
	g_pMAPIEDK->pHrMAPISetPropBoolean(m_lpMessage, PR_DELETE_AFTER_SUBMIT, bDeleteAfterSubmit);
	if ( !bDeleteAfterSubmit )
		SetPR_SENTMAIL_ENTRYID(m_lpMessage, m_lpMDB);

	if (m_lpMessage != NULL)
	{
		hr = m_lpMessage->SaveChanges(KEEP_OPEN_READWRITE);

		if (SUCCEEDED(hr))
		{
			if ((m_bBodyIsSet == TRUE) && (m_bRecipIsSet == TRUE) && (m_bSubjectIsSet == TRUE))
			{
				hr = m_lpMessage->SubmitMessage(0);

				if (SUCCEEDED(hr))
					m_bIsSubmitted = TRUE;
				else
					erRes = NoSubmit;
			}
			else
				erRes = NotAllSet;
		}
		else
			erRes = NoSave;
	}
	else
		erRes = NoMessage;

	return erRes;
}

void CSimpleMessage::Destroy()
{
	ULRELEASE(m_lpAddrBook);
	ULRELEASE(m_lpMessage);
	ULRELEASE(m_lpOutbox);
	ULRELEASE(m_lpMDB);
}

CSimpleMessage::CErrorReason CSimpleMessage::Attach(LPCTSTR lpszFullFileName, LPMESSAGE lpMessage, CAttachMethod amMethod, LPCTSTR lpszDisplayName)
{
	CErrorReason	erRes	=	NoError;
	HRESULT			hr		=	NOERROR;

	if (((amMethod == AsFile) && !lpszFullFileName) || ((amMethod == AsMessage) && !lpMessage))
		erRes = NoAttach;
	else
	{
		if (m_lpMessage)
		{
			ULONG		ulAttachNum	=	0;
			LPATTACH	lpAttach	=	0;

			hr = m_lpMessage->CreateAttach(0, 0, &ulAttachNum, &lpAttach);
			
			if (SUCCEEDED(hr))
			{
				if (amMethod == AsMessage)
				{
					hr = g_pMAPIEDK->pHrMAPISetPropLong(lpAttach, PR_ATTACH_METHOD, ATTACH_EMBEDDED_MSG);

					if (SUCCEEDED(hr))
					{
						ULONG	ulCount			=	0;
						LPTSTR	lpszOrigSubj	=	0;

						hr = g_pMAPIEDK->pHrMAPIGetPropString(lpMessage, PR_SUBJECT, &ulCount, reinterpret_cast<LPVOID *>(&lpszOrigSubj));

						if (SUCCEEDED(hr))
						{
							hr = g_pMAPIEDK->pHrMAPISetPropString(lpAttach, PR_DISPLAY_NAME, lpszOrigSubj);

							if (SUCCEEDED(hr))
							{
								LPMESSAGE	lpDestinationMessage	=	0;

								hr = lpAttach->OpenProperty(PR_ATTACH_DATA_OBJ, &IID_IMessage,
									0, MAPI_CREATE | MAPI_MODIFY,
									reinterpret_cast<LPUNKNOWN *>(&lpDestinationMessage));

								if (SUCCEEDED(hr))
								{
									// Здесь может быть проблемка - INVALID_ARG.
									// По-хорошему, нужно исключать некоторые проперти
									// См. Q170226

									hr = lpMessage->CopyTo(0, 0, 0, 0, 0, &IID_IMessage, 
										lpDestinationMessage, 0, 0);

									if (SUCCEEDED(hr))
									{
										lpDestinationMessage->SaveChanges(KEEP_OPEN_READWRITE);
										lpAttach->SaveChanges(KEEP_OPEN_READWRITE);
									}
									else
										erRes = NoAttach;

									ULRELEASE(lpDestinationMessage);
								}
								else
									erRes = NoAttach;
							}
							else
								erRes = NoAttach;

							MAPIFREEBUFFER(lpszOrigSubj);
						}
						else
							erRes = NoMessage;
					}
					else
						erRes = NoAttach;
				}
				else if (amMethod == AsFile)
				{
					// Add 03.08.01 by DVi
					//erRes = NotImplemented;
					const TCHAR	*lpszFileName = NULL;
					
					hr = g_pMAPIEDK->pHrMAPISetPropLong(lpAttach, PR_ATTACH_METHOD, ATTACH_BY_REF_RESOLVE  );
					
					if (FAILED(hr))
						goto cleanup;
					
					lpszFileName = strrchr(lpszFullFileName, '\\');
					
					if (!lpszFileName)
						lpszFileName = (LPTSTR)lpszFullFileName;
					else
						lpszFileName++;	// Advance past the backslash.
					
					hr = g_pMAPIEDK->pHrMAPISetPropString(lpAttach, PR_ATTACH_FILENAME, (void *)lpszFileName);
					
					if (FAILED(hr))
						goto cleanup;
					
					hr = g_pMAPIEDK->pHrMAPISetPropString(lpAttach, PR_ATTACH_LONG_FILENAME, (void *)lpszFileName);
					
					if (FAILED(hr))
						goto cleanup;

					hr = g_pMAPIEDK->pHrMAPISetPropString(lpAttach, PR_DISPLAY_NAME, (void *)lpszDisplayName);
					
					if (FAILED(hr))
						goto cleanup;
					
					TCHAR	szShortFileName[MAX_PATH];
					
					GetShortPathName(lpszFullFileName, szShortFileName, MAX_PATH);
					
					hr = g_pMAPIEDK->pHrMAPISetPropString(lpAttach, PR_ATTACH_PATHNAME, szShortFileName);
					
					if (FAILED(hr))
						goto cleanup;
					
					hr = g_pMAPIEDK->pHrMAPISetPropString(lpAttach, PR_ATTACH_LONG_PATHNAME, (void*)lpszFullFileName);
					
					if (FAILED(hr))
						goto cleanup;
					
					hr = lpAttach->SaveChanges(0);
					
					if (FAILED(hr))
						goto cleanup;
					
				cleanup:
					if (FAILED(hr))
						erRes = NoAttach;
					
				}
				else
					erRes = NoMethod;
				
				if (lpAttach != NULL)
					ULRELEASE(lpAttach);
			}
			else
				erRes = NoAttach;
		}
		else
			erRes = NoMessage;
	}

	return erRes;
}

CSimpleMessage::CErrorReason CSimpleMessage::AddRecipients(LPADRLIST lpAdrList)
{
	CErrorReason	erRes			=	NoError;
	HRESULT			hr				=	NOERROR;

	hr = m_lpMessage->ModifyRecipients(MODRECIP_ADD, lpAdrList);

	if (SUCCEEDED(hr))
		m_bRecipIsSet = TRUE;
	else
		erRes = NoModRecip;

	return erRes;
}

CSimpleMessage::CErrorReason CSimpleMessage::AttachValue(LPSBinary lpsbValue, LPCTSTR lpszDisplayName)
{
	CErrorReason	erRes	=	NoError;
	HRESULT			hr		=	NOERROR;

	if (lpsbValue->cb == 0)
		erRes = NoAttach;
	else
	{
		if (m_lpMessage)
		{
			ULONG		ulAttachNum	=	0;
			LPATTACH	lpAttach	=	0;

			hr = m_lpMessage->CreateAttach(0, 0, &ulAttachNum, &lpAttach);
			
			if (SUCCEEDED(hr))
			{
				hr = g_pMAPIEDK->pHrMAPISetPropLong(lpAttach, PR_ATTACH_METHOD, ATTACH_BY_VALUE);

				if (SUCCEEDED(hr))
				{
					hr = g_pMAPIEDK->pHrMAPISetPropBinary(lpAttach, PR_ATTACH_DATA_BIN, lpsbValue->cb, lpsbValue->lpb);

					if (SUCCEEDED(hr))
					{
						hr = g_pMAPIEDK->pHrMAPISetPropString(lpAttach, PR_DISPLAY_NAME, (void*)lpszDisplayName);

						if (SUCCEEDED(hr))
						{
							hr = lpAttach->SaveChanges(0);

						}
					}
				}
			}

			if (FAILED(hr))
				erRes = NoAttach;

			if (lpAttach != NULL)
				ULRELEASE(lpAttach);
		}
		else
			erRes = NoMessage;
	}

	return erRes;
}

void CSimpleMessage::GetErrorMessage(IN CErrorReason reason, OUT LPTSTR message, IN int size)
{
	if ( size < 64 ) 
	{
		return;
	}
	
	switch(reason) 
	{
	case	NoError: 
			_tcscpy(message, _T("NoError\0")); 
			break;
	case	NoMAPISession:
			_tcscpy(message, _T("NoMAPISession\0")); 
			break;
	case	NoPrivateStore:
			_tcscpy(message, _T("NoPrivateStore\0")); 
			break;
	case	NoOutbox:
			_tcscpy(message, _T("NoOutbox\0")); 
			break;
	case	NoMessage:
			_tcscpy(message, _T("NoMessage\0")); 
			break;
	case	NoSearchKey:
			_tcscpy(message, _T("NoSearchKey\0")); 
			break;
	case	NoAddrList:
			_tcscpy(message, _T("NoAddrList\0")); 
			break;
	case	NoModRecip:
			_tcscpy(message, _T("NoModRecip\0")); 
			break;
	case	NoMessageBody:
			_tcscpy(message, _T("NoMessageBody\0")); 
			break;
	case	NoSubject:
			_tcscpy(message, _T("NoSubject\0")); 
			break;
	case	NoSave:
			_tcscpy(message, _T("NoSave\0")); 
			break;
	case	NotAllSet:
			_tcscpy(message, _T("NotAllSet\0")); 
			break;
	case	NoSubmit:
			_tcscpy(message, _T("NoSubmit\0")); 
			break;
	case	NoMemory:
			_tcscpy(message, _T("NoMemory\0")); 
			break;
	case	NoAddrBook:
			_tcscpy(message, _T("NoAddrBook\0")); 
			break;
	case	NoResolve:
			_tcscpy(message, _T("NoResolve\0")); 
			break;
	case	NoAttach:
			_tcscpy(message, _T("NoAttach\0")); 
			break;
	case	NotImplemented:
			_tcscpy(message, _T("NotImplemented\0")); 
			break;
	case	NoMethod:
			_tcscpy(message, _T("NoMethod\0")); 
			break;
	default:
		_tcscpy(message, _T("unknown\0")); 
	}
}