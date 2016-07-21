
#ifndef __SHELLEXTCLASSFACTORY_H__
#define __SHELLEXTCLASSFACTORY_H__

#include <shlobj.h>

// class id:  dd230880-495a-11d1-b064-008048ec2fc5                              
#define SHLEX_CLASSID "{dd230880-495a-11d1-b064-008048ec2fc5}"

DEFINE_GUID(CLSID_ShellExtension, 0xdd230880, 0x495a, 0x11d1, 0xb0, 0x64, 0x00, 0x80, 0x48, 0xec, 0x2f, 0xc5) ;

//--------------------------- CShellExtClassFactory -----------------

class CShellExtClassFactory : public IClassFactory 
{
public:
    CShellExtClassFactory() ;
    ~CShellExtClassFactory() ;

    //IUnknown members
	STDMETHODIMP            QueryInterface(REFIID riid, LPVOID FAR * ppv) ;
    STDMETHODIMP_(ULONG)    AddRef() ;
    STDMETHODIMP_(ULONG)    Release() ;

    //IClassFactory members
    STDMETHODIMP            CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID FAR * ppv) ;
    STDMETHODIMP            LockServer(BOOL blLock) ;

protected:
    ULONG   m_cRef ;
};

//-------------------------------------------------------------------

#endif // __SHELLEXTCLASSFACTORY_H__