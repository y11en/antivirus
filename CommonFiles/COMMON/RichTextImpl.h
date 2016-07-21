// RichTextImpl.h: interface for the CRichTextImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RICHTEXTIMPL_H__02FFCF37_91B9_4A34_9072_57C247654A0F__INCLUDED_)
#define AFX_RICHTEXTIMPL_H__02FFCF37_91B9_4A34_9072_57C247654A0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Map>
#include <list>
#include <vector>
#include <RichEdit.h>
#include "tracktooltip.h"


/*#define _PARSE_ERROR_IF ( exp, text, arg1  ) \
do {\
KLSTD_USES_CONVERSION;\
if (exp) \
KLSTD::Trace(2, L"RichText", KLSTD_A2W(text), arg1);\
}while(0)
*/    
#ifdef _DEBUG
#define _RPT1_ASSERT_IF(expr, msg, arg1) \
	do { if ((expr) && \
	(1 == _CrtDbgReport(_CRT_ASSERT, NULL, 0, NULL, msg, arg1))) \
_CrtDbgBreak(); } while (0)
#else
#define _RPT1_ASSERT_IF(expr, msg, arg1)
#endif //_DEBUG

#define TOKEN_TYPE				0x0001
#define TOKEN_COLOR				0x0002
#define TOKEN_ID				0x0004
#define TOKEN_TEXT				0x0008
#define TOKEN_COMPRESS			0x0010
#define TOKEN_LOCKED			0x0020



#define RT_GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define RT_GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

class CRichTextImpl  
{
public:
	explicit CRichTextImpl(HWND hWnd);
	virtual ~CRichTextImpl();
	
protected:
	
	struct charrange_less : std::binary_function<CHARRANGE, CHARRANGE, bool>
	{
		bool operator()(const CHARRANGE& _X, const CHARRANGE& _Y) const
		{
			return ((_X.cpMin < _Y.cpMin) && (_X.cpMax < _Y.cpMax)); 
		}
	};
	
	typedef std::map < CHARRANGE, DWORD, charrange_less > RTF_LNK_MAP;
	typedef std::map < CHARRANGE, LPCTSTR, charrange_less > RTF_TOOLTIP_MAP;
	typedef std::map < CHARRANGE, TOOLINFO, charrange_less > RTF_CREATED_TOOLTIP_MAP;
	typedef std::map < CHARRANGE, RECT, charrange_less > RTF_MAPPING_CR2CRECT_MAP;
	
	
	
	struct TOKEN
	{
		
		TOKEN () : type(reg), color(0), id(0), mask(0), start_text(NULL), end_text(NULL) {}
		
		DWORD					mask;
		enum{link, bold, reg, comment}	type;
		LPTSTR					start_text;
		LPTSTR					end_text;
		DWORD					color;
		DWORD					id;
	};

	struct TOKEN_EX
	{
		TOKEN	token;
		CHARRANGE cr;
	};
	
    typedef std::list < TOKEN_EX > RTF_TOKEN_LIST;
    
protected:
	
	
	static LRESULT CALLBACK WndProc(
		HWND hwnd,      // handle to window
		UINT uMsg,      // message identifier
		WPARAM wParam,  // first message parameter
		LPARAM lParam   // second message parameter
		);
	
	void CreateTooltip (LPCTSTR szText, const CHARRANGE& cr, RECT& tip_rect);
	
	
	//return true to halt further processing, false proccess further
	static bool SetCtrlCursor (HWND hwnd);
	void AddToolTip(LPCTSTR szToolTipText, CHARRANGE& cr);
	bool CalcTextRanges(const CHARRANGE& cr, RTF_MAPPING_CR2CRECT_MAP& cr2rect);
	void CalcSingleLineRect(CHARRANGE& cr, RECT& rect);
	bool GetLineRangeByPos(LONG pos, CHARRANGE& cr);
	static bool get_next_token  ( IN OUT LPTSTR & start, OUT TOKEN & token );
	void attach_rtf_text ( IN const HWND hWnd, IN const TOKEN& token );
	void colorize ( HWND hWnd );
	void attach_selected( HWND hWnd, LPCTSTR text, CHARRANGE & crange );
	DWORD GetClickedLink ( ENLINK * pENLink );
	bool IsOverLink (HWND hwndRichCtrl, const POINT & pt, CHARRANGE * pCr = NULL, DWORD* pdwLinkID = NULL);
	bool IsOverTooltipArea( HWND hwnd, const POINT &pt, LPCTSTR& pszToolTipText, CHARRANGE& char_range );
	bool IsToolTipCreated(CHARRANGE& cr);
	void clear();
	void clear_tooltip();
	static bool parse_spec_word ( IN OUT LPTSTR & start, LPCTSTR end, OUT TOKEN & token);
	virtual void on_spec_word_click(CHARRANGE & cr);
	static LPTSTR find_end_URL( LPCTSTR szURL, LPCTSTR end);

    LRESULT GetListItems ( WPARAM wParam, LPARAM lParam );
	void ConvertCharformat2LogFont(HDC hDc, const CHARFORMAT2& cf, LOGFONT& lf);
	void CalcCharacterDim(long nChar, SIZE& dim, int& nExtra);
	static LPTSTR find_single_bracket( LPTSTR szBegin, int sym);
	static LPTSTR tstring_inc(LPCTSTR str);
	bool IsOverText(HWND hwndRichCtrl, const POINT & pt, DWORD& dwChar);
	
	void SetDragMode(bool bDrag);

	void DrawLocks(HWND hwnd, HDC hDC, const CPoint& ptOffset = CPoint(0,0));
		
		

protected:
	WNDPROC m_oldWndProc;
	HWND		m_Hwnd;
	RTF_LNK_MAP m_RichEditMap;
	RTF_TOOLTIP_MAP m_CompressPathMap;
	RTF_CREATED_TOOLTIP_MAP m_CreatedTooltipMap;
	RTF_TOKEN_LIST m_lstTokenText;
	HCURSOR m_hLinkCursor;
	bool m_bOverControl;

    HWND m_hHideWnd;
	CTitleTip m_ToolTip;

	bool m_bDrag;

	DWORD m_dwLBtnDownLinkID;
	
	bool m_bRedraw;

    friend class CRichText;
};

#endif // !defined(AFX_RICHTEXTIMPL_H__02FFCF37_91B9_4A34_9072_57C247654A0F__INCLUDED_)
