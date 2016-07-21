// TEdit.cpp : implementation file
//

#include "stdafx.h"
#include "AvpEdit.h"
#include "TEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplateEdit

CTemplateEdit::CTemplateEdit()
{
	FormatString=_T("");
}

CTemplateEdit::~CTemplateEdit()
{
}


BEGIN_MESSAGE_MAP(CTemplateEdit, CEdit)
	//{{AFX_MSG_MAP(CTemplateEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateEdit message handlers


BOOL CTemplateEdit::SubclassEdit(UINT nID, CWnd* pParent, char* formatString)
{
	if(formatString)FormatString=formatString;
	BOOL ret=SubclassDlgItem(nID, pParent);
	LimitText(FormatString.GetLength());
	return ret;
}

void CTemplateEdit::OnBadInput()
{
		MessageBeep(MB_OK);
}


void CTemplateEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	Sel=GetSel();
	GetWindowText(Text);
	
	if( (Sel & 0x0000FFFF) == (Sel >> 16) )	SetSel(Sel+0x00010000);

	CEdit::OnChar(nChar, nRepCnt, nFlags);

	CString s;
	GetWindowText(s);

	if(CheckString(s)){
		Sel=GetSel();
		SetWindowText(s);
		SetSel(Sel);
	}
	else{
		SetWindowText(Text);
		SetSel(Sel);
		OnBadInput();
	}
}

void CTemplateEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar==VK_DELETE){
		Sel=GetSel();
		GetWindowText(Text);
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTemplateEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CEdit::OnKeyUp(nChar, nRepCnt, nFlags);

	if(nChar==VK_DELETE){
		CString s;
		GetWindowText(s);
		if(!CheckString(s)){
			SetWindowText(Text);
			SetSel(Sel);
			OnBadInput();
		}	
	}
}


BOOL CTemplateEdit::CheckString(CString& s)
{
	int ls=s.GetLength();
	int lf=FormatString.GetLength();

	for(int i=0;i<ls;i++)
	{
		if(ls>lf)return FALSE;

		switch(FormatString[i]){
		case '-':
			if(s[i]=='+' || s[i]=='-')continue;
			else lf--;

		case 'H':s.MakeUpper();
		case 'h':
			if((s[i]>='A' && s[i]<='F') || (s[i]>='a' && s[i]<='f'))continue;
		case 'D':
		case 'd':
			if(IsCharAlpha((TCHAR)s[i]))return FALSE;
			if(IsCharAlphaNumeric((TCHAR)s[i]))continue;
			return FALSE;
		case '+':
			if(s[i]=='+' || s[i]=='-')continue;
			return FALSE;
		case ' ':
			if(s[i]==' ')continue;
			return FALSE;

		case 'S':
		case 's':
			s.SetAt(i,'_');
			continue;


		default:
			continue;
		}
	}
	return TRUE;

}


void CTemplateEdit::OnEnable(BOOL bEnable) 
{
	CEdit::OnEnable(bEnable);

	Check();
	// TODO: Add your message handler code here
	
}




void CTemplateEdit::Check()
{
	CString s;
	GetWindowText(s);

	if(CheckString(s))
		SetWindowText(s);

}
