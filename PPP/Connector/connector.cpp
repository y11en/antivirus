// -------- Monday,  23 December 2002,  12:56 --------
// -------------------------------------------
// Copyright (c) Kaspersky Labs. 1996-2002.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Purpose     -- AVS component
// Author      -- Mezhuev Pavel
// File Name   -- avs.cpp
// Additional info
//
// -------------------------------------------

#include <windows.h>
#include "connector.h"

//////////////////////////////////////////////////////////////////////////
// Module interface

HMODULE g_hModule = NULL;

static CConnectorImplPtr g_pConnector;

extern "C" __declspec(dllexport) bool KLCONN_INITDLL(KLERR::Error **ppError)
{
	*ppError = NULL;

	_tzset();

    return true;
}

extern "C" __declspec(dllexport) void KLCONN_DEINITDLL()
{
	if( g_pConnector )
		g_pConnector.Release();
}

extern "C" __declspec(dllexport) void KLCONN_CREATECONNECTOR(const wchar_t *szwProduct, const wchar_t *szwVersion, KLCONN::Connector **ppConnector, KLERR::Error **ppError)
{
	*ppError = NULL;

	if( !g_pConnector )
		g_pConnector.Attach(new CConnectorImpl);

	g_pConnector.CopyTo(ppConnector);
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD dwReason, LPVOID lpReserved )
{
	switch (dwReason)
	{
    case DLL_PROCESS_ATTACH:
		g_hModule = hModule;
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}


// self-registering code
// connector.dll is statically linked this klcsstd.dll and klcsrt.dll
// both of this dlls are used by netagent, in whitch address space the
// connector.dll is loaded. so its no reason to distribute this dlls this
// connector.dll package, but in this case we can't register dll by regsvr32
//
//#include <nagent\conn\connector_install.h>
//#include <atlbase.h>
//
//#define KAVWS_PRODUCT_NAME              TEXT("KAV6")
//#define KAVWS_PRODUCT_VERSION           TEXT("6.0.0.0")
//
//STDAPI DllRegisterServer(void)
//{
//	HRESULT rc = S_OK;
//
//	TCHAR szFileName[MAX_PATH];
//	if( !GetModuleFileName(g_hModule, szFileName, countof(szFileName)) )
//		return HRESULT_FROM_WIN32(GetLastError());
//
//	CRegKey reg;
//	if( SUCCEEDED(rc = HRESULT_FROM_WIN32(reg.Create(HKEY_LOCAL_MACHINE, KLCONN_REGKEY_INSTALL_ROOT TEXT("\\") KAVWS_PRODUCT_NAME TEXT("\\") KAVWS_PRODUCT_VERSION))) )
//	{
//		if(
//			SUCCEEDED(rc = HRESULT_FROM_WIN32(reg.SetValue(TEXT("Kaspersky Internet Security 2006"), KLCONN_REGVAL_DISPLAYNAME))) &&
//			SUCCEEDED(rc = HRESULT_FROM_WIN32(reg.SetValue(KAVWS_PRODUCT_VERSION, KLCONN_REGVAL_PRODVERSION))) &&
//			SUCCEEDED(rc = HRESULT_FROM_WIN32(reg.SetValue(KAVWS_PRODUCT_VERSION, KLCONN_REGVAL_CONNECTOR_VERSION))) &&
//			SUCCEEDED(rc = HRESULT_FROM_WIN32(reg.SetValue(KLCONN::CIF_HAS_FIREWALL|KLCONN::CIF_SUPPORTS_NETWORK_LIST|KLCONN::CIF_PROVIDES_RTP|KLCONN::CIF_OWN_SCHEDULER|KLCONN::CIF_SUPPORT_AK_POLICIES|KLCONN::CIF_SINGLE_THREADED, KLCONN_REGVAL_CONNECTOR_FLAGS))) &&
//			SUCCEEDED(rc = HRESULT_FROM_WIN32(reg.SetValue(szFileName, KLCONN_REGVAL_CONNECTOR_DLL)))
//			)
//		{
//			if( reg.Create(HKEY_LOCAL_MACHINE, KLCONN_REGKEY_CONN_INSTALLED) == ERROR_SUCCESS )
//				reg.SetValue(1, KLCONN_REGVAL_CONN_INSTALLED);
//		}
//	}
//
//	return rc;
//}
//
//STDAPI DllUnregisterServer(void)
//{
//	HRESULT rc = S_OK;
//
//	CRegKey reg;
//	if( SUCCEEDED(rc = HRESULT_FROM_WIN32(reg.Open(HKEY_LOCAL_MACHINE, KLCONN_REGKEY_INSTALL_ROOT))) )
//		rc = HRESULT_FROM_WIN32(reg.DeleteSubKey(KAVWS_PRODUCT_NAME));
//
//	return rc;
//}
