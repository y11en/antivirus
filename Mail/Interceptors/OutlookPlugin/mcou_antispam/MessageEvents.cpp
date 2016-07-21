/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	MessageEvents.cpp
*		\brief	IExchExtMessageEvents interface implementation
*		
*		\author Nick Lebedev
*		\date	19.06.2003 15:24:21
*		
*		Example:	
*		
*		
*		
*/		

#include "stdafx.h"
#include <mapidefs.h>
#include <mapiform.h>
#include "MessageEvents.h"

IMCOUExchExtMessageEvents::IMCOUExchExtMessageEvents (LPUNKNOWN pParentInterface)
{
	m_pExchExt = pParentInterface;
	m_cRef = 0;
	m_bInSubmitState = FALSE;

//	KLSTD_TRACE1 ( MCOU_TL_INFO, L"IMCOUExchExtMessageEvents::IMCOUExchExtMessageEvents (RefCount = %d)\n", m_cRef);
}

//////////////////////////////////////////////////////////////////////////
//
// IUnknown implementation
//

ULONG IMCOUExchExtMessageEvents::AddRef ()
{
	++m_cRef;

//	KLSTD_TRACE1 ( MCOU_TL_SPAM, L"IMCOUExchExtMessageEvents::AddRef (RefCount = %d)\n", m_cRef);
	
	return m_cRef;
}

STDMETHODIMP_(ULONG) IMCOUExchExtMessageEvents::Release () 
{ 
	ULONG ulCount = --m_cRef;

//	KLSTD_TRACE1 ( MCOU_TL_SPAM, L"IMCOUExchExtMessageEvents::Release (RefCount = %d)\n", ulCount);

	if (!ulCount) 
	{
//		KLSTD_TRACE1 ( MCOU_TL_INFO, L"deleting IMCOUExchExtMessageEvents instance\n", ulCount);
		delete this;
	}

	return ulCount;
}

STDMETHODIMP IMCOUExchExtMessageEvents::QueryInterface (REFIID riid, LPVOID * ppvObj)
{
#ifdef _DEBUG
	LPOLESTR _ps;
	StringFromIID ( riid, &_ps );
//	KLSTD_TRACE1 ( MCOU_TL_SPAM, L"IMCOUExchExtMessageEvents::QueryInterface %s ", _ps );
	CoTaskMemFree ( _ps );
#endif

	*ppvObj = NULL;

	if (riid == IID_IExchExtMessageEvents)
	{
//		KLSTD_TRACE0 ( MCOU_TL_SPAM, L"IID_IExchExtMessageEvents\n" );

		*ppvObj = (LPVOID)this;
		// Increase usage count of this object
		AddRef();
		return S_OK;
	}

	if (riid == IID_IUnknown)
	{
//		KLSTD_TRACE0 ( MCOU_TL_SPAM, L"IID_IUnknown\n" );

		*ppvObj = (LPVOID)m_pExchExt;  // return parent interface
		m_pExchExt->AddRef();
		return S_OK;
	}

//	KLSTD_TRACE0 ( MCOU_TL_SPAM, L"E_NOINTERFACE\n" );

	return E_NOINTERFACE;
}

//////////////////////////////////////////////////////////////////////////
//
// IExchExtMessageEvents implementation
//

STDMETHODIMP IMCOUExchExtMessageEvents::OnRead (LPEXCHEXTCALLBACK lpeecb)
{
//	KLSTD_TRACE0 ( MCOU_TL_INFO, L"IMCOUExchExtMessageEvents::OnRead\n" );

//	MCOU::ProcessMessage ( lpeecb, TRUE, TRUE );

	return S_FALSE;
}

STDMETHODIMP IMCOUExchExtMessageEvents::OnReadComplete (LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags)
{
//	KLSTD_TRACE0 ( MCOU_TL_INFO, L"IMCOUExchExtMessageEvents::OnReadComplete\n" );

	return S_FALSE;
}

STDMETHODIMP IMCOUExchExtMessageEvents::OnWrite (LPEXCHEXTCALLBACK lpeecb)
{
//	KLSTD_TRACE0 ( MCOU_TL_INFO, L"IMCOUExchExtMessageEvents::OnWrite\n" );

	return S_FALSE;
}

STDMETHODIMP IMCOUExchExtMessageEvents::OnWriteComplete (LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags)
{
//	KLSTD_TRACE0 ( MCOU_TL_INFO, L"IMCOUExchExtMessageEvents::OnWriteComplete\n" );

	if (!m_bInSubmitState)    // means user is just "saving the message"
		return S_FALSE;

//	MCOU::ProcessMessage ( lpeecb, FALSE, FALSE );

	return S_FALSE;
}

STDMETHODIMP IMCOUExchExtMessageEvents::OnCheckNames (LPEXCHEXTCALLBACK lpeecb)
{
//	KLSTD_TRACE0 ( MCOU_TL_INFO, L"IMCOUExchExtMessageEvents::OnCheckNames\n" );

	return S_FALSE;
}

STDMETHODIMP IMCOUExchExtMessageEvents::OnCheckNamesComplete (LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags)
{
//	KLSTD_TRACE0 ( MCOU_TL_INFO, L"IMCOUExchExtMessageEvents::OnCheckNamesComplete\n" );

	return S_FALSE;
}

STDMETHODIMP IMCOUExchExtMessageEvents::OnSubmit (LPEXCHEXTCALLBACK lpeecb)
{
//	KLSTD_TRACE0 ( MCOU_TL_INFO, L"IMCOUExchExtMessageEvents::OnSubmit\n" );

	m_bInSubmitState = TRUE;

	return S_FALSE;
}

STDMETHODIMP_ (VOID) IMCOUExchExtMessageEvents::OnSubmitComplete (LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags)
{
//	KLSTD_TRACE0 ( MCOU_TL_INFO, L"IMCOUExchExtMessageEvents::OnSubmitComplete\n" );

	m_bInSubmitState = FALSE;
}