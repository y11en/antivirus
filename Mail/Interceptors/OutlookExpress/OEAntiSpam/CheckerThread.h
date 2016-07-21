// CheckerThread.h: interface for the CCheckerThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHECKERTHREAD_H__9071D5D7_DD80_41C5_B8E1_4FE8086F1BE5__INCLUDED_)
#define AFX_CHECKERTHREAD_H__9071D5D7_DD80_41C5_B8E1_4FE8086F1BE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../TheBatAntispam/EnsureCleanup.h"
#include "boost/utility.hpp"

class CCheckerThread : public boost::noncopyable
{
public:
	CCheckerThread();
	bool Start();
	bool Stop();

private:
	static DWORD WINAPI ThreadProc(LPVOID lpParam);

	CEnsureCloseHandle m_hStopEvent;
	bool m_bIsRunning;
};

#endif // !defined(AFX_CHECKERTHREAD_H__9071D5D7_DD80_41C5_B8E1_4FE8086F1BE5__INCLUDED_)
