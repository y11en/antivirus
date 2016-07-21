#ifndef __SESSIONEVENTS_H__
#define __SESSIONEVENTS_H__

#include <exchext.h>

class IMCOUExchExtSessionEvents : public IExchExtSessionEvents
{
public:

    IMCOUExchExtSessionEvents (LPUNKNOWN pParentInterface);
    
    STDMETHODIMP QueryInterface (REFIID riid, LPVOID *ppvObj);
    STDMETHODIMP_(ULONG) AddRef ();
    STDMETHODIMP_(ULONG) Release ();

	STDMETHODIMP OnDelivery(LPEXCHEXTCALLBACK lpeecb);

protected:
    ULONG		m_cRef;
    LPUNKNOWN	m_pExchExt;
};

#endif