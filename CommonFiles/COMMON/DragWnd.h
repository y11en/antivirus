// DragWnd.h: interface for the CDragWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAGWND_H__09B6EC9C_9924_4253_888E_6A0BD7C72CFD__INCLUDED_)
#define AFX_DRAGWND_H__09B6EC9C_9924_4253_888E_6A0BD7C72CFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define POINT_TO_LPARAM(pt)	MAKELONG((short)pt.x, (short)pt.y)

inline LRESULT LButtonDragWnd ( HWND hwnd, LPARAM lParam)
{
	HWND hPopupWnd = hwnd;
	while ( hPopupWnd && !( ::GetWindowLong  ( hPopupWnd, GWL_STYLE ) & WS_POPUP ) )
		hPopupWnd = ::GetParent  (hPopupWnd);

	if  (hPopupWnd)
	{ 
		::PostMessage ( hPopupWnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam );
		::SetFocus(hwnd);
		return 0;
	}

	return 1;
}

//******************************************************************************
//
//******************************************************************************
template <class _Wnd> class CDragWnd  : public _Wnd
{
public:
	virtual ~CDragWnd ()
    {
    }

protected:
	virtual LRESULT WindowProc ( UINT message, WPARAM wParam, LPARAM lParam )
	{
		if  ( ( message == WM_LBUTTONDOWN ) && ( LButtonDragWnd ( GetSafeHwnd (), lParam ) == 0 ) )
        {
            return 0;
        }
		return _Wnd::WindowProc ( message, wParam, lParam );
	}

};


#endif // !defined(AFX_DRAGWND_H__09B6EC9C_9924_4253_888E_6A0BD7C72CFD__INCLUDED_)
