// UncontrolledFiles.h: interface for the CUncontrolledFiles class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNCONTROLLEDFILES_H__0FC2808F_9DC7_47FC_BF7D_A267DC189341__INCLUDED_)
#define AFX_UNCONTROLLEDFILES_H__0FC2808F_9DC7_47FC_BF7D_A267DC189341__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef std::vector<CString> _StrVector;

class CUncontrolledFiles  
{
public:
	CUncontrolledFiles();
	virtual ~CUncontrolledFiles();

	_StrVector m_UncotrolledFileList;
private:
	void PushEnvironmentString(PTCHAR ptchString);

};

#endif // !defined(AFX_UNCONTROLLEDFILES_H__0FC2808F_9DC7_47FC_BF7D_A267DC189341__INCLUDED_)
