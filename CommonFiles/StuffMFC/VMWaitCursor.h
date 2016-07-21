////////////////////////////////////////////////////////////////////
//
//  VMWaitCursor.h
//  CVMWaitCursor definition
//  AVP general purposes MFC stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2001
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __VMWAITCURSOR_H__
#define __VMWAITCURSOR_H__

#include <afxwin.h>

// ---
class CVMWaitCursor {
public:
	CVMWaitCursor();
	~CVMWaitCursor();

// Operations
public:
	void Restore();
protected :
	void SetCursorImp();
};

// ---
inline CVMWaitCursor::CVMWaitCursor()  {
	::AfxGetApp()->BeginWaitCursor();
	SetCursorImp();
}

// ---
inline CVMWaitCursor::~CVMWaitCursor()  {
	::AfxGetApp()->EndWaitCursor();
	SetCursorImp();
}

// ---
inline void CVMWaitCursor::Restore() {
	::AfxGetApp()->RestoreWaitCursor();
	SetCursorImp();
}

// ---
inline void CVMWaitCursor::SetCursorImp() {
	CPoint rcPoint;
	::GetCursorPos( &rcPoint );
	::SetCursorPos( rcPoint.x, rcPoint.y );
}

#endif //__VMWAITCURSOR_H__