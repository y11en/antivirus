//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#include "stdafx.h"
#include "RegEdit.h"
#include "DlgEditorInt.h"
#include "RegMetadata.h"

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif												  

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
CDlgEditorInt::CDlgEditorInt(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditorInt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEditorInt)
	m_strValueName = _T("");
	//}}AFX_DATA_INIT

	m_pBuf = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// Notes for implementing dialog data exchange and validation procs:
//  * always start with PrepareCtrl or PrepareEditCtrl
//  * always start with 'pDX->m_bSaveAndValidate' check
//  * pDX->Fail() will throw an exception - so be prepared
//  * avoid creating temporary HWNDs for dialog controls - i.e.
//      use HWNDs for child elements
//  * validation procs should only act if 'm_bSaveAndValidate'
//  * use the suffices:
//      DDX_ = exchange proc
//      DDV_ = validation proc
//
/////////////////////////////////////////////////////////////////////////////

// only supports '%d', '%u', '%sd', '%su', '%ld' and '%lu'
AFX_STATIC BOOL AFXAPI _AfxSimpleScanfHex(LPCTSTR lpszText,
	LPCTSTR lpszFormat, va_list pData)
{
	ASSERT(lpszText != NULL);
	ASSERT(lpszFormat != NULL);

	ASSERT(*lpszFormat == '%');
	lpszFormat++;        // skip '%'

	BOOL bLong = FALSE;
	BOOL bShort = FALSE;
	if (*lpszFormat == 'l')
	{
		bLong = TRUE;
		lpszFormat++;
	}
	else if (*lpszFormat == 's')
	{
		bShort = TRUE;
		lpszFormat++;
	}

	ASSERT(*lpszFormat == 'd' || *lpszFormat == 'u');
	ASSERT(lpszFormat[1] == '\0');

	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;
	TCHAR chFirst = lpszText[0];
	long l, l2;
	if (*lpszFormat == 'd')
	{
		// signed
		l = _tcstol(lpszText, (LPTSTR*)&lpszText, 16);
		l2 = (int)l;
	}
	else
	{
		// unsigned
		if (*lpszText == '-')
			return FALSE;
		l = (long)_tcstoul(lpszText, (LPTSTR*)&lpszText, 16);
		l2 = (unsigned int)l;
	}
	if (l == 0 && chFirst != '0')
		return FALSE;   // could not convert

	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;
	if (*lpszText != '\0')
		return FALSE;   // not terminated properly

	if (bShort)
	{
		if ((short)l != l)
			return FALSE;   // too big for short
		*va_arg(pData, short*) = (short)l;
	}
	else
	{
		ASSERT(sizeof(long) == sizeof(int));
		ASSERT(l == l2);
		*va_arg(pData, long*) = l;
	}

	// all ok
	return TRUE;
}

AFX_STATIC void AFX_CDECL _Afx_DDX_TextWithFormatHex(CDataExchange* pDX, int nIDC,
	LPCTSTR lpszFormat, UINT nIDPrompt, ...)
	// only supports windows output formats - no floating point
{
	va_list pData;
	va_start(pData, nIDPrompt);

	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	TCHAR szT[32];
	if (pDX->m_bSaveAndValidate)
	{
		// the following works for %d, %u, %ld, %lu
		::GetWindowText(hWndCtrl, szT, 32);
		if (!_AfxSimpleScanfHex(szT, lpszFormat, pData))
		{
			AfxMessageBox(nIDPrompt);
			pDX->Fail();        // throws exception
		}
	}
	else
	{
		wvsprintf(szT, lpszFormat, pData);
			// does not support floating point numbers - see dlgfloat.cpp
		AfxSetWindowText(hWndCtrl, szT);
	}

	va_end(pData);
}

/////////////////////////////////////////////////////////////////////////////
// Simple formatting to text item

void AFXAPI DDX_TextHex(CDataExchange* pDX, int nIDC, BYTE& value)
{
	int n = (int)value;
	if (pDX->m_bSaveAndValidate)
	{
		_Afx_DDX_TextWithFormatHex(pDX, nIDC, _T("%u"), AFX_IDP_PARSE_BYTE, &n);
		if (n > 255)
		{
			AfxMessageBox(AFX_IDP_PARSE_BYTE);
			pDX->Fail();        // throws exception
		}
		value = (BYTE)n;
	}
	else
		_Afx_DDX_TextWithFormatHex(pDX, nIDC, _T("0x%08x"), AFX_IDP_PARSE_BYTE, n);
}

void AFXAPI DDX_TextHex(CDataExchange* pDX, int nIDC, short& value)
{
	if (pDX->m_bSaveAndValidate)
		_Afx_DDX_TextWithFormatHex(pDX, nIDC, _T("%sd"), AFX_IDP_PARSE_INT, &value);
	else
		_Afx_DDX_TextWithFormatHex(pDX, nIDC, _T("0x%08x"), AFX_IDP_PARSE_INT, value);
}

void AFXAPI DDX_TextHex(CDataExchange* pDX, int nIDC, int& value)
{
	if (pDX->m_bSaveAndValidate)
		_Afx_DDX_TextWithFormatHex(pDX, nIDC, _T("%d"), AFX_IDP_PARSE_INT, &value);
	else
		_Afx_DDX_TextWithFormatHex(pDX, nIDC, _T("0x%08x"), AFX_IDP_PARSE_INT, value);
}

void AFXAPI DDX_TextHex(CDataExchange* pDX, int nIDC, UINT& value)
{
	if (pDX->m_bSaveAndValidate)
		_Afx_DDX_TextWithFormatHex(pDX, nIDC, _T("%u"), AFX_IDP_PARSE_UINT, &value);
	else
		_Afx_DDX_TextWithFormatHex(pDX, nIDC, _T("0x%08x"), AFX_IDP_PARSE_UINT, value);
}

void AFXAPI DDX_TextHex(CDataExchange* pDX, int nIDC, long& value)
{
	if (pDX->m_bSaveAndValidate)
		_Afx_DDX_TextWithFormatHex(pDX, nIDC, _T("%ld"), AFX_IDP_PARSE_INT, &value);
	else
		_Afx_DDX_TextWithFormatHex(pDX, nIDC, _T("0x%08x"), AFX_IDP_PARSE_INT, value);
}

void AFXAPI DDX_TextHex(CDataExchange* pDX, int nIDC, DWORD& value)
{
	if (pDX->m_bSaveAndValidate)
		_Afx_DDX_TextWithFormatHex(pDX, nIDC, _T("%lu"), AFX_IDP_PARSE_UINT, &value);
	else
		_Afx_DDX_TextWithFormatHex(pDX, nIDC, _T("0x%08x"), AFX_IDP_PARSE_UINT, value);
}

void CDlgEditorInt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEditorInt)
	DDX_Text(pDX, IDC_VALUE_NAME, m_strValueName);
	//}}AFX_DATA_MAP

	if(m_nDec)
	{
		if(MD::IsTypeEqShort(m_nType))
			DDX_Text(pDX, IDC_VALUE_DATA, *(short*)m_pBuf);
		else
		if(MD::IsTypeEqInt(m_nType))
			DDX_Text(pDX, IDC_VALUE_DATA, *(int*)m_pBuf);
		else
		if(MD::IsTypeEqUINT(m_nType))
			DDX_Text(pDX, IDC_VALUE_DATA, *(UINT*)m_pBuf);
		else
		if(MD::IsTypeEqLong(m_nType))
			DDX_Text(pDX, IDC_VALUE_DATA, *(long*)m_pBuf);
		else
		if(MD::IsTypeEqBYTE(m_nType))
			DDX_Text(pDX, IDC_VALUE_DATA, *(BYTE*)m_pBuf);
		else
		if(MD::IsTypeEqDWORD(m_nType))
			DDX_Text(pDX, IDC_VALUE_DATA, *(DWORD*)m_pBuf);
		else
			AfxMessageBox(_T("This type of value currently could not be editted!"), MB_ICONSTOP);
	}
	else
	{
		if(MD::IsTypeEqShort(m_nType) || m_nType == tid_WORD)
			DDX_TextHex(pDX, IDC_VALUE_DATA, *(short*)m_pBuf);
		else
		if(MD::IsTypeEqInt(m_nType) || m_nType == tid_BOOL)
			DDX_TextHex(pDX, IDC_VALUE_DATA, *(int*)m_pBuf);
		else
		if(MD::IsTypeEqUINT(m_nType))
			DDX_TextHex(pDX, IDC_VALUE_DATA, *(UINT*)m_pBuf);
		else
		if(MD::IsTypeEqLong(m_nType))
			DDX_TextHex(pDX, IDC_VALUE_DATA, *(long*)m_pBuf);
		else
		if(MD::IsTypeEqBYTE(m_nType))
			DDX_TextHex(pDX, IDC_VALUE_DATA, *(BYTE*)m_pBuf);
		else
		if(MD::IsTypeEqDWORD(m_nType))
			DDX_TextHex(pDX, IDC_VALUE_DATA, *(DWORD*)m_pBuf);
		else
			AfxMessageBox(_T("This type of value currently could not be editted!"), MB_ICONSTOP);
	}
	
	// important !!!
	// this call must be after DDX_Text(Hex)
	DDX_Radio(pDX, IDC_HEX, m_nDec);
}

void CDlgEditorInt::OnOK()
{
	CDialog::OnOK();
	if(0 != memcmp(m_pBuf, m_arrBuf, 16))
		m_bModified = TRUE;
}

BEGIN_MESSAGE_MAP(CDlgEditorInt, CDialog)
	//{{AFX_MSG_MAP(CDlgEditorInt)
	ON_BN_CLICKED(IDC_DEC, OnDec)
	ON_BN_CLICKED(IDC_HEX, OnHex)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CDlgEditorInt message handlers

BOOL CDlgEditorInt::OnInitDialog() 
{
	ASSERT(NULL != m_pBuf);

	m_bModified = FALSE;
	CopyMemory(m_arrBuf, m_pBuf, 16);
	m_nDec = 0;

	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditorInt::OnDec() 
{
	UpdateData();
	m_nDec = 1;
	UpdateData(FALSE);
}

void CDlgEditorInt::OnHex() 
{
	UpdateData();
	m_nDec = 0;
	UpdateData(FALSE);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

