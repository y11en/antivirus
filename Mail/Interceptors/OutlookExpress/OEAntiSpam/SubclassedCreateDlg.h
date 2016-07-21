// SubclassedCreateDlg.h: interface for the CSubclassedCreateDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBCLASSEDCREATEDLG_H__B9ADB838_6D0E_409C_AC69_6821DDCE4599__INCLUDED_)
#define AFX_SUBCLASSEDCREATEDLG_H__B9ADB838_6D0E_409C_AC69_6821DDCE4599__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SubclassedBase.h"

class CSubclassedCreateDlg : public CSubclassedBase  
{
public:
	virtual void AfterInitDialog();

	void SetDesiredFolderName(LPCTSTR szFolderName)
	{
		m_strDesiredFolderName = szFolderName;
	}

private:
	tstring m_strDesiredFolderName;
};

#endif // !defined(AFX_SUBCLASSEDCREATEDLG_H__B9ADB838_6D0E_409C_AC69_6821DDCE4599__INCLUDED_)
