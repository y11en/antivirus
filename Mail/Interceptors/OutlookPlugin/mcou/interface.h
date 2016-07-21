#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <exchext.h>

#include "MessageEvents.h"
#include "SessionEvents.h"

class IMCOUExchExt : public IExchExt
{
public:
    IMCOUExchExt();
    virtual ~IMCOUExchExt();

    STDMETHODIMP QueryInterface(REFIID riid, LPVOID *ppvObj);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release(); 

    STDMETHODIMP Install (LPEXCHEXTCALLBACK pmecb, 
	                        ULONG mecontext, ULONG ulFlags);

protected:
    ULONG m_cRef;
    UINT  m_context;

	IMCOUExchExtMessageEvents * m_pExchExtMessageEvents;
	IMCOUExchExtSessionEvents * m_pExchExtSessionEvents;
};

BOOL CheckBOOLValue(CHAR* name);

#endif