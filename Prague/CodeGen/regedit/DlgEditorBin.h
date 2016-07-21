//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifndef AFX_DLGEDITORBIN_H__941892CD_F267_4232_8676_86D514D63ED9__INCLUDED_
#define AFX_DLGEDITORBIN_H__941892CD_F267_4232_8676_86D514D63ED9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

class DlgEditorBin : public CDialog
{
public:
	DlgEditorBin(CWnd* pParent = NULL);  

	//{{AFX_DATA(DlgEditorBin)
	enum { IDD = IDD_EDITOR_BINARY };
	//}}AFX_DATA


	//{{AFX_VIRTUAL(DlgEditorBin)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);  
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(DlgEditorBin)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#endif 
