// RegHistory.cpp: implementation of the CRegHistory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegHistory.h"

#include "stuff//mm.h"
#include <shlwapi.h>
#include <shellapi.h>
#include "stuff/servfuncs.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CRegHistory::CRegHistory()
{
}

CRegHistory::~CRegHistory()
{
	for (_tvecStrReg::iterator _it = m_StrReg.begin(); _it != m_StrReg.end(); _it++)
	{
		tStrReg strreg = *_it;
		FreeStr(strreg.m_pwchRegKeyName);
		FreeStr(strreg.m_pwchRegValueName);
		FreeStr(strreg.m_pwchRegValue);
		FreeStr(strreg.m_pwchRegValueReparsedFilename);
	}
}

int CRegHistory::AddEvent(PWCHAR pwchRegKeyName, PWCHAR pwchRegValueName, PWCHAR pwchRegValue, tStrReg* pResult, int nMaxResults)
{	
	int results = 0;
	// remove existing values
	for (_tvecStrReg::iterator _it = m_StrReg.begin(); _it != m_StrReg.end();)
	{
		if (0 == wcsicmp(pwchRegKeyName, _it->m_pwchRegKeyName) 
			&& 0 == wcsicmp(pwchRegValueName, _it->m_pwchRegValueName))
		{
			FreeStr(_it->m_pwchRegKeyName);
			FreeStr(_it->m_pwchRegValueName);
			FreeStr(_it->m_pwchRegValue);
			FreeStr(_it->m_pwchRegValueReparsedFilename);
			_it = m_StrReg.erase(_it);
			continue;
		}
		_it++;
	}

	PWCHAR Params[10];
	int param_count;
	PWCHAR pwchCmdLine = GetCopyStr(pwchRegValue);
	_autoFree afp(&pwchCmdLine);
	if (pwchCmdLine)
		param_count = g_Service.ParseCmdLine(pwchCmdLine, Params, countof(Params));
	else
		param_count = 1;
	Params[0] = pwchRegValue;
	
	// add new values
	for (int cou = 0; cou < param_count; cou++)
	{
		PWCHAR pwchRegKeyNameNew = GetCopyStr(pwchRegKeyName);
		PWCHAR pwchRegValueNameNew = GetCopyStr(pwchRegValueName);
		PWCHAR pwchRegValueNew = GetCopyStr(Params[cou]);

		if (NULL == pwchRegKeyNameNew || NULL == pwchRegValueNameNew || NULL == pwchRegValueNew)
		{
			FreeStr(pwchRegKeyNameNew);
			FreeStr(pwchRegValueNameNew);
			FreeStr(pwchRegValueNew);
			return results;
		}
		tStrReg strreg;
		strreg.m_pwchRegKeyName = pwchRegKeyNameNew;
		strreg.m_pwchRegValueName = pwchRegValueNameNew;
		strreg.m_pwchRegValue = pwchRegValueNew;
		strreg.m_pwchRegValueReparsedFilename = g_Service.ReparseExecutePath(pwchRegValueNew);
		m_StrReg.push_back(strreg);
		if (pResult && results < nMaxResults)
			pResult[results++] = strreg;
	}
	return results;
}

bool CRegHistory::Contain(PWCHAR pwchFileName, tStrReg* pResult)
{
	PWCHAR pwchExecFN = g_Service.ReparseExecutePath(pwchFileName);
	if (pwchExecFN == NULL)
		return false;
	_autoFree aupVal(&pwchExecFN);
	for (_tvecStrReg::iterator _it = m_StrReg.begin(); _it != m_StrReg.end(); _it++)
	{
		if (_it->m_pwchRegValueReparsedFilename == NULL)
			_it->m_pwchRegValueReparsedFilename = g_Service.ReparseExecutePath(_it->m_pwchRegValue);
		if (_it->m_pwchRegValueReparsedFilename != NULL)
		{
			if (0 == wcsicmp(_it->m_pwchRegValueReparsedFilename, pwchExecFN))
			{
				if (pResult)
					*pResult = *_it;
				return true;
			}
		}
	}

	return false;
}

