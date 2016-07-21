// CfgPage.cpp: implementation of the CCfgPage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CfgPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCfgPage::CCfgPage()
{
	m_dwType = 0;
	m_dwHelpID = 0;
	m_bSettingsChanged = FALSE;
	m_bRestoreSplitter = TRUE;
	m_hPatternRoot = NULL;

	m_hCurrCTTData = NULL;
}

CCfgPage::CCfgPage(DWORD dwType, DWORD dwHelpID)
{
	m_dwType = dwType;
	m_dwHelpID = dwHelpID;
	m_bSettingsChanged = FALSE;
	m_bRestoreSplitter = TRUE;
	m_hPatternRoot = NULL;

	m_hCurrCTTData = NULL;
}

CCfgPage::~CCfgPage()
{
	if (m_hCurrCTTData)	::DATA_Remove(m_hCurrCTTData, NULL);
}

DWORD CCfgPage::GetData(PVOID* ppBuffer)
{
	return 0;
}

BOOL CCfgPage::SetData(HDATA hData)
{
	return FALSE;
}

CString CCfgPage::GetHelpFilePath()
{
	return AfxGetApp()->m_pszHelpFilePath;
}

DWORD CCfgPage::GetHelpID()
{
	return m_dwHelpID;
}

HICON CCfgPage::GetIcon()
{
	return NULL;
}

CString CCfgPage::GetDescription()
{
	return "";
}

CString CCfgPage::GetTitle()
{
	return "";
}
