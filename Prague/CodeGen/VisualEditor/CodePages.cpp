#include "stdafx.h"
#include "VisualEditorDlg.h"
#include "CodePages.h"


//	ids for code pages declared in:
//	prague\include\pr_types.h

CCodePages::SCodePage CCodePages::s_oCodePages[] = 
{
	{ cCP_OEM, _T("CP_OEM") },
	{ cCP_ANSI, _T("CP_ANSI") },
	{ cCP_IBM, _T("CP_IBM") },
	{ cCP_UTF8, _T("CP_UTF8") },
	{ cCP_UNICODE, _T("CP_UNICODE") },
	{ cCP_UNICODE_BIGENDIAN, _T("CP_UNICODE_BIGENDIAN") },
};
			     
CCodePages::CCodePages()
{
}

CString CCodePages::GetFirstCodePage()
{
	m_nNextCodePage = 0;
	return GetNextCodePage();
}

CString CCodePages::GetNextCodePage()
{
	int nCodePagesNumber = sizeof(s_oCodePages) / sizeof(s_oCodePages[0]);
	if(m_nNextCodePage < nCodePagesNumber)
		return s_oCodePages[m_nNextCodePage ++].m_strCPName;

	++m_nNextCodePage;

	return _T("");
}

CString CCodePages::GetCodePageById(int i_nCodePageId)
{
	int nCodePagesNumber = sizeof(s_oCodePages) / sizeof(s_oCodePages[0]);
	for(int i=0; i<nCodePagesNumber; i++)
		if(s_oCodePages[i].m_CPId == i_nCodePageId)
			return s_oCodePages[i].m_strCPName;

	return _T("CP_UNKNOWN");	
}

int	CCodePages::GetCodePageByName(LPCTSTR i_cstrCPName)
{
	int nCodePagesNumber = sizeof(s_oCodePages) / sizeof(s_oCodePages[0]);
	for(int i=0; i<nCodePagesNumber; i++)
		if(s_oCodePages[i].m_strCPName == CString(i_cstrCPName))
			return s_oCodePages[i].m_CPId;
		
	return cCP_NULL;	
}


