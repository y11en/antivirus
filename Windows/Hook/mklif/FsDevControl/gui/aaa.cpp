// aaa.cpp : implementation file
//

#include "stdafx.h"
#include "gui.h"
#include "aaa.h"


// aaa dialog

IMPLEMENT_DYNAMIC(aaa, CDialog)

aaa::aaa(CWnd* pParent /*=NULL*/)
	: CDialog(aaa::IDD, pParent)
{

}

aaa::~aaa()
{
}

void aaa::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(aaa, CDialog)
END_MESSAGE_MAP()


// aaa message handlers
