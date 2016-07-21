#if !defined(__REALTIME_H__)
#define __REALTIME_H__

#include "graphics.h"
#include "ClassArr.h"
#include "Str.h"


struct CMarker
{
	int	m_nMarkerX;
	int	m_nMarkerY;
	CStr m_sMarkerText;
	CMarker::CMarker()
	{
		m_nMarkerX = 0;
		m_nMarkerY = 0;
		m_sMarkerText = "";
	}
};

// support multi-line display with 
// m_nCyclesPerSec = 0.1, 0.2, 0.4, 0.5, 1, 2, 4, 5, 8, and 10 cycles per second
//////////////////////////////////////////////////////////////////////////////////
////// struct CALine
struct CALine
{
	struct CValue
	{
		bool	bFlag;
		double	YValue;
		// так проще, чем заводить отдельный m_MarkerArray
		bool	bMarker;
		CStr	sMarkerText;
	};

	COLORREF	m_nColor;			// line color
	int			m_nLineStyle;		// line style
	int			m_nLineWidth;		// line width

	CValue*		m_pValues;			// value array

	CValue*		m_pZoomedValues;	// zoomed value array

	CStr		m_sName;			// name
	CStr		m_sDescription;		// description
	CStr		m_sUnit;			// unit
	double		m_dMin;				// minimum value
	double		m_dMax;				// maximum value
	double		m_dScaleLow;		// scaled minimum value
	double		m_dScaleHigh;		// scaled maximum value
	double		m_dLastAbsoluteValue; //for diff graph's
	double		m_dbMaxVisibleValue; //for stripping
/*
	/// mrkr ///
	CClassArray<CMarker, CMarker&> m_MarkerArray;
	CMarker		m_marker;
	CClassArray<CMarker, CMarker&> m_ZoomedMarkerArray;

*/
	bool AddMarker(DWORD MarkerX, DWORD yMarkerY, CStr MarkerText);

	bool Reset(DWORD size);
	void AddValue(double& value, DWORD begin, DWORD end);
	CALine();
	CALine::~CALine()	{ 
		if (m_pValues)  free(m_pValues); 
		if (m_pZoomedValues)  free(m_pZoomedValues); 
/*		
		int n = m_MarkerArray.GetSize();
		for(int i = n-1; i >= 0; i --)
			m_MarkerArray.RemoveAt(i);
*/
	}

	void ChangeZoomedValues(int ZoomLevel, int WindowSize, int CurrentEnd);
	double Avg(int StartIndex, int Count, CStr &MarkerText);

	// Operation
	void SetLineColor(COLORREF cr)		{ m_nColor = cr; }
	COLORREF GetLineColor()				{ return m_nColor; }
	void SetLineStyle(int ls)			{ m_nLineStyle = ls; }
	int  GetLineStyle()					{ return m_nLineStyle; }
	void SetLineWidth(int wd)			{ m_nLineWidth = wd; }
	int  GetLineWidth()					{ return m_nLineWidth; }
	void SetLineName(char* name)		{ m_sName = name; }
	const char* GetLineName()			{ return m_sName.GetChar(); }
	void SetLineDescription(char* desc)	{ m_sDescription = desc; }
	const char* GetLineDescription()	{ return m_sDescription.GetChar(); }
	void SetLineUnit(char* unit)		{ m_sUnit = unit; }
	const char* GetLineUnit()			{ return m_sUnit.GetChar(); }
	void SetLineMinValue(double min)	{ m_dMin = min; }
	double GetLineMinValue()			{ return m_dMin; }
	void SetLineMaxValue(double max)	{ m_dMax = max; }
	double GetLineMaxValue()			{ return m_dMax; }
	void SetLineScaleLow(double low)	{ m_dScaleLow = low; }
	double GetLineScaleLow()			{ return m_dScaleLow; }
	void SetLineScaleHigh(double high)	{ m_dScaleHigh = high; }
	double GetLineScaleHigh()			{ return m_dScaleHigh; }

//private:
	DWORD		m_nSize;		// number of points in the line
};

//////////////////////////////////////////////////////////////////////////////////
////// class CRealTime
class CRealTime : public CGraphics
{
public:
	virtual bool SetRange(double xmin, double ymin, double xmax, double ymax);
	virtual void XAxis();
	virtual void Grid();

	bool InitialSetting(double CycsPerSec,  DWORD StartTime, DWORD TimeSpan,
						DWORD MaxSeconds, int XTick = 6, bool IsForward = true);

	void SetTickLineWidth(int width)	{ m_wdTickLine = width; }
	int  GetTickLineWidth()				{ return m_wdTickLine; }

	void SetTickLineColor(COLORREF cr)	{ m_crTickLine = cr; }
	COLORREF  GetTickLineColor()		{ return m_crTickLine; }

	void SetTickBarHight(int high)		{ m_sHigh = high; }
	void AddYValue(int i, double value){ m_LineArray[i].AddValue(value, m_nBegin, m_nEnd); }

	void DrawRealTimeLines();
	//
	bool AddALine(COLORREF color, double low = 0, double high = 100, 
				  const char* name = "N/A", const char* desc = "N/A", const char* unit = "N/A", 
				  double min = 0, double max = 100, int style = PS_SOLID, int width = 0);
	bool InsertALine(int index, COLORREF color, double low = 0, double high = 100, 
				  const char* name = "N/A", const char* desc = "N/A", const char* unit = "N/A", 
				  double min = 0, double max = 100, int style = PS_SOLID, int width = 0);
	bool RemoveALine(int Index);
	//
	void SetTimeRange(DWORD FromTime, DWORD ToTime);
	void UpdateTimeRange(DWORD& vtime);
	DWORD GetCursorTime();
	int	 GetCursorTimeAndIndex(DWORD&);
	
	void Redraw(HWND hWnd);
	RECT TickBarSize();
	void SetPrintTime(DWORD time, int flag);
	void PrintTime();	

	bool IsLegendEnabled();
	int CalculateY(int value);
	int CalculateX(int value);
	int CalculateValueByY(int Ycoord);
	int CalculateValueByX(int Xcoord);

	bool IsPointInLineArray(int XValue, int YValue);

	void ChangeZoomLevel(bool increase);

	void Reset();
	CRealTime();
	~CRealTime();

	CClassArray<CALine, CALine&> m_LineArray;
	CALine*	m_pCurLine;
	CALine	m_line;
	DWORD	m_nDataPerLine;
	bool	m_bSetingFailed;
	bool	m_bIsForwardDrawing;
	int		m_nSTicks;		// Cursor position
	int		m_nTick;
	int		m_nPage;
	RECT	m_SRect;
	int		m_sHigh;
	DWORD	m_nMaxTime;
	DWORD	m_nCursorTime;
	DWORD	m_nTimes;
	DWORD	m_nMaxPages;
	bool	m_bAutoScrollX;
	DWORD	m_SpeedLevel[10];
	
	// 06.01.2005
	int		m_nZoomLevel;
	int		m_nZoomStart;
	int		m_nStaticXLabels; // 0 - not static, 1 - static (time), 2 - static (ago labels)
		
	enum	TimeTypes{MaxTime = 0, CursorTime};

protected:
	DWORD	m_TimeToDraw;
	DWORD*	m_pTime;
public:
	int		m_nXTicks;
	DWORD	m_nTimeSpan;
protected:
	HPEN	m_CurPen;
	HPEN	m_OldPen;
	DWORD*	m_pValueTime;
	POINT*	m_pLineArray;
	DWORD	m_nFrom;
	DWORD	m_nTo; 
	DWORD	m_nBegin;
	DWORD	m_nEnd;
	// 32 -> 64
	// http://www.codeguru.com/Cpp/controls/controls/chartingandanaloguecontrols/comments.php/c2223/?thread=28952
	char	m_sTempStr[64];

private:
	int		ForwardDraw(int nB, int nE);
	int		BackwardDraw(int nB, int nE);
	void	DrawXAxisTimeTicks(int x, int y, DWORD& ticks);
	void	ShowTime(int x, int y, const char* Tag, DWORD& cTime);
	void	DrawCurrentLine();
	void	DrawTickLine();

	DWORD	m_nP;
	double	m_nCyclesPerSec;
public:
	DWORD	m_nTimeStart;
private:
	int		m_nCounter;
	DWORD	m_nMin;
	DWORD	m_nMax;
	int		m_wdTickLine;
	int		m_nStep;
	int		xb, yb, xe, ye; // save some time !!!
	COLORREF m_crTickLine;
};

#endif
