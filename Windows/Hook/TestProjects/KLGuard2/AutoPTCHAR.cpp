// AutoPTCHAR.cpp: implementation of the CAutoPTCHAR class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoPTCHAR.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AutoPTCHAR::AutoPTCHAR(CString* pstr, size_t len)
{
	m_pstrBase = pstr;
	m_ptch = m_pstrBase->GetBuffer(len);
	m_BufferLen = len;
}
AutoPTCHAR::~AutoPTCHAR()
{
	m_pstrBase->ReleaseBuffer();
}

