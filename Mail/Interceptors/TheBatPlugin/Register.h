// Register.h: interface for the CRegister class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTER_H__65890FC3_3AFF_48F1_BFDF_A7DD9B2C3484__INCLUDED_)
#define AFX_REGISTER_H__65890FC3_3AFF_48F1_BFDF_A7DD9B2C3484__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRegister  
{
public:
	static int RegisterAVPlugin(LPCTSTR szPluginPath);
	static int UnregisterAVPlugin(LPCTSTR szPluginPath);
	static int CleanRegistrationFlag(LPCTSTR szPluginPath);
};

#endif // !defined(AFX_REGISTER_H__65890FC3_3AFF_48F1_BFDF_A7DD9B2C3484__INCLUDED_)
