// OwnSync.cpp: implementation of the COwnSync class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OwnSync.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COwnSync::COwnSync()
{
	InitializeCriticalSection(&m_Sync);
}

COwnSync::~COwnSync()
{
	DeleteCriticalSection(&m_Sync);
}

void COwnSync::LockSync()
{
	EnterCriticalSection(&m_Sync);
}

void COwnSync::UnLockSync()
{
	LeaveCriticalSection(&m_Sync);
}