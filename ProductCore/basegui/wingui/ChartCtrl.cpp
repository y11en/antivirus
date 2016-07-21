#include "StdAfx.h"
#include "winroot.h"
#include "ChartCtrl.h"
#include "Chart/graphics.h"
//#include "AV/Structs/s_avs.h"

static int GetNextGraceTwoDigit(int nTwoDigitValue)
{
	if (nTwoDigitValue<0 || nTwoDigitValue>99)
		return 0;

	static int NiceDigit[] = {1,2,5,10,15,20,25,30,35,40,50,60,70,75,80,85,90,100};
	int i = 0;
	for (; NiceDigit[i]<=nTwoDigitValue;++i);

	return NiceDigit[i];
}
static double GracefulMax(double val, int nTicks)
{
	PR_ASSERT(val>0.0);
	PR_ASSERT(nTicks>0 && nTicks<100);

	int row = 1;
	while(val/row>99) row*=10;

	int Nice = GetNextGraceTwoDigit(int ((val/row + 0.5)/(double)nTicks + 0.5));
	int NextNice = GetNextGraceTwoDigit(Nice);

	if (NextNice && row*NextNice*nTicks < 1.15*val)
		return row*NextNice*nTicks;
	
	return row*Nice*nTicks;
}

class  CChartLine: public CItemBase
{
public:

	CChartLine() : m_crLineColor(RGB(0,0,0)), m_vVal(tQWORD(0)){}

	tTYPE_ID     GetType(){return tid_VOID;}
	bool         SetValue(const cVariant &pValue) {m_vVal = pValue; return true;}
	bool         GetValue(cVariant &pValue){pValue = m_vVal; return true;}

	COLORREF m_crLineColor;

protected:
	void InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx)
	{
		CItemBase::InitProps(strProps, pCtx);
		CItemPropVals& props = strProps.Get("color");
		m_crLineColor = m_pRoot->GetColor(props.Get().c_str());
	}

	cVariant m_vVal;


};


CChartCtrl::CChartCtrl(void)
{
//	m_nFlags |= STYLE_DRAW_BACKGROUND|STYLE_DRAW_CONTENT|STYLE_TRANSPARENT;
	m_nStyles = WS_CHILD;	
	glCurrentType = RTLINE;

	m_nCount = 0;
	m_dwTicks = 0;
	m_bDiff = false;

	m_dwTimeSpan = 60;

	m_bOn = false;

}

CChartCtrl::~CChartCtrl(void)
{
}

void CChartCtrl::InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx)
{
	TBase::InitProps(strProps, pCtx);
	CItemPropVals& props = strProps.Get(STR_PID_CHARTPROPS);
	
	do
	{
		if (props.Count()<=0)
			break;

		sswitch(props.Get().c_str())
		scase("line")   glCurrentType = RTLINE;                 
		sbreak;

		/*	scase("line")   
		glCurrentType = LINE;               
		DemoChart.m_Types  = DemoChart.LINE;
		DemoChart.m_pGraph = &DemoChart.m_BarLine;
		sbreak;

		scase("bar")   
		glCurrentType = BAR;                 
		DemoChart.m_Types  = DemoChart.BAR;
		DemoChart.m_pGraph = &DemoChart.m_BarLine;
		sbreak;

		scase("bar3d")
		glCurrentType = BAR3D;
		DemoChart.m_Types  = DemoChart.BAR3D;
		DemoChart.m_pGraph = &DemoChart.m_BarLine;
		sbreak;

		scase("ganttbar")   
		glCurrentType = GANTTBAR;
		DemoChart.m_Types  = DemoChart.GANTTBAR;
		DemoChart.m_pGraph = &DemoChart.m_BarLine;
		sbreak;
		*/
		scase("pie")   
			glCurrentType = PIE;
		DemoChart.m_Types  = DemoChart.PIE;
		DemoChart.m_pGraph = &DemoChart.m_Pie;
		sbreak;

		scase("cutpie")   
			glCurrentType = CUTPIE;
		DemoChart.m_Types  = DemoChart.CUTPIE;
		DemoChart.m_pGraph = &DemoChart.m_Pie;
		sbreak;

		send;

		if (props.Count()<=1)
			break;

		sswitch(props.Get(1).c_str())
		scase("abs")
			m_bDiff = false;
		sbreak;
		scase("diff")
			m_bDiff = true;
		sbreak;
		send;

		if (props.Count()<=2)
			break;

		m_dwTimeSpan = (tDWORD)props.GetLong(2);

	
	}while(0);

	m_dwRefresh = (tDWORD)strProps.Get(STR_PID_CHARTREFRESH).GetLong();

/*	CItemPropVals& props_axes = strProps.Get(STR_PID_CHART_AXES);
	
	tString label = props_axes.Get();
	for (int i = 1; i<props_axes.Count(); ++i)
	{
		tString d = props_axes.Get(i);
	}
*/
}

void CChartCtrl::UpdateChart()
{
	DWORD dwTicks = GetTickCount();
	double dDeltaTicks = dwTicks - m_dwTicks;

	if (dDeltaTicks==0.0)
		return;

	double max_value = DemoChart.m_Graph.m_Scale.ymax;
	double max_visible_value = 0;
	size_t nActiveLines = min((size_t)DemoChart.m_Graph.m_LineArray.GetSize(),m_aItems.size());
	
	for(size_t i = 0; i < nActiveLines; i++)
	{
		cVariant Value;
		CChartLine * pItem = (CChartLine*)m_aItems[i];

		if( pItem && pItem->m_pBinding && pItem->m_pBinding->GetHandler(CItemBinding::hValue) )
			pItem->m_pBinding->GetHandler(CItemBinding::hValue)->Exec(Value, TOR(cNodeExecCtx, (pItem->m_pRoot, pItem)));

			double val = (double)Value.ToDWORD();
			double dSpeed = m_bDiff?(1000.0*(val - DemoChart.m_Graph.m_LineArray[i].m_dLastAbsoluteValue)/dDeltaTicks):val;

			DemoChart.m_Graph.m_LineArray[i].m_dLastAbsoluteValue = val;

			if (m_dwTicks == 0)
				continue;

			if (DemoChart.m_Graph.m_Scale.ymax<=dSpeed)
				DemoChart.m_Graph.m_Scale.ymax = GracefulMax(dSpeed, 3);

			DemoChart.m_Graph.AddYValue(i, dSpeed);

			max_visible_value = max(DemoChart.m_Graph.m_LineArray[i].m_dbMaxVisibleValue, max_visible_value);
	}

	m_dwTicks = dwTicks;

	if (DemoChart.m_Graph.m_Scale.ymax<3)
		DemoChart.m_Graph.m_Scale.ymax = 3;

	if (DemoChart.m_Graph.m_Scale.ymax>max_value)
	{

		RECT rect;
		::GetClientRect(m_hWnd, &rect);
		DemoChart.m_Graph.RecalcRects(rect);
		InvalidateRect(NULL);
	}
	else if (max_visible_value && (2*max_visible_value<DemoChart.m_Graph.m_Scale.ymax || (max_visible_value+max_visible_value*0.15)>DemoChart.m_Graph.m_Scale.ymax))
	{
		DemoChart.m_Graph.m_Scale.ymax = GracefulMax(max_visible_value, 3);
		if (DemoChart.m_Graph.m_Scale.ymax<3)
			DemoChart.m_Graph.m_Scale.ymax = 3;

		RECT rect;
		::GetClientRect(m_hWnd, &rect);
		DemoChart.m_Graph.RecalcRects(rect);
		InvalidateRect(NULL);
	}

	DemoChart.OnTimer(m_hWnd);

}

bool CChartCtrl::Draw(HDC dc, RECT* rcUpdate)
{
	if (glCurrentType == RTLINE)			
	{			
		DemoChart.OnDraw(dc, m_hWnd, false);	

		if (DrawTestPoint)
		{
			DemoChart.OnDraw(dc, m_hWnd, true);	
			DrawTestPoint = false;
		}

	}
	else if (glCurrentType == PIE)
	{
	
		DemoChart.m_pGraph->BeginDraw(dc);

		cVector<float> fvPieStatistics;

		
		for(size_t i = 0; i < m_aItems.size(); ++i)
		{
			cVariant val;
			m_aItems[i]->GetValue(val);
			fvPieStatistics.push_back((float)val.ToDWORD());
		}
		
		if (!fvPieStatistics.empty())
			DemoChart.m_Pie.CutPie(&fvPieStatistics[0], fvPieStatistics.count(), NULL, false);

		//DemoChart.m_Pie.CutPie(DemoChart.x, 5, NULL, false);
		DemoChart.m_pGraph->EndDraw(dc);

	}
//		DemoChart.OnDrawBars(dc, m_hWnd);

//		m_Pie.CutPie(x, 5, colstr, false);

//	::FillRect(dc, rcUpdate, (HBRUSH)GetStockObject(BLACK_BRUSH));
	return true;
}

void CChartCtrl::OnInit()
{
	TBase::OnInited();

	::ShowWindow(m_hWnd, SW_SHOWNORMAL);
	::UpdateWindow(m_hWnd);


   DrawTestPoint = false;

   DemoChart.hWnd = m_hWnd;
   RECT rt;
   ::GetClientRect(m_hWnd, &rt);

   DemoChart.CyclesPerSec = m_dwRefresh?(1000.0/m_dwRefresh):1.0;
   if (DemoChart.CyclesPerSec < 0.09999)
	   DemoChart.CyclesPerSec = 0.1;
   else if (DemoChart.CyclesPerSec > 10.00001f)
	   DemoChart.CyclesPerSec = 10.0;

   DemoChart.m_nTimeSpan = (m_dwTimeSpan>=30)?m_dwTimeSpan:30;
   DemoChart.OnInitialUpdate(m_hWnd, rt);
   DemoChart.m_Graph.m_nStaticXLabels = 2;
   DemoChart.m_Graph.SetXNumOfGridTicks(3);
   DemoChart.m_Graph.SetYNumOfGridTicks(3);


   if (glCurrentType == RTLINE)
   {
	   for(size_t i = 0; i < m_aItems.size(); i++)
		{
			CChartLine * pItem = (CChartLine*)m_aItems[i];
			DemoChart.m_Graph.AddALine(pItem->m_crLineColor, 10, 20, pItem->GetDisplayText(), "N/A", "N/A", 0, 100, PS_SOLID, 2);
		}
//	   	DemoChart.m_Graph.AddALine(RGB(254,   0,   0), 10, 20, "Files", "N/A", "N/A", 0, 100, PS_SOLID, 2);	
//	    DemoChart.m_Graph.AddALine(RGB(0, 254,   128), 10, 20, "Mail's", "N/A", "N/A", 0, 100, PS_SOLID, 2);
//		DemoChart.m_Graph.AddALine(RGB(  0,   0, 254), 10, 20, "Scripts", "N/A", "N/A", 0, 100, PS_SOLID, 2);
//		DemoChart.RTRestoreTimer(m_hWnd);
	   ::SetTimer(m_hWnd, ID_TIMER_REFRESH, (UINT)(1000.0/DemoChart.CyclesPerSec), NULL); 
	   m_bOn = true;
		DemoChart.Redraw();
   }
   else if (glCurrentType == PIE)
   {
		for(size_t i = 0; i < m_aItems.size(); i++)
			m_PieStatistics.push_back(0.0);

	   if (m_dwRefresh)
			::SetTimer(m_hWnd, ID_TIMER_REFRESH, m_dwRefresh, NULL);

		DemoChart.RedrawBars();
   }

   DemoChart.InitDataBars();
   DemoChart.m_pGraph->SetData(DemoChart.RawData, 3, 5, DemoChart.rowstr, DemoChart.colstr);

}

CItemBase* CChartCtrl::CreateChild(LPCSTR strItemId, LPCSTR strItemType)
{
	return new CChartLine;
}

void CChartCtrl::UpdatePie()
{
/*	if (!m_pSink)
		return;

	tDWORD nCount;
	if (PR_SUCC(((CReportSink*)m_pSink)->GetRecordCount(&nCount)))
	{

		tBYTE buff[sizeof(tDWORD)+sizeof(tQWORD)];
		for(tDWORD i = 0; i<nCount; ++i)
		{
			if ( PR_SUCC(((CReportSink*)m_pSink)->GetRecord(NULL, i, buff, sizeof(buff), NULL)) )
			{
				for(size_t ii = 0; ii < m_aItems.size(); ++ii)
				{
					CChartLine * pItem = (CChartLine*)m_aItems[ii];
					if (pItem->m_ID == *(tDWORD*)buff)
					{
						if (ii<m_PieStatistics.count())
							m_PieStatistics[ii] = (float)(*(tQWORD*)(buff+sizeof(tDWORD)));
						else
						{
							PR_ASSERT(false);
						}

						break;
					}
				}
			}
		}
	}
*/
/*	g_pRoot->m_tm->
	char* buff[];
	Read(buff)
	for(each)
	for(size_t i = 0; i < m_aItems.size(); i++)
	{
		CChartPieSector * pItem = (CChartPieSector*)m_aItems[i];
		DemoChart.m_Graph.AddALine(pItem->m_crLineColor, 10, 20, pItem->GetDisplayText(), "N/A", "N/A", 0, 100, PS_SOLID, 2);
	}
*/
	DemoChart.RedrawBars();
}

LRESULT CChartCtrl::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( wParam == ID_TIMER_REFRESH )
	{
		if (glCurrentType==RTLINE)
			UpdateChart();
		else if (glCurrentType==PIE)
			UpdatePie();
	}
	
	return bHandled = FALSE, 0;
}


int CChartCtrl::DoCommand(LPCSTR szCmd, LPCSTR args)
{
	int ret = 0;
	sswitch(szCmd)
		scase("-")
			DemoChart.ChangeZoomLevel(false);
			OnChangedState(this, ISTATE_CUSTOM);
			DemoChart.Redraw();
		sbreak;
		scase("+")
			DemoChart.ChangeZoomLevel(true);
			OnChangedState(this, ISTATE_CUSTOM);
			DemoChart.Redraw();
		sbreak;
		scase("IsDecrease")
			ret = (DemoChart.m_Graph.m_nZoomLevel>1);
		sbreak;
		scase("IsIncrease")
			ret = (DemoChart.m_Graph.m_nZoomLevel<5);
		sbreak;
		scase("off")
			if (m_bOn)
				::KillTimer(m_hWnd, ID_TIMER_REFRESH),m_bOn=false;
		sbreak;
		scase("on")
			if (!m_bOn)
			{
				::SetTimer(m_hWnd, ID_TIMER_REFRESH, (UINT)(1000.0/DemoChart.CyclesPerSec), NULL);
				m_bOn = true;
			}
		sbreak;

	send;
	
	return ret;
}
