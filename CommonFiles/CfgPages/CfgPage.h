// CfgPage.h: interface for the CCfgPage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CFGPAGE_H__22B1A765_A71B_11D3_83C8_0000E816586C__INCLUDED_)
#define AFX_CFGPAGE_H__22B1A765_A71B_11D3_83C8_0000E816586C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <property/property.h>

class CCfgPage : public CDialog  
{
public:
	BOOL m_bRestoreSplitter;
	BOOL m_bSettingsChanged;
	HDATA m_hPatternRoot;

	HDATA m_hCurrCTTData;

	DWORD m_dwType;
	CString m_sHelpFilePath;
	DWORD m_dwHelpID;

	CCfgPage();
	CCfgPage(DWORD dwType, DWORD dwHelpID);
	virtual ~CCfgPage();

	virtual DWORD GetData(PVOID* ppBuffer);
	virtual BOOL SetData(HDATA hData);

	virtual CString GetTitle();
	virtual CString GetDescription();
	virtual HICON GetIcon();
	virtual DWORD GetHelpID();
	virtual CString GetHelpFilePath();
};

#endif // !defined(AFX_CFGPAGE_H__22B1A765_A71B_11D3_83C8_0000E816586C__INCLUDED_)
