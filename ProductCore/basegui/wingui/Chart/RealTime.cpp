#include "stdafx.h"
//#include <windows.h>
//#include <stdio.h>
#include "RealTime.h"


void DrawWuLine( HDC hDC, int X0, int Y0, int X1, int Y1, COLORREF clrLine )
{
    /* Make sure the line runs top to bottom */
    if (Y0 > Y1)
    {
        int Temp = Y0; Y0 = Y1; Y1 = Temp;
        Temp = X0; X0 = X1; X1 = Temp;
    }
    
    /* Draw the initial pixel, which is always exactly intersected by
    the line and so needs no weighting */
	::SetPixel(hDC, X0, Y0, clrLine );
    
    int XDir, DeltaX = X1 - X0;
    if( DeltaX >= 0 )
    {
        XDir = 1;
    }
    else
    {
        XDir   = -1;
        DeltaX = 0 - DeltaX; /* make DeltaX positive */
    }
    
    /* Special-case horizontal, vertical, and diagonal lines, which
    require no weighting because they go right through the center of
    every pixel */
    int DeltaY = Y1 - Y0;
    if (DeltaY == 0)
    {
        /* Horizontal line */
        while (DeltaX-- != 0)
        {
            X0 += XDir;
			::SetPixel(hDC, X0, Y0, clrLine );
        }
        return;
    }
    if (DeltaX == 0)
    {
        /* Vertical line */
        do
        {
            Y0++;
			::SetPixel(hDC, X0, Y0, clrLine );
        } while (--DeltaY != 0);
        return;
    }
    
    if (DeltaX == DeltaY)
    {
        /* Diagonal line */
        do
        {
            X0 += XDir;
            Y0++;
			::SetPixel(hDC, X0, Y0, clrLine );
        } while (--DeltaY != 0);
        return;
    }
    
    unsigned short ErrorAdj;
    unsigned short ErrorAccTemp, Weighting;
    
    /* Line is not horizontal, diagonal, or vertical */
    unsigned short ErrorAcc = 0;  /* initialize the line error accumulator to 0 */
    
    BYTE rl = GetRValue( clrLine );
    BYTE gl = GetGValue( clrLine );
    BYTE bl = GetBValue( clrLine );
    double grayl = rl * 0.299 + gl * 0.587 + bl * 0.114;
    
    /* Is this an X-major or Y-major line? */
    if (DeltaY > DeltaX)
    {
    /* Y-major line; calculate 16-bit fixed-point fractional part of a
    pixel that X advances each time Y advances 1 pixel, truncating the
        result so that we won't overrun the endpoint along the X axis */
        ErrorAdj = ((unsigned long) DeltaX << 16) / (unsigned long) DeltaY;
        /* Draw all pixels other than the first and last */
        while (--DeltaY) {
            ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
            ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
            if (ErrorAcc <= ErrorAccTemp) {
                /* The error accumulator turned over, so advance the X coord */
                X0 += XDir;
            }
            Y0++; /* Y-major, so always advance Y */
                  /* The IntensityBits most significant bits of ErrorAcc give us the
                  intensity weighting for this pixel, and the complement of the
            weighting for the paired pixel */
            Weighting = ErrorAcc >> 8;
            assert( Weighting < 256 );
            assert( ( Weighting ^ 255 ) < 256 );
            
            COLORREF clrBackGround = ::GetPixel( hDC, X0, Y0 );
            BYTE rb = GetRValue( clrBackGround );
            BYTE gb = GetGValue( clrBackGround );
            BYTE bb = GetBValue( clrBackGround );
            double grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;
            
            BYTE rr = ( rb > rl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( rl - rb ) + rb ) ) );
            BYTE gr = ( gb > gl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( gl - gb ) + gb ) ) );
            BYTE br = ( bb > bl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( bl - bb ) + bb ) ) );
			::SetPixel(hDC, X0, Y0, RGB( rr, gr, br ) );
            
            clrBackGround = ::GetPixel( hDC, X0 + XDir, Y0 );
            rb = GetRValue( clrBackGround );
            gb = GetGValue( clrBackGround );
            bb = GetBValue( clrBackGround );
            grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;
            
            rr = ( rb > rl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( rl - rb ) + rb ) ) );
            gr = ( gb > gl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( gl - gb ) + gb ) ) );
            br = ( bb > bl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( bl - bb ) + bb ) ) );
			::SetPixel( hDC, X0 + XDir, Y0, RGB( rr, gr, br ) );
        }
        /* Draw the final pixel, which is always exactly intersected by the line
        and so needs no weighting */
		::SetPixel(hDC, X1, Y1, clrLine );
        return;
    }
    /* It's an X-major line; calculate 16-bit fixed-point fractional part of a
    pixel that Y advances each time X advances 1 pixel, truncating the
    result to avoid overrunning the endpoint along the X axis */
    ErrorAdj = ((unsigned long) DeltaY << 16) / (unsigned long) DeltaX;
    /* Draw all pixels other than the first and last */
    while (--DeltaX) {
        ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
        ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
        if (ErrorAcc <= ErrorAccTemp) {
            /* The error accumulator turned over, so advance the Y coord */
            Y0++;
        }
        X0 += XDir; /* X-major, so always advance X */
                    /* The IntensityBits most significant bits of ErrorAcc give us the
                    intensity weighting for this pixel, and the complement of the
        weighting for the paired pixel */
        Weighting = ErrorAcc >> 8;
        assert( Weighting < 256 );
        assert( ( Weighting ^ 255 ) < 256 );
        
        COLORREF clrBackGround = ::GetPixel( hDC, X0, Y0 );
        BYTE rb = GetRValue( clrBackGround );
        BYTE gb = GetGValue( clrBackGround );
        BYTE bb = GetBValue( clrBackGround );
        double grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;
        
        BYTE rr = ( rb > rl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( rl - rb ) + rb ) ) );
        BYTE gr = ( gb > gl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( gl - gb ) + gb ) ) );
        BYTE br = ( bb > bl ? ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?Weighting:(Weighting ^ 255)) ) / 255.0 * ( bl - bb ) + bb ) ) );
        
		::SetPixel(hDC, X0, Y0, RGB( rr, gr, br ) );
        
        clrBackGround = ::GetPixel( hDC, X0, Y0 + 1 );
        rb = GetRValue( clrBackGround );
        gb = GetGValue( clrBackGround );
        bb = GetBValue( clrBackGround );
        grayb = rb * 0.299 + gb * 0.587 + bb * 0.114;
        
        rr = ( rb > rl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( rb - rl ) + rl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( rl - rb ) + rb ) ) );
        gr = ( gb > gl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( gb - gl ) + gl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( gl - gb ) + gb ) ) );
        br = ( bb > bl ? ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( bb - bl ) + bl ) ) : ( ( BYTE )( ( ( double )( grayl<grayb?(Weighting ^ 255):Weighting) ) / 255.0 * ( bl - bb ) + bb ) ) );
        
		::SetPixel(hDC, X0, Y0 + 1, RGB( rr, gr, br ) );
    }
    
    /* Draw the final pixel, which is always exactly intersected by the line
    and so needs no weighting */
	::SetPixel(hDC, X1, Y1, clrLine );
}

static void PolylineAA(HDC dc, const POINT *lppt, int cPoints, COLORREF color)
{
	if (cPoints<2)
		return;

	for(int i = 1; i<cPoints; ++i)
		DrawWuLine(dc, lppt[i-1].x, lppt[i-1].y, lppt[i].x, lppt[i].y, color);
}

//////////////////// CALine //////////////////////////////////////
CALine::CALine()
{ 
	m_nColor		= RGB(0, 0, 0);		
	m_nLineStyle	= PS_SOLID;	
	m_nLineWidth	= 0;	
	m_pValues		= NULL;		
	m_pZoomedValues	= NULL;			
	m_sName			= "";		
	m_sDescription	= "";	
	m_sUnit			= "";		
	m_dMin			= 0;			
	m_dMax			= 0;			
	m_dScaleLow		= 0;	
	m_dScaleHigh	= 0;	
	m_nSize			= 0; 
}


/// mrkr ///
bool CALine::AddMarker(DWORD MarkerX, DWORD MarkerY, CStr MarkerText)
{
	/*
	m_MarkerArray.Add(m_marker);
	int n = m_MarkerArray.GetSize() - 1;

	m_MarkerArray[n].m_nMarkerX		= MarkerX;
	m_MarkerArray[n].m_nMarkerY		= MarkerY;
	m_MarkerArray[n].m_sMarkerText	= MarkerText;
	*/
	m_pValues[MarkerX].bMarker	   = TRUE;
	m_pValues[MarkerX].sMarkerText = MarkerText;

	return true;
}


bool CALine::Reset(DWORD size)
{
	assert(size > 0);

	if (m_pValues) free(m_pValues);

	m_pValues  = (CValue*)calloc((size_t)(size + 1), sizeof(CValue));

	if(!m_pValues) 
		return false;

	m_nSize = size;
	return true;
}

void CALine::AddValue(double& value, DWORD begin, DWORD end)
{
	if (m_pValues == NULL) return;
	
	// http://www.codeguru.com/Cpp/controls/controls/chartingandanaloguecontrols/comments.php/c2223/?thread=28954
	if (begin == m_nSize) 
		m_pValues[0].YValue = value; 

	if (end == m_nSize)
	{
		m_pValues[begin].bFlag	= 1;
		m_pValues[begin].YValue = value;
	}
	else
	{
		m_pValues[end].bFlag	= 1;
		m_pValues[end].YValue	= value;
	}
}

double CALine::Avg(int StartIndex, int Count, CStr &MarkerText)
{
	double res = 0.00;
	int MarkersCount = 0;

	if (StartIndex<0)
		return res;

	for (int i=StartIndex; i<StartIndex+Count;i++)
	{
		res +=m_pValues[i].YValue;
		
		if (m_pValues[i].bMarker)
		{
			MarkersCount++;
			MarkerText += m_pValues[i].sMarkerText;
			//MarkerText += " \n\r";
		}
		if (MarkersCount>1)
		{
			CStr sMarkersCount(MarkersCount);
			MarkerText = sMarkersCount + " markers";
		}
	}

	return res/Count;
}

// m_nSize = 5
//  0  1 2  3 4
// 10 15 5 11 7
//
// WindowSize = 2 -> view [3-4] - 11 7
// ZoomLevel = 2  -> view [1-4] - (15+5)/2 (11+7)/2
void CALine::ChangeZoomedValues(int ZoomLevel, int WindowSize, int CurrentEnd)
{
	// alloc memory at first time	
	if (m_pZoomedValues == NULL)
	{		
		m_pZoomedValues = 
			(CValue*)calloc((size_t)(WindowSize), sizeof(CValue));

	}

	if (m_pZoomedValues == NULL) 
		return;
	// fill ZoomedValues array
	int nBegin = CurrentEnd - ZoomLevel*WindowSize;	

	int nZoomedValuesIndex = 0;
	CStr sMarkerText;
	// every ZoomLevel points in the m_pValues merge into 1 in the m_pZoomedValues
	for (int i=nBegin;i<CurrentEnd;i=i+ZoomLevel)
	{
		m_pZoomedValues[nZoomedValuesIndex].bFlag  = 1;

		sMarkerText = "";
		m_pZoomedValues[nZoomedValuesIndex].YValue = Avg(i,ZoomLevel, sMarkerText);	
		
		m_pZoomedValues[nZoomedValuesIndex].bMarker = (sMarkerText != "");
		m_pZoomedValues[nZoomedValuesIndex].sMarkerText = sMarkerText;

		if (m_pZoomedValues[nZoomedValuesIndex].YValue<0)
			m_pZoomedValues[nZoomedValuesIndex].YValue=0;


		nZoomedValuesIndex++;
	}
}


//////////////////// CRealTime //////////////////////////////////////

CRealTime::CRealTime()
{
	m_bSetingFailed	= true;
	m_nMaxPages		= 1;
	m_nTimeStart	= 0;
	m_nBegin		= 0;
	m_nEnd			= 0;
	m_nTimes		= 0;
	m_nCounter		= 1;
	m_nTick			= 0;
	m_sHigh			= 17;
	m_nPage			= 0;
	m_nMaxTime		= 0;
	m_nCursorTime	= 0;
	m_pValueTime	= NULL;
	m_pLineArray	= NULL;
	m_pCurLine		= NULL;
	m_wdTickLine	= 2;
	m_crTickLine	= RGB(255, 0, 0);

	m_SpeedLevel[0] = 10000;// 10 seconds	(0.1 cycle per second)
	m_SpeedLevel[1] = 5000;	// 5 seconds	(0.2 cycle per second)	
	m_SpeedLevel[2] = 2500;	// 2.5 seconds	(0.4 cycle per second)
	m_SpeedLevel[3] = 2000;	// 2 seconds	(0.5 cycle per second)
	m_SpeedLevel[4] = 1000;	// 1 seconds	(1 cycle per second)
	m_SpeedLevel[5] = 500;	// 0.5 seconds	(2 cycle per second)
	m_SpeedLevel[6] = 250;	// 0.4 seconds	(4 cycle per second)
	m_SpeedLevel[7] = 125;	// 0.125 seconds(8 cycle per second)
	m_SpeedLevel[8] = 50;	// 0.05 seconds (20 cycle per second)
	m_SpeedLevel[9] = 10;   // 0.01 second	(100 cycle per second)

	m_nZoomLevel	= 1;
	m_nZoomStart	= 0;

	m_nStaticXLabels = 0;
}

CRealTime::~CRealTime()
{
	if (m_pLineArray)
		free(m_pLineArray);

	if (m_pValueTime)
		free(m_pValueTime);

	int n = m_LineArray.GetSize();
	for(int i = n-1; i >= 0; i --)
		m_LineArray.RemoveAt(i);
}

RECT CRealTime::TickBarSize()
{
	m_SRect		   = m_PlotRect;
	m_SRect.top	   = m_SRect.top - m_sHigh;
	m_SRect.bottom = m_SRect.top  + m_sHigh - 1;
	m_SRect.left   = m_SRect.left - 7;
	m_SRect.right  = m_SRect.right + 7;

	return m_SRect;
}

bool CRealTime::SetRange(double xmin, double ymin, double xmax, double ymax)
{
	if (ymax <= ymin)
		return false; 

	m_Scale.xmin	= xmin;
    m_Scale.ymin	= ymin;
    m_Scale.xmax	= xmax;
    m_Scale.ymax	= ymax;
	return true; 
}

bool CRealTime::InitialSetting(double CycsPerSec, DWORD StartTime, DWORD TimeSpan,
							   DWORD MaxSeconds, int XTick, bool IsForward)
{
	if (!IsForward && 1000 * TimeSpan > StartTime)
	{
		int ret = MessageBox(NULL, "<1000 * TimeSpan - StartTime> must be positive value\nin initial backward drawing."
							 " Changed to foreward drawing?", 
							 "From InitialSetting() ...", MB_YESNO);
		if (ret == IDYES)
			IsForward = true;
		else
			return false;
	}

	assert(StartTime >= 0);
	assert(TimeSpan >= 30);
	assert(TimeSpan >= (DWORD)XTick);
	assert(MaxSeconds >= TimeSpan);
	MaxSeconds = TimeSpan * (MaxSeconds / TimeSpan);
	assert(CycsPerSec >= 0.09999 && CycsPerSec <= 10.00001f);

	if (MaxSeconds == TimeSpan ) MaxSeconds ++;
	m_nDataPerLine	= (DWORD)(0.5 + MaxSeconds * CycsPerSec + 1);

	m_pValueTime	= (DWORD*)calloc(m_nDataPerLine, sizeof(DWORD));
	if (!m_pValueTime)	return false;

	m_nMaxPages			= MaxSeconds / TimeSpan;
	m_nTimeSpan			= TimeSpan;
	m_nXTicks			= XTick;
	m_nCyclesPerSec		= CycsPerSec;
	m_nCounter			= (int)CycsPerSec;
	m_bIsForwardDrawing	= IsForward;
	m_nSTicks			= (int)(0.5 + TimeSpan * CycsPerSec);

	if (IsForward)
	{
		m_Scale.xmin = StartTime;
		m_Scale.xmax = StartTime + 1000. * TimeSpan;
	}
	else
	{
		m_Scale.xmin = StartTime - 1000. * TimeSpan;
		m_Scale.xmax = StartTime;
	}

	assert(m_Scale.xmin >= 0);
	assert(m_Scale.xmax >= 0);
	assert(m_Scale.xmax - m_Scale.xmin > 0);
	m_TimeToDraw = (DWORD)(0.5 + (m_Scale.xmax - m_Scale.xmin) / 1000 * m_nCyclesPerSec + 1);

	m_pLineArray = (POINT*)calloc(m_TimeToDraw, sizeof(POINT));
	if (!m_pLineArray)	
	{
		free(m_pValueTime);
		return false;
	}

	m_nMin = (DWORD)m_Scale.xmin;
	m_nMax = (DWORD)m_Scale.xmax;
	SetTimeRange(m_nMin, m_nMax);
	m_bAutoScrollX = true;
	
	/// LEGEND Feature ///
	EnableLegend(false);
	m_bLegendShadow = false;

	return	true;
}

void CRealTime::SetTimeRange(DWORD FromTime, DWORD ToTime)
{
	assert(ToTime - FromTime > 0);
	m_Scale.xmin = FromTime;
	m_Scale.xmax = ToTime;
	m_Scale.dx   = (m_Scale.xmax - m_Scale.xmin) / PX;
}

void CRealTime::Reset()
{
	if (m_bAutoScrollX)
		SetTimeRange(m_nMin, m_nMax);
	else
	{
		if (m_nEnd == m_nDataPerLine)
			m_nFrom = m_nBegin + m_nPage * (m_TimeToDraw - 1);
		else
			m_nFrom = m_nBegin + m_nPage * (m_TimeToDraw - 1); // -1 ;
		m_nFrom %= m_nDataPerLine;
		m_nTo    = m_nFrom + m_TimeToDraw - 1;
		m_nTo   %= m_nDataPerLine;
		SetTimeRange(m_pValueTime[m_nFrom], m_pValueTime[m_nTo]);
	}
}

bool CRealTime::AddALine(COLORREF color, double low, double high, 
						 const char* name, const char* desc, const char* unit, 
						 double min, double max, int style, int width)
{
	m_LineArray.Add(m_line);
	int n = m_LineArray.GetSize() - 1;

	// support only three styles : PS_SOLID, PS_DASH, PS_DOT
	if (style < PS_SOLID) style = PS_SOLID;
	if (style > PS_DOT) style	= PS_DOT;

	m_LineArray[n].m_nColor			= color;
	m_LineArray[n].m_dScaleLow		= low;			
	m_LineArray[n].m_dScaleHigh		= high;
	m_LineArray[n].m_sName			= name;
	m_LineArray[n].m_sDescription	= desc;	
	m_LineArray[n].m_sUnit			= unit;		
	m_LineArray[n].m_dMin			= min;			
	m_LineArray[n].m_dMax			= max;			
	m_LineArray[n].m_nLineStyle		= style;
	m_LineArray[n].m_nLineWidth		= width;
	//m_LineArray[n].m_marker.			= NULL;

	if (!m_LineArray[n].Reset(m_nDataPerLine))
	{
		m_LineArray.RemoveAt(n);
		return false;
	}

	return true;
}

bool CRealTime::InsertALine(int index, COLORREF color, double low, double high, 
						 const char* name, const char* desc, const char* unit, 
						 double min, double max, int style, int width)
{
	int n = m_LineArray.GetSize();
	if (index < 0 || index > n)
		return false;

	m_LineArray.InsertAt(index, m_line);

	if (style < PS_SOLID) style = PS_SOLID;
	if (style > PS_DOT) style	= PS_DOT;

	m_LineArray[index].m_nColor			= color;
	m_LineArray[index].m_dScaleLow		= low;			
	m_LineArray[index].m_dScaleHigh		= high;
	m_LineArray[index].m_sName			= name;
	m_LineArray[index].m_sDescription	= desc;	
	m_LineArray[index].m_sUnit			= unit;		
	m_LineArray[index].m_dMin			= min;			
	m_LineArray[index].m_dMax			= max;			
	m_LineArray[index].m_nLineStyle		= style;
	m_LineArray[index].m_nLineWidth		= width;

	if (!m_LineArray[index].Reset(m_nDataPerLine))
	{
		m_LineArray.RemoveAt(index);
		return false;
	}

	return true;
}

bool CRealTime::RemoveALine(int Index)
{
	int n = m_LineArray.GetSize();
	if (n <= 0 || n <= Index)
		return false;

	m_LineArray.RemoveAt(Index);
	return true;
}

void CRealTime::DrawXAxisTimeTicks(int x, int y, DWORD& ticks)
{
	sprintf(m_sTempStr, "%02d:%02d:%02d", ticks/3600, (ticks/60)%60, ticks%60);
	PrintString(x, y, 0, m_sTempStr);	
}

void CRealTime::XAxis()
{
	int CurrentTick = 0;
	char TempStr[30];
		
	m_nStep = (m_TimeToDraw > m_nEnd) ? m_nEnd : m_TimeToDraw;
	
	for(int i = 0; i < m_nStep; i ++) 
	{
		DWORD sec = (DWORD)(m_Scale.xmin + i * 1000 / m_nCyclesPerSec + 0.5);
		DWORD ticks = sec / 1000;
		
		xb = xe = (int)(GL + (sec - m_Scale.xmin) / m_Scale.dx);

		/// draw X labels feature///
		if (m_nStaticXLabels)
		{
			DWORD RTticks;
			SYSTEMTIME lpSystemTime;
			GetLocalTime(&lpSystemTime);			
			RTticks = lpSystemTime.wSecond + 60*lpSystemTime.wMinute + 3600*lpSystemTime.wHour;

			// draw LEFT label
			if (i == 0)
			{
				SetStringAlign(LEFT, TOP);

				RTticks = RTticks - m_nTimeSpan*(m_nZoomLevel);

				if (m_nStaticXLabels == 1)
					ShowTime(xb, GB + m_bM / 5, "", RTticks);
				else
				{
					sprintf(TempStr, "%.d minutes ago", m_nZoomLevel); 
					PrintString(xb, GB + m_bM / 5, 0, TempStr);
//					ShowTime(xb, GB + m_bM / 2, "", RTticks);
				}
			}
			// draw RIGHT label	
			if (i == m_nStep-1)
			{
				if (m_TimeToDraw <= m_nEnd)
					SetStringAlign(RIGHT, TOP);
				else
					SetStringAlign(CENTER, TOP);
				
				if (m_nStaticXLabels == 1)
					ShowTime(xb, GB + m_bM / 5, "", RTticks);			
				else
				{
/*					sprintf(TempStr, "now"); 
//					ShowTime(xb, GB + m_bM / 2 , "", RTticks);
					PrintString(xb, GB + m_bM / 5, 0, TempStr);
*/
				}
			}
		}

		if ((sec % 1000) == 0 && ((sec / 1000) % (m_nTimeSpan / m_nXTicks)) == 0)
		{
			// BUGGG - перескоки
			if (i == 0)
				SetStringAlign(LEFT, TOP);
			else if (i >= m_nStep - (int)m_nCyclesPerSec - 1 && m_TimeToDraw <= m_nEnd)
				SetStringAlign(RIGHT, TOP);
			else
				SetStringAlign(CENTER, TOP);

			ye = GB + m_bM / 7;			

			/// draw X labels feature///
			if (!m_nStaticXLabels)
			{	
				// 06.01.2005
				CurrentTick++;
				if (m_nZoomLevel>1)
					// new = old - (number <from end>)*length*
					ticks = ticks - (m_nXTicks - CurrentTick)*(m_nTimeSpan / m_nXTicks)*(m_nZoomLevel-1);

				DrawXAxisTimeTicks(xb, GB + m_bM / 5, ticks);
			}
		}
		else 
			ye = GB + m_bM / 14;

		yb=GB;
		if (!(sec % 1000) && i > 0 && i < m_nStep - 1)
			DrawLine(xb, yb, xe, ye);
	}

//	PrintTime();
}

void CRealTime::Grid()
{
	HPEN hPen	= ::CreatePen(PS_SOLID, 0, m_nGridColor);
	HPEN hOldPen = (HPEN)::SelectObject(m_hDC, hPen);

	m_nStep = m_TimeToDraw;//(m_TimeToDraw > m_nEnd) ? m_nEnd : m_TimeToDraw;
	for(int i = 0; i < m_nStep; i ++) 
	{
		DWORD sec = (DWORD)(m_Scale.xmin + i * 1000 / m_nCyclesPerSec + 0.5);
		xb = xe = (int)(GL + (sec - m_Scale.xmin) / m_Scale.dx);
		if (xb == GL)
			xb = xe = GL + 1;
		if ((sec % 1000) == 0 && ((sec / 1000) % (m_nTimeSpan / m_nXTicks)) == 0)
			DrawLine(xb, GT + 1, xe, GB - 1);
	}

	for(int i = 1; i < YGridTicks; i ++) 
	{
		yb = ye = GT + (int)(1.0 * i * (GB-GT) / YGridTicks);
		DrawLine(GL + 1, yb, GR - 1, ye);
	}

	::SelectObject(m_hDC, hOldPen);
	::DeleteObject(hPen);
}

void CRealTime::DrawRealTimeLines()
{
	int n = m_LineArray.GetSize();
	if (m_bLegendShadow)
		DrawShadow(n);

	for(int i = 0; i < n; i ++)
	{
		m_pCurLine = &m_LineArray[i];
		DrawCurrentLine();

		if (m_bEnableLegend)
		{
			m_CurPen = ::CreatePen(m_pCurLine->m_nLineStyle, m_pCurLine->m_nLineWidth, m_pCurLine->m_nColor);
			m_OldPen = (HPEN)::SelectObject(m_hDC, m_CurPen);
			CGraphics::Legend(m_pCurLine->m_nColor, i + 1, m_pCurLine->m_sName.GetChar());
			::SelectObject(m_hDC, m_OldPen);
			::DeleteObject(m_CurPen);
		}
	}
	/// TickLine feature ///
//	DrawTickLine();
}

void CRealTime::DrawCurrentLine()
{
	if (m_nEnd < 2)
		return;

	m_nP = 0;
	DWORD nB, nE;

	m_pCurLine->m_dbMaxVisibleValue = 0;
	////////////////////////////////////////////////////////////////
	// if Zoom is On then recalculate values of m_pZoomedValues array
	if (m_nZoomLevel>1)
	{
		// recalculate after every m_nZoomLevel steps
		if ((m_nEnd - m_nZoomStart) % m_nZoomLevel == 0)
			m_pCurLine->ChangeZoomedValues(m_nZoomLevel,m_nTimeSpan, m_nEnd);
	}

	if (m_bAutoScrollX)
	{
		nB = m_nBegin;
		nE = m_nEnd;

		// forward drawing: something like "*******------" 
		// or "------******------", or "------******"
		if (nB == 0 && nE <= m_TimeToDraw)
		{
			m_nTimeStart = nB;
			ForwardDraw(nB, nE);
		}
		else if (nB == 0 && nE > m_TimeToDraw)
		{
			nB = nE - m_TimeToDraw;
			m_nTimeStart = nB;
			ForwardDraw(nB, nE);
		}
		else if (nB >= m_TimeToDraw)
		{
			// MaxSeconds истекли ...
			m_nTimeStart = nB - m_TimeToDraw;
			ForwardDraw(nB - m_TimeToDraw, nB);
		}
		// backward drawing: something like "******--------******"
		else
		{
			m_nTimeStart = nE - m_TimeToDraw + nB;
			BackwardDraw(nB, nE);
		}
	}
	else
	{
		nB = m_nFrom;
		nE = m_nTo + 1;
		if (nB <= m_nDataPerLine - m_TimeToDraw)
		{
			ForwardDraw(nB, nE);
		}
		else
		{
			ForwardDraw(nB, m_nDataPerLine);
			ForwardDraw(0, nE);
		}
	}

	if(m_nP > 1)
		PolylineAA(m_hDC, m_pLineArray, m_nP, m_pCurLine->m_nColor);


/*
	//////////////////////////////////////////////
	// Draw markers
	// mrkr //
	int n = m_pCurLine->m_MarkerArray.GetSize();
	
	for(int i = 0; i< n; i++)
		TestPrint(
			CalculateX(m_pCurLine->m_MarkerArray[i].m_nMarkerX - m_nTimeStart), 
			CalculateY(m_pCurLine->m_MarkerArray[i].m_nMarkerY), 
			m_pCurLine->m_MarkerArray[i].m_sMarkerText);
*/
}

int CRealTime::ForwardDraw(int nB, int nE)
{
	///////////////////////////////////////////
	// Use m_pZoomedValues array if ZoomLevel>1
	if ((m_nZoomLevel>1) && (m_pCurLine->m_pZoomedValues))

	for(DWORD i = 0; i < m_nTimeSpan; i ++)
	{
		if (m_pCurLine->m_pZoomedValues[i].bFlag)
		{
			// Draw markers
			// 09.01.2005
			if (m_pCurLine->m_pZoomedValues[i].bMarker)
				TestPrint(CalculateX(i), 
						  CalculateY((int)m_pCurLine->m_pZoomedValues[i].YValue), 
						  m_pCurLine->m_pZoomedValues[i].sMarkerText);

			//m_Pt.x = (int)(GL + ((i - m_Scale.xmin) / m_Scale.dx));
			m_Pt.x = (int)CalculateX(i);

			m_Pt.y = (int)(GB - ((m_pCurLine->m_pZoomedValues[i].YValue - m_Scale.ymin) / m_Scale.dy));

			m_pCurLine->m_dbMaxVisibleValue = max(m_pCurLine->m_pZoomedValues[i].YValue, m_pCurLine->m_dbMaxVisibleValue);

			if(m_Pt.x >= GL && m_Pt.x <= GR)
			{
				assert(m_nP<m_nDataPerLine);
				m_pLineArray[m_nP].x = m_Pt.x;
				m_pLineArray[m_nP].y = m_Pt.y;
				m_nP++;
			}
			else // something wrong
				assert(false);
		}
	}		

	else
	///////////////////////////////////////////
	// Use m_pValues array without Zoom
	for(int i = nB; i < nE; i ++)
	{
		if (m_pCurLine->m_pValues[i].bFlag)
		{
			// Draw markers
			// 09.01.2005
			if (m_pCurLine->m_pValues[i].bMarker)
				TestPrint(CalculateX(i-m_nTimeStart), 
						  CalculateY((int)m_pCurLine->m_pValues[i].YValue), 
						  m_pCurLine->m_pValues[i].sMarkerText);

			m_Pt.x = (int)(GL + ((m_pValueTime[i] - m_Scale.xmin) / m_Scale.dx));

			m_Pt.y = (int)(GB - ((m_pCurLine->m_pValues[i].YValue - m_Scale.ymin) / m_Scale.dy));
			
			m_pCurLine->m_dbMaxVisibleValue = max(m_pCurLine->m_pValues[i].YValue, m_pCurLine->m_dbMaxVisibleValue);

			if(m_Pt.x >= GL && m_Pt.x <= GR)
			{
				assert(m_nP<m_nDataPerLine);
				m_pLineArray[m_nP].x = m_Pt.x;
				m_pLineArray[m_nP].y = m_Pt.y;
				m_nP++;
			}
			else // something wrong
				assert(false);
		}
	}

	return m_nP;
}

int CRealTime::BackwardDraw(int nB, int nE)
{
	//-----------------------------******
	ForwardDraw(nE - m_TimeToDraw + nB, nE);	
	nE = nB;
	nB = 0;
	//******-----------------------------
	ForwardDraw(nB, nE);

	return m_nP;
}

void CRealTime::UpdateTimeRange(DWORD& vtime)
{
	if ((int)m_nCyclesPerSec == 0)
		m_nTimes += (int)(0.5 + 1 / m_nCyclesPerSec);
	else
	{
		if (m_nCounter == (int)m_nCyclesPerSec)
		{
			m_nTimes ++;
			m_nCounter = 1;
		}
		else
			m_nCounter ++;
	}

	if (m_nEnd == m_nDataPerLine)
	{
		if (m_nBegin == m_nDataPerLine)	m_nBegin = 0;
		m_pValueTime[m_nBegin++] = vtime;
	}
	else
		m_pValueTime[m_nEnd++] = vtime;

	if (vtime > (DWORD)m_Scale.xmax)
	{
		DWORD span	 = (DWORD)(m_Scale.xmax - m_Scale.xmin);
		m_nMin = vtime - span; 
		m_nMax = vtime;
	}

	if(m_bAutoScrollX)
		SetTimeRange(m_nMin, m_nMax);
	else
	{
		m_nFrom ++; 
		m_nTo ++;

		if (m_nFrom == m_nDataPerLine)
			m_nFrom = 0;
		if (m_nTo == m_nDataPerLine)
			m_nTo = 0;
		
		SetTimeRange(m_pValueTime[m_nFrom], m_pValueTime[m_nTo]);
	}
}

DWORD CRealTime::GetCursorTime()
{
	DWORD time;
	if (m_bAutoScrollX)
	{
		if (m_nEnd < (int)m_TimeToDraw) 
		{
			time = m_pValueTime[m_nTick];
		}
		else if (m_nEnd < m_nDataPerLine)
		{
			time = m_pValueTime[m_nEnd - m_TimeToDraw + m_nTick];
		}
		else
		{
			int tv = m_nDataPerLine - m_nSTicks + m_nBegin + m_nTick - 1;
			tv %= m_nDataPerLine;
			time = m_pValueTime[tv];
		}
	}
	else
	{
		int tv = m_nFrom % m_nDataPerLine;
		tv += m_nTick;
		tv %= m_nDataPerLine;
		time = m_pValueTime[tv];
	}

	return time;
}

int CRealTime::GetCursorTimeAndIndex(DWORD& time)
{
	int Index;
	if (m_bAutoScrollX)
	{
		
		if (m_nEnd < (int)m_TimeToDraw) 
		{
			Index = m_nTick;
			time  = m_pValueTime[Index];
		}
		else if (m_nEnd < m_nDataPerLine)
		{
			Index = m_nEnd - m_TimeToDraw + m_nTick;
			time = m_pValueTime[Index];
		}
		else
		{
			int tv = m_nDataPerLine - m_nSTicks + m_nBegin + m_nTick - 1;
			tv %= m_nDataPerLine;
			Index = tv;
			time = m_pValueTime[Index];
		}
	}
	else
	{
		int tv = m_nFrom % m_nDataPerLine;
		tv += m_nTick;
		tv %= m_nDataPerLine;
		Index = tv;
		time = m_pValueTime[Index];
	}

	return Index;
}

void CRealTime::Redraw(HWND hWnd)
{
	RECT rt;
	rt.left   = GL;
	rt.top    = GT - 1;
	rt.right  = GR;
	rt.bottom = m_Rect.bottom - 1;
	::InvalidateRect(hWnd, &rt, FALSE);
}

void CRealTime::DrawTickLine()
{
	POINT pt;
	
	if (m_nEnd < (int)m_TimeToDraw)
	{
		if (m_nTick == 0) return;
		else if (!m_bIsForwardDrawing)
			pt.x = GR - 1;
		else
			pt.x = GL + PX * (m_nTick - 1) / (m_TimeToDraw - 1);
	}
	else
		pt.x = GL + PX * m_nTick / (m_TimeToDraw - 1);

	if (pt.x == GL)
		pt.x ++;
	if (pt.x == GR)
		pt.x --;

	pt.y = GT + 1;

	m_CurPen = ::CreatePen(PS_SOLID, m_wdTickLine, RGB(0, 0, 0));
	m_OldPen = (HPEN)::SelectObject(m_hDC, m_CurPen);
	::MoveToEx(m_hDC, pt.x + 1, pt.y, NULL);
	pt.y = GB - 1;
	::LineTo(m_hDC, pt.x + 1, pt.y);
	::SelectObject(m_hDC, m_OldPen);
	::DeleteObject(m_CurPen);

	m_CurPen = ::CreatePen(PS_SOLID, m_wdTickLine, m_crTickLine);
	m_OldPen = (HPEN)::SelectObject(m_hDC, m_CurPen);
	::MoveToEx(m_hDC, pt.x, pt.y, NULL);
	pt.y = GT + 1;
	::LineTo(m_hDC, pt.x, pt.y);
	::SelectObject(m_hDC, m_OldPen);
	::DeleteObject(m_CurPen);

}

void CRealTime::SetPrintTime(DWORD time, int flag)
{
	if (flag == MaxTime)
		m_nMaxTime = time / 1000;
	else
		m_nCursorTime = time / 1000;
}

void CRealTime::PrintTime()
{
	int n = 5;
	if (m_bPrinting)	n *= m_nPrintScale;
	n = m_Rect.bottom - n - 1;
	SetStringAlign(LEFT, BOTTOM);
	ShowTime(GL, n, "Cursor time", m_nCursorTime);
	SetStringAlign(RIGHT, BOTTOM);
	ShowTime(GR, n, "Total lapse time", m_nMaxTime);
}

void CRealTime::ShowTime(int x, int y, const char* Tag, DWORD& cTime)
{
	if (m_nCyclesPerSec > 0.9999 && m_nCyclesPerSec < 1.0001)
		sprintf(m_sTempStr, "%s %02d:%02d:%02d", Tag, 
				cTime / 3600, 
				(cTime / 60) % 60, 
				cTime % 60);
	else
		sprintf(m_sTempStr, "%s (%02d:%02d:%02d / %d:%d:%d)", Tag,
			    ((DWORD)(m_nCyclesPerSec * cTime)) / 3600, 
				(((DWORD)(m_nCyclesPerSec * cTime)) / 60) % 60, 
				((DWORD)(m_nCyclesPerSec * cTime)) % 60, 
				cTime / 3600, (cTime / 60) % 60, cTime % 60);
	PrintString(x, y, 0, m_sTempStr);	
}

bool CRealTime::IsLegendEnabled()
{
	return m_bEnableLegend;
}

int CRealTime::CalculateY(int value)
{
	double _value = m_Scale.ymax - value;
	return (int)(GT + ((GB-GT)/m_Scale.ymax) * _value);
}

int CRealTime::CalculateValueByY(int Ycoord)
{
	return (int)(m_Scale.ymax - (Ycoord-GT)*(m_Scale.ymax/(GB-GT)));	
}

int CRealTime::CalculateX(int value)
{
	//DWORD sec = (DWORD)(m_Scale.xmin + value * 1000 / m_nCyclesPerSec + 0.5);
	//return (int)(GL + (sec - m_Scale.xmin) / m_Scale.dx);
	return (int)(GL + (value * 1000 / m_nCyclesPerSec + 0.5) / m_Scale.dx);
}

int CRealTime::CalculateValueByX(int Xcoord)
{
	return (int)(((Xcoord - GL)*m_Scale.dx - 0.5)*m_nCyclesPerSec/1000);
}

void CRealTime::ChangeZoomLevel(bool increase)
{
	(increase) ? m_nZoomLevel++ : m_nZoomLevel--;

	if (m_nZoomLevel<1) m_nZoomLevel=1;

	m_nZoomStart = m_nEnd;
//	m_LineArray[0].ChangeZoomedValues(m_nZoomLevel,m_nTimeSpan, m_nEnd);	
}

//============================
bool CRealTime::IsPointInLineArray(int XValue, int YValue)
{
	int n = m_LineArray.GetSize();

	for(int i = 0; i < n; i ++)
	{
		m_pCurLine = &m_LineArray[i];
//if (m_nZoomLevel<1)
		
		int lValueInLine = (int)m_pCurLine->m_pValues[m_nTimeStart+XValue].YValue;

		if (lValueInLine == YValue)
			return true;
	}
	return false;
}