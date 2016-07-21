#if !defined(_CHARTCTRL__INCLUDED_)
#define _CHARTCTRL__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Chart/RTDemoView.h"

#define ID_TIMER_REFRESH 10

class CChartCtrl :
	public CItemControl<CChartCtrlBase>
{
public:
	typedef CItemControl<CChartCtrlBase> TBase;
	DECLARE_WND_AVPCLASS("AVP.ChartCtrl", PROGRESS_CLASS, NULL);

	BEGIN_MSG_MAP(CChartCtrl)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnChartEraseBkgnd)
		MESSAGE_HANDLER(WM_SIZE,             OnChartSize)
//		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUpChart)
		MESSAGE_HANDLER(WM_DESTROY, OnChartDestroy)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		CHAIN_MSG_MAP(TBase)
	END_MSG_MAP()

	LRESULT	OnChartEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return TRUE;
	}

	LRESULT OnChartSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		if (glCurrentType == RTLINE)
			DemoChart.OnSize();
		else
			DemoChart.OnSizeBars();

		return bHandled = FALSE, 0;
	}	

	LRESULT OnLButtonUpChart( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		return bHandled = FALSE, 0;
	}

	LRESULT OnChartDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		DemoChart.OnDestroy();
		::KillTimer(m_hWnd, ID_TIMER_REFRESH);
		return bHandled = FALSE, 0;
	}

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void UpdateChart();
	void UpdatePie();
	void UpdateData(bool bIn, CStructData *pData = NULL)
	{
		TBase::UpdateData(bIn, pData);
		UpdatePie();
	}

	int DoCommand(LPCSTR szCmd, LPCSTR args);



	CChartCtrl(void);
	~CChartCtrl(void);

	bool Draw(HDC dc, RECT* rcUpdate);


protected:
	void InitProps(CItemProps& strProps, CSinkCreateCtx * pCtx);
	void OnInit();
	CItemBase* CreateChild(LPCSTR strItemId, LPCSTR strItemType);

protected:
	CRTDemoView DemoChart;
	bool DrawTestPoint;

	enum Types { RTLINE, LINE, BAR, BAR3D, GANTTBAR, PIE, CUTPIE };
	Types glCurrentType;

	int m_nCount;
	tDWORD m_dwTicks;
	tDWORD m_dwRefresh;
	tDWORD m_dwTimeSpan;
	bool m_bDiff;
	bool m_bOn;

	tString m_szReportName;
	cVector<float> m_PieStatistics;

};

#endif //_CHARTCTRL__INCLUDED_
