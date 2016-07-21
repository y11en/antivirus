#include <windows.h>
#define _PRIVATE_EDKDEBUG_H
#include <edk.h>

#include <Prague/prague.h>
#include <Mail/iface/i_mailtask.h>
#include <Mail/structs/s_mc_oe.h>

#define CONNECT_USE_SEPARATE_CONNECTION	4
//#define CONNECT_USE_SEPARATE_CONNECTION	0


#include "backgroundscan.h"
#include "../antispamousupportclass.h"
using namespace AntispamOUSupport;

#include "..\trace.h"

#define WAIT_FOR_EVENTSTOP(_timeout_, _out_hr_)			\
{	\
	DWORD dwWaitResult = WaitForSingleObject( m_hEventStop, _timeout_);	\
	if ( dwWaitResult == WAIT_OBJECT_0 )	\
	{	\
		PR_TRACE((0, prtIMPORTANT, "WAIT_FOR_EVENTSTOP: received m_hEventStop"));	\
		_out_hr_ = E_FAIL;	\
	}	\
}

bool FindSenderInSentMessages( LPMDB lpMDB, LPMESSAGE pMessage );
__inline bool FindSenderInWhiteList( LPMDB lpMDB, LPMESSAGE pMessage )
{
	return FindSenderInSentMessages( lpMDB, pMessage );
}

CBackgroundScan::CBackgroundScan(void* pAntispamOUSupport):
	m_pAntispamOUSupport(pAntispamOUSupport)
{
	m_hEventStop = ::CreateEvent(0, TRUE, FALSE, 0); // Этот евент не должен опускаться самостоятельно
	m_hEventScanNow = ::CreateEvent(0, FALSE, FALSE, 0);
	m_hEvents[0] = m_hEventScanNow;
	m_hEvents[1] = m_hEventStop;
}

CBackgroundScan::~CBackgroundScan()
{
	m_hEvents[0] = 0;
	m_hEvents[1] = 0;
	if (m_hEventScanNow) CloseHandle(m_hEventScanNow);
	if (m_hEventStop) CloseHandle(m_hEventStop);
}

int CBackgroundScan::Run()
{
	DWORD dwWaitResult = WAIT_TIMEOUT;

	HRESULT			hRes = S_OK;
	LPMAPISESSION	lpMAPISession = NULL;
	LPMDB			lpMDB = NULL;
	MAPIINIT_0      MapiInit = {0, 0};
	
	ZeroMemory(&MapiInit, sizeof(MAPIINIT_0));
	MapiInit.ulVersion = MAPI_INIT_VERSION;
    MapiInit.ulFlags = 0/* MAPI_MULTITHREAD_NOTIFICATIONS | MAPI_NT_SERVICE*/ ;
	
    hRes = g_pMAPIEDK->pMAPIInitialize(&MapiInit);
	_TRACEHR(hRes, "CBackgroundScan::Run: MAPIInitialize");
	if ( SUCCEEDED(hRes) )
	{
		HRESULT hLogonRes = S_OK;
		do 
		{
			hLogonRes = g_pMAPIEDK->pMAPILogonEx(
				(ULONG) 0,
				TEXT(""),
				TEXT(""),
				MAPI_EXTENDED,
				//		MAPI_NEW_SESSION | MAPI_EXTENDED | MAPI_NT_SERVICE | MAPI_EXPLICIT_PROFILE,
				&lpMAPISession);
			_TRACEHR(hRes, "CBackgroundScan::Run: MAPILogonEx");
		} 
		while( 
			FAILED(hLogonRes) &&
			(WAIT_TIMEOUT == (dwWaitResult = WaitForSingleObject( m_hEventStop, 3000))) 
			);
		
		if ( SUCCEEDED(hLogonRes) && (WAIT_TIMEOUT == dwWaitResult) )
		{
			CAntispamOUSupportClass* pAntispamOUSupport = (CAntispamOUSupportClass*)m_pAntispamOUSupport;
			do 
			{
				if ( 
					pAntispamOUSupport && 
					pAntispamOUSupport->IsAntispamEngineEnabled() &&
					pAntispamOUSupport->GetCheckAlgorithm() == chal_UseBackgroundScan
					)
				{
					HRESULT hrActionResult = S_OK;
					PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::Run: Starting scan of MAPISession"));
					hRes = Scan(lpMAPISession, &hrActionResult);
					_TRACEHR(hRes, "CBackgroundScan::Run: Scan of MAPISession");
				}
				dwWaitResult = WaitForMultipleObjects( 2, m_hEvents, FALSE, 60000); // Пересканируем ящики раз в минуту, не дожидаясь сигнала ScanNow
				if ( dwWaitResult == WAIT_OBJECT_0 + 1 )
				{
					PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::Run: stop event received"));
					break;
				}
			} 
			while( true );
		}
	}

	if ( lpMAPISession ) 
	{
		PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::Run: starting logoff..."));
		hRes = lpMAPISession->Logoff(0, 0, 0);
		_TRACEHR(hRes, "CBackgroundScan::Run: logoff");
		ULRELEASE(lpMAPISession);
	}
	PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::Run: calling MAPIUninitialize"));
	g_pMAPIEDK->pMAPIUninitialize();
	PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::Run: MAPIUninitialize done"));

	PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::Run: finished"));
	return 0;
}

void CBackgroundScan::Terminate()
{
	if (m_hEventStop) SetEvent(m_hEventStop);	
	CThread::Terminate();
}

BOOL IsDelegated(LPMDB lpMDB)
{
/*edkmdb.h*/
#ifndef pbExchangeProviderDelegateGuid
#define pbExchangeProviderDelegateGuid    "\x9e\xb4\x77\x00\x74\xe4\x11\xce\x8c\x5e\x00\xaa\x00\x42\x54\xe2"
#endif
	BOOL bEqual = FALSE;
    LPSPropValue pprop;
    HRESULT hr = g_pMAPIEDK->pHrGetOneProp( lpMDB, PR_MDB_PROVIDER, &pprop );
    if( SUCCEEDED(hr) ) 
	{
        const MAPIUID *uid = (const MAPIUID *)pprop->Value.bin.lpb;
        bEqual = IsEqualMAPIUID( uid, (const MAPIUID *)pbExchangeProviderDelegateGuid );
        MAPIFREEBUFFER(pprop);
    }
	return bEqual;
}

#define GETSAFETEXTPROP(p) ((p.ulPropTag & PROP_TYPE_MASK) == PT_ERROR ? (_T("<not available>")) : p.Value.LPSZ)

HRESULT CBackgroundScan::Scan(LPMAPISESSION lpMAPISession, HRESULT* phrActionResult)
{
	if ( !m_pAntispamOUSupport )
		return E_FAIL;
	if ( !lpMAPISession ) 
		return MAPI_E_NO_ACCESS;

	//PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::Scan(lpMAPISession): Enter..."));
	
	HRESULT hRes = S_OK;
	LPMAPITABLE lpMDBTable = NULL;
	LPSRowSet lpRowSet = NULL;
	LPMDB lpMDB = NULL;
	ULONG ulObjType = 0;
	ULONG ulLoop = 0;
//	SRestriction Restriction;
//	SPropValue restrValue;
//		Restriction.rt = RES_PROPERTY;
//		Restriction.res.resProperty.relop = RELOP_EQ;
//		Restriction.res.resProperty.ulPropTag = PR_DEFAULT_STORE;
//		restrValue.dwAlignPad = 0;
//		restrValue.ulPropTag = PR_DEFAULT_STORE;
//		restrValue.Value.b = TRUE;
//		Restriction.res.resProperty.lpProp = &restrValue;
	static SizedSPropTagArray(2, sptaMDBs) = 
	{
		2,
		{
			PR_DISPLAY_NAME,
			PR_ENTRYID,
		}
	};

	if ( SUCCEEDED(hRes) )
	{
		hRes = lpMAPISession->GetMsgStoresTable(0, &lpMDBTable);
		_TRACEHR(hRes, "CBackgroundScan::ScanMAPISession: GetMsgStoresTable");
	}
	if ( SUCCEEDED(hRes) )
	{
		hRes = g_pMAPIEDK->pHrQueryAllRows (
			lpMDBTable,
			reinterpret_cast<LPSPropTagArray>(&sptaMDBs),
			NULL, //&Restriction,
			NULL,
			0,
			&lpRowSet
			);
		_TRACEHR(hRes, "CBackgroundScan::ScanMAPISession: HrQueryAllRows");
	}
	if ( SUCCEEDED(hRes) )
	{
		if(lpRowSet->cRows == 0)
			PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanMAPISession: no rows to process"));
		else
			PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanMAPISession: processing %d rows", lpRowSet->cRows));
		for (ulLoop = 0; ulLoop < lpRowSet->cRows; ++ulLoop)
		{
			PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanMAPISession: processing row '%s'", GETSAFETEXTPROP(lpRowSet->aRow[ulLoop].lpProps[0])));
			hRes = lpMAPISession->OpenMsgStore(
				(ULONG)::GetActiveWindow(),
				lpRowSet->aRow[ulLoop].lpProps[1].Value.bin.cb,
				reinterpret_cast<LPENTRYID>(lpRowSet->aRow[ulLoop].lpProps[1].Value.bin.lpb),
				&IID_IMsgStore,
				MAPI_BEST_ACCESS | MDB_NO_MAIL,
				&lpMDB
				);
			_TRACEHR(hRes, "CBackgroundScan::ScanMAPISession: OpenMsgStore");
			CAntispamOUSupportClass* pAntispamOUSupport = (CAntispamOUSupportClass*)m_pAntispamOUSupport;
			if ( 
				pAntispamOUSupport && 
				pAntispamOUSupport->IsAntispamEngineEnabled() &&
				pAntispamOUSupport->GetCheckAlgorithm() == chal_UseBackgroundScan &&
				SUCCEEDED(hRes) &&
				!IsDelegated(lpMDB)
				)
			{
				PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanMAPISession: Starting scan of MsgStore"));
				hRes = Scan(lpMAPISession, lpMDB);
				_TRACEHR(hRes, "CBackgroundScan::ScanMAPISession: Scan of MsgStore");
			}
			ULRELEASE(lpMDB);
			WAIT_FOR_EVENTSTOP( 1000, hRes );
			if ( phrActionResult && FAILED(*phrActionResult) )
				break;
		}
	}

	ULRELEASE(lpMDBTable);
	FREEPROWS(lpRowSet);

	//PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::Scan(lpMAPISession): Leave..."));

	return hRes;
}

HRESULT CBackgroundScan::Scan(LPMAPISESSION lpMAPISession, LPMDB lpMDB, HRESULT* phrActionResult)
{
	if ( !m_pAntispamOUSupport )
		return E_FAIL;
	if ( !lpMDB ) 
		return MAPI_E_NO_ACCESS;

	//PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanMsgStore: Enter..."));

	HRESULT      hRes			= S_OK;
	LPSRowSet    lpRowSet       = NULL;
	LPENTRYID    lpMsgStoreID   = NULL;
	ULONG        cbMsgStoreID   = 0;
	LPMAPIFOLDER lpFolder       = NULL;
	LPSPropValue lpProp			= NULL;
	LPMAPITABLE  lpFolderTable	= NULL;
	ULONG        ulLoop			= 0;
	static SizedSPropTagArray(2, sptaFolderAttributes) = 
	{
		2,
		{
			PR_MESSAGE_CLASS,
			PR_ENTRYID,
		}
	};


	if ( SUCCEEDED(hRes) )
	{
		hRes = lpMDB->GetReceiveFolderTable(/*MAPI_DEFERRED_ERRORS*/0, &lpFolderTable);
		_TRACEHR(hRes, "CBackgroundScan::ScanMsgStore: GetReceiveFolderTable");
	}
	if ( SUCCEEDED(hRes) )
	{
		hRes = lpFolderTable->SetColumns((LPSPropTagArray)&sptaFolderAttributes, NULL);
		_TRACEHR(hRes, "CBackgroundScan::ScanMsgStore: SetColumns");
	}
	if ( SUCCEEDED(hRes) )
	{
		hRes = g_pMAPIEDK->pHrQueryAllRows( 
			lpFolderTable, 
			(LPSPropTagArray)&sptaFolderAttributes,
			NULL,
			NULL,
			0,
			&lpRowSet 
			);
		_TRACEHR(hRes, "CBackgroundScan::ScanMsgStore: HrQueryAllRows");
	}
	if ( SUCCEEDED(hRes) )
	{
		if(lpRowSet->cRows == 0)
			PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanMsgStore: no rows to process"));
		else
			PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanMsgStore: processing %d rows", lpRowSet->cRows));
		for(ulLoop = 0; ulLoop < lpRowSet->cRows; ++ulLoop)
		{
			PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanMsgStore: processing row '%s'", GETSAFETEXTPROP(lpRowSet->aRow[ulLoop].lpProps[0])));
			LPMAPIFOLDER	lpFolder	=	NULL;
			ULONG			ulObjType	=	0;
//			hRes = lpMDB->OpenEntry(
//				lpRowSet->aRow[ulLoop].lpProps[1].Value.bin.cb,
//				reinterpret_cast<LPENTRYID>(lpRowSet->aRow[ulLoop].lpProps[1].Value.bin.lpb),
//				&IID_IMAPIFolder,
//				MAPI_BEST_ACCESS,
//				&ulObjType,
//				reinterpret_cast<LPUNKNOWN *>(&lpFolder)
//				);
			ULONG cbEntryID = 0;;
			LPENTRYID pEntryID = NULL;
			LPTSTR pszExplicitClass = 0;            
			hRes = lpMDB->GetReceiveFolder(
				lpRowSet->aRow[ulLoop].lpProps[0].Value.LPSZ,
				0,
				&cbEntryID,
				&pEntryID,
				&pszExplicitClass
				);
			_TRACEHR(hRes, "CBackgroundScan::ScanMsgStore: GetReceiveFolder");
			if( SUCCEEDED(hRes) ) 
			{
				hRes = lpMDB->OpenEntry(
					cbEntryID, 
					pEntryID, 
					NULL, 
					MAPI_BEST_ACCESS, 
					&ulObjType, 
					reinterpret_cast<LPUNKNOWN *>(&lpFolder)
					);
				_TRACEHR(hRes, "CBackgroundScan::ScanMsgStore: OpenEntry");
			}
			
			CAntispamOUSupportClass* pAntispamOUSupport = (CAntispamOUSupportClass*)m_pAntispamOUSupport;
			if ( 
				pAntispamOUSupport && 
				pAntispamOUSupport->IsAntispamEngineEnabled() &&
				pAntispamOUSupport->GetCheckAlgorithm() == chal_UseBackgroundScan &&
				SUCCEEDED(hRes) 
				)
			{
				PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanMsgStore: Starting scan of folder"));
				hRes = Scan(lpMAPISession, lpMDB, lpFolder);
				_TRACEHR(hRes, "CBackgroundScan::ScanMsgStore: Scan of folder");
				ULRELEASE(lpFolder);
				WAIT_FOR_EVENTSTOP( 1000, hRes );
			}
			MAPIFREEBUFFER(pEntryID);
			MAPIFREEBUFFER(pszExplicitClass);
			if ( phrActionResult && FAILED(*phrActionResult) )
				break;
		}
	}
	
	ULRELEASE(lpFolderTable);
	FREEPROWS(lpRowSet);

	//PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanMsgStore: Leave..."));

	return hRes;
}

HRESULT CBackgroundScan::Scan(LPMAPISESSION lpMAPISession, LPMDB lpMDB, LPMAPIFOLDER lpFolder, bool bUnreadOnly, bool bUncheckedOnly, HRESULT* phrActionResult)
{
	if ( !m_pAntispamOUSupport )
		return E_FAIL;
	if ( !lpMDB || !lpFolder ) 
		return MAPI_E_NO_ACCESS;

	//PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanFolder: Enter..."));

	HRESULT      hRes			= S_OK;
	LPMAPITABLE  lpMessageTable = NULL;
	LPSRowSet    lpRowSet       = NULL;
	ULONG        ulLoop			= 0;
	#define PR_IMAP_DOWNLOADED	PROP_TAG( PT_BOOLEAN, 0x800E)
	SRestriction Restriction, RestrictionAnd[2], RestrictionNot;
	SRestriction* pRestrictionToApply = NULL;
	static SizedSPropTagArray(3, sptaMessages) = 
	{
		3,
		{
			PR_ENTRYID,
			PR_IMAP_DOWNLOADED,
			PR_SUBJECT
		}
	};
	if ( bUnreadOnly )
	{
		RestrictionAnd[0].rt = RES_BITMASK;
		RestrictionAnd[0].res.resBitMask.relBMR = BMR_EQZ;
		RestrictionAnd[0].res.resBitMask.ulPropTag = PR_MESSAGE_FLAGS;
		RestrictionAnd[0].res.resBitMask.ulMask = MSGFLAG_READ;
		pRestrictionToApply = &RestrictionAnd[0];
	}
	if ( bUncheckedOnly )
	{
		RestrictionNot.rt = RES_EXIST;
		RestrictionNot.res.resExist.ulReserved1 = 0;
		RestrictionNot.res.resExist.ulReserved1 = 0;
		RestrictionNot.res.resExist.ulPropTag = PR_MESSAGE_AS_PROCESSED;
		RestrictionAnd[1].rt = RES_NOT;
		RestrictionAnd[1].res.resNot.ulReserved = 0;
		RestrictionAnd[1].res.resNot.lpRes = &RestrictionNot;
		pRestrictionToApply = &RestrictionAnd[1];
	}
	if ( bUnreadOnly && bUncheckedOnly ) 
	{
		Restriction.rt = RES_AND;
		Restriction.res.resAnd.cRes = 2;
		Restriction.res.resAnd.lpRes = RestrictionAnd;
		pRestrictionToApply = &Restriction;
	}

	hRes = lpFolder->GetContentsTable(
			MAPI_DEFERRED_ERRORS, 
			&lpMessageTable);
	_TRACEHR(hRes, "CBackgroundScan::ScanFolder: GetContentsTable");
	if ( SUCCEEDED(hRes) )
	{
		hRes = g_pMAPIEDK->pHrQueryAllRows (
			lpMessageTable,
			reinterpret_cast<LPSPropTagArray>(&sptaMessages),
			pRestrictionToApply,
			NULL,
			0,
			&lpRowSet
			);
		_TRACEHR(hRes, "CBackgroundScan::ScanFolder: HrQueryAllRows");
	}
	if ( SUCCEEDED(hRes) )
	{
		if(lpRowSet->cRows == 0)
			PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanFolder: no rows to process"));
		else
			PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanFolder: processing %d rows", lpRowSet->cRows));
		for (ulLoop = 0; ulLoop < lpRowSet->cRows; ++ulLoop)
		{
			PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanMsgStore: processing row '%s'", GETSAFETEXTPROP(lpRowSet->aRow[ulLoop].lpProps[2])));
			bool bMessageRead = false;
			LPMESSAGE pMessage = NULL;
			ULONG ulObjType = 0;

			// Проверить PR_IMAP_DOWNLOADED. Чекать только если он не существует или содержит не 0.
			if ( ( (lpRowSet->aRow[ulLoop].lpProps[1].ulPropTag & PROP_TYPE_MASK) == PT_ERROR ) )
				PR_TRACE((0, prtIMPORTANT, "BackgroundScan::ScanFolder: PR_IMAP_DOWNLOADED is unavailable"));
			else if ( lpRowSet->aRow[ulLoop].lpProps[1].Value.ul == 0 )
				PR_TRACE((0, prtIMPORTANT, "BackgroundScan::ScanFolder: PR_IMAP_DOWNLOADED = 0"));
			else
			{
				PR_TRACE((0, prtIMPORTANT, "BackgroundScan::ScanFolder: PR_IMAP_DOWNLOADED = 0x%X", lpRowSet->aRow[ulLoop].lpProps[1].Value.ul));
				PR_TRACE((0, prtIMPORTANT, "BackgroundScan::ScanFolder: PR_IMAP_DOWNLOADED ulPropTag = 0x%X", lpRowSet->aRow[ulLoop].lpProps[1].ulPropTag));
			}
			CAntispamOUSupportClass* pAntispamOUSupport = (CAntispamOUSupportClass*)m_pAntispamOUSupport;
			if ( 
				pAntispamOUSupport && 
				pAntispamOUSupport->IsAntispamEngineEnabled() &&
				pAntispamOUSupport->GetCheckAlgorithm() == chal_UseBackgroundScan &&
				( 
					( (lpRowSet->aRow[ulLoop].lpProps[1].ulPropTag & PROP_TYPE_MASK) == PT_ERROR ) ||
					( lpRowSet->aRow[ulLoop].lpProps[1].Value.b != FALSE )
				))
			{
				hRes = lpFolder->OpenEntry(
					lpRowSet->aRow[ulLoop].lpProps[0].Value.bin.cb,
					reinterpret_cast<LPENTRYID>(lpRowSet->aRow[ulLoop].lpProps[0].Value.bin.lpb),
					&IID_IMessage,
					MAPI_BEST_ACCESS,
					&ulObjType,
					reinterpret_cast<LPUNKNOWN *>(&pMessage)
					);
				_TRACEHR(hRes, "CBackgroundScan::ScanFolder: OpenEntry(Message)");
				if ( SUCCEEDED(hRes) )
				{
					// Проверим, не снялся ли флаг "Не прочитано" с письма, пока мы до него шли
					HRESULT hrReadFlag = S_OK;
					ULONG ulMessageFlags = 0;
					hrReadFlag = g_pMAPIEDK->pHrMAPIGetPropLong(pMessage, PR_MESSAGE_FLAGS, &ulMessageFlags);
					if(SUCCEEDED(hrReadFlag))
					{
						if(ulMessageFlags & MSGFLAG_READ)
						{
							PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanFolder: the message is already read"));
							bMessageRead = true;
							if ( phrActionResult ) 
							{
								*phrActionResult = E_FAIL; // Выходим из цикла
								ScanNow();                 // Чтобы вернуться ;)
							}
						}
					}
					else
						_TRACEHR(hrReadFlag, "CBackgroundScan::ScanFolder: Failed to get message flags");
				}

				if ( SUCCEEDED(hRes) && !bMessageRead )
				{
					PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanFolder: Calling ActionOnMessage..."));
					HRESULT hrActionResult = ((CAntispamOUSupportClass*)m_pAntispamOUSupport)->ActionOnMessage( 
						pMessage, 
						AntispamOUSupport::Process 
						);
					_TRACEHR(hrActionResult, "CBackgroundScan::ScanFolder: ActionOnMessage");
					if ( phrActionResult ) *phrActionResult = hrActionResult;
				}
				else
				{
					PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanFolder: the read message skipped"));
				}
				ULRELEASE(pMessage);
			}
			WAIT_FOR_EVENTSTOP( 1000, hRes );
			if ( phrActionResult && FAILED(*phrActionResult) )
				break;
		}
	}

	ULRELEASE(lpMessageTable);
	FREEPROWS(lpRowSet);

	//PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanFolder: Leave..."));

	return hRes;
}

bool CBackgroundScan::ScanNow()
{
	if (m_hEventScanNow) 
	{
		SetEvent(m_hEventScanNow);
		PR_TRACE((0, prtIMPORTANT, "CBackgroundScan::ScanNow: SetEvent..."));
		return true;
	}
	return false;
}

bool FindSenderInSentMessages( LPMDB lpMDB, LPMESSAGE pMessage )
{
	bool bResult = false;
	HRESULT hr = S_OK;
	// Определить отправителя в pMessage (PR_SENDER_EMAIL_ADDRESS, PR_SENT_REPRESENTING_EMAIL_ADDRESS)
	LPTSTR szSenderAddress = 0;
	LPTSTR szSentRepresentingAddress = 0;
	ULONG ulSize = 0;

	hr = g_pMAPIEDK->pHrMAPIGetPropString( 
		pMessage, 
		PR_SENDER_EMAIL_ADDRESS, 
		&ulSize, 
		(LPVOID*)&szSenderAddress 
		);
	hr = g_pMAPIEDK->pHrMAPIGetPropString( 
		pMessage, 
		PR_SENT_REPRESENTING_EMAIL_ADDRESS, 
		&ulSize, 
		(LPVOID*)&szSentRepresentingAddress 
		);
	// Определить папку SentItems
	if ( szSenderAddress || szSentRepresentingAddress )
	{
		LPMAPIFOLDER pSentItemsFolder = NULL;
		hr = g_pMAPIEDK->pHrMAPIOpenFolderEx(
			lpMDB, 
			'\\', 
			_T("@PR_IPM_SENTMAIL_ENTRYID"), 
			&pSentItemsFolder
			); 
		if ( SUCCEEDED(hr) && pSentItemsFolder )
		{
			// Найти хотя бы одно письмо в папке SentItems с получателем = отправителю pMessage
			SRestriction Restriction, SubRestriction, RestrictionOr[2];
			SPropValue restrValue_SenderAddress;
				restrValue_SenderAddress.dwAlignPad = 0;
				restrValue_SenderAddress.ulPropTag = PR_EMAIL_ADDRESS;
				restrValue_SenderAddress.Value.LPSZ = szSenderAddress;
			SPropValue restrValue_SentRepresentingAddress;
				restrValue_SentRepresentingAddress.dwAlignPad = 0;
				restrValue_SentRepresentingAddress.ulPropTag = PR_EMAIL_ADDRESS;
				restrValue_SentRepresentingAddress.Value.LPSZ = szSentRepresentingAddress;
			static SizedSPropTagArray(2, sptaMessages) = 
			{
				2,
				{
					PR_SUBJECT,
					PR_ENTRYID,
				}
			};
			LPSRowSet lpRowSet = NULL;
			ULONG ulLoop = 0;
			LPMAPITABLE  lpMessageTable = NULL;

			RestrictionOr[0].rt = RES_CONTENT;
			RestrictionOr[0].res.resContent.ulFuzzyLevel = FL_FULLSTRING | FL_LOOSE ;
			RestrictionOr[0].res.resContent.ulPropTag = PR_EMAIL_ADDRESS;
			RestrictionOr[0].res.resContent.lpProp = &restrValue_SenderAddress;
			RestrictionOr[1].rt = RES_CONTENT;
			RestrictionOr[1].res.resContent.ulFuzzyLevel = FL_FULLSTRING | FL_LOOSE ;
			RestrictionOr[1].res.resContent.ulPropTag = PR_EMAIL_ADDRESS;
			RestrictionOr[1].res.resContent.lpProp = &restrValue_SentRepresentingAddress;
			
			SubRestriction.rt = RES_OR;
			SubRestriction.res.resOr.cRes = 2;
			SubRestriction.res.resOr.lpRes = RestrictionOr;
			
			Restriction.rt = RES_SUBRESTRICTION;
			Restriction.res.resSub.ulSubObject = PR_MESSAGE_RECIPIENTS; 
			Restriction.res.resSub.lpRes = &RestrictionOr[0]; //&SubRestriction; 
			
			hr = pSentItemsFolder->GetContentsTable(
					MAPI_DEFERRED_ERRORS, 
					&lpMessageTable);
			if ( SUCCEEDED(hr) )
				hr = g_pMAPIEDK->pHrQueryAllRows (
					lpMessageTable,
					reinterpret_cast<LPSPropTagArray>(&sptaMessages),
					&Restriction,
					NULL,
					0,
					&lpRowSet
					);
			if ( SUCCEEDED(hr) && lpRowSet && lpRowSet->cRows )
				bResult = true;
			ULRELEASE(lpMessageTable);
			FREEPROWS(lpRowSet);
			ULRELEASE(pSentItemsFolder);
		}
	}

	MAPIFREEBUFFER(szSenderAddress);
	MAPIFREEBUFFER(szSentRepresentingAddress);
	return bResult;
}