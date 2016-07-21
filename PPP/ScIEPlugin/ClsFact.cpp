/**************************************************************************

   File:          ClsFact.cpp
   
   Description:   Implements CClassFactory.

**************************************************************************/

/**************************************************************************
   #include statements
**************************************************************************/

#include "ClsFact.h"
#include "Guid.h"

/**************************************************************************
   private function prototypes
**************************************************************************/

/**************************************************************************
   global variables
**************************************************************************/

///////////////////////////////////////////////////////////////////////////
//
// IClassFactory implementation
//

/**************************************************************************

   CClassFactory::CClassFactory

**************************************************************************/

CClassFactory::CClassFactory(CLSID clsid)
{
	m_clsidObject = clsid;
	m_ObjRefCount = 1;
	g_DllRefCount++;
}

/**************************************************************************

   CClassFactory::~CClassFactory

**************************************************************************/

CClassFactory::~CClassFactory()
{
	g_DllRefCount--;
}

/**************************************************************************

   CClassFactory::QueryInterface

**************************************************************************/

STDMETHODIMP CClassFactory::QueryInterface(REFIID riid, LPVOID *ppReturn)
{
	*ppReturn = NULL;

	if(IsEqualIID(riid, IID_IUnknown))
	{
		*ppReturn = this;
	}
   
	else if(IsEqualIID(riid, IID_IClassFactory))
	{
		*ppReturn = (IClassFactory*)this;
	}   

	if(*ppReturn)
	{
		(*(LPUNKNOWN*)ppReturn)->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}                                             

/**************************************************************************

   CClassFactory::AddRef

**************************************************************************/

STDMETHODIMP_(DWORD) CClassFactory::AddRef()
{
	return ++m_ObjRefCount;
}


/**************************************************************************

   CClassFactory::Release

**************************************************************************/

STDMETHODIMP_(DWORD) CClassFactory::Release()
{
	if(--m_ObjRefCount == 0)
	{
		delete this;
		return 0;
	}
   
	return m_ObjRefCount;
}

/**************************************************************************

   CClassFactory::CreateInstance

**************************************************************************/

STDMETHODIMP CClassFactory::CreateInstance(  LPUNKNOWN pUnknown, 
                                             REFIID riid, 
                                             LPVOID *ppObject)
{
	HRESULT  hResult = E_FAIL;
	LPVOID   pTemp = NULL;

	*ppObject = NULL;

	if(pUnknown != NULL)
		return CLASS_E_NOAGGREGATION;

	if(IsEqualCLSID(m_clsidObject, CLSID_SampleExplorerBar))
	{
		CExplorerBar *pExplorerBar = new CExplorerBar();
		if(NULL == pExplorerBar)
			return E_OUTOFMEMORY;
   
		pTemp = pExplorerBar;
	}
  

	if(pTemp)
	{
		//get the QueryInterface return for our return value
		hResult = ((LPUNKNOWN)pTemp)->QueryInterface(riid, ppObject);

		//call Release to decement the ref count
		((LPUNKNOWN)pTemp)->Release();
	}

	return hResult;
}

/**************************************************************************

   CClassFactory::LockServer

**************************************************************************/

STDMETHODIMP CClassFactory::LockServer(BOOL)
{
	return E_NOTIMPL;
}
