// SelfCopyHistory.cpp: implementation of the CSelfCopyHistory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SelfCopyHistory.h"
#include "stuff/mm.h"
#include "stuff/servfuncs.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define _INVALID_STR_IDX	-1
#define _EMPTY_STR_IDX		0

CSelfCopyHistory::CSelfCopyHistory()
{
	m_nNextIdx = 1;
	m_IdxToStr.reserve(1);
}

CSelfCopyHistory::~CSelfCopyHistory()
{
	for (_tvecStrIdx::iterator _it = m_IdxToStr.begin(); _it != m_IdxToStr.end(); _it++)
	{
		tStrWithLo strwl = *_it;
		FreeStr(strwl.m_pwchStr);
		FreeStr(strwl.m_pwchStrLo);
	}
}

int CSelfCopyHistory::GetStrIdx(PWCHAR pwchStr, bool bAllowCreate)
{
	int nRet = _INVALID_STR_IDX;

	if (NULL == pwchStr)
		return _EMPTY_STR_IDX;

	PWCHAR pwchBuff = NULL;
	_autoFree aupBuf(&pwchBuff);

	PWCHAR buf[MAX_PATH + 1];
	PWCHAR pwchOp = NULL;

	int len = _WSTR_LEN_B(pwchStr);
	if (len > MAX_PATH * sizeof(WCHAR))
		pwchOp = (PWCHAR) buf;
	else
	{
		pwchBuff = (PWCHAR) global_Alloc(len);
		pwchOp = pwchBuff;
	}

	if (NULL == pwchOp)
		return nRet;

	memcpy(pwchOp, pwchStr, len);
	CharLower(pwchOp);

	nRet = FindStr(pwchOp);
	if (_INVALID_STR_IDX == nRet && bAllowCreate)
		nRet = RegisterStr(pwchStr, pwchOp);

	return nRet;
}

bool CSelfCopyHistory::AddEvent(PWCHAR pwchImagePath, PWCHAR pwchDestinationPath, PWCHAR ParentImagePath, DWORD dwOffset)
{
	tSelfCopyHistory scHis;
	scHis.m_ImagePathIdx = GetStrIdx(pwchImagePath, true);
	scHis.m_DestinationPathIdx = GetStrIdx(pwchDestinationPath, true);
	scHis.m_ParentImagePathIdx = GetStrIdx(ParentImagePath, true);

	if (_INVALID_STR_IDX == scHis.m_ImagePathIdx || 
		_INVALID_STR_IDX == scHis.m_DestinationPathIdx ||
		_INVALID_STR_IDX == scHis.m_ParentImagePathIdx)
	{
		return false;
	}

	scHis.m_dwOffset = dwOffset;
	m_SelfCopyHistory.push_back(scHis);
	
	return true;
}

int CSelfCopyHistory::FindStr(PWCHAR pwchLowerStr)
{
	for (_tvecStrIdx::iterator _it = m_IdxToStr.begin(); _it != m_IdxToStr.end(); _it++)
	{
		tStrWithLo strwl = *_it;
		if (0 == wcscmp(strwl.m_pwchStrLo, pwchLowerStr))
			return strwl.m_Idx;
	}

	return _INVALID_STR_IDX;
}

int CSelfCopyHistory::RegisterStr(PWCHAR pwchStr, PWCHAR pwchLowerStr)
{
	int nRet = _INVALID_STR_IDX;
	PWCHAR pwchBuff = NULL;
	PWCHAR pwchBuffLo = NULL;

	pwchBuff = GetCopyStr(pwchStr);
	pwchBuffLo = GetCopyStr(pwchLowerStr);

	if (NULL == pwchBuff || NULL == pwchBuffLo)
	{
		FreeStr(pwchBuff);
		FreeStr(pwchBuffLo);
		return _INVALID_STR_IDX;
	}

	tStrWithLo strwl;

	strwl.m_pwchStr = pwchBuff;
	strwl.m_pwchStrLo = pwchBuffLo;

	nRet = m_nNextIdx;
	strwl.m_Idx  = m_nNextIdx++;
	m_IdxToStr.insert(m_IdxToStr.begin(), strwl);
	
	return nRet;
}

bool CSelfCopyHistory::Contain(PWCHAR pwchStr)
{
	if (NULL == pwchStr)
		return false;

	PWCHAR pwchLowerStr = GetLowerStr(pwchStr);
	if (NULL == pwchLowerStr)
		return false;
	_autoFree aupLower(&pwchLowerStr);
	
	for (_tvecStrIdx::iterator _it = m_IdxToStr.begin(); _it != m_IdxToStr.end(); _it++)
	{
		tStrWithLo strwl = *_it;
		if (0 == wcscmp(strwl.m_pwchStrLo, pwchLowerStr))
			return true;
	}

	return false;
}

bool CSelfCopyHistory::IsCopy2Path(PWCHAR pwchPath, PWCHAR* ppwchImagePath)
{
	int pathlen, longpathlen = 0;
	if (NULL == pwchPath)
		return false;
	if (ppwchImagePath)
		*ppwchImagePath = NULL;

	PWCHAR pwchLongPath = g_Service.GetLongPathName(pwchPath);
	_autoFree aupLong(&pwchLongPath);
	pathlen = wcslen(pwchPath);
	if (pwchLongPath)
		longpathlen = wcslen(pwchLongPath);

	for (_tvecStrIdx::iterator _it = m_IdxToStr.begin(); _it != m_IdxToStr.end(); _it++)
	{
		tStrWithLo strwl = *_it;
		bool bCmpResult;
		bCmpResult = (0 == wcsnicmp(strwl.m_pwchStr, pwchPath, pathlen));
		if (!bCmpResult && pwchLongPath)
			bCmpResult = (0 == wcsnicmp(strwl.m_pwchStr, pwchLongPath, longpathlen));
		if (bCmpResult)
		{
			for (_tvecSelfCopyHistory::iterator _it2 = m_SelfCopyHistory.begin(); _it2 != m_SelfCopyHistory.end(); _it2++)
			{
				if (_it2->m_DestinationPathIdx == strwl.m_Idx)
				{
					if (ppwchImagePath)
					{
						for (_tvecStrIdx::iterator _it = m_IdxToStr.begin(); _it != m_IdxToStr.end(); _it++)
						{
							tStrWithLo strwl = *_it;
							if (strwl.m_Idx == _it2->m_ImagePathIdx)
								*ppwchImagePath = strwl.m_pwchStr;
							break;
						}
					}
					return true;
				}
			}
		}
	}
	return false;
}