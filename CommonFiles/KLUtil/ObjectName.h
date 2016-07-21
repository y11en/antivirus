// ObjectName.h: helper functions for global OS objects' naming
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTNAME_H__42CBA362_A79B_4331_827C_71D33A5239DD__INCLUDED_)
#define AFX_OBJECTNAME_H__42CBA362_A79B_4331_827C_71D33A5239DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Stuff/StdString.h"
#include "defs.h"

namespace KLUTIL
{

	// MakeMutexName - removes '\',':','.' from mutex name
	KLUTIL_API CStdString MakeMutexName(LPCTSTR szPrefix, const CStdString &sName);

	// MakeGlobalObjectName - add 'Global\' prefix if Terminal Service is supported
	KLUTIL_API CStdString MakeGlobalObjectName(LPCTSTR pszName);

} // namespace KLUTIL

#endif // !defined(AFX_OBJECTNAME_H__42CBA362_A79B_4331_827C_71D33A5239DD__INCLUDED_)
