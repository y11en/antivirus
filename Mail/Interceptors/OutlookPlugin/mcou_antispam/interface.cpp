/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	interface.cpp
*		\brief	IExchExt interface implementation
*		
*		\author Nick Lebedev
*		\date	19.06.2003 15:23:46
*		
*		Example:	
*		
*		
*		
*/		

#include "stdafx.h"
#include <mapidefs.h>
#include <mapiform.h>
#include "initguid.h"

#undef MAPIGUID_H
//#define USES_PS_PUBLIC_STRINGS
//#define USES_IID_IMessage
#define USES_IID_IStreamDocfile
#include <mapiguid.h>

#include "interface.h"
//#include "initterm.h"

IMCOUExchExt::IMCOUExchExt()
{
	m_cRef = 1;
    m_context = 0;

	m_pExchExtMessageEvents = new IMCOUExchExtMessageEvents (this);
	m_pExchExtSessionEvents = new IMCOUExchExtSessionEvents (this);

//	MCOU::g_bInitSucceeded = InitEnvironment ();
//	if ( !MCOU::g_bInitSucceeded )
	{
//		KLSTD_TRACE0 ( MCOU_TL_CRITICAL, L"Initializing environment failed\n" );
	}

//	KLSTD_TRACE1 ( MCOU_TL_INFO, L"IMCOUExchExt::IMCOUExchExt (RefCount = %d)\n", m_cRef);
}

//////////////////////////////////////////////////////////////////////////
//
// IUnknown implementation
//

ULONG IMCOUExchExt::AddRef ()
{
	++m_cRef;

//	KLSTD_TRACE1 ( MCOU_TL_SPAM, L"IMCOUExchExt::AddRef (RefCount = %d)\n", m_cRef);
	
	return m_cRef;
}

STDMETHODIMP_(ULONG) IMCOUExchExt::Release ()
{ 
	ULONG ulCount = --m_cRef;

//	KLSTD_TRACE1 ( MCOU_TL_SPAM, L"IMCOUExchExt::Release (RefCount = %d)\n", ulCount);

	if (!ulCount) 
	{
//		KLSTD_TRACE0 ( MCOU_TL_INFO, L"deleting IMCOUExchExt instance\n");

//		TermEnvironment ();
		delete this;
	}

	return ulCount;
}

STDMETHODIMP IMCOUExchExt::QueryInterface (REFIID riid, LPVOID * ppvObj)
{
#ifdef _DEBUG
	LPOLESTR _ps;
	StringFromIID ( riid, &_ps );
//	KLSTD_TRACE1 ( MCOU_TL_SPAM, L"IMCOUExchExt::QueryInterface %s ", _ps );
	CoTaskMemFree ( _ps );
#endif

    HRESULT hResult = S_OK;

    *ppvObj = NULL;

    if (( IID_IUnknown == riid) || ( IID_IExchExt == riid ) )
    {
//		KLSTD_TRACE0 ( MCOU_TL_SPAM, L"IID_IUnknown or IID_IExchExt\n" );
        *ppvObj = (LPUNKNOWN)this;
    }
    else if (IID_IExchExtPropertySheets == riid )
    {
		// if we are in the read or send context, do not return
		// propertysheet interface
        if ( (m_context == EECONTEXT_SENDNOTEMESSAGE)   ||
             (m_context == EECONTEXT_SENDPOSTMESSAGE)   ||
             (m_context == EECONTEXT_SENDRESENDMESSAGE) ||
             (m_context == EECONTEXT_READNOTEMESSAGE)   ||
             (m_context == EECONTEXT_READPOSTMESSAGE)   ||
             (m_context == EECONTEXT_READREPORTMESSAGE) )
		{
//			KLSTD_TRACE0 ( MCOU_TL_SPAM, L"IID_IExchExtPropertySheets - E_NOINTERFACE\n" );
			return E_NOINTERFACE;
		}

//		KLSTD_TRACE0 ( MCOU_TL_SPAM, L"IID_IExchExtPropertySheets - S_OK\n" );
		// otherwise return the interface
//      *ppvObj = (LPUNKNOWN) m_pExchExtPropertySheets;

		return E_NOINTERFACE;
    }
    else if (IID_IExchExtMessageEvents == riid)
    {
//		KLSTD_TRACE0 ( MCOU_TL_SPAM, L"IID_IExchExtMessageEvents\n" );
        *ppvObj = (LPUNKNOWN) m_pExchExtMessageEvents;
    }
    else if (IID_IExchExtSessionEvents == riid)
	{
//		KLSTD_TRACE0 ( MCOU_TL_SPAM, L"IID_IExchExtSessionEvents\n" );
		*ppvObj = (LPUNKNOWN) m_pExchExtSessionEvents;
	}
    else if (IID_IExchExtAttachedFileEvents == riid)
	{
//		KLSTD_TRACE0 ( MCOU_TL_SPAM, L"IID_IExchExtAttachedFileEvents\n" );
//		*ppvObj = (LPUNKNOWN) m_pExchExtAttachedFileEvents;
		return E_NOINTERFACE;
	}
	else
	{
//		KLSTD_TRACE0 ( MCOU_TL_SPAM, L"E_NOINTERFACE\n" );
        hResult = E_NOINTERFACE;
	}

    if (NULL != *ppvObj)
        ((LPUNKNOWN)*ppvObj)->AddRef();
	else
		hResult = E_NOINTERFACE;

    return hResult;
}

//////////////////////////////////////////////////////////////////////////
//
// IExchExt implementation
//

STDMETHODIMP IMCOUExchExt::Install (LPEXCHEXTCALLBACK peecb, ULONG eecontext, ULONG ulFlags)
{
#ifdef _DEBUG
	LPCSTR szStrContext = NULL;
	switch (eecontext)
	{
	case EECONTEXT_SESSION:
		szStrContext = "EECONTEXT_SESSION";
		break;
	case EECONTEXT_VIEWER:
		szStrContext = "EECONTEXT_VIEWER";
		break;
	case EECONTEXT_REMOTEVIEWER:
		szStrContext = "EECONTEXT_REMOTEVIEWER";
		break;
	case EECONTEXT_SEARCHVIEWER:
		szStrContext = "EECONTEXT_SEARCHVIEWER";
		break;
	case EECONTEXT_ADDRBOOK:
		szStrContext = "EECONTEXT_ADDRBOOK";
		break;
	case EECONTEXT_SENDNOTEMESSAGE:
		szStrContext = "EECONTEXT_SENDNOTEMESSAGE";
		break;
	case EECONTEXT_READNOTEMESSAGE:
		szStrContext = "EECONTEXT_READNOTEMESSAGE";
		break;
	case EECONTEXT_SENDPOSTMESSAGE:
		szStrContext = "EECONTEXT_SENDPOSTMESSAGE";
		break;
	case EECONTEXT_READPOSTMESSAGE:
		szStrContext = "EECONTEXT_READPOSTMESSAGE";
		break;
	case EECONTEXT_READREPORTMESSAGE:
		szStrContext = "EECONTEXT_READREPORTMESSAGE";
		break;
	case EECONTEXT_SENDRESENDMESSAGE:
		szStrContext = "EECONTEXT_SENDRESENDMESSAGE";
		break;
	case EECONTEXT_PROPERTYSHEETS:
		szStrContext = "EECONTEXT_PROPERTYSHEETS";
		break;
	case EECONTEXT_ADVANCEDCRITERIA:
		szStrContext = "EECONTEXT_ADVANCEDCRITERIA";
		break;
	case EECONTEXT_TASK:
		szStrContext = "EECONTEXT_TASK";
		break;
	default:
		szStrContext = "UNKNOWN";
	}
//	KLSTD_TRACE1 ( MCOU_TL_INFO, L"IMCOUExchExt::Install (%hs context)\n", szStrContext);
#endif

	ULONG ulBuildVersion;
	HRESULT hr = S_FALSE;

	m_context = eecontext;
   
	// make sure this is the right version 
	hr = peecb->GetVersion(&ulBuildVersion, EECBGV_GETBUILDVERSION);
	
	if (FAILED(hr) || EECBGV_BUILDVERSION_MAJOR != (ulBuildVersion & EECBGV_BUILDVERSION_MAJOR_MASK))
		return S_FALSE;


	switch (eecontext)
	{
	case EECONTEXT_SESSION:
	case EECONTEXT_PROPERTYSHEETS:
	case EECONTEXT_SENDNOTEMESSAGE:
	case EECONTEXT_SENDPOSTMESSAGE:
	case EECONTEXT_SENDRESENDMESSAGE:
	case EECONTEXT_READNOTEMESSAGE:
	case EECONTEXT_READPOSTMESSAGE:
	case EECONTEXT_READREPORTMESSAGE:
		hr = S_OK;
		break;

	default:
		hr = S_FALSE;
		break;
	}

	return hr;
}
