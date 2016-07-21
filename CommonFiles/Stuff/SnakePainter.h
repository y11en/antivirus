////////////////////////////////////////////////////////////////////
//
//  SnakePainter.h
//  SnakePainter mixed class for "snake"-like gradients and images showing
//  AVP generic purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __SNAKEPAINTER_H__
#define __SNAKEPAINTER_H__

#include <windows.h>


class CSnakePainterThread;

/////////////////////////////////////////////////////////////////////////////
// CSnakePainter class
// ---
class CSnakePainter {
// Construction
public:
	CSnakePainter();
	virtual ~CSnakePainter();

// Attributes
public:

// Operations
public:

// Implementation
public:
	int GetMax();
	void ResetProgress();
	int GetTail();
	COLORREF GetGradientEndColor();
	COLORREF GetGradientStartColor();
	COLORREF GetBkColor();
	BOOL IsReverse();
	int GetSize();
	int GetStep();
	int GetPos();
	void SetReversable( BOOL bReversable );
	void SetMax(int nMax);
	void SetStep(int nStep);
	void SetPos(int nPos);
	int StepIt();
	void SetGradientFill(COLORREF clrStart,COLORREF clrEnd);
	void SetBkColor(COLORREF clrFG);
	void SetSize(int nSize);
	void SetWndRect( RECT rcWndRect );
	BOOL SetImage( HINSTANCE hResInstance, UINT uiImageID );
	void SetWindow( HWND hWnd );
	void SetTimeout( DWORD dwTimeout );
	void DoPaint();

	void Start();
	void Stop();

	// Generated message map functions
protected:
	void	SetupReverse();
	void	SetupDirect();
	int		SafeStepTail();
	int		SafeStepHead();
	void	DrawImage( HDC hDC, RECT rcRect );
	void	DrawGradient( HDC hDC,RECT rcGrad );

public :
	WNDPROC  m_pOldWndProc;

private:
	CSnakePainterThread *m_pThread;
	BOOL m_bTimeForTail;
	BOOL m_bReverse;
	BOOL m_bReversable;
	BOOL m_bGradient;

	int m_nStep;
	int m_nPos;
	int m_nTail;
	int m_nSize;
	int m_nMax;
	
	COLORREF m_clrBk;
	COLORREF m_clrEnd;
	COLORREF m_clrStart;
	COLORREF m_clrCurEnd;
	COLORREF m_clrCurStart;
	HBITMAP  m_hImage;
	RECT     m_WndRect;
	HWND     m_hWndHost;
	DWORD    m_dwTimeout;
};

#endif __SNAKEPAINTER_H__
