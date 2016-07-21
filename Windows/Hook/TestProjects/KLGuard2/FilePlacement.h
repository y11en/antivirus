// FilePlacement.h: interface for the CFilePlacement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEPLACEMENT_H__C71C03E2_0436_49C4_8E05_28EDBA420737__INCLUDED_)
#define AFX_FILEPLACEMENT_H__C71C03E2_0436_49C4_8E05_28EDBA420737__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef enum _eFilePlacement
{
	_placement_other = 0,
	_placement_system_folder = 1,
	_placement_program_files = 2,
	_placement_temp = 3,
	_placement_sandbox = 4,
};

class CFilePlacement  
{
public:
	CFilePlacement();
	virtual ~CFilePlacement();

	_eFilePlacement GetFilePlacement(PWCHAR pwchFileName);

	virtual void ReloadEnvironment();
	virtual void SetSandBoxPalcement(CString* pSandBoxPath);

	bool IsFolderPrefetch(PWCHAR pwchFileName);

private:
	bool IsThisFolder(CString* pFolder, PWCHAR pwchFileName);
	void GetEnvironmentString(CString* pMember, PTCHAR EnvVar);
	CString ExpandEnvironmentString(PTCHAR ptchString);

	CString m_SysRoot;
	CString m_ProgramFiles;
	CString m_Temp;
	CString m_SandBox;
	CString m_PrefetchFolder;
};

#endif // !defined(AFX_FILEPLACEMENT_H__C71C03E2_0436_49C4_8E05_28EDBA420737__INCLUDED_)
