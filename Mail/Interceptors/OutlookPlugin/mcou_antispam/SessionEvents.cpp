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
#include <mapidefs.h>
#include <mapiform.h>
#include "SessionEvents.h"
//#include "initterm.h"
#include <iface/i_mailtask.h>
//#include "../mcou_antispam/pmcMCOU_ANTISPAM.h"

IMCOUExchExtSessionEvents::IMCOUExchExtSessionEvents (LPUNKNOWN pParentInterface)
{
	m_pExchExt = pParentInterface;
	m_cRef = 0; 

//	KLSTD_TRACE1 ( MCOU_TL_INFO, L"IMCOUExchExtSessionEvents::IMCOUExchExtSessionEvents (RefCount = %d)\n", m_cRef);
}

//////////////////////////////////////////////////////////////////////////
//
// IUnknown implementation
//

ULONG IMCOUExchExtSessionEvents::AddRef ()
{
	++m_cRef;

//	KLSTD_TRACE1 ( MCOU_TL_SPAM, L"IMCOUExchExtSessionEvents::AddRef (RefCount = %d)\n", m_cRef);
	
	return m_cRef;
}

STDMETHODIMP_(ULONG) IMCOUExchExtSessionEvents::Release () 
{ 
	ULONG ulCount = --m_cRef;

//	KLSTD_TRACE1 ( MCOU_TL_SPAM, L"IMCOUExchExtSessionEvents::Release (RefCount = %d)\n", ulCount);

	if (!ulCount) 
	{
//		KLSTD_TRACE1 ( MCOU_TL_INFO, L"deleting IMCOUExchExtSessionEvents instance\n", ulCount);
		delete this;
	}

	return ulCount;
}

STDMETHODIMP IMCOUExchExtSessionEvents::QueryInterface (REFIID riid, LPVOID * ppvObj)
{
#ifdef _DEBUG
	LPOLESTR _ps;
	StringFromIID ( riid, &_ps );
//	KLSTD_TRACE1 ( MCOU_TL_SPAM, L"IMCOUExchExtSessionEvents::QueryInterface %s ", _ps );
	CoTaskMemFree ( _ps );
#endif

	*ppvObj = NULL;

	if (riid == IID_IExchExtSessionEvents)
	{
//		KLSTD_TRACE0 ( MCOU_TL_SPAM, L"IID_IExchExtSessionEvents\n" );

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
// IExchExtSessionEvents implementation
//

STDMETHODIMP IMCOUExchExtSessionEvents::OnDelivery(LPEXCHEXTCALLBACK lpeecb)
{
//	KLSTD_TRACE0 ( MCOU_TL_INFO, L"IMCOUExchExtSessionEvents::OnDelivery\n" );
	
/*	hMAILTASK hMBL = (hMAILTASK)GetMBL(TRUE);
	if ( hMBL && errOK_DECIDED == hMBL->sysSendMsg(pmcMCOU_ANTISPAM, pmMCOU_ANTISPAM_ENABLED, 0, 0, 0))
		// Не обрабатываем событие OnDelivery, если известно, что за это письмо ухватится антиспам (т.к. антиспам проверяет и на вирусы тоже)
		return S_FALSE;

	MCOU::ProcessMessage ( lpeecb, TRUE, TRUE );*/

	return S_FALSE;
}