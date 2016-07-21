/////////////////////////////////////////////////////////////////////////////
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright 1998 Microsoft Corporation.  All Rights Reserved.
//
// Author: Scott Roberts, Microsoft Developer Support - Internet Client SDK  
//
// Portions of this code were taken from the bandobj sample that comes
// with the Internet Client SDK for Internet Explorer 4.0x
//
//
// ClsFact.h - CClassFactory declaration
/////////////////////////////////////////////////////////////////////////////

#ifndef __ClsFact_h__
#define __ClsFact_h__

#include <windows.h>
#include "InstanceQ.h"

class CClassFactory : public IClassFactory
{
private:
	ULONG m_nRefCount;

public:
	CClassFactory(REFCLSID clsid, REFIID riid, LPVOID* ppv);
	~CClassFactory();
	
	//IUnknown methods
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	
	//IClassFactory methods
	STDMETHOD(CreateInstance)(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID* ppvObject);
	STDMETHOD(LockServer)(BOOL fLock);
	
public:
	CLSID m_clsidObject;
	IClassFactory* m_ppv;
	HRESULT m_hResult;
	WCHAR* m_pLanguage;
	BOOL   m_bEncoded;
};

#endif   // __ClsFact_h__
