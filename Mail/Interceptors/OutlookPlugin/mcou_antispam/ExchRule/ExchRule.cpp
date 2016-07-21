#include <atlbase.h>
#include "../antispamousupportclass.h"
#include <tchar.h>
#include <string>
#include "exchrule.h"
#include "../trace.h"

using namespace std;
using namespace AntispamOUSupport;
#ifdef _UNICODE
typedef wstring tString;
#else
typedef string tString;
#endif

#define KIS2006_COMMAND			_T("Check for SPAM: KIS2006")
#define KIS2006_COMMAND_DESCR	_T("Check messages for spam")

class CGuiRoot;
extern CGuiRoot g_RootSink;
extern CComModule _Module;

/////////////////////////////////////////////////////////////////////////////
// implementation of the CExchRuleExt class
// Copyright (C) Microsoft Corp. 1986-1998.  All Rights Reserved.

extern "C"
LPEXCHANGERULEEXT CALLBACK ExchEntryPoint(void)
{
//	static CExchRuleExt *g_pCExchRuleExt = NULL;
//
//	if (g_pCExchRuleExt == NULL)
//		g_pCExchRuleExt = new CExchRuleExt();
//	else
//		g_pCExchRuleExt->AddRef ();
//
//	return g_pCExchRuleExt;
	return new CExchRuleExt();
};

CExchRuleExt::CExchRuleExt()
{
	m_cRef = 1;
}

CExchRuleExt::~CExchRuleExt()
{
}

STDMETHODIMP CExchRuleExt::QueryInterface(REFIID riid, LPVOID * ppvObj)
{
    *ppvObj = NULL;

    if ( IID_IUnknown == riid )
    {
        *ppvObj = (LPUNKNOWN)this;
        AddRef();
		return S_OK;
    }
    if ( IID_IExchangeRuleExt == riid )
    {
        *ppvObj = (LPUNKNOWN)this;
        AddRef();
		return S_OK;
    }
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CExchRuleExt::AddRef()
{
    ++m_cRef;
    return m_cRef;
}

STDMETHODIMP_(ULONG) CExchRuleExt::Release()
{
	// Use stack parameter, in case we delete ourselves
    ULONG ulCount = --m_cRef;
                         
	if (!ulCount) 
	{ 
		delete this;
	}

	return ulCount;
}

//TODO: “ут ничего делать не надо
STDMETHODIMP CExchRuleExt::Install (
    LPEXCHANGERULEEXTCALLBACK percb,    // exchange rule callback
    ULONG context)                      // ERCONTEXT_INFO or ERCONTEXT_EXEC
{
    return S_OK;	
}

//TODO: “ут надо освободить все ресурсы, кроме this (ну не бред ли? я это сделаю в деструкторе)
STDMETHODIMP CExchRuleExt::QueryRelease()
{
	return S_OK;
}

//TODO: “ут надо задать тип команды (pszCommand) и ее описание (pszDisplayName)
STDMETHODIMP CExchRuleExt::GetCommand (
	LPEXCHANGERULEEXTCALLBACK percb,    // exchange rule callback
	LPTSTR pszCommand,                  // an IN/OUT buffer in which to return
                                        // the encoded command, the buffer may
                                        // be pre-initialized with a previously
                                        // existing command for this provider.
    ULONG cchCommand,                   // length of the buffer given
	LPTSTR pszDisplayName,              // IN/OUT, as the previous buffer,
                                        // but for the display string
    ULONG cchDisplayName)               // length of the buffer given
{
	if ( 
		_tcslen(KIS2006_COMMAND)       >= cchCommand/sizeof(TCHAR)    ||
		_tcslen(KIS2006_COMMAND_DESCR) >= cchDisplayName/sizeof(TCHAR) 
		)
        return TYPE_E_BUFFERTOOSMALL;

	_tcscpy( pszCommand,     KIS2006_COMMAND       );
	_tcscpy( pszDisplayName, KIS2006_COMMAND_DESCR );
	
	if ( !g_pMAPIEDK )
		return SPAPI_E_ERROR_NOT_INSTALLED;

	CAntispamOUSupportClass* pAS = CAntispamOUSupportClass::GetInstance(_Module.GetModuleInstance());
	if ( pAS && IDOK == pAS->ShowPluginSettings(false, false) )
	{
		AntispamPluginSettings_t settings;
		pAS->GetSettings(&settings);
		settings.dwCheckAlgorithm = chal_UseRule;
		pAS->SetSettings(&settings);
	}
		

    return S_OK;
}

//TODO: “ут надо задать только описание команды (pszDisplayName) (зачем, если уже есть CExchRuleExt::GetCommand?)
STDMETHODIMP CExchRuleExt::QueryCommandDisplayName(
	LPEXCHANGERULEEXTCALLBACK percb,    // exchange rule callback
	LPCTSTR pszCommand,                 // command buffer
	LPTSTR pszDisplayName,              // OUT buffer for the display name
    ULONG cchDisplayName)               // size of buffer
{
	if ( _tcslen(KIS2006_COMMAND_DESCR) >= cchDisplayName/sizeof(TCHAR) )
        return TYPE_E_BUFFERTOOSMALL;
	_tcscpy( pszDisplayName, KIS2006_COMMAND_DESCR );

	return S_OK;
}

//$--Command------------------------------------------------------------------
//  Run the command.
// ---------------------------------------------------------------------------
STDMETHODIMP CExchRuleExt::Command(
	LPEXCHANGERULEEXTCALLBACK percb,    // exchange rule callback
	LPCTSTR pszCommand,                 // IN command to run
	ULONG cb, LPENTRYID peid)           // EntryID of the message
{
	HRESULT			hr = S_OK;
	LPMESSAGE		lpMsg = NULL;
	LPMAPIFOLDER	lpFolder = NULL;
	LPMAPITABLE		lpContents = NULL;
	LPENTRYLIST		lpMsgList = NULL;
	LPSRowSet		lpRows = NULL;
	ULONG			ulObjType = 0;
	ULONG			ulCount = 0;
	LPSPropValue	pMsgProps = NULL;
	ULONG			cbStore = 0;
	LPENTRYID		peidStore = NULL;
	ULONG			cbFolder = 0;
	LPENTRYID		peidFolder = NULL;
	LPMAPISESSION   lpSession = NULL;
    SizedSPropTagArray (2, sptaItems) =
    {
        2, PR_SUBJECT, PR_DISPLAY_NAME
    };
	enum{ NAME, ID, ADDRTYPE, EMAILADDRESS, NUM};
	SizedSPropTagArray(NUM, sptSender) = 
	{ 
		NUM,
		{ PR_SENT_REPRESENTING_NAME,
		  PR_SENT_REPRESENTING_ENTRYID,
		  PR_SENT_REPRESENTING_ADDRTYPE,
		  PR_SENT_REPRESENTING_EMAIL_ADDRESS}
	};
    if ((NULL == pszCommand) ||
        (NULL == peid))
    {
		PR_TRACE((0, prtIMPORTANT, "Invalid Parameter passed to Command"));
        return MAPI_E_INVALID_PARAMETER;
    }
	if ( !g_pMAPIEDK )
		return SPAPI_E_ERROR_NOT_INSTALLED;

	PR_TRACE((0, prtIMPORTANT, "Checking message via OU rule (command = '%s')", pszCommand));

	hr = percb->GetSession (&lpSession);
	if(FAILED(hr))
	{
		_TRACEHR(hr, "GetSession");
		goto cleanup;
	}
	hr = g_pMAPIEDK->pMAPIInitialize(0);
	if(FAILED(hr))
	{
		_TRACEHR(hr, "MAPIInitialize()");
		goto cleanup;
	}	
	hr = lpSession->OpenEntry(cb,
								(LPENTRYID)(peid),
								NULL,
								MAPI_BEST_ACCESS,
								&ulObjType,
								(LPUNKNOWN*)&lpMsg);
	if (FAILED(hr))
	{
		_TRACEHR(hr, "Open entry on message");
		goto cleanup;
	}

	{
		CAntispamOUSupportClass* pAS = CAntispamOUSupportClass::GetInstance(_Module.GetModuleInstance());
		if(pAS)
		{
			if(pAS->GetCheckAlgorithm() == chal_UseRule)
			{
				HRESULT hrActionResult = pAS->ActionOnMessage(lpMsg, AntispamOUSupport::Process);
				_TRACEHR(hrActionResult, "Processing message via OU rule");
			}
			else
				PR_TRACE((0, prtIMPORTANT, "Wrong algorithm"));
		}
		else
			PR_TRACE((0, prtERROR, "Failed to get instance"));
	}

cleanup:
	if ( pMsgProps ) 
		MAPIFREEBUFFER(pMsgProps);
	if(lpMsg) 
	{
		lpMsg->Release();
		lpMsg=NULL;
	}
	if(lpSession) 
	{
		lpSession->Release();
		lpSession=NULL;
	}
	g_pMAPIEDK->pMAPIUninitialize();

	return hr;
}