// SandBoxSettings.h: interface for the CSandBoxSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SANDBOXSETTINGS_H__C085FEA8_BE28_45C9_893D_1AF66B0E934C__INCLUDED_)
#define AFX_SANDBOXSETTINGS_H__C085FEA8_BE28_45C9_893D_1AF66B0E934C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _SandBox_Path				_T("SandBox_Path")
#define _AllowCreateProcess			_T("SandBox_AllowCreateProcess")
#define _AllowWrite_SystemFolder	_T("SandBox_AllowWrite_SystemFolder")
#define _AllowWrite_ProgramFiles	_T("SandBox_AllowWrite_ProgramFiles")
#define _AllowWrite_SandBoxFolder	_T("SandBox_AllowWrite_SandBoxFolder")
#define _AllowWrite_TempFolder		_T("SandBox_AllowWrite_TempFolder")
#define _AllowWrite_Registry		_T("SandBox_AllowWrite_Registry")
#define _LogEvent					_T("SandBox_LogEvent")
#define _ShowWarning				_T("SandBox_ShowWarning")
#define _TerminateProcess			_T("SandBox_TerminateProcess")
//#define _							_T("SandBox_"

#define _StartOnlyKnownProcesses	_T("Generic_StartOnlyKnownProcesses")
#define _LogFileName				_T("Generic_LogFileName")
#define _EducationMode				_T("Generic_EducationMode")
#define _ProtectAutorun				_T("Generic_ProtectAutorun")
#define _ProtectSelf				_T("Generic_ProtectSelf")
#define _DontControlWriteBySystem	_T("Generic_DontControlWriteBySystem")
//#define _							_T("Generic_")


class CSandBoxSettings  
{
public:
	CSandBoxSettings();
	virtual ~CSandBoxSettings();

	void ReloadSettings();
	void SaveSettings();

	void SetSandBoxPath(CString *pSandBoxPath);
	CString GetSandBoxPath(){return m_SandBoxPath;}
	CString* GetSandBoxPathUpper(){return &m_SandBoxPathUpper;}

	bool IsStartFromBox(CString *pSandBoxPath);

	bool m_bAllowCreateProcess;
	
	bool m_bAllowWrite_SystemFolder;
	bool m_bAllowWrite_ProgramFiles;
	bool m_bAllowWrite_SandBoxFolder;
	bool m_bAllowWrite_TempFolder;
	bool m_bAllowWrite_Registry;
	bool m_bLogEvent;
	bool m_bShowWarning;
	bool m_bTerminateProcess;

private:
	CString m_SandBoxPath;
	CString m_SandBoxPathUpper;
};

//+ ------------------------------------------------------------------------------------------

class CGenericSettings
{
public:
	CGenericSettings();
	virtual ~CGenericSettings();

	void GS_ReloadSettings();
	void GS_SaveSettings();
	
	virtual void GS_SettingsChanged();

	CString m_LogFileName;
	bool m_bStartOnlyKnownProcesses;
	bool m_bEducationMode;
	bool m_bProtectAutoRun;
	bool m_bProtectSelf;
	bool m_bDontControlWriteBySystemProcess;
};

#endif // !defined(AFX_SANDBOXSETTINGS_H__C085FEA8_BE28_45C9_893D_1AF66B0E934C__INCLUDED_)
