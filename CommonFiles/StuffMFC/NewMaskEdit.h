#ifndef SCRIPTPRO_COMMON_MASKED_EDIT_CLASS
#define SCRIPTPRO_COMMON_MASKED_EDIT_CLASS


class SPMaskEdit : public CEdit
{
public:
	SPMaskEdit();
	virtual ~SPMaskEdit();

	void	SetWindowText(LPCTSTR lpszString);

	void	setText(LPCTSTR pszText);
	CString	getText();

	void	setMask(LPCTSTR pszMask, char cPlaceHolder = '_');
	CString	getMask();

	//{{AFX_VIRTUAL(SPMaskEdit)
	//}}AFX_VIRTUAL

protected:

	char	m_cPlaceHolder;
	
	CString m_strMaskText;

	CString m_strSaveText;

	bool	m_bLastKeyWasDelete;
	bool	m_bLastKeyWasBackspace;

	bool	isMaskChar(int nPos);

	int		gotoNextEntryChar(int nPos);

	bool	getReplacementChar(int nPos, char cWant, char& cGet);

	//{{AFX_MSG(SPMaskEdit)
	afx_msg void OnUpdate();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

inline CString SPMaskEdit::getMask()
{
	return m_strMaskText;
}

#endif
