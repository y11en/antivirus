/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	SessionEvents.cpp
*		\brief	IExchExtSessionEvents interface implementation
*		
*		\author Nick Lebedev
*		\date	19.06.2003 15:25:01
*		
*		Example:	
*		
*		
*		
*/		

#include "stdafx.h"
#include "util.h"
#include "SessionEvents.h"
#include "initterm.h"
#include "../mcou_antispam/pmcMCOU_ANTISPAM.h"
#include "interface.h"
#include "debug.h"

#include <Mail/iface/i_mailtask.h>

IMCOUExchExtSessionEvents::IMCOUExchExtSessionEvents (LPUNKNOWN pParentInterface)
{
	m_pExchExt = pParentInterface;
	m_cRef = 1; 

	PR_TRACE((0, prtIMPORTANT, "mcou\tIMCOUExchExtSessionEvents::IMCOUExchExtSessionEvents (RefCount = %d)", m_cRef));
}

//////////////////////////////////////////////////////////////////////////
//
// IUnknown implementation
//

ULONG IMCOUExchExtSessionEvents::AddRef ()
{
	++m_cRef;

	PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIMCOUExchExtSessionEvents::AddRef (RefCount = %d)", m_cRef));
	
	return m_cRef;
}

STDMETHODIMP_(ULONG) IMCOUExchExtSessionEvents::Release () 
{ 
	ULONG ulCount = --m_cRef;

	PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIMCOUExchExtSessionEvents::Release (RefCount = %d)", ulCount));

	if (!ulCount) 
	{
		PR_TRACE((0, prtIMPORTANT, "mcou\tdeleting IMCOUExchExtSessionEvents instance", ulCount));
		delete this;
	}

	return ulCount;
}

STDMETHODIMP IMCOUExchExtSessionEvents::QueryInterface (REFIID riid, LPVOID * ppvObj)
{
	LPOLESTR _ps;
	StringFromIID ( riid, &_ps );
	PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIMCOUExchExtSessionEvents::QueryInterface %s ", _ps));
	CoTaskMemFree ( _ps );

	*ppvObj = NULL;

	if (riid == IID_IExchExtSessionEvents)
	{
		PR_TRACE((0, prtNOT_IMPORTANT, "mcou\tIID_IExchExtSessionEvents"));

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
// IExchExtSessionEvents implementation
//

STDMETHODIMP IMCOUExchExtSessionEvents::OnDelivery(LPEXCHEXTCALLBACK lpeecb)
{
	PR_TRACE((0, prtIMPORTANT, "mcou\tIMCOUExchExtSessionEvents::OnDelivery"));
	
	if(!CheckBOOLValue("CheckOnDelivery"))
		return S_FALSE;

	HRESULT hrReturn = S_FALSE;

	MCOU_TRY
		MCOU::ProcessMessage(lpeecb, TRUE, TRUE, hrReturn); // incoming / async
	MCOU_END_TRY_RETHROW

	return hrReturn;
}