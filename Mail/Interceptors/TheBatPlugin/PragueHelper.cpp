// PragueHelper.cpp: implementation of the CPragueHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PragueHelper.h"
#include <boost/scoped_array.hpp>
#include <version/ver_product.h>

namespace
{
	LPCTSTR REGKEY_PRODUCT_SETTINGS = _T(VER_PRODUCT_REGISTRY_PATH "\\environment");
	LPCTSTR REGVALUE_PRODUCT_PATH = _T("ProductRoot");
}

tstring PragueHelper::GetProductPath(bool bAddSlash)
{
	CRegKey keyAVP6;
	bool bWOWFlagsAvailable = (DWORD)(((LOBYTE(LOWORD(GetVersion()))) << 8) | (HIBYTE(LOWORD(GetVersion())))) > 0x0500;
	if (keyAVP6.Open(HKEY_LOCAL_MACHINE, REGKEY_PRODUCT_SETTINGS, KEY_READ | (bWOWFlagsAvailable ? KEY_WOW64_32KEY : 0)) != ERROR_SUCCESS)
		return _T("");
	
	tstring strRet = GetRegStringValue(keyAVP6, REGVALUE_PRODUCT_PATH);
	if (bAddSlash && (strRet.size() > 0))
	{
		if ((*strRet.rbegin() != '\\') && (*strRet.rbegin() != '/'))
			strRet += '\\';
	}

	return strRet;
}

tstring PragueHelper::GetRegStringValue(CRegKey& reg, LPCTSTR szValueName)
{
	DWORD dwBufSize = 512;
	TCHAR szBuf[512] = {0};
	
	LONG lRes = reg.QueryStringValue(szValueName, szBuf, &dwBufSize);
	if (lRes == ERROR_SUCCESS)
		return szBuf;
	
	if (lRes != ERROR_MORE_DATA)
		return _T("");	// unknown error
	
	// allocate more memory
	boost::scoped_array<TCHAR> newBuf(new TCHAR[dwBufSize]);
	lRes = reg.QueryStringValue(szValueName, newBuf.get(), &dwBufSize);
	if (lRes != ERROR_SUCCESS)
		return _T("");	// unknown error
	
	return newBuf.get();
}
