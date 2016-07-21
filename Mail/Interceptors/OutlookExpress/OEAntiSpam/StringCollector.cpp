// StringCollector.cpp: implementation of the CStringCollector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StringCollector.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStringCollector::CStringCollector(LPCTSTR szItemsDelimiter) : 
	m_bIsItemFirst(true),
	m_strItemsDelimiter(szItemsDelimiter)
{
}

CStringCollector::~CStringCollector()
{
}

bool CStringCollector::AddItem(LPCTSTR szItem)
{
	if (!m_bIsItemFirst)
		m_strHolder += m_strItemsDelimiter;

	m_strHolder += szItem;

	m_bIsItemFirst = false;
	return TRUE;
}

bool CStringCollector::ReInit()
{
	m_bIsItemFirst = true;
	m_strHolder.swap(tstring());
	return TRUE;
}
