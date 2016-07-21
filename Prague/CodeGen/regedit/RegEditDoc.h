//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifndef AFX_REGEDITDOC_H__AC3509DF_36FB_4E4A_A525_31B058FB743F__INCLUDED_
#define AFX_REGEDITDOC_H__AC3509DF_36FB_4E4A_A525_31B058FB743F__INCLUDED_

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

struct cRegistry;

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

class CRegEditDoc : public CDocument
{
protected: 
	CRegEditDoc();
	DECLARE_DYNCREATE(CRegEditDoc)

public:
	cRegistry*	m_pReg;
	CString		m_strRegKey;
	BOOL		m_bWinReg;
	BOOL		m_bReadOnly;

	//{{AFX_VIRTUAL(CRegEditDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	void OnNewKey();
	protected:
	virtual BOOL SaveModified();
	virtual void SetTitle( LPCTSTR title );
	//}}AFX_VIRTUAL

	virtual ~CRegEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL SetSaveOnCloseProp();

	//{{AFX_MSG(CRegEditDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

const LPARAM UPD_HINT_SHOW_THE_KEY=1;

//{{AFX_INSERT_LOCATION}}
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#endif 
