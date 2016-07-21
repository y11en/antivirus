// AddMaskDlg.cpp : implementation file
//

#include "stdafx.h"
#include "gui.h"
#include "AddMaskDlg.h"


// CAddMaskDlg dialog

IMPLEMENT_DYNAMIC(CAddMaskDlg, CDialog)

CAddMaskDlg::CAddMaskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddMaskDlg::IDD, pParent)
{
	pMask = NULL;
	pValueName = NULL;
}

CAddMaskDlg::~CAddMaskDlg()
{
	if (pMask)
		delete [] pMask;

	if (pValueName)
		delete [] pValueName;
}

void CAddMaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAddMaskDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAddMaskDlg::OnBnClickedOk)
//	ON_EN_CHANGE(IDC_EDIT1, &CAddMaskDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CAddMaskDlg message handlers

void CAddMaskDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	
	int maskSize=((CEdit*)GetDlgItem(IDC_EDIT1))->GetWindowTextLength();
	if (maskSize)
		pMask = (PWCHAR) new char [(maskSize+1)*sizeof(WCHAR)];

	if (pMask)
	{
		((CEdit*)GetDlgItem(IDC_EDIT1))->GetWindowText( pMask, maskSize+1 );
	}


	int valueSize=((CEdit*)GetDlgItem(IDC_EDIT2))->GetWindowTextLength();
	if (valueSize)
		pValueName = (PWCHAR) new char [(valueSize+1)*sizeof(WCHAR)];

	if (pValueName)
	{
		((CEdit*)GetDlgItem(IDC_EDIT2))->GetWindowText( pValueName, valueSize+1 );
	}

	OnOK();
}

//void CAddMaskDlg::OnEnChangeEdit1()
//{
//	// TODO:  If this is a RICHEDIT control, the control will not
//	// send this notification unless you override the CDialog::OnInitDialog()
//	// function and call CRichEditCtrl().SetEventMask()
//	// with the ENM_CHANGE flag ORed into the mask.
//
//	// TODO:  Add your control notification handler code here
//}
