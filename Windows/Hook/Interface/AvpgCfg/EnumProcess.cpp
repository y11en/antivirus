// EnumProcess.cpp: implementation of the CEnumProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "avpgcfg.h"
#include "EnumProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnumProcess::CEnumProcess(CStringList* pStringList)
{
	m_pStringList = pStringList;
}

CEnumProcess::~CEnumProcess()
{

}

BOOL CEnumProcess::OnProcess(LPCTSTR lpszFileName, DWORD ProcessID)
{
/*
  убрать расширение и путь
*/
	if (ProcessID == 0)
		m_pStringList->AddTail("*");
	else
		m_pStringList->AddTail(lpszFileName);

	return TRUE;
}