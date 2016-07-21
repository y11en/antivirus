#ifndef CodePages_h_included
#define CodePages_h_included

class CCodePages
{
	struct SCodePage{ int m_CPId; LPCTSTR m_strCPName; };
	static SCodePage s_oCodePages[];
	int		m_nNextCodePage;

public:
	CCodePages();
	CString GetFirstCodePage();
	CString GetNextCodePage();
	CString GetCodePageById(int i_nCodePageId);
	int		GetCodePageByName(LPCTSTR i_cstrCPName);
};


#endif // CodePages_h_included