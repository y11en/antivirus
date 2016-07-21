// RTDemoView.h : interface of the CRTDemoView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RTDemoVIEW_H__12551283_E7E7_11D3_85BC_0008C777FFEE__INCLUDED_)
#define AFX_RTDemoVIEW_H__12551283_E7E7_11D3_85BC_0008C777FFEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//#include "stdafx.h"

#include "RealTime.h"
 #include "random.h"
#include "str.h"

#include "BarLine.h"
#include "Pie.h"

template <class T> 
struct CTPoint
{
	T x;
	T y;

	CTPoint()							{ x = 0; y = 0;	}
	CTPoint(T initX, T initY)			{ x = initX; y = initY; }
	void operator = (const CTPoint& pt)	{ x = pt.x;	y = pt.y; }
};

class CRTDemoView
{
public:
	HWND		hWnd;
	CRealTime	m_Graph;
	RndGen		Rn;
	//CRTSlider m_SliderTick;

	DWORD		m_nStartTime;
	int			m_nTimeSpan;
	DWORD		MaxSeconds;
	DWORD		mSec;
	double		CyclesPerSec;
	DWORD		CurrentTime;
	BOOL		m_bCanSize;

	RECT		m_SRect;
	int			m_sSize;
	int			m_sTicks;
	BOOL		m_bFlag;
	BOOL		CanTicks;
	DWORD		m_nTicks;

	BOOL		m_bRealTime;
	BOOL		m_bTimerOn;

	CStr		m_sString;
	DWORD		MillSeconds;
	int			SpeedLevel;

	// just want to save time using temporary variables
	int    i, m, n;
	double m_dY, m_dValue;

	// 09.01.2005
	CBarLine<float> m_BarLine;
	CPie<float>		m_Pie;
	CBarLine<float>*  m_pGraph;

	enum Types { LINE, BAR, BAR3D, GANTTBAR, PIE, CUTPIE };
	Types m_Types;

public:
	CRTDemoView();
	virtual ~CRTDemoView();

	void	DrawFrame(RECT& rect, COLORREF cr, const char* Title);
	void	Redraw();
	void	SetData(int i);
	void	RefreshGraph();

	void	TicksRanges();

	void	RTKillTimer();
	void	RTRestoreTimer(HWND hWnd);

	void	ChangeTimeSpan(int NewValue);

	BOOL	InsertALine(int index, COLORREF color, double low, double high, 
					  const char* name, const char* desc, const char* unit, 
					  double min, double max, int style, int width);
	BOOL	RemoveALine(int Index);
	void	SaveAModifiedLine();
	void	GetColorAndName(int index, COLORREF& cr, CStr& name);

	void	OnInitialUpdate(HWND hWnd, RECT rect);
	void	OnDraw(HDC pDC, HWND hWnd, bool TestPoint);

	void	OnDestroy();
	BOOL	OnEraseBkgnd(HDC* pDC);
	void	OnTimer(HWND hWnd);
	void	OnSize();

	void	ChangeSpeedLevel(bool increase);
	void	ChangeZoomLevel(bool increase);

	//// for BARS //////////////////////////////////////////////////////////////////////////
	void	OnSizeBars();	
	void	OnDrawBars(HDC pDC, HWND hWnd);
	void	DrawFrameBars(RECT& rect, COLORREF cr, const char* Title);
	void	RedrawBars();	
	void	OnTimerBars();

	float x[5];
	float y[5];
	CTPoint<float> pt[5];

	float RawData[3*5];
	string rowstr[3];
	string colstr[5];

	void	InitDataBars();
};

#endif // !defined(AFX_RTDemoVIEW_H__12551283_E7E7_11D3_85BC_0008C777FFEE__INCLUDED_)
