
#pragma warning(disable : 4786)

#pragma data_seg(".text")

    #define INITGUID
    #include <objbase.h>
    #include <initguid.h>
    #include <shlguid.h>

#pragma data_seg()

#include "ShellExt.h"
#include "ShellExtClassFactory.h"

UINT g_cRefThisDll = 0 ; // Reference count of this DLL.

//--------------------------- CShellExtClassFactory -----------------

CShellExtClassFactory::CShellExtClassFactory()
{
    m_cRef = 0 ;
    ++g_cRefThisDll ;
}

CShellExtClassFactory::~CShellExtClassFactory()
{
    --g_cRefThisDll ;
}

STDMETHODIMP CShellExtClassFactory::QueryInterface(REFIID riid, LPVOID FAR * ppv)
{
    *ppv = NULL ;
    
    // Any interface on this object is the object pointer
    if (::IsEqualIID(riid, IID_IUnknown) || ::IsEqualIID(riid, IID_IClassFactory))
    {
        *ppv = (LPCLASSFACTORY) this ;
        
        AddRef() ;
        
        return NOERROR ;
    }

    return E_NOINTERFACE ;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef ()
{
    return ++m_cRef ;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    if (m_cRef)
    {
        if (--m_cRef)
        {
            return m_cRef ;
        }

        delete this ;
    }

    return 0 ;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance ( LPUNKNOWN pUnkOuter, REFIID riid, LPVOID * ppvObj )
{
    *ppvObj = NULL ;
    
    // Shell extensions typically don't support aggregation (inheritance)
    
    if (pUnkOuter)
    {
        return CLASS_E_NOAGGREGATION ;
    }
    
    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.
    
    LPCSHELLEXT pShellExt = new CShellExt() ;
    
    if (pShellExt == NULL)
    {
        return E_OUTOFMEMORY ;
    }

    return pShellExt->QueryInterface (riid, ppvObj) ;
}

STDMETHODIMP CShellExtClassFactory::LockServer(BOOL fLock)
{
    return NOERROR ;
}

//-------------------------------------------------------------------