// EnumProcess.h: interface for the CEnumProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENUMPROCESS_H__E0C1CE05_D72A_43F2_8B07_69D4FBB33179__INCLUDED_)
#define AFX_ENUMPROCESS_H__E0C1CE05_D72A_43F2_8B07_69D4FBB33179__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PSAPI.h"

class CEnumProcess : public CPSAPI  
{
public:
	CEnumProcess(CStringList* pStringList);
	virtual ~CEnumProcess();
	virtual BOOL OnProcess(LPCTSTR lpszFileName, DWORD ProcessID);
private:
	CStringList* m_pStringList;
};

#endif // !defined(AFX_ENUMPROCESS_H__E0C1CE05_D72A_43F2_8B07_69D4FBB33179__INCLUDED_)
