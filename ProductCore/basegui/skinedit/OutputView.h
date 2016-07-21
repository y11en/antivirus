#pragma once

#ifndef __RICHEDITVIEW_H__
#define __RICHEDITVIEW_H__

#ifndef __ATLCTRLS_H__
	#error RichEditView.h requires atlctrls.h to be included first
#endif

class DockSplitTab::Frame;

class RichEdit : public CWindowImpl<RichEdit, WTL::CRichEditCtrl>
{
protected:
	ClientViewListener*	clientViewListener;
	CFont				m_font;
	
public:
	// constructor
	RichEdit( ClientViewListener* clientViewListener)
		: clientViewListener( clientViewListener)
	{}
	
	// public interface
	void create( HWND parentWnd)
	{
		Create( parentWnd
		            , rcDefault
		            , NULL
		            , WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
		            | WS_HSCROLL | WS_VSCROLL
		            | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE, WS_EX_CLIENTEDGE);
		
		LOGFONT _lf = {0};
		_lf.lfHeight = -10;
		_lf.lfWeight = FW_NORMAL;
		_tcsncpy(_lf.lfFaceName, _T("Courier"), countof(_lf.lfFaceName));
		
		m_font.CreateFontIndirect(&_lf);
		SetFont(m_font);
		return;
	}

	void Clear()
	{
		SetWindowText(_T(""));
	}
	
	// Message map handlers
	DECLARE_WND_SUPERCLASS(_T("KL.Output"), CRichEditCtrl::GetWndClassName())
	
	BEGIN_MSG_MAP(RichEdit)
		MESSAGE_HANDLER(WM_CLOSE,       OnClose)
	END_MSG_MAP()
	
	// DockFloatSplitTab Framework sends WM_CLOSE message
	LRESULT OnClose( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// do not call default event handler
		return 1;
	}
};
	

#endif // __RICHEDITVIEW_H__
