// Register.h: interface for the CRegister class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTER_H__AA4F2559_688F_4E25_BB0C_D78C4C219BDD__INCLUDED_)
#define AFX_REGISTER_H__AA4F2559_688F_4E25_BB0C_D78C4C219BDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRegister  
{
public:
	static int RegisterASPlugin(LPCTSTR szPluginPath);
	static int UnregisterASPlugin(LPCTSTR szPluginPath);
	static int CleanRegistrationFlag(LPCTSTR szPluginPath);
};

#endif // !defined(AFX_REGISTER_H__AA4F2559_688F_4E25_BB0C_D78C4C219BDD__INCLUDED_)
