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
#include "MessageEvents.h"
#include "util.h"
#include "interface.h"
#include "debug.h"

IMCOUExchExtMessageEvents::IMCOUExchExtMessageEvents (LPUNKNOWN pParentInterface)
{
	m_pExchExt = pParentInterface;
	m_cRef = 1;
	m_bInSubmitState = FALSE;

	PR_TRACE((0, prtIMPORTANT, "mcou\tIMCOUExchExtMessageEvents::IMCOUExchExtMessageEvents (RefCount = %d)", m_cRef));
}

//////////////////////////////////////////////////////////////////////////
//
// IUnknown implementation
//

ULONG IMCOUExchExtMessageEvents::AddRef ()
{
	++m_cRef;

	PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIMCOUExchExtMessageEvents::AddRef (RefCount = %d)", m_cRef));

	return m_cRef;
}

STDMETHODIMP_(ULONG) IMCOUExchExtMessageEvents::Release () 
{ 
	ULONG ulCount = --m_cRef;

	PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIMCOUExchExtMessageEvents::Release (RefCount = %d)", ulCount));

	if (!ulCount) 
	{
		PR_TRACE((0, prtIMPORTANT, "mcou\tdeleting IMCOUExchExtMessageEvents instance", ulCount));
		delete this;
	}

	return ulCount;
}

STDMETHODIMP IMCOUExchExtMessageEvents::QueryInterface (REFIID riid, LPVOID * ppvObj)
{
	LPOLESTR _ps;
	StringFromIID ( riid, &_ps );
	PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIMCOUExchExtMessageEvents::QueryInterface %s ", _ps));
	CoTaskMemFree ( _ps );

	*ppvObj = NULL;

	if (riid == IID_IExchExtMessageEvents)
	{
		PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIID_IExchExtMessageEvents"));

		*ppvObj = (LPVOID)this;
		// Increase usage count of this object
		AddRef();
		return S_OK;
	}

	if (riid == IID_IUnknown)
	{
		PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIID_IUnknown"));

		*ppvObj = (LPVOID)m_pExchExt;  // return parent interface
		m_pExchExt->AddRef();
		return S_OK;
	}

	PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tE_NOINTERFACE"));

	return E_NOINTERFACE;
}

//////////////////////////////////////////////////////////////////////////
//
// IExchExtMessageEvents implementation
//

STDMETHODIMP IMCOUExchExtMessageEvents::OnRead (LPEXCHEXTCALLBACK lpeecb)
{
	PR_TRACE((0, prtIMPORTANT, "mcou\tIMCOUExchExtMessageEvents::OnRead"));

	if(!CheckBOOLValue("CheckOnRead"))
		return S_FALSE;

	HRESULT hrReturn = S_FALSE;

	MCOU_TRY
		MCOU::ProcessMessage(lpeecb, TRUE, FALSE, hrReturn); // incoming / sync
	MCOU_END_TRY_RETHROW	

	return hrReturn;
}

STDMETHODIMP IMCOUExchExtMessageEvents::OnReadComplete (LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags)
{
	PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIMCOUExchExtMessageEvents::OnReadComplete"));

	return S_FALSE;
}

STDMETHODIMP IMCOUExchExtMessageEvents::OnWrite (LPEXCHEXTCALLBACK lpeecb)
{
	PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIMCOUExchExtMessageEvents::OnWrite"));

	return S_FALSE;
}

STDMETHODIMP IMCOUExchExtMessageEvents::OnWriteComplete (LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags)
{
	PR_TRACE((0, prtIMPORTANT, "mcou\tIMCOUExchExtMessageEvents::OnWriteComplete"));

	if(!m_bInSubmitState)    // means user is just "saving the message"
		return S_FALSE;

	if(!CheckBOOLValue("CheckOnWriteComplete"))
		return S_FALSE;

	HRESULT hrReturn = S_FALSE;

	MCOU_TRY
		MCOU::ProcessMessage(lpeecb, FALSE, FALSE, hrReturn); // outgoing / sync
	MCOU_END_TRY_RETHROW

	return hrReturn;
}

STDMETHODIMP IMCOUExchExtMessageEvents::OnCheckNames (LPEXCHEXTCALLBACK lpeecb)
{
	PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIMCOUExchExtMessageEvents::OnCheckNames"));

	return S_FALSE;
}

STDMETHODIMP IMCOUExchExtMessageEvents::OnCheckNamesComplete (LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags)
{
	PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIMCOUExchExtMessageEvents::OnCheckNamesComplete"));

	return S_FALSE;
}

STDMETHODIMP IMCOUExchExtMessageEvents::OnSubmit (LPEXCHEXTCALLBACK lpeecb)
{
	PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIMCOUExchExtMessageEvents::OnSubmit"));

	m_bInSubmitState = TRUE;

	return S_FALSE;
}

STDMETHODIMP_ (VOID) IMCOUExchExtMessageEvents::OnSubmitComplete (LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags)
{
	PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIMCOUExchExtMessageEvents::OnSubmitComplete"));

	m_bInSubmitState = FALSE;
}