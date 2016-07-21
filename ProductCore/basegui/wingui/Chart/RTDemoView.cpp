// RTDemoView.cpp : implementation of the CRTDemoView class
//

#include "stdafx.h"

#include "RTDemoView.h"
//#include "TimerSpeed.h"
/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/
#define SliderTickID 12001

///////////////////  I //////////////////////////////////////////////////////
// CRTDemoView construction/destruction
/////////////////////////////////////////////////////////////////////////////
CRTDemoView::CRTDemoView()
{
	SpeedLevel	 = 6;

	CyclesPerSec = 1000.0 / m_Graph.m_SpeedLevel[SpeedLevel];
	MillSeconds	 = m_Graph.m_SpeedLevel[SpeedLevel];
	
	m_bRealTime	 = FALSE;
	
	if (!m_bRealTime) 
		CyclesPerSec = 1;
	
	m_nStartTime = 0;
	mSec		 = m_nStartTime;
	m_nTimeSpan	 = 60;
	m_sTicks	 = (int)(0.5 + m_nTimeSpan * CyclesPerSec);
	// окно запоминания
	MaxSeconds   = 360;//1200; // 20 min
	CanTicks     = TRUE;
	m_bCanSize   = FALSE;
	m_bFlag		 = TRUE;
	m_nTicks	 = 0;
	Rn.init_random();
	m_Graph.SetXNumOfTicks(60);
	hWnd = NULL;

	m_pGraph	= &m_BarLine;
	m_Types		= BAR;
}

CRTDemoView::~CRTDemoView()
{
}

void CRTDemoView::OnInitialUpdate(HWND hWnd, RECT rect) 
{
	hWnd = hWnd;

	if (!m_Graph.SetRange(0, 0, 600, 3))
	{
		m_Graph.m_bSetingFailed = true;
		MessageBox(hWnd,"Setting Range failed","Error",MB_ICONMASK);
		return;
	}
	m_Graph.EnableMemoryDraw(true);
	m_Graph.SetBackColor(RGB(255, 255, 255));

	if (!m_Graph.InitialSetting(CyclesPerSec,  
								m_nStartTime, 
								m_nTimeSpan, 
								MaxSeconds, 
								6 /* XTicks count feature */))
	{
		m_Graph.m_bSetingFailed = true;
		return;
	}
	m_Graph.m_bSetingFailed = false;
	
	m_Graph.RecalcRects(rect);

	m_bCanSize = TRUE;
	m_sSize    = 17;
/*
	m_SliderTick.Create(WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS, 
						m_Graph.TickBarSize(), this, SliderTickID);
	m_SliderTick.SetRange(0, m_sTicks, TRUE);
	m_SliderTick.SetPageSize(1);
	m_SliderTick.SetPos(0);
	m_SliderTick.EnableWindow(FALSE);
*/	
	m_bTimerOn = TRUE;
}

///////////////////  II //////////////////////////////////////////////////////
// CRTDemoView drawing
/////////////////////////////////////////////////////////////////////////////

void CRTDemoView::DrawFrame(RECT& rect, COLORREF cr, const char* Title)
// call only from CRTDemoView::OnDraw
{
//	static bool first = true;
	/// Resize feature ///
	//m_Graph.RecalcRects(rect);

//	m_Graph.DrawBoundary(cr, 5);

	/// RTG specific ///
	//m_Graph.XAxisTitle("Time");
//	m_Graph.YAxisTitle("KAV 2006 test data");
//	m_Graph.Title(Title);	
	m_Graph.Grid();
	m_Graph.Axes();	
}

void CRTDemoView::OnDraw(HDC pDC, HWND hWnd, bool MarkerNeeded)
{
	
	
	if (m_Graph.m_bSetingFailed)
		return;

	m_Graph.BeginDraw(pDC);

	m_Graph.Grid();

	if (MarkerNeeded)
	{
		int Index = m_Graph.GetCursorTimeAndIndex(CurrentTime);

		if (Index>0)
		{
			double m_dValue = m_Graph.m_LineArray[0].m_pValues[Index-1].YValue;
			m_Graph.m_LineArray[0].AddMarker(Index-1, (DWORD)m_dValue, "Eicar");
		}

	}

/*	char TempStr[256];

	sprintf(TempStr, "Add new every %.2f seconds (%.2f value per second)",
			(float)m_Graph.m_SpeedLevel[SpeedLevel] / 1000,
			(float)1000 / m_Graph.m_SpeedLevel[SpeedLevel]);
	
	m_Graph.TestPrint(
		m_Graph.CalculateX(m_Graph.m_nTimeSpan/2), 
		m_Graph.CalculateY(-6),
		TempStr,
		false);


	sprintf(TempStr, "Zoom level = %.d (%.d values)",
			m_Graph.m_nZoomLevel, 
			m_Graph.m_nZoomLevel*m_Graph.m_nTimeSpan); 
	// m_Graph.m_nEnd
	
	m_Graph.TestPrint(
		m_Graph.CalculateX(m_Graph.m_nTimeSpan/2), 
		m_Graph.CalculateY(-8),
		TempStr,
		false);
*/	
	m_Graph.Axes();	

	/// GR 2.  DrawRealTimeLines ///
	m_Graph.DrawRealTimeLines();

	
	
	/// GR 3.  EndDraw ///
	m_Graph.EndDraw(pDC);
}


BOOL CRTDemoView::OnEraseBkgnd(HDC* pDC) 
{
	return TRUE;
}

///////////////////  III /////////////////////////////////////////////////////
// Periodic update
//////////////////////////////////////////////////////////////////////////////

void CRTDemoView::SetData(int i)
// call only from CRTDemoView::OnTimer
{
	//"Rn.randm()" is much faster than "1.0 * rand() / RAND_MAX";
/*	m_dY = m_Graph.m_LineArray[i].m_dScaleLow + 
		  (m_Graph.m_LineArray[i].m_dScaleHigh - 
		   m_Graph.m_LineArray[i].m_dScaleLow) * Rn.randm();
*/
//	m_Graph.AddYValue(i, m_dY);
}

void CRTDemoView::TicksRanges()
// call only from CRTDemoView::OnTimer
{

	if (CanTicks && m_Graph.m_nTimes / (m_nTimeSpan + 1) > m_nTicks && 
		MaxSeconds >= m_Graph.m_nTimes + 1)
	{
		m_nTicks = m_Graph.m_nTimes / m_nTimeSpan;
		if (m_nTicks >= m_Graph.m_nMaxPages) 
			CanTicks = FALSE;
		else
		{
//			ResetSlider(m_nTicks + 1);
			m_Graph.m_nPage = m_nTicks + 1;
		}
	}
}

void CRTDemoView::OnTimer(HWND hWnd) 
{
	// Number of chats
	n = m_Graph.m_LineArray.GetSize();

	// Add Y Value to the i-chat
	for(i=0; i<n; i++)
		SetData(i);
		

	m_Graph.UpdateTimeRange(mSec);
	
	//
	mSec += (UINT)(0.5 + 1000 / CyclesPerSec);
	// 
	m_Graph.SetPrintTime(mSec - m_nStartTime, m_Graph.MaxTime); // Sim time : max time
	n = m_Graph.GetCursorTimeAndIndex(CurrentTime);
	m_Graph.SetPrintTime(CurrentTime - m_nStartTime, m_Graph.CursorTime); // cursor time
//	TicksRanges(); - slider (go back to the previuos data ?!)

	if (m_bFlag)
	{
		if (m_Graph.m_bIsForwardDrawing)
		{
			if (m_Graph.m_nTick < m_Graph.m_nSTicks)
			{
//				m_SliderTick.SetPos(m_Graph.m_nTick);
				m_Graph.m_nTick ++;
			}
			else
			{
//				m_SliderTick.SetPos(m_Graph.m_nTick);
//				m_SliderTick.EnableWindow(TRUE);
				m_bFlag = FALSE;
			}
		}
		else // impossible
		{
//			m_SliderTick.SetPos(m_Graph.m_nSTicks);
			m_Graph.m_nTick ++;
			if (m_Graph.m_nTick == m_Graph.m_nSTicks)
			{
//				m_SliderTick.EnableWindow(TRUE);
				m_bFlag = FALSE;
			}
		}
	}

	m_Graph.Redraw(hWnd);
}

void CRTDemoView::OnDestroy() 
{
	if (!m_Graph.m_bSetingFailed)
		RTKillTimer();

}

void CRTDemoView::OnSize() 
{
	if(m_bCanSize)
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		m_Graph.RecalcRects(rect);
		//m_SliderTick.MoveWindow(&m_Graph.TickBarSize());
	}
}
/*
RECT& CRTDemoView::GetGraphRect()
{
	if (::IsWindow(m_hWnd))
		return m_Graph.m_PlotRect;
	else
	{
		static RECT rt;
		rt.left   = 0;
		rt.top    = 0;
		rt.right  = 0;
		rt.bottom = 0;
		return rt;
	}
}

void CRTDemoView::ResetSlider(int Ticks)
{
	CRTForm* pForm = (CRTForm*)GetRTForm();
	pForm->InitialSlider(0, Ticks - 1, 1, Ticks);
}
*/

void CRTDemoView::RTKillTimer()
{
	KillTimer(hWnd, 1);
	m_bTimerOn = FALSE;
}

void CRTDemoView::RTRestoreTimer(HWND hWnd)
{
	SetTimer(hWnd, 1, MillSeconds, NULL);
	m_bTimerOn = TRUE;
}

///////////////////////////////////////////////////////////////////

BOOL CRTDemoView::InsertALine(int index, COLORREF color, double low, double high, 
							  const char* name, const char* desc, const char* unit, 
							  double min, double max, int style, int width)
{
	return m_Graph.InsertALine(index, color, low, high, name, desc, 
							unit, min, max, style, width);
}

BOOL CRTDemoView::RemoveALine(int Index)
{
	return m_Graph.RemoveALine(Index);
}

void CRTDemoView::RefreshGraph() //?!
{
	m = m_Graph.GetCursorTimeAndIndex(CurrentTime);
	m_Graph.Redraw(hWnd);
}

void CRTDemoView::SaveAModifiedLine() // ?!
{
	RefreshGraph();
}

void CRTDemoView::GetColorAndName(int index, COLORREF& cr, CStr& name)
{
	cr   = m_Graph.m_LineArray[index].m_nColor;
	name = m_Graph.m_LineArray[index].m_sName.GetChar();
}

void CRTDemoView::Redraw()
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	m_Graph.RecalcRects(rect);
	InvalidateRect(hWnd, &rect, false);
}

/////////////////////////////////////////////////////////////////
void CRTDemoView::ChangeSpeedLevel(bool increase)
{
	RTKillTimer();
	increase ? SpeedLevel++ : SpeedLevel--;

	if (SpeedLevel < 0) SpeedLevel = 0;
	if (SpeedLevel > 9) SpeedLevel = 9;

	MillSeconds = m_Graph.m_SpeedLevel[SpeedLevel];
	RTRestoreTimer(hWnd);
}

void CRTDemoView::ChangeTimeSpan(int NewValue)
{
	m_nStartTime = NewValue;
	//
}

void CRTDemoView::ChangeZoomLevel(bool increase)
{
	m_Graph.ChangeZoomLevel(increase);
}
////////////// For BARS //////////////////////

void CRTDemoView::OnSizeBars() 
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	m_pGraph->RecalcRects(rect);
}

void CRTDemoView::DrawFrameBars(RECT& rect, COLORREF cr, const char* Title)
{
	m_pGraph->RecalcRects(rect);
//	m_pGraph->DrawBoundary(cr, 5);
	m_pGraph->Title(Title);
}

void CRTDemoView::OnDrawBars(HDC pDC, HWND hWnd)
{
	RECT rect;
	GetClientRect(hWnd, &rect);

	bool 	m_bGrid	= true;
	bool	m_bVertical = false;

	m_pGraph->BeginDraw(pDC);

	switch(m_Types)
	{
	case LINE:
		DrawFrameBars(rect, RGB(255, 0, 255), "Line");
		m_pGraph->Line(m_bGrid, PS_SOLID, 3);
		break;
	case BAR:
		DrawFrameBars(rect, RGB(255, 0, 255), "Bar");
		m_pGraph->Bar(m_bGrid, m_bVertical);
		break;
	case BAR3D:
		DrawFrameBars(rect, RGB(255, 0, 255), "Bar 3D");
		m_pGraph->Bar3D(m_bGrid, m_bVertical);
		break;
	case GANTTBAR:
		DrawFrameBars(rect, RGB(255, 0, 255), "Gantt Bar");
		m_pGraph->GanttBar(m_bGrid, m_bVertical);
		break;
	case PIE:
//		DrawFrameBars(rect, RGB(255, 0, 255), "Pie");
		m_Pie.CutPie(x, 5, colstr, false);
		break;
	case CUTPIE:
//		DrawFrameBars(rect, RGB(255, 0, 255), "Cut Pie");
		m_Pie.CutPie(x, 5, colstr);
		break;
	}

	m_pGraph->EndDraw(pDC);
}

void CRTDemoView::RedrawBars()
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	m_pGraph->RecalcRects(rect);
	InvalidateRect(hWnd, &rect, false);
}

void CRTDemoView::OnTimerBars() 
{
	//rowstr[0] = "changed row ";
/*	for (int j=0;j<15;j++)
		RawData[j]  = 900*Rn.randm();
*/
	for (int j=0;j<5;j++)
	{
		x[j]  = 30;//80*Rn.randm();
//		y[j]  = 80*Rn.randm();
	}

//	m_pGraph->SetData(RawData, 3, 5, rowstr, colstr);
	RedrawBars();
}

void CRTDemoView::InitDataBars()
{
	// init data for Bars
	x[0]  = 5;
	y[0]  = 70;
	x[1]  = 30;
	y[1]  = 30;
	x[2]  = 40;
	y[2]  = 4;
	x[3]  = 70;
	y[3]  = 50;
	x[4]  = 80;
	y[4]  = 80;
/*
	pt[0] = CTPoint<float>(4,  20); 
	pt[1] = CTPoint<float>(35, 40); 
	pt[2] = CTPoint<float>(50,  4); 
	pt[3] = CTPoint<float>(60, 60); 
	pt[4] = CTPoint<float>(75, 85);
*/
	RawData[0]  = 50;
	RawData[1]  = 150;
	RawData[2]  = 300;
	RawData[3]  = 100;
	RawData[4]  = 200;
	RawData[5]  = 700;
	RawData[6]  = 300;
	RawData[7]  = 200;
	RawData[8]  = 500;
	RawData[9]  = 100;
	RawData[10] = 30;
	RawData[11] = 100;
	RawData[12] = 400;
	RawData[13] = 700;
	RawData[14] = 800;

	rowstr[0] = "row1";
	rowstr[1] = "row2";
	rowstr[2] = "row3";

	colstr[0] = "Jan.";
	colstr[1] = "Feb.";
	colstr[2] = "March";
	colstr[3] = "April";
	colstr[4] = "May";
}