/**************************************************************************

   File:          ClsFact.h
   
   Description:   CClassFactory definitions.

**************************************************************************/

#ifndef CLASSFACTORY_H
#define CLASSFACTORY_H

#include <windows.h>
#include "Globals.h"
#include "ExplrBar.h"

/**************************************************************************

   CClassFactory class definition

**************************************************************************/

class CClassFactory : public IClassFactory
{
protected:
   DWORD m_ObjRefCount;

public:
   CClassFactory(CLSID);
   ~CClassFactory();

   //IUnknown methods
   STDMETHODIMP QueryInterface(REFIID, LPVOID*);
   STDMETHODIMP_(DWORD) AddRef();
   STDMETHODIMP_(DWORD) Release();

   //IClassFactory methods
   STDMETHODIMP CreateInstance(LPUNKNOWN, REFIID, LPVOID*);
   STDMETHODIMP LockServer(BOOL);

private:
   CLSID m_clsidObject;
};

#endif   //CLASSFACTORY_H
