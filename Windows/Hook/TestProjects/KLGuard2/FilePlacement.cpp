// FilePlacement.cpp: implementation of the CFilePlacement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FilePlacement.h"
#include "AutoPTCHAR.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilePlacement::CFilePlacement()
{
	m_SandBox.Empty();
	ReloadEnvironment();
}

CFilePlacement::~CFilePlacement()
{

}

void CFilePlacement::SetSandBoxPalcement(CString* pSandBoxPath)
{
	m_SandBox = *pSandBoxPath;
}

void CFilePlacement::GetEnvironmentString(CString* pMember, PTCHAR EnvVar)
{
	DWORD var_len;
	PWCHAR pwch_buf;

	pMember->Empty();
	
	var_len = GetEnvironmentVariable(EnvVar, NULL, 0);
	if (var_len != 0)
	{
		pwch_buf = pMember->GetBuffer(var_len);
		if (pwch_buf != NULL)
		{
			var_len = GetEnvironmentVariable(EnvVar, pwch_buf, var_len);
			pMember->ReleaseBuffer();
			pMember->MakeUpper();
		}
	}
}

CString CFilePlacement::ExpandEnvironmentString(PTCHAR ptchString)
{
	CString strtmp;
	strtmp.Empty();
	CString strExpanded;
	{
		AutoPTCHAR auptchExpanded(&strtmp, 4096);
		if (auptchExpanded.IsValid())
		{
			ExpandEnvironmentStrings(ptchString, auptchExpanded, auptchExpanded.GetBufferLength());
			strExpanded = auptchExpanded.GetPTCH();
			strExpanded.MakeUpper();
		}
	}

	return strtmp;
}

void CFilePlacement::ReloadEnvironment()
{
	GetEnvironmentString(&m_SysRoot, _T("SystemRoot"));
	GetEnvironmentString(&m_ProgramFiles, _T("ProgramFiles"));
	GetEnvironmentString(&m_Temp, _T("TEMP"));
	
	m_PrefetchFolder = ExpandEnvironmentString(_T("%SystemRoot%\\prefetch"));
	m_PrefetchFolder.MakeUpper();
}

bool CFilePlacement::IsThisFolder(CString* pFolder, PWCHAR pwchFileName)
{
	CString strtmp;
	strtmp = pwchFileName;
	
	strtmp = strtmp.Left(pFolder->GetLength());
	strtmp.MakeUpper();

	if (!strtmp.IsEmpty())
	{
		 if (strtmp == *pFolder)
			 return true;
	}

	return false;
}

bool CFilePlacement::IsFolderPrefetch(PWCHAR pwchFileName)
{
	return IsThisFolder(&m_PrefetchFolder, pwchFileName);
}

_eFilePlacement CFilePlacement::GetFilePlacement(PWCHAR pwchFileName)
{
	if (IsThisFolder(&m_SysRoot, pwchFileName))
		 return _placement_system_folder;

	if (IsThisFolder(&m_ProgramFiles, pwchFileName))
		 return _placement_program_files;

	if (IsThisFolder(&m_Temp, pwchFileName))
		return _placement_temp;

	if (IsThisFolder(&m_SandBox, pwchFileName))
		return _placement_sandbox;

	return _placement_other;
}
