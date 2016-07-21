// DInfoForUser.cpp : implementation file
//

#include "stdafx.h"
#include "iguard.h"
#include "DInfoForUser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDInfoForUser dialog


CDInfoForUser::CDInfoForUser(CWnd* pParent /*=NULL*/)
	: CDialog(CDInfoForUser::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDInfoForUser)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDInfoForUser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDInfoForUser)
	DDX_Control(pDX, IDC_EDIT_ADDITIONAL_INFO, m_eAdditionalInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDInfoForUser, CDialog)
	//{{AFX_MSG_MAP(CDInfoForUser)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDInfoForUser message handlers

void CDInfoForUser::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (IsWindow(m_eAdditionalInfo.m_hWnd))
	{
		static RULE rules[] = {
			// Action    Act-on							Relative-to				Offset
			// ------    ------							-----------				------
			{lSTRETCH,	lRIGHT(IDC_EDIT_ADDITIONAL_INFO),		lRIGHT(lPARENT)			, -1},
			{lSTRETCH,	lBOTTOM(IDC_EDIT_ADDITIONAL_INFO),		lBOTTOM(lPARENT)			, -1},
			{lEND}
		};
		Layout_ComputeLayout(m_hWnd, rules);	
	}			
}

BOOL CDInfoForUser::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString strtmp;

	strtmp = "\tИнформация о программе и как с ней работать.\r\n\r\nНо сначала основной совет: НИКОГДА не ставьте запрещающие фильтры если не уверены что знаете что делаете.\r\n";
		 
	strtmp += "Comment: Приложение хранит фильтры в системе и будет их выполнять даже в отсутствии интерфейсной части. Так что при автостарте\r\n";
	strtmp += "	 получите уже работающие фильтры. Поэтому будьте аккуратны...\r\n";
		 
	strtmp += "\r\nПростой фильтр - выбираешь что хочется перехватить. Затем выбираем что с этим делать: Popup - остановить поток, спросить у пользователя и после ответа отпустить поток. Info - поток не останавливается но к пользователю выдают такой же запрос как и у Popup (работает кэш с dirty значениями)\r\n";
	strtmp += "Touch filter - в комбинации с Info и Popup показывают информацию о событии.\r\n";
	
	strtmp += "\r\n";
	strtmp += "\r\nPS. Перехватчики (Packets) будут работаь если установлены соответствующие драйвера\r\n";
	strtmp += "\r\nPPS. Для технических вопросов пишите на Sobko@kasperksy.com\r\nПо комерческим вопросам в Sales Dept (см. информацию в закладке Info)";

	m_eAdditionalInfo.SetWindowText(strtmp);

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDInfoForUser::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if((LOWORD(wParam) == IDCANCEL) || (LOWORD(wParam) == IDOK))
		return TRUE;
	
	return CDialog::OnCommand(wParam, lParam);
}
