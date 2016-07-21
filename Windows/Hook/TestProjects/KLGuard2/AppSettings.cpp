// AppSettings.cpp: implementation of the CSandBoxSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AppSettings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSandBoxSettings::CSandBoxSettings()
{
	ReloadSettings();
}

CSandBoxSettings::~CSandBoxSettings()
{

}

void CSandBoxSettings::SetSandBoxPath(CString *pSandBoxPath)
{
	m_SandBoxPath = *pSandBoxPath;
	m_SandBoxPathUpper = *pSandBoxPath;
	m_SandBoxPathUpper.MakeUpper();
}

bool CSandBoxSettings::IsStartFromBox(CString *pSandBoxPath)
{
	if (m_SandBoxPathUpper.IsEmpty())
		return false;

	CString strtmp = pSandBoxPath->Left(m_SandBoxPath.GetLength());
	strtmp.MakeUpper();
	if (strtmp == m_SandBoxPathUpper)
		return true;

	return false;
}

void CSandBoxSettings::ReloadSettings()
{
	CString strtmp = g_pReestr->GetValue(_SandBox_Path, _T(""), false);
	SetSandBoxPath(&strtmp);

	m_bAllowCreateProcess = !!g_pReestr->GetValue(_AllowCreateProcess, 0);

	m_bAllowWrite_SystemFolder = !!g_pReestr->GetValue(_AllowWrite_SystemFolder, 0);
	m_bAllowWrite_ProgramFiles = !!g_pReestr->GetValue(_AllowWrite_ProgramFiles, 0);
	m_bAllowWrite_SandBoxFolder = !!g_pReestr->GetValue(_AllowWrite_SandBoxFolder, 0);
	m_bAllowWrite_TempFolder = !!g_pReestr->GetValue(_AllowWrite_TempFolder, 0);

	m_bAllowWrite_Registry = !!g_pReestr->GetValue(_AllowWrite_Registry, 1);

	m_bLogEvent = !!g_pReestr->GetValue(_LogEvent, 0);
	m_bShowWarning = !!g_pReestr->GetValue(_ShowWarning, 0);
	m_bTerminateProcess = !!g_pReestr->GetValue(_TerminateProcess, 0);
}

void CSandBoxSettings::SaveSettings()
{
	g_pReestr->SetValue(_SandBox_Path, m_SandBoxPath);
	
	g_pReestr->SetValue(_AllowCreateProcess, m_bAllowCreateProcess);

	g_pReestr->SetValue(_AllowWrite_SystemFolder, m_bAllowWrite_SystemFolder);
	g_pReestr->SetValue(_AllowWrite_ProgramFiles, m_bAllowWrite_ProgramFiles);
	g_pReestr->SetValue(_AllowWrite_SandBoxFolder, m_bAllowWrite_SandBoxFolder);
	g_pReestr->SetValue(_AllowWrite_TempFolder, m_bAllowWrite_TempFolder);

	g_pReestr->SetValue(_AllowWrite_Registry, m_bAllowWrite_Registry);

	g_pReestr->SetValue(_LogEvent, m_bLogEvent);
	g_pReestr->SetValue(_ShowWarning, m_bShowWarning);
	g_pReestr->SetValue(_TerminateProcess, m_bTerminateProcess);
}

//+ ------------------------------------------------------------------------------------------

CGenericSettings::CGenericSettings()
{
	GS_ReloadSettings();
}

CGenericSettings::~CGenericSettings()
{
}

void CGenericSettings::GS_ReloadSettings()
{
	m_LogFileName = g_pReestr->GetValue(_LogFileName, _T(""), false);
	m_bStartOnlyKnownProcesses = !!g_pReestr->GetValue(_StartOnlyKnownProcesses, 0);
	m_bEducationMode = !!g_pReestr->GetValue(_EducationMode, 0);

	m_bProtectAutoRun = !!g_pReestr->GetValue(_ProtectAutorun, 0);
	m_bProtectSelf = !!g_pReestr->GetValue(_ProtectSelf, 1);
	m_bDontControlWriteBySystemProcess = !!g_pReestr->GetValue(_DontControlWriteBySystem, 1);
}

void CGenericSettings::GS_SaveSettings()
{
	g_pReestr->SetValue(_LogFileName, m_LogFileName);
	g_pReestr->SetValue(_StartOnlyKnownProcesses, m_bStartOnlyKnownProcesses);
	g_pReestr->SetValue(_EducationMode, m_bEducationMode);
	g_pReestr->SetValue(_ProtectAutorun, m_bProtectAutoRun);
	g_pReestr->SetValue(_ProtectSelf, m_bProtectSelf);
	g_pReestr->GetValue(_DontControlWriteBySystem, m_bDontControlWriteBySystemProcess);

	GS_SettingsChanged();
}

void CGenericSettings::GS_SettingsChanged()
{
}