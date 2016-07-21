#include "stdafx.h"

#include <Mail/iface/i_mailtask.h>
#include <Mail/structs/s_mc.h>

#include "mcou.h"
#include "util.h"
#include "initterm.h"
#include "OUMessage.h"
#include "MCOUAsync.h"

BOOL MCOU::ProcessMessage(LPEXCHEXTCALLBACK lpeecb, BOOL bIsIncoming, BOOL bAsync, HRESULT &hrReturn)
{
	if ( !lpeecb )
		return FALSE;

	COUMessage Message;
	HRESULT hr = Message.CreateFromExchExtCallback(lpeecb);
	if(FAILED(hr))
	{
		PR_TRACE((0, prtIMPORTANT, "Creating message object from callback failed (0x%08x)", hr));
		return FALSE;
	}

	PR_TRACE((0, prtIMPORTANT, "Message from IExchExtCallback created"));

	// попробуем снять property, чтобы отсечь NTE_FAIL и т.п. (см. bug 22512)
	if(!bAsync && Message.CheckTestProperty())
	{
		PR_TRACE((0, prtIMPORTANT, "Failed to get test property, message processing skipped"));
		hrReturn = E_FAIL;
		return FALSE;
	}

	LPENTRYID lpEntryId = NULL;
	ULONG ulEntryIdSize = 0;
	Message.GetEntryId(lpEntryId, ulEntryIdSize);

	LPMDB lpIMsgStore = NULL;
	if(bAsync && 
		AddMessageToAsyncThread(lpEntryId,
			ulEntryIdSize,
			lpIMsgStore = Message.GetIMsgStore(),
			bIsIncoming != FALSE,
			Message.GetOutlookHWND()))
	{
		if(lpIMsgStore)
			lpIMsgStore->Release();
		PR_TRACE((0, prtIMPORTANT, "Message checking queued"));
		return TRUE;
	}
	if(lpIMsgStore)
		lpIMsgStore->Release();

	MCOU::CWaitCur wc;

	// process message
	PR_TRACE((0, prtIMPORTANT, "Checking message..."));
	if(!Message.Process(bIsIncoming))
	{
		PR_TRACE((0, prtIMPORTANT, "Failed to check message"));
		return FALSE;
	}
	PR_TRACE((0, prtIMPORTANT, "Message checked"));

	return TRUE;
}

DWORD MCOU::GetThisModuleFileName ( LPTSTR lpFilename, DWORD nSize )
{
	return GetModuleFileName (MCOU::g_hInstance, lpFilename, nSize);
}

ULONG MCOU::GetStoreSupportMask ( LPMESSAGE lpMessage )
{
	if ( lpMessage == NULL )
		return 0;

	ULONG ulRetVal = 0;

	LPSPropValue pPropSupportMask = NULL;
	HRESULT hr2 = g_pMAPIEDK->pHrGetOneProp (lpMessage, PR_STORE_SUPPORT_MASK, &pPropSupportMask);
	if ( FAILED(hr2) )
	{
		PR_TRACE((0, prtIMPORTANT, "mcou\tError retrieving store support mask (hr = 0x%08x)", hr2));
	}
	ulRetVal = pPropSupportMask[0].Value.ul;
	g_pMAPIEDK->pMAPIFreeBuffer (pPropSupportMask);

	return ulRetVal;
}



MCOU::MCState MCOU::CheckMCState ()
{
	PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tChecking MC state..."));
	MCOU::MCState mcRet = mcsNoRTP;

	// Проверим наличие связи с BL и MBL
	hOBJECT bl = GetBL();
	if ( bl )
	{
		mcRet = mcsOPDisabled;
		cERROR err = bl->sysSendMsg( pmc_MAILCHECKER_PROCESS, NULL, NULL, NULL, NULL);
		if ( err == errOK_DECIDED )
			mcRet = mcsOPEnabledDuplex;
	}
	

	return mcRet;
}



//! \fn				: DeleteTempFilesPrefix
//! \brief			: Удаляет из временной папки файлы по маске	
//! \return			: bool 
//! \param          : LPTSTR lpszPrefix
bool MCOU::DeleteTempFilesPrefix(LPCTSTR lpszPrefix, LPCTSTR lpszExtension)
{
		TCHAR szTempMask[ _MAX_PATH + 2 ] = _T(""),
			  szTempPath[ _MAX_PATH + 2 ] = _T("");

		WIN32_FIND_DATA FileData; 	

		long lLen = GetTempPath(_MAX_PATH, szTempPath);

		if( lLen <= 0 )					
			return false;		

		if( szTempPath[ lLen - 1 ] != _T('\\') )
			_tcscat(szTempPath, _T("\\") );

		if ( !lpszExtension )
			lpszExtension = _T("tmp");

		_sntprintf(szTempMask, _MAX_PATH, _T("%s%s*.%s"), szTempPath, lpszPrefix, lpszExtension);

		HANDLE hSearch = FindFirstFile(szTempMask, &FileData); 

		if( hSearch == NULL || hSearch == INVALID_HANDLE_VALUE )
			return true;

		do
		{
			DWORD dwAttrs = FileData.dwFileAttributes; 

			if( !(dwAttrs & FILE_ATTRIBUTE_DIRECTORY ||
				  FileData.cFileName[0] == _T('.')) )
			{
				TCHAR szFullName[ _MAX_PATH + 2 ];
				_sntprintf(szFullName, _MAX_PATH, _T("%s%s"), szTempPath, FileData.cFileName);

				if( dwAttrs & FILE_ATTRIBUTE_READONLY )         
					SetFileAttributes(szFullName, dwAttrs & ~FILE_ATTRIBUTE_READONLY);         

				_tremove(szFullName);
			}
		} while( FindNextFile(hSearch, &FileData) ); 

		return true;
}

