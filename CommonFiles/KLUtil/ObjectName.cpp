// ObjectName.cpp: implementation of the ObjectName class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ObjectName.h"
#include "OsVersion.h"

CStdString KLUTIL::MakeMutexName(LPCTSTR szPrefix, const CStdString &sName)
{
	CStdString sMutexName = szPrefix;
	
	for (int i = 0;i < sName.GetLength();i++)
	{
		if (sName[i] != _T('\\') &&
			sName[i] != _T(':') &&
			sName[i] != _T('/') &&
			sName[i] != _T(';') &&
			sName[i] != _T('+') &&
			sName[i] != _T('.')) sMutexName += sName[i];
		else
			sMutexName += _T("_");
	}
	
	return sMutexName;
}

CStdString KLUTIL::MakeGlobalObjectName(LPCTSTR pszName)
{
	CStdString rc;
	COSVersion ver;
	
	if (pszName && *pszName)
	{
		if (ver.IsGlobalNamespaceSupported())
			rc.Format(_T("Global\\%s"), pszName);
		else
			rc = pszName;
	}
	
	return rc;
}
