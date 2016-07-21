
#include "stdafx.h"
#include "OutlookAddin.h"
#include "Addin.h"
#include "handler.h"

CHandler::CHandler(REFIID riid, CAddin *pAddin) //!< Constructor. Initialize pointer to addin
 : m_pAddin(pAddin)
 , m_pConnection(NULL)
 , m_riid(riid) 
 , m_dwCookie(0)
 , m_cRef(0)
{
	m_pAddin->AddRef();
}
CHandler::~CHandler()
{
    FreeEvents(); // Free m_pConnection
	m_pAddin->Release();
}


/******************************************************************
 * CHandler
 *
 ******************************************************************/
/******************************************************************
 * IUnknown Implementation
 *
 ******************************************************************/
STDMETHODIMP CHandler::QueryInterface(REFIID riid, void** ppv)
{
    if (NULL == ppv) return E_POINTER;
    *ppv = NULL;
    
    HRESULT hr = S_OK;

    if ((m_riid == riid) ||
        (IID_IUnknown == riid) || (IID_IDispatch == riid))
        *ppv = static_cast<IDispatch*>(this);
    else
        hr = E_NOINTERFACE;

    if (NULL != *ppv)
        reinterpret_cast<IUnknown*>(*ppv)->AddRef();

    return hr;
}

/******************************************************************
 * IDispatch Implementation
 *
 ******************************************************************/
/******************************************************************
 * CHandler::GetTypeInfoCount()
 *
 *    We don't need to provide TypeInfo. Return 0 for *pctinfo. 
 *
 ******************************************************************/
STDMETHODIMP CHandler::GetTypeInfoCount(UINT* pctinfo)
{
    if (NULL == pctinfo) return E_POINTER;
    *pctinfo = 0;
    return S_OK;
}

/******************************************************************
 * CHandler::GetTypeInfo()
 *
 ******************************************************************/
STDMETHODIMP CHandler::GetTypeInfo(UINT /*iTInfo*/, LCID /*lcid*/, ITypeInfo** ppTInfo)
{
    if (NULL == ppTInfo) return E_POINTER;
    *ppTInfo = NULL;
    return E_FAIL;
}

/******************************************************************
 * CHandler::GetIDsOfNames()
 *
 *    This function is never called from Office since this is an
 *    event dispinterface and Office already knows the dispID for the
 *    method to be fired. It is implemented anyway for the fun of it...
 *
 ******************************************************************/
STDMETHODIMP CHandler::GetIDsOfNames(REFIID /*riid*/, LPOLESTR* /*rgszNames*/, UINT /*cNames*/, LCID /*lcid*/, DISPID* /*rgDispId*/)
{
    return E_FAIL;
}

/******************************************************************
 * CHandler::Invoke()
 *
 *    The click event is fired by this Invoke call. There is only
 *    one event for this interface (0x00000001) and it takes two
 *    parameters:
 *
 *    LPDISPATCH     pCtrl     -- The primary interface of the button
 *                            being pressed.
 *
 *    VARIANT_BOOL bCancel -- Determines whether the default action
 *                            should be cancled.
 *
 ******************************************************************/


STDMETHODIMP CHandler::Invoke(DISPID dispIdMember, REFIID riid, LCID /*lcid*/, WORD /*wFlags*/, DISPPARAMS* /*pDispParams*/, VARIANT* /*pVarResult*/, EXCEPINFO* /*pExcepInfo*/, UINT* /*puArgErr*/)
{
    if ((riid != IID_NULL))
        return E_INVALIDARG;
    return S_OK;
}


/******************************************************************
 * CHandler::SinkEvents()
 *
 *    Sets up the Click event sink and saves a pointer to the 
 *    IConnectionPoint for the button. The sink should be freed before
 *    the application is released.
 *
 ******************************************************************/
HRESULT CHandler::SinkEvents(LPDISPATCH pObject)
{
    HRESULT hr;

 // Get server's IConnectionPointContainer interface.
    IConnectionPointContainer* pCPC;
    hr = pObject->QueryInterface(IID_IConnectionPointContainer, (void **)&pCPC);
    if (SUCCEEDED(hr)) {

    // Find connection point for events we're interested in.
        hr = pCPC->FindConnectionPoint(m_riid, &m_pConnection);
        if (SUCCEEDED(hr)) {
            hr = m_pConnection->Advise(static_cast<IDispatch*>(this), &m_dwCookie);
        }
    // Release the IConnectionPointContainer
        pCPC->Release();
    }
    return hr;
}

/******************************************************************
 * CHandler::FreeEvents()
 *
 *    Calls Unadvise on our saved IConnectionPoint interface. 
 *
 ******************************************************************/
void CHandler::FreeEvents()
{
    if (NULL != m_pConnection) {
        m_pConnection->Unadvise(m_dwCookie);
        m_pConnection->Release();
        m_pConnection = NULL;
    }
}

STDMETHODIMP_(ULONG) CHandler::AddRef()
{
    ++m_cRef;
    return m_cRef;
}

STDMETHODIMP_(ULONG) CHandler::Release()
{
	// Use stack parameter, in case we delete ourselves
    ULONG ulCount = --m_cRef;
                         
	if (!ulCount) 
	{ 
		delete this;
	}

	return ulCount;
}


////////////////////////////////////////////////////////////////////////

