// ExpandButton.cpp : implementation file
//

#include "stdafx.h"
#include "common.h"
#include "ExpandButton.h"
#include "WinVerDiff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExpandButton

CExpandButton::CExpandButton() :
	m_bExpand(true),
	m_hiMore(NULL),
	m_hiLess(NULL)
{
	m_bIsThemed = CCXPThemeHelper::IsCurrentThemed();
}

CExpandButton::~CExpandButton()
{
	if (m_hiMore)
		DestroyIcon(m_hiMore);
	if (m_hiLess)
		DestroyIcon(m_hiLess);
}


BEGIN_MESSAGE_MAP(CExpandButton, CButton)
	//{{AFX_MSG_MAP(CExpandButton)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExpandButton message handlers

void CExpandButton::PreSubclassWindow() 
{
	ModifyStyle(0, BS_ICON);
	Initialize();	
	CButton::PreSubclassWindow();
}

int CExpandButton::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	lpCreateStruct->style |= BS_ICON;

	Initialize();

	return 0;
}

void CExpandButton::Initialize()
{
	if (m_hiMore == NULL)
	{
		m_hiMore = (HICON)LoadImage(
			AfxFindResourceHandle(MAKEINTRESOURCE(IDI_COMMON_MORE_INFO_ARROWS), RT_GROUP_ICON),
			MAKEINTRESOURCE(IDI_COMMON_MORE_INFO_ARROWS),
			IMAGE_ICON,
			0, 0, LR_DEFAULTCOLOR);
		VERIFY(m_hiMore != NULL);
	}
	if (m_hiLess == NULL)
	{
		m_hiLess = (HICON)LoadImage(
			AfxFindResourceHandle(MAKEINTRESOURCE(IDI_COMMON_FEWER_INFO_ARROWS), RT_GROUP_ICON),
			MAKEINTRESOURCE(IDI_COMMON_FEWER_INFO_ARROWS),
			IMAGE_ICON,
			0, 0, LR_DEFAULTCOLOR);
		VERIFY(m_hiLess != NULL);
	}
	
	if(m_bIsThemed)
	{
		if((HIMAGELIST)m_ilMore == NULL)
		{
			VERIFY(m_ilMore.Create(13, 12, ILC_COLOR4|ILC_MASK, 0, 1));
			m_ilMore.Add(m_hiMore);
		}
		if((HIMAGELIST)m_ilLess == NULL)
		{
			VERIFY(m_ilLess.Create(13, 12, ILC_COLOR4|ILC_MASK, 0, 1));
			m_ilLess.Add(m_hiLess);
		}

		BUTTON_IMAGELIST btn_img;
		btn_img.himl = m_bExpand ? (HIMAGELIST)m_ilMore : (HIMAGELIST)m_ilLess;
		btn_img.margin = CRect(0, 0, 0, 0);
		btn_img.uAlign = BUTTON_IMAGELIST_ALIGN_CENTER;

		Button_SetImageList(GetSafeHwnd(), &btn_img);
	}
	else
	{
		SetIcon(m_bExpand ? m_hiMore : m_hiLess);
	}
}

void CExpandButton::SetExpandState(bool bExpand)
{
	m_bExpand = bExpand;
	Initialize();
}
