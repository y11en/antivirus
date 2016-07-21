#ifndef _HANDLER_H_
#define _HANDLER_H_

#include "stdafx.h"

class CAddin; // forward declaration

class CHandler : public IDispatch
{
public:
    CHandler(REFIID riid, CAddin *pAddin); //!< Constructor. Initialize pointer to addin
    virtual ~CHandler();

    // *** IUnknown methods ***
    STDMETHODIMP         QueryInterface(REFIID riid, void ** ppv); //!< Retrieves a pointer to the requested interface
    STDMETHODIMP_(ULONG) AddRef(void);   //!< Increments the reference count on the object
    STDMETHODIMP_(ULONG) Release(void);  //!< Decrements the reference count on the object

    // *** IDispatch methods ***
    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo);                           //!< Retrieves the number of type information interfaces that an object provides (either 0 or 1).
    STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo);  //!< Retrieves the type information for an object, which can then be used to get the type information for an interface.
    STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId); //!< Maps a single member and an optional set of argument names to a corresponding set of integer DISPIDs
    STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr); //!< Provides access to properties and methods exposed by an object. 

    HRESULT SinkEvents(LPDISPATCH pButton);
    void FreeEvents();

protected:
    CAddin *m_pAddin;   //!< Pointer to addin
    ULONG m_cRef; //!< Reference count

private:
    REFIID m_riid;
    IConnectionPoint*     m_pConnection;
    DWORD                 m_dwCookie;

    CHandler(const CHandler &);
    void operator=(const CHandler &);
};

#endif//_HANDLER_H_