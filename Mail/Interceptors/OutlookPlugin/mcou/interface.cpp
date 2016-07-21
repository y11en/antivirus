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
#include "mcou.h"
#include "initguid.h"

#undef MAPIGUID_H
//#define USES_PS_PUBLIC_STRINGS
//#define USES_IID_IMessage
#define USES_IID_IStreamDocfile
#include <mapiguid.h>

#include "interface.h"
#include "initterm.h"
#include <ProductCore/GuiLoader.h>

extern IMCOUExchExt *g_pIMCOUExchExt;

IMCOUExchExt::IMCOUExchExt()
{
	m_cRef = 1;
    m_context = 0;

	m_pExchExtMessageEvents = new IMCOUExchExtMessageEvents (this);
	m_pExchExtSessionEvents = new IMCOUExchExtSessionEvents (this);

	MCOU::g_bInitSucceeded = InitEnvironment ();
	if ( !MCOU::g_bInitSucceeded )
	{
		PR_TRACE((0, prtERROR, "mcou\tInitializing environment failed"));
	}

	PR_TRACE((0, prtIMPORTANT, "IMCOUExchExt::IMCOUExchExt (RefCount = %d)", m_cRef));
}

IMCOUExchExt::~IMCOUExchExt()
{
	if(m_pExchExtMessageEvents)
		m_pExchExtMessageEvents->Release();
	if(m_pExchExtSessionEvents)
		m_pExchExtSessionEvents->Release();
	TermEnvironment ();
}

//////////////////////////////////////////////////////////////////////////
//
// IUnknown implementation
//

ULONG IMCOUExchExt::AddRef ()
{
	++m_cRef;

	PR_TRACE((0, prtNOTIFY, "mcou\tIMCOUExchExt::AddRef (RefCount = %d)", m_cRef));
	
	return m_cRef;
}

STDMETHODIMP_(ULONG) IMCOUExchExt::Release ()
{ 
	ULONG ulCount = --m_cRef;

	PR_TRACE((0, prtNOTIFY, "mcou\tIMCOUExchExt::Release (RefCount = %d)", ulCount));

	if (!ulCount) 
	{
		PR_TRACE((0, prtIMPORTANT, "mcou\tdeleting IMCOUExchExt instance"));

		CGuiLoader::Instance().Unload();
		delete this;
		g_pIMCOUExchExt = NULL;
	}

	return ulCount;
}

BOOL CheckBOOLValue(CHAR* name)
{
	HKEY key; 
	LONG res = RegOpenKeyEx(
		HKEY_CURRENT_USER,
		VER_PRODUCT_REGISTRY_PATH "\\mcou",
		0,
		KEY_READ,
		&key
		);
	if ( res == ERROR_SUCCESS )
	{
		DWORD type;
		DWORD size = sizeof(DWORD);
		BOOL bValue = TRUE;
		res = RegQueryValueEx(
			key,
			name,
			NULL,
			&type,
			(BYTE*)&bValue,
			&size
			);
		if ( res == ERROR_SUCCESS )
		{
			RegCloseKey(key);
			return bValue;
		}
		RegCloseKey(key);
	}
	return TRUE;
}


STDMETHODIMP IMCOUExchExt::QueryInterface (REFIID riid, LPVOID * ppvObj)
{
	LPOLESTR _ps;
	StringFromIID ( riid, &_ps );
	PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIMCOUExchExt::QueryInterface %s ", _ps));
	CoTaskMemFree ( _ps );

    HRESULT hResult = S_OK;

    *ppvObj = NULL;

    if (( IID_IUnknown == riid) || ( IID_IExchExt == riid ) )
    {
		PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIID_IUnknown or IID_IExchExt"));
        *ppvObj = (LPUNKNOWN)this;
    }
    else if (IID_IExchExtMessageEvents == riid)
    {
		PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIID_IExchExtMessageEvents"));
		*ppvObj = (LPUNKNOWN) m_pExchExtMessageEvents;
    }
    else if (IID_IExchExtSessionEvents == riid)
	{
		PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIID_IExchExtSessionEvents"));
		*ppvObj = (LPUNKNOWN) m_pExchExtSessionEvents;
	}
	else
	{
		PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tE_NOINTERFACE"));
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
	LPCSTR szStrContext = NULL;
	switch (eecontext)
	{
	case EECONTEXT_SESSION:
		szStrContext = _T("EECONTEXT_SESSION");
		break;
	case EECONTEXT_VIEWER:
		szStrContext = _T("EECONTEXT_VIEWER");
		break;
	case EECONTEXT_REMOTEVIEWER:
		szStrContext = _T("EECONTEXT_REMOTEVIEWER");
		break;
	case EECONTEXT_SEARCHVIEWER:
		szStrContext = _T("EECONTEXT_SEARCHVIEWER");
		break;
	case EECONTEXT_ADDRBOOK:
		szStrContext = _T("EECONTEXT_ADDRBOOK");
		break;
	case EECONTEXT_SENDNOTEMESSAGE:
		szStrContext = _T("EECONTEXT_SENDNOTEMESSAGE");
		break;
	case EECONTEXT_READNOTEMESSAGE:
		szStrContext = _T("EECONTEXT_READNOTEMESSAGE");
		break;
	case EECONTEXT_SENDPOSTMESSAGE:
		szStrContext = _T("EECONTEXT_SENDPOSTMESSAGE");
		break;
	case EECONTEXT_READPOSTMESSAGE:
		szStrContext = _T("EECONTEXT_READPOSTMESSAGE");
		break;
	case EECONTEXT_READREPORTMESSAGE:
		szStrContext = _T("EECONTEXT_READREPORTMESSAGE");
		break;
	case EECONTEXT_SENDRESENDMESSAGE:
		szStrContext = _T("EECONTEXT_SENDRESENDMESSAGE");
		break;
	case EECONTEXT_PROPERTYSHEETS:
		szStrContext = _T("EECONTEXT_PROPERTYSHEETS");
		break;
	case EECONTEXT_ADVANCEDCRITERIA:
		szStrContext = _T("EECONTEXT_ADVANCEDCRITERIA");
		break;
	case EECONTEXT_TASK:
		szStrContext = _T("EECONTEXT_TASK");
		break;
	default:
		szStrContext = _T("UNKNOWN");
	}
	PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIMCOUExchExt::Install (%s context)", szStrContext));

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
