// FilterTreeInfo.cpp: implementation of the CFilterTreeInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "avpgcfg.h"
#include "FilterTreeInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilterTreeInfo::CFilterTreeInfo()
{
	m_pFilter = NULL;
	m_nProperty = -1;
}

CFilterTreeInfo::CFilterTreeInfo(PFILTER_TRANSMIT pFilter, int nProperty)
{
	m_pFilter = pFilter;
	m_nProperty = nProperty;
}

CFilterTreeInfo::~CFilterTreeInfo()
{

}
