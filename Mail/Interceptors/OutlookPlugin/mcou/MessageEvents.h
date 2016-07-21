#ifndef __MESSAGEEVENTS_H__
#define __MESSAGEEVENTS_H__

#include <exchext.h>

class IMCOUExchExtMessageEvents : public IExchExtMessageEvents
{
public:

    IMCOUExchExtMessageEvents (LPUNKNOWN pParentInterface);

    STDMETHODIMP QueryInterface (REFIID riid, LPVOID *ppvObj);
    STDMETHODIMP_(ULONG) AddRef ();
    STDMETHODIMP_(ULONG) Release ();

    STDMETHODIMP OnRead(LPEXCHEXTCALLBACK lpeecb);
    STDMETHODIMP OnReadComplete(LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags);
    STDMETHODIMP OnWrite(LPEXCHEXTCALLBACK lpeecb);
    STDMETHODIMP OnWriteComplete(LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags);
    STDMETHODIMP OnCheckNames(LPEXCHEXTCALLBACK lpeecb);
    STDMETHODIMP OnCheckNamesComplete(LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags);
    STDMETHODIMP OnSubmit(LPEXCHEXTCALLBACK lpeecb);
    STDMETHODIMP_ (VOID)OnSubmitComplete(LPEXCHEXTCALLBACK lpeecb, ULONG ulFlags);

protected:
    ULONG		m_cRef;
    LPUNKNOWN	m_pExchExt;
    BOOL		m_bInSubmitState;
};

#endif