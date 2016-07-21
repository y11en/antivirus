#include "../stdafx.h"
#include <string>
#include "../antispamousupportclass.h"
#include "SimpleMessage/SimpleMessage.h"

#if defined(UNICODE) || defined(_UNICODE)
#define std_string	std::wstring
#define itoa		_itow
#define CCP			cCP_UNICODE
#else
#define std_string	std::string
#define CCP			cCP_ANSI
#endif

using namespace AntispamOUSupport;

#define REQUEST_BODY_BEGIN	_T("Извините, но Ваше сообщение было ошибочно определено как спам. Пришлите, пожалуйста, подтверждение того, что Вы не рассылаете спам (просто ответьте на это письмо c адреса \"")
#define REQUEST_BODY_END	_T("\", включив исходный текст и тему письма).")
#define THANKS_SUBJECT		_T("Re: Ваше сообщение было ошибочно определено как спам")
#define THANKS_BODY			_T("Спасибо за ответ. Теперь я знаю, что Вы - не спамер.")

__inline bool EntryID2Text( IN EntryID_t* entryId, OUT std_string& szEntryID)
{
	if ( !entryId )
		return false;
	TCHAR buf[10];
	tULONG i = 0;
	for ( i = 0; i < entryId->cb; i++ )
		szEntryID += 
			_itoa( 
				(int)(((tBYTE*)(entryId->get_lpb()))[i]), 
				buf, 
				16 
				);
	return true;
}

HRESULT CAntispamOUSupportClass::SendMessageRequestEx( LPMESSAGE pMessage, bool bIsThanks )
{
	HRESULT hr = S_OK;
	CSimpleMessage smMessage;
	CSimpleMessage::CErrorReason err = CSimpleMessage::NoError;
	
	if (err == CSimpleMessage::NoError)
	{
		err = smMessage.Create(m_pMAPISession);
		if (err == CSimpleMessage::NoError)
		{
		    LPSPropValue pPropEntryID = NULL;
			std_string szEntryID;
			LPTSTR szSenderAddress = 0;
			LPTSTR szSenderAddrType = 0;
			LPTSTR szSentRepresentingAddress = 0;
			LPTSTR szSentRepresentingAddrType = 0;
			LPTSTR szSubject = 0;
			ULONG ulSize = 0;

			hr = g_pMAPIEDK->pHrMAPIGetPropString( 
				pMessage, 
				PR_SENDER_EMAIL_ADDRESS, 
				&ulSize, 
				(LPVOID*)&szSenderAddress 
				);
			if ( szSenderAddress )
				hr = g_pMAPIEDK->pHrMAPIGetPropString( 
					pMessage, 
					PR_SENDER_ADDRTYPE, 
					&ulSize, 
					(LPVOID*)&szSenderAddrType 
					);
			hr = g_pMAPIEDK->pHrMAPIGetPropString( 
				pMessage, 
				PR_SENT_REPRESENTING_EMAIL_ADDRESS, 
				&ulSize, 
				(LPVOID*)&szSentRepresentingAddress 
				);
			if ( szSentRepresentingAddress )
				hr = g_pMAPIEDK->pHrMAPIGetPropString( 
					pMessage, 
					PR_SENT_REPRESENTING_ADDRTYPE, 
					&ulSize, 
					(LPVOID*)&szSentRepresentingAddrType 
					);
			hr = g_pMAPIEDK->pHrMAPIGetPropString( 
				pMessage, 
				PR_SUBJECT, 
				&ulSize, 
				(LPVOID*)&szSubject 
				);
			if ( SUCCEEDED(hr) )
				HRESULT hr = g_pMAPIEDK->pHrGetOneProp(pMessage, PR_ENTRYID, &pPropEntryID);
			if( SUCCEEDED(hr) && pPropEntryID ) 
			{
				EntryID_t entryId; entryId = pPropEntryID->Value.bin;
				if ( !EntryID2Text(&entryId, szEntryID) )
					hr = E_FAIL;
			}
			if ( szSenderAddress )
				err = smMessage.AddRecipient(
						_T(""), 
						szSenderAddrType, 
						szSenderAddress, 
						CSimpleMessage::TOField
						);
			if ( 
				szSentRepresentingAddress && 
				(szSenderAddress != szSentRepresentingAddress) 
				)
				err = smMessage.AddRecipient(
						_T(""), 
						szSentRepresentingAddrType, 
						szSentRepresentingAddress, 
						szSenderAddress ? CSimpleMessage::CCField : CSimpleMessage::TOField
						);
			if ( SUCCEEDED(hr) && (szSenderAddress || szSentRepresentingAddress) )
			{
				std_string szNewSubject = "Re: ";
				szNewSubject += szSubject;
				if ( !bIsThanks )
				{
					szNewSubject += " [code: ";
					szNewSubject += szEntryID;
					szNewSubject += "]";
					std_string szBody = REQUEST_BODY_BEGIN;
					szBody += szSenderAddress;
					szBody += REQUEST_BODY_END;
					if (err == CSimpleMessage::NoError)
						err = smMessage.SetBody( szBody.c_str() );
				}
				else
				{
					if (err == CSimpleMessage::NoError)
						err = smMessage.SetBody( THANKS_BODY );
				}
				if (err == CSimpleMessage::NoError)
					err = smMessage.SetSubject( szNewSubject.c_str() );
				if (err == CSimpleMessage::NoError)
					err = smMessage.Send( !bIsThanks );
				if (err != CSimpleMessage::NoError)
					hr = E_FAIL;
			}
			if ( SUCCEEDED(hr) && !bIsThanks )
			{
				// Добавляем запись в RequestList
				Request_t request;
				request.szCode = szEntryID.c_str();
				DTDateShift(&request.DateToForget, 0,0,m_pAntispamPluginSettings->dwRequestStoreDaysCount);
				m_pAntispamPluginSettings->RequestList.push_back(request);
			}

			MAPIFREEBUFFER(pPropEntryID);
			MAPIFREEBUFFER(szSubject);
			MAPIFREEBUFFER(szSenderAddress);
			MAPIFREEBUFFER(szSenderAddrType);
		}
	}
	smMessage.Destroy();

	return hr;
}

bool CAntispamOUSupportClass::IsMessageReRequest( LPMESSAGE pMessage, bool bRemoveFound )
{
	bool bResult = false;
	HRESULT hr = S_OK;
	LPTSTR szSubject = 0;
	ULONG ulSize = 0;

	if ( SUCCEEDED(hr) )
		hr = g_pMAPIEDK->pHrMAPIGetPropString( 
			pMessage, 
			PR_SUBJECT, 
			&ulSize, 
			(LPVOID*)&szSubject 
			);
	if ( SUCCEEDED(hr) )
	{
		tULONG ulRequestListCounter = 0;
		tULONG ulRequestListCount = m_pAntispamPluginSettings->RequestList.count();
		for ( 
			ulRequestListCounter = 0; 
			ulRequestListCounter < ulRequestListCount; 
			ulRequestListCounter++ 
			)
			{
				if ( 
					strstr(
						szSubject, 
						m_pAntispamPluginSettings->RequestList.at(ulRequestListCounter).szCode.c_str(CCP)
						)
					)
				{
					if ( bRemoveFound )
						m_pAntispamPluginSettings->RequestList.remove(ulRequestListCounter);
					bResult = true;
					break;
				}
			}
	}

	MAPIFREEBUFFER(szSubject);
	return bResult;
}

HRESULT CAntispamOUSupportClass::SendMessageRequest( LPMESSAGE pMessage )
{
	return SendMessageRequestEx(pMessage, false);
}
HRESULT CAntispamOUSupportClass::SendMessageThanks( LPMESSAGE pMessage )
{
	return SendMessageRequestEx(pMessage, true);
}

HRESULT CAntispamOUSupportClass::RemoveMessages( LPMESSAGE pMessage )
{
	// Найти и вернуть в оригинальные папки все письма от отправителя pMessage
	HRESULT hr = S_OK;
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
	if ( szSenderAddress || szSentRepresentingAddress )
	{
		LPMAPIFOLDER pSpamFolder = NULL;
		ULONG ulObjType = 0;
		HRESULT hr = m_pMAPISession->OpenEntry(
			m_pAntispamPluginSettings->CertainSpamRule.FolderForAction.EntryID.cb,
			reinterpret_cast<LPENTRYID>(m_pAntispamPluginSettings->CertainSpamRule.FolderForAction.EntryID.get_lpb()),
			&IID_IMAPIFolder,
			MAPI_MODIFY,
			&ulObjType,
			reinterpret_cast<LPUNKNOWN *>(&pSpamFolder)
			);
		if ( SUCCEEDED(hr) && pSpamFolder )
		{
			// Найти все письма в папке со спамом от отправителя pMessage
			SRestriction Restriction, RestrictionOr[2];
			SPropValue restrValue_SenderAddress;
				restrValue_SenderAddress.dwAlignPad = 0;
				restrValue_SenderAddress.ulPropTag = PR_SENDER_EMAIL_ADDRESS;
				restrValue_SenderAddress.Value.LPSZ = szSenderAddress;
			SPropValue restrValue_SentRepresentingAddress;
				restrValue_SentRepresentingAddress.dwAlignPad = 0;
				restrValue_SentRepresentingAddress.ulPropTag = PR_SENT_REPRESENTING_EMAIL_ADDRESS;
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
			RestrictionOr[0].res.resContent.ulPropTag = PR_SENDER_EMAIL_ADDRESS;
			RestrictionOr[0].res.resContent.lpProp = &restrValue_SenderAddress;
			RestrictionOr[1].rt = RES_CONTENT;
			RestrictionOr[1].res.resContent.ulFuzzyLevel = FL_FULLSTRING | FL_LOOSE ;
			RestrictionOr[1].res.resContent.ulPropTag = PR_SENT_REPRESENTING_EMAIL_ADDRESS;
			RestrictionOr[1].res.resContent.lpProp = &restrValue_SentRepresentingAddress;
			
			Restriction.rt = RES_OR;
			Restriction.res.resOr.cRes = 2;
			Restriction.res.resOr.lpRes = RestrictionOr;
			
			hr = pSpamFolder->GetContentsTable(
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
			{
				for ( ulLoop = 0; ulLoop < lpRowSet->cRows; ulLoop++ )
				{
					LPMESSAGE pSpamMessage = NULL;
					ULONG ulObjType = 0;
					
					hr = pSpamFolder->OpenEntry(
						lpRowSet->aRow[ulLoop].lpProps[1].Value.bin.cb,
						reinterpret_cast<LPENTRYID>(lpRowSet->aRow[ulLoop].lpProps[1].Value.bin.lpb),
						&IID_IMessage,
						MAPI_BEST_ACCESS,
						&ulObjType,
						reinterpret_cast<LPUNKNOWN *>(&pSpamMessage)
						);
					if ( SUCCEEDED(hr) )
						ActionOnMessage ( pSpamMessage, RemoveFromSpam );
					ULRELEASE(pSpamMessage);
				}
			}
			ULRELEASE(lpMessageTable);
			FREEPROWS(lpRowSet);
			ULRELEASE(pSpamFolder);
		}
	}

	MAPIFREEBUFFER(szSenderAddress);
	MAPIFREEBUFFER(szSentRepresentingAddress);

	return hr;
}

