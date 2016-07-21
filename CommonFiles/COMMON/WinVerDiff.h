// WinVerDiff.h: interface for the CWinVerDiff class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINVERDIFF_H__15B6C673_B6B2_4125_AA70_0828F163F824__INCLUDED_)
#define AFX_WINVERDIFF_H__15B6C673_B6B2_4125_AA70_0828F163F824__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if (_WIN32_WINNT < 0x501)

	#define ECM_FIRST               0x1500      // Edit control messages
	#define BCM_FIRST               0x1600      // Button control messages
	#define CBM_FIRST               0x1700      // Combobox control messages

	#define	EM_SETCUEBANNER	    (ECM_FIRST + 1)		// Set the cue banner with the lParm = LPCWSTR
	#define Edit_SetCueBannerText(hwnd, lpcwText) \
	(BOOL)SNDMSG((hwnd), EM_SETCUEBANNER, 0, (LPARAM)(lpcwText))

	#define	EM_GETCUEBANNER	    (ECM_FIRST + 2)		// Set the cue banner with the lParm = LPCWSTR

	#define Edit_GetCueBannerText(hwnd, lpwText, cchText) \
	(BOOL)SNDMSG((hwnd), EM_GETCUEBANNER, (WPARAM)(lpwText), (LPARAM)(cchText))

	typedef struct _tagEDITBALLOONTIP
	{
		DWORD   cbStruct;
		LPCWSTR pszTitle;
		LPCWSTR pszText;
		INT     ttiIcon; // From TTI_*
	} EDITBALLOONTIP, *PEDITBALLOONTIP;

	#define	EM_SHOWBALLOONTIP   (ECM_FIRST + 3)		// Show a balloon tip associated to the edit control

	#define Edit_ShowBalloonTip(hwnd, peditballoontip) \
	(BOOL)SNDMSG((hwnd), EM_SHOWBALLOONTIP, 0, (LPARAM)(peditballoontip))

	#define EM_HIDEBALLOONTIP   (ECM_FIRST + 4)     // Hide any balloon tip associated with the edit control

	#define Edit_HideBalloonTip(hwnd) \
	(BOOL)SNDMSG((hwnd), EM_HIDEBALLOONTIP, 0, 0)

	#define HDF_SORTUP              0x0400
	#define HDF_SORTDOWN            0x0200

	#define WM_THEMECHANGED                 0x031A
	

	#define BUTTON_IMAGELIST_ALIGN_LEFT     0
	#define BUTTON_IMAGELIST_ALIGN_RIGHT    1
	#define BUTTON_IMAGELIST_ALIGN_TOP      2
	#define BUTTON_IMAGELIST_ALIGN_BOTTOM   3
	#define BUTTON_IMAGELIST_ALIGN_CENTER   4       // Doesn't draw text
	
	typedef struct
	{
		HIMAGELIST  himl;   // Index: Normal, hot pushed, disabled. If count is less than 4, we use index 1
		RECT        margin; // Margin around icon.
		UINT        uAlign;
	} BUTTON_IMAGELIST, *PBUTTON_IMAGELIST;
	
	#define BCM_SETIMAGELIST        (BCM_FIRST + 0x0002)
	#define Button_SetImageList(hwnd, pbuttonImagelist)\
		(BOOL)SNDMSG((hwnd), BCM_SETIMAGELIST, 0, (LPARAM)(pbuttonImagelist))

	#define IDC_HAND            MAKEINTRESOURCE(32649)

	/*
	* Combobox information
	*/
	typedef struct tagCOMBOBOXINFO
	{
		DWORD cbSize;
		RECT  rcItem;
		RECT  rcButton;
		DWORD stateButton;
		HWND  hwndCombo;
		HWND  hwndItem;
		HWND  hwndList;
	} COMBOBOXINFO, *PCOMBOBOXINFO, *LPCOMBOBOXINFO;

	extern "C"
	BOOL WINAPI GetComboBoxInfo(HWND hwndCombo, PCOMBOBOXINFO pcbi);

	#define MB_CANCELTRYCONTINUE        0x00000006L

#endif //_WIN32_WINNT < 0x501

#endif // !defined(AFX_WINVERDIFF_H__15B6C673_B6B2_4125_AA70_0828F163F824__INCLUDED_)
