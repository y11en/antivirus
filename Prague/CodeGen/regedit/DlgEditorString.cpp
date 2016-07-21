//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#include "stdafx.h"
#include "RegEdit.h"
#include "DlgEditorString.h"
#include "RegMetadata.h"

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
DlgEditorString::DlgEditorString(CWnd* pParent /*=NULL*/)
	: CDialog(DlgEditorString::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgEditorString)
	m_strValueName = _T("");
	//}}AFX_DATA_INIT

	m_dwInnerBufLen = 0;
	m_pInnerBuf = NULL;
}

DlgEditorString::~DlgEditorString()
{
	DeallocateInnerBuf();
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void DlgEditorString::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgEditorString)
	DDX_Text(pDX, IDC_VALUE_NAME, m_strValueName);
	//}}AFX_DATA_MAP

	UpdateValue(pDX->m_bSaveAndValidate);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void DlgEditorString::OnOK()
{
	CDialog::OnOK();

	if(	m_dwInnerBufLen != m_dwSize || 
		0 != memcmp(m_pBuf, m_pInnerBuf, m_dwSize))
	{
		m_bModified = TRUE;

		m_dwSize = 4094;
		if(m_dwInnerBufLen > m_dwSize)
			m_dwInnerBufLen = m_dwSize;

		CopyMemory(m_pBuf, m_pInnerBuf, m_dwInnerBufLen);
		m_dwSize = m_dwInnerBufLen;
		m_pBuf[m_dwSize] = 
		m_pBuf[m_dwSize+1] = 0;
	}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void DlgEditorString::UpdateValue(BOOL i_bSave)
{
	HWND hWndCtrl;
	GetDlgItem(IDC_VALUE_DATA, &hWndCtrl);

	if (i_bSave)
	{
		int nLen = ::GetWindowTextLength(hWndCtrl);
		DWORD dwBufLen = nLen;

		if(MD::IsTypeUnicode(m_nType))
			dwBufLen *= 2;

		AllocateInnerBuf(NULL, dwBufLen);

		if(MD::IsTypeUnicode(m_nType))
			::GetWindowTextW(hWndCtrl, (LPWSTR) m_pInnerBuf, nLen+1);
		else
			::GetWindowTextA(hWndCtrl, (LPSTR) m_pInnerBuf, nLen+1);
	}
	else
	{
		if(MD::IsTypeUnicode(m_nType))
			::SetWindowTextW(hWndCtrl, (LPCWSTR)m_pInnerBuf);
		else
			::SetWindowTextA(hWndCtrl, (LPCSTR)m_pInnerBuf);
	}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void DlgEditorString::AllocateInnerBuf(BYTE* i_pBufSrc, DWORD i_dwLen)
{
	if( i_dwLen > m_dwInnerBufLen )
		DeallocateInnerBuf();

	if(NULL == m_pInnerBuf)
	{
		m_dwInnerBufLen = i_dwLen;
		m_pInnerBuf = new BYTE[m_dwInnerBufLen + 2];
	}

	if(i_pBufSrc)
	{
		CopyMemory(m_pInnerBuf, i_pBufSrc, i_dwLen);
		m_pInnerBuf[i_dwLen] = 
		m_pInnerBuf[i_dwLen+1] = 0;
	}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void DlgEditorString::DeallocateInnerBuf()
{
	if(m_pInnerBuf)
		delete m_pInnerBuf;
	m_pInnerBuf = NULL;
	m_dwInnerBufLen  = 0;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BEGIN_MESSAGE_MAP(DlgEditorString, CDialog)
	//{{AFX_MSG_MAP(DlgEditorString)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// DlgEditorString message handlers

BOOL DlgEditorString::OnInitDialog() 
{
	ASSERT(NULL != m_pBuf);

	m_bModified = FALSE;
	AllocateInnerBuf(m_pBuf, m_dwSize);

	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
